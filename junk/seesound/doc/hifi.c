/* <xmp tag needed for MSIE browser>
 * C program for visual sound generation. (C) P.B.L. Meijer 1996
 *
 *  Downloaded from:
 *
 * http://www.seeingwithsound.com/im2sound.htm
 *
 */
#define FNAME "hificode.wav"    /* User-defined parameters   */
#define N     64   /* Resolution, i.e., # rows and columns   */
#define FL   500   /* Lowest  frequency (Hz) in visual sound */
#define FH  5000   /* Highest frequency (Hz)                 */
#define FS 44100   /* Sample  frequency (Hz)                 */
#define T   1.05   /* Image to sound conversion time (s)     */
#define D      1   /* Linear|Exponential=0|1 distribution    */
#define HIFI   1   /* 8-bit|16-bit=0|1 sound quality         */
#define STEREO 1   /* Mono|Stereo=0|1 sound selection        */
#define DELAY  1   /* Nodelay|Delay=0|1 model   (STEREO=1)   */
#define FADE   1   /* Relative fade Yes|No=0|1  (STEREO=1)   */
#define DIFFR  1   /* Diffraction Yes|No=0|1    (STEREO=1)   */
#define BSPL   1   /* Rectangular|B-spline=0|1 time window   */
#define BW     0   /* 16|2-level=0|1 grey format in *P[]     */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define C_ALLOC(number, type) ((type *) calloc((number),sizeof(type)) )
#define TwoPi 6.283185307179586476925287
#define HIST  (1+HIFI)*(1+STEREO)
#define WHITE 1.00
#define BLACK 0.00

#if BW
static char *P[] = {    /* N x N pixels, black and white '#',' ' */
"########################### # ###### ### ## ####################",
"############################ ###################################",
"########################## #### #  #############################",
"########################## #  #  ##### ####### #################",
"############################## ######### #######################",
"############################# ############    ##################",
"###########################  #### ###  #### ### ### ############",
"########################## ## ### ## ## ##### ##################",
"######################### ##  ### #  #### ###### ###############",
"##########################   ### ############ ##  ##  ##########",
"######################### #  #  ### # ##### ### ################",
"############################# #  ### # #######  #  #############",
"#################### ###### ################# ##### ############",
"###################   ########### #############  # #############",
"##################  #  ####   # ####### ## ######### ###########",
"#################  ###  ###   #### # ###########################",
"################  #####  ##   ##################################",
"###############  #######  #   ##################################",
"##############  #########     ##################################",
"#############  ###########    ##################################",
"############  #############   ##################################",
"###########  ###############  ##################################",
"##########  #################  #################################",
"#########  ###################  #################   ##   ##   ##",
"########  #####################  ################   ##   ##   ##",
"#######  #######################  ###############   ##   ##   ##",
"######  #########################  ##############   ##   ##   ##",
"#####  ###########################  #############   ##   ##   ##",
"#####                               ############################",
"#####                               ############################",
"#####                               ############################",
"#####                               ############################",
"#####     #######      #######      ############################",
"#####     #######      #######      ############################",
"#####     #######      #######      ############################",
"#####     #######      #######      ############################",
"#####     #######      #######      ############################",
"#####     #######      #######      ############################",
"#####                               ############################",
"#####                               ############################",
"#####                               ############################",
"#####                               ############################",
"#####                               ############################",
"#####                               ############################",
"#####     ######      ########      ############################",
"#####     ######      ########      ############################",
"#####     ######      ########      ############################",
"#####     ######      ########      ############################",
"#####     ######      ########      ############################",
"#####     ######      ########      ############################",
"#####     ######                    ############################",
"#####     ######                    ##########       ###########",
"#####     ######                    ######### ## #### ##########",
"#####     ######                    ######## ### #### ##########",
"#####                               #####   #### #####     #####",
"########################################                    ####",
"########################################                    ####",
"########################################  #  #       #  #   ####",
"################################## #######    #######    #######",
"################################  ########    #######    #######",
"#############################   ###########  #########  ########",
"########################     ###################################",
"################        ########################################",
"##              ################################################"};
#else
static char *P[] = {  /* N x N pixels, 16 grey levels a,...,p */
"aaaaaaaaaaaaaaaaaaaaaaaaaaapapaaaaaapaaapaapaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaapaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaapaaaapappaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaapappappaaaaapaaaaaaapaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaapaaaaaaaaapaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaapaaaaaaaaaaaappppaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaappaaaapaaappaaaapaaapaaapaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaapaapaaapaapaapaaaaapaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaapaappaaapappaaaapaaaaaapaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaapppaaapaaaaaaaaaaaapaappaappaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaapappappaaapapaaaaapaaapaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaapappaaapapaaaaaaappappaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaapaaaaaapaaaaaaaaaaaaaaaaapaaaaapaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaaapppaaaaaaaaaaapaaaaaaaaaaaaappapaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaappappaaaapppapaaaaaaapaapaaaaaaaaapaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaappaaappaaapppaaaapapaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaappaaaaappaapppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaappaaaaaaappapppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaappaaaaaaaaapppppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaappaaaaaaaaaaappppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaappaaaaaaaaaaaaapppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaappaaaaaaaaaaaaaaappaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaappaaaaaaaaaaaaaaaaappaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaappaaaaaaaaaaaaaaaaaaappaaaaaaaaaaaaaaaaapppaapppaapppaa",
"aaaaaaaappaaaaaaaaaaaaaaaaaaaaappaaaaaaaaaaaaaaaapppaapppaapppaa",
"aaaaaaappaaaaaaaaaaaaaaaaaaaaaaappaaaaaaaaaaaaaaapppaapppaapppaa",
"aaaaaappaaaaaaaaaaaaaaaaaaaaaaaaappaaaaaaaaaaaaaapppaapppaapppaa",
"aaaaappaaaaaaaaaaaaaaaaaaaaaaaaaaappaaaaaaaaaaaaapppaapppaapppaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaaappppppaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaaappppppaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaaappppppaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaaappppppaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaaappppppaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaaappppppaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppaaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppaaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppaaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppaaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppaaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppaaaaaaaappppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppppppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaapppppaaaaaappppppppppppppppppppaaaaaaaaaapppppppaaaaaaaaaaa",
"aaaaapppppaaaaaappppppppppppppppppppaaaaaaaaapaapaaaapaaaaaaaaaa",
"aaaaapppppaaaaaappppppppppppppppppppaaaaaaaapaaapaaaapaaaaaaaaaa",
"aaaaapppppppppppppppppppppppppppppppaaaaapppaaaapaaaaapppppaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaappppppppppppppppppppaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaappppppppppppppppppppaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaappappapppppppappapppaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaapaaaaaaappppaaaaaaappppaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaappaaaaaaaappppaaaaaaappppaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaapppaaaaaaaaaaappaaaaaaaaappaaaaaaaa",
"aaaaaaaaaaaaaaaaaaaaaaaapppppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaappppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
"aappppppppppppppaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"};
#endif

FILE *fp; unsigned long ir=0L, ia=9301L, ic=49297L, im=233280L;
void wi(unsigned int i) { int b1,b0; b0=i%256; b1=(i-b0)/256; putc(b0,fp); putc(b1,fp); }
void wl(long l) { unsigned int i1,i0; i0=l%65536L; i1=(l-i0)/65536L; wi(i0); wi(i1); }
double rnd(void){ ir = (ir*ia+ic) % im; return ir / (1.0*im); }

int main(int argc, char *argv[])
{
   int i, j, d=D, ss;
   long k=0L, l, ns=2L*(long)(0.5*FS*T), m=ns/N,
        sso=HIFI?0L:128L, ssm=HIFI?32768L:128L;
   double **A, a, t, dt=1.0/FS, *w, *phi0, s, y, yp, z, tau1, tau2, x, theta,
                  scale=0.5/sqrt(N), q, q2, r, sl, sr, tl, tr, yl, ypl, yr, ypr,
          zl, zr, hrtf, hrtfl, hrtfr, v=340.0, /* v = speed of sound (m/s) */
          hs=0.20;  /* hs = characteristic acoustical size of head (m) */
   w    = C_ALLOC(N, double);
   phi0 = C_ALLOC(N, double);
   A    = C_ALLOC(N, double *);
   for (i=0; i<N; i++) A[i] = C_ALLOC(N, double);  /* N x N pixel matrix */

   /* Set hard-coded image */
   for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
         if (BW) { if (P[N-i-1][j]!='#') A[i][j]=WHITE; else A[i][j]=BLACK; }
         else if (P[i][j]>'a') A[N-i-1][j] = pow(10.0,(P[i][j]-'a'-15)/10.0); /* 2dB steps */
         else A[N-i-1][j] = 0.0;
      }
   }

   /* Set lin|exp (0|1) frequency distribution and random initial phase */
   if (d) for (i=0; i<N; i++) w[i] = TwoPi * FL * pow(1.0* FH/FL,1.0*i/(N-1));
   else   for (i=0; i<N; i++) w[i] = TwoPi * FL + TwoPi * (FH-FL)   *i/(N-1) ;
   for (i=0; i<N; i++) phi0[i] = TwoPi * rnd();

   /* Write 8/16-bit mono/stereo .wav file, using rectangular time window */
   fp = fopen(FNAME,"wb"); fprintf(fp,"RIFF"); wl(ns*HIST+36L);
   fprintf(fp,"WAVEfmt "); wl(16L); wi(1); wi(STEREO?2:1); wl(0L+FS);
   wl(0L+FS*HIST); wi(HIST); wi(HIFI?16:8); fprintf(fp,"data"); wl(ns*HIST);
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
      if (HIFI) wi(ss); else putc(ss,fp);
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
      if (HIFI) wi(ss); else putc(ss,fp);  /* Left channel */
      l   = sso + 0.5 + scale * ssm * yr;
      if (l >= sso-1+ssm) l = sso-1+ssm; if (l < sso-ssm) l = sso-ssm;
      ss  = (unsigned int) l;
      if (HIFI) wi(ss); else putc(ss,fp);  /* Right channel */
      k++;
   }
   fclose(fp); return(0);
}
/* End of program </xmp> */
