#ifndef TINYSTL_TREE_H_
#define TINYSTL_TREE_H_

#include "stl_iterator.hpp"
#include "debug.h"

#include <cstddef>

enum _Color { _RED = 0, _BLACK = 1 };

/* Red-Black Tree Node Structure */
template<typename _Key, typename _Value>
struct __rb_tree_node
{
    typedef _Key    key_type;
    typedef _Value  value_type;
    typedef _Color  color_type;
    typedef __rb_tree_node<key_type, value_type>    self;

    self *parent;
    self *left;
    self *right;

    color_type  color;
    key_type    key;
    value_type  value;

    __rb_tree_node(const color_type &c = color_type::_RED, 
                const key_type &k = key_type(),
                const value_type &v = value_type())
        : parent(nullptr), left(nullptr), right(nullptr), 
        color(c), key(k), value(v) {}

    static self *NIL;
};

template<typename _Key, typename _Value>
__rb_tree_node<_Key, _Value> *__rb_tree_node<_Key, _Value>::NIL = 
    new __rb_tree_node<_Key, _Value>(_Color::_BLACK);

/* Red-Black Tree Iterator */
template<typename _Key, typename _Value>
struct __rb_tree_iterator
{
    typedef bidirectional_iterator_tag  iterator_category;
    typedef _Key    key_type;
    typedef _Value  value_type;
    typedef key_type*   key_pointer;
    typedef key_type&   key_reference;
    typedef ptrdiff_t   difference_type;
    typedef size_t  size_type;

    typedef __rb_tree_node<key_type, value_type>    tree_node;
    typedef __rb_tree_iterator<key_type, value_type>    self;

    tree_node *p;
    
    __rb_tree_iterator(tree_node *_p = tree_node::NIL) : p(_p) {}

    bool operator==(const self &iter) { return (p == iter.p); }
    bool operator!=(const self &iter) { return (p != iter.p); }
    tree_node &operator*() const { return *p; }
    tree_node *operator->() const { return p; }
    self &operator++()
    {
        if (p->right != tree_node::NIL)
        {
            p = p->right;
            while (p->left != tree_node::NIL)
                p = p->left;
        }
        else 
        {
            while (p->parent!=tree_node::NIL && p==p->parent->right)
                p = p->parent;
            p = p->parent;
        }
        return *this;
    }
};

/* Red-Black Tree */
template<typename _Key, typename _Value>
class __rb_tree 
{
    public:
        typedef _Key    key_type;
        typedef _Value  value_type;
        typedef __rb_tree_node<key_type, value_type>    tree_node;
        typedef __rb_tree_iterator<key_type, value_type>    iterator;

    protected:
        tree_node *root;

    public:
        __rb_tree(tree_node *_p = tree_node::NIL) : root(_p) {}

    protected:
        void left_rotate(tree_node *p);
        void right_rotate(tree_node *p);
        void insert_fixup(tree_node *p);
        void remove_fixup(tree_node *p);

    public:
        iterator begin();
        iterator end();

        iterator find(const key_type &k) const;
        void insert(const key_type &k, const value_type &v = value_type());
        void remove(const key_type &k);

        void inorder_traversal() const;
        void preorder_traversal() const;
};

template<typename _Key, typename _Value>
typename __rb_tree<_Key, _Value>::iterator 
__rb_tree<_Key, _Value>::begin()
{
    if (root == __rb_tree_node<_Key, _Value>::NIL)
        return __rb_tree<_Key, _Value>::iterator();
    auto p = root;
    while (p->left != __rb_tree_node<_Key, _Value>::NIL)
        p = p->left;
    return __rb_tree<_Key, _Value>::iterator(p);
}

template<typename _Key, typename _Value>
typename __rb_tree<_Key, _Value>::iterator 
__rb_tree<_Key, _Value>::end()
{
    return __rb_tree<_Key, _Value>::iterator();
}

template<typename _Key, typename _Value>
void __rb_tree<_Key, _Value>::left_rotate(__rb_tree<_Key, _Value>::tree_node *p)
{
    auto t = p->right;
    p->right = t->left;
    if (t->left != tree_node::NIL) t->left->parent = p;
    if (t != tree_node::NIL)   t->parent = p->parent;
    if (p->parent == tree_node::NIL)   root = t;
    else
    {
        if (p == p->parent->left)   p->parent->left = t;
        else    p->parent->right = t;
    }
    t->left = p;
    if (p != tree_node::NIL)   p->parent = t;
}

template<typename _Key, typename _Value>
void __rb_tree<_Key, _Value>::right_rotate(__rb_tree<_Key, _Value>::tree_node *p)
{
    auto t = p->left;
    p->left = t->right;
    if (t->right != tree_node::NIL)    t->right->parent = p;
    if (t != tree_node::NIL)   t->parent = p->parent;
    if (p->parent == tree_node::NIL)   root = t;
    else
    {
        if (p == p->parent->left)   p->parent->left = t;
        else    p->parent->right = t;
    }
    t->right = p;
    if (p != tree_node::NIL)   p->parent = t;
}

template<typename _Key, typename _Value>
void __rb_tree<_Key, _Value>::insert_fixup(__rb_tree<_Key, _Value>::tree_node *p)
{
    __rb_tree<_Key, _Value>::tree_node *t = tree_node::NIL;
    while (p != root && p->parent->color == _Color::_RED) {
      	if (p->parent == p->parent->parent->left) {
      	    t = p->parent->parent->right;
      	    if (t->color == _Color::_RED) {
      	        p->parent->color = _Color::_BLACK;
      	        t->color = _Color::_BLACK;
      	        p->parent->parent->color = _Color::_RED;
      	        p = p->parent->parent;
      	    }
      	    else {
      	        if (p == p->parent->right) {
      	            p = p->parent;
      	            left_rotate(p);
      	        }
      	        p->parent->color = _Color::_BLACK;
      	        p->parent->parent->color = _Color::_RED;
      	        right_rotate(p->parent->parent);
            }
      	}
      	else {
      	    t = p->parent->parent->left;
      	    if (t->color == _Color::_RED) {
      	        p->parent->color = _Color::_BLACK;
      	        t->color = _Color::_BLACK;
      	        p->parent->parent->color = _Color::_RED;
      	        p = p->parent->parent;
      	    }
      	    else {
      	        if (p == p->parent->left) {
      	            p = p->parent;
      	            right_rotate(p);
      	        }     	
      	        p->parent->color = _Color::_BLACK;
      	        p->parent->parent->color = _Color::_BLACK;
                left_rotate(p->parent->parent);  	            }
      	}
    }
    root->color = _Color::_BLACK;
}

template<typename _Key, typename _Value>
void __rb_tree<_Key, _Value>::remove_fixup(__rb_tree<_Key, _Value>::tree_node *p)
{
    __rb_tree_node<_Key, _Value> *w = __rb_tree<_Key, _Value>::tree_node::NIL;
    while (p != root && p->color == _Color::_BLACK) 
    {
        if (p == p->parent->left) 
        {
            w = p->parent->right;
            if (w->color == _Color::_RED) 
            {
      	        w->color = _Color::_BLACK;
      	        p->parent->color = _Color::_RED;
      	        left_rotate(p->parent);
      	        w = p->parent->right;
      	    }
      	    if (w->left->color == _Color::_BLACK && w->right->color == _Color::_BLACK) 
            {
      	        w->color = _Color::_RED;
      	        p = p->parent;
      	    } 
            else 
            {
      	        if (w->right->color == _Color::_BLACK) 
                {
      	            w->left->color = _Color::_BLACK;
      	            w->color = _Color::_RED;
      	            right_rotate(w);
      	            w = p->parent->right;
      	        }
      	        w->color = p->parent->color;
      	        p->parent->color = _Color::_BLACK;
      	        w->right->color = _Color::_BLACK;
      	        left_rotate(p->parent);
      	        p = root;
      	    }
      	}
        else 
        {
      	    w = p->parent->left;
      	    if (w->color == _Color::_RED) 
            {
      	        w->color = _Color::_BLACK;
      	        p->parent->color = _Color::_RED;
      	        right_rotate(p->parent);
      	        w = p->parent->left;
      	    }
      	    if (w->right->color == _Color::_BLACK && w->left->color == _Color::_BLACK)
            {
      	        w->color = _Color::_RED;
      	        p = p->parent;
      	    } 
            else 
            {
      	        if (w->left->color == _Color::_BLACK)
                {
      	            w->right->color = _Color::_BLACK;
      	            w->color = _Color::_RED;
      	            left_rotate(w);
      	            w = p->parent->left;
      	        }
      	        w->color = p->parent->color;
      	        p->parent->color = _Color::_BLACK;
      	        w->left->color = _Color::_BLACK;
      	        right_rotate(p->parent);
      	        p = root;
            }
      	}
    }
    p->color = _Color::_BLACK;
}

/******************************* Interface **************************************/
template<typename _Key, typename _Value>
typename __rb_tree<_Key, _Value>::iterator 
__rb_tree<_Key, _Value>::find(const typename __rb_tree<_Key, _Value>::key_type &k) const
{
    __rb_tree_node<_Key, _Value> *p = root;
    while (p != __rb_tree_node<_Key, _Value>::NIL)
    {
        if (p->key < k) p = p->right;
        else if (p->key > k)    p = p->left;
        else    return __rb_tree<_Key, _Value>::iterator(p);
    }
    return __rb_tree<_Key, _Value>::iterator();
}

template<typename _Key, typename _Value>
void
__rb_tree<_Key, _Value>::insert(const __rb_tree<_Key, _Value>::key_type &k,
        const __rb_tree<_Key, _Value>::value_type &v)
{
    __rb_tree_node<_Key, _Value> *node = new __rb_tree_node<_Key, _Value>(_Color::_RED, k, v);
    if (root == __rb_tree_node<_Key, _Value>::NIL)
    {
        root = node;
        root->parent = root->left = root->right
                 = __rb_tree_node<_Key, _Value>::NIL;
        root->color = _Color::_BLACK;
        return;
    }
    auto cur = root;
    auto pre = __rb_tree_node<_Key, _Value>::NIL;
    while (cur != __rb_tree_node<_Key, _Value>::NIL)
    {
        pre = cur;
        if (k < cur->key)   cur = cur->left;
        else    cur = cur->right;
    }
    node->parent = pre;
    node->left = node->right = __rb_tree_node<_Key, _Value>::NIL;
    if (k < pre->key)   pre->left = node;
    else    pre->right = node;  
    insert_fixup(node);
}

template<typename _Key, typename _Value>
void __rb_tree<_Key, _Value>::remove(const _Key &k)
{
    __rb_tree_node<_Key, _Value> *cur = root;
    __rb_tree_node<_Key, _Value> *nil = __rb_tree_node<_Key, _Value>::NIL;
    while (cur != nil)
    {
        if (k < cur->key)   cur = cur->left;
        else if (k > cur->key)  cur = cur->right;
        else    break;
    }
    __rb_tree_node<_Key, _Value> *y = nil;
    __rb_tree_node<_Key, _Value> *x = nil;
    if (cur->left == nil || cur->right == nil)  y = cur;
    else
    {
        y = cur->right;
        while (y->left != nil)
            y = y->left;
    }
    if (y->left != nil) x = y->left;
    else    x = y->right;
    x->parent = y->parent;
    if (y->parent == nil)
        root = x;
    else
    {
        if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }
    if (y != cur)   cur->value = y->value;
    if (y->color == _Color::_BLACK)
        remove_fixup(x);
    delete y;
}

template<typename _Key, typename _Value>
void __rb_tree<_Key, _Value>::inorder_traversal() const
{
    __rb_tree_node<_Key, _Value> *nil = __rb_tree_node<_Key, _Value>::NIL;
    __rb_tree_node<_Key, _Value> *p = root, *free = nil;
    while (p != nil)
    {
        free = p->left;
        if (free != nil)
        {
            while (free->right != nil && free->right != p)
                free = free->right;
            if (free->right == nil)
            {
                free->right = p;
                p = p->left;
                continue;
            }
            else    free->right = nil;
        }
        std::cout << p->key << "\t";
        p = p->right;
    }
    std::cout << std::endl;
}

template<typename _Key, typename _Value>
void __rb_tree<_Key, _Value>::preorder_traversal() const
{
    __rb_tree_node<_Key, _Value> *nil = __rb_tree_node<_Key, _Value>::NIL;
    __rb_tree_node<_Key, _Value> *p = root, *free = nil;
    while (p != nil)
    {
        free = p->left;
        if (free != nil)
        {
            while (free->right!=nil && free->right!=p)
                free = free->right;
            if (free->right == nil)
            {
                std::cout << p->key << "\t";
                free->right = p;
                p = p->left;
                continue;
            }
            else    free->right = nil;
        }
        else    std::cout << p->key << "\t";
        p = p->right;
    }
    std::cout << std::endl;
}

#endif  // TINYSTL_TREE_H_