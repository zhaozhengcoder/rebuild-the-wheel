#ifndef TINYSTL_MAP_H_
#define TINYSTL_MAP_H_

#include "stl_tree.hpp"

template<typename _Key, typename _Value>
class map : public __rb_tree<_Key, _Value>
{
    public:
        typedef _Key    key_type;
        typedef _Value  value_type;
        typedef typename __rb_tree<key_type, value_type>::tree_node  node_type;
        typedef typename __rb_tree<key_type, value_type>::iterator   iterator;

        map(node_type *_p = node_type::NIL)
            : __rb_tree<key_type, value_type>(_p) {}

        iterator find(const key_type &k)
        {
            return __rb_tree<key_type, value_type>::find(k);
        }
        void insert(const key_type &k, const value_type &v)
        {
            __rb_tree<key_type, value_type>::insert(k, v);
        }
        void remove(const key_type &k)
        {
            __rb_tree<key_type, value_type>::remove(k);
        }
};

#endif