
#include <stdio.h>

/* Global declarations */

/* Functions */
extern long asm_sumtwo(int a, int b);



/*
 * ======== main ========
 */
void main()
{
    long sum;
    int a = 52;
    int b = 14;

    puts("example started\n");

	sum = asm_sumtwo(a,b);    
    return;
}


    
