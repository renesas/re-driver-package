/* Generated configuration header file - do not edit */
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
 * Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_basic_config_reference.h
 * Description  : USB User definition
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

#ifndef R_USB_BASIC_CFG_H
#define R_USB_BASIC_CFG_H

/******************************************************************************/
/** Common Settings in USB Host/USB Peripheral Mode                           */
/******************************************************************************/

/** [Device class setting]
 *  #define USB_CFG_HCDC_USE : Host Communication Device Class
 *  #define USB_CFG_HHID_USE : Host Human Interface Device Class
 *  #define USB_CFG_HMSC_USE : Host Mass Storage Class
 *  #define USB_CFG_HVND_USE : Host VENDER Class
 *  #define USB_CFG_PCDC_USE : Peripheral Communication Device Class
 *  #define USB_CFG_PHID_USE : Peripheral Human Interface Device Class
 *  #define USB_CFG_PMSC_USE : Peripheral Mass Storage Class
 *  #define USB_CFG_PVND_USE : Peripheral VENDER Class
 * */
#define USB_CFG_PVND_USE /* USB_CFG_DEVICE_CLASS */

/**  [DTC use setting]
 * USB_CFG_ENABLE       : Uses DTC
 * USB_CFG_DISABLE      : Does not use DTC
 */
#define    USB_CFG_DTC              (USB_CFG_DISABLE)

/**  [DMA use setting]
 * USB_CFG_ENABLE       : Uses DMA
 * USB_CFG_DISABLE      : Does not use DMA
 */
#define    USB_CFG_DMA              (USB_CFG_DISABLE)

/** [DMA Channel setting(USB0 module Send transfer)]
 * USB_CFG_CH0          : Uses DMAC0
 * USB_CFG_CH1          : Uses DMAC1
 * USB_CFG_CH2          : Uses DMAC2
 * USB_CFG_CH3          : Uses DMAC3
 */
#define     USB_CFG_USB0_DMA_TX     (USB_CFG_CH0)

/** [DMA Channel setting(USB0 module Receive transfer)]
 * USB_CFG_CH0          : Uses DMAC0
 * USB_CFG_CH1          : Uses DMAC1
 * USB_CFG_CH2          : Uses DMAC2
 * USB_CFG_CH3          : Uses DMAC3
 */
#define     USB_CFG_USB0_DMA_RX     (USB_CFG_CH1)

/**
 * SET USB INTERRUPT PRIORITY; 
 *  1                   : lowest
 *  15                  : highest
 */
#define USB_CFG_INTERRUPT_PRIORITY  (3)


/******************************************************************************/
/** OTHER DEFINITIONS SETTING                                                 */
/******************************************************************************/
/** [DBLB bit setting]
 * USB_CFG_DBLBON       : DBLB bit set.
 * USB_CFG_DBLBOFF      : DBLB bit cleared.
 */
#define USB_CFG_DBLB                (USB_CFG_DBLBON)

/******************************************************************************************************************//**
 * @name USB_DRIVER_LOCATION_CONFIG
 *       Definition of USB Driver location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define USB_CFG_FUNC        (SYSTEM_SECTION_CODE) /*!<  USB Driver section */

/* @} */

#endif  /* R_USB_BASIC_CFG_H */
/******************************************************************************
 End  Of File
 ******************************************************************************/

