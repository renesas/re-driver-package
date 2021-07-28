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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_lpm_api.h
 * Version      : 1.00
 * Description  : Device HAL LPM (Low Power Mode)
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 18.12.2019 0.40     First release
 *           04.03.2020 0.80     Changed the version number and changed constant of LPM_VERSION_MINOR from 40 to 80.
 *           27.05.2020 1.00     Updated the version number and changed constant of LPM_VERSION_MAJOR from 0 to 1
 *                               and LPM_VERSION_MINOR from 80 to 00.
 *********************************************************************************************************************/

/******************************************************************************************************************//**
 @ingroup  grp_device_hal
 @defgroup grp_device_hal_lpm Device HAL LPM Implementation
 @brief The LPM (Low Power Mode) driver implementation for the HAL-Driver LPM interface to drive the LPM peripheral on
 RE Family.
 @section sec_hal_driver_lpm_summary LPM-Driver LPM Summary
 This is the HAL-Driver LPM implementation which drives the LPM peripheral on RE Family via the LPM-Driver interface.
 Use the instance @ref Driver_LPM to acccess to LPM peripheral. Do not use R_LPM_XXX APIs directly. 
 @code
 @endcode
 @section 
 @{
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_lpm_cfg.h"

#ifndef R_LPM_API_H_
#define R_LPM_API_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/* Version Number of API. */
#define LPM_VERSION_MAJOR                (1)
#define LPM_VERSION_MINOR                (00)

/**
 * @defgroup LPM_FAST_MSTPT_A
          Available for R_LPM_FastModuleStart() and R_LPM_FastModuleStop().
          Setting the module stop control. Multiple factors can be set by logical OR.
 */
/* @{ */
#define LPM_FAST_MSTP_A_DTC_DMAC      (0x00400000UL)          /*!< DTC and DMAC module */
#define LPM_FAST_MSTP_A_NONE          (0x00000000UL)          /*!< Set no module belonging to MSTP_A */
#define LPM_FAST_MSTP_A_ALL           (0x00400000UL)          /*!< Set all modules belonging to MSTP_A */

/* @} */

/**
 * @defgroup LPM_FAST_MSTPT_B
          Available for R_LPM_FastModuleStart() and R_LPM_FastModuleStop().
          Setting the module stop control. Multiple factors can be set by logical OR.
 */
/* @{ */
#define LPM_FAST_MSTP_B_IRDA          (0x00000020UL)          /*!< IrDA module */
#define LPM_FAST_MSTP_B_QSPI          (0x00000040UL)          /*!< QSPI module */
#define LPM_FAST_MSTP_B_RIIC1         (0x00000100UL)          /*!< RIIC module channel 1 */
#define LPM_FAST_MSTP_B_RIIC0         (0x00000200UL)          /*!< RIIC module channel 0 */
#define LPM_FAST_MSTP_B_SPI1          (0x00040000UL)          /*!< SPI module channel 1 */
#define LPM_FAST_MSTP_B_SPI0          (0x00080000UL)          /*!< SPI module channel 0 */
#define LPM_FAST_MSTP_B_SCI9          (0x00400000UL)          /*!< SCI module channel 9 */
#define LPM_FAST_MSTP_B_SCI5          (0x04000000UL)          /*!< SCI module channel 5 */
#define LPM_FAST_MSTP_B_SCI4          (0x08000000UL)          /*!< SCI module channel 4 */
#define LPM_FAST_MSTP_B_SCI3          (0x10000000UL)          /*!< SCI module channel 3 */
#define LPM_FAST_MSTP_B_SCI2          (0x20000000UL)          /*!< SCI module channel 2 */
#define LPM_FAST_MSTP_B_SCI1          (0x40000000UL)          /*!< SCI module channel 1 */
#define LPM_FAST_MSTP_B_SCI0          (0x80000000UL)          /*!< SCI module channel 0 */
#define LPM_FAST_MSTP_B_NONE          (0x00000000UL)          /*!< Set no module belonging to MSTP_B */
#define LPM_FAST_MSTP_B_ALL           (0xFC4C0360UL)          /*!< Set all modules belonging to MSTP_B */

/* @} */

/**
 * @defgroup LPM_FAST_MSTPT_C
          Available for R_LPM_FastModuleStart() and R_LPM_FastModuleStop().
          Setting the module stop control. Multiple factors can be set by logical OR.
 */
/* @{ */
#define LPM_FAST_MSTP_C_CAC           (0x00000001UL)          /*!< CAC module */
#define LPM_FAST_MSTP_C_CRC           (0x00000002UL)          /*!< CRC module */
#define LPM_FAST_MSTP_C_DOC           (0x00002000UL)          /*!< DOC module */
#define LPM_FAST_MSTP_C_ELC           (0x00004000UL)          /*!< ELC module */
#define LPM_FAST_MSTP_C_DIV           (0x00008000UL)          /*!< DIV module */
#define LPM_FAST_MSTP_C_DIL           (0x00400000UL)          /*!< DIL module */
#define LPM_FAST_MSTP_C_MLCD          (0x02000000UL)          /*!< MLCD module */
#define LPM_FAST_MSTP_C_GDT           (0x04000000UL)          /*!< GDT module */
#define LPM_FAST_MSTP_C_TSIPLITE      (0x80000000UL)          /*!< TSIP-Lite module */
#define LPM_FAST_MSTP_C_NONE          (0x00000000UL)          /*!< Set no module belonging to MSTP_C */
#define LPM_FAST_MSTP_C_ALL           (0x8640E003UL)          /*!< Set all modules belonging to MSTP_C */

/* @} */

/**
 * @defgroup LPM_FAST_MSTPT_D
          Available for R_LPM_FastModuleStart() and R_LPM_FastModuleStop().
          Setting the module stop control. Multiple factors can be set by logical OR.
 */
/* @{ */
#define LPM_FAST_MSTP_D_LST           (0x00000001UL)          /*!< LST module */
#define LPM_FAST_MSTP_D_TMR           (0x00000002UL)          /*!< TMR module */
#define LPM_FAST_MSTP_D_AGT1          (0x00000004UL)          /*!< AGT module channel 1 */
#define LPM_FAST_MSTP_D_AGT0          (0x00000008UL)          /*!< AGT module channel 0 */
#define LPM_FAST_MSTP_D_GPT320_321    (0x00000020UL)          /*!< GPT module (GPT320 and GPT321) */
#define LPM_FAST_MSTP_D_GPT162_165    (0x00000040UL)          /*!< GPT module (GPT162, GPT163, GPT164 and GPT165) */
#define LPM_FAST_MSTP_D_CCC           (0x00000080UL)          /*!< CCC module */
#define LPM_FAST_MSTP_D_RTC           (0x00000200UL)          /*!< RTC module */
#define LPM_FAST_MSTP_D_IWDT          (0x00000400UL)          /*!< IWDT module */
#define LPM_FAST_MSTP_D_WDT           (0x00000800UL)          /*!< WDT module */
#define LPM_FAST_MSTP_D_AGTW0         (0x00001000UL)          /*!< AGTW module channel 0 */
#define LPM_FAST_MSTP_D_AGTW1         (0x00002000UL)          /*!< AGTW module channel 1 */
#define LPM_FAST_MSTP_D_POEG          (0x00004000UL)          /*!< POEG module */
#define LPM_FAST_MSTP_D_S14AD         (0x00010000UL)          /*!< S14AD module */
#define LPM_FAST_MSTP_D_VREF          (0x00020000UL)          /*!< VREF module */
#define LPM_FAST_MSTP_D_WUPT          (0x00040000UL)          /*!< WUPT module */
#define LPM_FAST_MSTP_D_TEMPS         (0x00400000UL)          /*!< TEMPS module */
#define LPM_FAST_MSTP_D_NONE          (0x00000000UL)          /*!< Set no module belonging to MSTP_D */
#define LPM_FAST_MSTP_D_ALL           (0x00477EEFUL)          /*!< Set all modules belonging to MSTP_D */

/* @} */

/**
 * @defgroup LPM_DSTBY_RESET
          Available for R_LPM_DSTBYResetStatusGet().
          Getting of Deep Software Standby mode canceling factor. Multiple factors are set by logical OR.
 */
/* @{ */
#define LPM_DSTBY_RESET_IRQ0DS          (0x0001UL)          /*!< IRQ0-DS pin */
#define LPM_DSTBY_RESET_IRQ1DS          (0x0002UL)          /*!< IRQ1-DS pin */
#define LPM_DSTBY_RESET_IRQ2DS          (0x0004UL)          /*!< IRQ2-DS pin */
#define LPM_DSTBY_RESET_IRQ3DS          (0x0008UL)          /*!< IRQ3-DS pin */
#define LPM_DSTBY_RESET_LVD1            (0x0100UL)          /*!< LVD1 signal */
#define LPM_DSTBY_RESET_LVDBAT          (0x0200UL)          /*!< LVDBAT signal */
#define LPM_DSTBY_RESET_RTCI            (0x0400UL)          /*!< RTC interval interru signal */
#define LPM_DSTBY_RESET_RTCA            (0x0800UL)          /*!< RTC alarm interru signal */
#define LPM_DSTBY_RESET_NMI             (0x1000UL)          /*!< NMI pin */
#define LPM_DSTBY_RESET_CCC             (0x2000UL)          /*!< CCC interval interrupt singnal */
#define LPM_DSTBY_RESET_WUPT            (0x2000UL)          /*!< WUPT interval interrupt singnal */
#define LPM_DSTBY_RESET_NONE            (0x0000UL)          /*!< No factor (Default) */

/* @} */

/**
 * @defgroup LPM_RAM_RETENTION_AREA
          Available for R_LPM_RamRetentionSet().
          Setting the RAM shutdown area in Software Standby mode. Multiple Areas can be set by logical OR.
          When this input parameter is used to set retention area, it sets corresponding RAMSn bit of RAMSDCR to 
          0 (retention). Note that this input parameter and the value of RAMSDCR register are inverted.\n\n
          E.G: Retain RAM Area0 and Area1. And, shutdown other areas.\n
          R_LPM_RamRetentionSet(LPM_RAM_RETENTION_AREA0 | LPM_RAM_RETENTION_AREA1);
 */
/* @{ */
#define LPM_RAM_RETENTION_AREA0         (0x01UL)          /*!< The data in the following address is retained. 0x20000000 - 0x20007FFF */
#define LPM_RAM_RETENTION_AREA1         (0x02UL)          /*!< The data in the following address is retained. 0x20008000 - 0x2000FFFF */
#define LPM_RAM_RETENTION_AREA2         (0x04UL)          /*!< The data in the following address is retained. 0x20010000 - 0x20017FFF */
#define LPM_RAM_RETENTION_AREA3         (0x08UL)          /*!< The data in the following address is retained. 0x20018000 - 0x2001FFFF */
#define LPM_RAM_RETENTION_NONE          (0x00UL)          /*!< Set no factor (Default) */
#define LPM_RAM_RETENTION_ALL           (0x0FUL)          /*!< Set all factor */

/* @} */

   
/**
 * @defgroup LPM_SSTBY_WUP
          Available for R_LPM_SSTBYModeSetup().
          Setting the interrupt factor to return to operation mode from Software Standby mode. Multiple factors can be
          set by logical OR.
 */
/* @{ */
#define LPM_SSTBY_WUP_IRQ0              (0x00000001UL)          /*!< IRQ0 interrupt */
#define LPM_SSTBY_WUP_IRQ1              (0x00000002UL)          /*!< IRQ1 interrupt */
#define LPM_SSTBY_WUP_IRQ2              (0x00000004UL)          /*!< IRQ2 interrupt */
#define LPM_SSTBY_WUP_IRQ3              (0x00000008UL)          /*!< IRQ3 interrupt */
#define LPM_SSTBY_WUP_IRQ4              (0x00000010UL)          /*!< IRQ4 interrupt */
#define LPM_SSTBY_WUP_IRQ5              (0x00000020UL)          /*!< IRQ5 interrupt */
#define LPM_SSTBY_WUP_IRQ6              (0x00000040UL)          /*!< IRQ6 interrupt */
#define LPM_SSTBY_WUP_IRQ7              (0x00000080UL)          /*!< IRQ7 interrupt */
#define LPM_SSTBY_WUP_IRQ8              (0x00000100UL)          /*!< IRQ8 interrupt */
#define LPM_SSTBY_WUP_IRQ9              (0x00000200UL)          /*!< IRQ9 interrupt */
#define LPM_SSTBY_WUP_IWDT              (0x00010000UL)          /*!< IWDT interrupt */
#define LPM_SSTBY_WUP_KEY               (0x00020000UL)          /*!< KEY interrupt */
#define LPM_SSTBY_WUP_LVD1              (0x00040000UL)          /*!< LVD1 interrupt */
#define LPM_SSTBY_WUP_LVDBAT            (0x00080000UL)          /*!< LVDBAT interrupt */
#define LPM_SSTBY_WUP_SOLDH             (0x00400000UL)          /*!< SOL_DH interrupt */
#define LPM_SSTBY_WUP_AGTW0CA           (0x00800000UL)          /*!< AGTW0 compare match A interrupt */
#define LPM_SSTBY_WUP_RTCALM            (0x01000000UL)          /*!< RTC alarm interrupt */
#define LPM_SSTBY_WUP_RTCPRD            (0x02000000UL)          /*!< RTC interval interrupt */
#define LPM_SSTBY_WUP_CCCPRD            (0x04000000UL)          /*!< CCC interval interrupt */
#define LPM_SSTBY_WUP_WUPTOVI           (0x04000000UL)          /*!< WUPT interval interrupt */
#define LPM_SSTBY_WUP_AGTW1CA           (0x08000000UL)          /*!< AGTW1 compare match A interrupt */
#define LPM_SSTBY_WUP_AGT1UD            (0x10000000UL)          /*!< AGT1 underflow interrupt */
#define LPM_SSTBY_WUP_AGT1CA            (0x20000000UL)          /*!< AGT1 compare match A interrupt */
#define LPM_SSTBY_WUP_AGT0CA            (0x40000000UL)          /*!< AGT0 compare match A interrupt */
#define LPM_SSTBY_WUP_SOLDL             (0x80000000UL)          /*!< SOL_DL interrupt */
#define LPM_SSTBY_WUP_NONE              (0x00000000UL)          /*!< Set no factor (Default) */
#define LPM_SSTBY_WUP_ALL               (0xFFCF03FFUL)          /*!< Set all factor */

/* @} */

/**
 * @defgroup LPM_DSTBY_WUP
          Available for R_LPM_DSTBYModeSetup().
          Setting the interrupt factor for canceling Deep Software Standby mode. Multiple factors can be set by logical
          OR.
 */
/* @{ */
#define LPM_DSTBY_WUP_IRQ0DS            (0x0001UL)          /*!< IRQ0-DS pin */
#define LPM_DSTBY_WUP_IRQ1DS            (0x0002UL)          /*!< IRQ1-DS pin */
#define LPM_DSTBY_WUP_IRQ2DS            (0x0004UL)          /*!< IRQ2-DS pin */
#define LPM_DSTBY_WUP_IRQ3DS            (0x0008UL)          /*!< IRQ3-DS pin */
#define LPM_DSTBY_WUP_LVD1              (0x0100UL)          /*!< LVD1 signal */
#define LPM_DSTBY_WUP_LVDBAT            (0x0200UL)          /*!< LVDBAT signal */
#define LPM_DSTBY_WUP_RTCI              (0x0400UL)          /*!< RTC interval interru signal */
#define LPM_DSTBY_WUP_RTCA              (0x0800UL)          /*!< RTC alarm interru signal */
#define LPM_DSTBY_WUP_NMI               (0x1000UL)          /*!< NMI pin */
#define LPM_DSTBY_WUP_CCC               (0x2000UL)          /*!< CCC interval interrupt singnal */
#define LPM_DSTBY_WUP_WUPT              (0x2000UL)          /*!< WUPT interval interrupt singnal */
#define LPM_DSTBY_WUP_NONE              (0x0000UL)          /*!< Set no factor (Default) */
#define LPM_DSTBY_WUP_ALL               (0x3F0FUL)          /*!< Set all factor */

/* @} */

/**
 * @defgroup LPM_DSTBY_WUP_EDGE
          Available for R_LPM_DSTBYModeSetup().
          Setting of rising edge of interrupt factor to cancel Deep Software Standby mode. Multiple factors can be set 
          by logical OR.
 */
/* @{ */
#define LPM_DSTBY_WUP_EDGE_IRQ0DS       (0x0001UL)          /*!< Rising edge of IRQ0-DS pin */
#define LPM_DSTBY_WUP_EDGE_IRQ1DS       (0x0002UL)          /*!< Rising edge of IRQ1-DS pin */
#define LPM_DSTBY_WUP_EDGE_IRQ2DS       (0x0004UL)          /*!< Rising edge of IRQ2-DS pin */
#define LPM_DSTBY_WUP_EDGE_IRQ3DS       (0x0008UL)          /*!< Rising edge of IRQ3-DS pin */
#define LPM_DSTBY_WUP_EDGE_LVD1         (0x0100UL)          /*!< Rising edge of LVD1 signal */
#define LPM_DSTBY_WUP_EDGE_LVDBAT       (0x0200UL)          /*!< Rising edge of LVDBAT signal */
#define LPM_DSTBY_WUP_EDGE_NMI          (0x1000UL)          /*!< Rising edge of NMI pin */
#define LPM_DSTBY_WUP_EDGE_NONE         (0x0000UL)          /*!< Set no rising edge (Default) */
#define LPM_DSTBY_WUP_EDGE_ALL          (0x130FUL)          /*!< Set rising edge for all factor */

/* @} */

/**
 * @defgroup LPM_SNZ_REQ
          Available for R_LPM_SnoozeSet().
          Setting the interrupt factor to return to Snooze mode from Software Standby mode. Multiple factors can be set
          by logical OR.
 */
/* @{ */
#define LPM_SNZ_REQ_IRQ0                (0x00000001UL)          /*!< IRQ0 interrupt */
#define LPM_SNZ_REQ_IRQ1                (0x00000002UL)          /*!< IRQ1 interrupt */
#define LPM_SNZ_REQ_IRQ2                (0x00000004UL)          /*!< IRQ2 interrupt */
#define LPM_SNZ_REQ_IRQ3                (0x00000008UL)          /*!< IRQ3 interrupt */
#define LPM_SNZ_REQ_IRQ4                (0x00000010UL)          /*!< IRQ4 interrupt */
#define LPM_SNZ_REQ_IRQ5                (0x00000020UL)          /*!< IRQ5 interrupt */
#define LPM_SNZ_REQ_IRQ6                (0x00000040UL)          /*!< IRQ6 interrupt */
#define LPM_SNZ_REQ_IRQ7                (0x00000080UL)          /*!< IRQ7 interrupt */
#define LPM_SNZ_REQ_KINT                (0x00020000UL)          /*!< KINT interrupt */
#define LPM_SNZ_REQ_AGTW0CA             (0x00800000UL)          /*!< AGTW0 compare match A interrupt */
#define LPM_SNZ_REQ_RTCALM              (0x01000000UL)          /*!< RTC alarm interrupt */
#define LPM_SNZ_REQ_RTCPRD              (0x02000000UL)          /*!< RTC interval interrupt */
#define LPM_SNZ_REQ_AGT1UD              (0x10000000UL)          /*!< AGT1 underflow interrupt */
#define LPM_SNZ_REQ_AGT1CA              (0x20000000UL)          /*!< AGT1 compare match A interrupt */
#define LPM_SNZ_REQ_AGT0CA              (0x40000000UL)          /*!< AGT0 compare match A interrupt */
#define LPM_SNZ_REQ_NONE                (0x00000000UL)          /*!< Set no factor (Default) */
#define LPM_SNZ_REQ_ALL                 (0x738200FFUL)          /*!< Set all factor */

/* @} */

/**
 * @defgroup LPM_SNZ_SSTBY_REQ
          Available for R_LPM_SnoozeSet().
          Setting the event factor to return to Software Standby mode from Snooze mode. Multiple factors can be set 
          by logical OR.
 */
/* @{ */
#define LPM_SNZ_SSTBY_AGT1UD            (0x01UL)          /*!< AGT1 underflow */
#define LPM_SNZ_SSTBY_DTCZ              (0x02UL)          /*!< Last DTC transmission completed */
#define LPM_SNZ_SSTBY_DTCN              (0x04UL)          /*!< DTC transmission completed except the last */
#define LPM_SNZ_SSTBY_AD0MAT            (0x08UL)          /*!< AD compare match */
#define LPM_SNZ_SSTBY_AD0UMT            (0x10UL)          /*!< AD compare mismatch */
#define LPM_SNZ_SSTBY_SCI0              (0x80UL)          /*!< SCI0 address mismatch */
#define LPM_SNZ_SSTBY_NONE              (0x00UL)          /*!< Set no factor (Default) */
#define LPM_SNZ_SSTBY_ALL               (0x9FUL)          /*!< Set all factor */

/* @} */

/**
 * @defgroup LPM_IOPOWER_SUPPLY
          Available for R_LPM_IOPowerSupplyModeSet() and R_LPM_IOPowerSupplyModeGet().
          Setting to enable the function to prevent the propagation of undefined signals from IO power supply domains that
          are not supplied power to other IO power supply domains that supplied power. Multiple factors can be set by logical OR.\n\n
          For example. If AVCC0 and IOVCC1 domains cannot be supplied then please set argument "LPM_IOPOWER_SUPPLY_AVCC0"
          and "LPM_IOPOWER_SUPPLY_IOVCC1" for R_LPM_IOPowerSupplyModeSet().\n
          E.g.:Enable the function for AVCC0 and IOVCC1 domain. Other domains are set to disabled.\n
          R_LPM_IOPowerSupplyModeSet(LPM_IOPOWER_SUPPLY_AVCC0 | LPM_IOPOWER_SUPPLY_IOVCC1);
 */
/* @{ */
#define LPM_IOPOWER_SUPPLY_AVCC0        (0x01)            /*!< when power cannot be supplied to AVCC0   : P000-P007 */
#define LPM_IOPOWER_SUPPLY_IOVCC0       (0x04)            /*!< when power cannot be supplied to IOCC0   : PORT8, P010-P015 */
#define LPM_IOPOWER_SUPPLY_IOVCC1       (0x08)            /*!< when power cannot be supplied to IOCC1   : PORT1, PORT3, PORT5, PORT6, PORT7,P202-P205 */
#define LPM_IOPOWER_SUPPLY_NONE         (0x00)            /*!< when power can be supplied to all power domain : Set no factor */
#define LPM_IOPOWER_SUPPLY_ALL          (0x0D)            /*!< when power can NOT be supplied to any power domain other than VCC/IOVCC : Set all factor (Default) */

/* @} */

/**
 * @defgroup internal
 */
/* @{ */
/** Define the location of each functions. */
#if (LPM_CFG_SECTION_R_LPM_GETVERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_GETVERSION                      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_GETVERSION
#endif
#if (LPM_CFG_SECTION_R_LPM_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_INITIALIZE                      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_INITIALIZE
#endif
#if (LPM_CFG_SECTION_R_LPM_MODULESTART == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_MODULESTART                     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_MODULESTART
#endif
#if (LPM_CFG_SECTION_R_LPM_MODULESTOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_MODULESTOP                      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_MODULESTOP
#endif
#if (LPM_CFG_SECTION_R_LPM_FASTMODULESTART == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_FASTMODULESTART                 __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_FASTMODULESTART
#endif
#if (LPM_CFG_SECTION_R_LPM_FASTMODULESTOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_FASTMODULESTOP                  __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_FASTMODULESTOP
#endif
#if (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEALLPWONSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEALLPWONSET              __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEALLPWONSET
#endif
#if (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEEXFPWONSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEEXFPWONSET              __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEEXFPWONSET
#endif
#if (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEMINPWONSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEMINPWONSET              __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEMINPWONSET
#endif
#if (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEGET              __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_POWERSUPPLYMODEGET
#endif
#if (LPM_CFG_SECTION_R_LPM_BACKBIASMODEENABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_BACKBIASMODEENABLE              __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_BACKBIASMODEENABLE
#endif
#if (LPM_CFG_SECTION_R_LPM_BACKBIASMODEDISABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_BACKBIASMODEDISABLE             __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_BACKBIASMODEDISABLE
#endif
#if (LPM_CFG_SECTION_R_LPM_BACKBIASMODEENABLESTATUSGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_BACKBIASMODEENABLESTATUSGET     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_BACKBIASMODEENABLESTATUSGET
#endif
#if (LPM_CFG_SECTION_R_LPM_BACKBIASMODEENTRY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_BACKBIASMODEENTRY               __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_BACKBIASMODEENTRY
#endif
#if (LPM_CFG_SECTION_R_LPM_BACKBIASMODEEXIT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_BACKBIASMODEEXIT                __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_BACKBIASMODEEXIT
#endif
#if (LPM_CFG_SECTION_R_LPM_BACKBIASMODEGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_BACKBIASMODEGET                 __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_BACKBIASMODEGET
#endif
#if (LPM_CFG_SECTION_R_LPM_SLEEPMODEENTRY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_SLEEPMODEENTRY                  __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_SLEEPMODEENTRY
#endif
#if (LPM_CFG_SECTION_R_LPM_SSTBYMODEENTRY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_SSTBYMODEENTRY        __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_SSTBYMODEENTRY
#endif
#if (LPM_CFG_SECTION_R_LPM_SSTBYMODESETUP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_SSTBYMODESETUP        __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_SSTBYMODESETUP
#endif
#if (LPM_CFG_SECTION_R_LPM_DSTBYMODEENTRY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_DSTBYMODEENTRY    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_DSTBYMODEENTRY
#endif
#if (LPM_CFG_SECTION_R_LPM_DSTBYMODESETUP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_DSTBYMODESETUP    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_DSTBYMODESETUP
#endif
#if (LPM_CFG_SECTION_R_LPM_DSTBYRESETSTATUSGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_DSTBYRESETSTATUSGET   __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_DSTBYRESETSTATUSGET
#endif
#if (LPM_CFG_SECTION_R_LPM_RAMRETENTIONSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_RAMRETENTIONSET                 __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_RAMRETENTIONSET
#endif
#if (LPM_CFG_SECTION_R_LPM_SNOOZESET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_SNOOZESET                       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_SNOOZESET
#endif
#if (LPM_CFG_SECTION_R_LPM_IOPOWERSUPPLYMODESET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_IOPOWERSUPPLYMODESET            __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_IOPOWERSUPPLYMODESET
#endif
#if (LPM_CFG_SECTION_R_LPM_IOPOWERSUPPLYMODEGET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_IOPOWERSUPPLYMODEGET            __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_IOPOWERSUPPLYMODEGET
#endif
#if (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEEXFPWONSET == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEALLPWONSET == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_SSTBYMODEENTRY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_LPM_WAITRELEASERESETISO2     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_LPM_WAITRELEASERESETISO2
#endif

/* @} */

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/** @brief Select module to enables or disable clocks to the peripheral and allows registers to be set.  */
typedef enum e_lpm_mstp
{
    LPM_MSTP_DTC_DMAC,          /*!< DTC and DMAC module */
    LPM_MSTP_IRDA,              /*!< IrDA module */
    LPM_MSTP_QSPI,              /*!< QSPI module */
    LPM_MSTP_RIIC1,             /*!< RIIC module channel 1 */
    LPM_MSTP_RIIC0,             /*!< RIIC module channel 0 */
    LPM_MSTP_SPI1,              /*!< SPI module channel 1 */
    LPM_MSTP_SPI0,              /*!< SPI module channel 0 */
    LPM_MSTP_SCI9,              /*!< SCI module channel 9 */
    LPM_MSTP_SCI5,              /*!< SCI module channel 5 */
    LPM_MSTP_SCI4,              /*!< SCI module channel 4 */
    LPM_MSTP_SCI3,              /*!< SCI module channel 3 */
    LPM_MSTP_SCI2,              /*!< SCI module channel 2 */
    LPM_MSTP_SCI1,              /*!< SCI module channel 1 */
    LPM_MSTP_SCI0,              /*!< SCI module channel 0 */
    LPM_MSTP_CAC,               /*!< CAC module */
    LPM_MSTP_CRC,               /*!< CRC module */
    LPM_MSTP_DOC,               /*!< DOC module */
    LPM_MSTP_ELC,               /*!< ELC module */
    LPM_MSTP_DIV,               /*!< DIV module */
    LPM_MSTP_DIL,               /*!< DIL module */
    LPM_MSTP_MLCD,              /*!< MLCD module */
    LPM_MSTP_GDT,               /*!< GDT module */
    LPM_MSTP_TSIP_LITE,         /*!< TSIP-Lite module */
    LPM_MSTP_LST,               /*!< LST module */
    LPM_MSTP_TMR,               /*!< TMR module */
    LPM_MSTP_AGT1,              /*!< AGT module channel 1 */
    LPM_MSTP_AGT0,              /*!< AGT module channel 0 */
    LPM_MSTP_GPT320_321,        /*!< GPT module (GPT320 and GPT321) */
    LPM_MSTP_GPT162_165,        /*!< GPT module (GPT162, GPT163, GPT164 and GPT165) */
    LPM_MSTP_CCC,               /*!< CCC module */
    LPM_MSTP_RTC,               /*!< RTC module */
    LPM_MSTP_IWDT,              /*!< IWDT module */
    LPM_MSTP_WDT,               /*!< WDT module */
    LPM_MSTP_AGTW0,             /*!< AGTW module  channel 0 */
    LPM_MSTP_AGTW1,             /*!< AGTW module  channel 1 */
    LPM_MSTP_POEG,              /*!< POEG module */
    LPM_MSTP_S14AD,             /*!< S14AD module */
    LPM_MSTP_VREF,              /*!< VREF module */
    LPM_MSTP_WUPT,              /*!< WUPT module */
    LPM_MSTP_TEMPS,             /*!< TEMPS module */
    LPM_MSTP_NUM                /*!< For error checking */
} e_lpm_mstp_t;

/**
 * @addtogroup configuration_of_snooze
 */
/* @{ */
/** @brief Select Snooze enable. */
typedef enum e_lpm_snz_enable
{
    LPM_SNZ_DISABLE     = 0,        /*!< Disable (Default) */
    LPM_SNZ_ENABLE      = 1         /*!< Enable */
} e_lpm_snz_enable_t;

/* @} */

/**
 * @addtogroup configuration_of_snooze
 */
/* @{ */
/** @brief Select RXD0 falling edge as trigger of snooze end request to return to Software Standby mode. */
typedef enum e_lpm_snz_rx0_falling_edge
{
    LPM_RX0_FALLING_EDGE_DISABLE    = 0,     /*!< Do not use RXD0 falling edge as trigger for snooze end request to return to software standby mode. (Default) */
    LPM_RX0_FALLING_EDGE_ENABLE     = 1      /*!< Use RXD0 falling edge as trigger for snooze end request to return to software standby mode.  */
} e_lpm_snz_rx0_falling_edge_t;

/* @} */

/**
 * @addtogroup configuration_of_snooze
 */
/* @{ */
/** @brief Select DTC operation enable in Snooze mode. */
typedef enum e_lpm_snz_dtc_enable
{
    LPM_SNZ_DTC_DISABLE = 0,     /*!< Disable DTC operation in Snooze mode. (Default) */
    LPM_SNZ_DTC_ENABLE  = 2      /*!< Enable DTC operation in Snooze mode.  */
} e_lpm_snz_dtc_enable_t;

/* @} */

/** @brief Power supply mode select */
typedef enum e_lpm_power_supply_mode
{
    LPM_POWER_SUPPLY_MODE_ALLPWON = 1,      /*!< ALLPWON mode (Default) */
    LPM_POWER_SUPPLY_MODE_EXFPWON = 2,      /*!< EXFPWON mode */
    LPM_POWER_SUPPLY_MODE_MINPWON = 4       /*!< MINPWON mode */
} e_lpm_power_supply_mode_t;


/** @brief Back Bias mode enable status */
typedef enum e_lpm_vbb_enable_status
{
    LPM_VBB_DISABLED = 0,      /*!< VBB Disabled (Default) */
    LPM_VBB_ENABLED  = 1       /*!< VBB Enabled */
} e_lpm_vbb_enable_status_t;

/**
 * @addtogroup configuration_of_sstby
 */
/* @{ */
/** @brief Select power supply mode and Back Bias mode when transitioning from opration mode to Software Standby mode. */
typedef enum e_lpm_sstby_power_supply_mode
{
    LPM_OPE_TO_SSTBY_EXFPWON     = 0,    /*!< Transition to EXFPWON Software Standby mode. (Default) */
    LPM_OPE_TO_SSTBY_MINPWON     = 1,    /*!< Transition to MINPWON Software Standby mode.  */
    LPM_OPE_TO_SSTBY_EXFPWON_VBB = 2,    /*!< Transition to EXFPWON VBB Software Standby mode.  */
    LPM_OPE_TO_SSTBY_MINPWON_VBB = 3     /*!< Transition to MINPWON VBB Software Standby mode.  */
} e_lpm_sstby_power_supply_mode_t;

/* @} */

/**
 * @addtogroup configuration_of_sstby
 */
/* @{ */
/** @brief Select taransiton speed between SSTBY mode and operation mode. */
typedef enum e_lpm_sstby_speed_mode
{
    LPM_SSTBY_SPEED_MODE_OFF = 0,    /*!< Transition time between SSTBY mode and operation mode is not shortened. (Default) */
    LPM_SSTBY_SPEED_MODE_ON  = 1     /*!< Transition time between SSTBY mode and operation mode is shortened. */
} e_lpm_sstby_speed_mode_t;

/* @} */

/** @brief Clock select for Back Bias control */
typedef enum e_lpm_vbb_clock
{
    LPM_VBB_CLOCK_LOCO  = 0,                /*!< Use LOCO (low speed on chip oscillator) for Back Bias control clock (Default) */
    LPM_VBB_CLOCK_SOSC  = 1                 /*!< Use SOSC (subclock oscillator) for Back Bias control clock */
} e_lpm_vbb_clock_t;

/** @brief Select Pin States when Deep Software Standby Mode is Canceled */
typedef enum e_lpm_dstby_io
{
    LPM_DSTBY_IO_CLEAR  = 0x00,         /*!< The I/O ports are initialized by an internal reset generated when Deep Software Standby mode is canceled. (Default) */
    LPM_DSTBY_IO_KEEP   = 0x40          /*!< The I/O ports states remain unchanged from Software Standby mode when Deep Software Standby mode is canceled.  */
} e_lpm_dstby_io_t;

/** @brief The state in the VBB mode is indicated. */
typedef enum e_lpm_backbias_mode
{
    LPM_BACKBIAS_MODE_NORMAL    = 0,    /*!< Normal mode status (Default) */
    LPM_BACKBIAS_MODE_VBB       = 1     /*!< VBB mode status */
} e_lpm_backbias_mode_t;

/**
 * @addtogroup configuration_of_snooze
 */
/* @{ */
/** @brief The interrupt factor to return to operation mode from snooze mode is selected. */
typedef enum e_lpm_snz_wup
{
    LPM_SNZ_WUP_DTC             = 0x0D,     /*!< Last DTC Transmission Completion Snooze End Enable */
    LPM_SNZ_WUP_ADC140_WCMPM    = 0x27,     /*!< AD Compare Match Snooze End Enable */
    LPM_SNZ_WUP_ADC140_WCMPUM   = 0x28,     /*!< AD Compare Mismatch Snooze End Enable */
    LPM_SNZ_WUP_DOC_DOPCI       = 0x38,     /*!< Data operation circuit interrupt Snooze End Enable */
    LPM_SNZ_WUP_SCI0_AM         = 0x79,     /*!< SCI0 Address Mismatch Snooze End Enable */
    LPM_SNZ_WUP_SCI0_RXI_OR_ERI = 0x7A,     /*!< SCI0 Receive data full and Receive error event Snooze End Enable */
    LPM_SNZ_WUP_NONE            = 0x00      /*!< Do not set any additional setting (Default) */
} e_lpm_snz_wup_t;

/* @} */

/**
 * @defgroup configuration_of_sstby
          Setting related to Software Standby mode.
 */
/* @{ */
/** @brief Setting related to Software Standby mode. */
typedef struct st_lpm_sstby_cfg
{
    e_lpm_sstby_power_supply_mode_t power_supply;   /*!< Select power supply mode and Back Bias mode when transitioning from opration mode to Software Standby mode.  */
    e_lpm_sstby_speed_mode_t        speed;          /*!< Select taransiton speed between SSTBY mode and operation mode.  */
    uint32_t                        wup;            /*!< Setting the interrupt factor to return to operation mode from Software Standby mode. (Refer to @ref LPM_SSTBY_WUP for setting value.) */
} st_lpm_sstby_cfg_t;

/* @} */

/**
 * @defgroup configuration_of_dstby
          Setting related to Deep Software Standby mode.
 */
/* @{ */
/** @brief Setting related to Deep Software Standby mode. */
typedef struct st_lpm_dstby_cfg
{
    uint32_t     wup;                                        /*!< Setting the interrupt factor for canceling Deep Software Standby mode. (Refer to @ref LPM_DSTBY_WUP for setting value.) */
    uint32_t     wup_edge;                                   /*!< Setting the interrupt factor edge for canceling Deep Software Standby mode. (Refer to @ref LPM_DSTBY_WUP_EDGE for setting value.) 0: Fall( or VCC<Vdet1), 1: Rise( or VCC <= Vdet1) */
} st_lpm_dstby_cfg_t;

/* @} */

/**
 * @defgroup configuration_of_snooze
          Setting relaed to Snooze mode.
 */
/* @{ */
/** @brief Setting relaed to Snooze mode. */
typedef struct st_lpm_snooze_cfg
{
    e_lpm_snz_enable_t              enable;             /*!< Setting whether to enable transition from Software Standby mode to Snooze mode.  */
    uint32_t                        req;                /*!< Setting the interrupt factor to return to Snooze mode from Software Standby mode. (Refer to @ref LPM_SNZ_REQ for setting value.) */
    uint32_t                        snz_sstby;          /*!< Setting the event factor to return to Software Standby mode from Snooze mode. (Refer to @ref LPM_SNZ_SSTBY_REQ for setting value.) */
    e_lpm_snz_wup_t                 wup;                /*!< Setting the interrupt factor to return to operation mode from Snooze mode.  */
    e_lpm_snz_dtc_enable_t          dtc_enable;         /*!< Setting whether to enable DTC operation in Snooze mode.  */
    e_lpm_snz_rx0_falling_edge_t    req_rx0;            /*!< Setting whether to use the falling edge of RXD0 as trigger for snooze end request to return to software standby mode.  */
} st_lpm_snooze_cfg_t;

/* @} */

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 *********************************************************************************************************************/

uint32_t R_LPM_GetVersion (void)                                    FUNC_LOCATION_R_LPM_GETVERSION;
void R_LPM_Initialize (void)                                        FUNC_LOCATION_R_LPM_INITIALIZE;
int32_t R_LPM_ModuleStart(e_lpm_mstp_t module)                      FUNC_LOCATION_R_LPM_MODULESTART;
int32_t R_LPM_ModuleStop(e_lpm_mstp_t module)                       FUNC_LOCATION_R_LPM_MODULESTOP;
int32_t R_LPM_FastModuleStart(uint32_t module_a, uint32_t module_b, uint32_t module_c, uint32_t module_d) FUNC_LOCATION_R_LPM_FASTMODULESTART;
int32_t R_LPM_FastModuleStop(uint32_t module_a, uint32_t module_b, uint32_t module_c, uint32_t module_d) FUNC_LOCATION_R_LPM_FASTMODULESTOP;
int32_t R_LPM_PowerSupplyModeAllpwonSet(void)                       FUNC_LOCATION_R_LPM_POWERSUPPLYMODEALLPWONSET;
int32_t R_LPM_PowerSupplyModeExfpwonSet(void)                       FUNC_LOCATION_R_LPM_POWERSUPPLYMODEEXFPWONSET;
int32_t R_LPM_PowerSupplyModeMinpwonSet(void)                       FUNC_LOCATION_R_LPM_POWERSUPPLYMODEMINPWONSET;
e_lpm_power_supply_mode_t R_LPM_PowerSupplyModeGet(void)            FUNC_LOCATION_R_LPM_POWERSUPPLYMODEGET;
int32_t R_LPM_BackBiasModeEnable(e_lpm_vbb_clock_t clock)           FUNC_LOCATION_R_LPM_BACKBIASMODEENABLE;
int32_t R_LPM_BackBiasModeDisable(void)                             FUNC_LOCATION_R_LPM_BACKBIASMODEDISABLE;
e_lpm_vbb_enable_status_t R_LPM_BackBiasModeEnableStatusGet(void)   FUNC_LOCATION_R_LPM_BACKBIASMODEENABLESTATUSGET;
int32_t R_LPM_BackBiasModeEntry(void)                               FUNC_LOCATION_R_LPM_BACKBIASMODEENTRY;
int32_t R_LPM_BackBiasModeExit(void)                                FUNC_LOCATION_R_LPM_BACKBIASMODEEXIT;
e_lpm_backbias_mode_t R_LPM_BackBiasModeGet(void)                   FUNC_LOCATION_R_LPM_BACKBIASMODEGET;
void R_LPM_SleepModeEntry(void)                                     FUNC_LOCATION_R_LPM_SLEEPMODEENTRY;
int32_t R_LPM_SSTBYModeEntry(void)                                  FUNC_LOCATION_R_LPM_SSTBYMODEENTRY;
int32_t R_LPM_SSTBYModeSetup(st_lpm_sstby_cfg_t * p_cfg)            FUNC_LOCATION_R_LPM_SSTBYMODESETUP;
int32_t R_LPM_DSTBYModeEntry(e_lpm_dstby_io_t io)                   FUNC_LOCATION_R_LPM_DSTBYMODEENTRY;
int32_t R_LPM_DSTBYModeSetup(st_lpm_dstby_cfg_t * p_cfg)            FUNC_LOCATION_R_LPM_DSTBYMODESETUP;
uint32_t R_LPM_DSTBYResetStatusGet(void)                            FUNC_LOCATION_R_LPM_DSTBYRESETSTATUSGET;
void R_LPM_RamRetentionSet(uint32_t area)                           FUNC_LOCATION_R_LPM_RAMRETENTIONSET;
int32_t R_LPM_SnoozeSet(st_lpm_snooze_cfg_t * p_cfg)                FUNC_LOCATION_R_LPM_SNOOZESET;
int32_t R_LPM_IOPowerSupplyModeSet(uint8_t vocr)                    FUNC_LOCATION_R_LPM_IOPOWERSUPPLYMODESET;
uint8_t R_LPM_IOPowerSupplyModeGet(void)                            FUNC_LOCATION_R_LPM_IOPOWERSUPPLYMODEGET;
void r_lpm_wait_release_reset_iso2(void)                            FUNC_LOCATION_R_LPM_WAITRELEASERESETISO2;

/******************************************************************************************************************//**
 * @} (end defgroup grp_device_hal_lpm)
 *********************************************************************************************************************/

#endif /* R_LPM_API_H_ */
/* End of file (r_lpm_api.h) */

