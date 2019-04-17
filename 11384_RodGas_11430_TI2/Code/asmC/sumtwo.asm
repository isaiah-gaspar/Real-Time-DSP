;
;  ======== load.asm ========
;
;  C-callable interface to assembly language utility functions for the
;  asmC example.

    .mmregs

    .global _asm_sumtwo     ; make this function available everywhere


    .text                    ; start of executable mnemonics 

	
_asm_sumtwo:
	MOV T0, AC1
	MOV T1, AC0
	ADD AC1, AC0							; add the two passed values and return the sum
done:
    RET                         ;/* end load() */


