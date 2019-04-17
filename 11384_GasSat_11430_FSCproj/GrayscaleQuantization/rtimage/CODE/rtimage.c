/** ###################################################################
**     Filename  : rtimage.C
**     Project   : rtimage
**     Processor : 56858
**     Version   : Driver 01.11
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 3/8/2007, 8:50 PM
**     Abstract  :
**         Main module.
**         Here is to be placed user's code.
**     Settings  :
**     Contents  :
**         No public methods
**
**     (c) Copyright UNIS, spol. s r.o. 1997-2006
**     UNIS, spol. s r.o.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/
/* MODULE rtimage */

/* Including used modules for compiling procedure */
#include "Cpu.h"
#include "Events.h"
#include "serial.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"



byte inp_buffer[256];
byte  temp;
byte thold1 = 80;
byte thold2 = 150;

byte ShiftFactor = 4;

byte A = 75;
byte B = 50;

word r;
byte contrast(byte,byte,byte );
byte binary (byte, byte );
byte brightness(byte inp,byte A,byte B);

byte GrayTransformation1_C(byte);
byte GrayTransformation2_C(byte, byte B, byte A);
byte GrayTransformation3_C(byte, byte B, byte A);

asm byte Transformation1_Hybrid(byte temp, byte thold1);

asm byte Transformation2_Hybrid(byte temp, byte thold1,byte thold2);
asm byte Quantize(byte temp, byte ShiftFactor);

asm byte GrayTransformation1_Hybrid(byte temp);
asm byte GrayTransformation3_Hybrid(byte temp, byte B,byte A);


void main(void)
{
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  for(;;) {

r = serial_GetCharsInRxBuf();

			if (r==1)

			{
				
				
					
				serial_RecvChar(&temp);

				//temp = Transformation1_Hybrid(temp, thold1);
				//temp = Transformation2_Hybrid(temp, thold1, thold2);				
				//temp = Quantize(temp, ShiftFactor);
				//temp = GrayTransformation1_Hybrid(temp);
				//temp = GrayTransformation3_Hybrid(temp,B , A);
				//temp = GrayTransformation1_C(temp);
				//temp = GrayTransformation2_C(temp,B , A);
				temp = GrayTransformation3_C(temp,B , A);

				serial_SendChar(temp);
			}
  		}
}

//////////////////////////////////////////////////////////////////////
// Gray Level Transformation C

byte GrayTransformation1_C(byte temp)
{

	
    
  	temp=255-temp;
  	return temp;
  
}



byte GrayTransformation2_C(byte temp,byte B, byte A)
{

	
    if(temp<=B)
  	temp= ((float)A/B)*temp;
    else if(B<=temp && temp<=255-B)
    temp = ((float)((2*A)-255)/((2*B)-255))*(temp-B)+A;
    else
    temp = ((float)A/B)*(temp-(255-B))+(255-A);
  	return temp;
  
}


byte GrayTransformation3_C(byte temp,byte B, byte A)
{

	
    if(temp<=B)
  	temp= ((float)A/B)*temp;
    else
    temp = ((float)(A-255)/(B-255))*(temp-B)+A;
   
  	return temp;
  
}



//////////////////////////////////////////////////////////////////////
// Gray Level Transformation Hybrid

asm byte GrayTransformation1_Hybrid(byte temp)
{
		//enter code
		// TEMP -> Y0, THOLD -> Y1

		//moveu.b Y0, Y1
		
		MOVE.W #255, Y1 // Moving 255 into y1
		
		
		SUB Y0, Y1 // Y1 - Y0 = Y1
		
		MOVE.W Y1, Y0 // movine y1 into y0

		RTS

}





asm byte GrayTransformation3_Hybrid(byte temp, byte B, byte A)
{
		//enter code
		// TEMP -> Y0, 'B' -> Y1, 'A' -> A

		CMP Y1, Y0 //Y0 <= Y1
		
		BLE ONE	// 
							//'B'<=X<=255
		MOVE.W Y1,B1            // move the B value into B1
  		SUB B,Y0                // subtract the 16-bit B value (in B1) from the 16-bit temp value (in Y0) -> input-B
  		MOVE.W #$FF,Y1          // load the 16-bit 255 value into Y1
 		SUB A,Y1                // subtract the 16-bit A value (in A1) from the 16-bit 255 value (in Y1) -> 255-A

   		 // The next instruction performs a 16-bit x 16-bit signed multiplication to a 32-bit signed integer result

	    IMPY.L Y0,Y1,Y          // multiply the 16-bit temp-B value (in Y0) by the 16-bit 255-2A value (in Y0), result in 32-bit Y -> 

	    MOVE.W #$FF,X0          // load a 255 value into X0 
	    SUB B,X0                // create a 255-B quotient (in X0) -> 255-B

    	// The next 4 instructions perform a 32/16-bit unsigned integer division (the dividend and divisor are both positive)
    
	    ASL Y                   // shift left of dividend required for integer division
	    BFCLR #$0001,SR         // clear carry bit: required for first DIV instruction
	    REP #16                 // do sixteen times for all 16 bits
	    DIV X0,Y                // divide Y1:Y0 by X0, the result is in 32-bit Y

	    ADD A,Y0                // add the 16-bit A value (in A1) to the 16-bit value in Y0
    
		
		JMP END
		
ONE:						//0<=X<='B'
		
		MOVE.W Y1, X0
		IMPY.L Y0, A1, Y
		
		ASL Y                   // shift left of dividend required for integer division
    	BFCLR #$0001,SR         // clear carry bit: required for first DIV instruction
   		REP #16                 // do sixteen times for all 16 bits
   		DIV X0,Y                // divide Y1:Y0 by X0, the result is in 32-bit Y

		
		
END:
		RTS

}



//////////////////////////////////////////////////////////////////////
// Quantization

asm byte Quantize(byte temp, byte ShiftFactor)
{
		// TEMP -> Y0, ShiftFactor -> Y1

	DO Y1, END
	
	ASR Y0
	
END:
	
	rts
}


//////////////////////////////////////////////////////////////////////
// Threshold Transformation

asm byte Transformation1_Hybrid(byte temp, byte thold1)
{
		//enter code
		// TEMP -> Y0, THOLD -> Y1

				CMP Y0, Y1
		
		BLE ONE
		
		MOVE.W #0, Y0
		
		JMP END
		
ONE:
		MOVE.W #255, Y0
		
END:
		RTS

}


asm byte Transformation2_Hybrid(byte temp, byte thold1, byte thold2 )
{
		//enter code
		// TEMP -> Y0, THOLD1 -> Y1, THOLD2 -> A
		
		
		CMP Y1, Y0
		
		BLE ZERO
		
		CMP A, Y0
		
		BGE ZERO
		
		MOVE.W #255, Y0
		
		JMP END
		
ZERO:
		MOVE.W #0, Y0
		
END:
		RTS
		
}




/* END rtimage */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.98 [03.79]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
