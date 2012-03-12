#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

template <class T>
int partition(vector <T> &v, int first, int last) {
  int j = first;
  swap(v[first + rand() % (last - first + 1)], v[first]);
  for (int i = first + 1; i <= last; ++i)
    if (v[i] <= v[first])
      swap(v[++j], v[i]);
  swap(v[j], v[first]);
  return j;
}

template <class T>
void qsort(vector <T> &v, int first, int last) {
  if (first >= last)
    return;
  int pivot = partition(v, first, last);
  qsort(v, pivot + 1, last);
  while (pivot > first && v[pivot] == v[pivot - 1]) // This cycle is important!
    pivot--;
  qsort(v, first, pivot - 1);
}

template <class T>
bool is_sorted(vector<T> &v){
  for (int i  = 0; i < (int)v.size() - 1; ++i)
    if (v[i] > v[i + 1])
      return false;
  return true;
}

int get_sum(vector<int> &v){
  int r = 0;
  for (unsigned i  = 0; i < v.size(); ++i)
    r += v[i];
  return r;
}

int
main(int argc, char *argv[])
{
  srand(time(NULL));

  vector <int> v;
  unsigned size = 10000000;
  for(unsigned j = 0; j < size; ++j)
    v.push_back(rand() % 10);

  int sum_before = get_sum(v);
  qsort<int>(v, 0, v.size() - 1);
  int sum_after = get_sum(v);

  if (sum_before != sum_after) {
    cerr << "Something went wrong with the data!" << endl;
    return 1;
  }

  if (!is_sorted<int>(v)) {
    cerr << "Unsorted result!" << endl;
    return 1;
  }

  return 0;
}
