#ifndef __STL_UNINITIALIZED_H
#define __STL_UNINITIALIZED_H

#include "stl_construct.hpp"
#include "stl_iterator.hpp"
#include "type_traits.hpp"

#include <cstring>

template<typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, 
                                                const T &x, __true)
{
    ForwardIterator cur = first;
    while (cur != last)
    {
        *cur = x;
        ++ cur;
    }
    return cur;
}

template<typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                                const T &x, __false)
{
    ForwardIterator cur = first;
    while (cur != last)
    {
        construct(&*cur, x);
        ++ cur;
    }
    return cur;
}

template<typename ForwardIterator, typename T, typename T1>
inline ForwardIterator __uninitialized_fill(ForwardIterator first, ForwardIterator last,
                                            const T &x, T1 *)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_aux(first, last, x, is_POD());
}

template<typename ForwardIterator, typename T>
inline ForwardIterator uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x)
{
    return __uninitialized_fill(first, last, x, value_type(first));
}

template<typename ForwardIterator, typename N, typename T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, N n, const T &x, __true)
{
    ForwardIterator cur = first;
    for (; n > 0; -- n)
    {
        *cur = x;
        ++ cur;
    }
    return cur;
}

template<typename ForwardIterator, typename N, typename T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, N n, const T &x, __false)
{
    ForwardIterator cur = first;
    for (; n > 0; -- n)
    {
        construct(&*cur, x);
        ++ cur;
    }
    return cur;
}

template<typename ForwardIterator, typename N, typename T, typename T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, N n, const T &x, T1 *)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template<typename ForwardIterator, typename N, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, N n, const T &x)
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
                                        ForwardIterator result, __true)
{
    ForwardIterator cur = result;
    while (first != last)
    {
        *cur = *first;
        ++ cur;
        ++ first;
    }
    return cur;
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, 
                                        ForwardIterator result, __false)
{
    ForwardIterator cur = result;
    while (first != last)
    {
        construct(&*cur, *first);
        ++ cur;
        ++ first;
    }
    return cur;
}

template<typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, 
                                    ForwardIterator result, T *)
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(result));
}

inline char *uninitialized_copy(const char *first, const char *last, char *result)
{
    memmove(result, first, (last - first));
    return result;
}

inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
{
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result;
}

#endif