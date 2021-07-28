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
 * File Name    : r_system_cfg.h
 * Version      : 1.30
 * Description  : Device HAL SYSTEM (System Control, Common code among modules)
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 21.08.2018 0.50     First release
 *         : 06.11.2018 0.51     Update comments
 *         : 08.01.2019 0.60     OFS_SECURITR_MPU_REGISTER moved. 
 *                               In order to used OSF1 register at CMSIS-CORE,
 *                               it has moved out of "r_system_cfg.h".
 *                               Deleted Unique ID code macro.
 *                               Adapted to GSCE coding rules.
 *                               Update Doxygen comments.
 *                               Unified default value of "SYSTEM_CFG_EVENT_NUMBER_XXXX"
 *                               to "SYSTEM_IRQ_EVENT_NUMBER_NOT_USED".
 *         : 19.02.2019 0.61     Added R_SYS_SystemClockDividerSet() function.
 *         : 23.04.2019 0.70     Deleted the SYSTEM_CFG_EVENT_NUMBER_AGT0_AGTCMAI
 *                               from group0 (IRQ_EVENT_LINK_NUMBER_CONFIG_PART_1).
 *                               Added the SYSTEM_CFG_EVENT_NUMBER_AGT0_AGTCMAI
 *                               to group2,6 (IRQ_EVENT_LINK_NUMBER_CONFIG_PART_3).
 *                               Deleted the SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTCMBI
 *                               from group2,6 (IRQ_EVENT_LINK_NUMBER_CONFIG_PART_3).
 *                               Changed the SYSTEM_CFG_EVENT_NUMBER_IOPORT_GROUP1
 *                               to SYSTEM_CFG_EVENT_NUMBER_IOPORT_GROUP3.
 *                               Changed the SYSTEM_CFG_EVENT_NUMBER_MTDV_PM14INT
 *                               to SYSTEM_CFG_EVENT_NUMBER_MTDV_PM1INT.
 *                               Added the SYSTEM_CFG_EVENT_NUMBER_LVD_LVDBAT
 *                               to group1,5 (IRQ_EVENT_LINK_NUMBER_CONFIG_PART_2).
 *                               Added the SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ8
 *                               to group0 (IRQ_EVENT_LINK_NUMBER_CONFIG_PART_1).
 *                               Added the SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ9
 *                               to group2 (IRQ_EVENT_LINK_NUMBER_CONFIG_PART_3).
 *         : 23.07.2019 1.00     Added r_system_wdt_refresh() function.
 *         : 24.11.2020 1.30     Deleted the Definitions.
 *                                - SYSTEM_CFG_STACK_MAIN_BYTES
 *                                - SYSTEM_CFG_STACK_PROCESS_BYTES
 *                                - SYSTEM_CFG_HEAP_BYTES
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 @addtogroup grp_device_hal_system
 @{
 **********************************************************************************************************************/
#ifndef R_SYSTEM_CFG_H
#define R_SYSTEM_CFG_H

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief Parameter check enable@n
 *        0 = Disable@n
 *        1 = Enable (default)@n
 **********************************************************************************************************************/
#define SYSTEM_CFG_PARAM_CHECKING_ENABLE    (1)

/*******************************************************************************************************************//**
 * @brief Entering the critical section by masking interrupt@n
 *        0 = Disable@n
 *        1 = Enable (default)@n
 *        If this setting is enabled, it masks the interrupt by the R_SYSTEM API and enters the critical section.
 *        If not, the user need to use R_SYS_EnterCriticalSection() or R_SYS_ExitCriticalSection() to 
 *        enter the critical section in the user application.
 **********************************************************************************************************************/
#define SYSTEM_CFG_ENTER_CRITICAL_SECTION_ENABLE    (1)

/*******************************************************************************************************************//**
 * @brief Register protection enable setting@n
 *        0 = Disable@n
 *        1 = Enable (default)@n
 *        If this setting is enabled, register protection is enable and disable by the R_SYSTEM API.
 *        If not, the user need to use R_SYS_RegisterProtectEnable() or R_SYS_RegisterProtectDisable() to 
 *        perform protection settings in the user application.
 **********************************************************************************************************************/
#define SYSTEM_CFG_REGISTER_PROTECTION_ENABLE       (1)

/*******************************************************************************************************************//**
 * @brief Time-out value of API until register value is changed.@n
 **********************************************************************************************************************/
#define SYSTEM_CFG_API_TIMEOUT_COUNT                (0x10000000)

/*******************************************************************************************************************//**
 * @brief WDT Refresh enable@n
 *        0 = Disable (default)@n
 *        1 = Enable@n
 **********************************************************************************************************************/
#define SYSTEM_CFG_WDT_REFRESH_ENABLE               (0)

/*******************************************************************************************************************//**
 * @name IRQ_EVENT_LINK_NUMBER_CONFIG_PART_1
 *       Definition of IRQ event link number configuration part 1
 *       Please select one of the IRQ event numbers SYSTEM_IRQ_EVENT_NUMBER0/8/16/24 or 
 *       SYSTEM_IRQ_EVENT_NUMBER4/12/20/28 for the following interrupt events.@n
 *       Do not duplicate the IRQ event link number.
 *       And, the Error Handlers had better be set smaller event link number
 *       than those peripheral Transmit and Receive Handlers.
 **********************************************************************************************************************/
/* @{ */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ0           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_DMAC0_INT           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_ICU_SNZCANCEL       (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_LVD_LVD1            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_WDT_NMIUNDF         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_ADC140_WCMPM        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC0_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_CCC_PRD             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT0_CCMPA          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT0_OVF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT2_CCMPA          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT_UVWEDGE         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI0_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI0_AM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI0_SPRI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/4/8/12/16/20/24/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SOL_DH              (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_USBFS_D0FIFO        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC1_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_KEY_INTKR           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT1_CCMPA          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT3_OVF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT4_CCMPA          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI1_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI3_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI5_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ8           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 0/8/16/24 only */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ4           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC1_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_TMR_OVF0            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_MTDV_PM25INT        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT1_OVF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT3_CCMPA          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT5_CCMPA          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI2_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI4_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI9_RXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI1_SPRI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
#define SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 4/12/20/28 only */
/* @} */

/*******************************************************************************************************************//**
 * @name IRQ_EVENT_LINK_NUMBER_CONFIG_PART_2
 *       Definition of IRQ event link number configuration part 2
 *       Please select one of the IRQ event numbers SYSTEM_IRQ_EVENT_NUMBER1/9/17/25 or 
 *       SYSTEM_IRQ_EVENT_NUMBER5/13/21/29 for the following interrupt events.@n
 *       Do not duplicate the IRQ event link number.
 *       And, the Error Handlers had better be set smaller event link number
 *       than those peripheral Transmit and Receive Handlers.
 **********************************************************************************************************************/
/* @{ */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ1           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_DMAC1_INT           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_LVD_LVDBAT          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTCMAI        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_RTC_ALM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_ADC140_GBADI        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_ADC140_WCMPUM       (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC0_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_DOC_DOPCI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_CAC_FEERI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_CCC_CUP             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT0_CCMPB          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT0_UDF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT2_CCMPB          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI0_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI0_SPTI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/5/9/13/17/21/25/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SOL_DL              (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_AGT0_AGTCMBI        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_USBFS_D1FIFO        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC1_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_IOPORT_GROUP3       (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT1_CCMPB          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT3_UDF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT4_CCMPB          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI1_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI2_AM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI3_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI5_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 1/9/17/25 only */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ5           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC1_EEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_TMR_CMIA1           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_MTDV_PM36INT        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT1_UDF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT3_CCMPB          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT5_CCMPB          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI1_AM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI2_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI4_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI9_TXI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI1_SPTI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
#define SYSTEM_CFG_EVENT_NUMBER_GDT_FDCENDI         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 5/13/21/29 only */
/* @} */

/*******************************************************************************************************************//**
 * @name IRQ_EVENT_LINK_NUMBER_CONFIG_PART_3
 *       Definition of IRQ event link number configuration part 3
 *       Please select one of the IRQ event numbers SYSTEM_IRQ_EVENT_NUMBER2/10/18/26 or 
 *       SYSTEM_IRQ_EVENT_NUMBER6/14/22/30 for the following interrupt events.@n
 *       Do not duplicate the IRQ event link number.
 *       And, the Error Handlers had better be set smaller event link number
 *       than those peripheral Transmit and Receive Handlers.
 **********************************************************************************************************************/
/* @{ */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ2           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_DMAC2_INT           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_AGT0_AGTCMAI        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_RTC_PRD             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_ADC140_CMPAI        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_ADC140_GCADI        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC0_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_CAC_MENDI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_ELC_SWEVT0          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_POEG_GROUPA         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT0_CMPC           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT2_CMPC           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT2_OVF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI0_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI0_SPII           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI0_SPTEND         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/6/10/14/18/22/26/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_USBFS_USBI          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_IOPORT_GROUP2       (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_TMR_CMIA0           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT1_CMPC           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT4_CMPC           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT5_OVF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI1_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI3_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI4_AM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI5_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI1_SPII           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ9           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 2/10/18/26 only */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ6           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_MOSC_STOP           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_TMR_CMIB1           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_CCC_ERR             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT3_CMPC           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT4_OVF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT5_CMPC           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI2_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI3_AM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI4_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI9_TEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI1_SPTEND         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 6/14/22/30 only */
/* @} */

/*******************************************************************************************************************//**
 * @name IRQ_EVENT_LINK_NUMBER_CONFIG_PART_4
 *       Definition of IRQ event link number configuration part 4
 *       Please select one of the IRQ event numbers SYSTEM_IRQ_EVENT_NUMBER3/11/19/27 or 
 *       SYSTEM_IRQ_EVENT_NUMBER7/15/23/31 for the following interrupt events.@n
 *       Do not duplicate the IRQ event link number.
 *       And, the Error Handlers had better be set smaller event link number
 *       than those peripheral Transmit and Receive Handlers.
 **********************************************************************************************************************/
/* @{ */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_DMAC3_INT           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SYSTEM_SNZREQ       (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_IWDT_NMIUNDF        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_RTC_CUP             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_ADC140_CMPBI        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_ACMP_CMPI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_IIC0_EEI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_CAC_OVFI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_ELC_SWEVT1          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_POEG_GROUPB         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_MTDV_PM1INT         (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT0_CMPD           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT2_CMPD           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT2_UDF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI0_ERI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI0_SPEI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_QSPI_INTR           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/7/11/15/19/23/27/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_AGT0_AGTI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_USBFS_USBR          (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_TMR_CMIB0           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_ELC_INT0            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT1_CMPD           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT4_CMPD           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT5_UDF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI1_ERI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI3_ERI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI5_ERI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI9_AM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_GDT_DATII           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 3/11/19/27 only */
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ7           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_TMR_OVF1            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_ELC_INT1            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT3_CMPD           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT4_UDF            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_GPT5_CMPD           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI2_ERI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI4_ERI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI5_AM             (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SCI9_ERI            (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_SPI1_SPEI           (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
#define SYSTEM_CFG_EVENT_NUMBER_DIV_CALCCOMP        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED)  /*!< Numbers 7/15/23/31 only */
/* @} */

/*******************************************************************************************************************//**
 * @name R_SYSTEM_API_LOCATION_CONFIG
 *       Definition of R_System API location configuration.
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".
 **********************************************************************************************************************/
/* @{ */
#define SYSTEM_CFG_SECTION_R_SYS_INITIALIZE                 (SYSTEM_SECTION_CODE)       /*!< R_SYS_Initialize() section */
#define SYSTEM_CFG_SECTION_R_SYS_BOOSTSPEEDMODESET          (SYSTEM_SECTION_CODE)       /*!< R_SYS_BoostSpeedModeSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDMODESET           (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_HighSpeedModeSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDMODESET            (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_LowSpeedModeSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_32KHZSPEEDMODESET          (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_32kHzSpeedModeSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SPEEDMODEGET               (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SpeedModeGet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKHOCOSET         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SystemClockHOCOSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOCOSET         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SystemClockMOCOSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKLOCOSET         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SystemClockLOCOSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKMOSCSET         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SystemClockMOSCSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKSOSCSET         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SystemClockSOSCSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKPLLSET          (SYSTEM_SECTION_CODE)       /*!< R_SYS_SystemClockPLLSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKFREQGET         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SystemClockFreqGet() section */
#define SYSTEM_CFG_SECTION_R_SYS_PERIPHERALCLOCKFREQGET     (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_PeripheralClockFreqGet() section */
#define SYSTEM_CFG_SECTION_R_SYS_SYSTEMCLOCKDIVIDERSET      (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SystemClockDividerSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTART     (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_MainOscSpeedClockStart() section */
#define SYSTEM_CFG_SECTION_R_SYS_MAINOSCSPEEDCLOCKSTOP      (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_MainOscSpeedClockStop() section */
#define SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTART        (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_HighSpeedClockStart() section */
#define SYSTEM_CFG_SECTION_R_SYS_HIGHSPEEDCLOCKSTOP         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_HighSpeedClockStop() section */
#define SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTART      (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_MediumSpeedClockStart() section */
#define SYSTEM_CFG_SECTION_R_SYS_MEDIUMSPEEDCLOCKSTOP       (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_MediumSpeedClockStop() section */
#define SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTART         (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_LowSpeedClockStart() section */
#define SYSTEM_CFG_SECTION_R_SYS_LOWSPEEDCLOCKSTOP          (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_LowSpeedClockStop() section */
#define SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTART      (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SubOscSpeedClockStart() section */
#define SYSTEM_CFG_SECTION_R_SYS_SUBOSCSPEEDCLOCKSTOP       (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SubOscSpeedClockStop() section */
#define SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTART         (SYSTEM_SECTION_CODE)       /*!< R_SYS_PLLSpeedClockStart() section */
#define SYSTEM_CFG_SECTION_R_SYS_PLLSPEEDCLOCKSTOP          (SYSTEM_SECTION_CODE)       /*!< R_SYS_PLLSpeedClockStop() section */
#define SYSTEM_CFG_SECTION_R_SYS_OSCSTABILIZATIONFLAGGET    (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_OscStabilizationFlagGet() section */
#define SYSTEM_CFG_SECTION_R_SYS_IRQEVENTLINKSET            (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_IrqEventLinkSet() section */
#define SYSTEM_CFG_SECTION_R_SYS_IRQSTATUSGET               (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_IrqStatusGet() section */
#define SYSTEM_CFG_SECTION_R_SYS_IRQSTATUSCLEAR             (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_IrqStatusClear() section */
#define SYSTEM_CFG_SECTION_R_SYS_ENTERCRITICALSECTION       (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_EnterCriticalSection() section */
#define SYSTEM_CFG_SECTION_R_SYS_EXITCRITICALSECTION        (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_ExitCriticalSection() section */
#define SYSTEM_CFG_SECTION_R_SYS_RESOURCELOCK               (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_ResourceLock() section */
#define SYSTEM_CFG_SECTION_R_SYS_RESOURCEUNLOCK             (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_ResourceUnlock() section */
#define SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTENABLE      (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_RegisterProtectEnable() section */
#define SYSTEM_CFG_SECTION_R_SYS_REGISTERPROTECTDISABLE     (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_RegisterProtectDisable() section */
#define SYSTEM_CFG_SECTION_R_SYS_SOFTWAREDELAY              (SYSTEM_SECTION_RAM_FUNC)   /*!< R_SYS_SoftwareDelay() section */
#define SYSTEM_CFG_SECTION_IEL0_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL0_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL1_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL1_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL2_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL2_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL3_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL3_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL4_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL4_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL5_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL5_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL6_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL6_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL7_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL7_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL8_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL8_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL9_IRQHANDLER                  (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL9_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL10_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL10_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL11_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL11_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL12_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL12_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL13_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL13_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL14_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL14_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL15_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL15_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL16_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL16_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL17_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL17_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL18_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL18_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL19_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL19_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL20_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL20_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL21_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL21_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL22_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL22_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL23_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL23_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL24_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL24_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL25_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL25_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL26_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL26_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL27_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL27_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL28_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL28_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL29_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL29_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL30_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL30_IRQHandler() section */
#define SYSTEM_CFG_SECTION_IEL31_IRQHANDLER                 (SYSTEM_SECTION_RAM_FUNC)   /*!< IEL31_IRQHandler() section */
#define SYSTEM_CFG_SECTION_R_SYS_GETVERSION                 (SYSTEM_SECTION_CODE)       /*!< R_SYS_GetVersion() section */
#define SYSTEM_CFG_SECTION_R_SYS_WDT_REFRESH                (SYSTEM_SECTION_RAM_FUNC)   /*!< r_system_wdt_refresh() section */
/* @} */

/*******************************************************************************************************************//**
 * @} (end ingroup grp_device_hal_system)
 **********************************************************************************************************************/

#endif /* R_SYSTEM_CFG_H */
/* End of file (r_system_cfg.h) */
