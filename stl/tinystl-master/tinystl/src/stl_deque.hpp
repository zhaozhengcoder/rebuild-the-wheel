#ifndef __STL_DEQUE_HPP
#define __STL_DEQUE_HPP

#include "stl_alloc.hpp"
#include "stl_construct.hpp"
#include "stl_iterator.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>

#define MAP_SIZE 8

template<typename T, size_t Bufsize = 0>
struct __deque_iterator
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T   value_type;
    typedef T*  pointer;
    typedef T&  reference;
    typedef ptrdiff_t   difference_type;

    typedef __deque_iterator<T, Bufsize>    self;

    size_t row;
    size_t col;
    T **buffer_map_pointer;

    __deque_iterator(const size_t &_row, const size_t &_col, T **_cur = 0) 
            : row(_row), col(_col), buffer_map_pointer(_cur) {}

    bool operator==(const self &iter) const
    {
        return (buffer_map_pointer==iter.buffer_map_pointer && 
                    row==iter.row && col==iter.col);
    }
    bool operator!=(const self &iter) const
    {
        return (buffer_map_pointer!=iter.buffer_map_pointer ||
                    row!=iter.row || col!=iter.col);
    }
    T operator*() const
    {
        return *(*(buffer_map_pointer + row) + col);
    }
    self &operator++() 
    {
        if (col == Bufsize - 1)
        {
            ++ row;
            col = 0;
        }
        else    ++ col;
        return *this;
    }
};

template<typename T, typename Alloc = default_alloc, size_t Bufsize = 0>
class deque 
{
    private:
        void oom_exception() const
        {
            size_t max_capacity = MAP_SIZE * Bufsize * sizeof(T);
            std::cout << "Out of Memroy, the max size to allocate is " << max_capacity << std::endl;
            exit(1);
        }

    public:
        typedef T   value_type;
        typedef Alloc   allocator_type;
        typedef T*  pointer;
        typedef T&  reference;
        typedef size_t  size_type;
        typedef __deque_iterator<T, Bufsize>    iterator;
        
    protected:
        typedef simple_alloc<T, Alloc>  data_allocator;

        T *buffer_map_pointer[MAP_SIZE];
        size_t buffer_cur_nums;

        size_t s_row;
        size_t s_col;
        size_t f_row;
        size_t f_col;

        void fill_initialize(size_t n, const T &val);
        void deallocate_deque();

    public:

        /* Constructor */
        deque() : buffer_cur_nums(0), s_row(0), s_col(0), f_row(0), f_col(0) {}
        deque(size_t n, const T &val) { fill_initialize(n, val); }
        deque(int n, const T &val) { fill_initialize(n, val); }
        deque(long n, const T &val) { fill_initialize(n, val); }
        explicit deque(size_t n) { fill_initialize(n, T()); }

        /* Iterator */
        iterator begin() { return iterator(s_row, s_col, buffer_map_pointer); }
        iterator end() { return iterator(f_row, f_col, buffer_map_pointer); }

        /* Access */
        T operator[](size_t n) const;
        size_t size() const { return ((f_row-s_row-1)*Bufsize + (Bufsize-s_col) + f_col); }
        bool empty() const { return (s_row==f_row && s_col==f_col); }
};

template<typename T, typename Alloc, size_t Bufsize>
void deque<T, Alloc, Bufsize>::fill_initialize(size_t n, const T &val)
{
    buffer_cur_nums = n / Bufsize + 1;
    if (buffer_cur_nums > MAP_SIZE) oom_exception();
    size_t i, j;
    for (i = 0; i < buffer_cur_nums; ++ i)
        buffer_map_pointer[i] = data_allocator::allocate(Bufsize);
    for (i = 0; i < buffer_cur_nums-1; ++ i)
        for (j = 0; j < Bufsize; ++ j)  construct(buffer_map_pointer[i]+j, val);
    for (j = 0; j < n - Bufsize * (buffer_cur_nums-1); ++ j)
        construct(buffer_map_pointer[buffer_cur_nums-1]+j, val);
    s_row = s_col = 0;
    f_row = buffer_cur_nums - 1; f_col = j;
}

/******************** Interface ******************************************/
template<typename T, typename Alloc, size_t Bufsize>
T deque<T, Alloc, Bufsize>::operator[](size_t n) const
{
    if (n <= Bufsize - s_col)
        return *(buffer_map_pointer[0] + s_col + n);
    n -= Bufsize - s_col;
    return *(buffer_map_pointer[(n-1)/Bufsize+1] + (n-1)%Bufsize);
}

template<typename T, typename Alloc, size_t Bufsize>
void deque<T, Alloc, Bufsize>::deallocate_deque()
{
    for (auto i = 0; i < buffer_cur_nums; ++ i)
    {
        if (i == 0)
        {
            for (auto j = Bufsize - 1; j >= s_col; -- j)
                destroy(buffer_map_pointer[i] + j);
        }
        if (i == buffer_cur_nums - 1)
        {
            for (auto j = f_col - 1; j >= 0; -- j)
                destroy(buffer_map_pointer[i] + j);
        }
        data_allocator::deallocate(buffer_map_pointer[i], Bufsize);
    }
}

#endif