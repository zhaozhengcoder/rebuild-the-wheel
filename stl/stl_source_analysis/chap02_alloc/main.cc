#include "2jjalloc.h"
#include <vector>
using namespace std;

int main()
{
    vector<int, JJ::allocator<int> > vi;
    vi.push_back(1);
    vi.push_back(2);

    for (int i = 0; i < vi.size(); i++)
    {
        cout << vi[i] << endl;
    }

    return 0;
}