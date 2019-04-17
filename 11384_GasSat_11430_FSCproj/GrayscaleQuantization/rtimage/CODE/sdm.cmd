## ###################################################################
##
##     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
##
##     Filename  : sdm.cmd
##
##     Project   : rtimage
##
##     Processor : 56858
##
##     Compiler  : Metrowerks DSP C Compiler
##
##     Date/Time : 3/13/2019, 7:44 PM
##
##     Abstract  :
##
##     This file is used by the linker. It describes files to be linked,
##     memory ranges, stack size, etc. For detailed description about linker
##     command files see CodeWarrior documentation. This file is generated by default.
##     You can switch off generation by setting the property "Generate linker file = no"
##     in the "Build options" tab of the CPU bean and then modify this file as needed.
##
##     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
##     
##     http      : www.freescale.com
##     mail      : support@freescale.com
##
## ###################################################################

MEMORY {
        # I/O registers area for on-chip peripherals
        .x_Peripherals (RW)   : ORIGIN = 0x1FFE20, LENGTH = 0

        # List of all sections specified in the "Build options" tab
        .p_Interrupts  (RWX) : ORIGIN = 0x00000000, LENGTH = 0x00000100
        .p_Code  (RWX) : ORIGIN = 0x00000100, LENGTH = 0x00009F00
        .x_Data  (RW) : ORIGIN = 0x00000001, LENGTH = 0x00005FFF

        #Other memory segments
        .p_internal_RAM  (RWX) : ORIGIN = 0x00000000, LENGTH = 0xA000

}

KEEP_SECTION { interrupt_vectors.text }

SECTIONS {

        .interrupt_vectors :
        {
          F_vector_addr = .;
          # interrupt vectors
          * (interrupt_vectors.text)
        } > .p_Interrupts

        .ApplicationCode :
        {

              F_Pcode_start_addr = .;

              # .text sections
              OBJECT (F_EntryPoint, Cpu.c) # The function _EntryPoint have to be placed at the beginning of the code
                                       # section for proper functionality of the serial bootloader.
              * (.text)
              * (rtlib.text)
              * (startup.text)
              * (fp_engine.text)
              * (ll_engine.text)
              * (user.text)
              * (.data.pmem)

              F_Pcode_end_addr = .;


        } > .p_Code

        .ApplicationData :
        {
              # .data sections
              * (.const.data.char)  # used if "Emit Separate Char Data Section" enabled
              * (.const.data)
              * (fp_state.data)
              * (rtlib.data)
              * (.data.char)        # used if "Emit Separate Char Data Section" enabled
              * (.data)

              # .bss sections
              * (rtlib.bss.lo)
              * (rtlib.bss)
              . = ALIGN(4);
              F_Xbss_start_addr = .;
              _START_BSS = .;
              * (.bss.char)         # used if "Emit Separate Char Data Section" enabled
              * (.bss)
              . = ALIGN(2);         # used to ensure proper functionality of the zeroBSS hardware loop utility
              _END_BSS   = .;
              F_Xbss_length = _END_BSS - _START_BSS;

              /* Setup the HEAP address */
              . = ALIGN(4);
              _HEAP_ADDR = .;
              _HEAP_SIZE = 0x00000100;
              _HEAP_END = _HEAP_ADDR + _HEAP_SIZE;
              . = _HEAP_END;

              /* SETUP the STACK address */
              _min_stack_size = 0x00000200;
              _stack_addr = _HEAP_END;
              _stack_end  = _stack_addr + _min_stack_size;
              . = _stack_end;

              /* EXPORT HEAP and STACK runtime to libraries */
              F_heap_addr   = _HEAP_ADDR;
              F_heap_end    = _HEAP_END;
              F_Lstack_addr = _HEAP_END;
              F_StackAddr = _HEAP_END;
              F_StackEndAddr = _stack_end - 1;

              # runtime code __init_sections uses these globals:
              F_start_bss   = _START_BSS;
              F_end_bss     = _END_BSS;

              __DATA_END=.;

        } > .x_Data

}