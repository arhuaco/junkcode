/* Testing merge sort */

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

void merge_sort(vector <int> &v) {
  if (v.size() <= 1)
    return;

  vector <int> left;
  vector <int> right;

  unsigned i = 0;
  while (i < v.size() / 2)
    left.push_back(v[i++]);
  while (i < v.size())
    right.push_back(v[i++]);

  merge_sort(left);
  merge_sort(right);

  i = 0;
  unsigned j = 0;
  unsigned k = 0;

  while (i < left.size() && j < right.size()) {
    if (left[i] < right[j])
      v[k++] = left[i++];
    else
      v[k++] = right[j++];
  }

  while (i < left.size())
    v[k++] = left[i++];
  while (j < right.size())
    v[k++] = right[j++];
}

int
main(int argc, char *argv[]) {
  vector<int> v;

  for (unsigned i = 0; i < 20; ++i)
    v.push_back(10 * ((double)random()/(double)RAND_MAX));

  cout << "before: " << endl;
  for (unsigned i = 0; i < v.size(); ++i)
    cout << v[i] << ' ';
  cout << endl;

  merge_sort(v);

  cout << "sorted: " << endl;
  for (unsigned i = 0; i < v.size(); ++i)
    cout << v[i] << ' ';
  cout << endl;
}
