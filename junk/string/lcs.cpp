#include <iostream>
#include <string>
#include <map>


// http://www.topcoder.com/tc?module=Static&d1=features&d2=040104

using namespace std;

/* Naive version, this one is exponential! */

string LCS(string S, string T)
{
  if (S.size() == 0 || T.size() == 0)
    return "";
  if (S[0] == T[0])
    return S[0] + LCS(S.substr(1, S.size() - 1),  T.substr(1, T.size() - 1));

  string s1 = LCS(S.substr(1, S.size() - 1), T);
  string s2 = LCS(S, T.substr(1, T.size() - 1));

  if (s1.size() > s2.size())
    return s1;
  return s2;
}


/* this use memoization */


string LCSMEMO(string S, string T, map < pair<string, string> ,  string > &memo)
{
  string LCS2(string S, string T, map < pair<string, string> ,  string > &memo);

  if (S.size() == 0 || T.size() == 0)
    return "";

  pair<string, string> idx = make_pair(S,T);
  map < pair<string, string> ,  string >::iterator it = memo.find(idx);

  if (it != memo.end())
    return it->second;

  string ret = LCS2(S, T, memo);

  memo[idx] = ret;
  cerr << S << ", " << T << " => " << ret << endl;
  return ret;
}

string LCS2(string S, string T, map < pair<string, string> , string > &memo)
{
  if (S.size() == 0 || T.size() == 0)
    return "";

  if (S[0] == T[0])
    return S[0] + LCSMEMO(S.substr(1, S.size() - 1),  T.substr(1, T.size() - 1), memo);

  string s1 = LCSMEMO(S.substr(1, S.size() - 1), T, memo);
  string s2 = LCSMEMO(S, T.substr(1, T.size() - 1), memo);

  if (s1.size() > s2.size())
    return s1;
  return s2;
}

string
LCSMEMO_MAIN(string S, string T)
{
  map < pair<string, string> ,  string > memo;
  return LCS2(S, T, memo);
}

int
main(int argc, char *argv[])
{
  if (argc != 3)
  {
    cerr << "enter 2 strings" << endl;
    return 1;
  }

  cerr << "LCS:" << LCSMEMO_MAIN(argv[1], argv[2]) << endl;
  return 0;
}
