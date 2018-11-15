/* Trait Type */

#ifndef __TYPE_TRAITS_H
#define __TYPE_TRAITS_H

struct __true{};
struct __false{};

template<typename T>
struct __type_traits 
{
    typedef __true     this_dummy_member_must_be_first;
    typedef __false    has_trivial_default_constructor;
    typedef __false    has_trivial_copy_constructor;
    typedef __false    has_trivial_assignment_operator;
    typedef __false    has_trivial_destructor;
    typedef __false    is_POD_type;
};

template<>
struct __type_traits<char>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<unsigned char>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<short>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<unsigned short>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<int>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<unsigned int>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<long>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<unsigned long>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<long long>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<unsigned long long>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<float>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<double>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<>
struct __type_traits<long double>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

template<typename T>
struct __type_traits<T *>
{
    typedef __true    has_trivial_default_constructor;
    typedef __true    has_trivial_copy_constructor;
    typedef __true    has_trivial_assignment_operator;
    typedef __true    has_trivial_destructor;
    typedef __true    is_POD_type;
};

#endif