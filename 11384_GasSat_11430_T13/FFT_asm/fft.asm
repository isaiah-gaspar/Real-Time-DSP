;
;   fft.asm - Radix-2 complex FFT (for N=2^EXP points)
;             Using table lookup method
;                               
;   Prototype: void fft(complex *, int, complex *, int); 
;
;   Entry:  arg0: AR0-FFT input sample buffer pointer   
;           arg1: T0-number of FFT stage, EXP
;           arg2: AR1-twiddle factor array pointer     
;           arg3: T1-scale flag 
;                 if scale needed, (T1==1) 
;                 if don't need scale (T1==0)
;                                                      
;   Note: 1. When used as the FFT routine it takes in time domain 
;         samples in Q15 and generates Q14 frequency domain samples
;         2. When used as the IFFT routine it takes in frequency domain 
;         samples in Q14 and generates Q15 time domain samples
;


        .global _fft		;comment

;
;	Declare local variables in SP address mode for C-callable
;
ARGS    .set    0       ; Number of variables passed via stack

FFT_var .struct         ; Define local variable structure
d_temp      .int (2)    ; Temporary variables (Re, Im) insert the number of temp variables needed
d_L         .int
d_N         .int 		; insert the directive needed		
d_T2        .int        ; Used to save content of T2
d_ST1       .int        ; Used to save content of ST1
d_ST3       .int        ; Used to save content of ST3
return_addr .int        ; Space for routine return address
Size        .endstruct
fft         .set 0
fft         .tag FFT_var  ; comment 

    .sect "fft_code"       ; comment
	
_fft:
    aadd #(ARGS-Size+1),SP      ; Adjust stack for local vars

    mov  mmap(ST1_55),AR2       ; Save ST1,ST3 to AR2 and AR3
    mov  mmap(ST3_55),AR3   
    mov  AR2,fft.d_ST1
    mov  AR3,fft.d_ST3
    btst @#0,T1,TC1             ; Check SCALE flag set		
    mov  #0x6340,mmap(ST1_55)   ; Set CPL,XF,SATD,SXAM,FRCT (SCALE=1) 
    mov  #0x1f22,mmap(ST3_55)   ; Set: HINT,SATA,SMUL   
    xcc  do_scale,TC1
    mov  #0x6300,mmap(ST1_55)   ; Set CPL,XF,SATD,SXAM (SCALE=2) 
do_scale
    
    mov  T2,fft.d_T2            ; Save T2                     
||  mov  #1,AC0
    mov  AC0,fft.d_L            ; Initialize L=1
||  sfts AC0,T0                 ; T0=EXP
    mov  AC0,fft.d_N            ; N=1<<EXP
    mov  XAR1,XCDP              ; comment
    mov  XSP,XAR4	
    add  #fft.d_temp,AR4        ; AR4 = pointer to temp
    mov  XAR0,XAR1              ; AR1 points to sample buffer
    mov  T0,T1					; comment
    mov  XAR0,XAR5              ; Copy externd bits to XAR5
			
outer_loop                      ; for (L=1; L<=EXP; L++)    
    mov  fft.d_L,T0             ; note: Since the buffer is
||  mov  #2,AC0                 ;       arranged in re,im pairs
    sfts AC0,T0                 ;       the index to the buffer
    neg  T0	                    ;       is doubled
||  mov  fft.d_N,AC1            ;       But the repeat coutners
    sftl AC1,T0                 ;       are not doubled
    mov  AC0,T0                 ; LE=2<<L 	
||  sfts AC0,#-1  				; comment	
    mov  AC0,AR0                ; LE1=LE>>1           
||  sfts AC0,#-1                              
    sub  #1,AC0                 ; Init mid_loop counter
    mov  mmap(AC0L),BRC0        ;   BRC0=LE1-1
    sub  #1,AC1                 ; Init inner loop counter
    mov  mmap(AC1L),BRC1        ;   BRC1=(N>>L)-1   
    add  AR1,AR0   				; comment
    mov  #0,T2                  ; j=0 
||  rptblocal mid_loop-1        ; for (j=0; j<LE1;j++) 
    mov  T2,AR5                 ; AR5=id=i+LE1
    mov  T2,AR3	
    add  AR0,AR5                ; AR5 = pointer to X[id].re   
    add  #1,AR5,AR2             ; AR2 = pointer to X[id].im  
    add  AR1,AR3                ; AR3 = pointer to X[i].re 
||  rptblocal inner_loop-1      ; for(i=j; i<N; i+=LE) 
    mpy  *AR5+,*CDP+,AC0        ; AC0=(X[id].re*U.re
::  mpy  *AR2-,*CDP+,AC1        ;     -X[id].im*U.im)/SCALE	 
    masr *AR5-,*CDP-,AC0        ; AC1=(X[id].im*U.re                       
::  macr *AR2+,*CDP-,AC1        ;     +X[id].re*U.im)/SCALE
    mov  pair(hi(AC0)),dbl(*AR4); AC0H=temp.re AC1H=temp.im 
||  mov  dbl(*AR3),AC2
    xcc  scale,TC1
||  mov  AC2>>#1,dual(*AR3)     ; Scale X[i] by 1/SCALE
    mov  dbl(*AR3),AC2  
scale
    add  T0,AR2
||  sub  dual(*AR4),AC2,AC1     ; comment
    mov  AC1,dbl(*(AR5+T0))     ; X[id].im=X[i].im/SCALE-temp.im
||  add  dual(*AR4),AC2         ; X[i].re=X[i].re/SCALE+temp.re
    mov  AC2,dbl(*(AR3+T0))     ; comment
inner_loop                      ; End of inner loop       
    amar *CDP+
    amar *CDP+                  ; Update k for pointer to U[k]
||  add  #1,T2                  ; Update j    
mid_loop                        ; End of mid-loop      
    sub  #1,T1          
    add  #1,fft.d_L             ; Update L
    bcc  outer_loop,T1>0        ; End of outer-loop

    mov  fft.d_ST1,AR2          ; Restore ST1,ST3,T2
    mov  fft.d_ST3,AR3
    mov  AR2,mmap(ST1_55)
    mov  AR3,mmap(ST3_55)                              
    mov  fft.d_T2,T2
    aadd #(Size-ARGS-1),SP      ; Reset SP
    ret

    .end 
