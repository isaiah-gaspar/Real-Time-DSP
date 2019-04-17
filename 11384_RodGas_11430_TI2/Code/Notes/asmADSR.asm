	
	.mmregs

    .global _asmADSR     ; make this function available everywhere


    .text  
	
_asmADSR:

	;MOV , AR0
	;MOV , AR1
	
	
	MOV #30, AR3 
	CMP T0 > AR3, TC1
	BCC MID,  TC1
	MOV #970, AR3
	CMP T0 > AR3, TC1
	BCC LAST, TC1
	
	B DONE

MID:		
	ADD #1, AR0
	ADD #1, AR1

	B DONE
LAST:
	ADD #2, AR0
	ADD #2, AR1


	B DONE
	
DONE:
	
	BSET FRCT
	MPY *AR0,*AR1, AC0
	BCLR FRCT
	
	SUB #1, T1, T3
	
	BSET FRCT
	MPY *AR0, T3, AC1
	BCLR FRCT
	
	SUB AC0, AC1
	ADD T3, AC1
	
	RET
	
.END  

