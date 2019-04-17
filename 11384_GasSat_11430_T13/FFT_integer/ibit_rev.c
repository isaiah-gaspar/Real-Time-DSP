/* Arrange input samples in bit-reverse addressing order
   the index j is the bit reverse of i */

#include "icomplex.h"    /* integer complex.h header file */

void bit_rev(complex *X, unsigned int EXP)
{
    unsigned int i,j,k;
    unsigned int N=1<<EXP; /* Number of points for FFT */
    unsigned int N2=N>>1;
    complex temp;          /* temporary storage of int complex variable */	
	
    for (j=0,i=1;i<N-1;i++)
    {
        k=N2;
        while(k<=j)
        {
            j-=k;
            k>>=1;
        }
        j+=k;

        if (i<j)
        {
            temp = X[j];
            X[j] = X[i];
            X[i] = temp;
        }
    }  
}
