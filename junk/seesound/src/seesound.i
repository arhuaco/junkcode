%module seesound
%{
#include "seesound.h"
%}

class SeeSound
{
public:
  SeeSound(int flow, int fhigh, char fdistro, int n);
  ~SeeSound();

  void set(int i, int j, int val);
  void write(char *filename);
  int SeeSound::is_busy();
}; 
