#include <iostream>
#include <string>
#include <map>

using namespace std;

string LCS(string S, string T, map < pair<string, string> , string > &memo)
{
  if (S.size() == 0 || T.size() == 0)
    return "";

  pair<string, string> idx = make_pair(S,T);
  map < pair<string, string> ,  string >::iterator it = memo.find(idx);
  if (it != memo.end())
    return it->second;

  string ret;

  if (S[0] == T[0])
    ret = S[0] + LCS(S.substr(1),  T.substr(1), memo);
  else {
    string s1 = LCS(S.substr(1), T, memo);
    string s2 = LCS(S, T.substr(1), memo);
    ret = s1.size() > s2.size() ? s1 : s2;
  }

  memo[idx] = ret;
  // Uncomment to see how the cache "memo" is filled out
  //cerr << S << ", " << T << " => " << ret << endl;

  return ret;
}

int
main(int argc, char *argv[])
{
  map < pair<string, string> ,  string > memo;
  cout << "LCS:" << LCS("abcdefghijklmnzyxwvutsrqpo",
                        "opqrstuvwxyzabcdefghijklmn", memo) << endl;
  return 0;
}
