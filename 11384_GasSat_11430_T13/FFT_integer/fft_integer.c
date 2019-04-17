/*
    fft_a.c - Radix 2 decimation in time FFT 
    Using C55x C intrinsics to perform in place FFT, 
    the output overwrite the input array
*/

#include "icomplex.h"
#include <intrindefs.h> 

#define  SFT16 16

void fft(complex *X, unsigned int EXP, complex *W, unsigned int SCALE)
{
    complex temp;          /* temporary storage of int complex variable */      
    lcomplex ltemp;        /* temporary storage of long complex variable */    
    complex U;             /* Twiddle factor W^k */
    unsigned int i,j;
    unsigned int id;       /* Index for lower point in butterfly */
    unsigned int N=1<<EXP; /* Number of points for FFT */
    unsigned int L;        /* FFT stage */
    unsigned int LE;       /* Number of points in sub DFT at stage L
                              and offset to next DFT in stage */
    unsigned int LE1;      /* Number of butterflies in one DFT at
                              stage L.  Also is offset to lower point
                              in butterfly at stage L */
    int scale;
    
    scale = 1;     
    if (SCALE == 0)         
        scale = 0;

    for (L=1; L<=EXP; L++) /* FFT butterfly */
    {
        LE=1<<L;    /* LE=2^L=points of sub DFT */
        LE1=LE>>1;  /* Number of butterflies in sub DFT */
        U.re = 0x7fff;
        U.im = 0;

        for (j=0; j<LE1;j++)
        {
            for(i=j; i<N; i+=LE) /* Do the butterflies */
            {
                id=i+LE1;                 
                ltemp.re = _lsmpy(U.re, X[id].re);								//ltemp.re = U.re*Q.re
                temp.re = (_smas(ltemp.re, X[id].im, U.im)>>SFT16);  			//temp.re = ltemp.re+(Q.im*U.im)
                temp.re = _sadd(temp.re, 1)>>scale; /* Rounding & scale */		//scaling part
                ltemp.im = _lsmpy(X[id].im, U.re);								//ltemp.im = Q.im*U.re
                temp.im = (_smac(ltemp.im, X[id].re, U.im)>>SFT16);				//temp.im = ltemp.im+(Q.re*U.im)
                temp.im = _sadd(temp.im, 1)>>scale; /* Rounding & scale */		//scaling part
                X[id].re = _ssub(X[i].re>>scale, temp.re);						//Qre = P.re-(temp.re)
                X[id].im = _ssub(X[i].im>>scale, temp.im); 						//Qimg = P.im-(temp.im)
                X[i].re = _sadd(X[i].re>>scale, temp.re); 						//Pre = P.re+temp.re
                X[i].im = _sadd(X[i].im>>scale, temp.im); 						//Pimg = P.im+temp.im
            }
            
            /* Recursive compute W^k as W*W^(k-1) */
            ltemp.re = _lsmpy(U.re, W[L-1].re);					//U.re*W.re
            ltemp.re = _smas(ltemp.re, U.im, W[L-1].im);  		//ltemp.re-(U.im*W.im)
            ltemp.im = _lsmpy(U.re, W[L-1].im);					//U.re*W.im
            ltemp.im = _smac(ltemp.im, U.im, W[L-1].re);		//U.im*W.re+ltemp.im
            U.re = ltemp.re>>SFT16;            
            U.im = ltemp.im>>SFT16;
        }
    }
}  
