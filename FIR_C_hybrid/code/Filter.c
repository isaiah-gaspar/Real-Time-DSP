/** ###################################################################
**     Filename  : Filter_C_fixed.C
**     Project   : Filter_C_fixed
**     Processor : 56858
**     Version   : Driver 01.05
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 12/17/03, 8:55 PM
**     Abstract  :
**         Main module.
**         Here is to be placed user's code.
**     Settings  :
**     Contents  :
**         No public methods
**
**     Freescale Inc.
**     (c) Copyright 2003 Freescale, Inc.
**     ALL RIGHTS RESERVED.
**
**     (c) Copyright UNIS, spol. s r.o. 1997-2003
**     UNIS, spol. s r.o.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/
/* MODULE Filter_C_fixed */



/* Including used modules for compilling procedure */
#include "Cpu.h"
#include "Events.h"
#include "LED1.h"
#include "Inhr1.h"
#include "Codec.h"
#include "Inhr2.h"
#include "Inhr3.h"
#include "Inhr4.h"
#include "Inhr5.h"
#include "Inhr6.h"
#include "EInt1.h"
#include "EInt2.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#define FIFO_SIZE   32
#define NUM_SAMPLES 2
#define NUM_BYTES   (2*NUM_SAMPLES)
#define SIZE 23
int j, index;
signed long y;
int sample_array[SIZE]={0};

volatile UWord16 LoopCount = 0;
volatile Word16 pSamples[FIFO_SIZE];

// Filter coefficients 


//Coeff for 2000Hz Stopband
int h[SIZE]={434,143,-738,-1193,-153,1287,682,-1940,-2591,2233,10089,13988,
   10089,2233,-2591,-1940,682,1287,-153,-1193,-738,143,434};  
  

//Coeff for 4000Hz Stopband
//int h[SIZE]={0,	-1,	6,	-31,	113,	-331,	787,	-1559,	2617,	-3764,	4668,	27754,	4668,	-3764,	2617,	-1559,	787,	-331,	113,	-31,	6,	-1,	0};    
   

//Declare Filter Flag


int filterFlag;
int Hfilter(int[],int[],const);

/*******************************************************************************
*
* Module:
*    main()
*
* Description:
*    This function is the main function of the Codec demo.
*
*******************************************************************************/
void main(void)
{
  Word16 samples;
  Word16 i;
  word   Rcv, Snd;


  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Set up LEDs */
  
  LED1_On();

  /* The executive loop just toggles the LED depending on the number of
	 Samples received from the Codec.  Meanwhile, all loopback is done
	 in the ISRs. */
	 
  while(1)
  {
    samples = 0;
  
	while (samples < NUM_SAMPLES) {
      Codec_Read((void*)&pSamples[samples], NUM_SAMPLES-samples, &Rcv);
      samples += Rcv;
    }

	samples = 0;
		for (j=SIZE-1; j>0; j--)	            
		{
			sample_array[j]=sample_array[j-1];		
		}
	
		sample_array[0] = pSamples[samples];   
		
		 
//if start
	//if(filterFlag == 1)
	//{		
			
			//Start change to ASM with Hfilter() \/ \/ \/
			/*y = 0;											
			
			for (j=0; j<SIZE; j++)
			{
				y += ((long)sample_array[j]*(long)h[j]);				 
			}*/
			 			 
			//pSamples[samples] = (int) (y>>15);
			//End change to ASM					 /\ /\ /\
			
			
			pSamples[samples] = Hfilter(sample_array, h, SIZE);
	//}

					
//if stop


	while (samples < NUM_SAMPLES) {
	  Codec_Write((void*)&pSamples[samples], NUM_SAMPLES-samples, &Snd);
	  samples += Snd;
	}

    LoopCount++;

    if (LoopCount > 1500) {
      LoopCount = 0;
      LED1_Toggle();
    }
  }
}

asm int Hfilter(int a[], int b[], const length)

{
	
	// a -> r2, b -> r3, length -> Y0	
	
	//	DO	Y0, END_LOOP
	//
	//	MOVE.W	X:(R2)+,C1
	//	MOVE.W	X:(R3)+,Y1
	
//		IMPY.W	C1,Y1,A
//END_LOOP:

		MOVE.W	Y0,	Y1
		MOVEU.W	R2,	R0
		
		CLR		A
		MOVE.W	X:(R0)+,Y0
		MOVE.W	X:(R3)+,X0
		REP		Y1
		MAC		Y0,X0,A	X:(R0)+,Y0	X:(R3)+,X0
		
		MOVE.W	A,	Y0
	
		RTS
}


/* END Filter_C_fixed */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 03.39 for
**     the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
