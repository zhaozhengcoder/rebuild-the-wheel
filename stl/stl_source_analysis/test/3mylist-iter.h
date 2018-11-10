#include "3mylist.h"
template <class Item> // Item可以是单向列表节点或双向列表节点。
struct ListIter       // 此处这个迭代器特定只为列表服务，因为其
{                     // 独特的 operator++之故。
    Item *ptr;        // 保持与容器之间的一个联系

    ListIter(Item *p = 0) // default ctor
        : ptr(p)
    {
    }

    // 不必实作 copy ctor，因为编译器提供的预设行为已足够。
    // 不必实作 operator=，因为编译器提供的预设行为已足够。

    Item &operator*() const { return *ptr; }
    Item *operator->() const { return ptr; }

    // 以下两个operator++遵循标准作法，参见[Meyers96]条款6
    // (1) pre-increment operator
    ListIter &operator++()
    {
        ptr = ptr->next();
        return *this;
    }

    // (2) post-increment operator
    ListIter operator++(int)
    {
        ListIter tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const ListIter &i) const
    {
        return ptr == i.ptr;
    }

    bool operator!=(const ListIter &i) const
    {
        return ptr != i.ptr;
    }
};