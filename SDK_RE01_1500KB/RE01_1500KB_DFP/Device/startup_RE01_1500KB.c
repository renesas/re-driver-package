/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2018-2019 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : startup_RE01_1500KB.c
 * Version      : 1.30
 * Description  : CMSIS Cortex-M# Core Device Startup File for Device <RE01_1500KB>
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 21.08.2018 0.50     First release
 *         : 25.12.2018 0.60     Adapted to GSCE coding rules.
 *         : 23.07.2019 1.00     Change the product name to RE Family
 *                                - Update comments
 *         : 24.11.2020 1.30     Added the Heap area definition.
 **********************************************************************************************************************/
/******************************************************************************
 *
 * This file contains an interrupt vector for Cortex-M written in C.
 * The actual interrupt functions must be provided by the application developer.
 *
 * Copyright 2007-2017 IAR Systems AB.
 *
 * $Revision: 112610 $
 *
 ******************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup grp_cmsis_core
 * @{
 **********************************************************************************************************************/

/*------------------------------------------------------------------------------
 * Include
 *----------------------------------------------------------------------------*/
#include <stdint.h>

/*------------------------------------------------------------------------------
 * Macro
 *----------------------------------------------------------------------------*/
#if defined(__ICCARM__)                  /* IAR Compiler */
#define NON_VOLATILE  __root             /* @suppress("Internal macro naming") */ /* @suppress("Macro expansion") */
#elif defined(__GNUC__)                  /* GCC Compiler */
#define NON_VOLATILE                     /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#else
#error "That compiler is not supported in this release."
#endif

/*------------------------------------------------------------------------------
 * Pragma
 *----------------------------------------------------------------------------*/
#if defined(__ICCARM__)                  /* IAR Compiler */
#pragma language=extended
#pragma segment="CSTACK"
#endif

/*------------------------------------------------------------------------------
 * Imported global variables and functions (from other files)
 *----------------------------------------------------------------------------*/
extern void Reset_Handler( void );       /* @suppress("Source line ordering") */ /* @suppress("Internal function declaration") */

extern void NMI_Handler( void );         /* @suppress("Internal function declaration") */
extern void HardFault_Handler( void );   /* @suppress("Internal function declaration") */
/* extern void MemManage_Handler( void );  */ /* Not in M0+ */ /* @suppress("Block comment") */ /* @suppress("Non-block comment") */
/* extern void BusFault_Handler( void );   */ /* Not in M0+ */
/* extern void UsageFault_Handler( void ); */ /* Not in M0+ */
extern void SVC_Handler( void );         /* @suppress("Internal function declaration") */
/* extern void DebugMon_Handler( void );   */ /* Not in M0+ */ /* @suppress("Block comment") */ /* @suppress("Non-block comment") */
extern void PendSV_Handler( void );      /* @suppress("Internal function declaration") */
extern void SysTick_Handler( void );     /* @suppress("Internal function declaration") */

extern void IEL0_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL1_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL2_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL3_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL4_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL5_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL6_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL7_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL8_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL9_IRQHandler( void );     /* @suppress("Internal function declaration") */
extern void IEL10_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL11_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL12_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL13_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL14_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL15_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL16_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL17_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL18_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL19_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL20_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL21_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL22_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL23_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL24_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL25_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL26_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL27_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL28_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL29_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL30_IRQHandler( void );    /* @suppress("Internal function declaration") */
extern void IEL31_IRQHandler( void );    /* @suppress("Internal function declaration") */

extern void SystemInit (void);           /* @suppress("Internal function declaration") */
extern void __iar_program_start (void);  /* @suppress("Internal function declaration") */

#if defined(__GNUC__)                    /* GCC Compiler */
extern uint32_t __etext;                 /* @suppress("Source line ordering") */
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

extern int main();                       /* @suppress("Non-portable variable type") */
#endif

/*------------------------------------------------------------------------------
 * Typedef
 *----------------------------------------------------------------------------*/
typedef void( *intfunc )( void );        /* @suppress("Source line ordering") */
typedef union { intfunc __fun; void * __ptr; } intvec_elem; /* @suppress("Data type member naming") */ /* @suppress("Data type naming") */
#if defined(__GNUC__)                    /* GCC Compiler */
#define SYSTEM_CFG_STACK_SIZE  (0x400)   /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SYSTEM_CFG_HEAP_SIZE  (0x400)
NON_VOLATILE uint8_t g_main_stack[SYSTEM_CFG_STACK_SIZE] __attribute__ ((section(".stack"))) __attribute__ ((aligned (8))) = {0U};
NON_VOLATILE uint8_t g_main_heap[SYSTEM_CFG_HEAP_SIZE] __attribute__ ((section(".heap"))) __attribute__ ((aligned (8))) = {0U};
#else
#endif
/*------------------------------------------------------------------------------
 * Vector table
 *----------------------------------------------------------------------------*/
const intvec_elem __vector_table[] __attribute__ ((section(".intvec"))) = /*  Exception no.     */
{
#if defined(__ICCARM__)              /* IAR Compiler */
    { .__ptr = __sfe( "CSTACK" ) },  /*  0 - Stack pointer */
#elif defined(__GNUC__)              /* GCC Compiler */
    { .__ptr = (&g_main_stack[0] + SYSTEM_CFG_STACK_SIZE) },              /* @suppress("Operand parentheses") */
#endif
    Reset_Handler,                   /*  1 - Reset vector  */
    /* Cortex-M0+ */                 /* @suppress("Block comment") */ /* @suppress("Non-block comment") */
    NMI_Handler,                     /*  2 - NMI           */
    HardFault_Handler,               /*  3 - HardFault     */
    0,                               /*  4 - Reserved      */
    0,                               /*  5 - Reserved      */
    0,                               /*  6 - Reserved      */
    0,                               /*  7 - Reserved      */
    0,                               /*  8 - Reserved      */
    0,                               /*  9 - Reserved      */
    0,                               /* 10 - Reserved      */
    SVC_Handler,                     /* 11 - SVCall        */
    0,                               /* 12 - Reserved      */
    0,                               /* 13 - Reserved      */
    PendSV_Handler,                  /* 14 - PnedSV        */
    SysTick_Handler,                 /* 15 - SysTick       */
    /* ICU interrupt */              /* @suppress("Block comment") */ /* @suppress("Non-block comment") */
    IEL0_IRQHandler,                 /* 16 - Interrupt  0  */
    IEL1_IRQHandler,                 /* 17 - Interrupt  1  */
    IEL2_IRQHandler,                 /* 18 - Interrupt  2  */
    IEL3_IRQHandler,                 /* 19 - Interrupt  3  */
    IEL4_IRQHandler,                 /* 20 - Interrupt  4  */
    IEL5_IRQHandler,                 /* 21 - Interrupt  5  */
    IEL6_IRQHandler,                 /* 22 - Interrupt  6  */
    IEL7_IRQHandler,                 /* 23 - Interrupt  7  */
    IEL8_IRQHandler,                 /* 24 - Interrupt  8  */
    IEL9_IRQHandler,                 /* 25 - Interrupt  9  */
    IEL10_IRQHandler,                /* 26 - Interrupt 10  */
    IEL11_IRQHandler,                /* 27 - Interrupt 11  */
    IEL12_IRQHandler,                /* 28 - Interrupt 12  */
    IEL13_IRQHandler,                /* 29 - Interrupt 13  */
    IEL14_IRQHandler,                /* 30 - Interrupt 14  */
    IEL15_IRQHandler,                /* 31 - Interrupt 15  */
    IEL16_IRQHandler,                /* 32 - Interrupt 16  */
    IEL17_IRQHandler,                /* 33 - Interrupt 17  */
    IEL18_IRQHandler,                /* 34 - Interrupt 18  */
    IEL19_IRQHandler,                /* 35 - Interrupt 19  */
    IEL20_IRQHandler,                /* 36 - Interrupt 20  */
    IEL21_IRQHandler,                /* 37 - Interrupt 21  */
    IEL22_IRQHandler,                /* 38 - Interrupt 22  */
    IEL23_IRQHandler,                /* 39 - Interrupt 23  */
    IEL24_IRQHandler,                /* 40 - Interrupt 24  */
    IEL25_IRQHandler,                /* 41 - Interrupt 25  */
    IEL26_IRQHandler,                /* 42 - Interrupt 26  */
    IEL27_IRQHandler,                /* 43 - Interrupt 27  */
    IEL28_IRQHandler,                /* 44 - Interrupt 28  */
    IEL29_IRQHandler,                /* 45 - Interrupt 29  */
    IEL30_IRQHandler,                /* 46 - Interrupt 30  */
    IEL31_IRQHandler                 /* 47 - Interrupt 31  */
};

#if defined(__ICCARM__)               /* IAR Compiler */
#pragma call_graph_root = "interrupt"
__weak void NMI_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void HardFault_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void MemManage_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void BusFault_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void UsageFault_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void SVC_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void DebugMon_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void PendSV_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void SysTick_Handler( void ) { while (1) {} }

#pragma call_graph_root = "interrupt"
__weak void IEL0_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL1_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL2_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL3_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL4_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL5_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL6_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL7_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL8_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL9_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL10_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL11_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL12_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL13_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL14_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL15_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL16_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL17_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL18_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL19_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL20_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL21_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL22_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL23_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL24_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL25_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL26_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL27_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL28_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL29_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL30_IRQHandler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IEL31_IRQHandler( void ) { while (1) {} }

#pragma call_graph_root = "interrupt"
/*------------------------------------------------------------------------------
 * Forced placement of unreferenced functions and data
 *----------------------------------------------------------------------------*/
#pragma required=__vector_table

#elif defined(__GNUC__)                  /* GCC Compiler */

/* Function Name : Default_Handler */
/*********************************************************************************************************************//**
 * @brief  Default exception handler.
 * @param[in]  None
 * @param[out]  None
 * @retval None
 ***********************************************************************************************************************/
void Default_Handler (void)              /* @suppress("Source line ordering") */ /* @suppress("Function declaration") */ /* @suppress("Non-API function naming") */
{
    while (1)
    {
        ;                                /* loop */
    }
}  /* End of function Default_Handler() */

void NMI_Handler( void )        __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void HardFault_Handler( void )  __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void MemManage_Handler( void )  __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void BusFault_Handler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void UsageFault_Handler( void ) __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void SVC_Handler( void )        __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void DebugMon_Handler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void PendSV_Handler( void )     __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void SysTick_Handler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */

void IEL0_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL1_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL2_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL3_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL4_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL5_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL6_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL7_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL8_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL9_IRQHandler( void )    __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL10_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL11_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL12_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL13_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL14_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL15_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL16_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL17_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL18_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL19_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL20_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL21_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL22_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL23_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL24_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL25_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL26_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL27_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL28_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL29_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL30_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */
void IEL31_IRQHandler( void )   __attribute__ ((weak, alias("Default_Handler"))); /* @suppress("Internal function declaration") */

#else                                    /* @suppress("Conditional directive comment") */
#endif

/***************************************************************************//**
* @brief  Processing after reset.
* @param  None
* @retval None
*******************************************************************************/
void Reset_Handler( void )               /* @suppress("Function description comment") */ /* @suppress("Non-static function declaration location") */ /* @suppress("Non-API function naming") */
{
    SystemInit();

#if defined(__ICCARM__)                  /* IAR Compiler */
    __iar_program_start();
#elif defined(__GNUC__)                  /* GCC Compiler */
    uint32_t  bytes;
    uint8_t * pstart  = (uint8_t *)&__bss_start__;  /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    uint8_t * psource = (uint8_t *)&__etext;        /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    uint8_t * pdest   = (uint8_t *)&__data_start__; /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */

    /* Zero out BSS */
    bytes = (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__;
    while (bytes > (uint32_t)0)          /* @suppress("Cast comment") */ /* @suppress("while statement braces") */
    {
        bytes--;
        pstart[bytes] = (uint32_t)0;     /* @suppress("Cast comment") */
    }

    /* Copy initialized RAM data from ROM to RAM. */
    bytes = (uint32_t)&__data_end__ - (uint32_t)&__data_start__;
    for (uint32_t index = 0U; index < bytes; index++, pdest++, psource++)
    {
        *pdest = *psource;
    }

    main();
#else
#endif
} /* End of function Reset_Handler() */

/*******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_core)
 **********************************************************************************************************************/

/* End of file (startup_RE01_1500KB.c) */
