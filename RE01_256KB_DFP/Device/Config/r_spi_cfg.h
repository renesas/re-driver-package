/**********************************************************************************************************************
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
/**********************************************************************************************************************
* File Name    : r_spi_cfg.h
* Version      : 1.00
* Description  : CMSIS-Driver for SPI.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
**********************************************************************************************************************/

#ifndef R_SPI_CFG_H
#define R_SPI_CFG_H
/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_spi
 * @{
 *********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Macro definitions
 *****************************************************************************/

/******************************************************************************************************************//**
 * @name SPIn_TRANSMIT_CONTROL/SPIn_RECEIVE_CONTROL
 *       Select whether to use interrupts or DMAC (DTC) for transmission and reception control.@n@n
 *       SPI_USED_INTERRUPT: Interrupt is used for transmission / reception control.@n
 *       SPI_USED_DMACn    : DMACn is used for transmission / reception control.@n
 *       SPI_USED_DTC      : DTC is used for  transmission / reception control.@n
 *          [Example]@n
 *          Example of definition when interrupt is used for transmission/reception of SPI0 
 *          and DMAC is used for transmission/reception of SPI1
 *          @code
 *          ...
 *          #define SPI0_TRANSMIT_CONTROL    SPI_USED_INTERRUPT
 *          #define SPI0_RECEIVE_CONTROL     SPI_USED_INTERRUPT
 *          #define SPI1_TRANSMIT_CONTROL    SPI_USED_DMAC0
 *          #define SPI1_RECEIVE_CONTROL     SPI_USED_DMAC1
 *
 *          ...
 *          @endcode
 *          [Note]@n
 *          Do not use the same DMAC channel for multiple SPI transmission or reception control.@n
 *          When performing Receive operation with DMAC / DTC, if the data length is not 32 bits, bits not 
 *          referred to in SPTX are stored in the associated bits in SPRX.
 *********************************************************************************************************************/
/* @{ */
#define SPI_USED_INTERRUPT  (0)                        ///< Use Interrupt for transmission and reception control of SPI
#define SPI_USED_DMAC0      (1<<0)                     ///< Use DMAC0 for transmission and reception control of SPI
#define SPI_USED_DMAC1      (1<<1)                     ///< Use DMAC1 for transmission and reception control of SPI
#define SPI_USED_DMAC2      (1<<2)                     ///< Use DMAC2 for transmission and reception control of SPI
#define SPI_USED_DMAC3      (1<<3)                     ///< Use DMAC3 for transmission and reception control of SPI
#define SPI_USED_DTC        (1<<15)                    ///< Use DTC for transmission and reception control of SPI

#define SPI0_TRANSMIT_CONTROL   SPI_USED_INTERRUPT     ///< SPI0 transmit control
#define SPI0_RECEIVE_CONTROL    SPI_USED_INTERRUPT     ///< SPI0 receive control
#define SPI1_TRANSMIT_CONTROL   SPI_USED_INTERRUPT     ///< SPI1 transmit control
#define SPI1_RECEIVE_CONTROL    SPI_USED_INTERRUPT     ///< SPI1 receive control
 
/* When using the ARM_SPI_CONTROL_SS command, cancel the following comment and set the terminal to use */
//#define SPI0_SS_PORT PORT0->PODR                     ///< Used SS port
#define SPI0_SS_PIN 0                                  ///< Used SS pin number
#define SPI0_SPTI_PRIORITY   (3)                       ///< SPTI0 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI0_SPRI_PRIORITY   (3)                       ///< SPRI0 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI0_SPII_PRIORITY   (3)                       ///< SPII0 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI0_SPEI_PRIORITY   (3)                       ///< SPEI0 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI0_SPTEND_PRIORITY (3)                       ///< SPTEND0 priority value(set to 0 to 3, 0 is highest priority.)

//#define SPI1_SS_PORT PORT0->PODR                     ///< Used SS port
#define SPI1_SS_PIN 1                                  ///< Used SS pin number
#define SPI1_SPTI_PRIORITY   (3)                       ///< SPTI1 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI1_SPRI_PRIORITY   (3)                       ///< SPRI1 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI1_SPII_PRIORITY   (3)                       ///< SPII1 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI1_SPEI_PRIORITY   (3)                       ///< SPEI1 priority value(set to 0 to 3, 0 is highest priority.)
#define SPI1_SPTEND_PRIORITY (3)                       ///< SPTEND1 priority value(set to 0 to 3, 0 is highest priority.)

/******************************************************************************************************************//**
 * @name R_SPI_API_LOCATION_CONFIG
 *       Definition of R_SPI API location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define SPI_CFG_ARM_SPI_GET_VERSION      (SYSTEM_SECTION_CODE)    //< ARM_SPI_GetVersion() section
#define SPI_CFG_ARM_SPI_GET_CAPABILITIES (SYSTEM_SECTION_CODE)    //< ARM_SPI_GetCapabilities() section
#define SPI_CFG_ARM_SPI_INITIALIZE       (SYSTEM_SECTION_CODE)    //< ARM_SPI_Initialize() section
#define SPI_CFG_ARM_SPI_UNINITIALIZE     (SYSTEM_SECTION_CODE)    //< ARM_SPI_Uninitialize() section
#define SPI_CFG_ARM_SPI_POWERCONTROL     (SYSTEM_SECTION_CODE)    //< ARM_SPI_PowerControl() section
#define SPI_CFG_ARM_SPI_SEND             (SYSTEM_SECTION_CODE)    //< ARM_SPI_Send() section
#define SPI_CFG_ARM_SPI_RECEIVE          (SYSTEM_SECTION_CODE)    //< ARM_SPI_Receive() section
#define SPI_CFG_ARM_SPI_TRANSFER         (SYSTEM_SECTION_CODE)    //< ARM_SPI_Transfer() section
#define SPI_CFG_ARM_SPI_GET_DATACOUNT    (SYSTEM_SECTION_CODE)    //< ARM_SPI_GetDataCount() section
#define SPI_CFG_ARM_SPI_CONTROL          (SYSTEM_SECTION_CODE)    //< ARM_SPI_Control() section
#define SPI_CFG_ARM_SPI_GET_STATUS       (SYSTEM_SECTION_CODE)    //< ARM_SPI_GetStatus() section

#define SPI_CFG_R_SPRI_HANDLER           (SYSTEM_SECTION_RAM_FUNC) //< r_spri_handler() section
#define SPI_CFG_R_SPTI_HANDLER           (SYSTEM_SECTION_RAM_FUNC) //< r_spti_handler() section
#define SPI_CFG_R_SPII_HANDLER           (SYSTEM_SECTION_RAM_FUNC) //< r_spii_handler() section
#define SPI_CFG_R_SPEI_HANDLER           (SYSTEM_SECTION_RAM_FUNC) //< r_spei_handler() section
#define SPI_CFG_R_SPTEND_HANDLER         (SYSTEM_SECTION_RAM_FUNC) //< r_sptend_handler() section
/* @} */

#define SPI_PRV_USED_TX_DMAC_DTC_DRV (SPI0_TRANSMIT_CONTROL | SPI1_TRANSMIT_CONTROL)
#define SPI_PRV_USED_TX_DMAC_DRV     (SPI_PRV_USED_TX_DMAC_DTC_DRV & 0x00FF)
#define SPI_PRV_USED_TX_DTC_DRV      (SPI_PRV_USED_TX_DMAC_DTC_DRV & SPI_USED_DTC)

#define SPI_PRV_USED_RX_DMAC_DTC_DRV (SPI0_RECEIVE_CONTROL | SPI1_RECEIVE_CONTROL)
#define SPI_PRV_USED_RX_DMAC_DRV     (SPI_PRV_USED_RX_DMAC_DTC_DRV & 0x00FF)
#define SPI_PRV_USED_RX_DTC_DRV      (SPI_PRV_USED_RX_DMAC_DTC_DRV & SPI_USED_DTC)

#define SPI_PRV_USED_DMAC_DTC_DRV          (SPI_PRV_USED_TX_DMAC_DTC_DRV | SPI_PRV_USED_RX_DMAC_DTC_DRV)
#define SPI_PRV_USED_DMAC_DRV              (SPI_PRV_USED_TX_DMAC_DRV | SPI_PRV_USED_RX_DMAC_DRV)
#define SPI_PRV_USED_DTC_DRV               (SPI_PRV_USED_TX_DTC_DRV | SPI_PRV_USED_RX_DTC_DRV)

#ifdef  __cplusplus
}
#endif
/*******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_spi)
 **********************************************************************************************************************/

#endif /** R_SPI_CFG_H */

/* End of file (r_spi_cfg.h) */

