/*
	Header file icomplex.h for fixed-point
*/

struct cmpx      /* Q15 format */
{
	int re;
	int im;
};
typedef struct cmpx complex;

struct lcmpx     /* Q31 format */
{
	long re;
	long im;
};
typedef struct lcmpx lcomplex;

