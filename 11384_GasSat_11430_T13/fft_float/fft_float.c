/*
    fft_float.c - Floating-point complex radix-2 decimation-in-time FFT
    Perform in place FFT the output overwrite the input array
*/

#include "fcomplex.h"       /* floating-point complex.h header file */

void fft(complex *X, unsigned int EXP, complex *W, unsigned int SCALE)
{
    complex temp;           /* temporary storage of complex variable */
    complex U;              /* Twiddle factor W^k */
    unsigned int i,j;
    unsigned int id;        /* Index for lower point in butterfly */
    unsigned int N=1<<EXP;  /* Number of points for FFT */
    unsigned int L;         /* FFT stage */
    unsigned int LE;        /* Number of points in sub DFT at stage L
                               and offset to next DFT in stage */
    unsigned int LE1;       /* Number of butterflies in one DFT at
                               stage L.  Also is offset to lower point
                               in butterfly at stage L */
                               
    complex t1, t2;	/* Temporary Variables*/
    
    float scale;
    
    scale = 0.5;     
    if (SCALE == 0)         
        scale = 1.0;
    
    for (L=1; L<=EXP; L++) 	/* FFT butterfly */
    {
        LE=1<<L;        	/* LE=2^L=points of sub DFT */
        LE1=LE>>1;      	/* Number of butterflies in sub-DFT */
        U.re = 1.0;
        U.im = 0.;

        for (j=0; j<LE1 ;j++) // Number of butterflies in a stage w/ different w, j<__
        {
            for(i=j; i<N; i+=LE) /* Do the butterflies */ //Offset b/w 2 butterflies with same W per stage
            {
                id=i+LE1; //i+____
           
               t1.re = (X[i].re + U.re * X[id].re - U.im * X[id].im);
               t1.im = (X[i].im + U.re * X[id].im + U.im * X[id].re);
               
               t2.re = (X[i].re - U.re * X[id].re + U.im * X[id].im); 
               t2.im = (X[i].im - U.re * X[id].im - U.im * X[id].re);
            
               X[i].re = scale * t1.re;              
               X[i].im = scale * t1.im;              
               
               
               X[id].re = scale * t2.re;
               X[id].im = scale * t2.im;
               
                                                           
               // Compute butterfly as defined in 3b - in place computation
            }
            
            /* Recursive compute W^k as U*W^(k-1) */
            //update twiddle factor by calculating temp.re
            temp.re = U.re;
            temp.im = U.im;
            U.im = U.re*W[L-1].im + U.im*W[L-1].re;
            
            U.re = temp.re*W[L-1].re - temp.im*W[L-1].im;
        }
    }
}
