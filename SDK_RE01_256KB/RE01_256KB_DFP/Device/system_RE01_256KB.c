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
 * Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : system_RE01_256KB.c
 * Version      : 1.20
 * Description  : CMSIS Cortex-M# Device Peripheral Access Layer Source File for Device RE01_256KB
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 25.10.2019 0.40     First release
 *         : 03.03.2020 0.80     Added volatile attribute to variables used in for loop to suppress optimization.
 *                                - system_moco_stop()
 *                                - system_loco_start()
 *                                - system_loco_stop()
 *                                - system_sosc_start()
 *                                - system_sosc_stop()
 *                               Added the FLL function operation.
 *                                - system_hoco_start()
 *         : 26.05.2020 1.00     Modified the transition in the Sub clock oscillator drive capability.
 *                                - system_sosc_start()
 *                               Added the "core_software_delay_loop()".
 *                               Changed the contents of function.
 *                                - system_moco_stop()
 *                                - system_loco_start()
 *                                - system_loco_stop()
 *                                - system_sosc_start()
 *                                - system_sosc_stop()
 *         : 24.11.2020 1.10     Added the operation when the EHC is not used, set QUICKMODE = 1
 *                               for external noise tolerance and current reduction.
 *                               Deleted the comments as the FLL test has completed.
 *         : 10.12.2021 1.20     Modified "In the case of set SOSC drive capability and config are unequal"
 *                                - system_sosc_start()
 **********************************************************************************************************************/
/*
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
 */

/*******************************************************************************************************************//**
 * @addtogroup grp_cmsis_core
 * @{
 **********************************************************************************************************************/

/*------------------------------------------------------------------------------
 * Include
 *----------------------------------------------------------------------------*/
#include <stdint.h>
#include "RE01_256KB.h"
#include "r_core_cfg.h"

/*------------------------------------------------------------------------------
 * Pragma
 *----------------------------------------------------------------------------*/
#pragma section = ".intvec"

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

/*------------------------------------------------------------------------------
 * Option function select register variable
 *----------------------------------------------------------------------------*/
/** Option Setting Memory */
NON_VOLATILE static const uint32_t OptionSettingMemory[] __attribute__ ((section(".OptionSetting"))) NON_VOLATILE_ATTRIBUTE = /* @suppress("Global static variable naming") */ /* @suppress("Source line ordering") */ /* @suppress("Unused variable declaration in file scope") */
{
    (uint32_t)SYSTEM_CFG_OFS0,                                  /* 0x400: OFS0 */ /* @suppress("Cast comment") */
    (uint32_t)SYSTEM_CFG_OFS1,                                  /* 0x404: OFS1 */
    (uint32_t)SYSTEM_CFG_SECMPU_PC0_START,                      /* 0x408: SECMPUPCS0 */
    (uint32_t)SYSTEM_CFG_SECMPU_PC0_END,                        /* 0x40c: SECMPUPCE0 */
    (uint32_t)SYSTEM_CFG_SECMPU_PC1_START,                      /* 0x410: SECMPUPCS1 */
    (uint32_t)SYSTEM_CFG_SECMPU_PC1_END,                        /* 0x414: SECMPUPCE1 */
    (uint32_t)SYSTEM_CFG_SECMPU_REGION0_START,                  /* 0x418: SECMPUS0 */
    (uint32_t)SYSTEM_CFG_SECMPU_REGION0_END,                    /* 0x41c: SECMPUE0 */
    (uint32_t)SYSTEM_CFG_SECMPU_REGION1_START,                  /* 0x420: SECMPUS1 */
    (uint32_t)SYSTEM_CFG_SECMPU_REGION1_END,                    /* 0x424: SECMPUE1 */
    (uint32_t)SYSTEM_CFG_SECMPU_REGION2_START,                  /* 0x428: SECMPUS2 */
    (uint32_t)SYSTEM_CFG_SECMPU_REGION2_END,                    /* 0x42c: SECMPUE2 */
    0xFFFFFFFF,                                                 /* 0x430: Reserve */
    0xFFFFFFFF,                                                 /* 0x434: Reserve */
    (uint16_t)SYSTEM_CFG_SECMPU_CONTROL_SETTING | 0xFFFF0000    /* 0x438: SECMPUAC */
};

/** Serial Programmer ID setting (OSIS) */
NON_VOLATILE static const uint32_t OptionSettingID[] __attribute__ ((section(".OptionSettingID"))) NON_VOLATILE_ATTRIBUTE = // @suppress("Unused variable declaration in file scope")
{
    (uint32_t)SYSTEM_CFG_ID_CODE_PROTECTION_1,      /* 0x0100A150: OSIS */
    (uint32_t)SYSTEM_CFG_ID_CODE_PROTECTION_2,      /* 0x0100A154: OSIS */
    (uint32_t)SYSTEM_CFG_ID_CODE_PROTECTION_3,      /* 0x0100A158: OSIS */
    (uint32_t)SYSTEM_CFG_ID_CODE_PROTECTION_4       /* 0x0100A15C: OSIS */
};

/** Access Window Setting (AWS) */
NON_VOLATILE static const uint32_t OptionSettingAWS[] __attribute__ ((section(".OptionSettingAWS"))) NON_VOLATILE_ATTRIBUTE = // @suppress("Unused variable declaration in file scope")
{
    (uint32_t)SYSTEM_CFG_AWS                        /* 0x0100A164: AWS */
};

/*------------------------------------------------------------------------------
 * Define
 *----------------------------------------------------------------------------*/
/* Used High-speed on-chip oscillator for system clock */
#if   (0 == SYSTEM_CFG_CLOCK_SOURCE)
#if   (0 == SYSTEM_CFG_HOCO_FREQUENCY)
#define SYSTEM_CLOCK_SEL    (24000000U)   /* System clock : HOCO-24MHz    */ /* @suppress("Internal macro naming") */
#elif (1 == SYSTEM_CFG_HOCO_FREQUENCY)
#define SYSTEM_CLOCK_SEL    (32000000U)   /* System clock : HOCO-32MHz    */ /* @suppress("Internal macro naming") */
#elif (2 == SYSTEM_CFG_HOCO_FREQUENCY)
#define SYSTEM_CLOCK_SEL    (48000000U)   /* System clock : HOCO-48MHz    */ /* @suppress("Internal macro naming") */
#else
#define SYSTEM_CLOCK_SEL    (64000000U)   /* System clock : HOCO-64MHz    */ /* @suppress("Internal macro naming") */
#endif

/* Used Medium-speed on-chip oscillator for system clock */
#elif (1 == SYSTEM_CFG_CLOCK_SOURCE)
#define SYSTEM_CLOCK_SEL    (2000000U)    /* System clock : MOCO-2MHz      */ /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/* Used Low-speed on-chip oscillator for system clock */
#elif (2 == SYSTEM_CFG_CLOCK_SOURCE)
#define SYSTEM_CLOCK_SEL    (32768U)      /* System clock : LOCO-32.768kHz */ /* @suppress("Internal macro naming") */

/* Used Main clock oscillator for system clock */
#elif (3 == SYSTEM_CFG_CLOCK_SOURCE)
#define SYSTEM_CLOCK_SEL    (SYSTEM_CFG_MOSC_FREQUENCY_HZ) /* @suppress("Internal macro naming") */
                                          /* System clock : MOSC           */

/* Used Sub clock oscillator for system clock */
#elif (4 == SYSTEM_CFG_CLOCK_SOURCE)
#define SYSTEM_CLOCK_SEL    (32768U)      /* System clock : 32.768kHz      */ /* @suppress("Internal macro naming") */
#endif

/* ICLK and PCLKA frequency division - /1 */
#if   (0 == SYSTEM_CFG_ICK_PCKA_DIV)
#define SYSTEM_CLOCK        (SYSTEM_CLOCK_SEL / 1)                            /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/* ICLK and PCLKA frequency division - /2 */
#elif (1 == SYSTEM_CFG_ICK_PCKA_DIV)
#define SYSTEM_CLOCK        (SYSTEM_CLOCK_SEL / 2)                            /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/* ICLK and PCLKA frequency division - /4 */
#elif (2 == SYSTEM_CFG_ICK_PCKA_DIV)
#define SYSTEM_CLOCK        (SYSTEM_CLOCK_SEL / 4)                            /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/* ICLK and PCLKA frequency division - /8 */
#elif (3 == SYSTEM_CFG_ICK_PCKA_DIV)
#define SYSTEM_CLOCK        (SYSTEM_CLOCK_SEL / 8)                            /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/* ICLK and PCLKA frequency division - /16 */
#elif (4 == SYSTEM_CFG_ICK_PCKA_DIV)
#define SYSTEM_CLOCK        (SYSTEM_CLOCK_SEL / 16)                            /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/* ICLK and PCLKA frequency division - /32 */
#elif (5 == SYSTEM_CFG_ICK_PCKA_DIV)
#define SYSTEM_CLOCK        (SYSTEM_CLOCK_SEL / 32)                            /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/* ICLK and PCLKA frequency division - /64 */
#else
#define SYSTEM_CLOCK        (SYSTEM_CLOCK_SEL / 64)                            /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#endif

/* System clock source check */

/* HOCO System clock source select */
#if   (0 == SYSTEM_CFG_CLOCK_SOURCE)
#if   (1 == SYSTEM_CFG_POWER_CONTROL_MODE)  /* Selected Normal mode (High-speed) */
#if   (32000000U < SYSTEM_CLOCK_SEL)        /* >32MHz */
    #error "ERROR - HOCO frequency can not selected Normal mode (High-speed) (r_core_cfg.h)."
#endif

#elif (4 == SYSTEM_CFG_POWER_CONTROL_MODE)  /* Selected Low-Leakage-Current mode (VBB) */
    #error "ERROR - HOCO frequency can not selected Low-Leakage-Current mode (VBB) (r_core_cfg.h)."
#endif

/* MOCO System clock source select */
#elif (1 == SYSTEM_CFG_CLOCK_SOURCE)
#if   (4 == SYSTEM_CFG_POWER_CONTROL_MODE)  /* Selected Low-Leakage-Current mode (VBB) */
    #error "ERROR - MOCO frequency can not selected Low-Leakage-Current mode (VBB) (r_core_cfg.h)"
#endif

/* LOCO System clock source select */
#elif (2 == SYSTEM_CFG_CLOCK_SOURCE)
#if   (4 == SYSTEM_CFG_POWER_CONTROL_MODE)  /* Selected Low-Leakage-Current mode (VBB) */
#if   (0 != SYSTEM_CFG_ICK_PCKA_DIV)            /* ICK(PCKA)==0 */
    #error "ERROR - LOCO frequency division ratio (r_core_cfg.h)"
#endif
#if   (SYSTEM_CFG_ICK_PCKA_DIV != SYSTEM_CFG_PCKB_DIV)
    #error "ERROR - LOCO frequency division ratio (r_core_cfg.h)"
#endif
#endif

/* MOSC System clock source select */
#elif (3 == SYSTEM_CFG_CLOCK_SOURCE)

#if   (32000000U < SYSTEM_CLOCK_SEL)        /* >32MHz */
    #error "ERROR - MOSC frequency out of range (r_core_cfg.h)"
#elif ( 8000000U > SYSTEM_CLOCK_SEL)        /* <8MHz */
    #error "ERROR - MOSC frequency out of range (r_core_cfg.h)"
#endif

#if   (4 == SYSTEM_CFG_POWER_CONTROL_MODE)  /* Selected Low-Leakage-Current mode (VBB) */
    #error "ERROR - MOSC frequency selected Low-Leakage-Current mode (VBB) (r_core_cfg.h)"
#endif

/* SOSC System clock source select */
#elif (4 == SYSTEM_CFG_CLOCK_SOURCE)
#if   (4 == SYSTEM_CFG_POWER_CONTROL_MODE)  /* Selected Low-Leakage-Current mode (VBB) */
#if   (0 != SYSTEM_CFG_ICK_PCKA_DIV)            /* ICK(PCKA)==0 */
    #error "ERROR - SOSC frequency division ratio (r_core_cfg.h)"
#endif
#if   (SYSTEM_CFG_ICK_PCKA_DIV != SYSTEM_CFG_PCKB_DIV)
    #error "ERROR - SOSC frequency division ratio (r_core_cfg.h)"
#endif
#endif
#endif

/* Other clock enable check */

/* BOOST mode check */
#if   (0U == SYSTEM_CFG_POWER_CONTROL_MODE)

/* Normal mode (High-speed) check */
#elif (1U == SYSTEM_CFG_POWER_CONTROL_MODE)

#if   (1U == SYSTEM_CFG_HOCO_ENABLE)
#if   (1 < SYSTEM_CFG_HOCO_FREQUENCY)        /* >32MHz */
    #error "ERROR - HOCO frequency can not selected Normal mode (High-speed) (r_core_cfg.h)."
#endif
#endif

/* Low-Leakage-Current mode (VBB) check */
#elif (4U == SYSTEM_CFG_POWER_CONTROL_MODE)
#if   (1U == SYSTEM_CFG_HOCO_ENABLE)
    #error "ERROR - HOCO frequency can not selected Low-Leakage-Current mode (VBB) (r_core_cfg.h)."
#endif
#if   (1U == SYSTEM_CFG_MOCO_ENABLE)
    #error "ERROR - MOCO frequency can not selected Low-Leakage-Current mode (VBB) (r_core_cfg.h)."
#endif
#if   (1U == SYSTEM_CFG_MOSC_ENABLE)
    #error "ERROR - MOSC frequency can not selected Low-Leakage-Current mode (VBB) (r_core_cfg.h)."
#endif

#endif

/* r_core_cfg.h parameter check  */
#if   (0 != SYSTEM_CFG_MOSC_ENABLE)
#if   (1 != SYSTEM_CFG_MOSC_ENABLE)
    #error "ERROR - MOSC frequency parameter of enable select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (32000000U < SYSTEM_CFG_MOSC_FREQUENCY_HZ)  /* input >32MHz */
    #error "ERROR - MOSC frequency (r_core_cfg.h)"
#elif ( 8000000U > SYSTEM_CFG_MOSC_FREQUENCY_HZ)  /* input <8MHz */
#if   (0U == SYSTEM_CFG_MOSC_CLOCK_SOURCE)
    #error "ERROR - MOSC frequency (r_core_cfg.h)"
#endif
#endif

#if   (7 < SYSTEM_CFG_MOSC_DRIVE)
    #error "ERROR - MOSC frequency parameter of drive capability select is out of range (r_core_cfg.h)"
#elif (1 > SYSTEM_CFG_MOSC_DRIVE)
    #error "ERROR - MOSC frequency parameter of drive capability select is out of range (r_core_cfg.h)"
#endif

#if   (0 != SYSTEM_CFG_MOSC_CLOCK_SOURCE)
#if   (1 != SYSTEM_CFG_MOSC_CLOCK_SOURCE)
    #error "ERROR - MOSC frequency parameter of clock source select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (0 != SYSTEM_CFG_MOSC_LOW_POWER_ENABLE)
#if   (1 != SYSTEM_CFG_MOSC_LOW_POWER_ENABLE)
    #error "ERROR - MOSC frequency parameter of low power enable select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (9 < SYSTEM_CFG_MOSC_WAIT_TIME)
    #error "ERROR - MOSC frequency parameter of stabilization time select is out of range (r_core_cfg.h)"
#elif (1 > SYSTEM_CFG_MOSC_WAIT_TIME)
    #error "ERROR - MOSC frequency parameter of stabilization time select is out of range (r_core_cfg.h)"
#endif

#if   (0 != SYSTEM_CFG_HOCO_ENABLE)
#if   (1 != SYSTEM_CFG_HOCO_ENABLE)
    #error "ERROR - HOCO frequency parameter of enable select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (3 < SYSTEM_CFG_HOCO_FREQUENCY)  /* input >64MHz */
    #error "ERROR - HOCO frequency (r_core_cfg.h)"
#elif (0 > SYSTEM_CFG_HOCO_FREQUENCY)  /* input <24MHz */
    #error "ERROR - HOCO frequency (r_core_cfg.h)"
#endif

#if   (0 != SYSTEM_CFG_MOCO_ENABLE)
#if   (1 != SYSTEM_CFG_MOCO_ENABLE)
    #error "ERROR - MOCO frequency parameter of enable select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (0 != SYSTEM_CFG_LOCO_ENABLE)
#if   (1 != SYSTEM_CFG_LOCO_ENABLE)
    #error "ERROR - LOCO frequency parameter of enable select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (0 != SYSTEM_CFG_SOSC_ENABLE)
#if   (1 != SYSTEM_CFG_SOSC_ENABLE)
    #error "ERROR - SOSC frequency parameter of enable select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (3 < SYSTEM_CFG_SOSC_DRIVE)
    #error "ERROR - SOSC frequency parameter of drive capability select is out of range (r_core_cfg.h)."
#elif (0 > SYSTEM_CFG_SOSC_DRIVE)
    #error "ERROR - SOSC frequency parameter of drive capability select is out of range (r_core_cfg.h)."
#endif

#if   (0 != SYSTEM_CFG_SOSC_NF_STOP)
#if   (1 != SYSTEM_CFG_SOSC_NF_STOP)
    #error "ERROR - SOSC frequency parameter of noise filter select is out of range (r_core_cfg.h)."
#endif
#endif

#if   (4 < SYSTEM_CFG_CLOCK_SOURCE)
    #error "ERROR - Clock source select is out of range (r_core_cfg.h)"
#elif (0 > SYSTEM_CFG_CLOCK_SOURCE)
    #error "ERROR - Clock source select is out of range (r_core_cfg.h)"
#endif

#if   (6 < SYSTEM_CFG_ICK_PCKA_DIV)
    #error "ERROR - ICLK and PCLKA frequency division ratio select is out of range (r_core_cfg.h)"
#elif (0 > SYSTEM_CFG_ICK_PCKA_DIV)
    #error "ERROR - ICLK and PCLKA frequency division ratio select is out of range (r_core_cfg.h)"
#endif

#if   (6 < SYSTEM_CFG_PCKB_DIV)
    #error "ERROR - PCLKB frequency division ratio select is out of range (r_core_cfg.h)"
#elif (0 > SYSTEM_CFG_PCKB_DIV)
    #error "ERROR - PCLKB frequency division ratio select is out of range (r_core_cfg.h)"
#endif

#if   (4 < SYSTEM_CFG_POWER_CONTROL_MODE)
    #error "ERROR - Power control mode select is out of range (r_core_cfg.h)"
#elif (0 > SYSTEM_CFG_POWER_CONTROL_MODE)
    #error "ERROR - Power control mode select is out of range (r_core_cfg.h)"
#elif (2 == SYSTEM_CFG_POWER_CONTROL_MODE)
    #error "ERROR - Power control mode select is out of range (r_core_cfg.h)"
#elif (3 == SYSTEM_CFG_POWER_CONTROL_MODE)
    #error "ERROR - Power control mode select is out of range (r_core_cfg.h)"
#endif

#if   (0 != SYSTEM_CFG_EHC_MODE)
#if   (1 != SYSTEM_CFG_EHC_MODE)
    #error "ERROR - EHC operating mode select is out of range (r_core_cfg.h)."
#endif
#endif

/* ICK(PCKA) PCKB division ratio check */
#if   (SYSTEM_CFG_ICK_PCKA_DIV > SYSTEM_CFG_PCKB_DIV)
    #error "ERROR - PCKB frequency division ratio (r_core_cfg.h)."
#endif

#if   (32000000U < SYSTEM_CLOCK_SEL)          /* >32MHz */
#if   (1 > SYSTEM_CFG_PCKB_DIV)
    #error "ERROR - PCKB frequency division ratio (r_core_cfg.h)."
#endif
#endif

/* ICLK 1cycle (sec) */
#define ICLK_1CYCLE                      (1.0F / SYSTEM_CLOCK)                       /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define ICLK_MOCO_1CYCLE                 (0.0000005F)                                /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
/* Wait time */
#define MOCO_TMOCOWT_SEC                 (0.000016F)                                 /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define MOCO_RESTART_SEC                 (0.0000025F)                                /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define LOCO_TLOCOWT_SEC                 (0.000130F)                                 /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define LOCO_RESTART_SEC                 (0.0001525879F)                             /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_SUBOSCOWT_SEC               (3.0F)                                      /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_RESTART_SEC                 (0.0001525879F)                             /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
/* In the case of the system clock is MOCO */
#define LOCO_TLOCOWT_TIME_START          (((LOCO_TLOCOWT_SEC   / ICLK_MOCO_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define LOCO_RESTART_TIME_START          (((LOCO_RESTART_SEC   / ICLK_MOCO_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_TSUBOSCOWT_TIME_START       (((SOSC_SUBOSCOWT_SEC / ICLK_MOCO_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_RESTART_TIME_START          (((SOSC_RESTART_SEC   / ICLK_MOCO_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
/* In the case of the changing the system clock setting */
#define MOCO_TMOCOWT_TIME_STOP           (((MOCO_TMOCOWT_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define MOCO_RESTART_TIME_STOP           (((MOCO_RESTART_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define LOCO_TLOCOWT_TIME_STOP           (((LOCO_TLOCOWT_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define LOCO_RESTART_TIME_STOP           (((LOCO_RESTART_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_TSUBOSCOWT_TIME_STOP        (((SOSC_SUBOSCOWT_SEC / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_RESTART_TIME_STOP           (((SOSC_RESTART_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

#define LOCO_TLOCOWT_TIME_START_ALONE    (((LOCO_TLOCOWT_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define LOCO_RESTART_TIME_START_ALONE    (((LOCO_RESTART_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_TSUBOSCOWT_TIME_START_ALONE (((SOSC_SUBOSCOWT_SEC / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */
#define SOSC_RESTART_TIME_START_ALONE    (((SOSC_RESTART_SEC   / ICLK_1CYCLE) / 4) + 1U) /* @suppress("Internal macro naming") */ /* @suppress("Non-block comment") */

/*------------------------------------------------------------------------------
 * System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Clock Frequency */ /* @suppress("Source line ordering") */

/*------------------------------------------------------------------------------
 * Private global variables and functions
 *----------------------------------------------------------------------------*/
static void system_clock_init(void);
static void system_mosc_start(void);
static void system_mosc_stop(void);
static void system_hoco_start(void);
static void system_hoco_stop(void);
static void system_moco_stop(void);
static void system_loco_start(void);
static void system_loco_stop(void);
static void system_sosc_start(void);
static void system_sosc_stop(void);
static void system_normal_to_boost(void);
static void system_normal_to_vbb(void);
#if defined(__ICCARM__)                   /* IAR Compiler */
__weak extern void r_ehc_Startup(void);
__weak extern void BoardInit(void);
#elif defined(__GNUC__)              /* GCC Compiler */
extern void r_ehc_Startup(void)              __attribute__ ((weak)); /* @suppress("Internal function declaration") */
extern void BoardInit( void )          __attribute__ ((weak)); /* @suppress("Internal function declaration") */
#else
#error "That compiler is not supported in this release."
#endif
NON_VOLATILE STACKLESS_ATTRIBUTE static void core_software_delay_loop(uint32_t loop_cnt) NON_VOLATILE_ATTRIBUTE;  /* @suppress("Internal function declaration") */


/* Function Name : SystemCoreClockUpdate */
/***************************************************************************//**
* @brief  Get core clock frequency.
* @param  None
* @retval None
*******************************************************************************/
void SystemCoreClockUpdate (void)         /* @suppress("Non-API function naming") */
{
    SystemCoreClock = SYSTEM_CLOCK;
} /* End of function SystemCoreClockUpdate() */

/* Function Name : SystemInit */
/***************************************************************************//**
* @brief  Initialization of system.
* @param  None
* @retval None
*******************************************************************************/
void SystemInit (void)                    /* @suppress("Function declaration") */ /* @suppress("Non-API function naming") */
{
#if SYSTEM_CFG_EHC_MODE == 1
    /* Initialization the EHC module */
    r_ehc_Startup();
#else
	SYSC->PRCR = 0xA502U;
	EHC->EHCCR1_b.QUICKMODE = 1U;
	SYSC->PRCR = 0xA500U;
#endif

    /* Initialization the pin direction and other settings to a safe condition. */    
    BoardInit();

    /* Initialization the clock */
    system_clock_init();

    /* Setting of nonexistent ports */

} /* End of function SystemInit() */

/* Function Name : system_clock_init */
/***************************************************************************//**
* @brief  Initialization of system clock and power control mode
* @param  None
* @retval None
*******************************************************************************/
static void system_clock_init(void) /* @suppress("Function length") */
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
    SYSC->PRCR = 0xA503U;

    /*--------------------------------------------------------------------------
     * Set the system clock source
     *------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------
     * In the case of system clock source is HOCO
     *------------------------------------------------------------------------*/
    if(0U == SYSTEM_CFG_CLOCK_SOURCE)
    {
        /*----------------------------------------------------------------------
         * In the case of HOCO frequency greater than 32MHz
         *--------------------------------------------------------------------*/
        if(1U < SYSTEM_CFG_HOCO_FREQUENCY)
        {
            /*------------------------------------------------------------------
             * Transition from normal mode to boost mode
             *----------------------------------------------------------------*/
            system_normal_to_boost();
        }
        else
        {
            ;/* Do nothing */
        }

        /*----------------------------------------------------------------------
         * Start the HOCO operation
         *--------------------------------------------------------------------*/
        system_hoco_start();
    }
    /*--------------------------------------------------------------------------
     * In the case of system clock source is MOCO
     *------------------------------------------------------------------------*/
    else if(1U == SYSTEM_CFG_CLOCK_SOURCE)
    {
        ;/* Do nothing - After reset is default clock:MOCO */
    }
    /*--------------------------------------------------------------------------
     * In the case of system clock source is LOCO
     *------------------------------------------------------------------------*/
    else if(2U == SYSTEM_CFG_CLOCK_SOURCE)
    {
        /*----------------------------------------------------------------------
         * Start the LOCO operation
         *--------------------------------------------------------------------*/
        system_loco_start();
    }
    /*--------------------------------------------------------------------------
     * In the case of system clock source is MOSC
     *------------------------------------------------------------------------*/
    else if(3U == SYSTEM_CFG_CLOCK_SOURCE) /* @suppress("If statement nesting") */
    {
        /*----------------------------------------------------------------------
         * Start the MOSC operation
         *--------------------------------------------------------------------*/
        system_mosc_start();
    }
    /*--------------------------------------------------------------------------
     * In the case of system clock source is SOSC
     *------------------------------------------------------------------------*/
    else
    {
        /*----------------------------------------------------------------------
         * Start the SOSC operation
         *--------------------------------------------------------------------*/
        system_sosc_start();
    }

    /*--------------------------------------------------------------------------
     * Set the Flash wait cycle resister
     *------------------------------------------------------------------------*/
    if(32000000U < SYSTEM_CLOCK)
    {
        /* If the frequency exceeds 32 MHz, change the flash wait cycle to 1 wait. */
        SYSF->FLWT = 1;
    }
    else
    {
        /* If the frequency is 32 MHz or less, set the flash wait cycle to 0 wait. */
        SYSF->FLWT = 0;
    }

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
    SYSC->SCKDIVCR = (uint32_t)((SYSTEM_CFG_ICK_PCKA_DIV << 0x18U) |
                                  (    SYSTEM_CFG_PCKB_DIV << 0x08U));

    /*--------------------------------------------------------------------------
     * Set the system clock source
     * SCKSCR register
     * b2-b0 : [   CKSEL] System clock source select
     *                      - [SYSTEM_CFG_CLOCK_SOURCE] User config
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSC->SCKSCR = (uint8_t) (SYSTEM_CFG_CLOCK_SOURCE);

    /*--------------------------------------------------------------------------
     * Set the power control mode
     *------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------
     * In the case of the power control mode is boost mode)
     *------------------------------------------------------------------------*/
    if(0U == SYSTEM_CFG_POWER_CONTROL_MODE)
    {
        /* In the case of not in boost mode */
        if(1U != SYSC->PWSTF_b.BOOSTM)
        {
            /* Transition to boost mode */
            system_normal_to_boost();
        }
        else
        {
            ;/* Do nothing */
        }
    }
    /*--------------------------------------------------------------------------
     * In the case of the power control mode is normal mode(High-speed)
     *------------------------------------------------------------------------*/
    else if(1U == SYSTEM_CFG_POWER_CONTROL_MODE)
    {
        ;/* Do nothing - After reset is default mode:normal mode(High-speed) */
    }
    /*--------------------------------------------------------------------------
     * In the case of the power control mode is Low-Leakage-Current(VBB) mode
     *------------------------------------------------------------------------*/
    else
    {
        /* Transition to normal mode(Low-Leakage-Current(VBB) mode) */
        system_normal_to_vbb();
    }

    /*--------------------------------------------------------------------------
     * Start the MOSC clock
     *------------------------------------------------------------------------*/
    if((1U == SYSTEM_CFG_MOSC_ENABLE) && (3U != SYSTEM_CFG_CLOCK_SOURCE))
    {
        system_mosc_start();
    }

    /*--------------------------------------------------------------------------
     * Start the HOCO clock
     *------------------------------------------------------------------------*/
    if(0U != SYSTEM_CFG_CLOCK_SOURCE)
    {
        if(1U != SYSTEM_CFG_HOCO_ENABLE)
        {
            system_hoco_stop();
        }
        else
        {
            system_hoco_start();
        }
    }

    /*--------------------------------------------------------------------------
     * Stop the MOCO clock
     *------------------------------------------------------------------------*/
    if((0U == SYSTEM_CFG_MOCO_ENABLE) && (1U != SYSTEM_CFG_CLOCK_SOURCE))
    {
        system_moco_stop();
    }

    /*--------------------------------------------------------------------------
     * Start the LOCO clock
     *------------------------------------------------------------------------*/
    if(2U != SYSTEM_CFG_CLOCK_SOURCE)
    {
        if(1U != SYSTEM_CFG_LOCO_ENABLE)
        {
            system_loco_stop();
        }
        else
        {
            system_loco_start();
        }
    }

    /*--------------------------------------------------------------------------
     * Start the SOSC enable clock
     *------------------------------------------------------------------------*/
    if(4U != SYSTEM_CFG_CLOCK_SOURCE)
    {
        if(1U != SYSTEM_CFG_SOSC_ENABLE)
        {
            system_sosc_stop();
        }
        else
        {
            system_sosc_start();
        }
    }

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
    SYSC->PRCR = 0xA500U;
} /* End of function system_clock_init() */

/* Function Name : system_mosc_start */
/***************************************************************************//**
* @brief  Start the main clock oscillator operation
* @param  None
* @retval None
*******************************************************************************/
static void system_mosc_start(void)
{
    /*--------------------------------------------------------------------------
     * Set the main clock oscillator operation
     * MOMCR register
     * b2-b0 : [Reserved] Set the 0
     * b5-b3 : [   MODRV] Main clock oscillator drive capability
     *                    - [SYSTEM_CFG_MOSC_DRIVE] User config
     *    b6 : [   MOSEL] Main clock oscillator clock source select
     *                    - [SYSTEM_CFG_MOSC_CLOCK_SOURCE] User config
     *    b7 : [ OSCLPEN] Main clock oscillator low power enable
     *                    - [SYSTEM_CFG_MOSC_LOW_POWER_ENABLE] User config
     *------------------------------------------------------------------------*/
    SYSC->MOMCR = (uint8_t) ((SYSTEM_CFG_MOSC_LOW_POWER_ENABLE << 0x07U) |
                               (    SYSTEM_CFG_MOSC_CLOCK_SOURCE << 0x06U) |
                               (           SYSTEM_CFG_MOSC_DRIVE << 0x03U));

    /*--------------------------------------------------------------------------
     * Set the main clock oscillator wait time
     * MOSCWTCR register
     * b3-b0 : [    MSTS] Main clock oscillation stabilization time
     *                    - [SYSTEM_CFG_MOSC_WAIT_TIME] User config
     * b7-b4 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSC->MOSCWTCR = (uint8_t)(SYSTEM_CFG_MOSC_WAIT_TIME);

    /*--------------------------------------------------------------------------
     * Start the main clock oscillator operation
     * MOSCCR register
     *    b0 : [   MOSTP] Main clock oscillation stop
     *                    - [0] Main clock oscillator operation start
     * b7-b1 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSC->MOSCCR = 0x00U;

    /*--------------------------------------------------------------------------
     * Wait the MOSCCR register read
     *------------------------------------------------------------------------*/
    while(0x00U != SYSC->MOSCCR)
    {
        ;/* wait */
    }

    /*--------------------------------------------------------------------------
     * Wait the main clock oscillation stabilization
     * OSCSF register
     * b3 : [MOSCSF] Main clock oscillation stabilization flag
     *               - [0] Waiting for oscillation stabilization
     *------------------------------------------------------------------------*/
    while(0U == SYSC->OSCSF_b.MOSCSF)
    {
        ;/* wait */
    }
} /* End of function system_mosc_start() */

/* Function Name : system_mosc_stop */
/***************************************************************************//**
* @brief  Stop the main clock oscillator operation
* @param  None
* @retval None
*******************************************************************************/
static void system_mosc_stop(void)
{
    /*--------------------------------------------------------------------------
     * In the case of main clock oscillation is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSC->MOSCCR_b.MOSTP)
    {
        /*----------------------------------------------------------------------
         * Wait the main clock oscillation stabilization
         * OSCSF register
         * b3 : [MOSCSF] Main clock oscillation stabilization flag
         *               - [0] Waiting for oscillation stabilization
         *--------------------------------------------------------------------*/
        while(0U == SYSC->OSCSF_b.MOSCSF)
        {
            ;/* wait */
        }

        /*----------------------------------------------------------------------
         * Stop the main clock oscillator operation
         * MOSCCR register
         *    b0 : [   MOSTP] Main clock oscillation stop
         *                    - [1] Main clock oscillator operation stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->MOSCCR = 0x01U;

        /*----------------------------------------------------------------------
         * Wait the MOSCCR register read
         *--------------------------------------------------------------------*/
        while(0x01U != SYSC->MOSCCR)
        {
            ;/* wait */
        }

        /*--------------------------------------------------------------------------
         * Wait the main clock oscillation stabilization
         * OSCSF register
         * b3 : [MOSCSF] Main clock oscillation stabilization flag
         *               - [1] Waiting for Main clock oscillator is stopped
         *------------------------------------------------------------------------*/
        while(1U == SYSC->OSCSF_b.MOSCSF)
        {
            ;/* wait */
        }
    }
    /*--------------------------------------------------------------------------
     * In the case of main clock oscillation is stop
     *------------------------------------------------------------------------*/
    else
    {
        ;/* Do nothing */
    }
} /* End of function system_mosc_stop() */

/* Function Name : system_hoco_start */
/***************************************************************************//**
* @brief  Start the high-speed on-chip oscillator operation
* @param  None
* @retval None
*******************************************************************************/
static void system_hoco_start(void) /* @suppress("Function length") */
{
    /*--------------------------------------------------------------------------
     * In the case of HOCO is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSC->HOCOCR_b.HCSTP)
    {
        /*----------------------------------------------------------------------
         * In the case of set frequency and config are unequal
         *--------------------------------------------------------------------*/
        if(SYSTEM_CFG_HOCO_FREQUENCY != SYSC->HOCOMCR)
        {
            /*------------------------------------------------------------------
             * Wait the HOCO stabilization
             * OSCSF register
             * b0 : [HOCOSF] HOCO clock stabilization flag
             *               - [0] Waiting for HOCO clock stabilization
             *----------------------------------------------------------------*/
            while(0U == SYSC->OSCSF_b.HOCOSF)
            {
                ;/* wait */
            }

            /*------------------------------------------------------------------
             * Stop the HOCO operation
             * HOCOCR register
             *    b0 : [   HCSTP] HOCO stop control
             *                    - [1] HOCO stop
             * b7-b1 : [Reserved] Set the 0
             *----------------------------------------------------------------*/
            SYSC->HOCOCR = 0x01U;

            /*------------------------------------------------------------------
             * Wait the HOCOCR register read
             *----------------------------------------------------------------*/
            while(0x01U != SYSC->HOCOCR)
            {
                ;/* wait */
            }

            /*------------------------------------------------------------------
             * Wait the HOCO stabilization
             * OSCSF register
             * b0 : [HOCOSF] HOCO clock stabilization flag
             *               - [1] Waiting for HOCO clock is stopped
             *----------------------------------------------------------------*/
            while(1U == SYSC->OSCSF_b.HOCOSF)
            {
                ;/* wait */
            }

            /*------------------------------------------------------------------
             * Set the FLL control
             * FLLCR1 register
             * b0 : [FLLEN]  FLL function enable flag
             *               - [1] FLL function is enable
             *----------------------------------------------------------------*/
            if (1U == SYSTEM_CFG_FLL_ENABLE)
            {
                /* Start the SOSC operation */
                system_sosc_start();
                
                /* FLL function is enable */
                SYSC->FLLCR1 = 0x01U;
            }
            
            /*------------------------------------------------------------------
             * Set the HOCO mode control
             * HOCOMCR register
             * b1-b0 : [   HCFRQ] HOCO oscillation frequency setting
             *                    - [SYSTEM_CFG_HOCO_FREQUENCY] User config
             * b7-b2 : [Reserved] Set the 0
             *----------------------------------------------------------------*/
            SYSC->HOCOMCR = (uint8_t) (SYSTEM_CFG_HOCO_FREQUENCY);

            /*------------------------------------------------------------------
             * Start the HOCO operation
             * HOCOCR register
             *    b0 : [   HCSTP] HOCO stop control
             *                    - [0] HOCO operation start
             * b7-b1 : [Reserved] Set the 0
             *----------------------------------------------------------------*/
            SYSC->HOCOCR = 0x00U;

            /*------------------------------------------------------------------
             * Wait the HOCOCR register read
             *----------------------------------------------------------------*/
            while(0x00U != SYSC->HOCOCR)
            {
                ;/* wait */
            }
        }
        /*----------------------------------------------------------------------
         * In the case of set frequency and config are equal
         *--------------------------------------------------------------------*/
        else
        {
            ;/* Do nothing */
        }
    }
    /*--------------------------------------------------------------------------
     * In the case of HOCO is stop
     *------------------------------------------------------------------------*/
    else
    {
        /*----------------------------------------------------------------------
         * Wait the HOCO stabilization
         * OSCSF register
         * b0 : [HOCOSF] HOCO clock stabilization flag
         *               - [1] Waiting for HOCO clock is stopped
         *--------------------------------------------------------------------*/
        while(1U == SYSC->OSCSF_b.HOCOSF)
        {
            ;/* wait */
        }

        /*------------------------------------------------------------------
         * Set the FLL control
         * FLLCR1 register
         * b0 : [FLLEN]  FLL function enable flag
         *               - [1] FLL function is enable
         *----------------------------------------------------------------*/
        if (1U == SYSTEM_CFG_FLL_ENABLE)
        {
            /* Start the SOSC operation */
            system_sosc_start();
            
            /* FLL function is enable */
            SYSC->FLLCR1 = 0x01U;
        }
        
        /*----------------------------------------------------------------------
         * Set the HOCO mode control
         * HOCOMCR register
         * b1-b0 : [   HCFRQ] HOCO oscillation frequency setting
         *                    - [SYSTEM_CFG_HOCO_FREQUENCY] User config
         * b7-b2 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->HOCOMCR = (uint8_t) (SYSTEM_CFG_HOCO_FREQUENCY);

        /*----------------------------------------------------------------------
         * Start the HOCO operation
         * HOCOCR register
         *    b0 : [   HCSTP] HOCO stop control
         *                    - [0] HOCO operation start
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->HOCOCR = 0x00U;

        /*----------------------------------------------------------------------
         * Wait the HOCOCR register read
         *--------------------------------------------------------------------*/
        while(0x00U != SYSC->HOCOCR)
        {
            ;/* wait */
        }
    }

    /*--------------------------------------------------------------------------
     * Wait the HOCO stabilization
     * OSCSF register
     * b0 : [HOCOSF] HOCO clock stabilization flag
     *               - [0] Waiting for HOCO clock stabilization
     *------------------------------------------------------------------------*/
    while(0U == SYSC->OSCSF_b.HOCOSF)
    {
        ;/* wait */
    }
} /* End of function system_hoco_start() */

/* Function Name : system_hoco_stop */
/***************************************************************************//**
* @brief  Stop the high-speed on-chip oscillator operation
* @param  None
* @retval None
*******************************************************************************/
static void system_hoco_stop(void)
{
    /*--------------------------------------------------------------------------
     * In the case of HOCO is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSC->HOCOCR_b.HCSTP)
    {
        /*----------------------------------------------------------------------
         * Wait the HOCO stabilization
         * OSCSF register
         *    b0 : [HOCOSF] HOCO clock stabilization flag
         *                  - [0] Waiting for HOCO clock stabilization
         *--------------------------------------------------------------------*/
        while(0U == SYSC->OSCSF_b.HOCOSF)
        {
            ;/* wait */
        }

        /*----------------------------------------------------------------------
         * Stop the HOCO operation
         * HOCOCR register
         *    b0 : [   HCSTP] HOCO stop control
         *                    - [1] HOCO stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->HOCOCR = 0x01U;

        /*----------------------------------------------------------------------
         * Wait the HOCOCR register read
         *--------------------------------------------------------------------*/
        while(0x01U != SYSC->HOCOCR)
        {
            ;/* wait */
        }

        /*--------------------------------------------------------------------------
         * Wait the HOCO stabilization
         * OSCSF register
         * b0 : [HOCOSF] HOCO clock stabilization flag
         *               - [1] Waiting for HOCO clock is stopped
         *------------------------------------------------------------------------*/
        while(1U == SYSC->OSCSF_b.HOCOSF)
        {
            ;/* wait */
        }
    }
    /*--------------------------------------------------------------------------
     * In the case of HOCO is stop
     *------------------------------------------------------------------------*/
    else
    {
        ;/* Do nothing */
    }
} /* End of function system_hoco_stop() */

/* Function Name : system_moco_stop */
/***************************************************************************//**
* @brief  Stop the medium-speed on-chip oscillator operation
* @param  None
* @retval None
*******************************************************************************/
static void system_moco_stop(void)
{
    /*--------------------------------------------------------------------------
     * Local variable declaration
     *------------------------------------------------------------------------*/
    
    /* MOCO oscillation stabilization time */
    uint32_t loop_count_tmocowt = (uint32_t)MOCO_TMOCOWT_TIME_STOP;

    /* MOCO oscillation stop time */
    uint32_t loop_count_restart = (uint32_t)MOCO_RESTART_TIME_STOP;

    /*--------------------------------------------------------------------------
     * In the case of MOCO is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSC->MOCOCR_b.MCSTP)
    {
        /*----------------------------------------------------------------------
         * Wait the MOCO clock stabilization (tMOCOWT)
         *--------------------------------------------------------------------*/
        core_software_delay_loop(loop_count_tmocowt);

        /*----------------------------------------------------------------------
         * Stop the MOCO operation
         * MOCOCR register
         *    b0 : [   MCSTP] MOCO stop control
         *                    - [1] MOCO stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->MOCOCR = 0x01U;

        /*----------------------------------------------------------------------
         * Wait the MOCOCR register read
         *--------------------------------------------------------------------*/
        while(0x01U != SYSC->MOCOCR)
        {
            ;/* wait */
        }

        /*--------------------------------------------------------------------------
         * Wait the MOCO restart time (MOCO clock(2MHz) * 5cycle)
         *------------------------------------------------------------------------*/
        core_software_delay_loop(loop_count_restart);

    }
    /*--------------------------------------------------------------------------
     * In the case of MOCO is stop
     *------------------------------------------------------------------------*/
    else
    {
        ;   /* Do nothing */
    }
} /* End of function system_moco_stop() */

/* Function Name : system_loco_start */
/***************************************************************************//**
* @brief  Start the low-speed on-chip oscillator operation
* @param  None
* @retval None
*******************************************************************************/
static void system_loco_start(void)
{
    /*--------------------------------------------------------------------------
     * Local variable declaration
     *------------------------------------------------------------------------*/

    /* LOCO oscillation stabilization time */
    uint32_t loop_count_tlocowt       = (uint32_t)LOCO_TLOCOWT_TIME_START;

    /* LOCO re-oscillation stabilization time */
    uint32_t loop_count_restart       = (uint32_t)LOCO_RESTART_TIME_START;
    
    /* LOCO oscillation stabilization time */
    uint32_t loop_count_tlocowt_alone = (uint32_t)LOCO_TLOCOWT_TIME_START_ALONE;

    /* LOCO oscillation stop time */
    uint32_t loop_count_restart_alone = (uint32_t)LOCO_RESTART_TIME_START_ALONE;

    /*--------------------------------------------------------------------------
     * In the case of LOCO is stop
     *------------------------------------------------------------------------*/
    if(1U == SYSC->LOCOCR_b.LCSTP)
    {
        /*----------------------------------------------------------------------
         * Wait the LOCO restart time (LOCO clock(32.768kHz) * 5cycle)
         *--------------------------------------------------------------------*/
        if(2U != SYSTEM_CFG_CLOCK_SOURCE)
        {
            core_software_delay_loop(loop_count_restart_alone);
        }
        else
        {
            core_software_delay_loop(loop_count_restart);
        }

        /*----------------------------------------------------------------------
         * Start the LOCO operation
         * LOCOCR register
         *    b0 : [   LCSTP] LOCO stop control
         *                    - [0] LOCO operation start
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->LOCOCR = 0x00U;

        /*----------------------------------------------------------------------
         * Wait the LOCOCR register read
         *--------------------------------------------------------------------*/
        while(0x00U != SYSC->LOCOCR)
        {
            ;/* wait */
        }
        
        /*--------------------------------------------------------------------------
         * Wait the LOCO clock stabilization (tLOCOWT)
         *------------------------------------------------------------------------*/
        if(2U != SYSTEM_CFG_CLOCK_SOURCE)
        {
            core_software_delay_loop(loop_count_tlocowt_alone);
        }
        else
        {
            core_software_delay_loop(loop_count_tlocowt);
        }

    }
    /*--------------------------------------------------------------------------
     * In the case of LOCO is operating
     *------------------------------------------------------------------------*/
    else
    {
        ;/* Do nothing */
    }

} /* End of function system_loco_start() */

/* Function Name : system_loco_stop */
/***************************************************************************//**
* @brief  Stop the low-speed on-chip oscillator operation
* @param  None
* @retval None
*******************************************************************************/
static void system_loco_stop(void)
{
    /*--------------------------------------------------------------------------
     * Local variable declaration
     *------------------------------------------------------------------------*/
   
    /* LOCO oscillation stabilization time */
    uint32_t loop_count_tlocowt = (uint32_t)LOCO_TLOCOWT_TIME_STOP;

    /* LOCO oscillation stop time */
    uint32_t loop_count_restart = (uint32_t)LOCO_RESTART_TIME_STOP;

    /*--------------------------------------------------------------------------
     * In the case of LOCO is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSC->LOCOCR_b.LCSTP)
    {
        /*----------------------------------------------------------------------
         * Wait the LOCO clock stabilization (tLOCOWT)
         *--------------------------------------------------------------------*/
        core_software_delay_loop(loop_count_tlocowt);

        /*----------------------------------------------------------------------
         * Stop the LOCO operation
         * LOCOCR register
         *    b0 : [   LCSTP] LOCO stop control
         *                    - [1] LOCO stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->LOCOCR = 0x01U;

        /*----------------------------------------------------------------------
         * Wait the LOCOCR register read
         *--------------------------------------------------------------------*/
        while(0x01U != SYSC->LOCOCR)
        {
            ;/* wait */
        }

        /*--------------------------------------------------------------------------
         * Wait the LOCO restart time (LOCO clock(32.768kHz) * 5cycle)
         *------------------------------------------------------------------------*/
        core_software_delay_loop(loop_count_restart);

    }
    /*--------------------------------------------------------------------------
     * In the case of LOCO is stop
     *------------------------------------------------------------------------*/
    else
    {
        ;/* Do nothing */
    }
} /* End of function system_loco_stop() */

/* Function Name : system_sosc_start */
/***************************************************************************//**
* @brief  Start the sub clock oscillator operation operation
* @param  None
* @retval None
*******************************************************************************/
static void system_sosc_start(void) /* @suppress("Function length") */
{
    /*--------------------------------------------------------------------------
     * Local variable declaration
     *------------------------------------------------------------------------*/
    
    /* SOSC oscillation stabilization time */
    uint32_t loop_count_tsuboscowt       = (uint32_t)SOSC_TSUBOSCOWT_TIME_START;

    /* SOSC re-oscillation stabilization time */
    uint32_t loop_count_restart          = (uint32_t)SOSC_RESTART_TIME_START;
    
    /* SOSC oscillation stabilization time */
    uint32_t loop_count_tsuboscowt_alone = (uint32_t)SOSC_TSUBOSCOWT_TIME_START_ALONE;

    /* SOSC oscillation stop time */
    uint32_t loop_count_restart_alone    = (uint32_t)SOSC_RESTART_TIME_START_ALONE;

    /*--------------------------------------------------------------------------
     * In the case of SOSC is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSC->SOSCCR_b.SOSTP)
    {
        /*----------------------------------------------------------------------
         * In the case of set SOSC drive capability and config are unequal
         *--------------------------------------------------------------------*/
        if(SYSTEM_CFG_SOSC_DRIVE != (SYSC->SOMCR & 0x03U))
        {
            /*------------------------------------------------------------------
             * Wait the SOSC clock stabilization (tSUBOSCOWT)
             *----------------------------------------------------------------*/
            if(4U != SYSTEM_CFG_CLOCK_SOURCE)
            {
                core_software_delay_loop(loop_count_tsuboscowt_alone);
            }
            else
            {
                core_software_delay_loop(loop_count_tsuboscowt);
            }

            /*------------------------------------------------------------------
             * Stop the SOSC operation
             * SOSCCR register
             *    b0 : [   SOSTP] SOSC stop control
             *                    - [1] SOSC stop
             * b7-b1 : [Reserved] Set the 0
             *----------------------------------------------------------------*/
            SYSC->SOSCCR = 0x01U;

            /*--------------------------------------------------------------------------
             * Wait the SOSCCR register read
             *------------------------------------------------------------------------*/
            while(0x01U != SYSC->SOSCCR)
            {
                ;/* wait */
            }

            /*------------------------------------------------------------------
             * Wait the SOSC restart time (SOSC clock(32.768kHz) * 5cycle)
             *----------------------------------------------------------------*/
            if(4U != SYSTEM_CFG_CLOCK_SOURCE)
            {
                core_software_delay_loop(loop_count_restart_alone);
            }
            else
            {
                core_software_delay_loop(loop_count_restart);
            }

            /*------------------------------------------------------------------
             * Set the SOSC mode control
             * SOMCR register
             * b0-b1 : [   SODRV] SOSC drive capability
             *                    - [SYSTEM_CFG_SOSC_DRIVE] User config
             * b3-b2 : [Reserved] Set the 0
             *    b4 : [ SONFSTP] SOSC noise filter
             *                    - [SYSTEM_CFG_SOSC_NF_STOP] User config
             * b7-b5 : [Reserved] Set the 0
             *----------------------------------------------------------------*/
            SYSC->SOMCR = (uint8_t) ((SYSTEM_CFG_SOSC_NF_STOP << 0x04U) |
                                     (SYSTEM_CFG_SOSC_DRIVE           ));

            /*------------------------------------------------------------------
             * Start the SOSC operation
             * SOSCCR register
             *    b0 : [   SOSTP] SOSC stop control
             *                    - [0] SOSC operation start
             * b7-b1 : [Reserved] Set the 0
             *----------------------------------------------------------------*/
            SYSC->SOSCCR = 0x00U;

            /*--------------------------------------------------------------------------
             * Wait the SOSCCR register read
             *------------------------------------------------------------------------*/
            while(0x00U != SYSC->SOSCCR)
            {
                ;/* wait */
            }

            /*--------------------------------------------------------------------------
             * Wait the SOSC clock stabilization (tSUBOSCOW)
             *------------------------------------------------------------------------*/
            if(4U != SYSTEM_CFG_CLOCK_SOURCE)
            {
                core_software_delay_loop(loop_count_tsuboscowt_alone);
            }
            else
            {
                core_software_delay_loop(loop_count_tsuboscowt);
            }

        }
        /*----------------------------------------------------------------------
         * In the case of set SOSC drive capability and config are equal
         *--------------------------------------------------------------------*/
        else
        {
            ;/* Do nothing */
        }
    }
    /*--------------------------------------------------------------------------
     * In the case of SOSC is stop
     *------------------------------------------------------------------------*/
    else
    {
        /*----------------------------------------------------------------------
         * Wait the SOSC restart time (SOSC clock(32.768kHz) * 5cycle)
         *--------------------------------------------------------------------*/
        if(4U != SYSTEM_CFG_CLOCK_SOURCE)
        {
            core_software_delay_loop(loop_count_restart_alone);
        }
        else
        {
            core_software_delay_loop(loop_count_restart);
        }

        /*----------------------------------------------------------------------
         * Set the SOSC mode control
         * SOMCR register
         * b0-b1 : [   SODRV] SOSC drive capability
         *                    - [SYSTEM_CFG_SOSC_DRIVE] User config
         * b3-b2 : [Reserved] Set the 0
         *    b4 : [ SONFSTP] SOSC noise filter
         *                    - [SYSTEM_CFG_SOSC_NF_STOP] User config
         * b7-b5 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->SOMCR = (uint8_t) ((SYSTEM_CFG_SOSC_NF_STOP << 0x04U) |
                                 (SYSTEM_CFG_SOSC_DRIVE           ));

        /*----------------------------------------------------------------------
         * Start the SOSC operation
         * SOSCCR register
         *    b0 : [   SOSTP] SOSC stop control
         *                    - [0] SOSC operation start
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->SOSCCR = 0x00U;

        /*--------------------------------------------------------------------------
         * Wait the SOSCCR register read
         *------------------------------------------------------------------------*/
        while(0x00U != SYSC->SOSCCR)
        {
            ;/* wait */
        }

        /*--------------------------------------------------------------------------
         * Wait the SOSC clock stabilization (tSUBOSCOW)
         *------------------------------------------------------------------------*/
        if(4U != SYSTEM_CFG_CLOCK_SOURCE)
        {
            core_software_delay_loop(loop_count_tsuboscowt_alone);
        }
        else
        {
            core_software_delay_loop(loop_count_tsuboscowt);
        }

    }

} /* End of function system_sosc_start() */

/* Function Name : system_sosc_stop */
/***************************************************************************//**
* @brief  Stop the sub clock oscillator operation operation
* @param  None
* @retval None
*******************************************************************************/
static void system_sosc_stop(void)
{
    /*--------------------------------------------------------------------------
     * Local variable declaration
     *------------------------------------------------------------------------*/
    /* SOSC oscillation stabilization time */
    uint32_t loop_count_tsuboscowt = (uint32_t)SOSC_TSUBOSCOWT_TIME_STOP;

    /* SOSC oscillation stop time */
    uint32_t loop_count_restart    = (uint32_t)SOSC_RESTART_TIME_STOP;

    /*--------------------------------------------------------------------------
     * In the case of SOSC is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSC->SOSCCR_b.SOSTP)
    {
        /*----------------------------------------------------------------------
         * Wait the SOSC clock stabilization (tSUBOSCOW)
         *--------------------------------------------------------------------*/
        core_software_delay_loop(loop_count_tsuboscowt);

        /*----------------------------------------------------------------------
         * Stop the SOSC operation
         * SOSCCR register
         *    b0 : [   SOSTP] SOSC stop control
         *                      - [1] SOSC stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSC->SOSCCR = 0x01U;

        /*--------------------------------------------------------------------------
         * Wait the SOSCCR register read
         *------------------------------------------------------------------------*/
        while(0x01U != SYSC->SOSCCR)
        {
            ;/* wait */
        }

        /*--------------------------------------------------------------------------
         * Wait the SOSC restart time (SOSC clock(32.768kHz) * 5cycle)
         *------------------------------------------------------------------------*/
        core_software_delay_loop(loop_count_restart);

    }
    
    /*--------------------------------------------------------------------------
     * In the case of SOSC is stop
     *------------------------------------------------------------------------*/
    else
    {
        ;/* Do nothing */
    }
} /* End of function system_sosc_stop() */

/* Function Name : system_normal_to_boost */
/***************************************************************************//**
* @brief  Transition the power control mode from normal mode(high-speed) to
*         boost mode
* @param  None
* @retval None
*******************************************************************************/
static void system_normal_to_boost(void)
{
    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    /* Set the software standby mode. (step1) */
    SYSC->SBYCR_b.SSBY    = 1U;

    /* Set the software standby mode. (step2) */
    SYSC->DPSBYCR_b.DPSBY = 0U;

    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSC->SNZCR_b.SNZE    = 0U;

    /*----------------------------------------------------------------------
     * Transition from normal mode to boost mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [101] Boost mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSC->PWSTCR = 0x05U;

    /*--------------------------------------------------------------------------
     * Wait the transition from normal mode to boost mode
     *------------------------------------------------------------------------*/
    while(0x05U != SYSC->PWSTCR)
    {
        /* Retry the writing */
        SYSC->PWSTCR = 0x05U;
    }

    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();

    /*--------------------------------------------------------------------------
     * Wait the transition from normal mode to boost mode
     *------------------------------------------------------------------------*/
    while(1U != SYSC->PWSTF_b.BOOSTM)
    {
      __WFE();
    }
} /* End of function system_normal_to_boost() */

/* Function Name : system_normal_to_vbb */
/***************************************************************************//**
* @brief  Transition the power control mode from normal mode(high-speed) to
*         low-leakage-current(VBB) mode
* @param  None
* @retval None
*******************************************************************************/
static void system_normal_to_vbb(void)
{
    /*--------------------------------------------------------------------------
     * Enable the VBB operation
     *------------------------------------------------------------------------*/
    if(4U == SYSTEM_CFG_CLOCK_SOURCE)
    {
        /*--------------------------------------------------------------------------
         * Set the clock for Back Bias control
         * VBBCR register
         * b0 : [CLKSEL] clock selection for Back Bias control
         *              - [1] Subosc clock
         *------------------------------------------------------------------------*/
        SYSC->VBBCR_b.CLKSEL = 1U;
    }
    else
    {
        /*--------------------------------------------------------------------------
         * Set the clock for Back Bias control
         * VBBCR register
         * b0 : [CLKSEL] clock selection for Back Bias control
         *              - [0] LOCO clock
         *------------------------------------------------------------------------*/
        SYSC->VBBCR_b.CLKSEL = 0U;
    }

    /*--------------------------------------------------------------------------
     * Set the enable of Back Bias control
     * VBBCR register
     * b7 : [VBBEN] enable of for Back Bias control
     *             - [1] VBB enable
     *------------------------------------------------------------------------*/
    SYSC->VBBCR_b.VBBEN = 1U;

    /*--------------------------------------------------------------------------
     * Stop the MOSC operation
     *------------------------------------------------------------------------*/
    system_mosc_stop();

    /*--------------------------------------------------------------------------
     * Stop the HOCO operation
     *------------------------------------------------------------------------*/
    system_hoco_stop();

    /*--------------------------------------------------------------------------
     * Stop the MOCO operation
     *------------------------------------------------------------------------*/
    system_moco_stop();

    /*--------------------------------------------------------------------------
     * Stop the LOCO operation
     *------------------------------------------------------------------------*/
    if((0U == SYSTEM_CFG_LOCO_ENABLE) && (2U != SYSTEM_CFG_CLOCK_SOURCE))
    {
        system_loco_stop();
    }

    /*--------------------------------------------------------------------------
     * In the case of stop the SOSC
     *------------------------------------------------------------------------*/
    if((0U == SYSTEM_CFG_SOSC_ENABLE) && (4U != SYSTEM_CFG_CLOCK_SOURCE))
    {
        system_sosc_stop();
    }

    /*--------------------------------------------------------------------------
     * Wait the back bias voltage control (VBB) Initial setup complete
     *------------------------------------------------------------------------*/
    while(0U == SYSC->VBBST_b.VBBSTUP)
    {
        ;/* wait */
    }

    /*--------------------------------------------------------------------------
     * Set the software standby mode
     *------------------------------------------------------------------------*/
    /* Set the software standby mode. (step1) */
    SYSC->SBYCR_b.SSBY    = 1U;

    /* Set the software standby mode. (step2) */
    SYSC->DPSBYCR_b.DPSBY = 0U;

    /*--------------------------------------------------------------------------
     * Disable the snooze mode
     *------------------------------------------------------------------------*/
    SYSC->SNZCR_b.SNZE    = 0U;

    /*----------------------------------------------------------------------
     * Transition from normal mode to low-leakage-current(VBB) mode
     * PWSTCR register
     * b2-b0 : [PWST] Power status control register
     *                - [110] Low-leakage-current(VBB) mode
     * b7-b3 : [Reserved] Set the 0
     *------------------------------------------------------------------------*/
    SYSC->PWSTCR = 0x06U;

    /*--------------------------------------------------------------------------
     * Wait the transition from normal mode to low-leakage-current(VBB) mode
     *------------------------------------------------------------------------*/
    while(0x06U != SYSC->PWSTCR)
    {
        /* Retry the writing */
        SYSC->PWSTCR = 0x06U;
    }

    /*--------------------------------------------------------------------------
     * Execute WFE instruction
     *------------------------------------------------------------------------*/
    __WFE();

    /*--------------------------------------------------------------------------
     * Wait the transition from normal mode to low-leakage-current(VBB) mode
     *------------------------------------------------------------------------*/
    while(1U != SYSC->PWSTF_b.VBBM)
    {
      __WFE();
    }
} /* End of function system_normal_to_vbb() */

/* Function Name : core_software_delay_loop */
/***************************************************************************//**
 * @brief This assembly language routine takes roughly 4 cycles per loop. 
 *        2 additional cycles occur when the loop exits. The 'naked'
 *        attribute indicates that the specified function does not need
 *        prologue/epilogue sequences generated by the compiler.
 * @param[in]     loop_cnt  The number of loops to iterate.
 * @retval        None.
*******************************************************************************/
NON_VOLATILE STACKLESS_ATTRIBUTE static void core_software_delay_loop (uint32_t loop_cnt) /* @suppress("Non-static function declaration location") */ /* @suppress("Non-API function naming") */
{
        __asm volatile ("core_sw_delay_loop:     \n"

#if defined(__ICCARM__)
                        "   subs r0, #1         \n"     ///< 1 cycle
#elif defined(__GNUC__)
                        "   sub r0, r0, #1      \n"     ///< 1 cycle
#endif

                        "   cmp r0, #0          \n"     ///< 1 cycle
/* CM0 has a different instruction set */
#ifdef __CORE_CM0PLUS_H_GENERIC
                        "   bne core_sw_delay_loop   \n" ///< 2 cycles
#else
                        "   bne.n core_sw_delay_loop \n" ///< 2 cycles
#endif
                        "   bx lr               \n");   ///< 2 cycles

} /* End of function core_software_delay_loop() */


/*******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_core)
 **********************************************************************************************************************/

/* End of file system_RE01_256KB.c */
