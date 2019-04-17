/*
 *  Copyright 2002 by Spectrum Digital Incorporated.
 *  All rights reserved.  Property of Spectrum Digital Incorporated.
 */

/*
 *  ======== dsk_app1.c ========
 *
 *  Version 1.00
 *
 *  This example digitally processes audio data from the line input on the
 *  AIC23 codec and plays the result on the line output.  It uses the McBSP
 *  and DMA to efficiently handle the data transfer without intervention from
 *  the DSP.
 *
 *  Data transfer
 *
 *  Audio data is transferred back and forth from the codec through McBSP2,
 *  a bidirectional serial port.  The DMA is configured to take every 16-bit
 *  signed audio sample arriving on McBSP2 and store it in a buffer in memory
 *  until it can be processed.  Once it has been processed, it is sent back out
 *  through McBSP2 to the codec for output.  DMA channel 0 is used for
 *  transmitting data to the codec and channel 1 is used to receive data from
 *  the codec.
 *
 *  McBSP1 is used to control/configure the AIC23.
 *
 *  Beyond basic DMA, this example uses 3 special features to implement an
 *  especially robust and efficient data transfer infrastructure:
 *
 *  1)  Ping-pong buffering
 *  2)  Automatic channel sorting by the DMA controller
 *  3)  Automatic re-initialization (autoinit is turned ON)
 *
 *  Ping-pong buffering is a technique where two buffers (referred to as the
 *  PING buffer and the PONG buffer) are used for a data transfer instead of
 *  only one.  The DMA is configured to use the PING buffer first.  When it is
 *  full, the DMA is reconfigured to use the PONG buffer.  At that time, the
 *  PING buffer can be processed without fear of being overwritten.  The
 *  ping-ponging continues indefinitely with one buffer always hosting the
 *  active transfer and one remaining stable for the DSP to operate on.  Ping-
 *  pong buffers are used because a single buffer is always being overwritten
 *  so it is never in a stable state that can be examined by the DSP for any
 *  reasonable length of time.  This example uses ping-pong buffers for both
 *  transmit and receive for a total of four logical buffers. Using a double-
 *  buffered system provides a processing window of time equal to an entire
 *  buffer size vs. a single sample time.
 *
 *  Audio data from the AIC23 arrives in interleaved format with a 16-bit
 *  sample from the left channel followed immediately by a 16-bit sample from
 *  the right channel.  Audio processing algorithms typically operate on only
 *  one channel at a time so the interleaved data must be channel sorted such
 *  that all the left samples are contiguous in one memory region and the
 *  right samples are contiguous in another.
 *
 *  This example uses a feature of the DMA controller called double indexing
 *  to channel sort the data so the left samples are placed in the first half
 *  of the data buffer and the right samples in the second half.  It takes
 *  advantage of a special DMA controller feature called double-indexing.  The
 *  DMA is set up so that each pair of element samples (one left and one right)
 *  is organized into a logical frame.
 *
 *  The CEI register contains the increment to add to the receive buffer
 *  pointer position after a single element is received.  In initDma(), CEI is
 *  set to jump forward by half a buffer length after the left element
 *  arrives.  CFI contains the increment used after the last element in the
 *  frame (right) arrives.  It is set to jump backwards by half a buffer
 *  length to just after the left sample that arrived before it which is the
 *  location of the next left sample.  The bouncing back and forth continues
 *  until the buffer is filled.  Use of the double-indexing feature allows the
 *  DMA controller to automatically sort the data while it is being received
 *  with no intervention by the DSP.
 *
 *  The DMA operates on its own while a buffer is being filled.  At that point
 *  an interrupt is generated and the DMA must be reconfigured for a new
 *  buffer and restarted before another audio sample is received.  If auto-
 *  initialization is enabled, the new buffer settings can be configured
 *  at any point while the current buffer is being filled by placing the
 *  new buffer address in a configuration register.  The DMA controller will
 *  automatically reload the working registers with the contents of the
 *  configuraiton register and start in on the new buffer when the current
 *  is full.  This greatly reduces the real-time requirements on the interrupt
 *  handler.
 *
 *
 *  Program flow
 *
 *  When the program is run, the individual DSP/BIOS modules are initialized
 *  as configured in dsk_app1.cdb with the DSP/BIOS configuration tool.  The
 *  main() function is then called as the main user thread.  In this example
 *  main() performs application initialization and starts the DMA data
 *  transfers.  When main exits, control passes back entirely to DSP/BIOS
 *  which services any interrupts or threads on an as-needed basis.  When
 *  there is no work to be done, the idle thread is run where we have placed
 *  power-down control instructions. So, when the BIOS IDL thread is reached,
 *  the processor powers down according to the bits that are set in the Idle
 *  Control Register. A McBSP interrupt or DMA interrupt can wake up the CPU.
 *
 *  The dmaHwi() interrupt service routine is called when a buffer has been
 *  filled.  It contains a state variable named pingOrPong that indicates
 *  whether the buffer is a PING or PONG buffer.  dmaHwi switches the buffer
 *  state to the opposite buffer and calls the SWI thread processBuffer to
 *  process the audio data.
 *
 *  The processBuffer() SWI uses a highly optimized assembly language high
 *  pass FIR filter the incoming audio data if DIP switch #3 is depressed.
 *  If switch 3 is up, the code operates in audio pass-thru mode only (no
 *  filtering).  While processBuffer() is running, LEDs #3 and #2 toggle when
 *  exiting the PING and PONG states respectively so you can visually monitor
 *  their operation.
 *
 *  The example includes a few other functions that are executed in the
 *  background as examples of the multitasking that DSP/BIOS is capable of:
 *
 *  1)  blinkLED() toggles LED #0 every 500ms if DIP switch #0 is depressed.
 *      It is a periodic thread with a period of 500 ticks.
 *
 *  2)  load() simulates a 20-25% dummy load if DIP switch #1 is depressed.
 *      It represents other computation that may need to be done.  It is a
 *      periodic thread with a period of 10ms.
 *
 *  Please see the 5510 DSK help file under Software/Examples for more
 *  detailed information on this example.
 */

/*
 *  DSP/BIOS is configured using the DSP/BIOS configuration tool.  Settings
 *  for this example are stored in a configuration file called dsk_app1.cdb.
 *  At compile time, Code Composer will auto-generate DSP/BIOS related files
 *  based on these settings.  A header file called dsk_app1cfg.h contains the
 *  results of the autogeneration and must be included for proper operation.
 *  The name of the file is taken from dsk_app1.cdb and adding cfg.h.
 */
#include "dsk_app1cfg.h"

/*
 *  The 5510 DSK Board Support Library is divided into several modules, each
 *  of which has its own include file.  The file dsk5510.h must be included
 *  in every program that uses the BSL.  This example also uses the
 *  DIP, LED and AIC23 modules.
 */
#include "dsk5510.h"
#include "dsk5510_led.h"
#include "dsk5510_dip.h"
#include "dsk5510_aic23.h"
#include "highpass.h"        // high pass filter coefficients

/*
 *  This program uses Code Composer's Chip Support Library to access 
 *  C55x peripheral registers and interrupt setup.  The following 
 *  include files are required for the CSL modules.
 */
#include <csl.h>
#include <csl_irq.h>
#include <csl_dma.h>
#include <csl_mcbsp.h>
#include <csl_pwr.h>
#include <csl_icache.h>
#include <clk.h>
#include <sts.h>

/* Constants for the buffered ping-pong transfer */
#define BUFFSIZE          2048
#define PING              0
#define PONG              1
#define ORDER             208

/* Function prototypes */
void initIrq(void);
void initDma(void);
void copyData(Int16 *inbuf, Int16 *outbuf, Int16 length);
void processBuffer(void);
void blinkLED(void);
void load(void);
void powerDown(void);
void dmaHwi(void);  

/* Codec configuration settings */
DSK5510_AIC23_Config config = { \
    0x0017,  /* 0 DSK5510_AIC23_LEFTINVOL  Left line input channel volume */ \
    0x0017,  /* 1 DSK5510_AIC23_RIGHTINVOL Right line input channel volume */\
    0x01f9,  /* 2 DSK5510_AIC23_LEFTHPVOL  Left channel headphone volume */  \
    0x01f9,  /* 3 DSK5510_AIC23_RIGHTHPVOL Right channel headphone volume */ \
    0x0010,  /* 4 DSK5510_AIC23_ANAPATH    Analog audio path control */      \
    0x0000,  /* 5 DSK5510_AIC23_DIGPATH    Digital audio path control */     \
    0x0000,  /* 6 DSK5510_AIC23_POWERDOWN  Power down control */             \
    0x0043,  /* 7 DSK5510_AIC23_DIGIF      Digital audio interface format */ \
    0x0081,  /* 8 DSK5510_AIC23_SAMPLERATE Sample rate control */            \
    0x0001   /* 9 DSK5510_AIC23_DIGACT     Digital interface activation */   \
};

/*
 * Data buffer declarations - the program uses four logical buffers of size
 * BUFFSIZE, one ping and one pong buffer on both receive and transmit sides.
 * Since the DMA controller sorts the data so that the left channel data
 * resides in the top half of the buffer and the right channel resides in the
 * bottom half, the buffers are actually defined as eight buffers of size
 * BUFFSIZE/2.  The total space is the same, but the left and right channels
 * can be referred to individually.  This compiler will allocate the buffers
 * sequentially so, for example, the receive ping buffer can be referred to a
 * single buffer of BUFFSIZE elements at base gBufferRcvPingL or two contiguous
 * buffers of BUFFSIZE/2 elements with bases at gBufferRcvPingL and
 * gBufferRcvPingR.
 */
Int16 gBufferRcvPingL[BUFFSIZE/2];  // Top of receive PING buffer
Int16 gBufferRcvPingR[BUFFSIZE/2];
Int16 gBufferRcvPongL[BUFFSIZE/2];  // Top of receive PONG buffer
Int16 gBufferRcvPongR[BUFFSIZE/2];
Int16 gBufferXmtPingL[BUFFSIZE/2];  // Top of transmit PING buffer
Int16 gBufferXmtPingR[BUFFSIZE/2];
Int16 gBufferXmtPongL[BUFFSIZE/2];  // Top of transmit PONG buffer
Int16 gBufferXmtPongR[BUFFSIZE/2];

Int16 delayBufferL[ORDER+2]={0};    // Delay Buffers for DSPLIB routine
Int16 delayBufferR[ORDER+2]={0};

/*
 * Create a pointer to hold the addresses of the buffers start 
 * The FIR routine requires pointer indirection
 */ 
Int16 *delayPtrL = &(delayBufferL[0]);
Int16 *delayPtrR = &(delayBufferR[0]);

/* Event IDs, global so they can be set in initIrq() and used everywhere */
Uint16 eventIdRcv;
Uint16 eventIdXmt;


/* ------------------------Helper Functions ----------------------------- */ 

/*
 *  initIrq() - Initialize and enable the DMA receive interrupt using the CSL.
 *              The interrupt service routine for this interrupt is hwiDma.
 *              The interrupt enable and flag bits of this interrupt is bit 9
 *              of the DSP's IER0 and IFR0 registers.  The transmit interrupt
 *              is configured but not enabled so the program can detect when
 *              a block has been fully transmitted.
 */
void initIrq(void)
{
    // Get Event ID associated with DMA channel interrupt.  Event IDs are a
    // CSL abstraction that lets code describe a logical event that gets
    // mapped to a real physical event at run time.  This helps to improve
    // code portability.
    eventIdRcv = DMA_getEventId(hDmaRcv);
    eventIdXmt = DMA_getEventId(hDmaXmt);
    
    // Clear any pending receive channel interrupts (IFR)
    IRQ_clear(eventIdRcv);
    IRQ_clear(eventIdXmt);
 
    // Enable receive DMA interrupt (IMR)
    IRQ_enable(eventIdRcv); 
    
    // Make sure global interrupts are enabled
    IRQ_globalEnable();
}


/*
 *  initDma() - Initialize the DMA controller.  The actual DMA register
 *              configuration is done in the DSP/BIOS configuration under
 *              Chip Support Library --> DMA --> DMA Configuration Manager
 *              and loaded at run time in the auto-generated file
 *              dsk_app1cfg_c.  initDma() initializes some registers not
 *              normally set in the DSP/BIOS config like CEI and CFI.  It
 *              also sets frame count based on BUFFSIZE so you can experiment
 *              with the buffer size by changing BUFFSIZE and recompiling
 *              without modifying your DSP/BIOS configuration settings.
 */
void initDma(void)
{
    volatile Int16 i;

    // Set indices and lengths for receive channel sorting.
    DMA_RSETH(hDmaRcv, DMACEI, BUFFSIZE - 1);
    DMA_RSETH(hDmaRcv, DMACFI, -(BUFFSIZE - 1));
    DMA_RSETH(hDmaRcv, DMACFN, BUFFSIZE >> 1);

    // Set indices for transfer channel unsorting
    DMA_RSETH(hDmaXmt, DMACEI, BUFFSIZE - 1);
    DMA_RSETH(hDmaXmt, DMACFI, -(BUFFSIZE - 1));
    DMA_RSETH(hDmaXmt, DMACFN, BUFFSIZE >> 1);
        
    // Clear the DMA status registers to receive new interrupts
    i = DMA_RGETH(hDmaRcv, DMACSR);
    i = DMA_RGETH(hDmaXmt, DMACSR);
}


/*
 *  copyData() - Copy one buffer with length elements to another.
 */
void copyData(Int16 *inbuf, Int16 *outbuf, Int16 length)
{
    Int16 i = 0;
    
    for (i = 0; i < length; i++) {                      
        outbuf[i]  = inbuf[i];
    }
}


/* ------------------------------- Threads ------------------------------ */

/*
 *  processBuffer() - Process audio data once it has been received, then
 *                    set the DMA configuration registers up for the next
 *                    transfer.  If DIP switch #3 is up, the audio passes
 *                    straight through.  If DIP switch #3 is down, the
 *                    audio is processed using the fir2 filter from TI's
 *                    DSPLIB.
 *                    
 */
void processBuffer(void)
{
    Uint32 addr;
    Int16 pingPong;
    Int16 switch3;

    // Wait until transmit DMA is finished too
    while(!IRQ_test(eventIdXmt));
    
    // Pong-pong state passed through mailbox from dmaHwi()
    pingPong = SWI_getmbox();

    // Read DIP switch 3
    switch3 = DSK5510_DIP_get(3);

    // Determine which ping-pong state we're in
    if (pingPong == PING)
    {
        // Toggle LED #3 as a visual cue
        DSK5510_LED_toggle(3);
        
        if (!switch3) {
            // Switch 3 is down, call assembly version of FIR filter
            STS_set(&fir_time,CLK_gethtime());

            // Process samples from left channel, then right channel
            fir2(gBufferRcvPingL, COEFFS, gBufferXmtPingL, delayBufferL, BUFFSIZE/2, ORDER);
             STS_delta(&fir_time,CLK_gethtime());
            fir2(gBufferRcvPingR, COEFFS, gBufferXmtPingR, delayBufferR, BUFFSIZE/2, ORDER);
           
        }

        if (switch3) {
            // Switch 3 is up, audio pass thru only
            copyData(gBufferRcvPingL, gBufferXmtPingL, BUFFSIZE/2);
            copyData(gBufferRcvPingR, gBufferXmtPingR, BUFFSIZE/2);
        }
        
        // Send a LOG_printf to CCS
        LOG_printf(&logTrace,"PING");

        // Configure the receive channel for ping input data
        addr = ((Uint32)gBufferRcvPingL) << 1;
        DMA_RSETH(hDmaRcv, DMACDSAL, addr & 0xffff);
        DMA_RSETH(hDmaRcv, DMACDSAU, (addr >> 16) & 0xffff);

        // Configure the transmit channel for ping output data
        addr = ((Uint32)gBufferXmtPingL) << 1;    
        DMA_RSETH(hDmaXmt, DMACSSAL, addr & 0xffff);
        DMA_RSETH(hDmaXmt, DMACSSAU, (addr >> 16) & 0xffff);    
    }
    else {
        // Toggle LED #2 as a visual cue
        DSK5510_LED_toggle(2);
        
        if (!switch3) {
            // Switch 3 is down, call assembly version of FIR filter

            // Process samples from left channel, then right channel
            fir2(gBufferRcvPongL, COEFFS, gBufferXmtPongL, delayBufferL, BUFFSIZE/2, ORDER);
            fir2(gBufferRcvPongR, COEFFS, gBufferXmtPongR, delayBufferR, BUFFSIZE/2, ORDER);
        }
        
        if (switch3) {        
            // Switch 3 is up, audio pass thru only
            copyData(gBufferRcvPongL, gBufferXmtPongL, BUFFSIZE/2);
            copyData(gBufferRcvPongR, gBufferXmtPongR, BUFFSIZE/2);
        }
        
        // Send a LOG_printf to CCS
        LOG_printf(&logTrace,"PONG");

        // Configure the receive channel for pong input data
        addr = ((Uint32)gBufferRcvPongL) << 1;
        DMA_RSETH(hDmaRcv, DMACDSAL, addr & 0xffff);
        DMA_RSETH(hDmaRcv, DMACDSAU, (addr >> 16) & 0xffff);

        // Configure the transmit channel for pong output data
        addr = ((Uint32)gBufferXmtPongL) << 1;    
        DMA_RSETH(hDmaXmt, DMACSSAL, addr & 0xffff);
        DMA_RSETH(hDmaXmt, DMACSSAU, (addr >> 16) & 0xffff);
    }
}


/*
 *  blinkLED() - Periodic thread (PRD) that toggles LED #0 every 500ms if 
 *               DIP switch #0 is depressed.  The thread is configured
 *               in the DSP/BIOS configuration tool under Scheduling -->
 *               PRD --> PRD_blinkLed.  The period is set there at 500
 *               ticks, with each tick corresponding to 1ms in real
 *               time.
 */
void blinkLED(void)
{
    // Toggle LED #0 if DIP switch #0 is off (depressed)
    if (!DSK5510_DIP_get(0))
        DSK5510_LED_toggle(0);
}


/*
 *  load() - PRD that simulates a 20-25% dummy load on a 200MHz 5510 if
 *           DIP switch #1 is depressed.  The thread is configured in
 *           the DSP/BIOS configuration tool under Scheduling --> PRD
 *           PRD_load.  The period is set there at 10 ticks, which each tick
 *           corresponding to 1ms in real time.
 */
void load(void)
{
    volatile Uint32 i;  

    if (!DSK5510_DIP_get(1))
        for (i = 0; i < 30000; i++);
}


/* ---------------------- Interrupt Service Routines -------------------- */
/*
 *  dmaHwi() - Interrupt service routine for the DMA transfer.  It is triggered
 *             when a DMA complete receive frame has been transferred.   The
 *             hwiDma ISR is inserted into the interrupt vector table at
 *             compile time through a setting in the DSP/BIOS configuration
 *             under Scheduling --> HWI --> HWI_INT9.  dmaHwi uses the DSP/BIOS
 *             Dispatcher to save register state and make sure the ISR
 *             co-exists with other DSP/BIOS functions.
 */
void dmaHwi(void)
{
    // Ping-pong state.  Initialized to PING initially but declared static so
    // contents are preserved as dmaHwi() is called repeatedly like a global.
    static Int16 pingOrPong = PING;

    // Determine if current state is PING or PONG
    if (pingOrPong == PING) {
        // Post SWI thread to process PING data
        SWI_or(&processBufferSwi, PING);

        // Set new state to PONG
        pingOrPong = PONG;
    }
    else {
        // Post SWI thread to process PONG data
        SWI_or(&processBufferSwi, PONG);

        // Set new state to PING
        pingOrPong = PING;
    }

    // Read the DMA status register to clear it so new interrupts will be seen
    DMA_RGETH(hDmaRcv, DMACSR);
}


/* --------------------------- main() function -------------------------- */
/*
 *  main() - The main user task.  Performs application initialization and
 *           starts the data transfer.
 */
void main()
{
    volatile DSK5510_AIC23_CodecHandle hCodec;

    // Initialize the board support library, must be called first
    DSK5510_init();
    
    // Initialize LEDs and DIP switches
    DSK5510_LED_init();
    DSK5510_DIP_init();

    // Clear buffers
    memset((void *)gBufferRcvPingL, 0, BUFFSIZE * 4);
    
    // Start the codec
    hCodec = DSK5510_AIC23_openCodec(0, &config);

    // Start the DMA controller for the receive transfer
    initDma();

    // Set up interrupts
    initIrq();

    // Start the DMA
    DMA_start(hDmaRcv);
    DMA_start(hDmaXmt);
}
