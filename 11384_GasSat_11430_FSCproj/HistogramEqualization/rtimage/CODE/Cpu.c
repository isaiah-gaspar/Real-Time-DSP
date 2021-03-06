/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : Cpu.C
**     Project   : rtimage
**     Processor : 56858
**     Component : 56858
**     Version   : Component 01.023, Driver 02.08, CPU db: 2.87.105
**     Datasheet : DSP56858/D Rev. 3.0, DSP5685XUM/D Rev. 2.0
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 2/27/2019, 6:34 PM
**     Abstract  :
**
**     Settings  :
**
**     Contents  :
**         EnableInt   - void Cpu_EnableInt(void);
**         DisableInt  - void Cpu_DisableInt(void);
**         SetWaitMode - void Cpu_SetWaitMode(void);
**         SetStopMode - void Cpu_SetStopMode(void);
**         SetDataMode - void Cpu_SetDataMode(byte Mode);
**
**     (c) Freescale Semiconductor
**     2004 All Rights Reserved
**
**     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

/* MODULE Cpu. */
#include "serial.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Events.h"
#include "Cpu.h"


/* Global variables */
volatile word SR_lock = 0;             /* Lock */
volatile word SR_reg;                  /* Current value of the SR register */
/*
** ===================================================================
**     Method      :  Cpu_Interrupt (component 56858)
**
**     Description :
**         The method services unhandled interrupt vectors.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#pragma interrupt alignsp
void Cpu_Interrupt(void)
{
  asm(DEBUGHLT);                       /* Halt the core and placing it in the debug processing state */
}

/*
** ===================================================================
**     Method      :  Cpu_DisableInt (component 56858)
**
**     Description :
**         Disables all maskable interrupts
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void Cpu_DisableInt(void)

**      This method is implemented as macro in the header module. **
*/

/*
** ===================================================================
**     Method      :  Cpu_EnableInt (component 56858)
**
**     Description :
**         Enables all maskable interrupts
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void Cpu_EnableInt(void)

**      This method is implemented as macro in the header module. **
*/

/*
** ===================================================================
**     Method      :  Cpu_SetStopMode (component 56858)
**
**     Description :
**         Sets low power mode - Stop mode.
**         For more information about the stop mode see this CPU
**         documentation.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void Cpu_SetStopMode(void)

**      This method is implemented as macro in the header module. **
*/

/*
** ===================================================================
**     Method      :  Cpu_SetWaitMode (component 56858)
**
**     Description :
**         Sets low power mode - Wait mode.
**         For more information about the wait mode see this CPU
**         documentation.
**         Release from wait mode: Reset or interrupt
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void Cpu_SetWaitMode(void)

**      This method is implemented as macro in the header module. **
*/

/*
** ===================================================================
**     Method      :  Cpu_SetDataMode (component 56858)
**
**     Description :
**         Sets the Data memory map to use either internal &
**         external resources or external resources only.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Mode            - Data memory access mode.
**                           Possible modes:
**                           INT_EXT_MODE - use internal & external
**                           resources
**                           EXT_MODE - use external resources only
**     Returns     : Nothing
** ===================================================================
*/
void Cpu_SetDataMode(byte Mode)
{
  if (Mode == INT_EXT_MODE) {
    asm { bfclr  #0x0008,OMR }         /* Set internal and external data memory mode */
  } else {
    asm { bfset  #0x0008,OMR }         /* Set external data memory mode */
  }
}

/*
** ===================================================================
**     Method      :  _EntryPoint (component 56858)
**
**     Description :
**         Initializes the whole system like timing and so on. At the end 
**         of this function, the C startup is invoked to initialize stack,
**         memory areas and so on.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
extern void init_56800_(void);         /* Forward declaration of external startup function declared in startup file */

/*** !!! Here you can place your own code using property "User data declarations" on the build options tab. !!! ***/

void _EntryPoint(void)
{
  #pragma constarray off

  /*** !!! Here you can place your own code before PE initialization using property "User code before PE initialization" on the build options tab. !!! ***/

  /*** ### 56858 "Cpu" init code ... ***/
  /*** PE initialization code after reset ***/
  /* System clock initialization */
  clrRegBit(CGMCR, SEL);               /* Select clock source from oscillator */
  setRegBit(CGMCR, PDN);               /* Disable PLL */
  /* CGMDB: POST=0,??=0,??=0,??=0,??=0,??=0,??=0,PLLDB=59 */
  setReg16(CGMDB, 59U);                /* Set the clock prescalers */ 
  setRegBit(CGMCR, LCKON);             /* Enable lock detector */
  clrRegBit(CGMCR, PDN);               /* Enable PLL */
  while(!getRegBit(CGMCR, LCK0)){}     /* Wait for PLL lock */
  setRegBit(CGMCR, SEL);               /* Select clock source from PLL */
  /* External bus initialization */
  /* BCR: DRV=1,BMDAR=0,??=0,??=0,BWWS=18,BRWS=28 */
  setReg16(BCR, 33372U);               /* Bus control register */ 
  /*** End of PE initialization code after reset ***/

  /*** !!! Here you can place your own code after PE initialization using property "User code after PE initialization" on the build options tab. !!! ***/

  asm(JMP init_56800_);                /* Jump to C startup code */
}

/*
** ===================================================================
**     Method      :  PE_low_level_init (component 56858)
**
**     Description :
**         Initializes beans and provides common register initialization. 
**         The method is called automatically as a part of the 
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PE_low_level_init(void)
{
  /* Common initialization of the CPU registers */
  /* GPIO_E_PER: PE|=3 */
  setReg16Bits(GPIO_E_PER, 3U);         
  /* IPR5: SCI0_RCV_IPL=2,SCI0_RERR_IPL=2,SCI0_TIDL_IPL=2,SCI0_XMIT_IPL=2 */
  clrSetReg16Bits(IPR5, 1300U, 2600U);  
  /* ### Asynchro serial "serial" init code ... */
  serial_Init();
  __EI(0);                             /* Enable interrupts of the selected priority level */
}

/* END Cpu. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.00 [04.35]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
