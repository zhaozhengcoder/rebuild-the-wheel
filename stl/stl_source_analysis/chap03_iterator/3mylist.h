#ifndef __MYLIST_H__
#define __MYLIST_H__

#include <iostream>
#include <ostream>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
using namespace std;

template <typename T>
class ListItem
{
  public:
    ListItem(T val)
    {
        m_value = val;
        m_next = NULL;
    }

    T value() const
    {
        return this->m_value;
    }

    ListItem *next() const
    {
        return this->m_next;
    }

    void set_next(ListItem<T> * next)
    {
        m_next = next;
    }

  //protected:
    T m_value;
    ListItem *m_next;
};

template <typename T>
class List
{
  public:
    List()
    {
        m_end = NULL;
        m_front = NULL;
        m_size = 0;
    }
    void insert_front(T value)
    {
        ListItem<T> * node = new ListItem<T>(value);
        node->set_next(m_front);
        m_front = node;
        m_size++;
    }

    ListItem<T> *front()
    {
        return m_front;
    }

    ListItem<T> *end()
    {
        return m_end;
    }

    void display(std::ostream &os = std::cout) const
    {
        ListItem<T> *tmp_front = m_front;
        while (tmp_front->next() != NULL)
        {
            std::cout<<tmp_front->value()<<"  ";
            tmp_front = tmp_front->next();
        }
        std::cout<<std::endl;
        cout<<"size : "<<m_size<<endl;
    }

  protected:
    ListItem<T> *m_end;
    ListItem<T> *m_front;
    long m_size;
};

#endif // __MYLIST_H__