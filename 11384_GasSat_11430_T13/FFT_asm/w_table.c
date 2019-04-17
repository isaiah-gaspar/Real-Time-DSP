/*
    w_table.c - Generate twiddle factor lookup table
*/                                                  

#include "icomplex.h"
#include <math.h>

#define pi 3.1415926535897

void w_table(complex *U, unsigned int EXP)
{
    complex W;               /* Twiddle factor W^k                  */
    unsigned int i,j,l;
    unsigned int SL;         /* Number of points in sub DFT at stage L
                                and offset to next DFT in stage      */
    unsigned int SL1;        /* Number of butterflies in one DFT at
                                stage L.  Also is offset to lower point
                                in butterfly at stage L              */

    for (i=0, l=1; l<=EXP; l++) 
    {
        SL=1<<l;        	/* LE=2^L=points of sub DFT */
        SL1=SL>>1;     		/* Number of twiddle factors in sub-DFT */
        for (j=0; j<SL1; j++)
        {                             
            W.re = (int)((0x7fff*cos(j*pi/SL1))+0.5);
            W.im = -(int)((0x7fff*sin(j*pi/SL1))+0.5);        
            U[i++] = W;
        }
    }
}
