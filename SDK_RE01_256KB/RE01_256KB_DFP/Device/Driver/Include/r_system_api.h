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
 * File Name    : r_system_api.h
 * Version      : 1.20
 * Description  : Device HAL SYSTEM (System Control, Common code among modules)
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 07.11.2019 0.40     First release.
 *         : 03.03.2020 0.80     Added the SYSTEM_REG_PROTECT_LPC_LDO to e_system_reg_protect_t.
 *         : 24.11.2020 1.10     Update the minor version.
 *         : 12.05.2021 1.20     Update R_NVIC_EnableIRQ() function.
 *                               Update R_NVIC_GetEnableIRQ() function.
 *                               Update R_NVIC_DisableIRQ() function.
 *                               Update R_NVIC_GetPendingIRQ() function.
 *                               Update R_NVIC_SetPendingIRQ() function.
 *                               Update R_NVIC_ClearPendingIRQ() function.
 *                               Update R_NVIC_SetPriority() function.
 *                               Update R_NVIC_SetVector() function.
 *                               Update R_NVIC_GetVector() function.
 *                               Update the minor version.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 @ingroup  grp_device_hal
 @defgroup grp_device_hal_system Device HAL SYSTEM Implementation
 @brief The SYSTEM driver implementation for the Device HAL driver interface on RE01 Group.

 @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* C99 includes. */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "RE01_256KB.h"
#include "cmsis_compiler.h"
#include "r_system_cfg.h"

#ifndef R_SYSTEM_API_H_
#define R_SYSTEM_API_H_

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* Version Number of API. */
#define SYSTEM_VERSION_MAJOR                (1)
#define SYSTEM_VERSION_MINOR                (32)

/*! @name IRQ_EVENT_LINK_NUMBER
          Definition of IRQ event link number
 */
/* @{ */
#define SYSTEM_IRQ_EVENT_NUMBER0            (IEL0_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER1            (IEL1_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER2            (IEL2_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER3            (IEL3_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER4            (IEL4_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER5            (IEL5_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER6            (IEL6_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER7            (IEL7_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER8            (IEL8_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER9            (IEL9_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER10           (IEL10_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER11           (IEL11_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER12           (IEL12_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER13           (IEL13_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER14           (IEL14_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER15           (IEL15_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER16           (IEL16_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER17           (IEL17_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER18           (IEL18_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER19           (IEL19_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER20           (IEL20_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER21           (IEL21_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER22           (IEL22_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER23           (IEL23_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER24           (IEL24_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER25           (IEL25_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER26           (IEL26_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER27           (IEL27_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER28           (IEL28_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER29           (IEL29_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER30           (IEL30_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER31           (IEL31_IRQn)
#define SYSTEM_IRQ_EVENT_NUMBER_NOT_USED    ((IRQn_Type)(IEL31_IRQn+1))
/* @} */

#define SYSTEM_SECTION_CODE         (1)
#define SYSTEM_SECTION_RAM_FUNC     (2)

#if (SYSTEM_CFG_SECTION_R_SYS_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_INITIALIZE          __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_INITIALIZE
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_BOOSTSPEEDMODESET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_BOOSTSPEEDMODESET   __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_BOOSTSPEEDMODESET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDMODESET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_HIGHSPEEDMODESET    __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_HIGHSPEEDMODESET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDMODESET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_LOWSPEEDMODESET     __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_LOWSPEEDMODESET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SPEEDMODEGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SPEEDMODEGET        __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SPEEDMODEGET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKHOCOSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKHOCOSET  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKHOCOSET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOCOSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKMOCOSET  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKMOCOSET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKLOCOSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKLOCOSET  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKLOCOSET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOSCSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKMOSCSET  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKMOSCSET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKSOSCSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKSOSCSET  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKSOSCSET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKFREQGET == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKHOCOSET == SYSTEM_SECTION_RAM_FUNC) || \
    (SYSTEM_CFG_SECTION_R_SYS_SOFTWAREDELAY      == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKFREQGET  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKFREQGET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_PERIPHERALCLOCKFREQGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_PERIPHERALCLOCKFREQGET  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_PERIPHERALCLOCKFREQGET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKDIVIDERSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKDIVIDERSET  __attribute__ ((section(".ramfunc")))    /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SYSTEMCLOCKDIVIDERSET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_MAINOSCSPEEDCLOCKSTART  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_MAINOSCSPEEDCLOCKSTART
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_MAINOSCSPEEDCLOCKSTOP   __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_MAINOSCSPEEDCLOCKSTOP
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_HIGHSPEEDCLOCKSTART     __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_HIGHSPEEDCLOCKSTART
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_HIGHSPEEDCLOCKSTOP      __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_HIGHSPEEDCLOCKSTOP
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_MEDIUMSPEEDCLOCKSTART   __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_MEDIUMSPEEDCLOCKSTART
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_MEDIUMSPEEDCLOCKSTOP    __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_MEDIUMSPEEDCLOCKSTOP
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_LOWSPEEDCLOCKSTART      __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_LOWSPEEDCLOCKSTART
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_LOWSPEEDCLOCKSTOP       __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_LOWSPEEDCLOCKSTOP
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTART == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SUBOSCSPEEDCLOCKSTART   __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SUBOSCSPEEDCLOCKSTART
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SUBOSCSPEEDCLOCKSTOP    __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SUBOSCSPEEDCLOCKSTOP
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_OSCSTABILIZATIONFLAGGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_OSCSTABILIZATIONFLAGGET __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_OSCSTABILIZATIONFLAGGET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_IRQEVENTLINKSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_IRQEVENTLINKSET         __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_IRQEVENTLINKSET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_IRQSTATUSGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_IRQSTATUSGET            __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_IRQSTATUSGET
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_IRQSTATUSCLEAR == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_IRQSTATUSCLEAR          __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_IRQSTATUSCLEAR
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_ENTERCRITICALSECTION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_ENTERCRITICALSECTION    __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_ENTERCRITICALSECTION
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_EXITCRITICALSECTION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_EXITCRITICALSECTION     __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_EXITCRITICALSECTION
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_RESOURCELOCK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_RESOURCELOCK            __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_RESOURCELOCK
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_RESOURCEUNLOCK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_RESOURCEUNLOCK          __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_RESOURCEUNLOCK
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTENABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_REGISTERPROTECTENABLE   __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_REGISTERPROTECTENABLE
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTDISABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_REGISTERPROTECTDISABLE  __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_REGISTERPROTECTDISABLE
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_SOFTWAREDELAY      == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_SOFTWAREDELAY           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_SOFTWAREDELAY
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_CODECOPY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_CODECOPY           __attribute__ ((section(".ramfunc")))    /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_CODECOPY
#endif
#define FUNC_LOCATION_R_SYS_BOOSTFLAGGET           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#define FUNC_LOCATION_R_SYS_BOOSTFLAGCLR           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#if (SYSTEM_CFG_SECTION_IEL0_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL0_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL0_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL1_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL1_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL1_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL2_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL2_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL2_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL3_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL3_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL3_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL4_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL4_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL4_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL5_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL5_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL5_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL6_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL6_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL6_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL7_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL7_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL7_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL8_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL8_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL8_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL9_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL9_IRQHANDLER           __attribute__ ((section(".ramfunc")))   /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL9_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL10_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL10_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL10_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL11_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL11_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL11_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL12_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL12_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL12_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL13_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL13_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL13_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL14_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL14_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL14_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL15_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL15_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL15_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL16_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL16_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL16_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL17_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL17_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL17_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL18_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL18_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL18_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL19_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL19_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL19_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL20_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL20_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL20_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL21_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL21_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL21_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL22_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL22_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL22_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL23_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL23_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL23_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL24_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL24_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL24_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL25_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL25_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL25_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL26_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL26_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL26_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL27_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL27_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL27_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL28_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL28_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL28_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL29_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL29_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL29_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL30_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL30_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL30_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_IEL31_IRQHANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_IEL31_IRQHANDLER           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_IEL31_IRQHANDLER
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_GETVERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_GETVERSION           __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_GETVERSION
#endif
#if (SYSTEM_CFG_SECTION_R_SYS_WDT_REFRESH == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SYS_WDT_REFRESH          __attribute__ ((section(".ramfunc")))  /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SYS_WDT_REFRESH
#endif

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/** Speed mode select */
typedef enum e_system_speed_mode
{
    SYSTEM_SPEED_MODE_BOOST,    /*!< Boost-mode */
    SYSTEM_SPEED_MODE_HIGH,     /*!< High-speed mode */
    SYSTEM_SPEED_MODE_LOW,      /*!< Low-speed mode */
    SYSTEM_SPEED_MODE_32KHZ,    /*!< Subosc-speed mode or Low-leakage-current(VBB) mode */
} e_system_speed_mode_t;

/** System Clock Divider */
typedef enum e_system_sys_clock_div
{
    SYSTEM_CLOCK_DIV_1          = 0x00,      /*!< System clock divided by 1.(Default) */
    SYSTEM_CLOCK_DIV_2          = 0x01,      /*!< System clock divided by 2.          */
    SYSTEM_CLOCK_DIV_4          = 0x02,      /*!< System clock divided by 4.          */
    SYSTEM_CLOCK_DIV_8          = 0x03,      /*!< System clock divided by 8.          */
    SYSTEM_CLOCK_DIV_16         = 0x04,      /*!< System clock divided by 16.         */
    SYSTEM_CLOCK_DIV_32         = 0x05,      /*!< System clock divided by 32.         */
    SYSTEM_CLOCK_DIV_64         = 0x06,      /*!< System clock divided by 64.         */
    SYSTEM_CLOCK_NONE_CHANGE    = 0x07,      /*!< Number to maintain.                 */
} e_system_sys_clock_div_t;

/** The different types of registers that can be protected. */
typedef enum e_system_reg_protect
{
    SYSTEM_REG_PROTECT_CGC = 0,     /*!< Enables writing to the registers related to the clock generation circuit.@n
                                         SCKDIVCR, SCKSCR, MOSCCR, HOCOCR, HOCOMCR, MOCOCR,
                                         HCCALCR, CKOCR, CKO32CR, OSTDCR, OSTDSR, SLCDSCKCR, MOSCWTCR, MOMCR,
                                         SOSCCR, SOMCR, LOCOCR */

    SYSTEM_REG_PROTECT_OM_LPC_BATT, /*!< Enables writing to the registers related to operating modes,
                                         low power consumption, and battery backup function.@n
                                         SBYCR, RAMSDCR, SNZCR, SNZEDCR, SNZREQCR, OPCCR, SOPCCR, DPSBYCR,
                                         DPSIER0-1, DPSIFR0-1, DPSIEGR0-1, SYOCDCR, EHCCR0-1, VOCR, LDOCR,
                                         PWSTCR, VBBCR */

    SYSTEM_REG_PROTECT_LVD,         /*!< Enables writing to the registers related to the LVD.@n
                                         LVD1CR1, LVD1SR, LVCMPCR, LVDLVLR, LVD1CR0 */

    SYSTEM_REG_PROTECT_MPC,         /*!< Enables writing to MPC's PFS registers. */

    SYSTEM_REG_PROTECT_LPC_LDO,     /*!< Enables writing to the registers related to low power consumption.@n
                                         LDOCR */

    SYSTEM_REG_PROTECT_TOTAL_ITEMS  /*!< This entry is used for getting the number of enum items.
                                         This must be the last entry. DO NOT REMOVE THIS ENTRY! */
} e_system_reg_protect_t;

/** The module locks a common resource */
typedef enum e_system_mcu_lock
{
    SYSTEM_LOCK_DTC = 0,
    SYSTEM_LOCK_DMAC0,
    SYSTEM_LOCK_DMAC1,
    SYSTEM_LOCK_DMAC2,
    SYSTEM_LOCK_DMAC3,
    SYSTEM_LOCK_IRDA,
    SYSTEM_LOCK_QSPI,
    SYSTEM_LOCK_RIIC1,
    SYSTEM_LOCK_RIIC0,
    SYSTEM_LOCK_USB,
    SYSTEM_LOCK_SPI1,
    SYSTEM_LOCK_SPI0,
    SYSTEM_LOCK_SCI9,
    SYSTEM_LOCK_SCI5,
    SYSTEM_LOCK_SCI4,
    SYSTEM_LOCK_SCI3,
    SYSTEM_LOCK_SCI2,
    SYSTEM_LOCK_SCI1,
    SYSTEM_LOCK_SCI0,
    SYSTEM_LOCK_CAC,
    SYSTEM_LOCK_CRC,
    SYSTEM_LOCK_SLCD,
    SYSTEM_LOCK_LED,
    SYSTEM_LOCK_DOC,
    SYSTEM_LOCK_ELC,
    SYSTEM_LOCK_DIV,
    SYSTEM_LOCK_MLCD,
    SYSTEM_LOCK_GDT,
    SYSTEM_LOCK_TSIP_LITE,
    SYSTEM_LOCK_LST,
    SYSTEM_LOCK_TMR,
    SYSTEM_LOCK_AGT1,
    SYSTEM_LOCK_AGT0,
    SYSTEM_LOCK_LPG,
    SYSTEM_LOCK_GPT320,
    SYSTEM_LOCK_GPT321,
    SYSTEM_LOCK_GPT162,
    SYSTEM_LOCK_GPT163,
    SYSTEM_LOCK_GPT164,
    SYSTEM_LOCK_GPT165,
    SYSTEM_LOCK_CCC,
    SYSTEM_LOCK_MTDV,
    SYSTEM_LOCK_POEG,
    SYSTEM_LOCK_S14AD,
    SYSTEM_LOCK_VREF,
    SYSTEM_LOCK_R12DA,
    SYSTEM_LOCK_TEMPS,
    SYSTEM_LOCK_ACMP,
    SYSTEM_LOCK_NUM
} e_system_mcu_lock_t;

/** Easy to use typedef for callback functions. */
typedef void (*system_int_cb_t)(void);

/** Available delay units for R_SYS_SoftwareDelay(). These are ultimately used to calculate a total # of microseconds */
typedef enum
{
    SYSTEM_DELAY_UNITS_MILLISECONDS = 1000,     /*!< Requested delay amount is in milliseconds. */
    SYSTEM_DELAY_UNITS_MICROSECONDS = 1         /*!< Requested delay amount is in microseconds. */
} e_system_delay_units_t;

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

void R_SYS_Initialize(void)                     FUNC_LOCATION_R_SYS_INITIALIZE;
int32_t R_SYS_BoostSpeedModeSet(void)           FUNC_LOCATION_R_SYS_BOOSTSPEEDMODESET;
int32_t R_SYS_HighSpeedModeSet(void)            FUNC_LOCATION_R_SYS_HIGHSPEEDMODESET;
int32_t R_SYS_LowSpeedModeSet(void)             FUNC_LOCATION_R_SYS_LOWSPEEDMODESET;
e_system_speed_mode_t R_SYS_SpeedModeGet(void)  FUNC_LOCATION_R_SYS_SPEEDMODEGET;
int32_t R_SYS_SystemClockHOCOSet(void)          FUNC_LOCATION_R_SYS_SYSTEMCLOCKHOCOSET;
int32_t R_SYS_SystemClockMOCOSet(void)          FUNC_LOCATION_R_SYS_SYSTEMCLOCKMOCOSET;
int32_t R_SYS_SystemClockLOCOSet(void)          FUNC_LOCATION_R_SYS_SYSTEMCLOCKLOCOSET;
int32_t R_SYS_SystemClockMOSCSet(void)          FUNC_LOCATION_R_SYS_SYSTEMCLOCKMOSCSET;
int32_t R_SYS_SystemClockSOSCSet(void)          FUNC_LOCATION_R_SYS_SYSTEMCLOCKSOSCSET;
int32_t R_SYS_SystemClockFreqGet(uint32_t * p_freq_hz)      FUNC_LOCATION_R_SYS_SYSTEMCLOCKFREQGET;
int32_t R_SYS_PeripheralClockFreqGet(uint32_t * p_freq_hz)  FUNC_LOCATION_R_SYS_PERIPHERALCLOCKFREQGET;
int32_t R_SYS_SystemClockDividerSet(e_system_sys_clock_div_t iclk_div, e_system_sys_clock_div_t pclkb_div) FUNC_LOCATION_R_SYS_SYSTEMCLOCKDIVIDERSET;
void R_SYS_MainOscSpeedClockStart(void)         FUNC_LOCATION_R_SYS_MAINOSCSPEEDCLOCKSTART;
int32_t R_SYS_MainOscSpeedClockStop(void)       FUNC_LOCATION_R_SYS_MAINOSCSPEEDCLOCKSTOP;
int32_t R_SYS_HighSpeedClockStart(void)         FUNC_LOCATION_R_SYS_HIGHSPEEDCLOCKSTART;
int32_t R_SYS_HighSpeedClockStop(void)          FUNC_LOCATION_R_SYS_HIGHSPEEDCLOCKSTOP;
int32_t R_SYS_MediumSpeedClockStart(void)       FUNC_LOCATION_R_SYS_MEDIUMSPEEDCLOCKSTART;
int32_t R_SYS_MediumSpeedClockStop(void)        FUNC_LOCATION_R_SYS_MEDIUMSPEEDCLOCKSTOP;
void R_SYS_LowSpeedClockStart(void)             FUNC_LOCATION_R_SYS_LOWSPEEDCLOCKSTART;
int32_t R_SYS_LowSpeedClockStop(void)           FUNC_LOCATION_R_SYS_LOWSPEEDCLOCKSTOP;
void R_SYS_SubOscSpeedClockStart(void)          FUNC_LOCATION_R_SYS_SUBOSCSPEEDCLOCKSTART;
int32_t R_SYS_SubOscSpeedClockStop(void)        FUNC_LOCATION_R_SYS_SUBOSCSPEEDCLOCKSTOP;
uint8_t R_SYS_OscStabilizationFlagGet(void)     FUNC_LOCATION_R_SYS_OSCSTABILIZATIONFLAGGET;
int32_t R_SYS_IrqEventLinkSet(IRQn_Type irq, uint32_t iels_value, system_int_cb_t callback) FUNC_LOCATION_R_SYS_IRQEVENTLINKSET;
int32_t R_SYS_IrqStatusGet(IRQn_Type irq, uint8_t * p_ir)   FUNC_LOCATION_R_SYS_IRQSTATUSGET;
int32_t R_SYS_IrqStatusClear(IRQn_Type irq)     FUNC_LOCATION_R_SYS_IRQSTATUSCLEAR;
void R_SYS_EnterCriticalSection(void)           FUNC_LOCATION_R_SYS_ENTERCRITICALSECTION;
void R_SYS_ExitCriticalSection(void)            FUNC_LOCATION_R_SYS_EXITCRITICALSECTION;
int32_t R_SYS_ResourceLock(e_system_mcu_lock_t hw_index)    FUNC_LOCATION_R_SYS_RESOURCELOCK;
void R_SYS_ResourceUnlock(e_system_mcu_lock_t hw_index)     FUNC_LOCATION_R_SYS_RESOURCEUNLOCK;
void R_SYS_RegisterProtectEnable(e_system_reg_protect_t regs_to_protect)    FUNC_LOCATION_R_SYS_REGISTERPROTECTENABLE;
void R_SYS_RegisterProtectDisable(e_system_reg_protect_t regs_to_unprotect) FUNC_LOCATION_R_SYS_REGISTERPROTECTDISABLE;
void R_SYS_SoftwareDelay(uint32_t delay, e_system_delay_units_t units)      FUNC_LOCATION_R_SYS_SOFTWAREDELAY;
void R_SYS_CodeCopy(void)                       FUNC_LOCATION_R_SYS_CODECOPY;
int32_t r_sys_BoostFlagGet(bool * boost_flg)    FUNC_LOCATION_R_SYS_BOOSTFLAGGET;
int32_t r_sys_BoostFlagSet(void);
int32_t r_sys_BoostFlagClr(void)                FUNC_LOCATION_R_SYS_BOOSTFLAGCLR;
void IEL0_IRQHandler(void)                      FUNC_LOCATION_IEL0_IRQHANDLER;
void IEL1_IRQHandler(void)                      FUNC_LOCATION_IEL1_IRQHANDLER;
void IEL2_IRQHandler(void)                      FUNC_LOCATION_IEL2_IRQHANDLER;
void IEL3_IRQHandler(void)                      FUNC_LOCATION_IEL3_IRQHANDLER;
void IEL4_IRQHandler(void)                      FUNC_LOCATION_IEL4_IRQHANDLER;
void IEL5_IRQHandler(void)                      FUNC_LOCATION_IEL5_IRQHANDLER;
void IEL6_IRQHandler(void)                      FUNC_LOCATION_IEL6_IRQHANDLER;
void IEL7_IRQHandler(void)                      FUNC_LOCATION_IEL7_IRQHANDLER;
void IEL8_IRQHandler(void)                      FUNC_LOCATION_IEL8_IRQHANDLER;
void IEL9_IRQHandler(void)                      FUNC_LOCATION_IEL9_IRQHANDLER;
void IEL10_IRQHandler(void)                     FUNC_LOCATION_IEL10_IRQHANDLER;
void IEL11_IRQHandler(void)                     FUNC_LOCATION_IEL11_IRQHANDLER;
void IEL12_IRQHandler(void)                     FUNC_LOCATION_IEL12_IRQHANDLER;
void IEL13_IRQHandler(void)                     FUNC_LOCATION_IEL13_IRQHANDLER;
void IEL14_IRQHandler(void)                     FUNC_LOCATION_IEL14_IRQHANDLER;
void IEL15_IRQHandler(void)                     FUNC_LOCATION_IEL15_IRQHANDLER;
void IEL16_IRQHandler(void)                     FUNC_LOCATION_IEL16_IRQHANDLER;
void IEL17_IRQHandler(void)                     FUNC_LOCATION_IEL17_IRQHANDLER;
void IEL18_IRQHandler(void)                     FUNC_LOCATION_IEL18_IRQHANDLER;
void IEL19_IRQHandler(void)                     FUNC_LOCATION_IEL19_IRQHANDLER;
void IEL20_IRQHandler(void)                     FUNC_LOCATION_IEL20_IRQHANDLER;
void IEL21_IRQHandler(void)                     FUNC_LOCATION_IEL21_IRQHANDLER;
void IEL22_IRQHandler(void)                     FUNC_LOCATION_IEL22_IRQHANDLER;
void IEL23_IRQHandler(void)                     FUNC_LOCATION_IEL23_IRQHANDLER;
void IEL24_IRQHandler(void)                     FUNC_LOCATION_IEL24_IRQHANDLER;
void IEL25_IRQHandler(void)                     FUNC_LOCATION_IEL25_IRQHANDLER;
void IEL26_IRQHandler(void)                     FUNC_LOCATION_IEL26_IRQHANDLER;
void IEL27_IRQHandler(void)                     FUNC_LOCATION_IEL27_IRQHANDLER;
void IEL28_IRQHandler(void)                     FUNC_LOCATION_IEL28_IRQHANDLER;
void IEL29_IRQHandler(void)                     FUNC_LOCATION_IEL29_IRQHANDLER;
void IEL30_IRQHandler(void)                     FUNC_LOCATION_IEL30_IRQHANDLER;
void IEL31_IRQHandler(void)                     FUNC_LOCATION_IEL31_IRQHANDLER;
uint32_t R_SYS_GetVersion(void)                 FUNC_LOCATION_R_SYS_GETVERSION;
void r_system_wdt_refresh(void)                 FUNC_LOCATION_R_SYS_WDT_REFRESH;

/* Function Name : R_NVIC_EnableIRQ */
/*******************************************************************************************************************//**
 * @brief       Enable Interrupt
 * @details     Enables a device specific interrupt in the NVIC interrupt controller.
 * @param[in]   IRQn    Device specific interrupt number.
 * @note        IRQn must not be negative.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE void R_NVIC_EnableIRQ(IRQn_Type IRQn)  /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Enables a device specific interrupt in the NVIC interrupt controller. */
        __COMPILER_BARRIER();
        NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
        __COMPILER_BARRIER();
    }
}   /* End of function R_NVIC_EnableIRQ() */

/* Function Name : R_NVIC_GetEnableIRQ */
/*******************************************************************************************************************//**
 * @brief        Get Interrupt Enable status
 * @details      Returns a device specific interrupt enable status from the NVIC interrupt controller.
 * @param[in]    IRQn  Device specific interrupt number.
 * @retval       0  Interrupt is not enabled.
 * @retval       1  Interrupt is enabled.
 * @note         IRQn must not be negative.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE uint32_t R_NVIC_GetEnableIRQ(IRQn_Type IRQn)   /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Returns a device specific interrupt enable status from the NVIC interrupt controller. */
        return((uint32_t)(((NVIC->ISER[0U] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
    }
    else
    {
        return(0U);
    }
}   /* End of function R_NVIC_GetEnableIRQ() */

/* Function Name : R_NVIC_DisableIRQ */
/*******************************************************************************************************************//**
 * @brief        Disable Interrupt
 * @details      Disables a device specific interrupt in the NVIC interrupt controller.
 * @param[in]    IRQn  Device specific interrupt number.
 * @note         IRQn must not be negative.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE void R_NVIC_DisableIRQ(IRQn_Type IRQn) /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Disables a device specific interrupt in the NVIC interrupt controller. */
        NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
        __DSB();
        __ISB();
    }
}   /* End of function R_NVIC_DisableIRQ() */

/* Function Name : R_NVIC_GetPendingIRQ */
/*******************************************************************************************************************//**
 * @brief        Get Pending Interrupt
 * @details      Reads the NVIC pending register and returns the pending bit for the specified device specific interrupt.
 * @param [in]   IRQn  Device specific interrupt number.
 * @retval       0  Interrupt status is not pending.
 * @retval       1  Interrupt status is pending.
 * @note         IRQn must not be negative.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE uint32_t R_NVIC_GetPendingIRQ(IRQn_Type IRQn)  /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Reads the NVIC pending register and returns the pending bit for the specified device specific interrupt. */
        return((uint32_t)(((NVIC->ISPR[0U] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
    }
    else
    {
        return(0U);
    }
}   /* End of function R_NVIC_GetPendingIRQ() */

/* Function Name : R_NVIC_SetPendingIRQ */
/*******************************************************************************************************************//**
 * @brief        Set Pending Interrupt
 * @details      Sets the pending bit of a device specific interrupt in the NVIC pending register.
 * @param[in]    IRQn  Device specific interrupt number.
 * @note         IRQn must not be negative.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE void R_NVIC_SetPendingIRQ(IRQn_Type IRQn)  /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Sets the pending bit of a device specific interrupt in the NVIC pending register. */
        NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    }
}   /* End of function R_NVIC_SetPendingIRQ() */

/* Function Name : R_NVIC_ClearPendingIRQ */
/*******************************************************************************************************************//**
 * @brief        Clear Pending Interrupt
 * @details      Clears the pending bit of a device specific interrupt in the NVIC pending register.
 * @param[in]    IRQn  Device specific interrupt number.
 * @note         IRQn must not be negative.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE void R_NVIC_ClearPendingIRQ(IRQn_Type IRQn)    /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Clears the pending bit of a device specific interrupt in the NVIC pending register. */
        NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    }
}   /* End of function R_NVIC_ClearPendingIRQ() */

/* Function Name : R_NVIC_SetPriority */
/*******************************************************************************************************************//**
 * @brief        Set Interrupt Priority
 * @details      Sets the priority of a device specific interrupt or a processor exception.
 *               The interrupt number can be positive to specify a device specific interrupt,
 *               or negative to specify a processor exception.
 * @param[in]    IRQn  Interrupt number.
 * @param[in]    priority  Priority to set.
 * @note         The priority cannot be set for every processor exception.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE void R_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Sets the priority of a device specific interrupt or a processor exception. */
        NVIC->IP[_IP_IDX(IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
        (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
    }
    else
    {
        /* Sets the priority of a device specific interrupt or a processor exception. */
        SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
        (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
    }
}   /* End of function R_NVIC_SetPriority() */

/* Function Name : R_NVIC_GetPriority */
/*******************************************************************************************************************//**
 * @brief        Get Interrupt Priority
 * @details      Reads the priority of a device specific interrupt or a processor exception.
 *               The interrupt number can be positive to specify a device specific interrupt,
 *               or negative to specify a processor exception.
 * @param[in]    IRQn  Interrupt number.
 * @retval       Interrupt Priority.
 *               Value is aligned automatically to the implemented priority bits of the microcontroller.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE uint32_t R_NVIC_GetPriority(IRQn_Type IRQn)    /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    /* Execute only RE01 Group specific interrupt numbers. */
    if ((int32_t)(IRQn) >= 0)
    {
        /* Return the priority of a device specific interrupt or a processor exception. */
        return((uint32_t)(((NVIC->IP[ _IP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
    }
    else
    {
        /* Return the priority of a device specific interrupt or a processor exception. */
        return((uint32_t)(((SCB->SHP[_SHP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
    }
}   /* End of function R_NVIC_GetPriority() */

/* Function Name : R_NVIC_SetVector */
/*******************************************************************************************************************//**
 * @brief        Set Interrupt Vector
 * @details      Sets an interrupt vector in SRAM based interrupt vector table.
 *               The interrupt number can be positive to specify a device specific interrupt,
 *               or negative to specify a processor exception.
 *               VTOR must been relocated to SRAM before.
 *               If VTOR is not present address 0 must be mapped to SRAM.
 * @param[in]    IRQn      Interrupt number
 * @param[in]    vector    Address of interrupt handler function
 **********************************************************************************************************************/
__STATIC_FORCEINLINE void R_NVIC_SetVector(IRQn_Type IRQn, uint32_t vector) /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    /* Reads an interrupt vector from interrupt vector table. */
    uint32_t *p_vectors = (uint32_t *)SCB->VTOR;
    /* Sets an interrupt vector in SRAM based interrupt vector table. */
    p_vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
#else
    /* Reads an interrupt vector from interrupt vector table. */
    uint32_t *p_vectors = (uint32_t *)(NVIC_USER_IRQ_OFFSET << 2);
    /* Sets an interrupt vector in SRAM based interrupt vector table. */
    *(p_vectors + (int32_t)IRQn) = vector;
#endif
}   /* End of function R_NVIC_SetVector() */

/* Function Name : R_NVIC_GetVector */
/*******************************************************************************************************************//**
 * @brief        Get Interrupt Vector
 * @details      Reads an interrupt vector from interrupt vector table.
 *               The interrupt number can be positive to specify a device specific interrupt,
 *               or negative to specify a processor exception.
 * @param[in]    IRQn      Interrupt number.
 * @retval       Address of interrupt handler function
 **********************************************************************************************************************/
__STATIC_FORCEINLINE uint32_t R_NVIC_GetVector(IRQn_Type IRQn)  /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    /* Reads an interrupt vector from interrupt vector table. */
    uint32_t *p_vectors = (uint32_t *)SCB->VTOR;
    /* Return an address of interrupt handler function. */
    return p_vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET];
#else
    /* Reads an interrupt vector from interrupt vector table. */
    uint32_t *p_vectors = (uint32_t *)(NVIC_USER_IRQ_OFFSET << 2);
    /* Return an address of interrupt handler function. */
    return *(p_vectors + (int32_t)IRQn); 
#endif
}   /* End of function R_NVIC_GetVector() */

/* Function Name : R_NVIC_SystemReset */
/*******************************************************************************************************************//**
 * @brief   System Reset
 * @details Initiates a system reset request to reset the MCU.
 **********************************************************************************************************************/
__STATIC_FORCEINLINE void R_NVIC_SystemReset(void)  /* @suppress("Function declaration") */ /* @suppress("Source file naming") */
{
    __DSB();                                                          /* Ensure all outstanding memory accesses included
                                                                         buffered write are completed before reset */
    SCB->AIRCR  = ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                     SCB_AIRCR_SYSRESETREQ_Msk);
    __DSB();                                                          /* Ensure completion of memory access */
    
    for(;;)                                                           /* wait until reset */
    {
        __NOP();
    }
}   /* End of function R_NVIC_SystemReset() */

/*******************************************************************************************************************//**
 * @} (end defgroup grp_device_hal_system)
 **********************************************************************************************************************/

#endif /* R_SYSTEM_API_H_ */
/* End of file (r_system_api.h) */
