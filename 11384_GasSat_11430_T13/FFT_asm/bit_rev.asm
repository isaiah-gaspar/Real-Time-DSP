;
;    bit_rev.asm - Bit-reversal for FFT (N=2^EXP points)
;                               
;    Prototype: void bit_rev(complex *, int); 
;
;    Entry: arg0: AR0-FFT input sample buffer pointer   
;           arg1: T0-number of FFT stage, EXP
;
;    

     .global  _bit_rev

     .sect    "fft_code"

_bit_rev
    psh  mmap(ST2_55)        ; Save ST2
    bclr ARMS                ; Reset ARMS bit
    mov  #1,AC0
    sfts AC0,T0	             ; T0=EXP, AC0=N=2^EXP
    mov  AC0,T0              ; T0=N
    mov  T0,T1              
    add  T0,T1              
    mov  mmap(T1),BK03       ; Circular buffer size=2N
    mov  mmap(AR0),BSA01     ; Init circular buffer base
    sub  #2,AC0
    mov  mmap(AC0L),BRC0     ; Init repeat counter to N-1
    mov  #0,AR0              ; Set buffer start address
    mov  #0,AR1              ;   as offset = 0
    bset AR0LC               ; Enable AR0 and AR1 as
    bset AR1LC               ;   circular pointers
||  rptblocal loop_end-1     ; Start bit reversal loop
    mov  dbl(*AR0),AC0       ;   Get a pair of sample
||  amov AR1,T1  
    mov  dbl(*AR1),AC1       ;   Get another pair
||  asub AR0,T1
    xccpart swap1,T1>=#0
||  mov  AC1,dbl(*AR0+)      ;   Swap samples if j>=i
swap1               
    xccpart loop_end,T1>=#0    
||  mov  AC0,dbl(*(AR1+T0B)) 
loop_end                     ; End bit reversal loop

    pop  mmap(ST2_55)        ; Restore ST2
    ret
    
    .end
