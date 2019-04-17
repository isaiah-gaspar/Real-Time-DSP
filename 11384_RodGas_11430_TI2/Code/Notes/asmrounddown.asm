	.mmregs

    .global _asmrounddown     ; make this function available everywhere


    .text  
	
_asmrounddown:
 

	BCC ZERO, AR0 == #0
	MOV #0, AC1
	MOV #0, AC0
	MOV T0, HI(AC0)
	 
	BCLR SXMD 
	ADD T1, AC0 
	BSET SXMD 
	
LOOP:
	ADD AC0, AC1
	ADD #-1,AR0
	BCC LOOP, AR0 != #0
	
	BCLR SXMD
	MOV HI(AC1), T2
	BSET SXMD
	
	ADD T2, AR1
	
ZERO:

	MOV *AR1,T0
	
	ADD #1, AR1
	MOV *AR1, T1
	SUB T0,T1
	
	BCLR SXMD
	MOV HI(AC1),T2
	BSET SXMD
	
	MOV #0, AC0
	MOV T2, AC0
	MPY T1, AC0
	
	BCLR SXMD 
	MOV HI(AC0), T1
	BSET SXMD
	ADD T1, T0
	RET
	
	
	
 .END


