#ifndef _jjalloc
#define _jjalloc

#include <new>
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <iostream>

namespace JJ
{

template <class T>
inline T *_allocate(ptrdiff_t size, T *)
{
    std::cout << "_allocate size : " << size << std::endl;
    // 这个表示的是如果分配内存失败的话，调用的函数
    // set_new_handler(0);
    T *tmp = (T *)(::operator new((size_t)(size * sizeof(T))));

    if (tmp == 0)
    {
        std::cout << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

template <class T>
inline void _deallocate(T *buffer)
{
    std::cout << "_deallocate" << std::endl;
    ::operator delete(buffer);
}

template <class T1, class T2>
inline void _construct(T1 *p, const T2 &value)
{
    std::cout << "_construct" << std::endl;
    new (p) T1(value); //placement new
}

template <class T>
inline void _destroy(T *ptr)
{
    std::cout << "_destroy" << std::endl;
    ptr->~T();
}

template <class T>
class allocator
{
  public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template <class U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    pointer allocate(size_type n, const void *hint = 0)
    {
        std::cout << "allocate" << std::endl;
        return _allocate((difference_type)n, (pointer)0);
    }

    void deallocate(pointer p, size_type n)
    {
        std::cout << "deallocate and size : " << n << std::endl;
        _deallocate(p);
    }

    void construct(pointer p, const T &value)
    {
        std::cout << "construct" << std::endl;
        _construct(p, value);
    }

    void destroy(pointer p)
    {
        std::cout << "destroy" << std::endl;
        _destroy(p);
    }

    pointer address(reference x)
    {
        return (pointer)&x;
    }

    const_pointer const_address(const_reference x)
    {
        return (const_pointer)&x;
    }

    size_type max_size() const
    {
        return size_type(UINT_MAX / sizeof(T));
    }
};

}; // namespace JJ

#endif