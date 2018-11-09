#ifdef _jjalloc
#define _jjalloc

#include <new>
#include <cstddef>
#include <cstdlib> 
#include <climits> 
#include <iostream> 

namespace JJ
{

template <class T> 
inline T* _allocate(ptrdiff_t size , T *)
{
    set_new_handler(0);
    T * tmp =(T*)(::operator new((size_t)(size * sizeof(T))));

    if(tmp == 0)
    {
        cerr<<"out of memory"<<endl;
        exit(1);
    }
    return tmp;
}

template <class T>
inline void _deallocate(T * buffer)
{
    ::operator delete(buffer);
}

template <class T1,class T2>
inline void _construct(T1 * p,const T2 & value)
{
    new (p) T1(value);   //placement new
}


template <class T>
inline void _destory(T * ptr)
{
    ptr->~T();
}


template <class T>
class allocator 
{
public:
    typedef T   value_type;
    typedef T*  pointer;
    typedef 
private: 

};


}

#endif 