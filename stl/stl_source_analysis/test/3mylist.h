#include <iostream>

template <typename T>
class ListItem
{
public:
    ListItem(T value, ListItem<T>* next)
    {
        _value = value;
        _next = next;
    }
    T value() const { return _value; }
    void value(T value) { _value = value; }
    ListItem* next() const { return _next; }
    void next(ListItem* next) { _next = next; }
    //
private:
    T _value;
    ListItem* _next;  // †ÎÏò´®ÁÐ£¨single linked list£©
};

template <typename T>
class List
{
public:
    List()
    {
        _size = 0;
        _front = NULL;
        _end = NULL;
    }
    void show()
    {
        if(_end == NULL )
        {
            std::cout<<"_end == NULL"<<std::endl;
           
        }
        if( _front == NULL)
        {
            std::cout<<"_front == NULL"<<std::endl;
        }
        std::cout<<"size : "<<_size<<std::endl;
    }
    ~List()
    {
        if(_front == _end) return;
        ListItem<T>* item = _front;
        while(item != _end)
        {
            ListItem<T>* iter = item;
            item = item->next();
            delete iter;
        }
    
    }
    void insert_front(T value)
    {
        _front = new ListItem<T>(value, _front);
    }
    void insert_end(T value)
    {
        // 链表的初始情况，_front 和 _end 都是空
        if(_front == _end)
        {
            _front = new ListItem<T>(value, _front);
        }
        ListItem<T>* item = _front;
        while(item->next() != _end)
        {
            item = item->next();
        }
        item->next(new ListItem<T>(value, _end));
    }
    void display(std::ostream &os = std::cout) const
    {
        ListItem<T>* item = _front;
        while(item != _end)
        {
            os<<item->value()<<" ";
            item = item->next();
        }
        os<<std::endl;
    }
    ListItem<T>* front(){ return _front;}
    ListItem<T>* end(){ return _end;}
    // 
private:
    ListItem<T>* _end;              // _end 永远指向null
    ListItem<T>* _front;            // _front 指向头部
    long _size;
};