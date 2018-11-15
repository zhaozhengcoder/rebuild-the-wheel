#ifndef __STL_CONSTRUCT_H
#define __STL_CONSTRUCT_H

#include "stl_iterator.hpp"
#include "type_traits.hpp"

#include <new.h>

template<typename T1, typename T2>
inline void construct(T1 *p, const T2 &value)
{
    new (p) T1(value);
}

template<typename T>
inline void destroy(T *p)
{
    p->~T();
}

template<typename ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __false)
{
    for (; first != last; ++ first)
        destroy(&*first);
}

template<typename ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __true)
{
}

template<typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T *)
{
    typedef typename __type_traits<T>::has_trivial_destructor   trivial_destructor;
    __destory_aux(first, last, trivial_destructor());
}

template<typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
    __destroy(first, last, value_type(first));
}

inline void destroy(char *, char *) {}
inline void destroy(wchar_t *, wchar_t *) {}

#endif