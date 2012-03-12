// This class is just a wrapper of this code I found on the Internet.
// It says it's "Free Software", but it lacks of an explicit license.

// http://www.seeingwithsound.com/im2sound.htm
// This file is based in the C program for visual sound generation.
// (C) P.B.L. Meijer 1996
#include <stdio.h>



class SeeSound
{

  int N;  // rows and columns
  double FL; // Lowest  frequency (Hz) in visual sound
  double FH; // 5000  
  double FS; // Sample  frequency (Hz) 
  double T;  // Image to sound conversion time (s) 

  double **A; // Image
  double *w;  // freqencies
  double *phi0; // Random phase start (Do I have to compute this for each image?)

  unsigned long ir, ia, ic, im;
  double rnd(void) // A RNG that Meijer included.
  {
    ir = (ir*ia+ic) % im;
    return ir / (1.0*im);
  }

  // to put numbers to the audio file independant to the endianess of the macine
  inline void wi(FILE *fp, unsigned int i) { int b1,b0; b0=i%256; b1=(i-b0)/256; putc(b0,fp); putc(b1,fp); }
  inline void wl(FILE *fp, long l) { unsigned int i1,i0; i0=l%65536L; i1=(l-i0)/65536L; wi(fp,i0); wi(fp,i1); }

  int busy;
  void write_internal (char* arg);
  friend void *thread_main(void *arg);

public:

  SeeSound(int flow, int fhigh, char fdistro, int n);
  ~SeeSound();

  void set(int i, int j, int val);
  void write(char *filename);
  int is_busy();
};

