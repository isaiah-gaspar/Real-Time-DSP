/*
    test_fft.c - Example to test floating-point Complex FFT
*/

#include <math.h>   
#include "fcomplex.h"   /* floating-point complex.h header file */
#include "sine_f.dat"  /* Experiment floating-point data */

extern void fft(complex *, unsigned int, complex *, unsigned int);
extern void bit_rev(complex *, int);

#define N 128           /* Number of FFT points */
#define EXP 7           /* EXP=log2(N) */
#define pi 3.1415926535897     

complex X[N];           /* Declare input array  */
complex W[EXP];         /* Twiddle e^(-j2pi/N) table */    
complex temp;
float spectrum[N];
float re1[N],im1[N];                                   

void main()
{
    unsigned int i,j,L,LE,LE1;
    
    for (L=1; L<=EXP; L++) /* Create twiddle factor table */
    {
        LE=1<<L;         /* LE=2^L=points of sub DFT */
        LE1=LE>>1;       /* Number of butterflies in sub DFT */
        W[L-1].re = cos(pi/LE1);
        W[L-1].im = -sin(pi/LE1);
    }    
    
    j=0;          
    for (;;)
    {
        for (i=0; i<N; i++) 
        {
            /* Generate input samples */
            X[i].re = sine_f[j++];	
            X[i].im = 0.0;   
            /* Copy to reference buffer */     
            re1[i] = X[i].re;
            im1[i] = X[i].im;
   
            if (j == 1664)
                j = 0;
        }
        
        /* Start FFT */
        bit_rev(X,EXP);      /* Arrange X[] in bit reversal order */
        fft(X,EXP,W,1);      /* Perform FFT */

        /* Verify FFT result */
        for (i=0; i<N; i++)  
        {
            /* Compute spectrum */
            temp.re = X[i].re*X[i].re;
            temp.im = X[i].im*X[i].im;        
            spectrum[i] = (temp.re+temp.im)*4;
        }        	
    }
}

