#ifndef __STL_ALLCH_H
#define __STL_ALLCH_H

#include <cstddef>
#include <cstdlib>
#include <iostream>

/* Malloc Allocator */
class __malloc_alloc_template
{
    private:
        static void oom_allocate() { std::cout << "Out of Memroy..." << std::endl; exit(1); }
        static void oom_reallocate() { std::cout << "Out of Memroy..." << std::endl; exit(1); }

    public:
        static void *allocate(size_t n)
        {
            void *res = malloc(n);
            if (res == 0)   oom_allocate();
            return res;
        }
        static void deallocate(void *p, size_t n)
        {
            free(p);
        }
        static void *reallocate(void *p, size_t n)
        {
            deallocate(p, n);
            p = allocate(n);
            if (p == 0) oom_reallocate();
            return p;
        }
};

typedef __malloc_alloc_template malloc_alloc;

/* Default Allocator */
#define __ALIGN 8
#define __MAX_BYTES 128
#define __NFREELISTS 16

inline size_t round_up(size_t bytes)
{
    return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
}

class __default_alloc_template
{
    private:
        union obj
        {
            union obj *free_list_link;
            char data[1];
        };

        static obj *free_list[__NFREELISTS];
        static size_t free_list_index(size_t bytes)
        {
            return ((bytes + __ALIGN - 1) / __ALIGN - 1);
        }

        static char *free_list_start;
        static char *free_list_end;
        static size_t heap_size;

        static void *refill(size_t n)
        {
            int nobjs = 20;
            char *chunk = chunk_alloc(n, nobjs);
            if (nobjs == 1) return (void *)chunk;
            obj **head = free_list + free_list_index(n);
            obj *cur, *nxt;
            void *res = (void *)chunk;
            *head = nxt = (obj *)(chunk + n);
            for (int i = 1; ; ++ i)
            {
                cur = nxt;
                nxt = (obj *)((char *)cur + n);
                if (i == nobjs - 1)
                {
                    cur -> free_list_link = 0;
                    break;
                }
                else
                    cur -> free_list_link = nxt;
            }
            return res;
        }

        static char *chunk_alloc(size_t n, int &nobjs)
        {
            char *res;
            size_t total_bytes = n * nobjs;
            size_t left_bytes = free_list_end - free_list_start;
            if (left_bytes >= total_bytes)
            {
                res = free_list_start;
                free_list_start += total_bytes;
                return res;
            }
            else if (left_bytes >= n)
            {
                nobjs = left_bytes / n;
                total_bytes = n * nobjs;
                res = free_list_start;
                free_list_start += total_bytes;
                return res;
            }
            else
            {
                if (left_bytes > 0)
                {
                    obj **head = free_list + free_list_index(n);
                    ((obj *)free_list_start) -> free_list_link = *head;
                    *head = (obj *)free_list_start;
                }
                size_t bytes_to_get = 2 * total_bytes + (heap_size >> 4);
                free_list_start = (char *)malloc(bytes_to_get);
                if (free_list_start == 0)
                {
                    obj **head, *p;
                    for (int i = n; i < __MAX_BYTES; i += __ALIGN)
                    {
                        head = free_list + free_list_index(i);
                        p = *head;
                        if (p != 0)
                        {
                            *head = p -> free_list_link;
                            free_list_start = (char *)p;
                            free_list_end = free_list_start + i;
                            return (chunk_alloc(n, nobjs));
                        }
                    }
                    free_list_end = 0;
                    res = (char *)malloc_alloc::allocate(bytes_to_get);
                    return res;
                }
                heap_size += bytes_to_get;
                free_list_end = free_list_start + bytes_to_get;
                return (chunk_alloc(n, nobjs));
            }
        }

    public:
        static void *allocate(size_t n)
        {
            obj *res;
            if (n > __MAX_BYTES)
                return malloc_alloc::allocate(n);
            obj **head = free_list + free_list_index(n);
            res = *head;
            if (res == 0)
            {
                void *r = refill(round_up(n));
                return r;
            }
            *head = res -> free_list_link;
            return (void *)res;
        }

        static void deallocate(void *p, size_t n)
        {
            if (n > __MAX_BYTES)
            {
                malloc_alloc::deallocate(p, n);
                return;
            }
            obj **head = free_list + free_list_index(n);
            ((obj *)p) -> free_list_link = *head;
            *head = (obj *)p;
        }

        static void reallocate(void *p, size_t old_n, size_t new_n)
        {
            deallocate(p, old_n);
            p = allocate(new_n);
        }
};

typedef __default_alloc_template default_alloc;

default_alloc::obj *default_alloc::free_list[__NFREELISTS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char *default_alloc::free_list_start = 0;
char *default_alloc::free_list_end = 0;
size_t default_alloc::heap_size = 0;

/* simple alloc, A wrapper */
template<typename T, typename Alloc = default_alloc>
class simple_alloc
{
    public:
        static T *allocate(size_t n)
        {
            return (T *)Alloc::allocate(n * sizeof(T));
        }
        static void deallocate(T *p, size_t n)
        {
            Alloc::deallocate((void *)p, n * sizeof(T));
        }
        static void reallocate(T *p, size_t old_n, size_t new_n)
        {
            Alloc::reallocate((void *)p, old_n * sizeof(T), new_n * sizeof(T));
        }
};

#endif 