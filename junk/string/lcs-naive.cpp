#include <iostream>
#include <string>


using namespace std;

string LCS(string S, string T)
{
  if (S.size() == 0 || T.size() == 0)
    return "";

  if (S[0] == T[0])
    return S[0] + LCS(S.substr(1),  T.substr(1));

  string s1 = LCS(S.substr(1), T);
  string s2 = LCS(S, T.substr(1));

  if (s1.size() > s2.size())
    return s1;

  return s2;
}

int
main(int argc, char *argv[])
{
  cout << "LCS:" << LCS("a1b2c3d4e", "zz1yy2xx3ww4vv") << endl;
  return 0;
}
