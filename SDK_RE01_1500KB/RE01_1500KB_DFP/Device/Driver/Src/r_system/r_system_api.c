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
 * Copyright (C) 2018-2020 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_system_api.c
 * Version      : 1.20
 * Description  : Device HAL SYSTEM (System Control, Common code among modules)
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 21.08.2018 0.50     First release
 *         : 06.11.2018 0.51     Change Register Name from "FCACHE" to "FLASH".
 *         : 15.01.2019 0.60     Integrate to "OptionSettingMemory[]" according to "gs_system_osm[]".
 *                               In order to used OSF1 register at CMSIS-CORE "system_E017.c",
 *                               it has moved out of "r_system.c".
 *                               Added "system_memcpy()", and
 *                               changed function used by R_SYS_CodeCopy() to system_memcpy().
 *                               Added the SOSC NF setting in R_SYS_SubOscSpeedClockStart().
 *                               Change Parameter check in R_SYS_SystemClockPLLSet().
 *                               Remove <intrinsics.h> in Include.
 *                               Change the power status control register(PWSTCR) wait processing.
 *                                - R_SYS_BoostSpeedModeSet()
 *                                - R_SYS_HighSpeedModeSet()
 *                                - R_SYS_LowSpeedModeSet()
 *                                - R_SYS_32kHzSpeedModeSet()
 *                               Adapted to GSCE coding rules.
 *                                - Changed file name to r_system_api.c
 *                               Update Doxygen comments.
 *         : 19.02.2019 0.61     Added R_SYS_SystemClockDividerSet() function.
 *                               Change the getting ICLK(PCLKA) frequency operation.
 *                                - R_SYS_SystemClockFreqGet()
 *                               Change the getting PCLKB frequency operation.
 *                                - R_SYS_PeripheralClockFreqGet()
 *                               Added the PCLKB parameter check in R_SYS_SystemClockHOCOSet().
 *                               Change the getting HOCO frequency operation in R_SYS_SystemClockHOCOSet().
 *                               Added the PCLKB parameter check in R_SYS_SystemClockPLLSet().
 *                               Change the getting PLL frequency operation in R_SYS_SystemClockPLLSet().
 *         : 16.04.2019 0.70     Change return value type "uint16_t" to "uint32_t" in R_SYS_GetVersion().
 *                               Added the waiting until the clock control register value has changed.
 *                                - R_SYS_MainOscSpeedClockStart()
 *                                - R_SYS_MainOscSpeedClockStop()
 *                                - R_SYS_HighSpeedClockStop()
 *                                - R_SYS_MediumSpeedClockStart()
 *                                - R_SYS_MediumSpeedClockStop()
 *                                - R_SYS_LowSpeedClockStart()
 *                                - R_SYS_LowSpeedClockStop()
 *                                - R_SYS_SubOscSpeedClockStart()
 *                                - R_SYS_SubOscSpeedClockStop()
 *                                - R_SYS_PLLSpeedClockStart()
 *                                - R_SYS_PLLSpeedClockStop()
 *                               Update the comments in R_SYS_SubOscSpeedClockStart().
 *                               Added code copy processing to RAM for GCC.
 *                                - R_SYS_CodeCopy()
 *                               Changed the gs_protect_counters[] initialize.
 *         : 11.06.2019 0.72     Added the gs_system_boost_flg and function.
 *                                - r_sys_BoostFlagGet()
 *                                - r_sys_BoostFlagSet()
 *                                - r_sys_BoostFlagClr()
 *                               Changed the section from ".intvec_init" to ".ramvect".
 *                               Added the section "__ramvect_start".
 *                               Changed code copy processing to RAM for section.
 *                               Added R_SYSTEM_PRV_ATTRIBUTE_STACKLESS_FOR_BOOSTFLAG for r_sys_BoostFlag function.
 *                               Added the volatile attribute to gs_prcr_masks[].
 *         : 06.08.2019 1.00     Added r_system_wdt_refresh() function.
 *                               Change the product name to RE Family.
 *                                - Change the include file names
 *                                - Update comments
 *                               Removed unnecessary wait processing.
 *                                - R_SYS_HighSpeedModeSet()
 *                                - R_SYS_32kHzSpeedModeSet()
 *         : 10.12.2019 1.01     Revised the register protection release period in following functions.
 *                                - R_SYS_BoostSpeedModeSet()
 *                                - R_SYS_HighSpeedModeSet()
 *                                - R_SYS_LowSpeedModeSet()
 *                                - R_SYS_32kHzSpeedModeSet()
 *                                - R_SYS_MainOscSpeedClockStart()
 *                                - R_SYS_MainOscSpeedClockStop()
 *                                - R_SYS_HighSpeedClockStart()
 *                                - R_SYS_HighSpeedClockStop()
 *                                - R_SYS_MediumSpeedClockStart()
 *                                - R_SYS_MediumSpeedClockStop()
 *                                - R_SYS_LowSpeedClockStart()
 *                                - R_SYS_LowSpeedClockStop()
 *                                - R_SYS_SubOscSpeedClockStart()
 *                                - R_SYS_SubOscSpeedClockStop()
 *                                - R_SYS_PLLSpeedClockStart()
 *                                - R_SYS_PLLSpeedClockStop()
 *                               Changed the placed section of gs_prcr_masks[] from ".data" to ".ramobj" on GNUC.
 *         : 18.02.2020 1.10     Added the value to gs_prcr_masks[] for PRC4 bit.
 *                               Modified the parameter checking of "R_SYSTEM_PRV_PLL_RANGE".
 *         : 26.05.2020 1.20     Modified the transition in the Sub clock oscillator drive capability.
 *                                - R_SYS_SubOscSpeedClockStart()
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup grp_device_hal_system
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "RE01_1500KB.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* Key code for writing PRCR register. */
#define R_SYSTEM_PRV_PRCR_KEY               (0xA500U)

/* Total number of IRQ event link in the ICU. */
#define R_SYSTEM_PRV_IRQ_EVENT_NUMBER_TOTAL (32)

/* Valid st_system_lock_t->lock values. */
#define R_SYSTEM_PRV_LOCK_LOCKED            (0x01)  /*!< Lock value when lock is taken */
#define R_SYSTEM_PRV_LOCK_UNLOCKED          (0x00)  /*!< Lock value when lock is available */

/* Definition of ICU->IELSR register. */
#define R_SYSTEM_PRV_IELSR_IR_MSK           (0x00010000)
#define R_SYSTEM_PRV_IELSR_IELS_MSK         (0x0000001F)

/* Definition of SYSTEM->OSCSF register. */
#define R_SYSTEM_PRV_OSCSF_HOCOSF_MSK       (0x01)
#define R_SYSTEM_PRV_OSCSF_MOSCSF_MSK       (0x08)
#define R_SYSTEM_PRV_OSCSF_PLLSF_MSK        (0x20)

/* Definition of SYSTEM->SCKSCR register. */
#define R_SYSTEM_PRV_SCKSCR_CKSEL_MSK       (0x07)
#define R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO      (0x00)
#define R_SYSTEM_PRV_SCKSCR_CKSEL_MOCO      (0x01)
#define R_SYSTEM_PRV_SCKSCR_CKSEL_LOCO      (0x02)
#define R_SYSTEM_PRV_SCKSCR_CKSEL_MOSC      (0x03)
#define R_SYSTEM_PRV_SCKSCR_CKSEL_SOSC      (0x04)
#define R_SYSTEM_PRV_SCKSCR_CKSEL_PLL       (0x05)

/* High-speed on-chip oscillator in Hz */
#if   (0 == SYSTEM_CFG_HOCO_FREQUENCY)
#define R_SYSTEM_PRV_HOCO_FREQUENCY_HZ      (24000000U)     /*!< HOCO-24MHz */
#elif (1 == SYSTEM_CFG_HOCO_FREQUENCY)
#define R_SYSTEM_PRV_HOCO_FREQUENCY_HZ      (32000000U)     /*!< HOCO-32MHz */
#elif (2 == SYSTEM_CFG_HOCO_FREQUENCY)
#define R_SYSTEM_PRV_HOCO_FREQUENCY_HZ      (48000000U)     /*!< HOCO-48MHz */
#else
#define R_SYSTEM_PRV_HOCO_FREQUENCY_HZ      (64000000U)     /*!< HOCO-64MHz */
#endif /* (0 == SYSTEM_CFG_HOCO_FREQUENCY) */

/* Medium-speed on-chip oscillator in Hz */
#define R_SYSTEM_PRV_MOCO_FREQUENCY_HZ      (2000000U)      /*!< MOCO-2MHz */

/* Low-speed on-chip oscillator in Hz */
#define R_SYSTEM_PRV_LOCO_FREQUENCY_HZ      (32768U)        /*!< LOCO-32.768kHz */

/* Main clock oscillator in Hz */
/* Refer to definition of SYSTEM_CFG_MOSC_FREQUENCY_HZ. */

/* Sub clock oscillator in Hz */
#define R_SYSTEM_PRV_SUBCLOCK_FREQUENCY_HZ  (32768U)        /*!< Subosc-32.768kHz */

/* PLL frequency in Hz */
#if (0 <= SYSTEM_CFG_PLL_DIV) && (3 >= SYSTEM_CFG_PLL_DIV)
    #define R_SYSTEM_PRV_PLL_DIV_FREQUENCY    (SYSTEM_CFG_MOSC_FREQUENCY_HZ / (SYSTEM_CFG_PLL_DIV+1))
#else
    #error "ERROR - SYSTEM_CFG_PLL_DIV value (config)"
#endif /* (0 <= SYSTEM_CFG_PLL_DIV) && (3 >= SYSTEM_CFG_PLL_DIV) */

#if   (0 == SYSTEM_CFG_PLL_MUL)
    #error "ERROR - SYSTEM_CFG_PLL_MUL value (config)"
#elif (1 <= SYSTEM_CFG_PLL_MUL) && (7 >= SYSTEM_CFG_PLL_MUL)
    #define R_SYSTEM_PRV_PLL_FREQUENCY_HZ        (R_SYSTEM_PRV_PLL_DIV_FREQUENCY*(SYSTEM_CFG_PLL_MUL+1))    /*!< PLL circuit */
#else
    #error "ERROR - SYSTEM_CFG_PLL_MUL value (config)"
#endif /* (0 == SYSTEM_CFG_PLL_MUL) */

/* PLL frequency range select(PLLCCR.FSEL0)
   0 = 32MHz - 48MHz
   1 = 48MHz - 64MHz */
#if   (64000000U < R_SYSTEM_PRV_PLL_FREQUENCY_HZ)
    #error "ERROR - PLL frequency (config)"
#elif (48000000U < R_SYSTEM_PRV_PLL_FREQUENCY_HZ)
    #define R_SYSTEM_PRV_PLL_RANGE            (1)
#elif (32000000U <= R_SYSTEM_PRV_PLL_FREQUENCY_HZ)
    #define R_SYSTEM_PRV_PLL_RANGE            (0)
#else
#if (1 == SYSTEM_CFG_PLL_ENABLE)
    #error "ERROR - PLL frequency (config)"
#else
    #define R_SYSTEM_PRV_PLL_RANGE          (0)
    #warning "WARNING - PLL frequency (config)"
#endif /* (1 == SYSTEM_CFG_PLL_ENABLE) */
#endif /* (64000000U < R_SYSTEM_PRV_PLL_FREQUENCY_HZ) */

/* Used High-speed on-chip oscillator for system clock */
#if   (0 == SYSTEM_CFG_CLOCK_SOURCE)
#define R_SYSTEM_PRV_CLOCK_SEL    (R_SYSTEM_PRV_HOCO_FREQUENCY_HZ)
/* Used Medium-speed on-chip oscillator for system clock */
#elif (1 == SYSTEM_CFG_CLOCK_SOURCE)
#define R_SYSTEM_PRV_CLOCK_SEL    (R_SYSTEM_PRV_MOCO_FREQUENCY_HZ)
/* Used Low-speed on-chip oscillator for system clock */
#elif (2 == SYSTEM_CFG_CLOCK_SOURCE)
#define R_SYSTEM_PRV_CLOCK_SEL    (R_SYSTEM_PRV_LOCO_FREQUENCY_HZ)
/* Used Main clock oscillator for system clock */
#elif (3 == SYSTEM_CFG_CLOCK_SOURCE)
#define R_SYSTEM_PRV_CLOCK_SEL    (SYSTEM_CFG_MOSC_FREQUENCY_HZ)
/* Used Sub clock oscillator for system clock */
#elif (4 == SYSTEM_CFG_CLOCK_SOURCE)
#define R_SYSTEM_PRV_CLOCK_SEL    (R_SYSTEM_PRV_SUBCLOCK_FREQUENCY_HZ)
/* Used PLL circuit for system clock */
#else
#define R_SYSTEM_PRV_CLOCK_SEL    (R_SYSTEM_PRV_PLL_FREQUENCY_HZ)
#endif /* (0 == SYSTEM_CFG_CLOCK_SOURCE) */

/* ICLK and PCLKA frequency division */
#if   (0 <= SYSTEM_CFG_ICK_PCKA_DIV) && (6 >= SYSTEM_CFG_ICK_PCKA_DIV)
    #define R_SYSTEM_PRV_CLOCK_ICK_PCKA   (R_SYSTEM_PRV_CLOCK_SEL / (1 << SYSTEM_CFG_ICK_PCKA_DIV))
#else
    #error "ERROR - ICLK and PCLKA frequency division (config)"
#endif /* (0 <= SYSTEM_CFG_ICK_PCKA_DIV) && (6 >= SYSTEM_CFG_ICK_PCKA_DIV) */

/* PCLKB frequency division */
#if   (0 <= SYSTEM_CFG_PCKB_DIV) && (6 >= SYSTEM_CFG_PCKB_DIV)
    #define R_SYSTEM_PRV_CLOCK_PCKB       (R_SYSTEM_PRV_CLOCK_SEL / (1 << SYSTEM_CFG_PCKB_DIV))
    #if (32000000U < R_SYSTEM_PRV_CLOCK_PCKB)
        #error "ERROR - It is necessary to set PLCKB to 32MHz or less (config)"
    #endif /* (32000000U < R_SYSTEM_PRV_CLOCK_PCKB) */
#else
    #error "ERROR - PCLKB frequency division (config)"
#endif /* (0 <= SYSTEM_CFG_PCKB_DIV) && (6 >= SYSTEM_CFG_PCKB_DIV) */

#define R_SYSTEM_PRV_DELAY_LOOP_CYCLES      (4)             /*! 4 cycles per loop for R_SYS_SoftwareDelay(). */

#if defined(__ICCARM__)
#define R_SYSTEM_PRV_ATTRIBUTE_STACKLESS               __stackless                  /* @suppress("Macro expansion") */
#define R_SYSTEM_PRV_ATTRIBUTE_STACKLESS_FOR_BOOSTFLAG __stackless                  /* @suppress("Macro expansion") */
#define R_SYSTEM_PRV_NON_VOLATILE                      __root                       /* @suppress("Macro expansion") */
#define R_SYSTEM_PRV_NON_VOLATILE_ATTRIBUTE
#elif defined(__GNUC__)
#define R_SYSTEM_PRV_ATTRIBUTE_STACKLESS               __attribute__((naked))       /* @suppress("Macro expansion") */
#define R_SYSTEM_PRV_ATTRIBUTE_STACKLESS_FOR_BOOSTFLAG                              /* @suppress("Macro expansion") */
#define R_SYSTEM_PRV_NON_VOLATILE
#define R_SYSTEM_PRV_NON_VOLATILE_ATTRIBUTE            __attribute__ ((__used__))   /* @suppress("Macro expansion") */
#else
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_INTERRUPTOPEN __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_INTERRUPTOPEN
#endif
#if (SYSTEM_CFG_SECTION_IEL0_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL1_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL2_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL3_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL4_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL5_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL6_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL7_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL8_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL9_IRQHANDLER  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL10_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL11_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL12_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL13_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL14_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL15_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL16_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL17_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL18_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL19_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL20_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL21_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL22_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL23_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL24_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL25_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL26_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL27_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL28_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL29_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL30_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_IEL31_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_IRQHANDLERCALL __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else /* @suppress("Conditional directive comment") */
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_IRQHANDLERCALL
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_RESOURCELOCKOPEN __attribute__ ((section(".ramfunc")))    /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_RESOURCELOCKOPEN
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_RESOURCELOCK == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_SOFTWARELOCK __attribute__ ((section(".ramfunc")))    /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_SOFTWARELOCK
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_BOOSTSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDMODESET       == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDMODESET        == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_32KHZSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKHOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKLOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKSOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKPLLSET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTOP  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTART    == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTOP     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_ENTERCRITICALSECTION   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_RESOURCELOCK           == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTENABLE  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTDISABLE == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_ENTERCRITICALSECTION __attribute__ ((section(".ramfunc")))    /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_ENTERCRITICALSECTION
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_BOOSTSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDMODESET       == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDMODESET        == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_32KHZSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKHOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKLOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKSOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKPLLSET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTOP  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTART    == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTOP     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_EXITCRITICALSECTION    == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_RESOURCELOCK           == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTENABLE  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTDISABLE == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_EXITCRITICALSECTION __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_EXITCRITICALSECTION
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_BOOSTSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDMODESET       == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDMODESET        == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_32KHZSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKHOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKLOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKSOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKPLLSET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTOP  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTART    == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTOP     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTENABLE  == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTENABLE __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTENABLE
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_BOOSTSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDMODESET       == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDMODESET        == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_32KHZSPEEDMODESET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKHOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKLOCOSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKSOSCSET     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKPLLSET      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTOP  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTART    == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTOP     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTART  == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTOP   == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTART     == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTDISABLE == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTDISABLE __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTDISABLE
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTOPEN __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTOPEN
#endif

#if (SYSTEM_CFG_SECTION_R_SYS_SOFTWAREDELAY == SYSTEM_SECTION_RAM_FUNC)
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_SOFTWARE_DELAYLOOP __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_SOFTWARE_DELAYLOOP
#endif

#if (SYSTEM_CFG_REGISTER_PROTECTION_ENABLE == 1)
#define R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(x)   (system_register_protect_enable(x))
#define R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(x)  (system_register_protect_disable(x))
#else
#define R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(x)
#define R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(x)
#endif /* (SYSTEM_CFG_REGISTER_PROTECTION_ENABLE == 1) */

#if (SYSTEM_CFG_ENTER_CRITICAL_SECTION_ENABLE == 1)
#define R_SYSTEM_PRV_ENTER_CRITICAL_SECTION   (system_enter_critical_section())
#define R_SYSTEM_PRV_EXIT_CRITICAL_SECTION    (system_exit_critical_section())
#else
#define R_SYSTEM_PRV_ENTER_CRITICAL_SECTION
#define R_SYSTEM_PRV_EXIT_CRITICAL_SECTION
#endif /* (SYSTEM_CFG_ENTER_CRITICAL_SECTION_ENABLE == 1) */

#if defined(__GNUC__)
#if (SYSTEM_CFG_SECTION_R_SYS_WDT_REFRESH == SYSTEM_SECTION_RAM_FUNC)
void r_system_wdt_refresh(void) __attribute__ ((weak, section(".ramfunc")));
#else
void r_system_wdt_refresh(void) __attribute__ ((weak));
#endif
#endif /* __GNUC__ */

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/** Lock structure. Passed into software locking functions such as R_SYS_SoftwareLock() and R_SYS_SoftwareUnLock. */
typedef struct st_system_lock
{
    uint8_t lock; /*!< A uint8_t is used instead of a enum because the size must be 8-bits. */
} st_system_lock_t;

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
/** Used for holding reference counters for protection bits */
static volatile uint16_t gs_protect_counters[SYSTEM_REG_PROTECT_TOTAL_ITEMS] = {0,0,0,0,0};

/** Masks for setting or clearing the PRCR register. Use -1 for size because PWPR in MPC is used differently. */
#if defined(__ICCARM__)
static uint16_t gs_prcr_masks[] =
#elif defined(__GNUC__)
__attribute__ ((section(".ramobj"))) static const uint16_t gs_prcr_masks[] =
#else
#error "unsupported compile"
#endif
{
    0x0001U,    /*!< PRC0 bit */
    0x0002U,    /*!< PRC1 bit */
    0x0008U,    /*!< PRC3 bit */
    0x0000U,    /*!< Dummy    */
    0x0010U,    /*!< PRC4 bit */
};

/** This macro defines a variable for saving previous mask value */
static uint32_t gs_old_mask_level = 0U;

/** Array of HW locks. */
static st_system_lock_t gs_system_locks[SYSTEM_LOCK_NUM];

/** FLag to prevent the calling more than once from application. */
static uint8_t gs_system_init_flg = 0;

/** Flag that holds transition to Boost mode. */
static bool gs_system_boost_flg = false;

/** This array holds callback functions. */
static void (* g_system_vectors[R_SYSTEM_PRV_IRQ_EVENT_NUMBER_TOTAL])(void);

static void system_interrupt_open(void) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_INTERRUPTOPEN;
static void system_irq_handler_call(IRQn_Type irq) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_IRQHANDLERCALL;
static void system_resource_lock_open(void) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_RESOURCELOCKOPEN;
static int32_t system_software_lock(st_system_lock_t * p_lock) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_SOFTWARELOCK;
static void system_enter_critical_section(void) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_ENTERCRITICALSECTION;
static void system_exit_critical_section(void) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_EXITCRITICALSECTION;
static void system_register_protect_enable(e_system_reg_protect_t regs_to_protect) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTENABLE;
static void system_register_protect_disable(e_system_reg_protect_t regs_to_unprotect) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTDISABLE;
static void system_register_protect_open(void) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_REGISTERPROTECTOPEN;
R_SYSTEM_PRV_ATTRIBUTE_STACKLESS static void software_delay_loop (uint32_t loop_cnt) R_SYSTEM_PRV_STATIC_FUNC_LOCATION_R_SYSTEM_SOFTWARE_DELAYLOOP;

/* Function Name : R_SYS_Initialize */
/*******************************************************************************************************************//**
 * @brief Initialize r_system function.
 * @note  Please call this API only once at the beginning of the main().
 **********************************************************************************************************************/
void R_SYS_Initialize(void) /* @suppress("Source line ordering") */
{
    if (0 == gs_system_init_flg)
    {
        gs_system_init_flg = 1;
        system_interrupt_open();
        system_resource_lock_open();
        system_register_protect_open();
        /* Boost mode check */
        if (SYSTEM_CFG_POWER_CONTROL_MODE == 0)
        {
            gs_system_boost_flg = true;
        }
    }
} /* End of function R_SYS_Initialize() */

/* Function Name : R_SYS_BoostSpeedModeSet */
/*******************************************************************************************************************//**
 * @brief The module shall support the BOOST speed mode (MOSC and PLL).
 * @param[in] None
 * @param[out] None
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_BoostSpeedModeSet(void)
{
    int32_t err = 0;

#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks PWSTF register if current power mode is ALLPWONM mode when entering into Boost-mode. 
       If not, return an error. */
    if (R_LPM_PowerSupplyModeGet() != LPM_POWER_SUPPLY_MODE_ALLPWON)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* If speed mode is High-speed mode when entering into Boost-mode. If not, return an error. */
    switch (R_SYS_SpeedModeGet())
    {
        case SYSTEM_SPEED_MODE_BOOST:
        {
            /* It is already in Boost mode. */
            ;
        }
        break;
        
        case SYSTEM_SPEED_MODE_HIGH:
        {
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the software standby mode. (step1) */
            SYSTEM->SBYCR_b.SSBYMP  = 0U;
            
            /* Set the software standby mode. (step2) */
            SYSTEM->SBYCR_b.SSBY    = 1U;
            
            /* Set the software standby mode. (step3) */
            SYSTEM->DPSBYCR_b.DPSBY = 0U;
            
            /* Disable the snooze mode. */
            SYSTEM->SNZCR_b.SNZE    = 0U;
            
            /* Transition from normal mode to Boost mode. */
            SYSTEM->PWSTCR = 0x05U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
            if(0x05U != SYSTEM->PWSTCR)
            {
                return -1;
            }
            
            /* Execute WFE instruction */
            __WFE();
            
            /* Wait the transition from normal mode to Boost mode. */
            while(1U != SYSTEM->PWSTF_b.BOOSTM)
            {
                __WFE();
            }
            
            /* Set the Boost mode flag. */
            r_sys_BoostFlagSet();
            
        }
        break;
        
        case SYSTEM_SPEED_MODE_LOW:
        case SYSTEM_SPEED_MODE_32KHZ:
        default:
        {
            err = -1;
        }
        break;
    }

    return err;
} /* End of function R_SYS_BoostSpeedModeSet() */

/* Function Name : R_SYS_HighSpeedModeSet */
/*******************************************************************************************************************//**
 * @brief The module shall support the High-Speed mode.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_HighSpeedModeSet(void) /* @suppress("Function length") */
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* In VBB mode, do not operate OPCCR register and SOPCCR register.  */
    if (R_LPM_BackBiasModeGet() != LPM_BACKBIAS_MODE_NORMAL)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    switch (R_SYS_SpeedModeGet())
    {
        case SYSTEM_SPEED_MODE_BOOST:
        {
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Excepting Subosc-speed mode. */
            SYSTEM->SOPCCR_b.SOPCM = 0U;
            
            /* Set the the transition to Normal High-speed mode. */
            SYSTEM->OPCCR_b.OPCM = 0U;
            
            /* Set the software standby mode. (step1) */
            SYSTEM->SBYCR_b.SSBYMP  = 0U;
            
            /* Set the software standby mode. (step2) */
            SYSTEM->SBYCR_b.SSBY    = 1U;
            
            /* Set the software standby mode. (step3) */
            SYSTEM->DPSBYCR_b.DPSBY = 0U;
            
            /* Disable the snooze mode. */
            SYSTEM->SNZCR_b.SNZE    = 0U;
            
            /* Transition from Boost mode to Normal mode. */
            SYSTEM->PWSTCR = 0x04U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
            if(0x04U != SYSTEM->PWSTCR)
            {
                return -1;
            }
            
            /* Execute WFE instruction */
            __WFE();
            
            /* Wait the transition from Boost mode to Normal mode. */
            while ((0U != SYSTEM->PWSTF_b.BOOSTM) || (0U != SYSTEM->PWSTF_b.VBBM))
            {
                __WFE();
            }
        }
        break;
        
        case SYSTEM_SPEED_MODE_HIGH:
        {
            /* It is already in Normal High-Speed mode. */
            ;   /* Do Nothing. */
        }
        break;
        
        case SYSTEM_SPEED_MODE_LOW:
        {
            
            /* Wait the transition to Normal Low-speed mode. */
            while (0U != SYSTEM->OPCCR_b.OPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal mode. */
            SYSTEM->PWSTCR = 0x00U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
            if(0x00U != SYSTEM->PWSTCR)
            {
                return -1;
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Excepting Subosc-speed mode. */
            SYSTEM->SOPCCR_b.SOPCM = 0U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Excepting Subosc-speed mode. */
            while (0U != SYSTEM->SOPCCR_b.SOPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal High-Speed mode. */
            SYSTEM->OPCCR_b.OPCM = 0U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Normal High-Speed mode. */
            while (0U != SYSTEM->OPCCR_b.OPCMTSF)
            {
                ;   /* wait */
            }
        }
        break;
        
        case SYSTEM_SPEED_MODE_32KHZ:
        {
            /* Wait the transition to Normal Subosc-speed mode. */
            while (0U != SYSTEM->SOPCCR_b.SOPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal mode. */
            SYSTEM->PWSTCR = 0x00U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
            if(0x00U != SYSTEM->PWSTCR)
            {
                return -1;
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Excepting Subosc-speed mode. */
            SYSTEM->SOPCCR_b.SOPCM = 0U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Excepting Subosc-speed mode. */
            while (0U != SYSTEM->SOPCCR_b.SOPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal High-Speed mode. */
            SYSTEM->OPCCR_b.OPCM = 0U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Normal High-Speed mode. */
            while (0U != SYSTEM->OPCCR_b.OPCMTSF)
            {
                ;
            }
        }
        break;
        
        default:
        {
            return -1;
        }
        break;
    }
    
    return 0;
    
} /* End of function R_SYS_HighSpeedModeSet() */

/* Function Name : R_SYS_LowSpeedModeSet */
/*******************************************************************************************************************//**
 * @brief The module shall support the Low-Speed mode.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_LowSpeedModeSet(void) /* @suppress("Function length") */
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* In VBB mode, do not operate OPCCR register and SOPCCR register.  */
    if (R_LPM_BackBiasModeGet() != LPM_BACKBIAS_MODE_NORMAL)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    switch (R_SYS_SpeedModeGet())
    {
        case SYSTEM_SPEED_MODE_HIGH:
        {
            /* Wait the transition to Normal High-Speed mode. */
            while (0U != SYSTEM->OPCCR_b.OPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal mode. */
            SYSTEM->PWSTCR = 0x00U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
            if(0x00U != SYSTEM->PWSTCR)
            {
                return -1;
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Excepting Subosc-speed mode. */
            SYSTEM->SOPCCR_b.SOPCM = 0U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Excepting Subosc-speed mode. */
            while (0x00U != SYSTEM->SOPCCR_b.SOPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal Low-Speed mode. */
            SYSTEM->OPCCR_b.OPCM = 3U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Normal Low-Speed mode. */
            while (0x00U != SYSTEM->OPCCR_b.OPCMTSF)
            {
                ;   /* wait */
            }
        }
        break;
        
        case SYSTEM_SPEED_MODE_32KHZ:
        {
            
            /* Wait the transition to Normal Subosc-speed mode. */
            while (0U != SYSTEM->SOPCCR_b.SOPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal mode. */
            SYSTEM->PWSTCR = 0x00U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
            if(0x00U != SYSTEM->PWSTCR)
            {
                return -1;
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Excepting Subosc-speed mode. */
            SYSTEM->SOPCCR_b.SOPCM = 0U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Excepting Subosc-speed mode. */
            while (0U != SYSTEM->SOPCCR_b.SOPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal Low-Speed mode. */
            SYSTEM->OPCCR_b.OPCM = 3U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Normal Low-Speed mode. */
            while (0U != SYSTEM->OPCCR_b.OPCMTSF)
            {
                ;   /* wait */
            }
        }
        break;
        
        case SYSTEM_SPEED_MODE_LOW:
        {
            /* It is already in Low-Speed mode. */
            /* Do Nothing. */
            ;
        }
        break;
        
        case SYSTEM_SPEED_MODE_BOOST:
        default:
        {
            return -1;
        }
        break;
    }
    
    return 0;
    
} /* End of function R_SYS_LowSpeedModeSet() */

/* Function Name : R_SYS_32kHzSpeedModeSet */
/*******************************************************************************************************************//**
 * @brief The module shall support the Subosc-Speed mode.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_32kHzSpeedModeSet(void) /* @suppress("Function length") */
{
    switch (R_SYS_SpeedModeGet())
    {
        case SYSTEM_SPEED_MODE_HIGH:
        case SYSTEM_SPEED_MODE_LOW:
        {
            /* Wait the transition to Normal mode. */
            while (0U != SYSTEM->OPCCR_b.OPCMTSF)
            {
                ;   /* wait */
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Normal mode. */
            SYSTEM->PWSTCR = 0x00U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
            if(0x00U != SYSTEM->PWSTCR)
            {
                return -1;
            }
            
            R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Set the the transition to Subosc-speed mode. */
            SYSTEM->SOPCCR_b.SOPCM = 1U;
            
            R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
            
            /* Wait the transition to Subosc-speed mode. */
            while (0x00U != SYSTEM->SOPCCR_b.SOPCMTSF)
            {
                ;   /* wait */
            }
        }
        break;
        
        case SYSTEM_SPEED_MODE_32KHZ:
        {
            /* In case of "VBB mode". */
            if (1U == SYSTEM->PWSTF_b.VBBM)
            {
                
                R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
                
                /* Set the the transition to Subosc-speed mode. */
                SYSTEM->SOPCCR_b.SOPCM = 1U;
                
                /* Set the software standby mode. (step1) */
                SYSTEM->SBYCR_b.SSBYMP  = 0U;
                
                /* Set the software standby mode. (step1) */
                SYSTEM->SBYCR_b.SSBY    = 1U;
                
                /* Set the software standby mode. (step1) */
                SYSTEM->DPSBYCR_b.DPSBY = 0U;
                
                /* Disable the snooze mode. */
                SYSTEM->SNZCR_b.SNZE    = 0U;
                
                /* Transition from VBB mode to Normal mode. */
                SYSTEM->PWSTCR = 0x04U;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
                
                /* Returns an error because the PWSTCR.PWST[2:0] bits could not be modified. */
                if(0x04U != SYSTEM->PWSTCR)
                {
                  return -1;
                }
                
                /* Execute WFE instruction */
                __WFE();
                
                /* Wait the transition from VBB mode to Normal mode. */
                while ((0U != SYSTEM->PWSTF_b.BOOSTM) || (0U != SYSTEM->PWSTF_b.VBBM))
                {
                    __WFE();
                }
            }
        }
        break;
        
        case SYSTEM_SPEED_MODE_BOOST:
        default:
        {
            return -1;
        }
        break;
    }
    
    return 0;
} /* End of function R_SYS_32kHzSpeedModeSet() */

/* Function Name : R_SYS_SpeedModeGet */
/*******************************************************************************************************************//**
 * @brief The module shall support reading current speed mode setting.
 * @retval  SYSTEM_SPEED_MODE_BOOST     Boost-mode
 * @retval  SYSTEM_SPEED_MODE_HIGH      High-speed mode
 * @retval  SYSTEM_SPEED_MODE_LOW       Low-speed mode
 * @retval  SYSTEM_SPEED_MODE_32KHZ     Subosc-speed mode or Low-leakage-current(VBB) mode
 **********************************************************************************************************************/
e_system_speed_mode_t R_SYS_SpeedModeGet(void)
{
    e_system_speed_mode_t speed_mode = SYSTEM_SPEED_MODE_BOOST;

    /* In case of "Boost mode". */
    if (1U == SYSTEM->PWSTF_b.BOOSTM)
    {
        speed_mode = SYSTEM_SPEED_MODE_BOOST;
    }
    else
    {
        /* In case of "Normal SubOsc-speed mode" or "VBB mode". */
        if ((1U == SYSTEM->SOPCCR_b.SOPCM) || (1U == SYSTEM->PWSTF_b.VBBM))
        {
            /* Return subosc-speed mode and Low-leakage-current(VBB) mode. */
            speed_mode = SYSTEM_SPEED_MODE_32KHZ;
        }
        else
        {
            /* In case of "Normal High-speed mode". */
            if (0x00U == SYSTEM->OPCCR_b.OPCM)
            {
                speed_mode = SYSTEM_SPEED_MODE_HIGH;
            }
            /* In case of "Normal Low-speed mode". */
            else
            {
                speed_mode = SYSTEM_SPEED_MODE_LOW;
            }
        }
    }

    return speed_mode;
} /* End of function R_SYS_SpeedModeGet() */

/* Function Name : R_SYS_SystemClockHOCOSet */
/*******************************************************************************************************************//**
 * @brief This module sets the system clock source to HOCO.
 * @pre   Make sure that bit0 of the return value of R_SYS_OscStabilizationFlagGet() is set to '1' (HOCOSF=1)
 *        before this API is called, unless there is enough time secured for the HOCO stabilization
 *        after the API call R_SYS_HighSpeedClockStart() is made.
 *        Please refer to the Hardware User's Manual for the HOCO stabilization time.
 * @retval  0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockHOCOSet(void) /* @suppress("Function length") */
{
    uint32_t freq_hz = 0;
    
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks HOCOCR register if HOCO operation.
       Also the module checks OSCSF register if HOCO is stable.
       If not, return an error. */
    if ((1U == SYSTEM->HOCOCR_b.HCSTP) || (0U == SYSTEM->OSCSF_b.HOCOSF))
    {
        return -1;
    }
    
    /* The module checks PWSTF register if speed mode is Boost-mode. If not, return an error. */
    if ((1U == SYSTEM->PWSTF_b.VBBM) || (1U == SYSTEM->SOPCCR_b.SOPCM))
    {
        return -1;
    }
    
    /* If the clock source exceeds 32MHz, PCLKB must be greater than 2 devide. */
    if ((SYSTEM_CFG_HOCO_FREQUENCY > 1) && (SYSTEM_CLOCK_DIV_1 == SYSTEM->SCKDIVCR_b.PCKB))
    {
        return -1;
    }
    
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    if (R_SYS_SystemClockFreqGet(&freq_hz) == 0)
    {
        if (32000000U >= freq_hz)
        {
            /* In case of the HOCO frequency is over 32MHz. */
            if (32000000U < (R_SYSTEM_PRV_HOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK)))
            {
                /* In case of "Boost mode". */
                if (1U == SYSTEM->PWSTF_b.BOOSTM)
                {
                    /* If the frequency exceeds 32 MHz, change the flash wait cycle to 1 wait. */
                    FLASH->FLWT = 1;
                    
                    R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                    
                    /* Set the system clock source. */
                    SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO;
                    
                    R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
                }
                else
                {
                    /* If the frequency exceeds 32 MHz, it is necessary change to the Boost mode.
                       If not, return an error. */
                    return -1;
                }
            }
            else
            {
                /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
                FLASH->FLWT = 0;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* Set the system clock source. */
                SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
            }
        }
        else
        {
            /* In case of the HOCO frequency is over 32MHz. */
            if (32000000U < (R_SYSTEM_PRV_HOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK)))
            {
                R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* Set the system clock source. */
                SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
            }
            else
            {
                R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* Set the system clock source. */
                SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
                FLASH->FLWT = 0;
            }
        }
    }
    else
    {
        return -1;
    }
    
    return 0;
    
} /* End of function R_SYS_SystemClockHOCOSet() */

/* Function Name : R_SYS_SystemClockMOCOSet */
/*******************************************************************************************************************//**
 * @brief This module sets the system clock source to MOCO.
 * @pre   To use the MOCO clock as the system clock, 
 *        the MOCO clock oscillation stabilization wait time (tMOCOWT) is required after starting the MOCO clock.
 *        Use this API after the MOCO clock oscillation stabilization wait time (tMOCOWT) has elapsed.
 *        Please refer to the Hardware User's Manual for the MOCO clock oscillattion stabilization wait time(tMOCOWT).
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockMOCOSet(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks MOCOCR register if MOCO operation.
       If not, return an error. */
    if (1U == SYSTEM->MOCOCR_b.MCSTP)
    {
        /* It is necessary to call R_SYS_MainOscSpeedClockStart(). */
        return -1;
    }
    
    /* The module checks PWSTF register if speed mode is VBB-mode. If not, return an error. */
    if ((1U == SYSTEM->PWSTF_b.VBBM) || (1U == SYSTEM->SOPCCR_b.SOPCM))
    {
        /* It is necessary to call R_SYS_BoostSpeedModeSet(). */
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* Set the system clock source. */
    SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_MOCO;
    
    R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
    FLASH->FLWT = 0;
    
    return 0;
} /* End of function R_SYS_SystemClockMOCOSet() */

/* Function Name : R_SYS_SystemClockLOCOSet */
/*******************************************************************************************************************//**
 * @brief This module sets the system clock source to LOCO.
 * @pre   To use the LOCO clock as the system clock, 
 *        the LOCO clock oscillation stabilization wait time (tLOCOWT) is required after starting the LOCO clock.
 *        Use this API after the LOCO clock oscillation stabilization wait time (tLOCOWT) has elapsed.
 *        Please refer to the Hardware User's Manual for the LOCO clock oscillattion stabilization wait time(tLOCOWT).
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockLOCOSet(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks LOCOCR register if LOCO operation.
       If not, return an error. */
    if (1U == SYSTEM->LOCOCR_b.LCSTP)
    {
        /* It is necessary to call R_SYS_MainOscSpeedClockStart(). */
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* Set the system clock source. */
    SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_LOCO;
    
    R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
    FLASH->FLWT = 0;
    
    return 0;
} /* End of function R_SYS_SystemClockLOCOSet() */

/* Function Name : R_SYS_SystemClockMOSCSet */
/*******************************************************************************************************************//**
 * @brief This module sets the system clock source to MOSC.
 * @pre   Make sure that bit3 of the return value of R_SYS_OscStabilizationFlagGet() is set to '1' (MOSCSF=1)
 *        before this API is called, unless there is enough time secured for the main clock oscillator stabilization
 *        after the API call R_SYS_MainOscSpeedClockStart() is made.
 *        Please refer to the Hardware User's Manual for the main clock oscillator stabilization time.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockMOSCSet(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks MOSCCR register if main clock operation.
       Also the module checks OSCSF register if main clock oscillation is stable.
       If not, return an error. */
    if ((1U == SYSTEM->MOSCCR_b.MOSTP) || (0U == SYSTEM->OSCSF_b.MOSCSF))
    {
        /* It is necessary to call R_SYS_MainOscSpeedClockStart(). */
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* Set the system clock source. */
    SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_MOSC;
    
    R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
    FLASH->FLWT = 0;
    
    return 0;
} /* End of function R_SYS_SystemClockMOSCSet() */

/* Function Name : R_SYS_SystemClockSOSCSet */
/*******************************************************************************************************************//**
 * @brief This module sets the system clock source to SOSC.
 * @pre   To use the sub clock as the system clock, 
 *        the sub clock oscillator stabilization wait time (tSUBOSCOWT) is required after starting the sub clock.
 *        Use this API after the sub clock oscillator stabilization wait time (tSUBOSCOWT) has elapsed.
 *        Please refer to the Hardware User's Manual for the sub clock oscillator stabilization wait time(tSUBOSCOWT).
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockSOSCSet(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks SOSCCR register if sub clock operation.
       If not, return an error. */
    if (1U == SYSTEM->SOSCCR_b.SOSTP)
    {
        /* It is necessary to call R_SYS_SubOscSpeedClockStart(). */
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* Set the system clock source. */
    SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_SOSC;
    
    R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
    
    /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
    FLASH->FLWT = 0;
    
    return 0;
} /* End of function R_SYS_SystemClockSOSCSet() */

/* Function Name : R_SYS_SystemClockPLLSet */
/*******************************************************************************************************************//**
 * @brief This module sets the system clock source to PLL.
 * @pre   Make sure that bit5 of the return value of R_SYS_OscStabilizationFlagGet() is set to '1' (PLLSF=1)
 *        before this API is called, unless there is enough time secured for the PLL clock oscillation stabilization
 *        after the API call R_SYS_PLLSpeedClockStart() is made.
 *        Please refer to the Hardware User's Manual for the PLL clock oscillation stabilization time.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockPLLSet(void) /* @suppress("Function length") */
{
    uint32_t freq_hz = 0;
    
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks PLLCR register if PLL clock operation.
       Also the module checks OSCSF register if PLL clock oscillation is stable.
       If not, return an error. */
    if ((1U == SYSTEM->PLLCR_b.PLLSTP) || (0U == SYSTEM->OSCSF_b.PLLSF))
    {
        /* It is necessary to call R_SYS_MainOscSpeedClockStart(). */
        return -1;
    }
    
    /* The module checks PWSTF register if speed mode is Boost-mode. If not, return an error. */
    if (0U == SYSTEM->PWSTF_b.BOOSTM)
    {
        /* It is necessary to call R_SYS_BoostSpeedModeSet(). */
        return -1;
    }
    
    /* If the clock source exceeds 32MHz, PCLKB must be greater than 2 devide. */
    if ((32000000U < R_SYSTEM_PRV_PLL_FREQUENCY_HZ) && (SYSTEM_CLOCK_DIV_1 == SYSTEM->SCKDIVCR_b.PCKB))
    {
        return -1;
    }
    
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    if (R_SYS_SystemClockFreqGet(&freq_hz) == 0)
    {
        if (32000000U >= freq_hz)
        {
            /* In case of the PLL frequency is over 32MHz. */
            if (32000000U < (R_SYSTEM_PRV_PLL_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK)))
            {
                /* In case of "Boost mode". */
                if (1U == SYSTEM->PWSTF_b.BOOSTM)
                {
                    /* If the frequency exceeds 32 MHz, change the flash wait cycle to 1 wait. */
                    FLASH->FLWT = 1;
                    
                    R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                    
                    /* Set the system clock source. */
                    SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_PLL;
                    
                    R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
                }
                else
                {
                    /* If the frequency exceeds 32 MHz, it is necessary change to the Boost mode.
                       If not, return an error. */
                    return -1;
                }
            }
            else
            {
                /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
                FLASH->FLWT = 0;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* Set the system clock source. */
                SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_PLL;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
            }
        }
        else
        {
            /* In case of the PLL frequency is over 32MHz. */
            if (32000000U < (R_SYSTEM_PRV_PLL_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK)))
            {
                R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* Set the system clock source. */
                SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_PLL;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
            }
            else
            {
                R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* Set the system clock source. */
                SYSTEM->SCKSCR = R_SYSTEM_PRV_SCKSCR_CKSEL_PLL;
                
                R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
                
                /* If the frequency is 32 MHz or less, change the flash wait cycle to 0 wait. */
                FLASH->FLWT = 0;
            }
        }
    }
    else
    {
        return -1;
    }
    
    return 0;
    
} /* End of function R_SYS_SystemClockPLLSet() */

/* Function Name : R_SYS_SystemClockFreqGet */
/*******************************************************************************************************************//**
 * @brief The module reads ICLK(PCLKA) frequency in Hz.
 * @param[out] *p_freq_hz   Current frequency (over the argument p_freq_hz).
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockFreqGet(uint32_t * p_freq_hz)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* In the case of argument is NULL, return an error. */
    if (NULL == p_freq_hz)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* Get the system clock source frequency. */
    switch (SYSTEM->SCKSCR & R_SYSTEM_PRV_SCKSCR_CKSEL_MSK)
    {
        case R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO:
        {
            /* Get the ICLK/PCLKA frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_HOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_MOCO:
        {
            /* Get the ICLK/PCLKA frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_MOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_LOCO:
        {
            /* Get the ICLK/PCLKA frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_LOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_MOSC:
        {
            /* Get the ICLK/PCLKA frequency. */
            *p_freq_hz = (SYSTEM_CFG_MOSC_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_SOSC:
        {
            /* Get the ICLK/PCLKA frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_SUBCLOCK_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_PLL:
        {
            /* Get the ICLK/PCLKA frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_PLL_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.ICK));
        }
        break;
        default:
        {
            return -1;
        }
        break;
    }

    return 0;
} /* End of function R_SYS_SystemClockFreqGet() */

/* Function Name : R_SYS_PeripheralClockFreqGet */
/*******************************************************************************************************************//**
 * @brief The module reads PCLKB frequency in Hz.
 * @param[out] *p_freq_hz   Current frequency (over the argument p_freq_hz).
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_PeripheralClockFreqGet(uint32_t * p_freq_hz)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* In the case of argument is NULL, return an error. */
    if (NULL == p_freq_hz)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* Get the system clock source frequency. */
    switch (SYSTEM->SCKSCR & R_SYSTEM_PRV_SCKSCR_CKSEL_MSK)
    {
        case R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO:
        {
            /* Get the PCLKB frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_HOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.PCKB));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_MOCO:
        {
            /* Get the PCLKB frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_MOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.PCKB));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_LOCO:
        {
            /* Get the PCLKB frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_LOCO_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.PCKB));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_MOSC:
        {
            /* Get the PCLKB frequency. */
            *p_freq_hz = (SYSTEM_CFG_MOSC_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.PCKB));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_SOSC:
        {
            /* Get the PCLKB frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_SUBCLOCK_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.PCKB));
        }
        break;
        case R_SYSTEM_PRV_SCKSCR_CKSEL_PLL:
        {
            /* Get the PCLKB frequency. */
            *p_freq_hz = (R_SYSTEM_PRV_PLL_FREQUENCY_HZ / (1 << SYSTEM->SCKDIVCR_b.PCKB));
        }
        break;
        default:
        {
            return -1;
        }
        break;
    }

    return 0;

} /* End of function R_SYS_PeripheralClockFreqGet() */

/* Function Name : R_SYS_SystemClockDividerSet */
/*******************************************************************************************************************//**
 * @brief The module sets system clock divider.
 * @param[in] iclk_div
 * @param[in] pclkb_div
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SystemClockDividerSet(e_system_sys_clock_div_t iclk_div, e_system_sys_clock_div_t pclkb_div) /* @suppress("Function length") */
{
    uint8_t iclk_div_set  = iclk_div;
    uint8_t pclkb_div_set = pclkb_div;
    
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    /* If "iclk_div" is out of range, return an error. */
    if (SYSTEM_CLOCK_NONE_CHANGE < iclk_div)
    {
        return -1;
    }
    
    /* If "pclkb_div" is out of range, return an error. */
    if (SYSTEM_CLOCK_NONE_CHANGE < pclkb_div)
    {
        return -1;
    }
    
    /* If "iclk_div" is greater than "pclkb_div", return an error. */
    if ((SYSTEM_CLOCK_NONE_CHANGE > iclk_div) && (SYSTEM_CLOCK_NONE_CHANGE > pclkb_div))
    {
        if (iclk_div > pclkb_div)
        {
            return -1;
        }
    }
    
    /* If current setting divider of ICLK/PCLKA is greater than "pclkb_div", return an error. */
    if ((SYSTEM_CLOCK_NONE_CHANGE == iclk_div) && (pclkb_div < SYSTEM->SCKDIVCR_b.ICK))
    {
        return -1;
    }
    
    /* If "iclk_div" is greater than current setting divider of PCLKB, return an error. */
    if (SYSTEM_CLOCK_NONE_CHANGE == pclkb_div)
    {
        /* Check only when "iclk_div" is SYSTEM_CLOCK_NONE_CHANGE. */
        if ((SYSTEM_CLOCK_NONE_CHANGE != iclk_div) && (iclk_div > SYSTEM->SCKDIVCR_b.PCKB))
        {
            return -1;
        }
    }
    
    /* PCLKB: Max 32MHz */
    /* If the clock source exceeds 32MHz, PCLKB must be greater than 2 devide. */
    if (SYSTEM_CLOCK_DIV_1 == pclkb_div)
    {
        /* Get the system clock source. */
        switch (SYSTEM->SCKSCR & R_SYSTEM_PRV_SCKSCR_CKSEL_MSK)
        {
            case R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO:
            {
                if (1 < SYSTEM_CFG_HOCO_FREQUENCY)
                {
                    return -1;
                }
            }
            break;
            case R_SYSTEM_PRV_SCKSCR_CKSEL_PLL:
            {
                if (32000000U < R_SYSTEM_PRV_PLL_FREQUENCY_HZ)
                {
                    return -1;
                }
            }
            break;
            default:
            {
                /* Do Nothing. */
                ;
            }
            break;
        }
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /* In case of the argument is different from the current setting divider. */
    if ((iclk_div != SYSTEM->SCKDIVCR_b.ICK) && (SYSTEM_CLOCK_NONE_CHANGE == iclk_div))
    {
        /* Get the current setting divider. */
        iclk_div_set = SYSTEM->SCKDIVCR_b.ICK;
    }
    
    /* In case of the argument is different from the current setting divider. */
    if ((pclkb_div != SYSTEM->SCKDIVCR_b.PCKB) && (SYSTEM_CLOCK_NONE_CHANGE == pclkb_div))
    {
        /* Get the current setting divider. */
        pclkb_div_set = SYSTEM->SCKDIVCR_b.PCKB;
    }
    
    /* In case of arguments are same as the current setting dividers. */
    if ((SYSTEM->SCKDIVCR_b.ICK == iclk_div_set) && (SYSTEM->SCKDIVCR_b.PCKB == pclkb_div_set))
    {
        /* Do Nothing. */
        ;
    }
    else
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*--------------------------------------------------------------------------
         * Set the system clock division
         * SCKDIVCR register
         *  b7- b0 : [Reserved] Set the 0
         * b10- b8 : [    PCKB] Peripheral module clock B (PCLKB) select
         * b23-b11 : [Reserved] Set the 0
         * b26-b24 : [     ICK] System clock (ICLK) / PCLKA select
         * b31-b27 : [Reserved] Set the 0
         *------------------------------------------------------------------------*/
        SYSTEM->SCKDIVCR = (uint32_t)(( iclk_div_set << 0x18U) |
                                      (pclkb_div_set << 0x08U));
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
    }
    
    return 0;

} /* End of function R_SYS_SystemClockDividerSet() */

/* Function Name : R_SYS_MainOscSpeedClockStart */
/*******************************************************************************************************************//**
 * @brief This module shall support starting the main clock oscillator operation.@n
 *        In this API, main clock oscillation stabilization wait is not executed after main clock oscillation starts.
 *        It is necessary to wait until oscillation stabilizes with user application.@n
 *        Check the return value of the main clock oscillation stabilization wait flag 
 *        using R_SYS_OscStabilizationFlagGet().
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
void R_SYS_MainOscSpeedClockStart(void)
{
    /* The module checks MOSCCR register if main clock not operation.
       Also the module checks OSCSF register if main clock oscillation is stopped.
       If not, do nothing. */
    if ((1U == SYSTEM->MOSCCR_b.MOSTP) && (0U == SYSTEM->OSCSF_b.MOSCSF))
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Set the main clock oscillator operation */
        SYSTEM->MOMCR = (uint8_t) ((SYSTEM_CFG_MOSC_LOW_POWER_ENABLE << 0x07U) |
                                   (    SYSTEM_CFG_MOSC_CLOCK_SOURCE << 0x06U) |
                                   (           SYSTEM_CFG_MOSC_DRIVE << 0x03U));
        
        /* Set the main clock oscillator wait time. */
        SYSTEM->MOSCWTCR = (uint8_t)(SYSTEM_CFG_MOSC_WAIT_TIME);
        
        /* Start the main clock oscillator operation. */
        SYSTEM->MOSCCR_b.MOSTP = 0U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(0U != SYSTEM->MOSCCR_b.MOSTP)
        {
            ;   /* wait */
        }
        
    }

} /* End of function R_SYS_MainOscSpeedClockStart() */

/* Function Name : R_SYS_MainOscSpeedClockStop */
/*******************************************************************************************************************//**
 * @brief This module shall support stopping the main clock oscillator operation.@n
 *        In this API, main clock stop wait is not executed after the main clock oscillation stops.
 *        It is necessary to wait until the oscillation stops in the user application.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_MainOscSpeedClockStop(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks OSCSF register if main clock oscillation stable state. If not, return an error. */
    if (0U == SYSTEM->OSCSF_b.MOSCSF)
    {
        /* It is necessary to confirm using the R_SYS_OscStabilizationFlagGet()
           that the main clock is in the stable oscillation state. (OSCSF.MOSCSF = 1) */
        return -1;
    }
    
    /* If the system clock source is main clock or PLL clock oscillation, do not stop the main clock. */
    if ((R_SYSTEM_PRV_SCKSCR_CKSEL_MOSC == SYSTEM->SCKSCR_b.CKSEL) ||
        (R_SYSTEM_PRV_SCKSCR_CKSEL_PLL  == SYSTEM->SCKSCR_b.CKSEL))
    {
        /* It is used as a system clock source and can not be stopped. */
        return -1;
    }
    
    /* If PLL clock is operating, do not stop the main clock. */
    if (0U == SYSTEM->PLLCR_b.PLLSTP)
    {
        /* It is necessary to stop the PLL clock using R_SYS_PLLSpeedClockStop(). */
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* The module checks MOSCCR register if main clock operation. If not, do nothing. */
    if (0U == SYSTEM->MOSCCR_b.MOSTP)
    {
        
        /* Wait the main clock oscillation stabilization. */
        while(0U == SYSTEM->OSCSF_b.MOSCSF)
        {
            ;   /* wait */
        }
        
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Start the main clock oscillator operation. */
        SYSTEM->MOSCCR_b.MOSTP = 1U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(1U != SYSTEM->MOSCCR_b.MOSTP)
        {
            ;   /* wait */
        }
        
    }

    return 0;
} /* End of function R_SYS_MainOscSpeedClockStop() */

/* Function Name : R_SYS_HighSpeedClockStart */
/*******************************************************************************************************************//**
 * @brief  This module shall support starting the High-speed on-chip oscillator operation.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_HighSpeedClockStart(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    /* If it is not in Boost OPE, do not set 48MHz or 64MHz. */
    if ((0U == SYSTEM->PWSTF_b.BOOSTM) && (1U < SYSTEM_CFG_HOCO_FREQUENCY))
    {
        return -1;
    }
    
    /* The module checks PWSTF register if speed mode is not VBB OPE. If not, return an error. */
    if ((1U == SYSTEM->PWSTF_b.VBBM) || (1U == SYSTEM->SOPCCR_b.SOPCM))
    {
        return -1;
    }
    
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /* Execute only when HOCO is stopped.  */
    if((1U == SYSTEM->HOCOCR_b.HCSTP) && (0U == SYSTEM->OSCSF_b.HOCOSF))
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*----------------------------------------------------------------------
         * Set the HOCO mode control
         * HOCOMCR register
         * b1-b0 : [   HCFRQ] HOCO oscillation frequency setting
         *                    - [SYSTEM_CFG_HOCO_FREQUENCY] User config
         * b7-b2 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->HOCOMCR = (uint8_t) (SYSTEM_CFG_HOCO_FREQUENCY);
        
        /*----------------------------------------------------------------------
         * Start the HOCO operation
         * HOCOCR register
         *    b0 : [   HCSTP] HOCO stop control
         *                    - [0] HOCO operation start
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->HOCOCR_b.HCSTP = 0U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(0U != SYSTEM->HOCOCR_b.HCSTP)
        {
            ;   /* wait */
        }
        
    }
    
    return 0;
    
} /* End of function R_SYS_HighSpeedClockStart() */

/* Function Name : R_SYS_HighSpeedClockStop */
/*******************************************************************************************************************//**
 * @brief  This module shall support stopping the High-speed on-chip oscillator operation.
 * @retval 0 Success
 * @retval -1 Error
 * @note   When stopping HOCO, set HOCOMCR register to 0x01(32MHz).
 *         Because 48MHz or 64MHz setting is prohibited except in Boost-mode.
 **********************************************************************************************************************/
int32_t R_SYS_HighSpeedClockStop(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    /* If HOCO is started and selected clock source, this API returns an error. */
    if ((0U == SYSTEM->HOCOCR_b.HCSTP) && (R_SYSTEM_PRV_SCKSCR_CKSEL_HOCO == SYSTEM->SCKSCR_b.CKSEL))
    {
        return -1;
    }
    
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /*--------------------------------------------------------------------------
     * In the case of HOCO is operating
     *------------------------------------------------------------------------*/
    if (0U == SYSTEM->HOCOCR_b.HCSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*----------------------------------------------------------------------
         * Stop the HOCO operation
         * HOCOCR register
         *    b0 : [   HCSTP] HOCO stop control
         *                    - [1] HOCO stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->HOCOCR_b.HCSTP = 1U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(1U != SYSTEM->HOCOCR_b.HCSTP)
        {
            ;   /* wait */
        }
        
    }
    
    R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
    
    /*----------------------------------------------------------------------
     * Set the HOCO mode control
     * HOCOMCR register
     * b1-b0 : [   HCFRQ] HOCO oscillation frequency setting
     *                    - [SYSTEM_CFG_HOCO_FREQUENCY] User config
     * b7-b2 : [Reserved] Set the 0
     *--------------------------------------------------------------------*/
    SYSTEM->HOCOMCR = 0x01U;
    
    R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
    
    return 0;
    
} /* End of function R_SYS_HighSpeedClockStop() */

/* Function Name : R_SYS_MediumSpeedClockStart */
/*******************************************************************************************************************//**
 * @brief  This module shall support starting the Medium-speed on-chip oscillator operation.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_MediumSpeedClockStart(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    /* If it is in VBB OPE and in SOSC-speed mode, do not start MOCO. */
    if ((1U == SYSTEM->PWSTF_b.VBBM) || (1U == SYSTEM->SOPCCR_b.SOPCM))
    {
        return -1;
    }
    
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /*--------------------------------------------------------------------------
     * In the case of MOCO is stop
     *------------------------------------------------------------------------*/
    if(1U == SYSTEM->MOCOCR_b.MCSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*----------------------------------------------------------------------
         * Start the MOCO operation
         * MOCOCR register
         *    b0 : [   MCSTP] MOCO stop control
         *                    - [0] MOCO operation start
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->MOCOCR_b.MCSTP = 0U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(0U != SYSTEM->MOCOCR_b.MCSTP)
        {
            ;   /* wait */
        }
    }
    
    return 0;
    
} /* End of function R_SYS_MediumSpeedClockStart() */

/* Function Name : R_SYS_MediumSpeedClockStop */
/*******************************************************************************************************************//**
 * @brief  This module shall support stopping the Medium-speed on-chip oscillator operation.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_MediumSpeedClockStop(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    /* If clock source is MOCO, do not stop the MOCO. */
    if (R_SYSTEM_PRV_SCKSCR_CKSEL_MOCO == SYSTEM->SCKSCR_b.CKSEL)
    {
        return -1;
    }
    
#ifndef CHIP_VERSION_WS1
    /* While the oscillation stop detection function is enabled, do not MOCO stop. */
    if (1U == SYSTEM->OSTDCR_b.OSTDE)
    {
        return -1;
    }
#endif /* CHIP_VERSION_WS1 */

#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /*--------------------------------------------------------------------------
     * In the case of MOCO is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSTEM->MOCOCR_b.MCSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*----------------------------------------------------------------------
         * Stop the MOCO operation
         * MOCOCR register
         *    b0 : [   MCSTP] MOCO stop control
         *                    - [1] MOCO stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->MOCOCR_b.MCSTP = 1U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(1U != SYSTEM->MOCOCR_b.MCSTP)
        {
            ;   /* wait */
        }
    }
    
    return 0;
    
} /* End of function R_SYS_MediumSpeedClockStop() */

/* Function Name : R_SYS_LowSpeedClockStart */
/*******************************************************************************************************************//**
 * @brief  This module shall support starting the Low-speed on-chip oscillator operation.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
void R_SYS_LowSpeedClockStart(void)
{
    /*--------------------------------------------------------------------------
     * In the case of LOCO is stop
     *------------------------------------------------------------------------*/
    if(1U == SYSTEM->LOCOCR_b.LCSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*----------------------------------------------------------------------
         * Stop the LOCO operation
         * LOCOCR register
         *    b0 : [   LCSTP] LOCO stop control
         *                    - [0] LOCO operation start
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->LOCOCR_b.LCSTP = 0U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(0U != SYSTEM->LOCOCR_b.LCSTP)
        {
            ;   /* wait */
        }
    }
    
} /* End of function R_SYS_LowSpeedClockStart() */

/* Function Name : R_SYS_LowSpeedClockStop */
/*******************************************************************************************************************//**
 * @brief  This module shall support stopping the Low-speed on-chip oscillator operation.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_LowSpeedClockStop(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    /* If clock source is LOCO, do not stop the LOCO. */
    if (R_SYSTEM_PRV_SCKSCR_CKSEL_LOCO == SYSTEM->SCKSCR_b.CKSEL)
    {
        return -1;
    }
    
    /* In VBB mode and VBB clock source is LOCO, do not stop LOCO. */
    if ((1U == SYSTEM->VBBCR_b.VBBEN) && (0U == SYSTEM->VBBCR_b.CLKSEL))
    {
        return -1;
    }
    
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /*--------------------------------------------------------------------------
     * In the case of LOCO is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSTEM->LOCOCR_b.LCSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*----------------------------------------------------------------------
         * Stop the LOCO operation
         * LOCOCR register
         *    b0 : [   LCSTP] LOCO stop control
         *                    - [1] LOCO stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->LOCOCR_b.LCSTP = 1U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(1U != SYSTEM->LOCOCR_b.LCSTP)
        {
            ;   /* wait */
        }
    }
    
    return 0;
    
} /* End of function R_SYS_LowSpeedClockStop() */

/* Function Name : R_SYS_SubOscSpeedClockStart */
/*******************************************************************************************************************//**
 * @brief  This module shall support starting the sub-clock oscillator operation.
 **********************************************************************************************************************/
void R_SYS_SubOscSpeedClockStart(void)
{
    /*--------------------------------------------------------------------------
     * In the case of SOSC is operating
     *------------------------------------------------------------------------*/
    if(1U == SYSTEM->SOSCCR_b.SOSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
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
        SYSTEM->SOMCR = (uint8_t) ((SYSTEM_CFG_SOSC_NF_STOP << 0x04U) |
                                   (SYSTEM_CFG_SOSC_DRIVE           ));
        
        /*----------------------------------------------------------------------
         * Start the SOSC operation
         * SOSCCR register
         *    b0 : [   SOSTP] SOSC stop control
         *                    - [0] SOSC operation start
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->SOSCCR_b.SOSTP = 0U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(0U != SYSTEM->SOSCCR_b.SOSTP)
        {
            ;   /* wait */
        }
    }
    
} /* End of function R_SYS_SubOscSpeedClockStart() */

/* Function Name : R_SYS_SubOscSpeedClockStop */
/*******************************************************************************************************************//**
 * @brief  This module shall support stopping the sub-clock oscillator operation.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_SubOscSpeedClockStop(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    /* If clock source is SOSC, do not stop SOSC. */
    if (R_SYSTEM_PRV_SCKSCR_CKSEL_SOSC == SYSTEM->SCKSCR_b.CKSEL)
    {
        return -1;
    }
    
    /* If it is in VBB OPE and selected SOSC, do not stop SOSC. */
    if ((1U == SYSTEM->VBBCR_b.VBBEN) && (1U == SYSTEM->VBBCR_b.CLKSEL))
    {
        return -1;
    }
    
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /*--------------------------------------------------------------------------
     * In the case of SOSC is operating
     *------------------------------------------------------------------------*/
    if(0U == SYSTEM->SOSCCR_b.SOSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /*----------------------------------------------------------------------
         * Stop the SOSC operation
         * SOSCCR register
         *    b0 : [   SOSTP] SOSC stop control
         *                      - [1] SOSC stop
         * b7-b1 : [Reserved] Set the 0
         *--------------------------------------------------------------------*/
        SYSTEM->SOSCCR_b.SOSTP = 1U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(1U != SYSTEM->SOSCCR_b.SOSTP)
        {
            ;   /* wait */
        }
    }
    
    return 0;
    
} /* End of function R_SYS_SubOscSpeedClockStop() */

/* Function Name : R_SYS_PLLSpeedClockStart */
/*******************************************************************************************************************//**
 * @brief This module shall support starting the PLL circuit operation.@n
 *        In this API, PLL clock oscillation stabilization wait is not executed after PLL clock oscillation starts.
 *        It is necessary to wait until oscillation stabilizes with user application.@n
 *        Check the return value of the PLL clock oscillation stabilization wait flag 
 *        using R_SYS_OscStabilizationFlagGet().
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_PLLSpeedClockStart(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks MOSCCR register if main clock operation.
       Also the module checks OSCSF register if main clock oscillation is stable.
       If not, return an error. */
    if ((1U == SYSTEM->MOSCCR_b.MOSTP) || (0U == SYSTEM->OSCSF_b.MOSCSF))
    {
        /* It is necessary to call R_SYS_MainOscSpeedClockStart(). */
        return -1;
    }
    
    /* The module checks PWSTF register if speed mode is Boost-mode. If not, return an error. */
    if (0U == SYSTEM->PWSTF_b.BOOSTM)
    {
        /* It is necessary to call R_SYS_BoostSpeedModeSet(). */
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* The module checks PLLCR register if PLL is not operating. If not, do nothing. */
    if (1U == SYSTEM->PLLCR_b.PLLSTP)
    {
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Set the PLL circuit clock control */
        SYSTEM->PLLCCR = (uint16_t) ((R_SYSTEM_PRV_PLL_RANGE   << 0x0EU) |
                                     (SYSTEM_CFG_PLL_MUL << 0x08U) |
                                     (SYSTEM_CFG_PLL_DIV         ));
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* After setting of PLLCCR is completed, it is necessary to elapse more than 1us. the module wait 2us. */
        R_SYS_SoftwareDelay(2, SYSTEM_DELAY_UNITS_MICROSECONDS);
        
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Start the PLL circuit operation. */
        SYSTEM->PLLCR_b.PLLSTP = 0U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(0U != SYSTEM->PLLCR_b.PLLSTP)
        {
            ;   /* wait */
        }
    }
    
    return 0;
    
} /* End of function R_SYS_PLLSpeedClockStart() */

/* Function Name : R_SYS_PLLSpeedClockStop */
/*******************************************************************************************************************//**
 * @brief This module shall support stopping the PLL circuit operation.@n
 *        In this API, PLL clock stop wait is not executed after the PLL clock oscillation stops.
 *        It is necessary to wait until the oscillation stops in the user application.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_PLLSpeedClockStop(void)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    /* The module checks OSCSF register if PLL clock stable state. If not, return an error. */
    if (0U == SYSTEM->OSCSF_b.PLLSF)
    {
        /* It is necessary to confirm using the R_SYS_OscStabilizationFlagGet()
           that the PLL clock is in the stable oscillation state. (OSCSF.PLLSF = 1) */
        return -1;
    }
    
    /* If the system clock source is PLL, do not stop the PLL. */
    if (R_SYSTEM_PRV_SCKSCR_CKSEL_PLL == SYSTEM->SCKSCR_b.CKSEL)
    {
        /* It is used as a system clock source and can not be stopped. */
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* The module checks PLLCR register if PLL is operating. If not, do nothing. */
    if (0U == SYSTEM->PLLCR_b.PLLSTP)
    {
        
        /* Wait the PLL circuit stabilization. */
        while (0U == SYSTEM->OSCSF_b.PLLSF)
        {
            ;   /* wait */
        }
        
        R_SYSTEM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Start the PLL circuit operation. */
        SYSTEM->PLLCR_b.PLLSTP = 1U;
        
        R_SYSTEM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_CGC);
        
        /* Wait until the register value has changed. */
        while(1U != SYSTEM->PLLCR_b.PLLSTP)
        {
            ;   /* wait */
        }
    }
    
    return 0;
    
} /* End of function R_SYS_PLLSpeedClockStop() */

/* Function Name : R_SYS_OscStabilizationFlagGet */
/*******************************************************************************************************************//**
 * @brief Get OSCSF register.@n
 *        If wait the main clock oscillation, HOCO clock oscillation or PLL clock oscillation,
 *        call this API and check the oscillation stability flag.
 * @retval OSCSF register value
 * @note  OSCSF register@n
 *        b0   : [HOCOSF] HOCO clock stabilization flag@n
 *        b1-2 : Reserved (0 fixed)@n
 *        b3   : [MOSCSF] Main clock oscillation stabilization flag@n
 *        b4   : Reserved (0 fixed)@n
 *        b5   : [PLLSF] PLL circuit stabilization flag@n
 *        b6-7 : Reserved (0 fixed)@n
 **********************************************************************************************************************/
uint8_t R_SYS_OscStabilizationFlagGet(void)
{
    /* Return OSCSF register value. */
    return SYSTEM->OSCSF;
} /* End of function R_SYS_OscStabilizationFlagGet() */

/* Function Name : system_interrupt_open */
/*******************************************************************************************************************//**
 * @brief Initialize callback function array.
 **********************************************************************************************************************/
static void system_interrupt_open(void)
{
    uint32_t i;
    
    for (i = 0; i < R_SYSTEM_PRV_IRQ_EVENT_NUMBER_TOTAL; i++)
    {
        g_system_vectors[i] = 0;
    }
} /* End of function system_interrupt_open() */

/* Function Name : R_SYS_IrqEventLinkSet */
/*******************************************************************************************************************//**
 * @brief The module registers the peripheral interrupt handler processing called from the interrupt handler of
 *        the specified "ICU event link number".
 * @param[in] irq       Which ICU event link number is being used.
 * @param[in] iels_value Value to be set in the IELSRn.IELS bit.
 * @param[in] callback  Pointer to function to call when interrupt occurs.
 *                      It does not return an error even if the function is NULL.
 * @retval 0 Success
 * @retval -1 Error
 * @note  IELSRn register IELS bit@n
 *        b0-4   : [IELS] ICU event link select@n
 **********************************************************************************************************************/
int32_t R_SYS_IrqEventLinkSet(IRQn_Type irq, uint32_t iels_value, system_int_cb_t callback)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (irq > IEL31_IRQn)
    {
        return -1;
    }
    
    if (R_SYSTEM_PRV_IELSR_IELS_MSK < iels_value)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* Get the IR bit in the selected IELSR register. */
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0);
    
    /* Set IELSRn.IELS bit. */
    *(p_ielsr + (uint32_t)irq) = (iels_value | ((*(p_ielsr + (uint32_t)irq)) & (~R_SYSTEM_PRV_IELSR_IELS_MSK)));
    
    /* Check for valid address. */
#if defined(__GNUC__)
    /* In case of using the GCC compiler, cast NULL with uint32_t. */
    if ((uint32_t)callback != (uint32_t)NULL)
#else
    if ((uint32_t)callback != NULL)
#endif
    {
        g_system_vectors[irq] = callback;
    }
    
    return 0;
} /* End of function R_SYS_IrqEventLinkSet() */

/* Function Name : system_irq_handler_call */
/*******************************************************************************************************************//**
 * @brief The module executes peripheral interrupt handler processing registered in the specified
 *        "ICU event link number".
 * @param[in] irq       Which ICU event link number is being used.
 **********************************************************************************************************************/
static void system_irq_handler_call(IRQn_Type irq)
{
#if defined(__GNUC__)
    /* In case of using the GCC compiler, cast NULL with uint32_t. */
    if ((uint32_t)g_system_vectors[irq] != (uint32_t)NULL)
#else
    if ((uint32_t)g_system_vectors[irq] != NULL)
#endif
    {
        g_system_vectors[irq]();
    }
} /* End of function system_irq_handler_call() */

/* Function Name : R_SYS_IrqStatusGet */
/*******************************************************************************************************************//**
 * @brief The module returns the IR flag status of the specified "ICU event link number".
 * @param[in] irq       Which ICU event link number is being used.
 * @param[out] *p_ir    Pointer to definition of the ICU event link number IR flag.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
int32_t R_SYS_IrqStatusGet(IRQn_Type irq, uint8_t * p_ir)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (irq > IEL31_IRQn)
    {
        return -1;
    }
    
    /* In the case of argument is NULL, return an error. */
    if (NULL == p_ir)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* Get the IR bit in the selected IELSR register. */
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0);
    
    /* Get IELSRn.IELS bit. */
    *p_ir = (((*(p_ielsr + (uint32_t)irq)) & R_SYSTEM_PRV_IELSR_IR_MSK) >> 16);

    return 0;
    
} /* End of function R_SYS_IrqStatusGet() */

/* Function Name : R_SYS_IrqStatusClear */
/*******************************************************************************************************************//**
 * @brief Clear the interrupt status flag (IR) for a given interrupt. When an interrupt is triggered the IR bit
 *        is set. If it is not cleared in the ISR then the interrupt will trigger again immediately.
 * @param[in] irq Interrupt for which to clear the IR bit.
 * @note This does not work for system exceptions where the IRQn_Type value is < 0.
 **********************************************************************************************************************/
int32_t R_SYS_IrqStatusClear(IRQn_Type irq)
{
#if (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (irq > IEL31_IRQn)
    {
        return -1;
    }
#endif /* (SYSTEM_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* Clear the IR bit in the selected IELSR register. */
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0);
    
    /* Clear IELSRn.IELS bit. */
    (*(p_ielsr + (uint32_t)irq)) &= (~R_SYSTEM_PRV_IELSR_IR_MSK);

    return 0;
    
} /* End of function R_SYS_IrqStatusClear() */

/* Function Name : R_SYS_EnterCriticalSection */
/*******************************************************************************************************************//**
 * @brief The module is capable of entering the critical section by masking interrupt with
 *       __get_PRIMASK or __get_BASEPRI, and __set_PRIMASK or __set_BASEPRI.
 *       Current IRQ mask level must be kept in a global variable.
 **********************************************************************************************************************/
void R_SYS_EnterCriticalSection(void)
{
    system_enter_critical_section();
} /* End of function R_SYS_EnterCriticalSection() */

/* Function Name : system_enter_critical_section */
/*******************************************************************************************************************//**
 * @brief Refer to R_SYS_EnterCriticalSection().
 **********************************************************************************************************************/
static void system_enter_critical_section(void)
{
    gs_old_mask_level = __get_PRIMASK();
    __set_PRIMASK(1U);
} /* End of function system_enter_critical_section() */

/* Function Name : R_SYS_ExitCriticalSection */
/*******************************************************************************************************************//**
 * @brief The module is capable of leaving the critical section by masking interrupt with
 *        __set_PRIMASK or __set_BASEPRI. Set back the IRQ mask level saved in a global variable.
 **********************************************************************************************************************/
void R_SYS_ExitCriticalSection(void)
{
    system_exit_critical_section();
} /* End of function R_SYS_ExitCriticalSection() */

/* Function Name : system_exit_critical_section */
/*******************************************************************************************************************//**
 * @brief Refer to R_SYS_ExitCriticalSection().
 **********************************************************************************************************************/
static void system_exit_critical_section(void)
{
    __set_PRIMASK(gs_old_mask_level);
} /* End of function system_exit_critical_section() */

/* Function Name : R_SYS_ResourceLock */
/*******************************************************************************************************************//**
 * @brief The module locks a common resource for the atomic access.
 * @param[in] hw_index IP index to resource lock.
 * @retval 0    Lock was acquired
 * @retval -1   Lock was not acquired
 **********************************************************************************************************************/
int32_t R_SYS_ResourceLock(e_system_mcu_lock_t hw_index)
{
    return system_software_lock(&gs_system_locks[hw_index]);
} /* End of function R_SYS_ResourceLock() */

/* Function Name : R_SYS_ResourceUnlock */
/*******************************************************************************************************************//**
 * @brief The module unlocks the common resource locked by lock API.
 * @param[in] hw_index IP index to resource unlock.
 **********************************************************************************************************************/
void R_SYS_ResourceUnlock(e_system_mcu_lock_t hw_index)
{
    gs_system_locks[hw_index].lock = R_SYSTEM_PRV_LOCK_UNLOCKED;
} /* End of function R_SYS_ResourceUnlock() */

/* Function Name : system_resource_lock_open */
/*******************************************************************************************************************//**
 * @brief Initializes variables needed for resource lock functionality.
 **********************************************************************************************************************/
static void system_resource_lock_open(void)
{
    uint32_t i;
    
    for (i = 0U; i < SYSTEM_LOCK_NUM; i++)
    {
        /* Initialize reference counters to 0. */
        gs_system_locks[i].lock = (uint8_t)0;
    }
} /* End of function system_resource_lock_open() */

/* Function Name : system_software_lock */
/*******************************************************************************************************************//**
 * @brief Attempt to acquire the lock that has been sent in (CM0+ implementation).
 * @param[in] p_lock Pointer to the structure which contains the lock to be acquired.
 * @retval 0    Lock was acquired
 * @retval -1   Lock was not acquired
 **********************************************************************************************************************/
static int32_t system_software_lock(st_system_lock_t * p_lock)
{
    int32_t err = -1;
    
    R_SYSTEM_PRV_ENTER_CRITICAL_SECTION;
    
    /* Check if lock is available. */
    if (R_SYSTEM_PRV_LOCK_UNLOCKED == p_lock->lock)
    {
        /* Lock is available. Lock it. */
        p_lock->lock = R_SYSTEM_PRV_LOCK_LOCKED;
        err = 0;
    }
    
    R_SYSTEM_PRV_EXIT_CRITICAL_SECTION;
    
    return err;
} /* End of function system_software_lock() */

/* Function Name : system_register_protect_open */
/*******************************************************************************************************************//**
 * @brief Initializes variables needed for register protection functionality.
 **********************************************************************************************************************/
static void system_register_protect_open(void)
{
    uint32_t i;

    for (i = 0U; i < SYSTEM_REG_PROTECT_TOTAL_ITEMS; i++)
    {
        /* Initialize reference counters to 0. */
        gs_protect_counters[i] = (uint16_t)0;
    }
} /* End of function system_register_protect_open() */

/* Function Name : R_SYS_RegisterProtectEnable */
/*******************************************************************************************************************//**
 * @brief Enable register protection. Registers that are protected cannot be written to. Register protection is
 *        enabled by using the Protect Register (PRCR) and the MPC's Write-Protect Register (PWPR).
 * @param[in] regs_to_protect Registers which have write protection enabled.
 **********************************************************************************************************************/
void R_SYS_RegisterProtectEnable(e_system_reg_protect_t regs_to_protect)
{
    system_register_protect_enable(regs_to_protect);
} /* End of function R_SYS_RegisterProtectEnable() */

/* Function Name : system_register_protect_enable */
/*******************************************************************************************************************//**
 * @brief Refer to R_SYS_RegisterProtectEnable().
 * @param[in] Refer to R_SYS_RegisterProtectEnable().
 **********************************************************************************************************************/
static void system_register_protect_enable(e_system_reg_protect_t regs_to_protect)
{
    R_SYSTEM_PRV_ENTER_CRITICAL_SECTION;
    
    /* Is it safe to disable write access? */
    if (0U != gs_protect_counters[regs_to_protect])
    {
        /* Decrement the protect counter */
        gs_protect_counters[regs_to_protect]--;
    }
    
    /* Is it safe to disable write access? */
    if (0U == gs_protect_counters[regs_to_protect])
    {
        if (SYSTEM_REG_PROTECT_MPC != regs_to_protect)
        {
            /* Enable protection using PRCR register. */
            /* When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 0 to
               disable writes. */
            SYSTEM->PRCR = (uint16_t)((SYSTEM->PRCR | 
                                      (uint16_t)R_SYSTEM_PRV_PRCR_KEY) & (uint16_t)(~gs_prcr_masks[regs_to_protect]));
        }
        else
        {
            /* Enable protection for MPC using PWPR register. */
            /* Enable writing of PFSWE bit. It could be assumed that the B0WI bit is still cleared from a call to 
               protection disable function, but it is written here to make sure that the PFSWE bit always gets 
               cleared. */
            PMISC->PWPR_b.B0WI = 0U;
            
            /* Disable writing to PFS registers. */ 
            PMISC->PWPR_b.PFSWE = 0U;
            
            /* Disable writing of PFSWE bit. */
            PMISC->PWPR_b.B0WI = 1U;
        }
    }
    
    R_SYSTEM_PRV_EXIT_CRITICAL_SECTION;
    
} /* End of function system_register_protect_enable() */

/* Function Name : R_SYS_RegisterProtectDisable */
/*******************************************************************************************************************//**
 * @brief Disable register protection. Registers that are protected cannot be written to. Register protection is
 *        disabled by using the Protect Register (PRCR) and the MPC's Write-Protect Register (PWPR).
 * @param[in] regs_to_unprotect Registers which have write protection disabled.
 **********************************************************************************************************************/
void R_SYS_RegisterProtectDisable(e_system_reg_protect_t regs_to_unprotect)
{
    system_register_protect_disable(regs_to_unprotect);
} /* End of function R_SYS_RegisterProtectDisable() */

/* Function Name : system_register_protect_disable */
/*******************************************************************************************************************//**
 * @brief Refer to R_SYS_RegisterProtectDisable().
 * @param[in] Refer to R_SYS_RegisterProtectDisable().
 **********************************************************************************************************************/
static void system_register_protect_disable(e_system_reg_protect_t regs_to_unprotect)
{
    R_SYSTEM_PRV_ENTER_CRITICAL_SECTION;

    /* If this is first entry then disable protection. */
    if (0U == gs_protect_counters[regs_to_unprotect])
    {
        if (SYSTEM_REG_PROTECT_MPC != regs_to_unprotect)
        {
            /* Disable protection using PRCR register. */
            /* When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 0 to
               disable writes. */
            SYSTEM->PRCR = (uint16_t)((SYSTEM->PRCR | 
                                      (uint16_t)R_SYSTEM_PRV_PRCR_KEY) | (uint16_t)gs_prcr_masks[regs_to_unprotect]);
        }
        else
        {
            /* Disable protection for MPC using PWPR register. */
            /* Enable writing of PFSWE bit. */
            PMISC->PWPR_b.B0WI = 0U;
            
            /* Enable writing to PFS registers. */ 
            PMISC->PWPR_b.PFSWE = 1U;
        }
    }
    
    /* Increment the protect counter */
    gs_protect_counters[regs_to_unprotect]++;
    
    R_SYSTEM_PRV_EXIT_CRITICAL_SECTION;
    
} /* End of function system_register_protect_disable() */

/* Function Name : software_delay_loop */
/*******************************************************************************************************************//**
 * @brief This assembly language routine takes roughly 4 cycles per loop. 2 additional cycles
 *        occur when the loop exits. The 'naked' attribute  indicates that the specified function does not need
 *        prologue/epilogue sequences generated by the compiler.
 * @param[in]     loop_cnt  The number of loops to iterate.
 * @retval        None.
 **********************************************************************************************************************/
R_SYSTEM_PRV_ATTRIBUTE_STACKLESS static void software_delay_loop (uint32_t loop_cnt)
{
        __asm volatile ("sw_delay_loop:         \n"

#if defined(__ICCARM__)
                        "   subs r0, #1         \n"     ///< 1 cycle
#elif defined(__GNUC__)
                        "   sub r0, r0, #1      \n"     ///< 1 cycle
#endif

                        "   cmp r0, #0          \n"     ///< 1 cycle
/* CM0 has a different instruction set */
#ifdef __CORE_CM0PLUS_H_GENERIC
                        "   bne sw_delay_loop   \n"     ///< 2 cycles
#else
                        "   bne.n sw_delay_loop \n"     ///< 2 cycles
#endif
                        "   bx lr               \n");   ///< 2 cycles

}   /* End of function software_delay_loop() */

/* Function Name : R_SYS_SoftwareDelay */
/*******************************************************************************************************************//**
 * @brief       Delay the specified duration in units and return.
 * @param[in]   delay  The number of 'units' to delay.
 * @param[in]   units  The 'base' (e_system_delay_units_t) for the units specified. Valid values are:
 *              SYSTEM_DELAY_UNITS_MILLISECONDS, SYSTEM_DELAY_UNITS_MICROSECONDS.@n
 *              For example:@n
 *              At 1 MHz one cycle takes 1 microsecond (.000001 seconds).@n
 *              At 12 MHz one cycle takes 1/12 microsecond or 83 nanoseconds.@n
 *              Therefore one run through software_delay_loop() takes:
 *              ~ (83 * R_SYSTEM_PRV_DELAY_LOOP_CYCLES) or 332 ns.
 *              A delay of 2 us therefore requires 2000ns/332ns or 6 loops.
 *
 *              The 'theoretical' maximum delay that may be obtained is determined by a full 32 bit loop count
 *              and the system clock rate.
 *              @240MHz:  ((0xFFFFFFFF loops * 4 cycles /loop) / 240000000) = 71 seconds.
 *              @32MHz:  ((0xFFFFFFFF loops * 4 cycles /loop) / 32000000) = 536 seconds.
 *
 *              Note that requests for very large delays will be affected by rounding in the calculations
 *              and the actual delay achieved may be slightly less.
 *              @32 MHz, for example, a request for 532 seconds will be closer to 536 seconds.
 *
 *              Note also that if the calculations result in a loop_cnt of zero,
 *              the software_delay_loop() function is not called at all.
 *              In this case the requested delay is too small (nanoseconds) to be carried out by the loop itself,
 *              and the overhead associated with executing the code to just get to this point has certainly satisfied
 *              the requested delay.
 *
 * @retval        None.
 **********************************************************************************************************************/
void R_SYS_SoftwareDelay(uint32_t delay, e_system_delay_units_t units)
{
    uint32_t iclk_hz;
    uint32_t cycles_requested;
    uint32_t ns_per_cycle;
    uint32_t loops_required = 0;
    uint32_t total_us = (delay * units);  /** Convert the requested time to microseconds. */
    uint64_t ns_64bits;
    
    R_SYS_SystemClockFreqGet(&iclk_hz);        /** Get the system clock frequency in Hz. */
    
    /* Running on the Sub-clock (32768 Hz) there are 30517 ns/cycle. This means one cycle takes 31 us. One execution
     * loop of the delay_loop takes 6 cycles which at 32768 Hz is 180 us.
     * That does not include the overhead below prior to even getting to the delay loop.
     * Given this, at this frequency anything less then a delay request of 122 us will not even generate a single
     * pass through the delay loop.
     * For this reason small delays (<=~200 us) at this slow clock rate will not be possible and such a request
     * will generate a minimum delay of ~200 us. */
    
    ns_per_cycle = (1000000000)/iclk_hz;            /** Get the # of nanoseconds/cycle. */
    
    /* We want to get the time in total nanoseconds but need to be conscious of overflowing 32 bits.
     * We also do not want to do 64 bit */
    /* division as that pulls in a division library. */
    ns_64bits = (uint64_t)total_us * (uint64_t)1000;      // Convert to ns.
    
    /* Have we overflowed 32 bits? */
    if (ns_64bits <= 0xFFFFFFFFUL)
    {
        /* No, we will not overflow. */
        cycles_requested =  ((uint32_t)ns_64bits / ns_per_cycle);
        loops_required = cycles_requested / R_SYSTEM_PRV_DELAY_LOOP_CYCLES;
    }
    else
    {
        /* We did overflow. Try dividing down first. */
        total_us = (total_us / (ns_per_cycle * R_SYSTEM_PRV_DELAY_LOOP_CYCLES));
        
        /* Convert to ns. */
        ns_64bits = (uint64_t)total_us * (uint64_t)1000;
        
        /* Have we overflowed 32 bits? */
        if (ns_64bits <= 0xFFFFFFFFUL)
        {
            /* No, we will not overflow. */
            loops_required =  (uint32_t)ns_64bits;
        }
        else
        {
            /* We still overflowed, use the max count for cycles */
            loops_required =  0xFFFFFFFFUL;
        }
    }
    
    /** Only delay if the supplied parameters constitute a delay. */
    if (loops_required > (uint32_t)0)
    {
        software_delay_loop(loops_required);
    }
    
} /* End of function R_SYS_SoftwareDelay() */

/* Function Name : system_memcpy */
/*******************************************************************************************************************//**
 * @brief memcpy() for R_SYS_CodeCopy().
 * @param[in]   p_destination   Destination buffer.
 * @param[in]   p_source        Source buffer.
 * @param[in]   num             Number of bytes to copy.
 **********************************************************************************************************************/
static void * system_memcpy(void *p_destination, const void *p_source, size_t num)
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
    
} /* End of function system_memcpy() */

/* Function Name : R_SYS_CodeCopy */
/*******************************************************************************************************************//**
 * @brief Soource Code Copy from ROM to RAM.
 **********************************************************************************************************************/
#if defined(__ICCARM__)              /* IAR Compiler */
#pragma section = ".ramvect"
#pragma section = ".intvec"
#pragma section = ".ramfunc"
#pragma section = ".ramfunc_init"
#pragma section = ".ramdata"
#pragma section = ".ramdata_init"
#pragma section = ".ramobj"
#pragma section = ".ramobj_init"
#elif defined(__GNUC__)              /* GCC Compiler */
extern uint32_t __Vectors_Start;
extern uint32_t __Vectors_Size;
extern uint32_t __ramvect_start;
extern uint32_t __ramfunc_init_start;
extern uint32_t __ramfunc_start;
extern uint32_t __ramfunc_end;
extern uint32_t __ramdata_init_start;
extern uint32_t __ramdata_start;
extern uint32_t __ramdata_end;
extern uint32_t __ramobj_init_start;
extern uint32_t __ramobj_start;
extern uint32_t __ramobj_end;
#else
#endif

void R_SYS_CodeCopy(void)
{
    uint8_t * p_from;
    uint8_t * p_to;
    uint32_t  size;
    
    /* Copy the vector table from ROM to RAM for execution on RAM */
#if defined(__ICCARM__)              /* IAR Compiler */
    p_from = __section_begin(".intvec");
    p_to   = __section_begin(".ramvect");
    size = __section_size(".intvec");
#elif defined(__GNUC__)              /* GCC Compiler */
    p_from = (uint8_t *)&__Vectors_Start;
    p_to   = (uint8_t *)&__ramvect_start;
    size = (uint32_t)&__Vectors_Size;
#else
#error "That compiler is not supported in this release."
#endif
    system_memcpy(p_to, p_from, size);
    
#if defined(__ICCARM__)              /* IAR Compiler */
    /* Copy API code from ROM to RAM for execution on RAM */
    p_from = __section_begin(".ramfunc_init");
    p_to   = __section_begin(".ramfunc");
    system_memcpy(p_to, p_from, __section_size(".ramfunc_init"));
    
    /* Copy const data code from ROM to RAM for execution on RAM */
    p_from = __section_begin(".ramdata_init");
    p_to   = __section_begin(".ramdata");
    system_memcpy(p_to, p_from, __section_size(".ramdata_init"));
    
    /* Copy object code from ROM to RAM for execution on RAM */
    p_from = __section_begin(".ramobj_init");
    p_to   = __section_begin(".ramobj");
    system_memcpy(p_to, p_from, __section_size(".ramobj_init"));
#elif defined(__GNUC__)              /* GCC Compiler */
    /* Copy API code from ROM to RAM for execution on RAM */
    p_from = (uint8_t *)&__ramfunc_init_start;
    p_to   = (uint8_t *)&__ramfunc_start;
    size   = (uint32_t)&__ramfunc_end - (uint32_t)&__ramfunc_start;
    system_memcpy(p_to, p_from, size);
    
    /* Copy const data code from ROM to RAM for execution on RAM */
    p_from = (uint8_t *)&__ramdata_init_start;
    p_to   = (uint8_t *)&__ramdata_start;
    size   = (uint32_t)&__ramdata_end - (uint32_t)&__ramdata_start;
    system_memcpy(p_to, p_from, size);
    
    /* Copy object code from ROM to RAM for execution on RAM */
    p_from = (uint8_t *)&__ramobj_init_start;
    p_to   = (uint8_t *)&__ramobj_start;
    size   = (uint32_t)&__ramobj_end - (uint32_t)&__ramobj_start;
    system_memcpy(p_to, p_from, size);
#else
#error "That compiler is not supported in this release."
#endif
    
    /* Always map Vector table on RAM */
#if defined(__ICCARM__)              /* IAR Compiler */
    SCB->VTOR = (uint32_t)__section_begin(".ramvect");
#elif defined(__GNUC__)              /* GCC Compiler */
    SCB->VTOR = (uint32_t)&__ramvect_start;
#else
#error "That compiler is not supported in this release."
#endif
    
} /* End of function R_SYS_CodeCopy() */

/* Function Name : r_sys_BoostFlagGet */
/*******************************************************************************************************************//**
 * @brief Get the Boost mode holding flag.
 * @param[in] None
 * @param[out] bool boost_flg
 * @retval 0 false
 * @retval 1 true
 **********************************************************************************************************************/
R_SYSTEM_PRV_ATTRIBUTE_STACKLESS_FOR_BOOSTFLAG int32_t r_sys_BoostFlagGet(bool * boost_flg)
{
    *boost_flg = gs_system_boost_flg;

    return 0;

} /* End of function r_sys_BoostFlagGet() */

/* Function Name : r_sys_BoostFlagSet */
/*******************************************************************************************************************//**
 * @brief Set the Boost mode holding flag.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
R_SYSTEM_PRV_ATTRIBUTE_STACKLESS_FOR_BOOSTFLAG int32_t r_sys_BoostFlagSet(void)
{
    gs_system_boost_flg = true;

    return 0;

} /* End of function r_sys_BoostFlagSet() */

/* Function Name : r_sys_BoostFlagClr */
/*******************************************************************************************************************//**
 * @brief Clear the Boost mode holding flag.
 * @retval 0 Success
 * @retval -1 Error
 **********************************************************************************************************************/
R_SYSTEM_PRV_ATTRIBUTE_STACKLESS_FOR_BOOSTFLAG int32_t r_sys_BoostFlagClr(void)
{
    gs_system_boost_flg = false;

    return 0;

} /* End of function r_sys_BoostFlagClr() */

/* Function Name : IEL0_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL0 IRQ Handler.
 **********************************************************************************************************************/
void IEL0_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL0_IRQn);
    system_irq_handler_call(IEL0_IRQn);
} /* End of function IEL0_IRQHandler() */

/* Function Name : IEL1_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL1 IRQ Handler.
 **********************************************************************************************************************/
void IEL1_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL1_IRQn);
    system_irq_handler_call(IEL1_IRQn);
} /* End of function IEL1_IRQHandler() */

/* Function Name : IEL2_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL2 IRQ Handler.
 **********************************************************************************************************************/
void IEL2_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL2_IRQn);
    system_irq_handler_call(IEL2_IRQn);
} /* End of function IEL2_IRQHandler() */

/* Function Name : IEL3_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL3 IRQ Handler.
 **********************************************************************************************************************/
void IEL3_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL3_IRQn);
    system_irq_handler_call(IEL3_IRQn);
} /* End of function IEL3_IRQHandler() */

/* Function Name : IEL4_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL4 IRQ Handler.
 **********************************************************************************************************************/
void IEL4_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL4_IRQn);
    system_irq_handler_call(IEL4_IRQn);
} /* End of function IEL4_IRQHandler() */

/* Function Name : IEL5_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL5 IRQ Handler.
 **********************************************************************************************************************/
void IEL5_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL5_IRQn);
    system_irq_handler_call(IEL5_IRQn);
} /* End of function IEL5_IRQHandler() */

/* Function Name : IEL6_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL6 IRQ Handler.
 **********************************************************************************************************************/
void IEL6_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL6_IRQn);
    system_irq_handler_call(IEL6_IRQn);
} /* End of function IEL6_IRQHandler() */

/* Function Name : IEL7_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL7 IRQ Handler.
 **********************************************************************************************************************/
void IEL7_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL7_IRQn);
    system_irq_handler_call(IEL7_IRQn);
} /* End of function IEL7_IRQHandler() */

/* Function Name : IEL8_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL8 IRQ Handler.
 **********************************************************************************************************************/
void IEL8_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL8_IRQn);
    system_irq_handler_call(IEL8_IRQn);
} /* End of function IEL8_IRQHandler() */

/* Function Name : IEL9_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL9 IRQ Handler.
 **********************************************************************************************************************/
void IEL9_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL9_IRQn);
    system_irq_handler_call(IEL9_IRQn);
} /* End of function IEL9_IRQHandler() */

/* Function Name : IEL10_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL10 IRQ Handler.
 **********************************************************************************************************************/
void IEL10_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL10_IRQn);
    system_irq_handler_call(IEL10_IRQn);
} /* End of function IEL10_IRQHandler() */

/* Function Name : IEL11_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL11 IRQ Handler.
 **********************************************************************************************************************/
void IEL11_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL11_IRQn);
    system_irq_handler_call(IEL11_IRQn);
} /* End of function IEL11_IRQHandler() */

/* Function Name : IEL12_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL12 IRQ Handler.
 **********************************************************************************************************************/
void IEL12_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL12_IRQn);
    system_irq_handler_call(IEL12_IRQn);
} /* End of function IEL12_IRQHandler() */

/* Function Name : IEL13_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL13 IRQ Handler.
 **********************************************************************************************************************/
void IEL13_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL13_IRQn);
    system_irq_handler_call(IEL13_IRQn);
} /* End of function IEL13_IRQHandler() */

/* Function Name : IEL14_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL14 IRQ Handler.
 **********************************************************************************************************************/
void IEL14_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL14_IRQn);
    system_irq_handler_call(IEL14_IRQn);
} /* End of function IEL14_IRQHandler() */

/* Function Name : IEL15_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL15 IRQ Handler.
 **********************************************************************************************************************/
void IEL15_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL15_IRQn);
    system_irq_handler_call(IEL15_IRQn);
} /* End of function IEL15_IRQHandler() */

/* Function Name : IEL16_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL16 IRQ Handler.
 **********************************************************************************************************************/
void IEL16_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL16_IRQn);
    system_irq_handler_call(IEL16_IRQn);
} /* End of function IEL16_IRQHandler() */

/* Function Name : IEL17_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL17 IRQ Handler.
 **********************************************************************************************************************/
void IEL17_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL17_IRQn);
    system_irq_handler_call(IEL17_IRQn);
} /* End of function IEL17_IRQHandler() */

/* Function Name : IEL18_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL18 IRQ Handler.
 **********************************************************************************************************************/
void IEL18_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL18_IRQn);
    system_irq_handler_call(IEL18_IRQn);
} /* End of function IEL18_IRQHandler() */

/* Function Name : IEL19_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL19 IRQ Handler.
 **********************************************************************************************************************/
void IEL19_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL19_IRQn);
    system_irq_handler_call(IEL19_IRQn);
} /* End of function IEL19_IRQHandler() */

/* Function Name : IEL20_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL20 IRQ Handler.
 **********************************************************************************************************************/
void IEL20_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL20_IRQn);
    system_irq_handler_call(IEL20_IRQn);
} /* End of function IEL20_IRQHandler() */

/* Function Name : IEL21_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL21 IRQ Handler.
 **********************************************************************************************************************/
void IEL21_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL21_IRQn);
    system_irq_handler_call(IEL21_IRQn);
} /* End of function IEL21_IRQHandler() */

/* Function Name : IEL22_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL22 IRQ Handler.
 **********************************************************************************************************************/
void IEL22_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL22_IRQn);
    system_irq_handler_call(IEL22_IRQn);
} /* End of function IEL22_IRQHandler() */

/* Function Name : IEL23_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL23 IRQ Handler.
 **********************************************************************************************************************/
void IEL23_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL23_IRQn);
    system_irq_handler_call(IEL23_IRQn);
} /* End of function IEL23_IRQHandler() */

/* Function Name : IEL24_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL24 IRQ Handler.
 **********************************************************************************************************************/
void IEL24_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL24_IRQn);
    system_irq_handler_call(IEL24_IRQn);
} /* End of function IEL24_IRQHandler() */

/* Function Name : IEL25_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL25 IRQ Handler.
 **********************************************************************************************************************/
void IEL25_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL25_IRQn);
    system_irq_handler_call(IEL25_IRQn);
} /* End of function IEL25_IRQHandler() */

/* Function Name : IEL26_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL26 IRQ Handler.
 **********************************************************************************************************************/
void IEL26_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL26_IRQn);
    system_irq_handler_call(IEL26_IRQn);
} /* End of function IEL26_IRQHandler() */

/* Function Name : IEL27_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL27 IRQ Handler.
 **********************************************************************************************************************/
void IEL27_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL27_IRQn);
    system_irq_handler_call(IEL27_IRQn);
} /* End of function IEL27_IRQHandler() */

/* Function Name : IEL28_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL28 IRQ Handler.
 **********************************************************************************************************************/
void IEL28_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL28_IRQn);
    system_irq_handler_call(IEL28_IRQn);
} /* End of function IEL28_IRQHandler() */

/* Function Name : IEL29_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL29 IRQ Handler.
 **********************************************************************************************************************/
void IEL29_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL29_IRQn);
    system_irq_handler_call(IEL29_IRQn);
} /* End of function IEL29_IRQHandler() */

/* Function Name : IEL30_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL30 IRQ Handler.
 **********************************************************************************************************************/
void IEL30_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL30_IRQn);
    system_irq_handler_call(IEL30_IRQn);
} /* End of function IEL30_IRQHandler() */

/* Function Name : IEL31_IRQHandler */
/*******************************************************************************************************************//**
 * @brief IEL31 IRQ Handler.
 **********************************************************************************************************************/
void IEL31_IRQHandler(void) /* @suppress("Non-API function naming") */ /* @suppress("Function declaration") */
{
    R_SYS_IrqStatusClear(IEL31_IRQn);
    system_irq_handler_call(IEL31_IRQn);
} /* End of function IEL31_IRQHandler() */

/* Function Name : R_SYS_GetVersion */
/*******************************************************************************************************************//**
 * @brief Get R_SYSTEM version number.
 **********************************************************************************************************************/
uint32_t R_SYS_GetVersion(void)
{
    /* The version number size must be 32-bits. */
    uint32_t const version = ((((uint32_t)SYSTEM_VERSION_MAJOR) << 16) | (uint32_t)SYSTEM_VERSION_MINOR);
    
    return version;
    
}   /* End of function R_SYS_GetVersion() */

/* Function Name : r_system_wdt_refresh */
/******************************************************************************************************************//**
 * @brief       Refresh the watchdog timer
 *********************************************************************************************************************/
#if defined(__ICCARM__)
__WEAK void r_system_wdt_refresh(void)
#elif defined(__GNUC__)
void r_system_wdt_refresh(void)
#endif
{
    /* refresh the WDT down counter */
    WDT->WDTRR = 0x00;
    WDT->WDTRR = 0xFF;
}   /* End of function r_system_wdt_refresh() */

/*******************************************************************************************************************//**
 * @} (end addtogroup Device HAL SYSTEM (System Control, Common code among modules))
 **********************************************************************************************************************/

/* End of File (r_system_api.c) */
