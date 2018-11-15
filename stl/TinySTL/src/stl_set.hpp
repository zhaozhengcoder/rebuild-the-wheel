#ifndef TINYSTL_SET_H_
#define TINYSTL_SET_H_

#include "stl_tree.hpp"

template<typename _Key>
class set : public __rb_tree<_Key, int>
{
    public:
        typedef _Key    key_type;
        typedef int     value_type;
        typedef typename __rb_tree<key_type, value_type>::tree_node  node_type;
        typedef typename __rb_tree<key_type, value_type>::iterator   iterator;

        set(node_type *_p = node_type::NIL) 
            : __rb_tree<key_type, value_type>(_p) {}
        
        iterator find(const key_type &k) const
        {
            return __rb_tree<key_type, value_type>::find(k);
        }
        void insert(const key_type &k) 
        {
            __rb_tree<key_type, value_type>::insert(k);
        }
        void remove(const key_type &k)
        {
            __rb_tree<key_type, value_type>::remove(k);
        }
};

#endif  // TINYSTL_SET_H_