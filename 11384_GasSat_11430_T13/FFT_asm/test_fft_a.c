/*
     exp9 
      Example to test FFT (fixed-point)
     
*/

#include "icomplex.h"
#include "sine.dat" /* Experiment data */

extern void fft(complex *, unsigned int, complex *, unsigned int);
extern void bit_rev(complex *, unsigned int);
extern void w_table(complex *, unsigned int);

#define N 128           /* Number of FFT points */
#define EXP 7           /* log2(N)              */

#pragma DATA_SECTION(U, "fft_coef");
#pragma DATA_SECTION(X, "fft_in");  
#pragma DATA_SECTION(spectrum, "fft_out");
#pragma DATA_SECTION(ltemp, "fft_temp"); 
#pragma DATA_SECTION(temp, "fft_temp"); 
#pragma CODE_SECTION(main, "fft_code"); 
                          
complex X[N];           /* Declare input array  */
complex U[N];           /* Twiddle e^(-j2pi/N) table */    
complex temp;
lcomplex ltemp;
int spectrum[N];
int re1[N],im1[N];

void main()
{
    unsigned int i,j;
    
    /* Create Twiddle lookup table for FFT */
    w_table(U,EXP);      
  
    /* Start FFT test */   
    j=0;
    for (;;)
    {
        for (i=0; i<N; i++)  
        {
            /* Generate input samples */
            X[i].re = sine_i[j++];
            X[i].im = 0;
            /* Copy to reference buffer */
            re1[i] = X[i].re;
            im1[i] = X[i].im;        

            if (j==128)
                j=0;
    	}
    
        bit_rev(X,EXP);   /* Arrange sample in bit reversal order */
        fft(X,EXP,U,1);   /* Perform FFT */
    
        for (i=0; i<N; i++) /* Verify FFT result */ 
        {
            ltemp.re = (long)X[i].re*X[i].re;
            ltemp.im = (long)X[i].im*X[i].im;        
            spectrum[i] = (int)((ltemp.re+ltemp.im)>>13); 
        }
    }
}

