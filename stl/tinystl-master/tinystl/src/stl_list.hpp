#ifndef __STL_LIST_H
#define __STL_LIST_H

#include "stl_iterator.hpp"
#include "stl_alloc.hpp"
#include "stl_construct.hpp"

template<typename T>
struct __list_node
{
    __list_node<T> *prev;
    __list_node<T> *next;
    T data;

    __list_node(const T &val) : prev(0), next(0), data(val) {}
};

template<typename T>
struct __list_iterator
{
    public:
        typedef bidirectional_iterator_tag  iterator_category;
        typedef T   value_type;
        typedef T&  reference;
        typedef T*  pointer;
        typedef ptrdiff_t   difference_type;
        typedef size_t  size_type;

        typedef __list_node<T>* node_ptr;
        typedef __list_iterator<T> self;

        node_ptr p;

        __list_iterator(const node_ptr &_p = 0) : p(_p) {}

        bool operator==(const __list_iterator<T> &iter) const
        {
            return (p == iter.p);
        }
        bool operator!=(const __list_iterator<T> &iter) const
        {
            return (p != iter.p);
        }
        self &operator++()
        {
            p = (node_ptr)(p->next);
            return *this;
        }
        self operator++(int)
        {
            self tmp = *this;
            p = (node_ptr)(p->next);
            return tmp;
        }
        self &operator--()
        {
            p = (node_ptr)(p->prev);
            return *this;
        }
        self operator--(int)
        {
            self tmp = *this;
            p = (node_ptr)(p->prev);
            return tmp;
        }
        reference operator*() const { return p->data; }
        pointer operator->() const { return &(p->data); }
};

template<typename T, typename Alloc = default_alloc>
class list 
{
    public:
        typedef T   value_type;
        typedef Alloc   allocator_type;
        typedef size_t  size_type;
        typedef __list_node<T>* node_ptr;
        typedef __list_iterator<T>  iterator;

    protected:
        typedef simple_alloc<__list_node<value_type>, allocator_type> data_allocator;

        node_ptr node;

        __list_node<value_type> *new_node(const value_type &x) const
        {
            __list_node<value_type> *p = data_allocator::allocate(1);
            construct(p, x);
            return p;
        }
        void destroy_node(__list_node<value_type> *p) const
        {
            destroy(p);
            data_allocator::deallocate(p, 1);
        }
        iterator insert_aux(iterator position, const value_type &x)
        {
            __list_node<value_type> *p = new_node(x);
            __list_node<value_type> *tmp = position.p;
            tmp->prev->next = p;
            p->next = tmp;
            p->prev = tmp->prev;
            tmp->prev = p;
            return iterator(p);
        }
        iterator erase_aux(iterator position)
        {
            __list_node<value_type> *tmp = position.p;
            __list_node<value_type> *res = tmp->next;
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;

            destroy(tmp);
            data_allocator::deallocate(tmp, 1);
            return res;
        }

    public:
        list() 
        {
            node = data_allocator::allocate(1);
            node->next = node;
            node->prev = node;
        }
        ~list() 
        {
            clear();
            destroy_node(node);
        }

        iterator begin() { return iterator(node->next); }
        iterator end() { return iterator(node); }
        bool empty() const { return (node_ptr(node->next) == node); }
        size_type size() const 
        {
            size_type n = 0;
            for (auto p = node->next; p != node; p = p->next)
                ++ n;
            return n;
        }
        size_type max_size() const { return size_type(-1) / sizeof(__list_node<value_type>); }

        /* Insert */
        iterator push_back(const value_type &x)
        {
            iterator res = insert_aux(end(), x);
            return res;
        }
        iterator push_front(const value_type &x)
        {
            iterator res = insert_aux(begin(), x);
            return res;
        }
        iterator insert(iterator position, const value_type &x)
        {
            iterator res = insert_aux(position, x);
            return res;
        }

        /* Erase */
        iterator pop_back()
        {
            iterator res = erase_aux(-- end());
            return res;
        }
        iterator pop_front()
        {
            iterator res = erase_aux(begin());
            return res;
        }
        iterator erase(iterator position)
        {
            iterator res = erase_aux(position);
            return res;
        }
        void clear()
        {
            auto p = node->next;
            while (p != node)
            {
                auto tmp = p;
                p = p->next;
                destroy_node(tmp);
            }
            node->next = node->prev = node;
        }
        void remove(const value_type &x)
        {
            auto it = begin();
            while (it != end()) 
            {
                if (*it == x)   it = erase(it);
                else    ++ it;
            }
        }
};

#endif