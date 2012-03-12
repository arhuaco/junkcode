#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <queue>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <assert.h>

using namespace std;

typedef unsigned int uint;
#define loop(i, n) for(uint i = 0; i < n; ++i)
#define foreach(i, c) for(typeof((c).begin()) i = (c).begin(); i != (c).end() ; i++)
typedef vector <int> VI;

class BoredPhilosophers {
  public:
  vector <int> simulate(vector <string> text, int N);
  // BEGIN CUT HERE
  
// BEGIN CUT HERE
	public:
	void run_test(int Case) { if ((Case == -1) || (Case == 0)) test_case_0(); if ((Case == -1) || (Case == 1)) test_case_1(); if ((Case == -1) || (Case == 2)) test_case_2(); if ((Case == -1) || (Case == 3)) test_case_3(); if ((Case == -1) || (Case == 4)) test_case_4(); }
	private:
	template <typename T> string print_array(const vector<T> &V) { ostringstream os; os << "{ "; for (typename vector<T>::const_iterator iter = V.begin(); iter != V.end(); ++iter) os << '\"' << *iter << "\","; os << " }"; return os.str(); }
	void verify_case(int Case, const vector <int> &Expected, const vector <int> &Received) { cerr << "Test Case #" << Case << "..."; if (Expected == Received) cerr << "PASSED" << endl; else { cerr << "FAILED" << endl; cerr << "\tExpected: " << print_array(Expected) << endl; cerr << "\tReceived: " << print_array(Received) << endl; } }
	void test_case_0() { string Arr0[] = {"hello world"}; vector <string> Arg0(Arr0, Arr0 + (sizeof(Arr0) / sizeof(Arr0[0]))); int Arg1 = 2; int Arr2[] = {2, 1 }; vector <int> Arg2(Arr2, Arr2 + (sizeof(Arr2) / sizeof(Arr2[0]))); verify_case(0, Arg2, simulate(Arg0, Arg1)); }
	void test_case_1() { string Arr0[] = {"aaa bbb aaa bbb aaa aaa"}; vector <string> Arg0(Arr0, Arr0 + (sizeof(Arr0) / sizeof(Arr0[0]))); int Arg1 = 4; int Arr2[] = {2, 3, 3, 3 }; vector <int> Arg2(Arr2, Arr2 + (sizeof(Arr2) / sizeof(Arr2[0]))); verify_case(1, Arg2, simulate(Arg0, Arg1)); }
	void test_case_2() { string Arr0[] = {"remember"," t","o concatenate ","the"," ","text"}; vector <string> Arg0(Arr0, Arr0 + (sizeof(Arr0) / sizeof(Arr0[0]))); int Arg1 = 1; int Arr2[] = {5 }; vector <int> Arg2(Arr2, Arr2 + (sizeof(Arr2) / sizeof(Arr2[0]))); verify_case(2, Arg2, simulate(Arg0, Arg1)); }
	void test_case_3() { string Arr0[] = {"a a a a a a a b a a b a a a b b a b a a a b a b"}; vector <string> Arg0(Arr0, Arr0 + (sizeof(Arr0) / sizeof(Arr0[0]))); int Arg1 = 6; int Arr2[] = {2, 4, 7, 11, 14, 17 }; vector <int> Arg2(Arr2, Arr2 + (sizeof(Arr2) / sizeof(Arr2[0]))); verify_case(3, Arg2, simulate(Arg0, Arg1)); }
	void test_case_4() { string Arr0[] = {"aa ababaa c cbbcbc cabcbcb ba bccc ababb",
 "c cabcba caa ababaa c cbbcbc cabcbcb ba ",
 "bccc ababbc cabcba c bbcbab",
 "b aaaa cbccaaa bccc ababbc cabcba c bbcb",
 "ab cbcaca"}; vector <string> Arg0(Arr0, Arr0 + (sizeof(Arr0) / sizeof(Arr0[0]))); int Arg1 = 7; int Arr2[] = {15, 17, 18, 19, 20, 20, 20 }; vector <int> Arg2(Arr2, Arr2 + (sizeof(Arr2) / sizeof(Arr2[0]))); verify_case(4, Arg2, simulate(Arg0, Arg1)); }

// END CUT HERE

  // END CUT HERE
};

// BEGIN CUT HERE
int
main(void)
{
  BoredPhilosophers ___test;
  ___test.run_test(-1);
  //___test.run_test(1);
  return 0;
}


// END CUT HERE

void tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters = " ")
{
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

int diff(vector <string> &v, unsigned N) {
  set<string> count;
  loop(i, v.size()) {
    string c;
    if (i + N > v.size())
      break;
    loop(j, N)
      c += v[i + j] + ".";
    count.insert(c);
  }
  return count.size();
}

vector <int>
BoredPhilosophers::simulate(vector <string> text, int N)
{
  vector<int> ret;
  vector<string> words;
  string sum = "";
  loop(i, text.size())
   sum += text[i];
  tokenize(sum, words);
  set <string> uniq;
  loop(i, words.size())
    uniq.insert(words[i]);
  ret.push_back(uniq.size());
  for(int i = 2; i <= N; ++i)
    ret.push_back(diff(words, i));
  return ret;
}
