/*
     exp7a.c - Example to test FFT
*/

#include <math.h>   
#include "icomplex.h"    /* integer complex.h header file */
#include "sine_i.dat"

extern void fft(complex *, unsigned int, complex *, unsigned int);
extern void bit_rev(complex *, unsigned int);

#define N 128           /* Number of FFT points */
#define EXP 7           /* EXP=log2(N)              */
#define pi 3.1415926535897  
#define K 3             /* Index of Xk (0<K< N/2) */ 
                       
complex X[N];           /* Declare input array  */
complex W[EXP];         /* Twiddle e^(-j2pi/N) table */    
lcomplex ltemp;
int spectrum[N];
int re1[N],im1[N];

void main()
{
    unsigned int i,j,L,LE,LE1;   

    for (L=1; L<=EXP; L++) /* Create twiddle factor table */
    {
        LE=1<<L;           /* LE=2^L=points of sub DFT */
        LE1=LE>>1;     	   /* Number of butterflies in sub-DFT */
        W[L-1].re = (int)((0x7fff*cos(pi/LE1))+0.5);
        W[L-1].im = -(int)((0x7fff*sin(pi/LE1))+0.5);
    }    
    
    j=0;
    for (;;)
    {
        for (i=0; i<N; i++) 
        {
            /* Construct input samples */
            X[i].re = sine_i[j++];
            X[i].im = 0;        
            /* Copy to reference buffer */
            re1[i] = X[i].re;
            im1[i] = X[i].im;
 
            if (j==1664)
                j=0;
        } 	

        /* Start FFT */
        bit_rev(X,EXP);     /* Arrange X[] in bit-reverse order */
        fft(X,EXP,W,1);	    /* Perform FFT */

        for (i=0; i<N; i++) /* Verify FFT result */
        {
            ltemp.re = (long)X[i].re*X[i].re;
            ltemp.im = (long)X[i].im*X[i].im;        
            spectrum[i] = (int)((ltemp.re+ltemp.im)>>16);
        }
    }
}

