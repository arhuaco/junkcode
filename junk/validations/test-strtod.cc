#include <cstdlib>
#include <cerrno>
#include <iostream>

// C wrapper to parse a double.

int
strtod_wrapper(const char *str, double *result)
{
  char *endptr;
  double val;

  errno = 0;

  val = strtod(str, &endptr);

  if (errno || endptr == str || *endptr)
    return 1;

  *result = val;

  return 0;
}

#define PROMPT "> "
#define BUF_SIZE 256 

using namespace std;

int
main(int argc, char *argv[]) {
  char buffer[BUF_SIZE];
  cout <<  PROMPT;
  while(cin.getline(buffer, BUF_SIZE))
  {
    double number;
    if (strtod_wrapper(buffer, &number)) {
      cerr << "Error" << endl;
    } else {
      cout << "Parsed " << number << endl;
    }
    cout <<  PROMPT;
  }
  return 0;
}
