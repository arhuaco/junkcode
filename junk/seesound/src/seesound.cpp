// This class is just a wrapper of this code I found on the Internet.
// It says it's "Free Software", but it lacks of an explicit license.

// http://www.seeingwithsound.com/im2sound.htm
// This file is based in the C program for visual sound generation.
// (C) P.B.L. Meijer 1996

// This class is not intended to be thread-safe.


#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include <seesound.h>

using namespace std;


#define D      1   /* Linear|Exponential=0|1 distribution    */
#define HIFI   1   /* 8-bit|16-bit=0|1 sound quality         */
#define STEREO 1   /* Mono|Stereo=0|1 sound selection        */
#define DELAY  1   /* Nodelay|Delay=0|1 model   (STEREO=1)   */
#define FADE   1   /* Relative fade Yes|No=0|1  (STEREO=1)   */
#define DIFFR  1   /* Diffraction Yes|No=0|1    (STEREO=1)   */
#define BSPL   1   /* Rectangular|B-spline=0|1 time window   */
#define BW     0   /* 16|2-level=0|1 grey format in *P[]     */
#define TwoPi 6.283185307179586476925287
#define HIST  (1+HIFI)*(1+STEREO)

SeeSound::SeeSound(int flow, int fhigh, char fdistro, int n)
{
  ir=0L, ia=9301L, ic=49297L, im=233280L; // RNG

  assert(fdistro == 'l' || fdistro == 'e');
  assert(flow >= 200 && fhigh <= 20000);
  assert(n >=1 && n <= 128);

  N = n;
  FL = flow;
  FH = fhigh;
  // default values
  FS = 44100;
  T = 1.05;

  w    = (double*) malloc(sizeof(double) * N);
  assert(w);
  phi0 = (double*) malloc(sizeof(double) * N);
  assert(phi0);
  phi0 = (double*) malloc(sizeof(double) * N);

  A  = (double**) malloc(sizeof(double*) * N);
  for (int i=0; i<N; i++)
   A[i] = (double*) malloc(sizeof(double) * N);

  /* Set frequency distribution */
  if (fdistro == 'e')
    for (int i=0; i<N; i++) w[i] = TwoPi * FL * pow(1.0* FH/FL,1.0*i/(N-1));
  else
    for (int i=0; i<N; i++) w[i] = TwoPi * FL + TwoPi * (FH-FL)   *i/(N-1) ;

  busy = 0;
}

void
SeeSound::set(int i, int j, int val)
{
  assert(val >=0 and val < 15);
  assert(i >=0 and i < N );
  assert(j >=0 and j < N);

  if (val)
    A[N-i-1][j] =  pow(10.0, (val - 15)/10.0); /* 2dB steps */
  else
    A[N-i-1][j] = 0.0;
}


void
SeeSound::write_internal(char *filename)
{
   int i, j, ss;
   long k=0L, l, ns=2L*(long)(0.5*FS*T), m=ns/N,
        sso=HIFI?0L:128L, ssm=HIFI?32768L:128L;
   double a, t, dt=1.0/FS, s, y, yp, z, tau1, tau2, x, theta,
                  scale=0.5/sqrt(N), q, q2, r, sl, sr, tl, tr, yl, ypl, yr, ypr,
          zl, zr, hrtf, hrtfl, hrtfr, v=340.0, /* v = speed of sound (m/s) */
          hs=0.20;  /* hs = characteristic acoustical size of head (m) */

   for (int i=0; i<N; i++)
     phi0[i] = TwoPi * rnd();

   /* Write 8/16-bit mono/stereo .wav file, using rectangular time window */
   FILE *fp = fopen(filename,"wb");

   if (!fp)
   {
     cerr << "could not open file : " << filename << endl;
     return;
   }

   fprintf(fp,"RIFF");
   wl(fp, ns*HIST+36L);
   fprintf(fp,"WAVEfmt ");
   wl(fp, 16L);
   wi(fp, 1);
   wi(fp, STEREO?2:1);
   wl(fp, int(0L+FS));
   wl(fp, (long int)( 0L+FS*HIST));
   wi(fp, HIST);
   wi(fp, HIFI?16:8);
   fprintf(fp,"data");
   wl(fp, ns*HIST);
   tau1 = 0.5 / w[N-1]; tau2 = 0.25 * tau1*tau1;
   y = yl = yr = z = zl = zr = 0.0;
   /* Not optimized for speed */
   while (k < ns && !STEREO) {
      if (BSPL) { q = 1.0 * (k%m)/(m-1); q2 = 0.5*q*q; }
      j = k / m; if (j>N-1) j=N-1; s = 0.0; t = k * dt;
      if (k < ns/(5*N)) s = (2.0*rnd()-1.0) / scale;  /* "click" */
      else for (i=0; i<N; i++) {
         if (BSPL) {  /* Quadratic B-spline for smooth C1 time window */
            if (j==0) a = (1.0-q2)*A[i][j]+q2*A[i][j+1];
            else if (j==N-1) a = (q2-q+0.5)*A[i][j-1]+(0.5+q-q2)*A[i][j];
            else a = (q2-q+0.5)*A[i][j-1]+(0.5+q-q*q)*A[i][j]+q2*A[i][j+1];
         }
         else a = A[i][j];
         s += a * sin(w[i] * t + phi0[i]);
      }
      yp = y; y = tau1/dt + tau2/(dt*dt);
      y  = (s + y * yp + tau2/dt * z) / (1.0 + y); z = (y - yp) / dt;
      l  = sso + 0.5 + scale * ssm * y; /* y = 2nd order filtered s */
      if (l >= sso-1+ssm) l = sso-1+ssm; if (l < sso-ssm) l = sso-ssm;
      ss = (unsigned int) l;
      if (HIFI) wi(fp,ss); else putc(ss,fp);
      k++;
   }
   while (k < ns && STEREO) {
      if (BSPL) { q = 1.0 * (k%m)/(m-1); q2 = 0.5*q*q; }
      j = k / m; if (j>N-1) j=N-1;
      r = 1.0 * k/(ns-1);  /* Binaural attenuation/delay parameter */
      theta = (r-0.5) * TwoPi/3; x = 0.5 * hs * (theta + sin(theta));
      tl = tr = k * dt; if (DELAY) tr += x / v; /* Time delay model */
      x  = fabs(x); sl = sr = 0.0; hrtfl = hrtfr = 1.0;
      for (i=0; i<N; i++) {
         if (DIFFR) {
            /* First order frequency-dependent azimuth diffraction model */
            if (TwoPi*v/w[i] > x) hrtf = 1.0; else hrtf = TwoPi*v/(x*w[i]);
            if (theta < 0.0) { hrtfl =  1.0; hrtfr = hrtf; }
            else             { hrtfl = hrtf; hrtfr =  1.0; }
         }
         if (FADE) {
            /* Simple frequency-independent relative fade model */
                    hrtfl *= (1.0-0.7*r);
                    hrtfr *= (0.3+0.7*r);
         }
         if (BSPL) {
            if (j==0) a = (1.0-q2)*A[i][j]+q2*A[i][j+1];
            else if (j==N-1) a = (q2-q+0.5)*A[i][j-1]+(0.5+q-q2)*A[i][j];
            else a = (q2-q+0.5)*A[i][j-1]+(0.5+q-q*q)*A[i][j]+q2*A[i][j+1];
         }
         else a = A[i][j];
         sl += hrtfl * a * sin(w[i] * tl + phi0[i]);
         sr += hrtfr * a * sin(w[i] * tr + phi0[i]);
      }
      if (k < ns/(5*N)) sl = (2.0*rnd()-1.0) / scale;  /* Left "click" */
      if (tl < 0.0) sl = 0.0;
      if (tr < 0.0) sr = 0.0;
      ypl = yl; yl = tau1/dt + tau2/(dt*dt);
      yl  = (sl + yl * ypl + tau2/dt * zl) / (1.0 + yl); zl = (yl - ypl) / dt;
      ypr = yr; yr = tau1/dt + tau2/(dt*dt);
      yr  = (sr + yr * ypr + tau2/dt * zr) / (1.0 + yr); zr = (yr - ypr) / dt;
      l   = sso + 0.5 + scale * ssm * yl;
      if (l >= sso-1+ssm) l = sso-1+ssm; if (l < sso-ssm) l = sso-ssm;
      ss  = (unsigned int) l;
      if (HIFI)wi(fp, ss); else putc(ss,fp);  /* Left channel */
      l   = sso + 0.5 + scale * ssm * yr;
      if (l >= sso-1+ssm) l = sso-1+ssm; if (l < sso-ssm) l = sso-ssm;
      ss  = (unsigned int) l;
      if (HIFI) wi(fp, ss); else putc(ss,fp);  /* Right channel */
      k++;
   }

   fclose(fp);
}

SeeSound::~SeeSound()
{
  free(w);
  free(phi0);

  for (int i=0; i<N; i++)
    free(A[i]);
  free(A);

}


// What follows here is a horrible hack.
// I'm trying to avoid the python global interpreter lock

#include <pthread.h>
#include <string.h>

SeeSound *me;
char *file_name;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_id;
pthread_attr_t thread_attr;

void *thread_main(void *arg)
{
  me->write_internal(file_name);
  free(file_name);

  pthread_mutex_lock (&mutex);
  me->busy = 0;
  pthread_mutex_unlock (&mutex);
  pthread_exit((void*) 0);
}

void
SeeSound::write(char *filename)
{

  assert(filename);

  pthread_mutex_lock (&mutex);

  if (busy)
    goto write_exit;

  me = this;
  me->busy = 1;

  file_name = strdup(filename);
  assert(file_name);

  pthread_attr_init(&thread_attr);
  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&thread_id, &thread_attr, thread_main, (void *)NULL);

  write_exit:

  pthread_mutex_unlock (&mutex);
}

int
SeeSound::is_busy()
{
  int b;

  pthread_mutex_lock (&mutex);
  b = busy;
  pthread_mutex_unlock (&mutex);

  return b; 
}
