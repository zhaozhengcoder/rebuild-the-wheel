#ifndef __STL_VECTOR_h
#define __STL_VECTOR_h

#include "stl_alloc.hpp"
#include "stl_construct.hpp"
#include "stl_uninitialized.hpp"

#include <initializer_list>
#include <cstddef>
#include <cstdlib>

template<typename T, typename Alloc = default_alloc>
class vector 
{
    public:
        typedef T   value_type;
        typedef Alloc   allocator_type;
        typedef value_type*    pointer;
        typedef const value_type*  const_pointer;
        typedef value_type&    reference;
        typedef const value_type&  const_reference;
        typedef value_type*    iterator;
        typedef const value_type*  const_iterator;
        typedef ptrdiff_t   difference_type;
        typedef size_t  size_type;

    protected:
        typedef vector<T, Alloc>    self;
        typedef simple_alloc<value_type, allocator_type> data_allocator;

        iterator start;
        iterator finish;
        iterator end_of_storage;

        iterator insert_aux(iterator position, const T &x)
        {
            if (finish == end_of_storage)
            {
                difference_type diff = position - start;
                size_type old_size = end_of_storage - start;
                size_type new_size = 0 ? 1 : 2 * old_size;
                iterator new_start = data_allocator::allocate(new_size);
                iterator new_finish = new_start;
                new_finish = uninitialized_copy(start, finish, new_finish);

                destroy(start, finish);
                deallocate();

                start = new_start;
                finish = new_finish;
                end_of_storage = start + new_size;

                position = start + diff;
            }
            for (auto it = finish - 1; it != position; -- it)
                construct(it + 1, *it);
            construct(position, x);
            return position;
        }

        iterator erase_aux(iterator position)
        {
            destroy(position);
            for (auto it = position + 1; it != finish; ++ it)
                *(it - 1) = *it;
            -- finish;
            return position;
        }
        
        void fill_initialize(size_type n, const T &val)
        {
            start = allocate_and_fill(n, val);
            finish = start + n;
            end_of_storage = finish;
        }
        iterator allocate_and_fill(size_type n, const T &x)
        {
            iterator res = data_allocator::allocate(n);
            uninitialized_fill_n(res, n, x);
            return res;
        }
        void deallocate()
        {
            if (start != 0)
                data_allocator::deallocate(start, end_of_storage - start);
        }

    public:

        /* Constructor */
        vector() : start(0), finish(0), end_of_storage(0) {}
        vector(size_type n, const T &x) { fill_initialize(n, x); }
        vector(int n, const T &x)  { fill_initialize(n, x); }
        vector(long n, const T &x) { fill_initialize(n, x); }
        explicit vector(size_type n) { fill_initialize(n, T()); }
        

        /* Iterator */
        iterator begin() { return start; }
        const_iterator begin() const { return start; }
        iterator end() { return finish; }
        const_iterator end() const { return finish; }

        /* Capacity */
        size_type size() const noexcept { return size_type(end() - begin()); }
        size_type max_size() const noexcept { return size_type(-1) / sizeof(value_type); }
        size_type capacity() const noexcept { return size_type(end_of_storage - begin()); }
        bool empty() const noexcept { return (start == finish); }

        /* Member Access */
        reference operator[] (size_type n)  { return *(begin() + n); }
        const_reference operator[] (size_type n) const { return *(begin() + n); }
        reference at (size_type n) { return *(begin() + n); }
        const_reference at (size_type n) const { return *(begin() + n); }
        reference front() { return *(begin()); }
        const_reference front() const { return *(begin()); }
        reference back() { return *(end() - 1); }
        const_reference back() const { return *(end() - 1); }

        /* Insert Operation */
        void push_back(const T &val)
        {
            insert_aux(finish, val);
        }
        iterator insert(iterator position, const T &val)
        {
            iterator res = insert_aux(position, val);
            return res;
        }
        
        /* Delete Operation */
        void pop_back()
        {
            erase_aux(finish - 1);
        }
        iterator erase(iterator position)
        {
            iterator res = erase_aux(position);
            return res;
        }

        /* Other Operations */
        void clear()
        {
            destroy(start, finish);
            finish = start;
        }
        void swap(vector &x)
        {
            auto tmp_start = start;
            auto tmp_finish = finish;
            auto tmp_end_of_storage = end_of_storage;

            start = x.begin();
            finish = x.end();
            end_of_storage = x.end_of_storage;

            x.start = tmp_start;
            x.finish = tmp_finish;
            x.end_of_storage = tmp_end_of_storage;
        }


};

#endif