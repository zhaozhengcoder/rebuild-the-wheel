/*************************************************************************
    > File Name: 3mylist-iter-test.cpp
    > Author: gatieme
    > Created Time: 2016年03月21日 星期一 15时13分39秒
 ************************************************************************/

#include <iostream>
//#include <algorithm>
using namespace std;

#include "3mylist.h"
#include "3mylist_iter.h"

template <class InputIterator, class T>
InputIterator myfind(InputIterator begin,InputIterator end, T key)
{
    while(begin != end && (*begin).value() != key)
    {
        begin++;
    }
    return begin;
}

int main()
{
    List<int> mylist;
    for (int i = 0; i < 10; i++)
    {
        mylist.insert_front(i);
    }

    mylist.display();
    ListIter<ListItem<int> > begin(mylist.front());
    ListIter<ListItem<int> > end(mylist.end());  // default 0, null
    ListIter<ListItem<int> > iter; // default 0, null

    if(begin == end)
    {}

    int key = 3;
    iter = myfind(begin, end, key);
    if (iter == end)
    {
        cout << "not found" << endl;
    }
    else
    {
        cout << "found." << iter->value() << endl;
    }

    iter = myfind(begin, end, 7);
    if (iter == end)
    {
        cout << "not found" << endl;
    }
    else
    {
        cout << "found." << iter->value() << endl;   
    }
    return 0;
}