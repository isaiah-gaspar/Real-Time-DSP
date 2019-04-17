/** ###################################################################
**     Filename  : rtimage.C
**     Project   : rtimage
**     Processor : 56858
**     Version   : Driver 01.04
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 3/15/2004, 11:41 PM
**     Abstract  :
**         Main module. 
**         Here is to be placed user's code.
**     Settings  :
**     Contents  :
**         No public methods
**
**     (c) Copyright UNIS, spol. s r.o. 1997-2003
**     UNIS, spol. s r.o.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/

/*



Hybrid


*/


/* Including used modules for compiling procedure */
#include "Cpu.h"
#include "Events.h"
#include "serial.h"

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#define N (96*128)              // 96 lines of 128 pixels = 12,288 pixels total
#define size 256                // 256 gray-scale brightness levels

void clear_histograms();
void input_output_images();
void calculate_histogram();
void map_levels();
void transform_image();

byte image_org[N];
byte image_equ[N];
int hist_org[size] = {0};
byte mapped_levels[size];

asm void Acalculate_histogram(int[], byte[], unsigned int);
asm void Amap_levels(int[], byte[], unsigned int, unsigned int);
asm void Atransform_image(byte[], byte[], byte[], unsigned int);



void main(void)
{
    /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
    PE_low_level_init();
    /*** End of Processor Expert internal initialization.                    ***/

    for(;;)
    {
        clear_histograms();
 
        input_output_images();

        //calculate_histogram();
        Acalculate_histogram(hist_org, image_org, N);

        //map_levels();
		Amap_levels(hist_org, mapped_levels, N, size);

        Atransform_image(image_org,mapped_levels,image_equ, N);


    }
}


/*

"clear_histograms" is a function that sets each element of the globally declared integer
array to zero.  A function value is not returned.

Input values:   The elements of the globally declared integer array hist_org.
Output values:  The elements of the integer array are set to zero.

*/

void clear_histograms()
{
    int i;
    for(i=0; i<size; i++)               // do this for each grayscale level
    {
      // initialize histogram values to zero
      hist_org[i] = 0;
      
    }
}






/*

"input_output_images" is a function that receives asynchronous SCI characters and saves
them  sequentially to the global byte array, image_org.  At the same time, this function
transmits characters sequentially stored in another byte array, image_equ.  A complete
frame of video pixels is received and transmitted.  The number of pixels in a frame is
globally defined as "N".  A function value is not returned.

Input values:   The elements of the globally declared byte array image_equ are
                sequentially transmitted serially via the SCI.
Output values:  The characters serially received via the SCI are sequentially stored in
                the globally declared byte array image_org.

*/

void input_output_images()
{ // no need to edit this function. Use as is.
    word r;
    int i;
    for(i=0; i<N;)                      // do this for each pixel in one frame
    {
        r = serial_GetCharsInRxBuf();   // check for a received SCI character

        if (r==1)
        {

            // get the received serial character and store this pixel in the image_org array
            serial_RecvChar(&image_org[i]);
            
            // send a character stored as a pixel in the image_equ array via the SCI
            serial_SendChar(image_equ[i]);

            // increment the for-loop counter here if successful
            i++;
        }
    }
}


/*

"calculate_histogram" is a function that counts the frequency of the occurance of each
video level stored in the globally declared byte array, image_org.  The number of pixels
processed from the array is set by the globally defined constant, N.  A function value
is not returned.

Input values:   The elements of the globally declared byte array, image_org.
Output values:  The elements of the globally declared integer array, hist_org.

*/
/*
void calculate_histogram()
{

	int temp;
    int i;
    for(i=0; i<N; i++)                  // do this for each pixel in a frame
    {
    
		//insert code here
		temp = image_org[i];
		hist_org[temp]++;
    }
}
*/

asm void Acalculate_histogram(int*, byte*, unsigned int)
{
	// hist_org -> r2, image_org -> r3, N -> Y0, size -> y1
	
	DO Y0, ENDLOOP
	
	moveu.bp X:(R3)+, A
	moveu.w A,R1
	
	ADDA R2, R1
	
	INC.W X:(R1)
	
	
ENDLOOP:

	
	RTS
	
	
}


/*

"map_levels" is a function that maps 8-bit video grayscale levels represented by an array
index to a new value stored as an element in the array, mapped_levels.  This function
calls another function, cdf().  A function value is not returned.

Input values:   The elements of the globally declared byte array, mapped_levels.
Output values:  The elements of the globally declared byte array, mapped_levels.

*/
/*
void map_levels()
{
    int w;
    int j;
    float sum=0;
    for(w=0; w<size; w++)
    {

    sum = hist_org[w]+sum;
    
    //compute cumulative sum and store in 'sum'
    mapped_levels[w]= (byte)(255*sum/N);        //get the new intensity using Histogram equalization, it is 255*sum/N
    
    
    }
        
    
}
*/

asm void Amap_levels(int*, byte*, unsigned int, unsigned int)
{
	
	// hist_org -> r2, mapped_levels -> r3, N -> Y0, size -> y1
	
	//Move n and size out of Y0 and Y1 to avoid overwriting in divion
	
	move.w Y0, X0 // X0 = N
	
	move.w Y1, C // C = size
	
	clr A
	
	DO C1, ENDLOOP
	
	move.w X:(R2)+,B
	add b,A
	move.w #255, B
	IMPY.L B1,a1,Y
	ASL Y                   // shift left of dividend required for integer division
    BFCLR #$0001,SR         // clear carry bit: required for first DIV instruction
    REP #16                 // do sixteen times for all 16 bits
    DIV X0,Y 
	move.bp Y0,X:(r3)+
	
	
	
	
	
ENDLOOP:
	
	rts
}


/*

"transform_image" is a function that transforms the video brightness levels of a
globally declared array, image_org, to an output array, image_equ.  A third globally
declared array, mapped_levels, is used to make the transformation by mapping a byte
address to a byte value.  A function value is not returned.

Input values:   The elements of the globally declared byte array, image_org.
                The elements of the globally declared byte array, mapped_levels.
Output values:  The elements of the globally declared byte array, image_equ.

*/


/*  int i;
    for(i=0; i<N; i++)
    {
        //inset code here
        image_equ[i] = mapped_levels[image_org[i]];
    }
*/  

asm void Atransform_image(byte*, byte*, byte*, unsigned int)
{
  
    
	// image_org -> r2, mapped_levels -> r3, image_equ -> r4, N -> Y0    
    
	DO Y0, ENDLOOP
	
	moveu.bp X:(R2)+,B
	moveu.w B, R1
	ADDA R3, R1
	
	moveu.bp X:(R1),Y1
	move.bp Y1,X:(R4)+

	
ENDLOOP:

	
	RTS
    
    
    
    
    
}


/* END rtimage */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 03.36 for 
**     the Motorola 56800 series of microcontrollers.
**
** ###################################################################
*/
