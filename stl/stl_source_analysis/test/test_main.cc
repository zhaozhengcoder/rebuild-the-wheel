#include <iostream>

using namespace std;

template <typename T>
T get(T val)
{
    cout<<val<<endl;
    return val;
} 

template <typename T,typename U> 
void get(T t, U u)
{
    cout<<t<<"  "<<u<<endl;
}

int main()
{
    int val = 5;
    get(val);

    get(3,3.12);

    return 0;
}