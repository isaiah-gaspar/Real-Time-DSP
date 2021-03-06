/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : EInt2.H
**     Project   : Filter
**     Processor : 56858
**     Component : ExtInt
**     Version   : Component 02.094, Driver 02.04, CPU db: 2.87.105
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 2/20/2019, 9:05 PM
**     Abstract  :
**         This bean "ExtInt" implements an external 
**         interrupt, its control methods and interrupt/event 
**         handling procedure.
**         The bean uses one pin which generates interrupt on 
**         selected edge.
**     Settings  :
**         Interrupt name              : INT_IRQB
**         User handling procedure     : EInt2_OnInterrupt
**
**         Used pin                    :
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       23            |  IRQB
**             ----------------------------------------------------
**
**         Port name                   : IRQ
**
**         Bit number (in port)        : 3
**         Bit mask of the port        : 8
**
**         Signal edge/level           : falling
**         Priority                    : 1
**         Pull option                 : up
**         Initial state               : Enabled
**
**         Edge register               : ICTL      [2096954]
**         Priority register           : IPR2      [2096930]
**         Enable register             : IPR2      [2096930]
**
**         Port data register          : ICTL      [2096954]
**     Contents  :
**         GetVal - bool EInt2_GetVal(void);
**
**     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __EInt2_H
#define __EInt2_H

/* MODULE EInt2. */

/*Including shared modules, which are used in the whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Events.h"
#include "Cpu.h"


#define EInt2_PIN_MASK          ((byte)8) /* Bit mask */

void EInt2_Interrupt(void);
/*
** ===================================================================
**     Method      :  EInt2_Interrupt (component ExtInt)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and invokes the beans event.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

#define EInt2_GetVal() ((bool)(getRegBits(ICTL,EInt2_PIN_MASK)))
/*
** ===================================================================
**     Method      :  EInt2_GetVal (component ExtInt)
**
**     Description :
**         Returns the actual value of the input pin of the bean.
**     Parameters  : None
**     Returns     :
**         ---             - Returned input value. Possible values:
**                           FALSE - logical "0" (Low level)
**                           TRUE - logical "1" (High level)
** ===================================================================
*/

/* END EInt2. */

#endif /* __EInt2_H*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.00 [04.35]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
