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
 * Copyright (C) 2019-2021 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_ehc.c
 * Version      : 1.40
 * Description  : CMSIS Cortex-M# Device EHC Source File for Device <RE01_1500KB>
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2019 0.60     First release
 *         : 26.03.2019 0.70     Update comments
 *                               Compatible with GCC compiler and ram execution function
 *         : 18.06.2019 0.72     Changed the section from ".intvec_init" to ".ramvect".
 *                               Added the section "__ramvect_start".
 *                               Changed code copy processing to RAM for section.
 *                               Changed the function name from "EhcInit" to "r_ehc_Startup".
 *                               Changed the function name from "EhcInit_ramfunc" to "ehc_Startup_ramfunc".
 *                               Added the "NON_VOLATILE" declaration to "ehc_Startup_ramfunc" function.
 *                               Added the "NON_VOLATILE_ATTRIBUTE" declaration to "ehc_Startup_ramfunc" function.
 *                               Moved the LVD NMI comment.
 *         : 23.07.2019 1.00     Change the product name to RE Family.
 *                                - Change the include file names
 *                                - Update comments
 *         : 25.02.2020 1.10     Renamed from "NMI_Handler" to "NMI_Handler_EHC".
 *                               Added the ehc_software_delay_loop() function.
 *                               Modified the processing of ehc_Startup_ramfunc() function.
 *                               Modified the processing of r_ehc_Startup() function.
 *         : 26.05.2020 1.20     Changed the function of LVD1LVL
 *                                - r_ehc_Startup()
 *                               Added the function of LVD1
 *                                - ehc_Startup_ramfunc()
 *         : 24.11.2020 1.30     Modified the code copy function from .ehc_ramfunc_init
 *                               to .ehc_ramfunc in IAR Compiler.
 *                               Modified the code copy function from .intvec to .ramvect in IAR Compiler.
 *         : 12.05.2021 1.40     Modified the code copy function from .ehc_ramfunc_init
 *                               to .ehc_ramfunc in using IAR Compiler.
 *                               Modified the code copy function from .intvec to .ramvect in using IAR Compiler.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup grp_cmsis_core
 * @{
 **********************************************************************************************************************/

/*------------------------------------------------------------------------------
 * Include
 *----------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "RE01_1500KB.h"
#include "r_core_cfg.h"

/*------------------------------------------------------------------------------
 * Pragma
 *----------------------------------------------------------------------------*/
#pragma section = ".ramvect"
#pragma section = ".intvec"
#pragma section = ".ehc_ramfunc"
#pragma section = ".ehc_ramfunc_init"

/*------------------------------------------------------------------------------
 * Macro
 *----------------------------------------------------------------------------*/
#if defined(__ICCARM__)                  /* IAR Compiler */
#define NON_VOLATILE  __root             /* @suppress("Macro expansion") */ /* @suppress("Internal macro naming") */
#define NON_VOLATILE_ATTRIBUTE           /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define STACKLESS_ATTRIBUTE     __stackless
#elif defined(__GNUC__)                  /* GCC Compiler */
#define NON_VOLATILE                     /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define NON_VOLATILE_ATTRIBUTE    __attribute__ ((__used__))    /* @suppress("Macro expansion") */ /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define STACKLESS_ATTRIBUTE     __attribute__((naked))
#else
#error "That compiler is not supported in this release."
#endif

/* Delay count for LVD stabilization. */
#define LVD_STABILIZAION_DELAY_MOCO     (300U)
#define LVD_STABILIZAION_DELAY_LOCO     (5U)

/* Initial setting of LVD1 */
#define R_EHC_PRV_EHC_LVD1LVL           (0x07U)

/* LVD level select */
#if   (0 > SYSTEM_CFG_EHC_LVD1LVL || 7 < SYSTEM_CFG_EHC_LVD1LVL)
    #error "ERROR - LVD1 level can not selected."
#endif
#if   (0 > SYSTEM_CFG_EHC_LVDBATLVL || 4 < SYSTEM_CFG_EHC_LVDBATLVL)
    #error "ERROR - LVDBAT level can not selected."
#else
    #define R_EHC_PRV_EHC_LVDBATLVL     (SYSTEM_CFG_EHC_LVDBATLVL + 3)
#endif

/*------------------------------------------------------------------------------
 * Private global variables and functions
 *----------------------------------------------------------------------------*/
NON_VOLATILE static void ehc_Startup_ramfunc(void) __attribute__ ((section(".ehc_ramfunc"))) NON_VOLATILE_ATTRIBUTE;  /* @suppress("Internal function declaration") */
void r_ehc_Startup(void);                                                   /* @suppress("Internal function declaration") */
NON_VOLATILE void NMI_Handler_EHC( void )  __attribute__ ((section(".ehc_ramfunc"))) NON_VOLATILE_ATTRIBUTE;              /* @suppress("Internal function declaration") */
NON_VOLATILE void IEL1_IRQHandler_EHC( void )  __attribute__ ((section(".ehc_ramfunc"))) NON_VOLATILE_ATTRIBUTE;      /* @suppress("Internal function declaration") */
NON_VOLATILE STACKLESS_ATTRIBUTE static void ehc_software_delay_loop(uint32_t loop_cnt)  __attribute__ ((section(".ehc_ramfunc"))) NON_VOLATILE_ATTRIBUTE;  /* @suppress("Internal function declaration") */
static void * ehc_memcpy(void *p_destination, const void *p_source, size_t num);

#if defined(__GNUC__)                    /* GCC Compiler */
extern uint32_t __ehc_ramfunc_init_start;                 /* @suppress("Source line ordering") */
extern uint32_t __ehc_ramfunc_start;
extern uint32_t __ehc_ramfunc_end;
extern uint32_t __Vectors_Start;
extern uint32_t __Vectors_Size;
extern uint32_t __ramvect_start;
#endif

/* Function Name : r_ehc_Startup */
/***************************************************************************//**
* @brief       Initialization of system ehc
* @param[in]   None
* @param[out]  None
* @retval      None
*******************************************************************************/
void r_ehc_Startup(void) /* @suppress("Function length") */ /* @suppress("Function declaration") */ /* @suppress("Non-API function naming") */
{
    
    /*--------------------------------------------------------------------------
     * Set the protect function of the register
     * PRCR register
     *     b0 : [    PRC0] Writing to the clock generation circuit related
     *                     registers
     *                     - [1] Enable
     *     b1 : [    PRC1] Writing to the low power consumption mode related
     *                     registers
     *                     - [1] Enable
     *     b2 : [Reserved] Set the 0
     *     b3 : [    PRC3] Writing to the LVD related registers
     *                     - [0] Disable
     *  b7-b4 : [Reserved] Set the 0
     * b15-b8 : [   PRKEY] Control the write to PRCR register
     *                     - [10100101] Rewrite PRCR register
     *------------------------------------------------------------------------*/
    SYSTEM->PRCR = 0xA50BU;
    
    /**** Enable Stop module clock for DTC/DMAC */
    SYSTEM->MSTPCRA_b.MSTPA22 = 1U;     /* Stop module clock for DTC/DMAC */
    
    
#if defined(__ICCARM__)              /* IAR Compiler */
    uint8_t   * p_from;
    uint8_t   * p_to;
    uint32_t  * p_ehc_handler;
    uint32_t  * p_ehc_nmi_handler;
    
    p_from = __section_begin(".ehc_ramfunc_init");
    p_to   = __section_begin(".ehc_ramfunc");

    ehc_memcpy(p_to, p_from, __section_size(".ehc_ramfunc_init"));
    
    p_from = __section_begin(".intvec");
    p_to   = __section_begin(".ramvect");

    ehc_memcpy(p_to, p_from, __section_size(".intvec"));
    
#elif defined(__GNUC__)                  /* GCC Compiler */
    uint32_t  bytes;
    uint32_t  * p_ehc_handler;
    uint32_t  * p_ehc_nmi_handler;
    uint8_t * psource = (uint8_t *)0;    /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    uint8_t * pdest = (uint8_t *)0;      /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    
    /* Copy initialized .ehc_ramfunc from ROM to RAM. */
    psource = (uint8_t *)&__ehc_ramfunc_init_start;        /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    pdest   = (uint8_t *)&__ehc_ramfunc_start;             /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    bytes = (uint32_t)&__ehc_ramfunc_end - (uint32_t)&__ehc_ramfunc_start;
    for (uint32_t index = 0U; index < bytes; index++, pdest++, psource++)
    {
        *pdest = *psource;
    }

    /* Copy initialized .intvec from ROM to RAM. */
    psource = (uint8_t *)&__Vectors_Start;                 /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    pdest   = (uint8_t *)&__ramvect_start;                 /* @suppress("Local pointer variable naming") */ /* @suppress("Cast comment") */
    bytes = (uint32_t)&__Vectors_Size;
    for (uint32_t index = 0U; index < bytes; index++, pdest++, psource++)
    {
        *pdest = *psource;
    }
#endif

#if defined(__ICCARM__)              /* IAR Compiler */
    p_ehc_handler  = (uint32_t *)((uint32_t)__section_begin(".ramvect") + 0x44U);
#elif defined(__GNUC__)              /* GCC Compiler */    
    p_ehc_handler  = (uint32_t *)((uint32_t)&__ramvect_start + 0x44U);
#endif
    *p_ehc_handler = (uint32_t)IEL1_IRQHandler_EHC;
    
#if defined(__ICCARM__)              /* IAR Compiler */
    p_ehc_nmi_handler  = (uint32_t *)((uint32_t)__section_begin(".ramvect") + 0x08U);
#elif defined(__GNUC__)              /* GCC Compiler */    
    p_ehc_nmi_handler  = (uint32_t *)((uint32_t)&__ramvect_start + 0x08U);
#endif
    *p_ehc_nmi_handler = (uint32_t)NMI_Handler_EHC;
    
    /* Always map ehc_ramfunc on RAM */
#if defined(__ICCARM__)              /* IAR Compiler */
    SCB->VTOR = (uint32_t)__section_begin(".ramvect");
#elif defined(__GNUC__)              /* GCC Compiler */
    SCB->VTOR = (uint32_t)&__ramvect_start;
#endif
    
    /**** Initialize LVD NMI ****/
    /*--------------------------------------------------------------------------
     * NMI(LVD) interrupt setting
     *------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------
     * LVD setting
     *------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------
     * Set the interrupt LVD1 of the register
     * NMICLR register
     *    b2 : [ LVD1CLR] -[1] Clear NMISR.LVD1ST flag 
     *------------------------------------------------------------------------*/
    /* Initialize LVD NMI */
    ICU->NMICLR_b.LVD1CLR = 1U;

    /*--------------------------------------------------------------------------
     * Set the interrupt LVD1 of the register
     * NMIER register
     *    b2 : [  LVD1EN] -[1] Allow LVD1 interrupt
     *------------------------------------------------------------------------*/
    ICU->NMIER_b.LVD1EN   = 1U;
    
    /*--------------------------------------------------------------------------
     * Set the LVD circuit monitor enable
     * LVCMPCR register
     *    b6 : [ LVDBATE] Power status selection
     *                    - [0] LVDVBAT circuit monitor disable
     *    b5 : [   LVD1E] Power status selection
     *                    - [0] LVD1 circuit monitor disable
     *------------------------------------------------------------------------*/
    SYSTEM->LVCMPCR = 0x00U;    /* SYSTEM->LVCMPCR_b.LVD1E   = 0U; *
                                 * SYSTEM->LVCMPCR_b.LVDBATE = 0U; */
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 0
     * LVD1CR0 register
     *    b7 : [      RN] VCC>Vdet1 negate or LVD1 reset negate
     *                    - [0] VCC>Vdet1 negate
     *    b6 : [      RI] Vdet1 Selection of interrupt or reset enable
     *                    - [0] interrupt select
     * b5-b4 : [   FSAMP] Sampling clock select
     *                    - [0 0] LOCO/2 select
     *    b2 : [    CMPE] Vdet1 circuit compare result output enable
     *                    - [0] disable select
     *    b1 : [   DFDIS] Digital filter disable
     *                    - [1] Digital filter disable select
     *    b0 : [     RIE] interrupt or reset enable
     *                    - [0] interrupt or reset disable select
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR0 = 0x02U;    /* SYSTEM->LVD1CR0_b.RIE     = 0U; *
                                 * SYSTEM->LVD1CR0_b.DFDIS   = 1U; *
                                 * SYSTEM->LVD1CR0_b.CMPE    = 0U; *
                                 * SYSTEM->LVD1CR0_b.FSAMP   = 0U; *
                                 * SYSTEM->LVD1CR0_b.RI      = 0U; *
                                 * SYSTEM->LVD1CR0_b.RN      = 0U; */
    
     /*--------------------------------------------------------------------------
     * Set the LVD voltage detection level select
     * LVDLVLR register
     *------------------------------------------------------------------------*/
    SYSTEM->LVDLVLR = (uint8_t)((R_EHC_PRV_EHC_LVDBATLVL << 0x04U) |
                                (R_EHC_PRV_EHC_LVD1LVL           ));
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor enable
     * LVCMPCR register
     *    b6 : [ LVDBATE] Power status selection
     *                    - [0] LVDVBAT circuit monitor disable
     *    b5 : [   LVD1E] Power status selection
     *                    - [1] LVD1 circuit monitor enable
     *------------------------------------------------------------------------*/
    SYSTEM->LVCMPCR = 0x20U;    /* SYSTEM->LVCMPCR_b.LVD1E   = 1U; *
                                 * SYSTEM->LVCMPCR_b.LVDBATE = 0U; */
    
    /*--------------------------------------------------------------------------
     * Wait LVD1 stabilization (System clock source : MOCO)
     *------------------------------------------------------------------------*/
    ehc_software_delay_loop(LVD_STABILIZAION_DELAY_MOCO);
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 1
     * LVD1CR1 register
     * b1-b0 : [  IDTSEL] Select the condition under which the interrupt occurs
     *                    - [01] VCC < Vdet1 select
     *    b2 : [  IRQSEL] Selection of interrupt type
     *                    - [0] NMI interrupt select
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR1 = 0x01U;    /* SYSTEM->LVD1CR1_b.IDTSEL  = 1U; *
                                 * SYSTEM->LVD1CR1_b.IRQSEL  = 0U; */
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor status register
     * LVD1SR register
     *    b0 : [     DET] Vdet1 detection flg
     *                    - [0] Clear detection flg
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1SR_b.DET = 0U;
    
    /*--------------------------------------------------------------------------
     * Wait the clear detection flg
     *------------------------------------------------------------------------*/
    while( 0 != SYSTEM->LVD1SR_b.DET )
    {
        ;    /* wait */
    }

    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 0
     * LVD1CR0 register
     *    b0 : [     RIE] Vdet1 monitor control selection interrupt/reset enable
     *                    - [1] interrupt enable start
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR0_b.RIE = 1U;
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 0
     * LVD1CR0 register
     *    b2 : [    CMPE] Vdet1 circuit compare result output enable
     *                    - [1] comparison output enable start
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR0_b.CMPE = 1U;
    
    /**** Initialize END LVD NMI ****/

    /**** Transition to VBB mode ****/
    /*--------------------------------------------------------------------------
     * Enable the VBB operation
     *------------------------------------------------------------------------*/
    /* Transition from Subosc-speed mode to VBB mode */
    SYSTEM->VBBCR = 0x80U;      /* SYSTEM->VBBCR_b.CLKSEL  = 0U; *
                                 * SYSTEM->VBBCR_b.VBBEN   = 1U; */
    
    /*--------------------------------------------------------------------------
     * Start the LOCO operation
     *------------------------------------------------------------------------*/
    /***** Set clock source to LOCO, and Transition to Subosc-speed mode. *****/
    /*--------------------------------------------------------------------------
     * Set the system clock division
     * SCKDIVCR register
     *  b7-b0  : [Reserved] Set the 0
     * b10-b8  : [    PCKB] Peripheral module clock B (PCLKB) select
     *                      - [SYSTEM_CFG_PCKB_DIV] User config
     * b23-b11 : [Reserved] Set the 0
     * b26-b24 : [     ICK] System clock (ICLK) / PCLKA select
     *                      - [SYSTEM_CFG_ICK_PCKA_DIV] User config
     * b31-b27 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->SCKDIVCR = (uint32_t)(0x00000000);

    /*--------------------------------------------------------------------------
     * Set the system clock source
     * SCKSCR register
     * b2-b0 : [   CKSEL] System clock source select
     *                      - [010] LOCO selection
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->SCKSCR = 0x02U;     /* Clock source : LOCO */
    while(0x02U != SYSTEM->SCKSCR)
    {
        /* Retry the writing */
        SYSTEM->SCKSCR = 0x02U;
    }
    
    /*--------------------------------------------------------------------------
     * Transition the normal mode to subosc-speed mode
     *------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------
     * Stop the MOCO operation
     *------------------------------------------------------------------------*/
    SYSTEM->MOCOCR = 0x01U;    /* Stop MOCO */
    while(0x01U != SYSTEM->MOCOCR)
    {
        /* Retry the writing */
        SYSTEM->MOCOCR = 0x01U;
    }
    
    /* Transition from High-speed mode to Subosc-speed mode */
    while(1U == SYSTEM->SOPCCR_b.SOPCMTSF)
    {
        ;    /* wait */
    }

    /*--------------------------------------------------------------------------
     * Set the power state control
     * PWSTCR register
     * b2-b0 : [    PWST] Power status selection
     *                    - [000] No power state transition
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x00U;

    /*--------------------------------------------------------------------------
     * Wait the transition from Normal mode
     *------------------------------------------------------------------------*/
    while(0x00U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x00U;
    }

    /*--------------------------------------------------------------------------
     * Wait the operating power control mode transition
     * SOPCCR register
     *    b4 : [SOPCMTSF] Sub operation Power control mode Transition state flag
     *                   - [1] During transition
     *    b0 : [   SOPCM] Sub operation Power control mode Selection
     *                   - [1] Subosc-speed mode selection
     *------------------------------------------------------------------------*/
    SYSTEM->SOPCCR_b.SOPCM = 1U;

    /*--------------------------------------------------------------------------
     * Wait the transition from Subosc-Speed mode
     *------------------------------------------------------------------------*/
    while(1U == SYSTEM->SOPCCR_b.SOPCMTSF)
    {
        ;    /* wait */
    }
    
    /*--------------------------------------------------------------------------
     * Wait the back bias voltage control (VBB) Initial setup complete
     *------------------------------------------------------------------------*/
    while(1U != SYSTEM->VBBST_b.VBBSTUP)
    {
        ;    /* wait */
    }
    
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    SYSTEM->SBYCR = 0x8000U;    /* SYSTEM->SBYCR_b.SSBYMP  = 0U; *
                                 * SYSTEM->SBYCR_b.SSBY    = 1U; */
    SYSTEM->DPSBYCR = 0x00U;    /* SYSTEM->DPSBYCR_b.DPSBY = 0U; */
    
    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSTEM->SNZCR = 0x00U;      /* SYSTEM->SNZCR_b.SNZE    = 0U; */
    
    /*--------------------------------------------------------------------------
     * Transition from normal mode to low-leakage-current(VBB) mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [110] Low-leakage-current(VBB) mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x06U;
    
    /*--------------------------------------------------------------------------
     * Wait the transition from normal mode to low-leakage-current(VBB) mode
     *------------------------------------------------------------------------*/
    while(0x06U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x06U;
    }
    
    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();

    /*--------------------------------------------------------------------------
     * Wait the transition from normal mode to low-leakage-current(VBB) mode
     *------------------------------------------------------------------------*/
    while(1U != SYSTEM->PWSTF_b.VBBM)
    {
        __WFE();
    }
    
    ehc_Startup_ramfunc();


    /**** Transition to Subosc-speed mode ****/
    /* Transition from VBB mode to Subosc-speed mode */
    /*--------------------------------------------------------------------------
     * Wait the operating power control mode transition
     * SOPCCR register
     *    b0 : [   SOPCM] Sub operation Power control mode Selection
     *                   - [1] Subosc-speed mode selection
     *------------------------------------------------------------------------*/
    SYSTEM->SOPCCR = 0x01U;     /* SYSTEM->SOPCCR_b.SOPCM  = 1U; */
    
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    SYSTEM->SBYCR = 0x8000U;    /* SYSTEM->SBYCR_b.SSBYMP  = 0U; *
                                 * SYSTEM->SBYCR_b.SSBY    = 1U; */
    SYSTEM->DPSBYCR = 0x00U;    /* SYSTEM->DPSBYCR_b.DPSBY = 0U; */
    
    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSTEM->SNZCR = 0x00U;      /* SYSTEM->SNZCR_b.SNZE    = 0U; */
    
    /*--------------------------------------------------------------------------
     * Transition from VBB mode to Subosc-speed mode (normal mode)
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [100] Subosc-speed mode (normal mode)
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x04U;

    /*--------------------------------------------------------------------------
     * Wait the transition from VBB mode to Subosc-speed mode (normal mode)
     *------------------------------------------------------------------------*/
    while(0x04U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x04U;
    }
    
    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();

    /*--------------------------------------------------------------------------
     * Wait the transition from VBB mode to Subosc-speed mode (normal mode)
     *------------------------------------------------------------------------*/
    while((0U != SYSTEM->PWSTF_b.BOOSTM) || (0U != SYSTEM->PWSTF_b.VBBM))
    {
        __WFE();
    }


    /**** Transition to High-speed mode, and Set clock source to MOCO ****/
    /* Transition from Subosc-speed mode to High-speed mode */
    while(0U != SYSTEM->SOPCCR_b.SOPCMTSF)
    {
        ;   /* wait */
    }
    
    /*--------------------------------------------------------------------------
     * Transition from Subosc-speed mode to High-speed mode (normal mode)
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [000] High-speed mode (normal mode)
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x00U;

    /*--------------------------------------------------------------------------
     * Wait the transition from Subosc-speed mode to High-speed mode (normal mode)
     *------------------------------------------------------------------------*/
    while(0x00U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x00U;
    }
    
    /*--------------------------------------------------------------------------
     * Set the power state control
     * PWSTCR register
     * b1-b0 : [OPCM] Power status selection
     *                - [00] High-speed mode
     *------------------------------------------------------------------------*/
    SYSTEM->OPCCR = 0x00U;

    /*--------------------------------------------------------------------------
     * Wait the operating power control mode transition
     * SOPCCR register
     *    b0 : [   SOPCM] Sub operation Power control mode Selection
     *                   - [0] Other than Subosc-speed mode selection
     *------------------------------------------------------------------------*/
    SYSTEM->SOPCCR_b.SOPCM = 0U;

    /*--------------------------------------------------------------------------
     * Wait the transition from Subosc-speed mode to High-speed mode (normal mode)
     *------------------------------------------------------------------------*/
    while(0U != SYSTEM->SOPCCR_b.SOPCMTSF)
    {
        ;   /* wait */
    }
    
    
    /*--------------------------------------------------------------------------
     * Start the MOCO operation
     * MOCOCR register
     *    b0 : [   MCSTP] MOCO start control
     *                    - [0] MOCO start
     *------------------------------------------------------------------------*/
    SYSTEM->MOCOCR = 0x00U;     /* Start MOCO */

    /*--------------------------------------------------------------------------
     * Wait the MOCO clock stabilization
     *------------------------------------------------------------------------*/
    while(0x00 != SYSTEM->MOCOCR)
    {
        /* Retry the writing */
        SYSTEM->MOCOCR = 0x00U;
    }
    
    /*--------------------------------------------------------------------------
     * Set the system clock source
     * SCKSCR register
     * b2-b0 : [   CKSEL] System clock source select
     *                      - [001] MOCO selection
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->SCKSCR = 0x01U;     /* Clock source : MOCO */
    while(0x01U != SYSTEM->SCKSCR)
    {
        /* Retry the writing */
        SYSTEM->SCKSCR = 0x01U;
    }
    
    /**** Disable Stop module clock for DTC/DMAC */
    SYSTEM->MSTPCRA_b.MSTPA22 = 0U;     /* Not stop module clock for DTC/DMAC */

    /*--------------------------------------------------------------------------
     * Set the protect function of the register
     * PRCR register
     *     b0 : [    PRC0] Writing to the clock generation circuit related
     *                     registers
     *                     - [1] Enable
     *     b1 : [    PRC1] Writing to the low power consumption mode related
     *                     registers
     *                     - [1] Enable
     *     b2 : [Reserved] Set the 0
     *     b3 : [    PRC3] Writing to the LVD related registers
     *                     - [0] Disable
     *  b7-b4 : [Reserved] Set the 0
     * b15-b8 : [   PRKEY] Control the write to PRCR register
     *                     - [10100101] Rewrite PRCR register
     *------------------------------------------------------------------------*/
    SYSTEM->PRCR = 0xA500U;
} /* End of function r_ehc_Startup() */


#if defined(__ICCARM__)              /* IAR Compiler */
#pragma location = ".ehc_ramfunc"
#endif

/* Function Name : ehc_Startup_ramfunc */
/***************************************************************************//**
 * @brief  EHC ram execution function
 *******************************************************************************/
NON_VOLATILE static void ehc_Startup_ramfunc(void) /* @suppress("Function length") */ /* @suppress("Function declaration") */ /* @suppress("Non-API function naming") */
{
    
    /**** Transition from all power supply(ALLPWON) mode to except flash power supply(EXFPWON) mode ****/
    /* Transition from all power supply(ALLPWON) mode to except flash power supply(EXFPWON) mode */
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    SYSTEM->SBYCR = 0x8000U;    /* SYSTEM->SBYCR_b.SSBYMP  = 0U; *
                                 * SYSTEM->SBYCR_b.SSBY    = 1U; */
    SYSTEM->DPSBYCR = 0x00U;    /* SYSTEM->DPSBYCR_b.DPSBY = 0U; */
    
    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
     SYSTEM->SNZCR = 0x00U;      /* SYSTEM->SNZCR_b.SNZE    = 0U; */
    
    /*----------------------------------------------------------------------
     * Transition from normal mode to except flash power supply(EXFPWON) mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [010] except flash power supply(EXFPWON) mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x02U;
    
    /*--------------------------------------------------------------------------
     * Wait the transition from all power supply(ALLPWON) mode to except flash power supply(EXFPWON) mode
     *------------------------------------------------------------------------*/
    while(0x02U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x02U;
    }
    
    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();
    
    /*--------------------------------------------------------------------------
     * Wait the transition from all power supply(ALLPWON) mode to except flash power supply(EXFPWON) mode
     *------------------------------------------------------------------------*/
    while(1U != SYSTEM->PWSTF_b.EXFPWONM)
    {
      __WFE();
    }

    /**** Transition END ****/
    
    
    /**** Transition from except flash power supply(EXFPWON) mode to minimum power supply(MINPWON) mode ****/
    /* Transition from except flash power supply(EXFPWON) mode to minimum power supply(MINPWON) mode */
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    SYSTEM->SBYCR = 0x8000U;    /* SYSTEM->SBYCR_b.SSBYMP  = 0U; *
                                 * SYSTEM->SBYCR_b.SSBY    = 1U; */
    SYSTEM->DPSBYCR = 0x00U;    /* SYSTEM->DPSBYCR_b.DPSBY = 0U; */
    
    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSTEM->SNZCR = 0x00U;      /* SYSTEM->SNZCR_b.SNZE    = 0U; */
    
    /*----------------------------------------------------------------------
     * Transition from except flash power supply(EXFPWON) mode to minimum power supply(MINPWON) mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [011] minimum power supply(MINPWON) mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x03U;
    
    /*--------------------------------------------------------------------------
     * Wait the transition from except flash power supply(EXFPWON) mode to minimum power supply(MINPWON) mode
     *------------------------------------------------------------------------*/
    while(0x03U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x03U;
    }
    
    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();
    
    /*--------------------------------------------------------------------------
     * Wait the transition from except flash power supply(EXFPWON) mode to minimum power supply(MINPWON) mode
     *------------------------------------------------------------------------*/
    while(1U != SYSTEM->PWSTF_b.MINPWONM)
    {
      __WFE();
    }
    
    /**** Enable checking voltage of Secondary battery (with LVDBAT) ****/
    /* Initialize LVDBAT */
    /*--------------------------------------------------------------------------
     * Set the LVD circuit monitor enable
     * LVCMPCR register
     *    b6 : [ LVDBATE] Power status selection
     *                    - [0] LVDVBAT circuit monitor disable
     *------------------------------------------------------------------------*/
    SYSTEM->LVCMPCR_b.LVDBATE = 0U;
    
    /*--------------------------------------------------------------------------
     * Set the LVD1BAT circuit monitor control register 0
     * LVDBATCR0 register
     *    b7 : [      RN] VBAT_EHC>VdetBAT negate or LVDBAT reset negate
     *                    - [0] VBAT_EHC>VdetBAT negate
     *    b6 : [      RI] VdetBAT Selection of interrupt or reset enable
     *                    - [0] interrupt select
     * b5-b4 : [   FSAMP] Sampling clock select
     *                    - [0 0] LOCO/2 select
     *    b2 : [    CMPE] VdetBAT circuit compare result output enable
     *                    - [0] disable select
     *    b1 : [   DFDIS] Digital filter disable
     *                    - [1] Digital filter disable select
     *    b0 : [     RIE] interrupt or reset enable
     *                    - [0] interrupt or reset disable select
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATCR0 = 0x02U;  /* SYSTEM->LVDBATCR0_b.RIE   = 0U; *
                                 * SYSTEM->LVDBATCR0_b.DFDIS = 1U; *
                                 * SYSTEM->LVDBATCR0_b.CMPE  = 0U; *
                                 * SYSTEM->LVDBATCR0_b.FSAMP = 0U; *
                                 * SYSTEM->LVDBATCR0_b.RI    = 0U; *
                                 * SYSTEM->LVDBATCR0_b.RN    = 0U; */
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor enable
     * LVCMPCR register
     *    b6 : [ LVDBATE] Power status selection
     *                    - [1] LVDVBAT circuit monitor enable
     *------------------------------------------------------------------------*/
    SYSTEM->LVCMPCR_b.LVDBATE = 1U;
    
    /*--------------------------------------------------------------------------
     * Wait LVDBAT stabilization (System clock source : LOCO)
     *------------------------------------------------------------------------*/
    ehc_software_delay_loop(LVD_STABILIZAION_DELAY_LOCO);
    
    /*--------------------------------------------------------------------------
     * Set the LVDBAT circuit monitor control register 1
     * VDBATCR1 register
     * b1-b0 : [  IDTSEL] Select the condition under which the interrupt occurs
     *                    - [00] VBAT_EHC >= VdetBAT select
     *    b2 : [  IRQSEL] Selection of interrupt type
     *                    - [1]  Masqaple interrupt select
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATCR1 = 0x04;   /* SYSTEM->LVDBATCR1_b.IDTSEL = 0U; *
                                 * SYSTEM->LVDBATCR1_b.IRQSEL = 1U; */
    
    /*--------------------------------------------------------------------------
     * Set the LVDBAT circuit monitor status register
     * LVDBATSR register
     *    b0 : [     DET] VdetBAT detection flg
     *                    - [0] Clear detection flg
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATSR_b.DET = 0U;
    
    /*--------------------------------------------------------------------------
     * Set the LVDBAT circuit monitor control register 0
     * LVDBATCR0 register
     *    b0 : [     RIE] interrupt or reset enable
     *                    - [1] interrupt enable start
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATCR0_b.RIE = 1U;
    
    /*--------------------------------------------------------------------------
     * Set the LVDBAT circuit monitor control register 0
     * LVDBATCR0 register
     *    b2 : [    CMPE] VdetBAT circuit compare result output enable
     *                    - [1] comparison output enable start
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATCR0_b.CMPE = 1U;
    
    
    /*--------------------------------------------------------------------------
     * Set the Enable interrupt wakeup register
     * WUPEN register
     *   b19 : [LVDBATWUPEN] Enable interrupt wakeup from software standby mode
     *                       - [1] interrupt wakeup enable 
     *------------------------------------------------------------------------*/
    ICU->WUPEN_b.LVDBATWUPEN = 1U;    /* Enable cancel of Software standby mode by LVD_LVDBAT */
    
    /*--------------------------------------------------------------------------
     * Set the ICU event link settings register
     * IELSR1 register
     * b4-b0 : [  IELS] ICU event link select
     *                - [00100] Event nummber 4
     *   b16 : [    IR] Interrupt status flag.
     *                - [0] 0 clear
     *   b24 : [  DTCE] DTC enabled.
     *                - [0] DTC disabled.
     *------------------------------------------------------------------------*/
    ICU->IELSR1 = 0x00000004U;  /* ICU->IELSR1_b.IELS = 4U; *
                                 * ICU->IELSR1_b.IR   = 0U; *
                                 * ICU->IELSR1_b.DTCE = 0U; */
    
    /* __NVIC_EnableIRQ(IEL0_IRQn); */
    NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IEL1_IRQn) & 0x1FUL));

    /**** Transition END ****/
    
    /*--------------------------------------------------------------------------
     *  Enable charging battery
     *------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------
     * Set the EHCCR0 register and charge the secondary battery.
     * EHCCR0 register
     * b9-b8 : [  VBATCTL] -[11] Charge the secondary battery
     *------------------------------------------------------------------------*/
    SYSTEM->EHCCR0_b.VBATCTL = 3U;
    
    /**** Enable checking voltage of Secondary battery (with LVDBAT) ****/
    /* Transition to Software standby mode (Disable Snooze mode) */
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    SYSTEM->SBYCR = 0x8000U;    /* SYSTEM->SBYCR_b.SSBYMP  = 0U; *
                                 * SYSTEM->SBYCR_b.SSBY    = 1U; */
    SYSTEM->DPSBYCR = 0x00U;    /* SYSTEM->DPSBYCR_b.DPSBY = 0U; */
    
    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSTEM->SNZCR = 0x00U;      /* SYSTEM->SNZCR_b.SNZE    = 0U; */
    
    /*--------------------------------------------------------------------------
     * Transition from minimum power supply(MINPWON) mode to Software standby mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [000] software standby mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x00U;

    /*--------------------------------------------------------------------------
     * Wait the transition from except flash power supply(MINPWON) mode to Software standby mode
     *------------------------------------------------------------------------*/
    while(0x00U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x00U;
    }
    
    /* If VBAT_EHC has exceeded VdetBAT already, do not transition to Software standby mode. */
    if(0U == SYSTEM->LVDBATSR_b.MON)
    {
        __WFI();
    }
    
    
    /* Disable LVDBAT */
    /*--------------------------------------------------------------------------
     * Set the LVDBAT circuit monitor control register 0
     * LVDBATCR0 register
     *    b2 : [    CMPE] VdetBAT circuit compare result output enable
     *                    - [0] disable select
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATCR0_b.CMPE = 0U;
    
    /*--------------------------------------------------------------------------
     * Set the LVDBAT circuit monitor control register 0
     * LVDBATCR0 register
     *    b0 : [     RIE] interrupt or reset enable
     *                    - [0] interrupt or reset disable select
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATCR0_b.RIE = 0U;
    
    /*--------------------------------------------------------------------------
     * Set the LVDBAT circuit monitor status register
     * LVDBATSR register
     *    b0 : [     DET] VdetBAT detection flg
     *                    - [0] Clear detection flg
     *------------------------------------------------------------------------*/
    SYSTEM->LVDBATSR_b.DET = 0U;
    
    /*--------------------------------------------------------------------------
     * Set the LVD circuit monitor enable
     * LVCMPCR register
     *    b6 : [ LVDBATE] Power status selection
     *                    - [0] LVDVBAT circuit monitor disable
     *------------------------------------------------------------------------*/
    SYSTEM->LVCMPCR_b.LVDBATE = 0U;
    
    
    /* Disable LVD_LVDBAT */
    /* __NVIC_DisableIRQ(IEL1_IRQn); */
    NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IEL1_IRQn) & 0x1FUL));
    __DSB();
    __ISB();
    
    
    /*-------------------------------------------------------------------------
     * ICU register
     * WUPEN register
     *   b19 : [LVDBATWUPEN] Enable interrupt wakeup from software standby mode
     *                       - [0] interrupt wakeup disable 
     *------------------------------------------------------------------------*/
    ICU->WUPEN_b.LVDBATWUPEN = 0U;
    
    /*--------------------------------------------------------------------------
     * Set the ICU event link settings register
     * IELSR1 register
     * b4-b0 : [  IELS] ICU event link select
     *                - [00000] interrupt disabled.
     *   b16 : [    IR] Interrupt status flag.
     *                - [0] 0 clear
     *   b24 : [  DTCE] DTC enabled.
     *                - [0] DTC disabled.
     *------------------------------------------------------------------------*/
    ICU->IELSR1 = 0x00000000U;  /* ICU->IELSR1_b.IELS = 0U; *
                                 * ICU->IELSR1_b.IR   = 0U; *
                                 * ICU->IELSR1_b.DTCE = 0U; */

    /**** Enable checking voltage of Secondary battery (with LVDBAT) ****/
    
    
    /**** Wait for charging */
    /* Disable EHC Charging Capacitor Quick Wake-up function */
    /*--------------------------------------------------------------------------
     * Set the EHC control register1.
     * EHCCR1 register
     *     b0 : [QUICKMODE] EHC capacitor charge fast start function.
     *                      -[1] Enable
     *------------------------------------------------------------------------*/
    SYSTEM->EHCCR1_b.QUICKMODE = 1U;
    
    /*--------------------------------------------------------------------------
     * Set the LVD circuit monitor enable
     * LVCMPCR register
     *    b6 : [ LVDBATE] Power status selection
     *                    - [0] LVDVBAT circuit monitor disable
     *    b5 : [   LVD1E] Power status selection
     *                    - [0] LVD1 circuit monitor disable
     *------------------------------------------------------------------------*/
    SYSTEM->LVCMPCR = 0x00U;    /* SYSTEM->LVCMPCR_b.LVD1E   = 0U; *
                                 * SYSTEM->LVCMPCR_b.LVDBATE = 0U; */
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 0
     * LVD1CR0 register
     *    b7 : [      RN] VCC>Vdet1 negate or LVD1 reset negate
     *                    - [0] VCC>Vdet1 negate
     *    b6 : [      RI] Vdet1 Selection of interrupt or reset enable
     *                    - [0] interrupt select
     * b5-b4 : [   FSAMP] Sampling clock select
     *                    - [0 0] LOCO/2 select
     *    b2 : [    CMPE] Vdet1 circuit compare result output enable
     *                    - [0] disable select
     *    b1 : [   DFDIS] Digital filter disable
     *                    - [1] Digital filter disable select
     *    b0 : [     RIE] interrupt or reset enable
     *                    - [0] interrupt or reset disable select
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR0 = 0x02U;    /* SYSTEM->LVD1CR0_b.RIE     = 0U; *
                                 * SYSTEM->LVD1CR0_b.DFDIS   = 1U; *
                                 * SYSTEM->LVD1CR0_b.CMPE    = 0U; *
                                 * SYSTEM->LVD1CR0_b.FSAMP   = 0U; *
                                 * SYSTEM->LVD1CR0_b.RI      = 0U; *
                                 * SYSTEM->LVD1CR0_b.RN      = 0U; */
    
    /*--------------------------------------------------------------------------
     * Set the LVD voltage detection level select
     * LVDLVLR register
     *------------------------------------------------------------------------*/
    SYSTEM->LVDLVLR = (uint8_t)((R_EHC_PRV_EHC_LVDBATLVL << 0x04U) |
                                (SYSTEM_CFG_EHC_LVD1LVL          ));
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor enable
     * LVCMPCR register
     *    b6 : [ LVDBATE] Power status selection
     *                    - [0] LVDVBAT circuit monitor disable
     *    b5 : [   LVD1E] Power status selection
     *                    - [1] LVD1 circuit monitor enable
     *------------------------------------------------------------------------*/
    SYSTEM->LVCMPCR = 0x20U;    /* SYSTEM->LVCMPCR_b.LVD1E   = 1U; *
                                 * SYSTEM->LVCMPCR_b.LVDBATE = 0U; */
    
    /*--------------------------------------------------------------------------
     * Wait LVD1 stabilization (System clock source : MOCO)
     *------------------------------------------------------------------------*/
    ehc_software_delay_loop(LVD_STABILIZAION_DELAY_MOCO);
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 1
     * LVD1CR1 register
     * b1-b0 : [  IDTSEL] Select the condition under which the interrupt occurs
     *                    - [01] VCC < Vdet1 select
     *    b2 : [  IRQSEL] Selection of interrupt type
     *                    - [0] NMI interrupt select
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR1 = 0x01U;    /* SYSTEM->LVD1CR1_b.IDTSEL  = 1U; *
                                 * SYSTEM->LVD1CR1_b.IRQSEL  = 0U; */
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor status register
     * LVD1SR register
     *    b0 : [     DET] Vdet1 detection flg
     *                    - [0] Clear detection flg
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1SR_b.DET = 0U;
    
    /*--------------------------------------------------------------------------
     * Wait the clear detection flg
     *------------------------------------------------------------------------*/
    while( 0 != SYSTEM->LVD1SR_b.DET )
    {
        ;    /* wait */
    }

    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 0
     * LVD1CR0 register
     *    b0 : [     RIE] Vdet1 monitor control selection interrupt/reset enable
     *                    - [1] interrupt enable start
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR0_b.RIE = 1U;
    
    /*--------------------------------------------------------------------------
     * Set the LVD1 circuit monitor control register 0
     * LVD1CR0 register
     *    b2 : [    CMPE] Vdet1 circuit compare result output enable
     *                    - [1] comparison output enable start
     *------------------------------------------------------------------------*/
    SYSTEM->LVD1CR0_b.CMPE = 1U;
    
    
    /**** Transition from minimum power supply(MINPWON) mode to except flash power supply(EXFPWON) mode ****/
    /* Transition from minimum power supply(MINPWON) mode to except flash power supply(EXFPWON) mode */
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    SYSTEM->SBYCR = 0x8000U;    /* SYSTEM->SBYCR_b.SSBYMP  = 0U; *
                                 * SYSTEM->SBYCR_b.SSBY    = 1U; */
    SYSTEM->DPSBYCR = 0x00U;    /* SYSTEM->DPSBYCR_b.DPSBY = 0U; */
    
    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSTEM->SNZCR = 0x00U;      /* SYSTEM->SNZCR_b.SNZE    = 0U; */
    
    /*--------------------------------------------------------------------------
     * Transition from minimum power supply(MINPWON) mode to except flash power supply(EXFPWON) mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [010] except flash power supply(EXFPWON) mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x02U;
    
    /*--------------------------------------------------------------------------
     * Wait the transition from minimum power supply(MINPWON) mode to except flash power supply(EXFPWON) mode
     *------------------------------------------------------------------------*/
    while(0x02U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x02U;
    }
    
    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();
    
    /*--------------------------------------------------------------------------
     * Wait the transition from minimum power supply(MINPWON) mode to except flash power supply(EXFPWON) mode
     *------------------------------------------------------------------------*/
    while(1U != SYSTEM->PWSTF_b.EXFPWONM)
    {
      __WFE();
    }
    
    /**** Transition from except flash power supply(EXFPWON) mode to all power supply(ALLPWON) mode ****/
    /* Transition from except flash power supply(EXFPWON) mode to all power supply(ALLPWON) mode */
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    SYSTEM->SBYCR = 0x8000U;    /* SYSTEM->SBYCR_b.SSBYMP  = 0U; *
                                 * SYSTEM->SBYCR_b.SSBY    = 1U; */
    SYSTEM->DPSBYCR = 0x00U;    /* SYSTEM->DPSBYCR_b.DPSBY = 0U; */
    
    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSTEM->SNZCR = 0x00U;      /* SYSTEM->SNZCR_b.SNZE    = 0U; */
    
    /*----------------------------------------------------------------------
     * Transition from except flash power supply(EXFPWON) mode to all power supply(ALLPWON) mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [001] all power supply(ALLPWON) mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->PWSTCR = 0x01U;

    /*--------------------------------------------------------------------------
     * Wait the transition from except flash power supply(EXFPWON) mode to all power supply(ALLPWON) mode
     *------------------------------------------------------------------------*/
    while(0x01U != SYSTEM->PWSTCR)
    {
        /* Retry the writing */
        SYSTEM->PWSTCR = 0x01U;
    }
    
    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();

    /*--------------------------------------------------------------------------
     * Wait the transition from except flash power supply(EXFPWON) mode to all power supply(ALLPWON) mode
     *------------------------------------------------------------------------*/
    while(1U != SYSTEM->PWSTF_b.ALLPWONM)
    {
      __WFE();
    }

} /* End of function ehc_Startup_ramfunc() */


#if defined(__ICCARM__)              /* IAR Compiler */
#pragma location = ".ehc_ramfunc"
#endif

/* Function Name : NMI_Handler_EHC */
/***************************************************************************//**
* @brief  NMI Handler EHC
*******************************************************************************/
NON_VOLATILE void NMI_Handler_EHC( void ) /* @suppress("Function declaration") */ /* @suppress("Non-API function naming") */
{
    /*--------------------------------------------------------------------------
     * Set the protect function of the register
     * PRCR register
     *     b0 : [    PRC0] Writing to the clock generation circuit related
     *                     registers
     *                     - [1] Enable
     *     b1 : [    PRC1] Writing to the low power consumption mode related
     *                     registers
     *                     - [1] Enable
     *     b2 : [Reserved] Set the 0
     *     b3 : [    PRC3] Writing to the LVD related registers
     *                     - [0] Disable
     *  b7-b4 : [Reserved] Set the 0
     * b15-b8 : [   PRKEY] Control the write to PRCR register
     *                     - [10100101] Rewrite PRCR register
     *------------------------------------------------------------------------*/
    SYSTEM->PRCR = 0xA503U;
    
     /*--------------------------------------------------------------------------
     * Start the LOCO operation
     * LOCOCR register
     *    b0 : [   LCSTP] LOCO start control
     *                    - [0] LOCO start
     *------------------------------------------------------------------------*/
    SYSTEM->LOCOCR = 0x00U;     /* Start LOCO */
    
    /*--------------------------------------------------------------------------
     * Wait the LOCO clock stabilization
     *------------------------------------------------------------------------*/
    while(0x00 != SYSTEM->LOCOCR)
    {
        /* Retry the writing */
        SYSTEM->LOCOCR = 0x00U;
    }
    
    /*--------------------------------------------------------------------------
     * Set the system clock source
     * SCKSCR register
     * b2-b0 : [   CKSEL] System clock source select
     *                      - [010] LOCO selection
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSTEM->SCKSCR = 0x02U;     /* Clock source : LOCO */
    while(0x02U != SYSTEM->SCKSCR)
    {
        /* Retry the writing */
        SYSTEM->SCKSCR = 0x02U;
    }
    
    /**** Enable Stop module clock for DTC/DMAC */
    SYSTEM->MSTPCRA_b.MSTPA22 = 1U;     /* Stop module clock for DTC/DMAC */
    
    /* Enable EHC Charging Capacitor Quick Wake-up function */
    SYSTEM->EHCCR1_b.QUICKMODE = 0U;
    
    while(1)
    {
        ;    /* loop */
    }
} /* End of function NMI_Handler_EHC() */



#if defined(__ICCARM__)              /* IAR Compiler */
#pragma location = ".ehc_ramfunc"
#endif

/* Function Name : IEL1_IRQHandler_EHC */
/***************************************************************************//**
* @brief  IEL1 IRQHandler EHC
*******************************************************************************/
NON_VOLATILE void IEL1_IRQHandler_EHC( void ) /* @suppress("Non-static function declaration location") */ /* @suppress("Non-API function naming") */
{
    /* 0 clear interrupt flg */
    ICU->IELSR1_b.IR = 0U;
} /* End of function IEL1_IRQHandler_EHC() */


#if defined(__ICCARM__)              /* IAR Compiler */
#pragma location = ".ehc_ramfunc"
#endif

/* Function Name : ehc_software_delay_loop */
/***************************************************************************//**
 * @brief This assembly language routine takes roughly 4 cycles per loop. 
 *        2 additional cycles occur when the loop exits. The 'naked'
 *        attribute indicates that the specified function does not need
 *        prologue/epilogue sequences generated by the compiler.
 * @param[in]     loop_cnt  The number of loops to iterate.
 * @retval        None.
*******************************************************************************/
NON_VOLATILE STACKLESS_ATTRIBUTE static void ehc_software_delay_loop(uint32_t loop_cnt) /* @suppress("Non-static function declaration location") */ /* @suppress("Non-API function naming") */
{
        __asm volatile ("ehc_sw_delay_loop:     \n"

#if defined(__ICCARM__)
                        "   subs r0, #1         \n"     ///< 1 cycle
#elif defined(__GNUC__)
                        "   sub r0, r0, #1      \n"     ///< 1 cycle
#endif

                        "   cmp r0, #0          \n"     ///< 1 cycle
/* CM0 has a different instruction set */
#ifdef __CORE_CM0PLUS_H_GENERIC
                        "   bne ehc_sw_delay_loop   \n" ///< 2 cycles
#else
                        "   bne.n ehc_sw_delay_loop \n" ///< 2 cycles
#endif
                        "   bx lr               \n");   ///< 2 cycles

} /* End of function ehc_software_delay_loop() */

/* Function Name : ehc_memcpy */
/*******************************************************************************************************************//**
 * @brief memcpy() for r_ehc_Startup().
 * @param[in]   p_destination   Destination buffer.
 * @param[in]   p_source        Source buffer.
 * @param[in]   num             Number of bytes to copy.
 **********************************************************************************************************************/
static void * ehc_memcpy(void *p_destination, const void *p_source, size_t num)
{
    uint8_t *p_byte_destination;
    uint8_t *p_byte_source;

    /** Copy 1 bytes **/

    /* Cast to uint8_t pointer */
    p_byte_destination = (uint8_t *) p_destination;

    /* Cast to uint8_t pointer */
    p_byte_source = (uint8_t *) p_source;
    while (num--)
    {
        *p_byte_destination++ = *p_byte_source++;
    }

    return p_destination;

} /* End of function ehc_memcpy() */

/*******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_core)
 **********************************************************************************************************************/

/* End of file r_ehc.c */
