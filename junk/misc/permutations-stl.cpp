
/* http://www.hispabyte.net/foro/index.php?act=ST&f=92&t=18338  */

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{

    vector < int >v;
    vector < int >::iterator start, end, it;

    for (int i = 0; i < 4; i++)
        v.push_back(i);

    start = v.begin();
    end = v.end();

    do {
        for (it = v.begin(); it != v.end(); it++)
            cout << *it << " ";
        cout << endl;
    } while (next_permutation(start, end));

    return 0;
}
