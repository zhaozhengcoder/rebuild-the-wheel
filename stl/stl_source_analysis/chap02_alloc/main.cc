#include "2jjalloc.h"
#include <vector>
using namespace std;

int main()
{
    vector<int, JJ::allocator<int> > vi;

    for(int i=0;i<10;i++)
    {
        vi.push_back(i);
    }

    for (int i = 0; i < vi.size(); i++)
    {
        cout << vi[i] << endl;
    }

    return 0;
}