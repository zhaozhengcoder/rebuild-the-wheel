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
    iter = myfind(begin, end, 3);
    if (iter == end)
    {
        cout << "not found" << endl;
    }
    else
    {
        cout << "found." << iter->value() << endl;
    }

    List<char> mylist2;
    for(int i=0;i<10;i++)
    {
        char c = 'a';
        mylist2.insert_front((c+i));
    }

    mylist2.display();
    ListIter<ListItem<char> > begin2(mylist2.front());
    ListIter<ListItem<char> > end2(mylist2.end());
    ListIter<ListItem<char> > iter2; 

    iter2 = myfind(begin2, end2, 'c');
    if (iter2 == end2)
    {
        cout << "not found" << endl;
    }
    else
    {
        cout << "found." << iter2->value() << endl;
    }    

    return 0;
}