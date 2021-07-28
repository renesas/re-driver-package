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
* Copyright (C) 2018-2021 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_usart_cmsis_api.c
* Version      : 1.20
* Description  : CMSIS-Driver for USART
**********************************************************************************************************************/
/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 31.01.2020 0.80     Fixed transmission permission processing
*                               Fixed a bug that interrupt does not occur when SCI9 is set to DTC control.
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
*         : 05.01.2021 1.20     Fixed DTC Close processing when using DTC.
*                               If there are other DTC factors that can be used, close will be skipped.
**********************************************************************************************************************/

/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_usart
 * @{
 *********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <r_usart_cmsis_api.h>

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define ARM_USART_PRV_DRV_VERSION    (ARM_DRIVER_VERSION_MAJOR_MINOR(1, 20))  /*!< driver version */

/*  BAUD DIVISOR INFO                      
    reg_value: b1-b0: cks[1:0]             
                  b2: bgmd                 
                  b3: abcs                 
                  b4: abcse                
               b6-b5: bcp[1:0]             
                  b7: bcp2                 */
#define REG_PRV_VALUE_CKS0      (0)             /*!<  reg_value CKS0 position */
#define REG_PRV_VALUE_CKS1      (1)             /*!<  reg_value CKS1 position */
#define REG_PRV_VALUE_BGMD      (2)             /*!<  reg_value BGMD position */
#define REG_PRV_VALUE_ABCS      (3)             /*!<  reg_value ABCS position */
#define REG_PRV_VALUE_ABCSE     (4)             /*!<  reg_value ABCSE position */
#define REG_PRV_VALUE_BCP0      (5)             /*!<  reg_value BCP0 position */
#define REG_PRV_VALUE_BCP1      (6)             /*!<  reg_value BCP1 position */
#define REG_PRV_VALUE_BCP2      (7)             /*!<  reg_value BCP2 position */

/******************************************************************************
 Local Typedef definitions
 *****************************************************************************/

/******************************************************************************
 Exported global variables (to be accessed by other files)
 *****************************************************************************/

/******************************************************************************
 Private global variables and functions
 *****************************************************************************/
#if (USART_CFG_SECTION_ARM_USART_GET_VERSION     == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_VERSION  __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_VERSION
#endif

#if (USART_CFG_SECTION_ARM_USART_GET_CAPABILITES == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_CAPABILITIES __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_CAPABILITIES
#endif

#if (USART_CFG_SECTION_ARM_USART_CONTROL         == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_ST_BAUD_DIVISOR     __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_ST_BAUD_DIVISOR
#endif

#if ((USART_CFG_SECTION_ARM_USART_INITIALIZE        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_UNINITIALIZE      == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_POWER_CONTROL     == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_SEND              == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_RECEIVE           == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_TRANSFER          == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_TX_COUNT      == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_RX_COUNT      == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_CONTROL           == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_STATUS        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_SET_MODEM_CONTROL == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_MODEM_STATUS  == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_TXI_HANDLER                 == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_RXI_HANDLER                 == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ERI_HANDLER                 == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_SCI_RESOURCES          __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_SCI_RESOURCES
#endif

#if ((USART_CFG_SECTION_ARM_USART_GET_VERSION       == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_CAPABILITES   == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_INITIALIZE        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_UNINITIALIZE      == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_POWER_CONTROL     == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_SEND              == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_RECEIVE           == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_TRANSFER          == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_TX_COUNT      == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_RX_COUNT      == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_CONTROL           == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_STATUS        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_SET_MODEM_CONTROL == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_GET_MODEM_STATUS  == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_DRIVER_USART          __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_USART
#endif

#if (USART_CFG_SECTION_TXI_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_TXI_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_TXI_HANDLER
#endif

#if (USART_CFG_SECTION_RXI_HANDLER               == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_RXI_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_RXI_HANDLER
#endif

#if (USART_CFG_SECTION_ERI_HANDLER               == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ERI_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ERI_HANDLER
#endif

#if (USART_CFG_SECTION_ARM_USART_CONTROL         == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_MODE_SET      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_MODE_SET
#endif

#if (USART_CFG_SECTION_ARM_USART_CONTROL         == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SCI_BITRATE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SCI_BITRATE
#endif

#if (USART_CFG_SECTION_ARM_USART_CONTROL         == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SCI_SET_REG_CLEAR      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SCI_SET_REG_CLEAR
#endif

#if (USART_CFG_SECTION_ARM_USART_CONTROL         == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_ENABLE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_ENABLE
#endif

#if ((USART_CFG_SECTION_ARM_USART_CONTROL        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_POWER_CONTROL  == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_DISABLE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_DISABLE
#endif

#if (USART_CFG_SECTION_ARM_USART_INITIALIZE      == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_CHECK_TX_AVAILABLE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#define STATIC_FUNC_LOCATION_PRV_CHECK_RX_AVAILABLE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_CHECK_TX_AVAILABLE
#define STATIC_FUNC_LOCATION_PRV_CHECK_RX_AVAILABLE
#endif

#if ((USART_CFG_SECTION_ARM_USART_SEND           == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_RECEIVE        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_TRANSFER       == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_PRV_DMA_CONFIG_INIT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_DMA_CONFIG_INIT
#endif


#if ((USART_CFG_SECTION_ARM_USART_CONTROL        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_POWER_CONTROL  == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ERI_HANDLER              == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_PRV_SCI_TRANSMIT_STOP      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SCI_TRANSMIT_STOP
#endif


#if ((USART_CFG_SECTION_ARM_USART_CONTROL        == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ARM_USART_POWER_CONTROL  == SYSTEM_SECTION_RAM_FUNC) || \
     (USART_CFG_SECTION_ERI_HANDLER              == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_PRV_SCI_RECEIVE_STOP      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SCI_RECEIVE_STOP
#endif


/** Version of CMSIS-Driver USART module */
/* API version is the version of the CMSIS-Driver specification used to implement this driver. */
/* Driver version is source code version of the actual driver implementation. */
static const ARM_DRIVER_VERSION gs_driver_version DATA_LOCATION_PRV_DRIVER_VERSION = { // @suppress("Source line ordering")
    ARM_USART_API_VERSION,               /*!< CMSIS-Driver Version */
    ARM_USART_PRV_DRV_VERSION            /*!< Driver-SourceCode Version */
};

/** Driver Capabilities */
static const ARM_USART_CAPABILITIES gs_driver_capabilities DATA_LOCATION_PRV_DRIVER_CAPABILITIES = { // @suppress("Source line ordering")
    1, /*!< supports UART (Asynchronous) mode */
    1, /*!< supports Synchronous Master mode */
    1, /*!< supports Synchronous Slave mode */
    0, /*!< supports UART Single-wire mode */
    0, /*!< supports UART IrDA mode */
    1, /*!< supports UART Smart Card mode */
    1, /*!< Smart Card Clock generator available */
    1, /*!< RTS Flow Control available */
    1, /*!< CTS Flow Control available */
    0, /*!< Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    0, /*!< Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    1, /*!< RTS Line: 0=not available, 1=available */
    1, /*!< CTS Line: 0=not available, 1=available */
    0, /*!< DTR Line: 0=not available, 1=available */
    0, /*!< DSR Line: 0=not available, 1=available */
    0, /*!< DCD Line: 0=not available, 1=available */
    0, /*!< RI Line: 0=not available, 1=available */
    0, /*!< Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /*!< Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /*!< Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0  /*!< Signal RI change event: \ref ARM_USART_EVENT_RI */
};

/** Asynchronous Division table */
#define NUM_PRV_DIVISORS_ASYNC  (sizeof(gs_async_baud) / sizeof(gs_async_baud[0]))

static const st_baud_divisor_t gs_async_baud[] DATA_LOCATION_PRV_ST_BAUD_DIVISOR =
{
    {    6,  (0 << REG_PRV_VALUE_CKS0) | (0 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (1 << REG_PRV_VALUE_ABCSE)},
    {    8,  (0 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (1 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    {   16,  (0 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    {   24,  (1 << REG_PRV_VALUE_CKS0) | (0 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (1 << REG_PRV_VALUE_ABCSE)},
    {   32,  (1 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (1 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    {   64,  (1 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    {   96,  (2 << REG_PRV_VALUE_CKS0) | (0 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (1 << REG_PRV_VALUE_ABCSE)},
    {  128,  (2 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (1 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    {  256,  (2 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    {  384,  (3 << REG_PRV_VALUE_CKS0) | (0 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (1 << REG_PRV_VALUE_ABCSE)},
    {  512,  (3 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (1 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    { 1024,  (3 << REG_PRV_VALUE_CKS0) | (1 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)},
    { 2048,  (3 << REG_PRV_VALUE_CKS0) | (0 << REG_PRV_VALUE_BGMD) | (0 << REG_PRV_VALUE_ABCS) | (0 << REG_PRV_VALUE_ABCSE)}
};

/** Synchronous Division table */
#define NUM_PRV_DIVISORS_SYNC  (sizeof(gs_sync_baud) / sizeof(const st_baud_divisor_t))
static const st_baud_divisor_t gs_sync_baud[] DATA_LOCATION_PRV_ST_BAUD_DIVISOR =
{
    {      4,      (0 << REG_PRV_VALUE_CKS0) },
    {     16,      (1 << REG_PRV_VALUE_CKS0) },
    {     64,      (2 << REG_PRV_VALUE_CKS0) },
    {    256,      (3 << REG_PRV_VALUE_CKS0) }
};

/** Smartcard Division table  */
#define NUM_PRV_DIVISORS_SMC  (sizeof(gs_smc_baud) / sizeof(const st_baud_divisor_t))
static const st_baud_divisor_t gs_smc_baud[] DATA_LOCATION_PRV_ST_BAUD_DIVISOR =
{
    {    64,    (1 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (0 << REG_PRV_VALUE_CKS0)},
    {   128,    (1 << REG_PRV_VALUE_BCP2) | (1 << REG_PRV_VALUE_BCP0) | (0 << REG_PRV_VALUE_CKS0)},
    {   186,    (0 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (0 << REG_PRV_VALUE_CKS0)},
    {   256,    (1 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (1 << REG_PRV_VALUE_CKS0)},
    {   372,    (0 << REG_PRV_VALUE_BCP2) | (2 << REG_PRV_VALUE_BCP0) | (0 << REG_PRV_VALUE_CKS0)},
    {   512,    (1 << REG_PRV_VALUE_BCP2) | (1 << REG_PRV_VALUE_BCP0) | (1 << REG_PRV_VALUE_CKS0)},
    {   744,    (0 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (1 << REG_PRV_VALUE_CKS0)},
    {  1024,    (1 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (2 << REG_PRV_VALUE_CKS0)},
    {  1488,    (0 << REG_PRV_VALUE_BCP2) | (2 << REG_PRV_VALUE_BCP0) | (1 << REG_PRV_VALUE_CKS0)},
    {  2048,    (1 << REG_PRV_VALUE_BCP2) | (1 << REG_PRV_VALUE_BCP0) | (2 << REG_PRV_VALUE_CKS0)},
    {  2976,    (0 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (2 << REG_PRV_VALUE_CKS0)},
    {  4096,    (1 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)},
    {  5952,    (0 << REG_PRV_VALUE_BCP2) | (2 << REG_PRV_VALUE_BCP0) | (2 << REG_PRV_VALUE_CKS0)},
    {  8192,    (1 << REG_PRV_VALUE_BCP2) | (1 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)},
    { 11904,    (0 << REG_PRV_VALUE_BCP2) | (0 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)},
    { 16384,    (0 << REG_PRV_VALUE_BCP2) | (1 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)},
    { 23808,    (0 << REG_PRV_VALUE_BCP2) | (2 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)},
    { 32768,    (1 << REG_PRV_VALUE_BCP2) | (3 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)},
    { 47616,    (1 << REG_PRV_VALUE_BCP2) | (2 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)},
    { 65536,    (0 << REG_PRV_VALUE_BCP2) | (3 << REG_PRV_VALUE_BCP0) | (3 << REG_PRV_VALUE_CKS0)}
};


static int32_t mode_set_asynchronous(uint32_t control, st_sci_reg_set_t * const p_sci_regs, uint32_t baud,
                                        st_usart_resources_t * const p_usart)
                                        STATIC_FUNC_LOCATION_PRV_MODE_SET;
static int32_t mode_set_synchronous(uint32_t control, st_sci_reg_set_t * const p_sci_regs,
                                        uint32_t baud, st_usart_resources_t * const p_usart)
                                        STATIC_FUNC_LOCATION_PRV_MODE_SET;
static int32_t mode_set_smartcard(uint32_t control, st_sci_reg_set_t * const p_sci_regs,
                                        uint32_t baud, st_usart_resources_t * const p_usart)
                                        STATIC_FUNC_LOCATION_PRV_MODE_SET;
static int32_t sci_bitrate(st_sci_reg_set_t * p_sci_regs, uint32_t baud, e_usart_base_clk_t base_clk, 
                                st_baud_divisor_t const * p_baud_info, uint8_t num_divisors, uint32_t mode)
                                STATIC_FUNC_LOCATION_PRV_SCI_BITRATE;
static void sci_set_regs_clear(st_sci_reg_set_t * const p_regs) STATIC_FUNC_LOCATION_PRV_SCI_SET_REG_CLEAR;
static void sci_tx_enable(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_ENABLE;
static void sci_tx_disable(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_ENABLE;
static void sci_rx_enable(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_ENABLE;
static void sci_rx_disable(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_ENABLE;
static void sci_tx_rx_enable(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_ENABLE;
static void sci_tx_rx_disable(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_TX_RX_DISABLE;
static int32_t check_tx_available(int16_t * const p_flag, st_usart_resources_t * const p_usart) 
                                                                 STATIC_FUNC_LOCATION_PRV_CHECK_TX_AVAILABLE;
static int32_t check_rx_available(int16_t * const p_flag, st_usart_resources_t * const p_usart) 
                                                                 STATIC_FUNC_LOCATION_PRV_CHECK_RX_AVAILABLE;
static void txi_handler(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;

#if (0 != USART_PRV_USED_TX_DTC_DRV)
static void txi_dtc_handler(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
static void txi_dmac_handler(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
#endif
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
static void rxi_dmac_handler(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
#endif
#if (0 != USART_PRV_USED_DMAC_DTC_DRV)
static void dma_config_init(st_dma_transfer_data_cfg_t *p_cfg) STATIC_FUNC_LOCATION_PRV_DMA_CONFIG_INIT;
#endif

static void rxi_handler(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void eri_handler(st_usart_resources_t * const p_usart) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;
static void sci_transmit_stop(st_usart_resources_t const * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_TRANSMIT_STOP;
static void sci_receive_stop(st_usart_resources_t const * const p_usart) STATIC_FUNC_LOCATION_PRV_SCI_RECEIVE_STOP;

/** USART Resources define start */


/** SCI0 */
#if (0 != R_SCI0_ENABLE)
static int32_t USART0_Initialize(ARM_USART_SignalEvent_t cb_event) FUNC_LOCATION_ARM_USART_INITIALIZE;
static int32_t USART0_Uninitialize(void) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
static int32_t USART0_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_USART_POWER_CONTROL;
static int32_t USART0_Send(void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_SEND;
static int32_t USART0_Receive(void * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_RECEIVE;
static int32_t USART0_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
                               FUNC_LOCATION_ARM_USART_TRANSFER;
static uint32_t USART0_GetTxCount(void) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
static uint32_t USART0_GetRxCount(void) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
static int32_t USART0_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_USART_CONTROL;
static ARM_USART_STATUS USART0_GetStatus(void) FUNC_LOCATION_ARM_USART_GET_STATUS;
static ARM_USART_MODEM_STATUS USART0_GetModemStatus(void) FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;
static int32_t USART0_SetModemControl(ARM_USART_MODEM_CONTROL control) FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
static void sci0_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
static void sci0_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void sci0_eri_interrupt(void) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;

static st_usart_info_t gs_sci0_info = {0};

#if (SCI_USED_DTC == SCI0_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  gs_sci0_tx_dtc_info;
#endif
#if (SCI_USED_DTC == SCI0_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  gs_sci0_rx_dtc_info;
#endif

/** SCI0 Resources */
static st_usart_resources_t gs_sci0_resources DATA_LOCATION_PRV_SCI_RESOURCES =
{
    (SCI2_Type*)SCI0, // @suppress("Cast comment")
    R_SCI_Pinset_CH0,
    R_SCI_Pinclr_CH0,
    &gs_sci0_info,
#ifdef USART0_CTS_PORT
    (uint16_t*)&USART0_CTS_PORT,
    USART0_CTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif

#ifdef USART0_RTS_PORT
    (uint16_t*)&USART0_RTS_PORT,
    USART0_RTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif
    USART_BASE_PCLKA,
    SYSTEM_LOCK_SCI0,
    LPM_MSTP_SCI0,
    SYSTEM_CFG_EVENT_NUMBER_SCI0_TXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI0_RXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI0_ERI, // @suppress("Cast comment")
    USART_TXI0_IESR_VAL,
    USART_RXI0_IESR_VAL,
    USART_ERI0_IESR_VAL,
    SCI0_TXI_PRIORITY,
    SCI0_RXI_PRIORITY,
    SCI0_ERI_PRIORITY,
#if (SCI_USED_DMAC0 == SCI0_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    USART_TXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI0_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    USART_TXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI0_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    USART_TXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI0_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    USART_TXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI0_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI0_TXI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SCI_USED_DMAC0 == SCI0_RECEIVE_CONTROL)
    &Driver_DMAC0,
    USART_RXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI0_RECEIVE_CONTROL)
    &Driver_DMAC1,
    USART_RXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI0_RECEIVE_CONTROL)
    &Driver_DMAC2,
    USART_RXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI0_RECEIVE_CONTROL)
    &Driver_DMAC3,
    USART_RXI0_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI0_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI0_RXI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#if (SCI_USED_DTC == SCI0_TRANSMIT_CONTROL)
    &gs_sci0_tx_dtc_info,
#else
    NULL,
#endif
#if (SCI_USED_DTC == SCI0_RECEIVE_CONTROL)
    &gs_sci0_rx_dtc_info,
#else
    NULL,
#endif
#endif
    sci0_txi_interrupt,
    sci0_rxi_interrupt,
    sci0_eri_interrupt
};


/****************************************************************************************************************//**
 * @brief USART initialize method for user applications to initialize the SCI channel0. User applications should call
 * this method through the Driver Instance Driver_USART0.Initialize().
 *
 * @param[in] cb_event   Callback event for USART.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_Initialize */
static int32_t USART0_Initialize(ARM_USART_SignalEvent_t cb_event) // @suppress("Source line ordering") @suppress("Non-API function naming")
{
    return (ARM_USART_Initialize(cb_event,  &gs_sci0_resources));
}/* End of function USART0_Initialize() */

/****************************************************************************************************************//**
 * @brief USART uninitialize method for user applications to initialize the SCI channel0. User applications should call
 * this method through the Driver Instance Driver_USART0.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_Uninitialize */
static int32_t USART0_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_Uninitialize(&gs_sci0_resources));
}/* End of function USART0_Uninitialize() */

/****************************************************************************************************************//**
 * @brief USART power control method for user applications to control the power of SCI channel0. User applications
 * should call this method through the Driver Instance Driver_USART0.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_PowerControl */
static int32_t USART0_PowerControl(ARM_POWER_STATE state) //@suppress("Non-API function naming")
{
    return (ARM_USART_PowerControl(state, &gs_sci0_resources));
}/* End of function USART0_PowerControl() */

/****************************************************************************************************************//**
 * @brief USART data transmission method for user applications to send data over SCI channel0. User applications should
 * call this method through the Driver Instance Driver_USART0.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_Send */
static int32_t USART0_Send(void const * const p_data, uint32_t num) //@suppress("Non-API function naming")
{
    return (ARM_USART_Send(p_data, num , &gs_sci0_resources));
}/* End of function USART0_Send() */

/****************************************************************************************************************//**
 * @brief USART data reception method for user applications to receive data over SCI channel0. User applications should
 * call this method through the Driver Instance Driver_USART0.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_Receive */
static int32_t USART0_Receive(void * const p_data, uint32_t num) //@suppress("Non-API function naming")
{
    return (ARM_USART_Receive(p_data, num, &gs_sci0_resources));
}/* End of function USART0_Receive() */

/****************************************************************************************************************//**
 * @brief USART data tranmission method for user applications to send and receive data over SCI channel0 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_USART0.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_Transfer */
static int32_t USART0_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num) //@suppress("Non-API function naming")
{
    return (ARM_USART_Transfer(p_data_out, p_data_in, num, &gs_sci0_resources));
}/* End of function USART0_Transfer() */

/****************************************************************************************************************//**
 * @brief USART data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SCI channel0. User applications should call this method through the Driver Instance
 * Driver_USART0.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_GetTxCount */
static uint32_t USART0_GetTxCount(void) //@suppress("Non-API function naming")
{
    return (ARM_USART_GetTxCount(&gs_sci0_resources));
}/* End of function USART0_GetTxCount() */

/***************************************************************************************************************//**
 * @brief USART data reception count getter method for user applications to get the transmission counter value for the
 * RX channel on SCI channel0. User applications should call this method through the Driver
 * Instance Driver_USART0.GetRxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_GetRxCount */
static uint32_t USART0_GetRxCount(void) //@suppress("Non-API function naming")
{
    return (ARM_USART_GetRxCount(&gs_sci0_resources));
}/* End of function USART0_GetRxCount() */

/****************************************************************************************************************//**
 * @brief USART driver control method for user applications to set USART configuration for SCI channel0. User 
 * applications should call this method through the Driver Instance Driver_USART0.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_Control */
static int32_t USART0_Control(uint32_t control, uint32_t arg) //@suppress("Non-API function naming")
{
    return (ARM_USART_Control(control, arg, &gs_sci0_resources));
}/* End of function USART0_Control() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the driver status for SCI channel0. User applications
 * should call this method through the Driver Instance Driver_USART0.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_GetStatus */
static ARM_USART_STATUS USART0_GetStatus(void) //@suppress("Non-API function naming")
{
    return (ARM_USART_GetStatus(&gs_sci0_resources));
}/* End of function USART0_GetStatus() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the modem status for SCI channel0. User applications
 * should call this method through the Driver Instance Driver_USART0.GetModemStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_GetModemStatus */
static ARM_USART_MODEM_STATUS USART0_GetModemStatus(void) //@suppress("Non-API function naming")
{
    return (ARM_USART_GetModemStatus(&gs_sci0_resources));
}/* End of function USART0_GetModemStatus() */

/****************************************************************************************************************//**
 * @brief USART status setter method for user applications to set a modem configuration for SCI channel0. User 
 * applications should call this method through the Driver Instance Driver_USART0.SetModemControl().
 * @param[in]  control  ARM_USART_RTS_CLEAR, ARM_USART_RTS_SET, ARM_USART_DTR_SET or ARM_USART_DTR_CLEAR
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART0_SetModemControl */
static int32_t USART0_SetModemControl(ARM_USART_MODEM_CONTROL control) //@suppress("Non-API function naming")
{
    return (ARM_USART_SetModemControl(control, &gs_sci0_resources));
}/* End of function USART0_SetModemControl() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI0
 *******************************************************************************************************************/
/* Function Name: sci0_txi_interrupt */
static void sci0_txi_interrupt(void) //@suppress("Non-API function naming") @suppress("ISR declaration") @suppress("Function definition ordering")
{
    if (NULL == gs_sci0_resources.tx_dma_drv) // @suppress("Cast comment")
    {
        txi_handler(&gs_sci0_resources);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == gs_sci0_resources.tx_dma_drv)
    {
        txi_dtc_handler(&gs_sci0_resources);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        txi_dmac_handler(&gs_sci0_resources);
    }
#endif
}/* End of function sci0_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI0
 *******************************************************************************************************************/
/* Function Name: sci0_rxi_interrupt */
static void sci0_rxi_interrupt(void) //@suppress("Non-API function naming") @suppress("ISR declaration") @suppress("Function definition ordering")
{
    if (NULL == gs_sci0_resources.rx_dma_drv) // @suppress("Cast comment")
    {
        rxi_handler(&gs_sci0_resources);
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        rxi_dmac_handler(&gs_sci0_resources);
    }
#endif
}/* End of function sci0_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI
 *******************************************************************************************************************/
/* Function Name: sci0_eri_interrupt */
static void sci0_eri_interrupt(void) //@suppress("Non-API function naming") @suppress("ISR declaration") @suppress("Function definition ordering")
{
    eri_handler(&gs_sci0_resources);
}/* End of function sci0_eri_interrupt() */
#endif /* (0 != R_SCI0_ENABLE) */

/** SCI1 */
#if (0 != R_SCI1_ENABLE)
static int32_t USART1_Initialize(ARM_USART_SignalEvent_t cb_event) FUNC_LOCATION_ARM_USART_INITIALIZE;
static int32_t USART1_Uninitialize(void) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
static int32_t USART1_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_USART_POWER_CONTROL;
static int32_t USART1_Send(void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_SEND;
static int32_t USART1_Receive(void * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_RECEIVE;
static int32_t USART1_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
                               FUNC_LOCATION_ARM_USART_TRANSFER;
static uint32_t USART1_GetTxCount(void) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
static uint32_t USART1_GetRxCount(void) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
static int32_t USART1_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_USART_CONTROL;
static ARM_USART_STATUS USART1_GetStatus(void) FUNC_LOCATION_ARM_USART_GET_STATUS;
static ARM_USART_MODEM_STATUS USART1_GetModemStatus(void) FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;
static int32_t USART1_SetModemControl(ARM_USART_MODEM_CONTROL control) FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
static void sci1_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
static void sci1_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void sci1_eri_interrupt(void) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;

static st_usart_info_t gs_sci1_info = {0};

#if (SCI_USED_DTC == SCI1_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  gs_sci1_tx_dtc_info;
#endif
#if (SCI_USED_DTC == SCI1_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  gs_sci1_rx_dtc_info;
#endif

/** SCI1 Resources */
static st_usart_resources_t gs_sci1_resources DATA_LOCATION_PRV_SCI_RESOURCES =
{
    (SCI2_Type*)SCI1, // @suppress("Cast comment")
    R_SCI_Pinset_CH1,
    R_SCI_Pinclr_CH1,
    &gs_sci1_info,
#ifdef USART1_CTS_PORT
    (uint16_t*)&USART1_CTS_PORT,
    USART1_CTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif

#ifdef USART1_RTS_PORT
    (uint16_t*)&USART1_RTS_PORT,
    USART1_RTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif
    USART_BASE_PCLKA,
    SYSTEM_LOCK_SCI1,
    LPM_MSTP_SCI1,
    SYSTEM_CFG_EVENT_NUMBER_SCI1_TXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI1_RXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI1_ERI, // @suppress("Cast comment")
    USART_TXI1_IESR_VAL,
    USART_RXI1_IESR_VAL,
    USART_ERI1_IESR_VAL,
    SCI1_TXI_PRIORITY,
    SCI1_RXI_PRIORITY,
    SCI1_ERI_PRIORITY,
#if (SCI_USED_DMAC0 == SCI1_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    USART_TXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI1_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    USART_TXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI1_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    USART_TXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI1_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    USART_TXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI1_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI1_TXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SCI_USED_DMAC0 == SCI1_RECEIVE_CONTROL)
    &Driver_DMAC0,
    USART_RXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI1_RECEIVE_CONTROL)
    &Driver_DMAC1,
    USART_RXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI1_RECEIVE_CONTROL)
    &Driver_DMAC2,
    USART_RXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI1_RECEIVE_CONTROL)
    &Driver_DMAC3,
    USART_RXI1_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI1_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI1_RXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#if (SCI_USED_DTC == SCI1_TRANSMIT_CONTROL)
    &gs_sci1_tx_dtc_info,
#else
    NULL,
#endif
#if (SCI_USED_DTC == SCI1_RECEIVE_CONTROL)
    &gs_sci1_rx_dtc_info,
#else
    NULL,
#endif
#endif
    sci1_txi_interrupt,
    sci1_rxi_interrupt,
    sci1_eri_interrupt
};


/****************************************************************************************************************//**
 * @brief USART initialize method for user applications to initialize the SCI channel1. User applications should call
 * this method through the Driver Instance Driver_USART1.Initialize().
 *
 * @param[in] cb_event   Callback event for USART.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_Initialize */
static int32_t USART1_Initialize(ARM_USART_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_USART_Initialize(cb_event,  &gs_sci1_resources));
}/* End of function USART1_Initialize() */

/****************************************************************************************************************//**
 * @brief USART uninitialize method for user applications to initialize the SCI channel1. User applications should call
 * this method through the Driver Instance Driver_USART1.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_Uninitialize */
static int32_t USART1_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_Uninitialize(&gs_sci1_resources));
}/* End of function USART1_Uninitialize() */

/****************************************************************************************************************//**
 * @brief USART power control method for user applications to control the power of SCI channel1. User applications
 * should call this method through the Driver Instance Driver_USART1.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_PowerControl */
static int32_t USART1_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_USART_PowerControl(state, &gs_sci1_resources));
}/* End of function USART1_PowerControl() */

/****************************************************************************************************************//**
 * @brief USART data transmission method for user applications to send data over SCI channel1. User applications should
 * call this method through the Driver Instance Driver_USART1.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_Send */
static int32_t USART1_Send(void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Send(p_data, num , &gs_sci1_resources));
}/* End of function USART1_Send() */

/****************************************************************************************************************//**
 * @brief USART data reception method for user applications to receive data over SCI channel1. User applications should
 * call this method through the Driver Instance Driver_USART1.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_Receive */
static int32_t USART1_Receive(void * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Receive(p_data, num, &gs_sci1_resources));
}/* End of function USART1_Receive() */

/****************************************************************************************************************//**
 * @brief USART data tranmission method for user applications to send and receive data over SCI channel1 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_USART1.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_Transfer */
static int32_t USART1_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Transfer(p_data_out, p_data_in, num, &gs_sci1_resources));
}/* End of function USART1_Transfer() */

/****************************************************************************************************************//**
 * @brief USART data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SCI channel1. User applications should call this method through the Driver Instance
 * Driver_USART1.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_GetTxCount */
static uint32_t USART1_GetTxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetTxCount(&gs_sci1_resources));
}/* End of function USART1_GetTxCount() */

/***************************************************************************************************************//**
 * @brief USART data reception count getter method for user applications to get the transmission counter value for the
 * RX channel on SCI channel1. User applications should call this method through the Driver
 * Instance Driver_USART1.GetRxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_GetRxCount */
static uint32_t USART1_GetRxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetRxCount(&gs_sci1_resources));
}/* End of function USART1_GetRxCount() */

/****************************************************************************************************************//**
 * @brief USART driver control method for user applications to set USART configuration for SCI channel1. User 
 * applications should call this method through the Driver Instance Driver_USART1.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_Control */
static int32_t USART1_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_USART_Control(control, arg, &gs_sci1_resources));
}/* End of function USART1_Control() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the driver status for SCI channel1. User applications
 * should call this method through the Driver Instance Driver_USART1.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_GetStatus */
static ARM_USART_STATUS USART1_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetStatus(&gs_sci1_resources));
}/* End of function USART1_GetStatus() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the modem status for SCI channel1. User applications
 * should call this method through the Driver Instance Driver_USART1.GetModemStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_GetModemStatus */
static ARM_USART_MODEM_STATUS USART1_GetModemStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetModemStatus(&gs_sci1_resources));
}/* End of function USART1_GetModemStatus() */

/****************************************************************************************************************//**
 * @brief USART status setter method for user applications to set a modem configuration for SCI channel1. User 
 * applications should call this method through the Driver Instance Driver_USART1.SetModemControl().
 * @param[in]  control  ARM_USART_RTS_CLEAR, ARM_USART_RTS_SET, ARM_USART_DTR_SET or ARM_USART_DTR_CLEAR
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART1_SetModemControl */
static int32_t USART1_SetModemControl(ARM_USART_MODEM_CONTROL control) // @suppress("Non-API function naming")
{
    return (ARM_USART_SetModemControl(control, &gs_sci1_resources));
}/* End of function USART1_SetModemControl() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI1
 *******************************************************************************************************************/
/* Function Name: sci1_txi_interrupt */
static void sci1_txi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci1_resources.tx_dma_drv) // @suppress("Cast comment")
    {
        txi_handler(&gs_sci1_resources);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == gs_sci1_resources.tx_dma_drv)
    {
        txi_dtc_handler(&gs_sci1_resources);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        txi_dmac_handler(&gs_sci1_resources);
    }
#endif
}/* End of function sci1_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI1
 *******************************************************************************************************************/
/* Function Name: sci1_rxi_interrupt */
static void sci1_rxi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci1_resources.rx_dma_drv) // @suppress("Cast comment")
    {
        rxi_handler(&gs_sci1_resources);
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        rxi_dmac_handler(&gs_sci1_resources);
    }
#endif
}/* End of function sci1_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI
 *******************************************************************************************************************/
/* Function Name: sci1_eri_interrupt */
static void sci1_eri_interrupt(void) // @suppress("ISR declaration")
{
    eri_handler(&gs_sci1_resources);
}/* End of function sci1_eri_interrupt() */
#endif /* (0 != R_SCI1_ENABLE) */

/** SCI2 */
#if (0 != R_SCI2_ENABLE)
static int32_t USART2_Initialize(ARM_USART_SignalEvent_t cb_event) FUNC_LOCATION_ARM_USART_INITIALIZE;
static int32_t USART2_Uninitialize(void) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
static int32_t USART2_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_USART_POWER_CONTROL;
static int32_t USART2_Send(void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_SEND;
static int32_t USART2_Receive(void * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_RECEIVE;
static int32_t USART2_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
                               FUNC_LOCATION_ARM_USART_TRANSFER;
static uint32_t USART2_GetTxCount(void) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
static uint32_t USART2_GetRxCount(void) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
static int32_t USART2_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_USART_CONTROL;
static ARM_USART_STATUS USART2_GetStatus(void) FUNC_LOCATION_ARM_USART_GET_STATUS;
static ARM_USART_MODEM_STATUS USART2_GetModemStatus(void) FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;
static int32_t USART2_SetModemControl(ARM_USART_MODEM_CONTROL control) FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
static void sci2_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
static void sci2_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void sci2_eri_interrupt(void) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;

static st_usart_info_t gs_sci2_info = {0};

#if (SCI_USED_DTC == SCI2_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  gs_sci2_tx_dtc_info;
#endif
#if (SCI_USED_DTC == SCI2_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  gs_sci2_rx_dtc_info;
#endif

/** SCI2 Resources */
static st_usart_resources_t gs_sci2_resources DATA_LOCATION_PRV_SCI_RESOURCES =
{
    (SCI2_Type*)SCI2, // @suppress("Cast comment")
    R_SCI_Pinset_CH2,
    R_SCI_Pinclr_CH2,
    &gs_sci2_info,
#ifdef USART2_CTS_PORT
    (uint16_t*)&USART2_CTS_PORT,
    USART2_CTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif

#ifdef USART2_RTS_PORT
    (uint16_t*)&USART2_RTS_PORT,
    USART2_RTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif
    USART_BASE_PCLKB,
    SYSTEM_LOCK_SCI2,
    LPM_MSTP_SCI2,
    SYSTEM_CFG_EVENT_NUMBER_SCI2_TXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI2_RXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI2_ERI, // @suppress("Cast comment")
    USART_TXI2_IESR_VAL,
    USART_RXI2_IESR_VAL,
    USART_ERI2_IESR_VAL,
    SCI2_TXI_PRIORITY,
    SCI2_RXI_PRIORITY,
    SCI2_ERI_PRIORITY,
#if (SCI_USED_DMAC0 == SCI2_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    USART_TXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI2_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    USART_TXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI2_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    USART_TXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI2_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    USART_TXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI2_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI2_TXI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SCI_USED_DMAC0 == SCI2_RECEIVE_CONTROL)
    &Driver_DMAC0,
    USART_RXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI2_RECEIVE_CONTROL)
    &Driver_DMAC1,
    USART_RXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI2_RECEIVE_CONTROL)
    &Driver_DMAC2,
    USART_RXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI2_RECEIVE_CONTROL)
    &Driver_DMAC3,
    USART_RXI2_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI2_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI2_RXI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#if (SCI_USED_DTC == SCI2_TRANSMIT_CONTROL)
    &gs_sci2_tx_dtc_info,
#else
    NULL,
#endif
#if (SCI_USED_DTC == SCI2_RECEIVE_CONTROL)
    &gs_sci2_rx_dtc_info,
#else
    NULL,
#endif
#endif
    sci2_txi_interrupt,
    sci2_rxi_interrupt,
    sci2_eri_interrupt
};


/****************************************************************************************************************//**
 * @brief USART initialize method for user applications to initialize the SCI channel2. User applications should call
 * this method through the Driver Instance Driver_USART2.Initialize().
 *
 * @param[in] cb_event   Callback event for USART.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_Initialize */
static int32_t USART2_Initialize(ARM_USART_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_USART_Initialize(cb_event,  &gs_sci2_resources));
}/* End of function USART2_Initialize() */

/****************************************************************************************************************//**
 * @brief USART uninitialize method for user applications to initialize the SCI channel2. User applications should call
 * this method through the Driver Instance Driver_USART2.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_Uninitialize */
static int32_t USART2_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_Uninitialize(&gs_sci2_resources));
}/* End of function USART2_Uninitialize() */

/****************************************************************************************************************//**
 * @brief USART power control method for user applications to control the power of SCI channel2. User applications
 * should call this method through the Driver Instance Driver_USART2.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_PowerControl */
static int32_t USART2_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_USART_PowerControl(state, &gs_sci2_resources));
}/* End of function USART2_PowerControl() */

/****************************************************************************************************************//**
 * @brief USART data transmission method for user applications to send data over SCI channel2. User applications should
 * call this method through the Driver Instance Driver_USART2.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_Send */
static int32_t USART2_Send(void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Send(p_data, num , &gs_sci2_resources));
}/* End of function USART2_Send() */

/****************************************************************************************************************//**
 * @brief USART data reception method for user applications to receive data over SCI channel2. User applications should
 * call this method through the Driver Instance Driver_USART2.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_Receive */
static int32_t USART2_Receive(void * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Receive(p_data, num, &gs_sci2_resources));
}/* End of function USART2_Receive() */

/****************************************************************************************************************//**
 * @brief USART data tranmission method for user applications to send and receive data over SCI channel2 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_USART2.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_Transfer */
static int32_t USART2_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Transfer(p_data_out, p_data_in, num, &gs_sci2_resources));
}/* End of function USART2_Transfer() */

/****************************************************************************************************************//**
 * @brief USART data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SCI channel2. User applications should call this method through the Driver Instance
 * Driver_USART2.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_GetTxCount */
static uint32_t USART2_GetTxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetTxCount(&gs_sci2_resources));
}/* End of function USART2_GetTxCount() */

/***************************************************************************************************************//**
 * @brief USART data reception count getter method for user applications to get the transmission counter value for the
 * RX channel on SCI channel2. User applications should call this method through the Driver
 * Instance Driver_USART2.GetRxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_GetRxCount */
static uint32_t USART2_GetRxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetRxCount(&gs_sci2_resources));
}/* End of function USART2_GetRxCount() */

/****************************************************************************************************************//**
 * @brief USART driver control method for user applications to set USART configuration for SCI channel2. User 
 * applications should call this method through the Driver Instance Driver_USART2.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_Control */
static int32_t USART2_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_USART_Control(control, arg, &gs_sci2_resources));
}/* End of function USART2_Control() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the driver status for SCI channel2. User applications
 * should call this method through the Driver Instance Driver_USART2.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_GetStatus */
static ARM_USART_STATUS USART2_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetStatus(&gs_sci2_resources));
}/* End of function USART2_GetStatus() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the modem status for SCI channel2. User applications
 * should call this method through the Driver Instance Driver_USART2.GetModemStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_GetModemStatus */
static ARM_USART_MODEM_STATUS USART2_GetModemStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetModemStatus(&gs_sci2_resources));
}/* End of function USART2_GetModemStatus() */

/****************************************************************************************************************//**
 * @brief USART status setter method for user applications to set a modem configuration for SCI channel2. User 
 * applications should call this method through the Driver Instance Driver_USART2.SetModemControl().
 * @param[in]  control  ARM_USART_RTS_CLEAR, ARM_USART_RTS_SET, ARM_USART_DTR_SET or ARM_USART_DTR_CLEAR
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART2_SetModemControl */
static int32_t USART2_SetModemControl(ARM_USART_MODEM_CONTROL control) // @suppress("Non-API function naming")
{
    return (ARM_USART_SetModemControl(control, &gs_sci2_resources));
}/* End of function USART2_SetModemControl() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI2
 *******************************************************************************************************************/
/* Function Name: sci2_txi_interrupt */
static void sci2_txi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci2_resources.tx_dma_drv) // @suppress("Cast comment")
    {
        txi_handler(&gs_sci2_resources);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == gs_sci2_resources.tx_dma_drv)
    {
        txi_dtc_handler(&gs_sci2_resources);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        txi_dmac_handler(&gs_sci2_resources);
    }
#endif
}/* End of function sci2_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI2
 *******************************************************************************************************************/
/* Function Name: sci2_rxi_interrupt */
static void sci2_rxi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci2_resources.rx_dma_drv) // @suppress("Cast comment")
    {
        rxi_handler(&gs_sci2_resources);
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        rxi_dmac_handler(&gs_sci2_resources);
    }
#endif
}/* End of function sci2_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI
 *******************************************************************************************************************/
/* Function Name: sci2_eri_interrupt */
static void sci2_eri_interrupt(void) // @suppress("ISR declaration")
{
    eri_handler(&gs_sci2_resources);
}/* End of function sci2_eri_interrupt() */
#endif /* (0 != R_SCI2_ENABLE) */

/** SCI3 */
#if (0 != R_SCI3_ENABLE)
static int32_t USART3_Initialize(ARM_USART_SignalEvent_t cb_event) FUNC_LOCATION_ARM_USART_INITIALIZE;
static int32_t USART3_Uninitialize(void) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
static int32_t USART3_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_USART_POWER_CONTROL;
static int32_t USART3_Send(void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_SEND;
static int32_t USART3_Receive(void * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_RECEIVE;
static int32_t USART3_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
                               FUNC_LOCATION_ARM_USART_TRANSFER;
static uint32_t USART3_GetTxCount(void) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
static uint32_t USART3_GetRxCount(void) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
static int32_t USART3_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_USART_CONTROL;
static ARM_USART_STATUS USART3_GetStatus(void) FUNC_LOCATION_ARM_USART_GET_STATUS;
static ARM_USART_MODEM_STATUS USART3_GetModemStatus(void) FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;
static int32_t USART3_SetModemControl(ARM_USART_MODEM_CONTROL control) FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
static void sci3_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
static void sci3_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void sci3_eri_interrupt(void) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;

static st_usart_info_t gs_sci3_info = {0};

#if (SCI_USED_DTC == SCI3_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  gs_sci3_tx_dtc_info;
#endif
#if (SCI_USED_DTC == SCI3_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  gs_sci3_rx_dtc_info;
#endif

/** SCI3 Resources */
static st_usart_resources_t gs_sci3_resources DATA_LOCATION_PRV_SCI_RESOURCES =
{
    (SCI2_Type*)SCI3, // @suppress("Cast comment")
    R_SCI_Pinset_CH3,
    R_SCI_Pinclr_CH3,
    &gs_sci3_info,
#ifdef USART3_CTS_PORT
    (uint16_t*)&USART3_CTS_PORT,
    USART3_CTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif

#ifdef USART3_RTS_PORT
    (uint16_t*)&USART3_RTS_PORT,
    USART3_RTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif
    USART_BASE_PCLKB,
    SYSTEM_LOCK_SCI3,
    LPM_MSTP_SCI3,
    SYSTEM_CFG_EVENT_NUMBER_SCI3_TXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI3_RXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI3_ERI, // @suppress("Cast comment")
    USART_TXI3_IESR_VAL,
    USART_RXI3_IESR_VAL,
    USART_ERI3_IESR_VAL,
    SCI3_TXI_PRIORITY,
    SCI3_RXI_PRIORITY,
    SCI3_ERI_PRIORITY,
#if (SCI_USED_DMAC0 == SCI3_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    USART_TXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI3_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    USART_TXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI3_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    USART_TXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI3_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    USART_TXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI3_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI3_TXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SCI_USED_DMAC0 == SCI3_RECEIVE_CONTROL)
    &Driver_DMAC0,
    USART_RXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI3_RECEIVE_CONTROL)
    &Driver_DMAC1,
    USART_RXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI3_RECEIVE_CONTROL)
    &Driver_DMAC2,
    USART_RXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI3_RECEIVE_CONTROL)
    &Driver_DMAC3,
    USART_RXI3_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI3_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI3_RXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#if (SCI_USED_DTC == SCI3_TRANSMIT_CONTROL)
    &gs_sci3_tx_dtc_info,
#else
    NULL,
#endif
#if (SCI_USED_DTC == SCI3_RECEIVE_CONTROL)
    &gs_sci3_rx_dtc_info,
#else
    NULL,
#endif
#endif
    sci3_txi_interrupt,
    sci3_rxi_interrupt,
    sci3_eri_interrupt
};


/****************************************************************************************************************//**
 * @brief USART initialize method for user applications to initialize the SCI channel3. User applications should call
 * this method through the Driver Instance Driver_USART3.Initialize().
 *
 * @param[in] cb_event   Callback event for USART.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_Initialize */
static int32_t USART3_Initialize(ARM_USART_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_USART_Initialize(cb_event,  &gs_sci3_resources));
}/* End of function USART3_Initialize() */

/****************************************************************************************************************//**
 * @brief USART uninitialize method for user applications to initialize the SCI channel3. User applications should call
 * this method through the Driver Instance Driver_USART3.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_Uninitialize */
static int32_t USART3_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_Uninitialize(&gs_sci3_resources));
}/* End of function USART3_Uninitialize() */

/****************************************************************************************************************//**
 * @brief USART power control method for user applications to control the power of SCI channel3. User applications
 * should call this method through the Driver Instance Driver_USART3.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_PowerControl */
static int32_t USART3_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_USART_PowerControl(state, &gs_sci3_resources));
}/* End of function USART3_PowerControl() */

/****************************************************************************************************************//**
 * @brief USART data transmission method for user applications to send data over SCI channel3. User applications should
 * call this method through the Driver Instance Driver_USART3.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_Send */
static int32_t USART3_Send(void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Send(p_data, num , &gs_sci3_resources));
}/* End of function USART3_Send() */

/****************************************************************************************************************//**
 * @brief USART data reception method for user applications to receive data over SCI channel3. User applications should
 * call this method through the Driver Instance Driver_USART3.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_Receive */
static int32_t USART3_Receive(void * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Receive(p_data, num, &gs_sci3_resources));
}/* End of function USART3_Receive() */

/****************************************************************************************************************//**
 * @brief USART data tranmission method for user applications to send and receive data over SCI channel3 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_USART3.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_Transfer */
static int32_t USART3_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Transfer(p_data_out, p_data_in, num, &gs_sci3_resources));
}/* End of function USART3_Transfer() */

/****************************************************************************************************************//**
 * @brief USART data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SCI channel3. User applications should call this method through the Driver Instance
 * Driver_USART3.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_GetTxCount */
static uint32_t USART3_GetTxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetTxCount(&gs_sci3_resources));
}/* End of function USART3_GetTxCount() */

/***************************************************************************************************************//**
 * @brief USART data reception count getter method for user applications to get the transmission counter value for the
 * RX channel on SCI channel3. User applications should call this method through the Driver
 * Instance Driver_USART3.GetRxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_GetRxCount */
static uint32_t USART3_GetRxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetRxCount(&gs_sci3_resources));
}/* End of function USART3_GetRxCount() */

/****************************************************************************************************************//**
 * @brief USART driver control method for user applications to set USART configuration for SCI channel3. User 
 * applications should call this method through the Driver Instance Driver_USART3.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_Control */
static int32_t USART3_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_USART_Control(control, arg, &gs_sci3_resources));
}/* End of function USART3_Control() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the driver status for SCI channel3. User applications
 * should call this method through the Driver Instance Driver_USART3.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_GetStatus */
static ARM_USART_STATUS USART3_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetStatus(&gs_sci3_resources));
}/* End of function USART3_GetStatus() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the modem status for SCI channel3. User applications
 * should call this method through the Driver Instance Driver_USART3.GetModemStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_GetModemStatus */
static ARM_USART_MODEM_STATUS USART3_GetModemStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetModemStatus(&gs_sci3_resources));
}/* End of function USART3_GetModemStatus() */

/****************************************************************************************************************//**
 * @brief USART status setter method for user applications to set a modem configuration for SCI channel3. User 
 * applications should call this method through the Driver Instance Driver_USART3.SetModemControl().
 * @param[in]  control  ARM_USART_RTS_CLEAR, ARM_USART_RTS_SET, ARM_USART_DTR_SET or ARM_USART_DTR_CLEAR
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART3_SetModemControl */
static int32_t USART3_SetModemControl(ARM_USART_MODEM_CONTROL control) // @suppress("Non-API function naming")
{
    return (ARM_USART_SetModemControl(control, &gs_sci3_resources));
}/* End of function USART3_SetModemControl() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI3
 *******************************************************************************************************************/
/* Function Name: sci3_txi_interrupt */
static void sci3_txi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci3_resources.tx_dma_drv) // @suppress("Cast comment")
    {
        txi_handler(&gs_sci3_resources);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == gs_sci3_resources.tx_dma_drv)
    {
        txi_dtc_handler(&gs_sci3_resources);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        txi_dmac_handler(&gs_sci3_resources);
    }
#endif
}/* End of function sci3_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI3
 *******************************************************************************************************************/
/* Function Name: sci3_rxi_interrupt */
static void sci3_rxi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci3_resources.rx_dma_drv) // @suppress("Cast comment")
    {
        rxi_handler(&gs_sci3_resources);
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        rxi_dmac_handler(&gs_sci3_resources);
    }
#endif
}/* End of function sci3_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI
 *******************************************************************************************************************/
/* Function Name: sci3_eri_interrupt */
static void sci3_eri_interrupt(void) // @suppress("ISR declaration")
{
    eri_handler(&gs_sci3_resources);
}/* End of function sci3_eri_interrupt() */
#endif /* (0 != R_SCI3_ENABLE) */

/** SCI4 */
#if (0 != R_SCI4_ENABLE)
static int32_t USART4_Initialize(ARM_USART_SignalEvent_t cb_event) FUNC_LOCATION_ARM_USART_INITIALIZE;
static int32_t USART4_Uninitialize(void) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
static int32_t USART4_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_USART_POWER_CONTROL;
static int32_t USART4_Send(void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_SEND;
static int32_t USART4_Receive(void * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_RECEIVE;
static int32_t USART4_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
                               FUNC_LOCATION_ARM_USART_TRANSFER;
static uint32_t USART4_GetTxCount(void) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
static uint32_t USART4_GetRxCount(void) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
static int32_t USART4_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_USART_CONTROL;
static ARM_USART_STATUS USART4_GetStatus(void) FUNC_LOCATION_ARM_USART_GET_STATUS;
static ARM_USART_MODEM_STATUS USART4_GetModemStatus(void) FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;
static int32_t USART4_SetModemControl(ARM_USART_MODEM_CONTROL control) FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
static void sci4_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
static void sci4_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void sci4_eri_interrupt(void) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;

static st_usart_info_t gs_sci4_info = {0};

#if (SCI_USED_DTC == SCI4_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  gs_sci4_tx_dtc_info;
#endif
#if (SCI_USED_DTC == SCI4_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  gs_sci4_rx_dtc_info;
#endif

/** SCI4 Resources */
static st_usart_resources_t gs_sci4_resources DATA_LOCATION_PRV_SCI_RESOURCES =
{
    (SCI2_Type*)SCI4, // @suppress("Cast comment")
    R_SCI_Pinset_CH4,
    R_SCI_Pinclr_CH4,
    &gs_sci4_info,
#ifdef USART4_CTS_PORT
    (uint16_t*)&USART4_CTS_PORT,
    USART4_CTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif

#ifdef USART4_RTS_PORT
    (uint16_t*)&USART4_RTS_PORT,
    USART4_RTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif
    USART_BASE_PCLKB,
    SYSTEM_LOCK_SCI4,
    LPM_MSTP_SCI4,
    SYSTEM_CFG_EVENT_NUMBER_SCI4_TXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI4_RXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI4_ERI, // @suppress("Cast comment")
    USART_TXI4_IESR_VAL,
    USART_RXI4_IESR_VAL,
    USART_ERI4_IESR_VAL,
    SCI4_TXI_PRIORITY,
    SCI4_RXI_PRIORITY,
    SCI4_ERI_PRIORITY,
#if (SCI_USED_DMAC0 == SCI4_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    USART_TXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI4_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    USART_TXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI4_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    USART_TXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI4_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    USART_TXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI4_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI4_TXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SCI_USED_DMAC0 == SCI4_RECEIVE_CONTROL)
    &Driver_DMAC0,
    USART_RXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI4_RECEIVE_CONTROL)
    &Driver_DMAC1,
    USART_RXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI4_RECEIVE_CONTROL)
    &Driver_DMAC2,
    USART_RXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI4_RECEIVE_CONTROL)
    &Driver_DMAC3,
    USART_RXI4_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI4_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI4_RXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#if (SCI_USED_DTC == SCI4_TRANSMIT_CONTROL)
    &gs_sci4_tx_dtc_info,
#else
    NULL,
#endif
#if (SCI_USED_DTC == SCI4_RECEIVE_CONTROL)
    &gs_sci4_rx_dtc_info,
#else
    NULL,
#endif
#endif
    sci4_txi_interrupt,
    sci4_rxi_interrupt,
    sci4_eri_interrupt
};


/****************************************************************************************************************//**
 * @brief USART initialize method for user applications to initialize the SCI channel4. User applications should call
 * this method through the Driver Instance Driver_USART4.Initialize().
 *
 * @param[in] cb_event   Callback event for USART.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_Initialize */
static int32_t USART4_Initialize(ARM_USART_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_USART_Initialize(cb_event,  &gs_sci4_resources));
}/* End of function USART4_Initialize() */

/****************************************************************************************************************//**
 * @brief USART uninitialize method for user applications to initialize the SCI channel4. User applications should call
 * this method through the Driver Instance Driver_USART4.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_Uninitialize */
static int32_t USART4_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_Uninitialize(&gs_sci4_resources));
}/* End of function USART4_Uninitialize() */

/****************************************************************************************************************//**
 * @brief USART power control method for user applications to control the power of SCI channel4. User applications
 * should call this method through the Driver Instance Driver_USART4.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_PowerControl */
static int32_t USART4_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_USART_PowerControl(state, &gs_sci4_resources));
}/* End of function USART4_PowerControl() */

/****************************************************************************************************************//**
 * @brief USART data transmission method for user applications to send data over SCI channel4. User applications should
 * call this method through the Driver Instance Driver_USART4.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_Send */
static int32_t USART4_Send(void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Send(p_data, num , &gs_sci4_resources));
}/* End of function USART4_Send() */

/****************************************************************************************************************//**
 * @brief USART data reception method for user applications to receive data over SCI channel4. User applications should
 * call this method through the Driver Instance Driver_USART4.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_Receive */
static int32_t USART4_Receive(void * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Receive(p_data, num, &gs_sci4_resources));
}/* End of function USART4_Receive() */

/****************************************************************************************************************//**
 * @brief USART data tranmission method for user applications to send and receive data over SCI channel4 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_USART4.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_Transfer */
static int32_t USART4_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Transfer(p_data_out, p_data_in, num, &gs_sci4_resources));
}/* End of function USART4_Transfer() */

/****************************************************************************************************************//**
 * @brief USART data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SCI channel4. User applications should call this method through the Driver Instance
 * Driver_USART4.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_GetTxCount */
static uint32_t USART4_GetTxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetTxCount(&gs_sci4_resources));
}/* End of function USART4_GetTxCount() */

/***************************************************************************************************************//**
 * @brief USART data reception count getter method for user applications to get the transmission counter value for the
 * RX channel on SCI channel4. User applications should call this method through the Driver
 * Instance Driver_USART4.GetRxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_GetRxCount */
static uint32_t USART4_GetRxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetRxCount(&gs_sci4_resources));
}/* End of function USART4_GetRxCount() */

/****************************************************************************************************************//**
 * @brief USART driver control method for user applications to set USART configuration for SCI channel4. User 
 * applications should call this method through the Driver Instance Driver_USART4.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_Control */
static int32_t USART4_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_USART_Control(control, arg, &gs_sci4_resources));
}/* End of function USART4_Control() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the driver status for SCI channel4. User applications
 * should call this method through the Driver Instance Driver_USART4.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_GetStatus */
static ARM_USART_STATUS USART4_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetStatus(&gs_sci4_resources));
}/* End of function USART4_GetStatus() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the modem status for SCI channel4. User applications
 * should call this method through the Driver Instance Driver_USART4.GetModemStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_GetModemStatus */
static ARM_USART_MODEM_STATUS USART4_GetModemStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetModemStatus(&gs_sci4_resources));
}/* End of function USART4_GetModemStatus() */

/****************************************************************************************************************//**
 * @brief USART status setter method for user applications to set a modem configuration for SCI channel4. User 
 * applications should call this method through the Driver Instance Driver_USART4.SetModemControl().
 * @param[in]  control  ARM_USART_RTS_CLEAR, ARM_USART_RTS_SET, ARM_USART_DTR_SET or ARM_USART_DTR_CLEAR
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART4_SetModemControl */
static int32_t USART4_SetModemControl(ARM_USART_MODEM_CONTROL control) // @suppress("Non-API function naming")
{
    return (ARM_USART_SetModemControl(control, &gs_sci4_resources));
}/* End of function USART4_SetModemControl() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI4
 *******************************************************************************************************************/
/* Function Name: sci4_txi_interrupt */
static void sci4_txi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci4_resources.tx_dma_drv) // @suppress("Cast comment")
    {
        txi_handler(&gs_sci4_resources);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == gs_sci4_resources.tx_dma_drv)
    {
        txi_dtc_handler(&gs_sci4_resources);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        txi_dmac_handler(&gs_sci4_resources);
    }
#endif
}/* End of function sci4_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI4
 *******************************************************************************************************************/
/* Function Name: sci4_rxi_interrupt */
static void sci4_rxi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci4_resources.rx_dma_drv) // @suppress("Cast comment")
    {
        rxi_handler(&gs_sci4_resources);
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        rxi_dmac_handler(&gs_sci4_resources);
    }
#endif
}/* End of function sci4_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI
 *******************************************************************************************************************/
/* Function Name: sci4_eri_interrupt */
static void sci4_eri_interrupt(void) // @suppress("ISR declaration")
{
    eri_handler(&gs_sci4_resources);
}/* End of function sci4_eri_interrupt() */
#endif /* (0 != R_SCI4_ENABLE) */

/** SCI5 */
#if (0 != R_SCI5_ENABLE)
static int32_t USART5_Initialize(ARM_USART_SignalEvent_t cb_event) FUNC_LOCATION_ARM_USART_INITIALIZE;
static int32_t USART5_Uninitialize(void) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
static int32_t USART5_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_USART_POWER_CONTROL;
static int32_t USART5_Send(void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_SEND;
static int32_t USART5_Receive(void * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_RECEIVE;
static int32_t USART5_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
                               FUNC_LOCATION_ARM_USART_TRANSFER;
static uint32_t USART5_GetTxCount(void) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
static uint32_t USART5_GetRxCount(void) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
static int32_t USART5_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_USART_CONTROL;
static ARM_USART_STATUS USART5_GetStatus(void) FUNC_LOCATION_ARM_USART_GET_STATUS;
static ARM_USART_MODEM_STATUS USART5_GetModemStatus(void) FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;
static int32_t USART5_SetModemControl(ARM_USART_MODEM_CONTROL control) FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
static void sci5_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
static void sci5_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void sci5_eri_interrupt(void) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;

static st_usart_info_t gs_sci5_info = {0};

#if (SCI_USED_DTC == SCI5_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  gs_sci5_tx_dtc_info;
#endif
#if (SCI_USED_DTC == SCI5_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  gs_sci5_rx_dtc_info;
#endif

/** SCI5 Resources */
static st_usart_resources_t gs_sci5_resources DATA_LOCATION_PRV_SCI_RESOURCES =
{
    (SCI2_Type*)SCI5, // @suppress("Cast comment")
    R_SCI_Pinset_CH5,
    R_SCI_Pinclr_CH5,
    &gs_sci5_info,
#ifdef USART5_CTS_PORT
    (uint16_t*)&USART5_CTS_PORT,
    USART5_CTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif

#ifdef USART5_RTS_PORT
    (uint16_t*)&USART5_RTS_PORT,
    USART5_RTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif
    USART_BASE_PCLKB,
    SYSTEM_LOCK_SCI5,
    LPM_MSTP_SCI5,
    SYSTEM_CFG_EVENT_NUMBER_SCI5_TXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI5_RXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI5_ERI, // @suppress("Cast comment")
    USART_TXI5_IESR_VAL,
    USART_RXI5_IESR_VAL,
    USART_ERI5_IESR_VAL,
    SCI5_TXI_PRIORITY,
    SCI5_RXI_PRIORITY,
    SCI5_ERI_PRIORITY,
#if (SCI_USED_DMAC0 == SCI5_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    USART_TXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI5_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    USART_TXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI5_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    USART_TXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI5_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    USART_TXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI5_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI5_TXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SCI_USED_DMAC0 == SCI5_RECEIVE_CONTROL)
    &Driver_DMAC0,
    USART_RXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI5_RECEIVE_CONTROL)
    &Driver_DMAC1,
    USART_RXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI5_RECEIVE_CONTROL)
    &Driver_DMAC2,
    USART_RXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI5_RECEIVE_CONTROL)
    &Driver_DMAC3,
    USART_RXI5_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI5_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI5_RXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#if (SCI_USED_DTC == SCI5_TRANSMIT_CONTROL)
    &gs_sci5_tx_dtc_info,
#else
    NULL,
#endif
#if (SCI_USED_DTC == SCI5_RECEIVE_CONTROL)
    &gs_sci5_rx_dtc_info,
#else
    NULL,
#endif
#endif
    sci5_txi_interrupt,
    sci5_rxi_interrupt,
    sci5_eri_interrupt
};


/****************************************************************************************************************//**
 * @brief USART initialize method for user applications to initialize the SCI channel5. User applications should call
 * this method through the Driver Instance Driver_USART5.Initialize().
 *
 * @param[in] cb_event   Callback event for USART.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_Initialize */
static int32_t USART5_Initialize(ARM_USART_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_USART_Initialize(cb_event,  &gs_sci5_resources));
}/* End of function USART5_Initialize() */

/****************************************************************************************************************//**
 * @brief USART uninitialize method for user applications to initialize the SCI channel5. User applications should call
 * this method through the Driver Instance Driver_USART5.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_Uninitialize */
static int32_t USART5_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_Uninitialize(&gs_sci5_resources));
}/* End of function USART5_Uninitialize() */

/****************************************************************************************************************//**
 * @brief USART power control method for user applications to control the power of SCI channel5. User applications
 * should call this method through the Driver Instance Driver_USART5.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_PowerControl */
static int32_t USART5_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_USART_PowerControl(state, &gs_sci5_resources));
}/* End of function USART5_PowerControl() */

/****************************************************************************************************************//**
 * @brief USART data transmission method for user applications to send data over SCI channel5. User applications should
 * call this method through the Driver Instance Driver_USART5.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_Send */
static int32_t USART5_Send(void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Send(p_data, num , &gs_sci5_resources));
}/* End of function USART5_Send() */

/****************************************************************************************************************//**
 * @brief USART data reception method for user applications to receive data over SCI channel5. User applications should
 * call this method through the Driver Instance Driver_USART5.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_Receive */
static int32_t USART5_Receive(void * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Receive(p_data, num, &gs_sci5_resources));
}/* End of function USART5_Receive() */

/****************************************************************************************************************//**
 * @brief USART data tranmission method for user applications to send and receive data over SCI channel5 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_USART5.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_Transfer */
static int32_t USART5_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Transfer(p_data_out, p_data_in, num, &gs_sci5_resources));
}/* End of function USART5_Transfer() */

/****************************************************************************************************************//**
 * @brief USART data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SCI channel5. User applications should call this method through the Driver Instance
 * Driver_USART5.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_GetTxCount */
static uint32_t USART5_GetTxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetTxCount(&gs_sci5_resources));
}/* End of function USART5_GetTxCount() */

/***************************************************************************************************************//**
 * @brief USART data reception count getter method for user applications to get the transmission counter value for the
 * RX channel on SCI channel5. User applications should call this method through the Driver
 * Instance Driver_USART5.GetRxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_GetRxCount */
static uint32_t USART5_GetRxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetRxCount(&gs_sci5_resources));
}/* End of function USART5_GetRxCount() */

/****************************************************************************************************************//**
 * @brief USART driver control method for user applications to set USART configuration for SCI channel5. User 
 * applications should call this method through the Driver Instance Driver_USART5.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_Control */
static int32_t USART5_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_USART_Control(control, arg, &gs_sci5_resources));
}/* End of function USART5_Control() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the driver status for SCI channel5. User applications
 * should call this method through the Driver Instance Driver_USART5.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_GetStatus */
static ARM_USART_STATUS USART5_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetStatus(&gs_sci5_resources));
}/* End of function USART5_GetStatus() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the modem status for SCI channel5. User applications
 * should call this method through the Driver Instance Driver_USART5.GetModemStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_GetModemStatus */
static ARM_USART_MODEM_STATUS USART5_GetModemStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetModemStatus(&gs_sci5_resources));
}/* End of function USART5_GetModemStatus() */

/****************************************************************************************************************//**
 * @brief USART status setter method for user applications to set a modem configuration for SCI channel5. User 
 * applications should call this method through the Driver Instance Driver_USART5.SetModemControl().
 * @param[in]  control  ARM_USART_RTS_CLEAR, ARM_USART_RTS_SET, ARM_USART_DTR_SET or ARM_USART_DTR_CLEAR
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART5_SetModemControl */
static int32_t USART5_SetModemControl(ARM_USART_MODEM_CONTROL control) // @suppress("Non-API function naming")
{
    return (ARM_USART_SetModemControl(control, &gs_sci5_resources));
}/* End of function USART5_SetModemControl() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI5
 *******************************************************************************************************************/
/* Function Name: sci5_txi_interrupt */
static void sci5_txi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci5_resources.tx_dma_drv) // @suppress("Cast comment")
    {
        txi_handler(&gs_sci5_resources);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == gs_sci5_resources.tx_dma_drv)
    {
        txi_dtc_handler(&gs_sci5_resources);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        txi_dmac_handler(&gs_sci5_resources);
    }
#endif
}/* End of function sci5_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI5
 *******************************************************************************************************************/
/* Function Name: sci5_rxi_interrupt */
static void sci5_rxi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci5_resources.rx_dma_drv) // @suppress("Cast comment")
    {
        rxi_handler(&gs_sci5_resources);
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        rxi_dmac_handler(&gs_sci5_resources);
    }
#endif
}/* End of function sci5_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI
 *******************************************************************************************************************/
/* Function Name: sci5_eri_interrupt */
static void sci5_eri_interrupt(void) // @suppress("ISR declaration")
{
    eri_handler(&gs_sci5_resources);
}/* End of function sci5_eri_interrupt() */
#endif /* (0 != R_SCI5_ENABLE) */

/** SCI9 */
#if (0 != R_SCI9_ENABLE)
static int32_t USART9_Initialize(ARM_USART_SignalEvent_t cb_event) FUNC_LOCATION_ARM_USART_INITIALIZE;
static int32_t USART9_Uninitialize(void) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
static int32_t USART9_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_USART_POWER_CONTROL;
static int32_t USART9_Send(void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_SEND;
static int32_t USART9_Receive(void * const p_data, uint32_t num) FUNC_LOCATION_ARM_USART_RECEIVE;
static int32_t USART9_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
                               FUNC_LOCATION_ARM_USART_TRANSFER;
static uint32_t USART9_GetTxCount(void) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
static uint32_t USART9_GetRxCount(void) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
static int32_t USART9_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_USART_CONTROL;
static ARM_USART_STATUS USART9_GetStatus(void) FUNC_LOCATION_ARM_USART_GET_STATUS;
static ARM_USART_MODEM_STATUS USART9_GetModemStatus(void) FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;
static int32_t USART9_SetModemControl(ARM_USART_MODEM_CONTROL control) FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
static void sci9_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_TXI_HANDLER;
static void sci9_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_RXI_HANDLER;
static void sci9_eri_interrupt(void) STATIC_FUNC_LOCATION_PRV_ERI_HANDLER;

static st_usart_info_t gs_sci9_info = {0};

#if (SCI_USED_DTC == SCI9_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  gs_sci9_tx_dtc_info;
#endif
#if (SCI_USED_DTC == SCI9_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  gs_sci9_rx_dtc_info;
#endif

/** SCI9 Resources */
static st_usart_resources_t gs_sci9_resources DATA_LOCATION_PRV_SCI_RESOURCES = 
{
    (SCI2_Type*)SCI9, // @suppress("Cast comment")
    R_SCI_Pinset_CH9,
    R_SCI_Pinclr_CH9,
    &gs_sci9_info,
#ifdef USART9_CTS_PORT
    (uint16_t*)&USART9_CTS_PORT,
    USART9_CTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif

#ifdef USART9_RTS_PORT
    (uint16_t*)&USART9_RTS_PORT,
    USART9_RTS_PIN,
#else
    (uint16_t*)NULL, // @suppress("Cast comment")
    0,
#endif
    USART_BASE_PCLKB,
    SYSTEM_LOCK_SCI9,
    LPM_MSTP_SCI9,
    SYSTEM_CFG_EVENT_NUMBER_SCI9_TXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI9_RXI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_SCI9_ERI, // @suppress("Cast comment")
    USART_TXI9_IESR_VAL,
    USART_RXI9_IESR_VAL,
    USART_ERI9_IESR_VAL,
    SCI9_TXI_PRIORITY,
    SCI9_RXI_PRIORITY,
    SCI9_ERI_PRIORITY,
#if (SCI_USED_DMAC0 == SCI9_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    USART_TXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI9_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    USART_TXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI9_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    USART_TXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI9_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    USART_TXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI9_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI9_TXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SCI_USED_DMAC0 == SCI9_RECEIVE_CONTROL)
    &Driver_DMAC0,
    USART_RXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC1 == SCI9_RECEIVE_CONTROL)
    &Driver_DMAC1,
    USART_RXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC2 == SCI9_RECEIVE_CONTROL)
    &Driver_DMAC2,
    USART_RXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DMAC3 == SCI9_RECEIVE_CONTROL)
    &Driver_DMAC3,
    USART_RXI9_DMAC_SOURCE_ID,
#elif (SCI_USED_DTC == SCI9_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SCI9_RXI, // @suppress("Cast comment")
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#if (SCI_USED_DTC == SCI9_TRANSMIT_CONTROL)
    &gs_sci9_tx_dtc_info,
#else
    NULL,
#endif
#if (SCI_USED_DTC == SCI9_RECEIVE_CONTROL)
    &gs_sci9_rx_dtc_info,
#else
    NULL,
#endif
#endif
    sci9_txi_interrupt,
    sci9_rxi_interrupt,
    sci9_eri_interrupt
};


/****************************************************************************************************************//**
 * @brief USART initialize method for user applications to initialize the SCI channel9. User applications should call
 * this method through the Driver Instance Driver_USART9.Initialize().
 *
 * @param[in] cb_event   Callback event for USART.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_Initialize */
static int32_t USART9_Initialize(ARM_USART_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_USART_Initialize(cb_event,  &gs_sci9_resources));
}/* End of function USART9_Initialize() */

/****************************************************************************************************************//**
 * @brief USART uninitialize method for user applications to initialize the SCI channel9. User applications should call
 * this method through the Driver Instance Driver_USART9.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_Uninitialize */
static int32_t USART9_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_Uninitialize(&gs_sci9_resources));
}/* End of function USART9_Uninitialize() */

/****************************************************************************************************************//**
 * @brief USART power control method for user applications to control the power of SCI channel9. User applications
 * should call this method through the Driver Instance Driver_USART9.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_PowerControl */
static int32_t USART9_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_USART_PowerControl(state, &gs_sci9_resources));
}/* End of function USART9_PowerControl() */

/****************************************************************************************************************//**
 * @brief USART data transmission method for user applications to send data over SCI channel9. User applications should
 * call this method through the Driver Instance Driver_USART9.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_Send */
static int32_t USART9_Send(void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Send(p_data, num , &gs_sci9_resources));
}/* End of function USART9_Send() */

/****************************************************************************************************************//**
 * @brief USART data reception method for user applications to receive data over SCI channel9. User applications should
 * call this method through the Driver Instance Driver_USART9.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_Receive */
static int32_t USART9_Receive(void * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Receive(p_data, num, &gs_sci9_resources));
}/* End of function USART9_Receive() */

/****************************************************************************************************************//**
 * @brief USART data tranmission method for user applications to send and receive data over SCI channel9 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_USART9.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_Transfer */
static int32_t USART9_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_USART_Transfer(p_data_out, p_data_in, num, &gs_sci9_resources));
}/* End of function USART9_Transfer() */

/****************************************************************************************************************//**
 * @brief USART data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SCI channel9. User applications should call this method through the Driver Instance
 * Driver_USART9.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_GetTxCount */
static uint32_t USART9_GetTxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetTxCount(&gs_sci9_resources));
}/* End of function USART9_GetTxCount() */

/***************************************************************************************************************//**
 * @brief USART data reception count getter method for user applications to get the transmission counter value for the
 * RX channel on SCI channel9. User applications should call this method through the Driver
 * Instance Driver_USART9.GetRxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_GetRxCount */
static uint32_t USART9_GetRxCount(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetRxCount(&gs_sci9_resources));
}/* End of function USART9_GetRxCount() */

/****************************************************************************************************************//**
 * @brief USART driver control method for user applications to set USART configuration for SCI channel9. User 
 * applications should call this method through the Driver Instance Driver_USART9.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_Control */
static int32_t USART9_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_USART_Control(control, arg, &gs_sci9_resources));
}/* End of function USART9_Control() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the driver status for SCI channel9. User applications
 * should call this method through the Driver Instance Driver_USART9.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_GetStatus */
static ARM_USART_STATUS USART9_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetStatus(&gs_sci9_resources));
}/* End of function USART9_GetStatus() */

/****************************************************************************************************************//**
 * @brief USART status getter method for user applications to get the modem status for SCI channel9. User applications
 * should call this method through the Driver Instance Driver_USART9.GetModemStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_GetModemStatus */
static ARM_USART_MODEM_STATUS USART9_GetModemStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_USART_GetModemStatus(&gs_sci9_resources));
}/* End of function USART9_GetModemStatus() */

/****************************************************************************************************************//**
 * @brief USART status setter method for user applications to set a modem configuration for SCI channel9. User 
 * applications should call this method through the Driver Instance Driver_USART9.SetModemControl().
 * @param[in]  control  ARM_USART_RTS_CLEAR, ARM_USART_RTS_SET, ARM_USART_DTR_SET or ARM_USART_DTR_CLEAR
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: USART9_SetModemControl */
static int32_t USART9_SetModemControl(ARM_USART_MODEM_CONTROL control) // @suppress("Non-API function naming")
{
    return (ARM_USART_SetModemControl(control, &gs_sci9_resources));
}/* End of function USART9_SetModemControl() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI9
 *******************************************************************************************************************/
/* Function Name: sci9_txi_interrupt */
static void sci9_txi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci9_resources.tx_dma_drv) // @suppress("Cast comment")
    {
        txi_handler(&gs_sci9_resources);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == gs_sci9_resources.tx_dma_drv)
    {
        txi_dtc_handler(&gs_sci9_resources);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        txi_dmac_handler(&gs_sci9_resources);
    }
#endif
}/* End of function sci9_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI9
 *******************************************************************************************************************/
/* Function Name: sci9_rxi_interrupt */
static void sci9_rxi_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL == gs_sci9_resources.rx_dma_drv) // @suppress("Cast comment")
    {
        rxi_handler(&gs_sci9_resources);
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        rxi_dmac_handler(&gs_sci9_resources);
    }
#endif
}/* End of function sci9_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SCI
 *******************************************************************************************************************/
/* Function Name: sci9_eri_interrupt */
static void sci9_eri_interrupt(void) // @suppress("ISR declaration")
{
    eri_handler(&gs_sci9_resources);
}/* End of function sci9_eri_interrupt() */
#endif /* (0 != R_SCI9_ENABLE) */

/** USART Resources define end */

/****************************************************************************************************************//**
 * @brief The implementation of CMSIS-Driver USART Get Version API for the SCI peripheral on RE Family.
 * @retval Driver version number
 *******************************************************************************************************************/
/* Function Name: ARM_USART_GetVersion */
ARM_DRIVER_VERSION ARM_USART_GetVersion(void) // @suppress("Function declaration")  @suppress("Non-API function naming")
{
  return (gs_driver_version);
}/* End of function ARM_USART_GetVersion() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver USART Get Capabilities API for the SCI peripheral on RE Family.
 * @retval      ARM_USART_CAPABILITIES @ref ARM_USART_CAPABILITIES
 *******************************************************************************************************************/
/* Function Name: ARM_USART_GetCapabilities */
ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void) // @suppress("Function declaration")  @suppress("Non-API function naming")
{
  return (gs_driver_capabilities);
}/* End of function ARM_USART_GetCapabilities() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver USART Initialization API for the SCI peripheral on RE Family.
 * @param[in]        cb_event  Pointer to \ref ARM_USART_SignalEvent
 * @param[in,out]    p_usart     Pointer to USART resource data
 * @retval      ARM_DRIVER_OK  Initialization was successful.
 * @note        If TXI is not used and (RXI is not used or ERI is not used) Initialize fails and 
 *              ARM_DRIVER_ERROR_SYSTEM is returned
 *******************************************************************************************************************/
/* Function Name: ARM_USART_Initialize */
int32_t ARM_USART_Initialize(ARM_USART_SignalEvent_t cb_event, st_usart_resources_t * const p_usart) // @suppress("Function declaration")  @suppress("Non-API function naming")
{
    int32_t result = ARM_DRIVER_OK;
    int16_t init_flag = USART_FLAG_INITIALIZED;
    
    if (0 != (p_usart->info->flags & USART_FLAG_INITIALIZED))
    {
        /* Target usart is already initialized */
        return (ARM_DRIVER_OK);
    }
    
    result = check_tx_available(&init_flag, p_usart);
    if (ARM_DRIVER_OK == result)
    {
        result = check_rx_available(&init_flag, p_usart);
    }

    if (USART_FLAG_INITIALIZED == init_flag)
    {
        result = ARM_DRIVER_ERROR;
    }
    
    /* Lock USART resource */
    if (ARM_DRIVER_OK == result)
    {
        if (0 != R_SYS_ResourceLock(p_usart->lock_id))
        {
            result = ARM_DRIVER_ERROR;
        }
    }
    
    if (ARM_DRIVER_OK == result)
    {
    
        /* Set callback function */
        p_usart->info->cb_event  = cb_event;
        
        /* Clear RX status */
        p_usart->info->rx_status.busy          = 0;
        p_usart->info->rx_status.overflow      = 0;
        p_usart->info->rx_status.framing_error = 0;
        p_usart->info->rx_status.parity_error  = 0;
        p_usart->info->tx_status.rx_cnt        = 0;
        
        /* Clear TX status */
        p_usart->info->tx_status.send_active   = 0;
        p_usart->info->tx_status.tx_def_val    = 0xFF;
        p_usart->info->tx_status.tx_cnt        = 0;
        
        /* Clear USART info */
        p_usart->info->flow_mode = USART_FLOW_CTS_DISABLE;
        
        p_usart->info->flags = init_flag;
        
        /* Clear interrupt request */
        sci_clear_irq_req(p_usart->txi_irq);
        sci_clear_irq_req(p_usart->rxi_irq);
        sci_clear_irq_req(p_usart->eri_irq);
    }
#if (0 != USART_PRV_USED_DMAC_DTC_DRV)
    else
    {
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
        /* Close tx dma driver */
        sci_close_dma(p_usart->tx_dma_drv, p_usart->tx_dma_source);
#endif

#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
        /* Close rx dma driver */
        sci_close_dma(p_usart->rx_dma_drv, p_usart->rx_dma_source);
#endif
    }
#endif

    return (result);
}/* End of function ARM_USART_Initialize() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver USART Uninitialization API for the SCI peripheral on RE Family.
 * @param[in]   p_usart        Pointer to USART resource data
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
 *******************************************************************************************************************/
/* Function Name: ARM_USART_Uninitialize */
int32_t ARM_USART_Uninitialize(st_usart_resources_t * const p_usart) // @suppress("Function declaration")  @suppress("Non-API function naming") @suppress("Function length")
{
    int32_t result = ARM_DRIVER_OK;

    if (0 == (p_usart->info->flags & USART_FLAG_INITIALIZED))
    {
        /* Target usart is not initialized */
        return (ARM_DRIVER_OK);
    }

    if (0 == (p_usart->info->flags & USART_FLAG_POWERED))
    {
        /* Release module stop */
        (void)R_LPM_ModuleStart(p_usart->mstp_id);
    }
    
    /* Disable interrupt request */
    sci_disable_irq(p_usart->txi_irq);
    sci_disable_irq(p_usart->rxi_irq);
    sci_disable_irq(p_usart->eri_irq);

    /* Restore the SCI related registers to their values after resetting */
    /* SCR   - Serial Control Register for Non-Smart Card Interface Mode
       b7    - TIE  - Transmit Interrupt Enable - A TXI interrupt request is disabled
       b6    - RIE  - Receive Interrupt Enable - RXI and ERI interrupt requests are disabled
       b5    - TE   - Transmit Enable - Serial transmission is disabled
       b4    - RE   - Receive Enable - Serial transmission is disabled
       b3    - MPIE - Multi-Processor Interrupt Enable - Normal reception
       b2    - TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
       b1:b0 - CKE[1:0] - Clock Enable - On-chip baud rate generator
                                         The SCKn pin functions as I/O port. */
    p_usart->reg->SCR   = 0x00;

    /* SMR   - Serial Mode Register for Non-Smart Card Interface Mode
       b7    - CM  - Communications Mode - Asynchronous mode or simple I2C mode
       b6    - CHR - Character Length - Transmit/receive in 9-bit data length
       b5    - PE  - Parity Enable - When transmitting : Parity bit addition is not performed
                                     When receiving    : Parity bit checking is not performed
       b4    - PM  - Parity Mode - Selects even parity(Valid only when the PE bit is 1)
       b3    - STOP - Stop Bit Length - 1 stop bit (Valid only in asynchronous mode)
       b2    - MP - Multi-Processor Mode - Multi-processor communications function is disabled
                                          (Valid only in asynchronous mode)
       b1:b0 - Clock Select - PCLK (n = 0) */
    p_usart->reg->SMR   = 0x00;

    /* SSR - Serial Status Register for Non-Smart Card Interface Mode and non-FIFO Mode
       b7  - TDRE - Transmit Data Empty Flag
       b6  - RDRF - Receive Data Full Flag - No valid data is held in the RDR register
       b5  - ORER - Overrun Error Flag - No overrun error occurred
       b4  - FER - Framing Error Flag - No framing error occurred
       b3  - PER - Parity Error Flag
       b2  - TEND - Transmit End Flag - A character is being transmitted.
       b1  - MPB - Multi-Processor - Data transmission cycles
       b0  - MPBT - Multi-Processor Bit Transfer - Data transmission cycles */
    p_usart->reg->SSR  &= 0x84;

    /* SCMR  - Smart Card Mode Register
       b7    - BCP2 - Base Clock Pulse 2
       b6:b5 - Reserved - This bit is read as 1. The write value should be 1.
       b4    - CHR1 - Character Length 1 - Transmit/receive in 7-bit data length.
       b3    - SDIR - Transmitted/Received Data Transfer Direction - Transfer with LSB first
       b2    - SINV - Transmitted/Received Data Invert
                    - TDR contents are transmitted as they are. Receive data is stored as it is in RDR. 
       b1    - Reserved - This bit is read as 1. The write value should be 1.
       b0    - SMIF - Smart Card Interface Mode Select - Non-smart card interface mode */
    p_usart->reg->SCMR  = 0xF2;

    /* BRR - Bit Rate Register - initial value */
    p_usart->reg->BRR   = 0xFF;

    /* SEMR  - Serial Extended Mode Register
       b7    - RXDESEL - Asynchronous Start Bit Edge Detection Select
                       - The low level on the RXDn pin is detected as the start bit.
       b6    - BGDM - Baud Rate Generator Double-Speed Mode Select
                    - Baud rate generator outputs the clock with normal frequency. 
       b5    - NFEN - Digital Noise Filter Function Enable
                    - Noise cancellation function for the RXDn input signal is disabled.
       b4    - ABCS - Asynchronous Mode Base Clock Select
                    - Selects 16 base clock cycles for 1-bit period.
       b3    - ABCSE - Asynchronous Mode Extended Base Clock Select1
                     - The number of clocks in one bit period is determined 
                       by the combination of BGDM and ABCS in the SEMR register.
       b2    - BRME - Bit Rate Modulation Enable - Bit rate modulation function is disabled.
       b1:b0 - Reserved - This bit is read as 0. The write value should be 0. */
    p_usart->reg->SEMR  = 0x00;

    /* MDDR - Modulation Duty Register */
    p_usart->reg->MDDR  = 0xFF;

    /* SNFR  - Noise Filter Setting Register
       b7:b3 - Reserved - These bits are read as 0. The write value should be 0.
       b2:b0 - NFCS[2:0] - Noise Filter Clock Select - The clock signal divided by 1 is used with the noise filter. */
    p_usart->reg->SNFR  = 0x00;

    /* SIMR1 - I2C Mode Register 1
       b7:b3 - IICDL - SSDA Output Delay Select - No output delay
       b2:b1 - Reserved - These bits are read as 0. The write value should be 0.
       b0    - IICM - Simple I2C Mode Select - Asynchronous mode, Multi-processor mode,
                                               Clock synchronous mode */
    p_usart->reg->SIMR1 = 0x00;

    /* SIMR2 - I2C Mode Register 2
       b7:b6 - Reserved - These bits are read as 0. The write value should be 0.
       b5    - IICACKT - ACK Transmission Data - ACK transmission
       b4:b2 - Reserved - These bits are read as 0. The write value should be 0.
       b1    - IICCSC - Clock Synchronization - No synchronization with the clock signal
       b0    - IICINTM - I2C Interrupt Mode Select 0: Use ACK/NACK interrupts. */
    p_usart->reg->SIMR2 = 0x00;

    /* SIMR3 - I2C Mode Register 3 
       b7:b6 - IICSCLS[1:0] - SSCL Output Select - Serial clock output
       b5:b4 - IICSDAS[1:0] - SSDA Output Select - Serial data output
       b3    - IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
                       - There are no requests for generating conditions or a condition is being generated.
       b2    - IICSTPREQ - Stop Condition Generation - A stop condition is not generated.
       b1    - IICRSTAREQ - Restart Condition Generation - A restart condition is not generated.
       b0    - IICSTAREQ - Start Condition Generation - A start condition is not generated. */
    p_usart->reg->SIMR3 = 0x00;

    /* SPMR - SPI Mode Register
       b7   - CKPH - Clock Phase Select - Clock is not delayed
       b6   - CKPOL - Clock Polarity Select - Clock polarity is not inverted.
       b5   - Reserved  - This bit is read as 0. The write value should be 0.
       b4   - MFF - Mode Fault Flag - No mode fault error
       b3   - Reserved - This bit is read as 0. The write value should be 0.
       b2   - MSS - Master Slave Select - Transmission is through
                                          the TXDn pin and reception is through the RXDn pin (master mode).
       b1   - CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled).
       b0   - SSE - SSn Pin Function Enable - SSn pin function is disabled */
    p_usart->reg->SPMR  = 0x00;

    /* CDR - Compear Match Data Register */
    p_usart->reg->CDR  = 0x0000;

    /* DCCR - Data Compear Match Control Register */
    p_usart->reg->DCCR  = 0x40;

    /* SPTR - Serial Port Register */
    p_usart->reg->SPTR  = 0x03;
    
    /* Clear interrupt request */
    sci_clear_irq_req(p_usart->txi_irq);
    sci_clear_irq_req(p_usart->rxi_irq);
    sci_clear_irq_req(p_usart->eri_irq);

    p_usart->pin_clr();
    (void) R_LPM_ModuleStop(p_usart->mstp_id); // @suppress("Cast comment")
    p_usart->info->flags = 0;
    p_usart->info->rx_status.busy          = 0;
    p_usart->info->tx_status.send_active   = 0;
    
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
        /* Close tx dma driver */
        sci_close_dma(p_usart->tx_dma_drv, p_usart->tx_dma_source);
#endif

#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
        /* Close rx dma driver */
        sci_close_dma(p_usart->rx_dma_drv, p_usart->rx_dma_source);
#endif
    
    /* Unlock USART resource */
    R_SYS_ResourceUnlock(p_usart->lock_id);

    return (result);
}/* End of function ARM_USART_Uninitialize() */

/***************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver USART Power Control API for the SCI peripheral on RE Family.
 * @param[in]   state  ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock)
 * @param[in]   p_usart  Pointer to USART resource data
 * @retval      ARM_DRIVER_OK                Power control operation was successful.
 * @retval      ARM_DRIVER_ERROR             Specified SCI mode is not supported.
 * @retval      ARM_DRIVER_ERROR_UNSUPPORTED Power mode specified is not supported.
 * @note Power state ARM_POWER_LOW is not supported.
 *******************************************************************************************************************/
/* Function Name: ARM_USART_PowerControl */
int32_t ARM_USART_PowerControl(ARM_POWER_STATE state, st_usart_resources_t * const p_usart) // @suppress("Function declaration")  @suppress("Non-API function naming") @suppress("Function length")
{
    int32_t result = ARM_DRIVER_OK;
    if (0 == (p_usart->info->flags & USART_FLAG_INITIALIZED)) 
    {
        return (ARM_DRIVER_ERROR);
    }

    switch (state)
    {
        case ARM_POWER_OFF:
        {
            /* Disable when transmission / reception is enabled */
            if ((0U != (p_usart->info->flags & USART_FLAG_TX_ENABLED)) ||
                 (0U != (p_usart->info->flags & USART_FLAG_RX_ENABLED)))
            {
                sci_tx_rx_disable(p_usart);
            }

            /* Set to module stop mode */
            (void) R_LPM_ModuleStop(p_usart->mstp_id);
            p_usart->info->flags &= (~USART_FLAG_POWERED);
        }
        break;

        case ARM_POWER_FULL:
        {
            if (0U != (p_usart->info->flags & USART_FLAG_POWERED))
            {
                break;
            }
            
            /* Release module stop */
            if ( 0 != R_LPM_ModuleStart(p_usart->mstp_id))
            {
                result = ARM_DRIVER_ERROR;
                break;
            }
            p_usart->info->flags |= USART_FLAG_POWERED;
        }
        break;

        /* The default case is intentionally combined.  */
        case ARM_POWER_LOW:
        default:
        {
            result = ARM_DRIVER_ERROR_UNSUPPORTED;
        }
        break;
    }
    return (result);
}/* End of function ARM_USART_PowerControl() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver USART Data Transmission API for the SCI peripheral on RE Family.
 * @param[in]        p_data      Pointer to output data
 * @param[in]        num         Number of data to transmit
 * @param[in,out]    p_usart     Pointer to USART resource data
 * @retval      ARM_DRIVER_OK    Data transmission was successful.
 * @retval      ARM_DRIVER_ERROR Specified SCI mode is not supported.
 * @retval      ARM_DRIVER_ERROR_PARAMETER An invalid parameter is specified.
 * @note        Please note the following when DMAC or DTC control is selected.@n
 *              If the transmit buffer is not empty (SSR.TDRE = 0), wait until the transmit buffer becomes empty 
 *              until the timeout set value(@ref SCI_CHECK_TDRE_TIMEOUT ).If timeout occurs, ARM_DRIVER_ERROR_BUSY will be returned.
 *******************************************************************************************************************/
/* Function Name: ARM_USART_Send */
int32_t ARM_USART_Send(void const * const p_data, uint32_t num, st_usart_resources_t * const p_usart) // @suppress("Function declaration")  @suppress("Non-API function naming") @suppress("Function length")
{
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    st_dma_transfer_data_cfg_t dma_config;
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    uint16_t timeout;
#endif
    
    if ((NULL == p_data) || (0 == num))  // @suppress("Cast comment")
    {
        /* Invalid parameters */
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    if (0 == (p_usart->info->flags & USART_FLAG_TX_ENABLED))
    {
        return (ARM_DRIVER_ERROR);
    }
    
    if (1 == p_usart->info->tx_status.send_active)
    {
        /* Send is not completed yet */
        return (ARM_DRIVER_ERROR_BUSY);
    }

    if ((USART_MODE_ASYNC != p_usart->info->mode) &&
        (1 == p_usart->info->rx_status.busy))
    {
        /* Unable to transmit during reception except for asynchronous mode */
        return (ARM_DRIVER_ERROR_BUSY);
    }

    /* Save transmit buffer info */
    p_usart->info->tx_status.tx_buf = (void*)p_data;
    p_usart->info->tx_status.tx_num = num;
    p_usart->info->tx_status.tx_cnt = 0;
    p_usart->info->tx_status.sync_mode = USART_SYNC_TX_MODE;
    
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    /* Initialize DTC/DMAC Config Data */
    dma_config_init(&dma_config);
    dma_config.src_addr = (uint32_t)p_data;  // @suppress("Cast comment")
    if (USART_SIZE_9 == p_usart->info->data_size)
    {
        /* If the bit size is 9 bits, increment the address by 2 */
        dma_config.src_addr += 2;
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_WORD | DMA_SRC_INCR | DMA_DEST_FIXED;
        dma_config.dest_addr = (uint32_t)(&(p_usart->reg->TDRHL));  // @suppress("Cast comment")
    }
    else
    {
        dma_config.src_addr += 1;
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_BYTE | DMA_SRC_INCR | DMA_DEST_FIXED;
        dma_config.dest_addr = (uint32_t)(&(p_usart->reg->TDR));  // @suppress("Cast comment")
    }
    dma_config.transfer_count = num - 1;
#endif
    
    if (NULL == p_usart->tx_dma_drv)  // @suppress("Cast comment")
    {
        /* When sending by interrupt */
        /* Enable txi interrupt for NVIC */
        p_usart->info->tx_status.send_active   = 1;
        R_NVIC_EnableIRQ(p_usart->txi_irq);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == p_usart->tx_dma_drv)
    {
        /* Check TDRE bit */
        timeout = SCI_CHECK_TDRE_TIMEOUT;
        while (0 == p_usart->reg->SSR_b.TDRE)
        {
            if (0 == (timeout--))
            {
                /* TDR register is not empty */
                return (ARM_DRIVER_ERROR_BUSY);
            }
        }
        /* When sending by DTC */
        /* DTC transfer setting */
        if (1 != num)
        {
            dma_config.p_transfer_data = p_usart->tx_dtc_info;
            if (p_usart->tx_dma_drv->Create (p_usart->tx_dma_source, &dma_config) != DMA_OK)
            {
                p_usart->info->tx_status.send_active   = 0;
                return (ARM_DRIVER_ERROR);
            }
    
            /* DTC Start */
            p_usart->tx_dma_drv->Control(DMA_CMD_START, NULL);
        }
        p_usart->info->tx_status.send_active   = 1;

        /* Enable txi interrupt for NVIC */
        R_NVIC_EnableIRQ(p_usart->txi_irq);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        /* Check TDRE bit */
        timeout = SCI_CHECK_TDRE_TIMEOUT;
        while (0 == p_usart->reg->SSR_b.TDRE)
        {
            if (0 == (timeout--))
            {
                /* TDR register is not empty */
                return (ARM_DRIVER_ERROR_BUSY);
            }
        }

        /* When sending by DMAC */
        /* DMAC transfer setting */
        if (1 != num)
        {
            if (p_usart->tx_dma_drv->Create (p_usart->tx_dma_source, &dma_config) != DMA_OK)
            {
                p_usart->info->tx_status.send_active   = 0;
                return (ARM_DRIVER_ERROR);
            }
            
            (void)p_usart->tx_dma_drv->InterruptEnable(DMA_INT_COMPLETE, p_usart->txi_callback);  // @suppress("Cast comment")
            
            /* DMAC Start */
            (void)p_usart->tx_dma_drv->Control(DMA_CMD_START, NULL);
        }
        
        p_usart->info->tx_status.send_active   = 1;

        /* Write the first data here */
        if (USART_SIZE_9 == p_usart->info->data_size)
        {
            /* It is unnecessary to judge at the 9-bit time because the start-stop synchronization is confirmed. */
            /* TDRHL - Transmit Data Register HL */
            p_usart->reg->TDRHL = ((uint16_t*)p_data)[0];
        }
        else
        {
            /* TDR - Transmit Data Register */
            p_usart->reg->TDR = ((uint8_t*)p_data)[0];
        }
        p_usart->info->tx_status.tx_cnt++;
        if (1 == num)
        {
            p_usart->info->tx_status.send_active   = 0;
            if (NULL != p_usart->info->cb_event)  // @suppress("Cast comment")
            {
                /* Execution of callback function */
                p_usart->info->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
            }
        }
    }
#endif
    return (ARM_DRIVER_OK);
    
}/* End of function ARM_USART_Send() */

/****************************************************************************************************************//**
 *  @brief       The implementation of CMSIS-Driver USART Data Reception API for the SCI peripheral on RE Family.
 *  @param[in]        p_data         Pointer to input data
 *  @param[in]        num          Number of data to receive
 *  @param[in,out]    p_usart        Pointer to USART resource data
 *  @retval      ARM_DRIVER_OK     Data reception was successful.
 *  @retval      ARM_DRIVER_ERROR  Specified SCI mode is not supported.
 *  @retval      ARM_DRIVER_ERROR_PARAMETER An illegal parameter is specified.
 *  @note        Please note the following when DMAC or DTC control is selected.@n
 *               If the transmit buffer is not empty (SSR.TDRE = 0), wait until the transmit buffer becomes empty 
 *               until the timeout set value(@ref SCI_CHECK_TDRE_TIMEOUT ).If timeout occurs, ARM_DRIVER_ERROR_BUSY will be returned.
 *******************************************************************************************************************/
/* Function Name: ARM_USART_Receive */
int32_t ARM_USART_Receive(void * const p_data, uint32_t num, st_usart_resources_t * const p_usart) // @suppress("Function declaration")  @suppress("Non-API function naming") @suppress("Function length")
{
#if (0 != USART_PRV_USED_DMAC_DTC_DRV)
    st_dma_transfer_data_cfg_t dma_config;
#endif
    volatile uint16_t dummy;
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    uint16_t timeout;
#endif

    if ((NULL == p_data) || (0 == num))   // @suppress("Cast comment")
    {
        /* Invalid parameters */
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    if (0 == (p_usart->info->flags & USART_FLAG_RX_ENABLED))
    {
        return (ARM_DRIVER_ERROR);
    }
    
     if (1 == p_usart->info->rx_status.busy)
    {
        /* Receive is not completed yet */
        return (ARM_DRIVER_ERROR_BUSY);
    }

    if ((USART_MODE_ASYNC != p_usart->info->mode) &&
        (1 == p_usart->info->tx_status.send_active))
    {
        /* Can not receive during transmission except for asynchronous mode */
        return (ARM_DRIVER_ERROR_BUSY);
    }
    
    /* Receive information initialization */
    p_usart->info->tx_status.rx_buf = p_data;
    p_usart->info->tx_status.rx_num = num;
    p_usart->info->tx_status.rx_cnt = 0;
    
    /* Clear RX status */
    p_usart->info->rx_status.overflow      = 0;
    p_usart->info->rx_status.framing_error = 0;
    p_usart->info->rx_status.parity_error  = 0;

#if (0 != USART_PRV_USED_DMAC_DTC_DRV)
    /* Initialize DTC/DMAC Config Data */
    dma_config_init(&dma_config);
#endif

#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    dma_config.dest_addr = (uint32_t)p_data;  // @suppress("Cast comment")
    if (USART_SIZE_9 == p_usart->info->data_size)
    {
        /* If the bit size is 9 bits, increment the address by 2 */
        dma_config.src_addr = (uint32_t)(&(p_usart->reg->RDRHL));
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_WORD | DMA_SRC_FIXED | DMA_DEST_INCR;
    }
    else
    {
        dma_config.src_addr = (uint32_t)(&(p_usart->reg->RDR));  // @suppress("Cast comment")
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_BYTE | DMA_SRC_FIXED | DMA_DEST_INCR;
    }
    dma_config.transfer_count = num;
#endif

    if (NULL == p_usart->rx_dma_drv)  // @suppress("Cast comment")
    {
        /* When receiving  by interrupt */
        p_usart->info->tx_status.sync_mode = USART_SYNC_RX_MODE;
        p_usart->info->rx_status.busy = 1;
    }
#if (0 != USART_PRV_USED_RX_DTC_DRV)
    else if ((&Driver_DTC) == p_usart->rx_dma_drv)
    {
        /* When receiving by DTC */
        /* DTC transfer setting */
        dma_config.p_transfer_data = p_usart->rx_dtc_info;
        if (p_usart->rx_dma_drv->Create (p_usart->rx_dma_source, &dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }
    
        p_usart->info->tx_status.sync_mode = USART_SYNC_RX_MODE;
        p_usart->info->rx_status.busy = 1;

        /* DTC Start */
        p_usart->rx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")
    }
#endif
#if (0 != USART_PRV_USED_RX_DMAC_DRV)
    else
    {
        /* When receiving by DMAC */
        /* DMAC transfer setting */
        if (p_usart->rx_dma_drv->Create (p_usart->rx_dma_source, &dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }
        sci_disable_irq(p_usart->rxi_irq);
        (void)R_SYS_IrqEventLinkSet(p_usart->rxi_irq, 0x00000000, p_usart->rxi_callback);  // @suppress("Cast comment")

        sci_clear_irq_req(p_usart->rxi_irq);
        (void)p_usart->rx_dma_drv->InterruptEnable(DMA_INT_COMPLETE, p_usart->rxi_callback);  // @suppress("Cast comment")

        p_usart->info->tx_status.sync_mode = USART_SYNC_RX_MODE;
        p_usart->info->rx_status.busy = 1;

        /* DMAC Start */
        (void)p_usart->rx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")

        /* Dummy read */
        dummy = p_usart->reg->RDRHL;
    }
#endif

    if ((USART_MODE_SYNC == p_usart->info->mode) && 
        (0 != (p_usart->info->flags & USART_FLAG_TX_ENABLED)))
    {
        p_usart->info->tx_status.tx_num = num;
        p_usart->info->tx_status.tx_cnt = 0;

#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
        dma_config.src_addr = (uint32_t)(&(p_usart->info->tx_status.tx_def_val));  // @suppress("Cast comment")
        dma_config.dest_addr = (uint32_t)(&(p_usart->reg->TDR));  // @suppress("Cast comment")
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_BYTE | DMA_SRC_FIXED | DMA_DEST_FIXED;
        dma_config.transfer_count = num - 1;
#endif

        if (NULL == p_usart->tx_dma_drv)  // @suppress("Cast comment")
        {
            /* When sending by interrupt */
            /* Enable txi interrupt for NVIC */
            R_NVIC_EnableIRQ(p_usart->txi_irq);
        }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
        else if ((&Driver_DTC) == p_usart->tx_dma_drv)
        {
            /* When sending by DTC */
            /* DTC transfer setting */
            /* Check TDRE bit */
            timeout = SCI_CHECK_TDRE_TIMEOUT;
            while (0 == p_usart->reg->SSR_b.TDRE)
            {
                if (0 == (timeout--))
                {
                    p_usart->info->rx_status.busy = 0;
                    sci_receive_stop(p_usart);

                    /* TDR register is not empty */
                    return (ARM_DRIVER_ERROR_BUSY);
                }
            }
            if (1 != num)
            {
                dma_config.p_transfer_data = p_usart->tx_dtc_info;
                if (p_usart->tx_dma_drv->Create (p_usart->tx_dma_source, &dma_config) != DMA_OK) // @suppress("If statement nesting")
                {
                    p_usart->info->rx_status.busy = 0;
                    sci_receive_stop(p_usart);
                    return (ARM_DRIVER_ERROR);
                }

                /* DTC Start */
                p_usart->tx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")
            }
            p_usart->info->tx_status.send_active   = 1;

            /* Enable txi interrupt for NVIC */
            R_NVIC_EnableIRQ(p_usart->txi_irq);
        }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
        else
        {
            /* When sending by DMAC */
            /* DMAC transfer setting */
            /* Check TDRE bit */
            timeout = SCI_CHECK_TDRE_TIMEOUT;
            while (0 == p_usart->reg->SSR_b.TDRE)
            {
                if (0 == (timeout--))
                {
                    p_usart->info->rx_status.busy = 0;
                    sci_receive_stop(p_usart);

                    /* TDR register is not empty */
                    return (ARM_DRIVER_ERROR_BUSY);
                }
            }
            if (1 != num)
            {
                if (p_usart->tx_dma_drv->Create (p_usart->tx_dma_source, &dma_config) != DMA_OK) // @suppress("If statement nesting")
                {
                    p_usart->info->rx_status.busy = 0;
                    sci_receive_stop(p_usart);
                    return (ARM_DRIVER_ERROR);
                }
    
                (void)p_usart->tx_dma_drv->InterruptEnable(DMA_INT_COMPLETE, p_usart->txi_callback);  // @suppress("Cast comment")
                (void)p_usart->tx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")
                
                /* DMAC Start */
                p_usart->info->tx_status.send_active   = 1;
            
            }

            /* TDR - Transmit Data Register */
            p_usart->reg->TDR = ((uint8_t)(p_usart->info->tx_status.tx_def_val));
        }
#endif
    }
    
    return (ARM_DRIVER_OK);
}/* End of function ARM_USART_Receive() */

/****************************************************************************************************************//**
 *  @brief       The implementation of CMSIS-Driver USART Data Transfer API for the SCI peripheral on RE Family.
 *               This function supports clock synchronous mode.
 *  @param[in]        p_data_out     Pointer to output data
 *  @param[in]        p_data_in      Pointer to iunput data
 *  @param[in]        num          Number of data to transfer
 *  @param[in,out]    p_usart        Pointer to USART resource data
 *  @retval      ARM_DRIVER_OK     Data transfer was successful.
 *  @retval      ARM_DRIVER_ERROR  Specified SCI mode is not supported.
 *  @retval      ARM_DRIVER_ERROR_PARAMETER Illegal parameter is specified.
 *  @note        Please note the following when DMAC or DTC control is selected.@n
 *               If the transmit buffer is not empty (SSR.TDRE = 0), wait until the transmit buffer becomes empty 
 *               until the timeout set value(@ref SCI_CHECK_TDRE_TIMEOUT ).If timeout occurs, ARM_DRIVER_ERROR_BUSY will be returned.
 *******************************************************************************************************************/
/* Function Name: ARM_USART_Transfer */
int32_t ARM_USART_Transfer(void const * const p_data_out, void * const p_data_in,// @suppress("Function declaration")  @suppress("Non-API function naming") @suppress("Function length")
                           uint32_t num, st_usart_resources_t * const p_usart)
{

#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    st_dma_transfer_data_cfg_t tx_dma_config;
#endif
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    st_dma_transfer_data_cfg_t rx_dma_config;
#endif

    volatile uint16_t dummy;
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    uint16_t timeout;
#endif
    if ((NULL == p_data_out) || (NULL == p_data_in) || (0 == num))   // @suppress("Cast comment")
    {
        /* Invalid parameters */
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    if (USART_MODE_SYNC != p_usart->info->mode)
    {
        /* Enable clock synchronous mode only */
        return (ARM_DRIVER_ERROR);
    }
    
    if ((0 == (p_usart->info->flags & USART_FLAG_TX_ENABLED))
      ||(0 == (p_usart->info->flags & USART_FLAG_RX_ENABLED)))
    {
        /*  */
        return (ARM_DRIVER_ERROR);
    }
    
    if ((1 == p_usart->info->rx_status.busy)
     || (1 == p_usart->info->tx_status.send_active))
    {
        /* Transmit or Receive is not completed yet */
        return (ARM_DRIVER_ERROR_BUSY);
    }
    
    /* Receive information initialization */
    p_usart->info->tx_status.rx_buf = p_data_in;
    p_usart->info->tx_status.rx_num = num;
    p_usart->info->tx_status.rx_cnt = 0;

    /* Save transmit buffer info */
    p_usart->info->tx_status.tx_buf = (void*)p_data_out;
    p_usart->info->tx_status.tx_num = num;
    p_usart->info->tx_status.tx_cnt = 0;
    p_usart->info->tx_status.sync_mode = USART_SYNC_TX_RX_MODE;

    /* Clear RX status */
    p_usart->info->rx_status.overflow      = 0;
    p_usart->info->rx_status.framing_error = 0;
    p_usart->info->rx_status.parity_error  = 0;

#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    /* Initialize DTC/DMAC Config Data */
    dma_config_init(&rx_dma_config);
    rx_dma_config.src_addr = (uint32_t)(&(p_usart->reg->RDR));  // @suppress("Cast comment")
    rx_dma_config.dest_addr = (uint32_t)p_data_in;  // @suppress("Cast comment")
    rx_dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_BYTE | DMA_SRC_FIXED | DMA_DEST_INCR;
    rx_dma_config.transfer_count = num;
#endif

#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    dma_config_init(&tx_dma_config);
    tx_dma_config.src_addr = (uint32_t)p_data_out;  // @suppress("Cast comment")
    tx_dma_config.src_addr += 1;
    tx_dma_config.dest_addr = (uint32_t)(&(p_usart->reg->TDR));  // @suppress("Cast comment")
    tx_dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_BYTE | DMA_SRC_INCR | DMA_DEST_FIXED;
    tx_dma_config.transfer_count = num-1;
#endif
    
    /* Receive setting */
    if (NULL == p_usart->rx_dma_drv)
    {   /* @suppress("Empty compound statement") */ /* @suppress("Block comment") */
        /* When receiving by interrupt */
        /* No processing to be executed */
    }
#if (0 != USART_PRV_USED_RX_DTC_DRV)
    else if ((&Driver_DTC) == p_usart->rx_dma_drv)
    {
        /* When receiving by DTC */
        /* DTC transfer setting */
        rx_dma_config.p_transfer_data = p_usart->rx_dtc_info;
        if (p_usart->rx_dma_drv->Create (p_usart->rx_dma_source, &rx_dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }

        /* DTC Start */
        p_usart->rx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")
    }
#endif
#if (0 != USART_PRV_USED_RX_DMAC_DRV)
    else
    {
        /* When receiving by DMAC */
        /* DMAC transfer setting */
        if (p_usart->rx_dma_drv->Create (p_usart->rx_dma_source, &rx_dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }
        sci_disable_irq(p_usart->rxi_irq);
        sci_clear_irq_req(p_usart->rxi_irq);
        (void)R_SYS_IrqEventLinkSet(p_usart->rxi_irq, 0x00000000, p_usart->rxi_callback);  // @suppress("Cast comment")
        (void)p_usart->rx_dma_drv->InterruptEnable(DMA_INT_COMPLETE, p_usart->rxi_callback);  // @suppress("Cast comment")

        /* DMAC Start */
        (void)p_usart->rx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")

        /* Dummy read */
        dummy = p_usart->reg->RDRHL;
    }
#endif
    
    /* Send setting */
    if (NULL == p_usart->tx_dma_drv)  // @suppress("Cast comment")
    {
        /* When sending by interrupt */
        /* Enable txi interrupt for NVIC */
        p_usart->info->rx_status.busy = 1;
        p_usart->info->tx_status.send_active = 1;
        R_NVIC_EnableIRQ(p_usart->txi_irq);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == p_usart->tx_dma_drv)
    {
        /* When sending by DTC */
        /* DTC transfer setting */
        /* Check TDRE bit */
        timeout = SCI_CHECK_TDRE_TIMEOUT;
        while (0 == p_usart->reg->SSR_b.TDRE)
        {
            if (0 == (timeout--))
            {
                p_usart->info->rx_status.busy = 0;
                sci_receive_stop(p_usart);

                /* TDR register is not empty */
                return (ARM_DRIVER_ERROR_BUSY);
            }
        }
        
        if (1 != num)
        {
            tx_dma_config.p_transfer_data = p_usart->tx_dtc_info;
            if (p_usart->tx_dma_drv->Create (p_usart->tx_dma_source, &tx_dma_config) != DMA_OK)
            {
                sci_receive_stop(p_usart);
                return (ARM_DRIVER_ERROR);
            }
    
            /* DTC Start */
            p_usart->tx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")
        }
        p_usart->info->rx_status.busy = 1;
        p_usart->info->tx_status.send_active = 1;
        
        /* Enable txi interrupt for NVIC */
        R_NVIC_EnableIRQ(p_usart->txi_irq);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        /* When sending by DMAC */
        /* DMAC transfer setting */
        /* Check TDRE bit */
        timeout = SCI_CHECK_TDRE_TIMEOUT;
        while (0 == p_usart->reg->SSR_b.TDRE)
        {
            if (0 == (timeout--))
            {
                p_usart->info->rx_status.busy = 0;
                sci_receive_stop(p_usart);

                /* TDR register is not empty */
                return (ARM_DRIVER_ERROR_BUSY);
            }
        }
        if (1 != num)
        {
            if (p_usart->tx_dma_drv->Create (p_usart->tx_dma_source, &tx_dma_config) != DMA_OK)
            {
                sci_receive_stop(p_usart);
                return (ARM_DRIVER_ERROR);
            }
    
            (void)p_usart->tx_dma_drv->InterruptEnable(DMA_INT_COMPLETE, p_usart->txi_callback);  // @suppress("Cast comment")
    
            /* DMAC Start */
            (void)p_usart->tx_dma_drv->Control(DMA_CMD_START, NULL);  // @suppress("Cast comment")
            p_usart->info->tx_status.send_active = 1;
        }
        p_usart->info->rx_status.busy = 1;

        /* TDR - Transmit Data Register */
        p_usart->reg->TDR = ((uint8_t*)p_data_out)[0];
        p_usart->info->tx_status.tx_cnt++;
    }
#endif
    
    return (ARM_DRIVER_OK);
}/* End of function ARM_USART_Transfer() */

/****************************************************************************************************************//**
* @brief    Get transmitted data count.
* @param[in,out]    p_usart        Pointer to USART resource data
* @retval   number of data items transmitted
********************************************************************************************************************/
/* Function Name: ARM_USART_GetTxCount */
uint32_t ARM_USART_GetTxCount(st_usart_resources_t const * const p_usart) // @suppress("Function declaration") @suppress("Non-API function naming")
{
    uint32_t retval = 0;
    
    if ((NULL == p_usart->tx_dma_drv) || (1 == p_usart->info->tx_status.tx_num))  // @suppress("Cast comment")
    {
        retval = p_usart->info->tx_status.tx_cnt;
    }
#if (0 != USART_PRV_USED_TX_DMAC_DTC_DRV)
    else
    {
        (void)p_usart->tx_dma_drv->GetTransferByte(p_usart->tx_dma_source ,&retval);
        retval += p_usart->info->tx_status.tx_cnt;
    }
#endif
    return (retval);
}/* End of function ARM_USART_GetTxCount() */

/****************************************************************************************************************//**
* @brief    Get received data count.
* @param[in,out]    p_usart        Pointer to USART resource data
* @retval   number of data items received
********************************************************************************************************************/
/* Function Name: ARM_USART_GetRxCount */
uint32_t ARM_USART_GetRxCount(st_usart_resources_t const * const p_usart) // @suppress("Function declaration") @suppress("Non-API function naming")
{
    uint32_t retval = 0;
    
    if (NULL == p_usart->rx_dma_drv)  // @suppress("Cast comment")
    {
        retval = p_usart->info->tx_status.rx_cnt;
    }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        (void)p_usart->rx_dma_drv->GetTransferByte(p_usart->rx_dma_source ,&retval);
    }
#endif
    return (retval);
}/* End of function ARM_USART_GetRxCount() */

/****************************************************************************************************************//**
 * @brief       CMSIS-Driver USART Control API implementation for the SCI peripheral on RE Family.
 * @param[in]        control      Operation command
 * @param[in]        arg          Argument of operation command
 * @param[in,out]    p_usart        Pointer to a USART resource structure
 * @retval      ARM_DRIVER_OK     Specified contol operation was successful.
 * @retval      ARM_DRIVER_ERROR  Specified command is not supported.
 * @retval      ARM_USART_ERROR_MODE Specified SCI mode is not supported.
 * @note        The ARM_USART_CONTROL_SMART_CARD_NACK command is valid only for "1" (enable). 
 *              "0" (disable) setting is invalid.
 *              In the case of clock synchronous mode, if you execute the ARM_USART_ABORT_RECEIVE command, 
 *              transmission will also be interrupted
 *              In the case of smart card mode, When you execute the argument of ARM_USART_CONTROL_TX with 1, 
 *              reception is also permitted. Likewise  When you execute the argument of ARM_USART_CONTROL_TX with 0, 
 *              reception is also prohibited.
 *              In the case of smart card mode ,When ARM_USART_CONTROL_RX is executed with argument 0 in the 
 *              transmission enabled state, ARM_DRIVER_ERROR is returned 
********************************************************************************************************************/
/* Function Name: ARM_USART_Control */
int32_t ARM_USART_Control(uint32_t control, uint32_t arg, st_usart_resources_t const * const p_usart) // @suppress("Function length") @suppress("Function declaration") @suppress("Non-API function naming")
{
    int32_t result = ARM_DRIVER_OK;
    st_sci_reg_set_t sci_regs;
    volatile uint16_t dummy;
    
    if (0U == (p_usart->info->flags & USART_FLAG_POWERED)) 
    {
        /* USART not powered */
        return (ARM_DRIVER_ERROR);
    }
    
    sci_set_regs_clear(&sci_regs);

    switch (control & ARM_USART_CONTROL_Msk)
    {
        case ARM_USART_MODE_ASYNCHRONOUS:
        {
            if ((0U == (p_usart->info->flags & USART_FLAG_CONFIGURED)) && (0 != arg))
            {
                sci_bit_clear_8(&sci_regs.smr, 7);           /* CM = 0(Asynchronous mode) */    
                result = mode_set_asynchronous(control, &sci_regs, arg, p_usart);
                if (ARM_DRIVER_OK == result)
                {
                    p_usart->info->flags |= USART_FLAG_CONFIGURED;
                }
            }
            else
            {
                /* If another setting is already completed, or        */
                /* if the baud rate setting is 0, it returns an error */
                result = ARM_DRIVER_ERROR;
            }
        }
        break;

        case ARM_USART_MODE_SYNCHRONOUS_MASTER:
        {
            if ((0U == (p_usart->info->flags & USART_FLAG_CONFIGURED)) && (0 != arg))
            {
                /* CKE[1:0] - Clock Enable - Internal clock The SCKn pin functions as the clock output pin.  */
                sci_bit_clear_8(&sci_regs.scr, 0);          /* CKE[1:0] = 00b(Clock output) */
                sci_bit_clear_8(&sci_regs.scr, 1);
                sci_bit_set_8(&sci_regs.smr, 7);           /* CM = 1(Clock synchronous mode) */    
                result = mode_set_synchronous(control, &sci_regs, arg, p_usart);
                if (ARM_DRIVER_OK == result)
                {
                    p_usart->info->flags |= USART_FLAG_CONFIGURED;
                }
            }
            else
            {
                /* If another setting is already completed, an error is returned */
                result = ARM_DRIVER_ERROR;
            }
        }
        break;

        case ARM_USART_MODE_SYNCHRONOUS_SLAVE:
        {
            if (0U == (p_usart->info->flags & USART_FLAG_CONFIGURED))
            {
                /* CKE[1:0] - Clock Enable - External clock The SCKn pin functions as the clock input pin.  */
                sci_bit_clear_8(&sci_regs.scr, 0);          /* SKE[1:0] = 10b(Clock input) */
                sci_bit_set_8(&sci_regs.scr, 1);
                sci_bit_set_8(&sci_regs.smr, 7);           /* CM = 1(Clock synchronous mode) */    
                result = mode_set_synchronous(control, &sci_regs, 0, p_usart);
                if (ARM_DRIVER_OK == result)
                {
                    p_usart->info->flags |= USART_FLAG_CONFIGURED;
                }
            }
            else
            {
                /* If another setting is already completed, an error is returned */
                result = ARM_DRIVER_ERROR;
            }
        }
        break;

        case ARM_USART_MODE_SMART_CARD:
        {
            if ((SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_usart->rxi_irq) || // @suppress("Cast comment")
                (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_usart->eri_irq)) // @suppress("Cast comment")
            {
                /* In smart card mode, since receive interrupts are always used,   */
                /* if an incoming interrupt can not be used, an error is returned  */
                result = ARM_DRIVER_ERROR;
                break;
            }

            if ((0U == (p_usart->info->flags & USART_FLAG_CONFIGURED)) && (0 != arg))
            {
                /* SMIF - Smart Card Interface Mode Select - Smart card interface mode */
                sci_bit_set_8(&sci_regs.scmr, 0);           /* SMIF = 1(smartcard interface) */
                sci_bit_clear_8(&sci_regs.smr, 7);          /* CM = 0(Asynchronous mode) */    
                result = mode_set_smartcard(control, &sci_regs, arg, p_usart);
                if (ARM_DRIVER_OK == result)
                {
                    p_usart->info->flags |= USART_FLAG_CONFIGURED;
                }
            }
            else
            {
                /* If another setting is already completed, an error is returned */
                result = ARM_DRIVER_ERROR;
            }
        }
        break;
        
        /* These 2 case is intentionally combined.*/
        case ARM_USART_MODE_SINGLE_WIRE:
        case ARM_USART_MODE_IRDA:
        {
            result = ARM_USART_ERROR_MODE;
        }
        break;
        
        /* Setting default transmission data */
        case ARM_USART_SET_DEFAULT_TX_VALUE:
        {
            /* tx_def_val can be up to 8 bits (because clock synchronization is used only).  */
            /* After masking with 0xFF, cast it with uint16_t */
            p_usart->info->tx_status.tx_def_val = (uint16_t)(arg & 0x0FF);
        }
        break;
        
        case ARM_USART_SET_SMART_CARD_CLOCK:
        {
            /* Valid only when smart card mode and not being sent or received */
            if ((0U != (p_usart->info->flags & USART_FLAG_CONFIGURED)) &&       
                (USART_MODE_SMC == p_usart->info->mode) &&
                (0U == (p_usart->info->flags & (USART_FLAG_TX_ENABLED | USART_FLAG_RX_ENABLED))))

            {
                if ( arg == p_usart->info->baudrate )
                {
                    /* CKE - Clock Enable - On-chip baud rate generator
                                            The clock with the same frequency as the bit rate is output
                                            from the SCKn pin. */
                    p_usart->reg->SCR_b.CKE = 1;                /* Clock output enable */
                    result = ARM_DRIVER_OK;
                }
                else if(0 == arg)
                {
                    /* CKE - Clock Enable - On-chip baud rate generator
                                            The SCKn pin functions as I/O port. */
                    p_usart->reg->SCR_b.CKE = 0;                /* Clock output disable */
                    result = ARM_DRIVER_OK;
                }
                else
                {
                    result = ARM_DRIVER_ERROR;
                }
            }
            else
            {
                result = ARM_DRIVER_ERROR;
            }
        }
        break;
        
        case ARM_USART_CONTROL_SMART_CARD_NACK:
        {
            /* Valid only when smart card mode */
            if ((0U != (p_usart->info->flags & USART_FLAG_CONFIGURED)) &&
                (USART_MODE_SMC == p_usart->info->mode))
            {
                if (1 == arg)
                {
                    result = ARM_DRIVER_OK;
                }
                else
                {
                    result = ARM_DRIVER_ERROR;
                }
            }
            else
            {
                result = ARM_DRIVER_ERROR;
            }
        }
        break;
            
        case ARM_USART_CONTROL_TX:
        {
            if (0 == (p_usart->info->flags & USART_FLAG_TX_AVAILABLE))
            {
                /* If a txi interrupt can not be used, an error is returned */
                result = ARM_DRIVER_ERROR;
                break;
            }

            if (1 == arg)
            {
                /* Operating condition of TX valid setting is that                          */
                /* configuration is completed and transmission permission is not set        */
                if (0 != (p_usart->info->flags & USART_FLAG_CONFIGURED))
                {
                    if (0 == (p_usart->info->flags & USART_FLAG_TX_ENABLED))
                    {
                        /* It is not clock synchronous or not receivable */
                        if ((USART_MODE_SYNC != p_usart->info->mode) ||
                              (0U == (p_usart->info->flags & USART_FLAG_RX_ENABLED)))
                        {
                            sci_tx_enable(p_usart);
                        }
                        else
                        {
                            result = ARM_DRIVER_ERROR;
                        }
                    }
                }   
                else
                {
                    /* If it is out of condition, it returns an error */
                    result = ARM_DRIVER_ERROR;
                }
            }
            else
            {
                /* Operating condition of TX invalid setting is that the configuration  */
                /* is completed and transmission permission is set                      */
                if (0 != (p_usart->info->flags & USART_FLAG_CONFIGURED))
                {
                    if (0 != (p_usart->info->flags & USART_FLAG_TX_ENABLED))
                    {
                        /* Not clock synchronized or not RX enabled */
                        if ((USART_MODE_SYNC != p_usart->info->mode) ||
                            (0U == (p_usart->info->flags & USART_FLAG_RX_ENABLED)))
                        {
                            sci_tx_disable(p_usart);
                        }
                        else
                        {
                            result = ARM_DRIVER_ERROR;
                        }
                    }
                }
                else
                {
                    /* If it is out of condition, it returns an error */
                    result = ARM_DRIVER_ERROR;
                    
                }
            }
        }
        break;
        
        case ARM_USART_CONTROL_RX:
        {
            if (0 == (p_usart->info->flags & USART_FLAG_RX_AVAILABLE))
            {
                /* If a rxi interrupt can not be used, an error is returned */
                result = ARM_DRIVER_ERROR;
                break;
            }

            if (1 == arg)
            {
                /* Operating condition of RX valid setting is that              */
                /* configuration is completed and reception is not permitted    */
                if (0 != (p_usart->info->flags & USART_FLAG_CONFIGURED))
                {
                    if (0 == (p_usart->info->flags & USART_FLAG_RX_ENABLED))
                    {
                        /* It is not clock synchronous or it is not in the transmission enabled state */
                        if ((USART_MODE_SYNC != p_usart->info->mode) ||
                            (0 == (p_usart->info->flags & USART_FLAG_TX_ENABLED)))
                        {
                             sci_rx_enable(p_usart);
                        }
                        else
                        {
                            result = ARM_DRIVER_ERROR;
                        }
                    }
                }
                else
                {
                    /* If it is out of condition, it returns an error */
                    result = ARM_DRIVER_ERROR;
                }
            }
            else
            {
                /* Operating condition of RX invalid setting is that       */
                /* configuration is completed and reception is enabled     */
                if (0 != (p_usart->info->flags & USART_FLAG_CONFIGURED))
                {
                    if (0 != (p_usart->info->flags & USART_FLAG_RX_ENABLED))
                    {
                        /* Not in asynchronous mode or TX enabled */
                        if ((USART_MODE_ASYNC == p_usart->info->mode) ||
                            (0U == (p_usart->info->flags & USART_FLAG_TX_ENABLED)))
                        {
                            sci_rx_disable(p_usart);
                        }
                        else
                        {
                            result = ARM_DRIVER_ERROR;
                        }
                    }
                }
                else
                {
                    /* If it is out of condition, it returns an error */
                    result = ARM_DRIVER_ERROR;
                }
            }
        }
        break;
        
        case ARM_USART_CONTROL_TX_RX:
        {
            if ((0 == (p_usart->info->flags & USART_FLAG_TX_AVAILABLE)) ||
                (0 == (p_usart->info->flags & USART_FLAG_RX_AVAILABLE)))
            {
                /* If a txi interrupt or rxi interrupt can not be used, an error is returned */
                /* Please check your EVENT_NUMBER settings in "r_system_cfg.h file".         */
                /* Especially, it often happen to lack of setting of ERI interrupt setting in it */
                /*  when use Receive feature. */
                result = ARM_DRIVER_ERROR;
                break;
            }
        
            if (1 == arg)
            {
                /* The operating conditions of TX and RX valid setting are that                       */
                /* the configuration is completed and that transmission and reception is not enabled  */
                if (0 != (p_usart->info->flags & USART_FLAG_CONFIGURED))
                {
                    switch (p_usart->info->flags & (USART_FLAG_RX_ENABLED | USART_FLAG_TX_ENABLED))
                    {
                        /* If either sending or receiving is permitted, an error is returned */
                        case USART_FLAG_RX_ENABLED:
                        case USART_FLAG_TX_ENABLED:
                        {
                            result = ARM_DRIVER_ERROR;
                        }
                        break;

                        /* Transmission enabled state */
                        case (USART_FLAG_RX_ENABLED | USART_FLAG_TX_ENABLED):
                        {
                            /* As it is already in the permitted state, it exits as it is */
                            __NOP();
                        }
                        break;
                        default:
                        {
                            /* Enable transmission and reception */
                            sci_tx_rx_enable(p_usart);
                        }
                        break;
                    }
                }
                else
                {
                    /* If it is out of condition, it returns an error */
                    result = ARM_DRIVER_ERROR;
                }
            }
            else
            {
                /* Operating conditions for TX and RX invalid setting are that              */
                /* configuration is completed and transmission and reception is enabled     */
                if (0 != (p_usart->info->flags & USART_FLAG_CONFIGURED))
                {
                    switch (p_usart->info->flags & (USART_FLAG_RX_ENABLED | USART_FLAG_TX_ENABLED))
                    {
                        /* If either sending or receiving is permitted, an error is returned */
                        case USART_FLAG_RX_ENABLED:
                        case USART_FLAG_TX_ENABLED:
                        {
                            result = ARM_DRIVER_ERROR;
                        }
                        break;
                        case (USART_FLAG_RX_ENABLED | USART_FLAG_TX_ENABLED):
                        {
                            /* Disable transmission and reception */
                            sci_tx_rx_disable(p_usart);
                        }
                        break;
                        default:
                        {
                            /* Transmission disabled state */
                            __NOP();
                        }
                        break;
                    }
                }
                else
                {
                    /* If it is out of condition, it returns an error */
                    result = ARM_DRIVER_ERROR;
                }
            }
        }
        break;
        case ARM_USART_ABORT_SEND:
        {
            if (0 != (p_usart->info->flags & USART_FLAG_TX_ENABLED))
            {
                p_usart->info->tx_status.tx_num       = 0;              /*  Initialize transmission size */
                p_usart->info->tx_status.send_active  = 0;              /*  Clear transmission active    */
                sci_transmit_stop(p_usart);
                
                sci_regs.scr = p_usart->reg->SCR;                       /*  SCR backup                   */
                if ((USART_MODE_SYNC == p_usart->info->mode) ||
                    (USART_MODE_SMC == p_usart->info->mode))
                {
                    p_usart->reg->SCR &= 0x0F;                          /*  Disable transmit and receive */
                }
                else
                {
                    p_usart->reg->SCR &= 0x5F;                          /*  Disable transmit             */
                }
                sci_clear_irq_req(p_usart->txi_irq);
                p_usart->reg->SCR = sci_regs.scr;                       /*  Returning the send / receive status */
            }
            else
            {
                result = ARM_DRIVER_ERROR;
            }
        }
        break;
        
        case ARM_USART_ABORT_RECEIVE:
        {
            if (0 != (p_usart->info->flags & USART_FLAG_RX_ENABLED))
            {
                p_usart->info->rx_status.busy        = 0;
                sci_receive_stop(p_usart);
                if (USART_MODE_SYNC == p_usart->info->mode)
                {
                /* Disable interrupt for NVIC */
                    p_usart->info->tx_status.tx_num       = 0;              /*  Initialize transmission size */
                    p_usart->info->tx_status.send_active  = 0;              /*  Clear transmission active    */
                    sci_transmit_stop(p_usart);
                    sci_regs.scr = p_usart->reg->SCR;                       /*  SCR backup                   */
                    p_usart->reg->SCR &= 0x0F;                              /*  Disable transmit and receive */
                    sci_clear_irq_req(p_usart->txi_irq);
                    p_usart->reg->SCR = sci_regs.scr;                       /*  Returning the send / receive status */
                }
                if (USART_MODE_SMC == p_usart->info->mode)
                {
                    sci_regs.scr = p_usart->reg->SCR;                       /*  SCR backup                   */
                    p_usart->reg->SCR &= 0xAF;                              /*  Disable receive              */
                    p_usart->reg->SCR = sci_regs.scr;                       /*  Returning the send / receive status */
                }
            }
            else
            {
                result = ARM_DRIVER_ERROR;
            }
        }
        break;
        
        case ARM_USART_ABORT_TRANSFER:
        {
            if ((0 != (p_usart->info->flags & USART_FLAG_TX_ENABLED)) &&
                (0 != (p_usart->info->flags & USART_FLAG_RX_ENABLED)))
            {
                /* Disable interrupt for NVIC */
                sci_transmit_stop(p_usart);
                p_usart->info->rx_status.busy        = 0;
                p_usart->info->tx_status.tx_num      = 0;               /*  Initialize transmission size */
                p_usart->info->tx_status.send_active = 0;               /*  Clear transmission active    */
                sci_receive_stop(p_usart);
                sci_regs.scr = p_usart->reg->SCR;                       /*  SCR backup                   */
                p_usart->reg->SCR &= 0x0F;                              /*  Disable transmit and receive */
                sci_clear_irq_req(p_usart->txi_irq);
                p_usart->reg->SCR = sci_regs.scr;                       /*  Returning the send / receive status */
            }
            else
            {
                result = ARM_DRIVER_ERROR;
            }
        }
        break;

        /* The default case is intentionally combined.  */
        /* Unsupported command */
        case ARM_USART_CONTROL_BREAK:
        case ARM_USART_SET_IRDA_PULSE:
        case ARM_USART_SET_SMART_CARD_GUARD_TIME:
        default:
        {
            result = ARM_DRIVER_ERROR;
        }
        break;
        
    }
  return (result);
}/* End of function ARM_USART_Control() */

/****************************************************************************************************************//**
 *  @brief       Get USART Status
 *  @param[in]   p_usart        Pointer to a USART resource structure
 *  @retval      ARM_USART_STATUS
**********************************************************************************************************************/
/* Function Name: ARM_USART_GetStatus */
ARM_USART_STATUS ARM_USART_GetStatus(st_usart_resources_t const * const p_usart) // @suppress("Function declaration") @suppress("Non-API function naming")
{
    ARM_USART_STATUS status;
    
    if (1 == p_usart->info->tx_status.send_active)
    {
        status.tx_busy  = 1;
    }
    else
    {
        status.tx_busy  = ~(p_usart->reg->SSR_b.TEND);
    }
    status.rx_busy          = p_usart->info->rx_status.busy;
    status.tx_underflow     = 0;
    status.rx_overflow      = p_usart->info->rx_status.overflow;
    status.rx_break         = 0;
    status.rx_framing_error = p_usart->info->rx_status.framing_error;
    status.rx_parity_error  = p_usart->info->rx_status.parity_error;
    status.reserved         = 0;
    return (status);
    
}/* End of function ARM_USART_GetStatus() */

/****************************************************************************************************************//**
* @brie        Set USART Modem Control line state.
* @param[in]   control    ARM_USART_MODEM_CONTROL
* @param[in]   arg        Argument of operation (optional)
* @param[in]   p_usart    Pointer to USART resources
* @retval      Status Error Codes
* @note        When using SetModemControl function, please define USART_RTS_PORT and USART_RTS_PIN of r_usart_cfg.h.
**********************************************************************************************************************/
/* Function Name: ARM_USART_SetModemControl */
int32_t ARM_USART_SetModemControl(ARM_USART_MODEM_CONTROL control, st_usart_resources_t const * const p_usart)// @suppress("Function length") @suppress("Function declaration") @suppress("Non-API function naming")
{
    int32_t result = ARM_DRIVER_OK;
    
    if (NULL == p_usart->rts_port) // @suppress("Cast comment")
    {
        return (ARM_DRIVER_ERROR);
    }
    
    if (0 == (p_usart->info->flags & USART_FLAG_CONFIGURED))
    {
        /* USART not Initialize */
        return (ARM_DRIVER_ERROR);
    }
    
    switch (control)
    {
        case ARM_USART_RTS_CLEAR:
        {
            if ((USART_FLOW_CTS_DISABLE == p_usart->info->flow_mode) ||
                (USART_FLOW_CTS_ENABLE == p_usart->info->flow_mode))
            {
                (*(p_usart->rts_port)) &= (~(1 << (p_usart->rts_pin_no)));
            
            }
            else
            {
                /* Hardware RTS */
                result = ARM_DRIVER_ERROR;
            }
        }
        break;

        case ARM_USART_RTS_SET:
        {
            if ((USART_FLOW_CTS_DISABLE == p_usart->info->flow_mode) ||
                (USART_FLOW_CTS_ENABLE == p_usart->info->flow_mode))
            {
                (*(p_usart->rts_port)) |= (1 << (p_usart->rts_pin_no));
            } 
            else 
            {
                /* Hardware RTS */
                result = ARM_DRIVER_ERROR;
            }
        }
        break;

        /* The default case is intentionally combined.  */
        case ARM_USART_DTR_CLEAR:
        case ARM_USART_DTR_SET:
        default:
        {
            result = ARM_DRIVER_ERROR;
        }
        break;
    }
    return (result);
}/* End of function ARM_USART_SetModemControl() */

/****************************************************************************************************************//**
* @brief    Get USART Modem Status lines state.
* @param[in]   p_usart    Pointer to USART resources
* @retval   modem status ARM_USART_MODEM_STATUS
* @note     When using GetModemStatus function, please define USART_CTS_PORT and USART_CTS_PIN of r_usart_cfg.h.
**********************************************************************************************************************/
/* Function Name: ARM_USART_GetModemStatus */
ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus(st_usart_resources_t const * const p_usart) // @suppress("Function declaration") @suppress("Non-API function naming")
{
    ARM_USART_MODEM_STATUS modem_status;

    modem_status.cts = 0U;
    if ((USART_FLOW_CTS_DISABLE == p_usart->info->flow_mode) ||
        (USART_FLOW_RTS_ENABLE == p_usart->info->flow_mode))
    {
        if (NULL != p_usart->cts_port) // @suppress("Cast comment")
        {
            if (0 == ((*(p_usart->cts_port)) & (1<<(p_usart->cts_pin_no))))
            {
                modem_status.cts = 1U;
            }
        }
    }
    modem_status.dsr = 0U;
    modem_status.ri  = 0U;
    modem_status.dcd = 0U;

  return (modem_status);
}/* End of function ARM_USART_GetModemStatus() */

/* End USART Interface */

/****************************************************************************************************************//**
* @brief        Set to asynchronous mode.
* @param[in]    control      Operation command
* @param[in]    p_sci_regs   Pointer to SCI register setting buffer
* @param[in]    baud         bitrate
* @param[in]    p_usart      Pointer to USART resources
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: mode_set_asynchronous */
static int32_t mode_set_asynchronous(uint32_t control, st_sci_reg_set_t * const p_sci_regs, uint32_t baud, // @suppress("Function length")
                                        st_usart_resources_t * const p_usart)
{
    int32_t result = ARM_DRIVER_OK;
    
    /* Data size setting */
    switch (control & ARM_USART_DATA_BITS_Msk)
    {
        case ARM_USART_DATA_BITS_7:
        {
            /* Character Length - Transmit/receive in 7-bit data length */
            /* CHR - Character Length - 1 (Selects in combination with the SCMR.CHR1 bit.) */
            sci_bit_set_8(&p_sci_regs->smr,  6);           /* CHR  = 1 */

            /* CHR1 - Character Length 1 - 1 (Selects in combination with the SMR.CHR bit.) */
            sci_bit_set_8(&p_sci_regs->scmr, 4);           /* CHR1 = 1 */
            p_sci_regs->data_size = USART_SIZE_7;
        }
        break;
        
        case ARM_USART_DATA_BITS_8:
        {
            /* Character Length - Transmit/receive in 8-bit data length  */
            /* CHR - Character Length - 0 (Selects in combination with the SCMR.CHR1 bit.) */
            sci_bit_clear_8(&p_sci_regs->smr,  6);             /* CHR  = 0 */

            /* CHR1 - Character Length 1 - 1 (Selects in combination with the SMR.CHR bit.) */
            sci_bit_set_8(&p_sci_regs->scmr, 4);           /* CHR1 = 1 */
            p_sci_regs->data_size = USART_SIZE_8;
        }
        break;
        
        case ARM_USART_DATA_BITS_9:
        {
            /* Character Length - Transmit/receive in 9-bit data length */
            /* CHR - Character Length - 0 (Selects in combination with the SMR.CHR1 bit.) */
            sci_bit_clear_8(&p_sci_regs->smr,  6);           /* CHR  = 0 */

            /* CHR1 - Character Length 1 - 0 (Selects in combination with the SMR.CHR bit.) */
            sci_bit_clear_8(&p_sci_regs->scmr, 4);           /* CHR1 = 0 */
            p_sci_regs->data_size = USART_SIZE_9;
        }
        break;
        
        default:
        {
            result = ARM_USART_ERROR_DATA_BITS;
        }
        break;
    }
    
    /* Parity setting */
    switch (control & ARM_USART_PARITY_Msk)
    {
        case ARM_USART_PARITY_NONE:                        /* No parity */
        {
            /* PE - Parity Enable - Disable */
            sci_bit_clear_8(&p_sci_regs->smr, 5);          /* PE = 0(Parity disable) */
        }
        break;
        
        case ARM_USART_PARITY_EVEN:
        {
            /* PM - Parity Mode - Selects even parity */
            sci_bit_clear_8(&p_sci_regs->smr, 4);          /* PM = 0(Even parity) */

            /* PE - Parity Enable - Enable */
            sci_bit_set_8(&p_sci_regs->smr, 5);            /* PE = 1(Parity enable) */
        }
        break;
        
        case ARM_USART_PARITY_ODD:
        {
            /* PM - Parity Mode - Selects odd parity */
            sci_bit_set_8(&p_sci_regs->smr, 4);            /* PM = 1(Odd parity) */

            /* PE - Parity Enable - Enable */
            sci_bit_set_8(&p_sci_regs->smr, 5);            /* PE = 1(Parity enable) */
        }
        break;
        
        default:
        {
            result = ARM_USART_ERROR_PARITY;
        }
        break;
    }

    /* Stop bit setting */
    switch (control & ARM_USART_STOP_BITS_Msk)
    {
        case ARM_USART_STOP_BITS_1:
        {
            /* STOP - Stop Bit Length - 1 stop bit */
            sci_bit_clear_8(&p_sci_regs->smr, 3);          /* STOP = 0(1 stop bit) */
        }
        break;
        
        case ARM_USART_STOP_BITS_2:
        {
            /* STOP - Stop Bit Length - 2 stop bit */
            sci_bit_set_8(&p_sci_regs->smr, 3);            /* STPP = 1(2 stop bit) */
        }
        break;
        
        default:
        {
            result = ARM_USART_ERROR_STOP_BITS;
        }
        break;
    }
    
    /* Flow control setting */
    switch (control & ARM_USART_FLOW_CONTROL_Msk)
    {
        case ARM_USART_FLOW_CONTROL_NONE:
        {
            /* CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled). */
            sci_bit_clear_8(&p_sci_regs->spmr, 1);           /* CTSE = 0(CTS function is disabled) */
            p_sci_regs->flow_mode = USART_FLOW_CTS_DISABLE;
        }
        break;
        
        case ARM_USART_FLOW_CONTROL_RTS:
        {
            /* CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled). */
            sci_bit_clear_8(&p_sci_regs->spmr, 1);           /* CTSE = 0(CTS function is disabled) */
            p_sci_regs->flow_mode = USART_FLOW_RTS_ENABLE;
        }
        break;
        
        case ARM_USART_FLOW_CONTROL_CTS:
        {
            /* CTSE - CTS Enable - CTS function is enabled.*/
            sci_bit_set_8(&p_sci_regs->spmr, 1);           /* CTSE = 1(CTS function is enabled) */
            p_sci_regs->flow_mode = USART_FLOW_CTS_ENABLE;
        }
        break;
        
        default:
        {
            result = ARM_USART_ERROR_FLOW_CONTROL;
        }
        break;
    }
    
    if (ARM_DRIVER_OK == result)
    {
        /* Setting the bit rate */
        result = sci_bitrate(p_sci_regs, baud, p_usart->pclk, &gs_async_baud[0],
                              NUM_PRV_DIVISORS_ASYNC, ARM_USART_MODE_ASYNCHRONOUS);
    }
    
    if (ARM_DRIVER_OK == result)
    {
        /* TE - Transmit Enable - Serial transmission is disabled */
        p_usart->reg->SCR_b.TE  = 0;

        /* RE - Receive Enable - Serial reception is disabled */
        p_usart->reg->SCR_b.RE  = 0;
        
        /* TIE - Transmit Interrupt Enable - A TXI interrupt request is disabled */
        p_usart->reg->SCR_b.TIE = 0;

        /* RIE - Receive Interrupt Enable - RXI and ERI interrupt requests are disabled */
        p_usart->reg->SCR_b.RIE = 0;

        /* TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled */
        p_usart->reg->SCR_b.TEIE = 0;
        
        /* CKE - Clock Enable - On-chip baud rate generator The SCKn pin is available 
                                for use as an I/O port according to the I/O port settings. */
        p_usart->reg->SCR_b.CKE = 0; 
        
        /* IICM - Simple I2C Mode Select - Asynchronous mode, Multi-processor mode, Clock synchronous mode */
        p_usart->reg->SIMR1_b.IICM = 0;

        /* SPMR - SPI Mode Register - The set value depends on the variable.
           b7   - CKPH - Clock Phase Select
           b6   - CKPOL - Clock Polarity Select
           b5   - Reserved  - This bit is read as 0. The write value should be 0.
           b4   - MFF - Mode Fault Flag
           b3   - Reserved - This bit is read as 0. The write value should be 0.
           b2   - MSS - Master Slave Select 
           b1   - CTSE - CTS Enable
           b0   - SSE - SSn Pin Function Enable */
        p_usart->reg->SPMR = p_sci_regs->spmr;

        /* SMR   - Serial Mode Register for Non-Smart Card Interface Mode - The set value depends on the variable.
           b7    - CM  - Communications Mode
           b6    - CHR - Character Length
           b5    - PE  - Parity Enable
           b4    - PM  - Parity Mode
           b3    - STOP - Stop Bit Length
           b2    - MP - Multi-Processor Mode
           b1:b0 - Clock Select - PCLK (n = 0) */
        p_usart->reg->SMR  = p_sci_regs->smr;

        /* SCMR  - Smart Card Mode Register - The set value depends on the variable.
           b7    - BCP2 - Base Clock Pulse 2
           b6:b5 - Reserved - This bit is read as 1. The write value should be 1.
           b4    - CHR1 - Character Length 1
           b3    - SDIR - Transmitted/Received Data Transfer Direction
           b2    - SINV - Transmitted/Received Data Invert
           b1    - Reserved - This bit is read as 1. The write value should be 1.
           b0    - SMIF - Smart Card Interface Mode Select */
        p_usart->reg->SCMR = p_sci_regs->scmr;

        /* SEMR  - Serial Extended Mode Register - The set value depends on the variable.
           b7    - RXDESEL - Asynchronous Start Bit Edge Detection Select
           b6    - BGDM - Baud Rate Generator Double-Speed Mode Select
           b5    - NFEN - Digital Noise Filter Function Enable
           b4    - ABCS - Asynchronous Mode Base Clock Select
           b3    - ABCSE - Asynchronous Mode Extended Base Clock Select1
           b2    - BRME - Bit Rate Modulation Enable - Bit rate modulation function is disabled.
           b1:b0 - Reserved - This bit is read as 0. The write value should be 0. */
        p_usart->reg->SEMR = p_sci_regs->semr;

        /* BRR - Bit Rate Register - initial value */
        p_usart->reg->BRR = p_sci_regs->brr;

        /* MDDR - Modulation Duty Register */
        p_usart->reg->MDDR = p_sci_regs->mddr;
        
        p_usart->info->flow_mode = p_sci_regs->flow_mode;
        p_usart->info->data_size = p_sci_regs->data_size;
        p_usart->info->mode = USART_MODE_ASYNC;
        p_usart->info->baudrate = baud;
    }
    
    return (result);
}/* End of function mode_set_asynchronous() */


/****************************************************************************************************************//**
* @brief        Set to synchronous mode.
* @param[in]    control    Operation command
* @param[in]    p_sci_regs   Pointer to SCI register setting buffer
* @param[in]    baud       bitrate
* @param[in]    p_usart      Pointer to USART resources
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: mode_set_synchronous */
static int32_t mode_set_synchronous(uint32_t control, st_sci_reg_set_t * const p_sci_regs, // @suppress("Function length")
                                        uint32_t baud, st_usart_resources_t * const p_usart)
{
    int32_t result = ARM_DRIVER_OK;
    
    /* Flow control setting */
    switch (control & ARM_USART_FLOW_CONTROL_Msk)
    {
        case ARM_USART_FLOW_CONTROL_NONE:
        {
            /* CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled). */
            sci_bit_clear_8(&p_sci_regs->spmr, 1);           /* CTSE = 0(CTS function is disabled) */
            p_sci_regs->flow_mode = USART_FLOW_CTS_DISABLE;
        }
        break;
        
        case ARM_USART_FLOW_CONTROL_RTS:
        {
            /* CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled). */
            sci_bit_clear_8(&p_sci_regs->spmr, 1);           /* CTSE = 0(CTS function is disabled) */
            p_sci_regs->flow_mode = USART_FLOW_RTS_ENABLE;
        }
        break;
        
        case ARM_USART_FLOW_CONTROL_CTS:
        {
            /* CTSE - CTS Enable - CTS function is enabled.*/
            sci_bit_set_8(&p_sci_regs->spmr, 1);           /* CTSE = 1(CTS function is enabled) */
            p_sci_regs->flow_mode = USART_FLOW_CTS_ENABLE;
        }
        break;
        
        default:
        {
            result = ARM_USART_ERROR_FLOW_CONTROL;
        }
        break;
    }
    
    /* CPOL setting */
    switch (control & ARM_USART_CPOL_Msk)
    {
        case ARM_USART_CPOL0:
        {
            /* CKPOL - Clock Polarity Select - Clock polarity is not inverted. */
            sci_bit_clear_8(&p_sci_regs->spmr, 6);           /* CKPOL = 0(No clock polarity inversion) */
        }
        break;
        
        case ARM_USART_CPOL1:
        {
            /* CKPOL - Clock Polarity Select - Clock polarity is inverted. */
            sci_bit_set_8(&p_sci_regs->spmr, 6);           /* CKPOL = 1(Clock polarity inversion) */
        }
        break;
        
        default:
        {
            /* It never goes through here */
            result = ARM_DRIVER_ERROR;
        }
        break;
    }

    /* CKPH setting */
    switch (control & ARM_USART_CPHA_Msk)
    {
        case ARM_USART_CPHA0:
        {
            /* CKPH - Clock Phase Select - Clock is not delayed. */
            sci_bit_clear_8(&p_sci_regs->spmr, 7);           /* CKPH = 0(Clock is not delayed) */
        }
        break;
        
        case ARM_USART_CPHA1:
        {
            /* CKPH - Clock Phase Select - Clock is delayed. */
            sci_bit_set_8(&p_sci_regs->spmr, 7);           /* CKPH = 1(Clock is delayed) */
        }
        break;
        
        default:
        {
            /* It never goes through here */
            result = ARM_DRIVER_ERROR;
        }
        break;
    }
    
    if ((ARM_DRIVER_OK == result) && (0 != baud))               /* baud==0: Slave mode */
    {
        /* Setting the bit rate */
        result = sci_bitrate(p_sci_regs, baud, p_usart->pclk, &gs_sync_baud[0],
                             NUM_PRV_DIVISORS_SYNC, ARM_USART_MODE_SYNCHRONOUS_MASTER);
    }
    

    
    if (ARM_DRIVER_OK == result)
    {
        /* TE - Transmit Enable - Serial transmission is disabled */
        p_usart->reg->SCR_b.TE  = 0;

        /* RE - Receive Enable - Serial reception is disabled */
        p_usart->reg->SCR_b.RE  = 0;
        
        /* TIE - Transmit Interrupt Enable - A TXI interrupt request is disabled */
        p_usart->reg->SCR_b.TIE = 0;

        /* RIE - Receive Interrupt Enable - RXI and ERI interrupt requests are disabled */
        p_usart->reg->SCR_b.RIE = 0;

        /* TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled */       
        p_usart->reg->SCR_b.TEIE = 0;

        /* SCR   - Serial Control Register for Non-Smart Card Interface Mode - The set value depends on the variable.
           b7    - TIE  - Transmit Interrupt Enable
           b6    - RIE  - Receive Interrupt Enable
           b5    - TE   - Transmit Enable
           b4    - RE   - Receive Enable
           b3    - MPIE - Multi-Processor Interrupt Enable
           b2    - TEIE - Transmit End Interrupt Enable
           b1:b0 - CKE[1:0] - Clock Enable       */
        p_usart->reg->SCR = p_sci_regs->scr;

        /* IICM - Simple I2C Mode Select - Asynchronous mode, Multi-processor mode, Clock synchronous mode */
        p_usart->reg->SIMR1_b.IICM = 0;

        /* SPMR - SPI Mode Register - The set value depends on the variable.
           b7   - CKPH - Clock Phase Select
           b6   - CKPOL - Clock Polarity Select
           b5   - Reserved  - This bit is read as 0. The write value should be 0.
           b4   - MFF - Mode Fault Flag
           b3   - Reserved - This bit is read as 0. The write value should be 0.
           b2   - MSS - Master Slave Select
           b1   - CTSE - CTS Enable
           b0   - SSE - SSn Pin Function Enable */
        p_usart->reg->SPMR = p_sci_regs->spmr;
        
        /* SMR   - Serial Mode Register for Non-Smart Card Interface Mode - The set value depends on the variable.
           b7    - CM  - Communications Mode
           b6    - CHR - Character Length
           b5    - PE  - Parity Enable
           b4    - PM  - Parity Mode
           b3    - STOP - Stop Bit Length
           b2    - MP - Multi-Processor Mode
           b1:b0 - Clock Select - PCLK (n = 0) */
        p_usart->reg->SMR  = p_sci_regs->smr;

        /* SCMR  - Smart Card Mode Register - The set value depends on the variable.
           b7    - BCP2 - Base Clock Pulse 2
           b6:b5 - Reserved - This bit is read as 1. The write value should be 1.
           b4    - CHR1 - Character Length 1
           b3    - SDIR - Transmitted/Received Data Transfer Direction
           b2    - SINV - Transmitted/Received Data Invert
           b1    - Reserved - This bit is read as 1. The write value should be 1.
           b0    - SMIF - Smart Card Interface Mode Select */
        p_usart->reg->SCMR = p_sci_regs->scmr;

        /* SEMR  - Serial Extended Mode Register - The set value depends on the variable.
           b7    - RXDESEL - Asynchronous Start Bit Edge Detection Select
           b6    - BGDM - Baud Rate Generator Double-Speed Mode Select
           b5    - NFEN - Digital Noise Filter Function Enable
           b4    - ABCS - Asynchronous Mode Base Clock Select
           b3    - ABCSE - Asynchronous Mode Extended Base Clock Select1
           b2    - BRME - Bit Rate Modulation Enable
           b1:b0 - Reserved - This bit is read as 0. The write value should be 0. */
        p_usart->reg->SEMR = p_sci_regs->semr;

        /* BRR - Bit Rate Register - initial value */
        p_usart->reg->BRR = p_sci_regs->brr;

        /* MDDR - Modulation Duty Register */
        p_usart->reg->MDDR = p_sci_regs->mddr;
        
        p_usart->info->flow_mode = p_sci_regs->flow_mode;
        p_usart->info->data_size = ARM_USART_DATA_BITS_8;
        p_usart->info->mode = USART_MODE_SYNC;
        p_usart->info->baudrate = baud;
    }
    
    return (result);
}/* End of function mode_set_synchronous() */

/****************************************************************************************************************//**
* @brief        Set to smartcard mode.
* @param[in]    control    Operation command
* @param[in]    p_sci_regs   Pointer to SCI register setting buffer
* @param[in]    baud       bitrate
* @param[in]    p_usart      Pointer to USART resources
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: mode_set_smartcard */
static int32_t mode_set_smartcard(uint32_t control, st_sci_reg_set_t * const p_sci_regs, // @suppress("Function length")
                                        uint32_t baud, st_usart_resources_t * const p_usart)
{
    int32_t result = ARM_DRIVER_OK;
    
    /* Parity setting */
    switch (control & ARM_USART_PARITY_Msk)
    {
        case ARM_USART_PARITY_EVEN:
        {
            /* PM - Parity Mode - Selects even parity */
            sci_bit_clear_8(&p_sci_regs->smr, 4);          /* PM = 0(Even parity) */

            /* PE - Parity Enable - Enable */
            sci_bit_set_8(&p_sci_regs->smr, 5);            /* PE = 1(Parity enable) */
        }
        break;
        
        case ARM_USART_PARITY_ODD:
        {
            /* PM - Parity Mode - Selects odd parity */
            sci_bit_set_8(&p_sci_regs->smr, 4);            /* PM = 1(Odd parity) */

            /* PE - Parity Enable - Enable */
            sci_bit_set_8(&p_sci_regs->smr, 5);            /* PE = 1(Parity enable) */
        }
        break;
        
        case ARM_USART_PARITY_NONE:                        /* No parity */
        default:
        {
            result = ARM_USART_ERROR_PARITY;
        }
        break;
    }

    if ((ARM_DRIVER_OK == result) && (0 != baud))               /* baud==0: Slave mode */
    {
        /* Setting the bit rate */
        result = sci_bitrate(p_sci_regs, baud, p_usart->pclk,
                             &gs_smc_baud[0], NUM_PRV_DIVISORS_SMC, ARM_USART_MODE_SMART_CARD);
    }
    
    if (ARM_DRIVER_OK == result)
    {
        /* TE - Transmit Enable - Serial transmission is disabled */
        p_usart->reg->SCR_b.TE  = 0;

        /* RE - Receive Enable - Serial reception is disabled */
        p_usart->reg->SCR_b.RE  = 0;
        
        /* TIE - Transmit Interrupt Enable - A TXI interrupt request is disabled */
        p_usart->reg->SCR_b.TIE = 0;

        /* RIE - Receive Interrupt Enable - RXI and ERI interrupt requests are disabled */
        p_usart->reg->SCR_b.RIE = 0;

        /* TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled */
        p_usart->reg->SCR_b.TEIE = 0;

        /* SSR_SMCI - Serial Status Register for smart card interface mode
           b5       - ORER - Overrun Error Flag - No overrun error occurred
           b4       - ERS - Error Signal Status Flag - Error signal Low No response.
           b3       - PER - Parity Error Flag - No parity error occurred   */
        p_usart->reg->SSR_SMCI &= 0xC7;

        /* IICM - Simple I2C Mode Select - Asynchronous mode, Multi-processor mode,
                                           Clock synchronous mode                    */
        p_usart->reg->SIMR1_b.IICM = 0;

        /* SPMR - SPI Mode Register
           b7   - CKPH - Clock Phase Select - Clock is not delayed
           b6   - CKPOL - Clock Polarity Select - Clock polarity is not inverted.
           b5   - Reserved  - This bit is read as 0. The write value should be 0.
           b4   - MFF - Mode Fault Flag - No mode fault error
           b3   - Reserved - This bit is read as 0. The write value should be 0.
           b2   - MSS - Master Slave Select - Transmission is through
                                              the TXDn pin and reception is through the RXDn pin (master mode).
           b1   - CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled).
           b0   - SSE - SSn Pin Function Enable - SSn pin function is disabled */
        p_usart->reg->SPMR = 0x00;

        /* SMR   - Serial Mode Register for Non-Smart Card Interface Mode - The set value depends on the variable.
           b7    - CM  - Communications Mode
           b6    - CHR - Character Length
           b5    - PE  - Parity Enable
           b4    - PM  - Parity Mode
           b3    - STOP - Stop Bit Length
           b2    - MP - Multi-Processor Mode
           b1:b0 - Clock Select - PCLK (n = 0) */
        p_usart->reg->SMR  = p_sci_regs->smr;

        /* SCMR  - Smart Card Mode Register - The set value depends on the variable.
           b7    - BCP2 - Base Clock Pulse 2
           b6:b5 - Reserved - This bit is read as 1. The write value should be 1.
           b4    - CHR1 - Character Length 1
           b3    - SDIR - Transmitted/Received Data Transfer Direction
           b2    - SINV - Transmitted/Received Data Invert
           b1    - Reserved - This bit is read as 1. The write value should be 1.
           b0    - SMIF - Smart Card Interface Mode Select */
        p_usart->reg->SCMR = p_sci_regs->scmr;

        /* SEMR  - Serial Extended Mode Register - The set value depends on the variable.
           b7    - RXDESEL - Asynchronous Start Bit Edge Detection Select
           b6    - BGDM - Baud Rate Generator Double-Speed Mode Select
           b5    - NFEN - Digital Noise Filter Function Enable
           b4    - ABCS - Asynchronous Mode Base Clock Select
           b3    - ABCSE - Asynchronous Mode Extended Base Clock Select1
           b2    - BRME - Bit Rate Modulation Enable - Bit rate modulation function is disabled.
           b1:b0 - Reserved - This bit is read as 0. The write value should be 0. */
        p_usart->reg->SEMR = p_sci_regs->semr;

        /* BRR - Bit Rate Register - initial value */
        p_usart->reg->BRR = p_sci_regs->brr;

        /* MDDR - Modulation Duty Register */
        p_usart->reg->MDDR = p_sci_regs->mddr;

        /* SCR   - Serial Control Register for Non-Smart Card Interface Mode
           b7    - TIE  - Transmit Interrupt Enable - A TXI interrupt request is disabled
           b6    - RIE  - Receive Interrupt Enable - RXI and ERI interrupt requests are disabled
           b5    - TE   - Transmit Enable - Serial transmission is disabled
           b4    - RE   - Receive Enable - Serial transmission is disabled
           b3    - MPIE - Multi-Processor Interrupt Enable - Normal reception
           b2    - TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
           b1:b0 - CKE[1:0] - Clock Enable - On-chip baud rate generator */
        p_usart->reg->SCR = 0x00;
        
        p_usart->info->flow_mode = USART_FLOW_CTS_DISABLE;
        p_usart->info->data_size = ARM_USART_DATA_BITS_8;
        p_usart->info->mode = USART_MODE_SMC;
        p_usart->info->baudrate = baud;
    }
    
    return (result);
}/* End of function mode_set_smartcard() */

/****************************************************************************************************************//**
* @brief        Calculate the setting value from the bitrate.
* @param[in]    p_sci_regs     Pointer to SCI register setting buffer
* @param[in]    baud           bitrate
* @param[in]    p_baud_info    Pointer to bitrate calculation table
* @param[in]    num_divisors   Number of bitrate calculation table elements
* @param[in]    mode           USART mode
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: sci_bitrate */
static int32_t sci_bitrate(st_sci_reg_set_t *p_sci_regs, uint32_t baud, e_usart_base_clk_t base_clk, // @suppress("Function length")
                                st_baud_divisor_t const *p_baud_info, uint8_t num_divisors, uint32_t mode)
{
    uint32_t i;
    uint32_t ratio;
    uint32_t tmp;

    uint32_t divisor;
    uint32_t pclk = 0;
    int32_t result = ARM_DRIVER_OK;

#if (1 == USART_MODULATION_FUNC)
    uint32_t int_M;
    float    float_M;
    float    error;
    float    abs_error;
#endif
    
    switch (base_clk)
    {
        case USART_BASE_PCLKA:
        {
            R_SYS_SystemClockFreqGet(&pclk);
        }
        break;

        case USART_BASE_PCLKB:
        {
            R_SYS_PeripheralClockFreqGet(&pclk);
        }
        break;

        default:
        {
            return (ARM_DRIVER_ERROR);
        }
        break;
    }

    if ((0 == pclk) || (0 == baud))
    {
        return (ARM_USART_ERROR_BAUDRATE);
    }

    /* FIND DIVISOR; table has associated ABCS, BGDM and CKS values */
    /* BRR must be 255 or less */
    /* the "- 1" is ignored in some steps for approximations */
    /* BRR = (PCLK/(divisor * baud)) - 1 */
    /* BRR = (ratio / divisor) - 1 */
    ratio = pclk / baud;
    for (i = 0; i < num_divisors; i++)
    {
        if (ratio < (uint32_t)(p_baud_info[i].divisor * 256)) // @suppress("Cast comment")
        {
            break; // @suppress("Break statement")
        }
    }

    /* RETURN IF BRR WILL BE >255 OR LESS THAN 0 */
    if (i == num_divisors)
    {
        result = ARM_USART_ERROR_BAUDRATE;              /* impossible baud rate requested; return 100% error */
    }

    if (ARM_DRIVER_OK == result)
    {
        divisor = (uint32_t)p_baud_info[i].divisor; // @suppress("Cast comment")
        tmp = ratio / (divisor);                          /* tmp = PCLK/(baud * divisor) = BRR+1 = N+1 */
        if (0 == tmp)
        {
            result = ARM_USART_ERROR_BAUDRATE;              /* illegal value; return 100% error */
        }
    }

    if (ARM_DRIVER_OK == result)
    {

        /* SET BRR, ABCS, BDGM, and CKS */
        tmp = ratio / (divisor/2);  /* divide by half the divisor */
    
        /* if odd, "round up" by ignoring -1; divide by 2 again for rest of divisor */
        p_sci_regs->brr = (uint8_t)((tmp & 0x01) ? (tmp / 2) : ((tmp / 2) - 1));
        USART_DIVISORS_SET_BIT(REG_PRV_VALUE_ABCSE, p_baud_info[i].reg_value, p_sci_regs->semr, 3);
        USART_DIVISORS_SET_BIT(REG_PRV_VALUE_ABCS , p_baud_info[i].reg_value, p_sci_regs->semr, 4);
        USART_DIVISORS_SET_BIT(REG_PRV_VALUE_BGMD , p_baud_info[i].reg_value, p_sci_regs->semr, 6);
        USART_DIVISORS_SET_BIT(REG_PRV_VALUE_CKS0 , p_baud_info[i].reg_value, p_sci_regs->smr , 0);
        USART_DIVISORS_SET_BIT(REG_PRV_VALUE_CKS1 , p_baud_info[i].reg_value, p_sci_regs->smr , 1);
        if (ARM_USART_MODE_SMART_CARD == mode)
        {
            USART_DIVISORS_SET_BIT(REG_PRV_VALUE_BCP0 , p_baud_info[i].reg_value, p_sci_regs->smr , 2);
            USART_DIVISORS_SET_BIT(REG_PRV_VALUE_BCP1 , p_baud_info[i].reg_value, p_sci_regs->smr , 3);
            USART_DIVISORS_SET_BIT(REG_PRV_VALUE_BCP2 , p_baud_info[i].reg_value, p_sci_regs->scmr, 7);
        }
    
#if (1 == USART_MODULATION_FUNC)
        /* CALCULATE BIT RATE ERROR.
         * RETURN IF ERROR LESS THAN 1% OR IF IN SYNCHRONOUS/SSPI MODE.
         */
        tmp = ratio / (divisor);      /* tmp = PCLK / (baud * divisor) = BRR + 1 = N + 1 */
        error = (((float)pclk / ((baud * divisor) * tmp)) - 1) * 100;
        abs_error  = (error < 0) ? (-error) : error;
    
        if ((abs_error <= 1.0) || (ARM_USART_MODE_SYNCHRONOUS_MASTER == mode))
        {
            /* BRME - Modulation Duty Register - Bit rate modulation function is disabled. */
            sci_bit_clear_8(&p_sci_regs->semr, 2);                     /* disable MDDR */
            return (ARM_DRIVER_OK);
        }
        else
        {
            /* CALCULATE M ASSUMING A 0% ERROR then WRITE REGISTER */
            p_sci_regs->brr = (uint8_t)(tmp - 1);
            float_M = ((float)((baud * divisor) * 256) * tmp) / pclk;
            float_M *= 2;
            int_M = (uint32_t)float_M;
            int_M = (int_M & 0x01) ? ((int_M/2) + 1) : (int_M / 2);
            
            p_sci_regs->mddr = (uint8_t)int_M;
            /* BRME - Modulation Duty Register - Bit rate modulation function is enabled. */
            sci_bit_set_8(&p_sci_regs->semr, 2);                     /* enable MDDR */
        }
#endif
    }
    
    return (result);

}/* End of function sci_bitrate() */

/****************************************************************************************************************//**
* @brief        Clear SCI register setting buffer.
* @param[in]    p_regs   Pointer to SCI register setting buffer
* @param[in]    regs    Pointer to SCI register setting buffer
**********************************************************************************************************************/
/* Function Name: sci_set_regs_clear */
static void sci_set_regs_clear(st_sci_reg_set_t * const p_regs)
{
    /* SMR   - Serial Mode Register for Non-Smart Card Interface Mode
       b7    - CM  - Communications Mode - Asynchronous mode or simple I2C mode
       b6    - CHR - Character Length - Transmit/receive in 9-bit data length
       b5    - PE  - Parity Enable - When transmitting : Parity bit addition is not performed
                                     When receiving    : Parity bit checking is not performed
       b4    - PM  - Parity Mode - Selects even parity(Valid only when the PE bit is 1)
       b3    - STOP - Stop Bit Length - 1 stop bit (Valid only in asynchronous mode)
       b2    - MP - Multi-Processor Mode - Multi-processor communications function is disabled
                                          (Valid only in asynchronous mode)
       b1:b0 - Clock Select - PCLK (n = 0) */
    p_regs->smr  = 0x00;

    /* SCR   - Serial Control Register for Non-Smart Card Interface Mode
       b7    - TIE  - Transmit Interrupt Enable - A TXI interrupt request is disabled
       b6    - RIE  - Receive Interrupt Enable - RXI and ERI interrupt requests are disabled
       b5    - TE   - Transmit Enable - Serial transmission is disabled
       b4    - RE   - Receive Enable - Serial transmission is disabled
       b3    - MPIE - Multi-Processor Interrupt Enable - Normal reception
       b2    - TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
       b1:b0 - CKE[1:0] - Clock Enable - On-chip baud rate generator
                                         The SCKn pin functions as I/O port. */
    p_regs->scr  = 0x00;

    /* BRR - Bit Rate Register - initial value */
    p_regs->brr  = 0xff;

    /* BRR - Bit Rate Register - initial value */
    p_regs->scmr = 0xF2;

    /* SEMR  - Serial Extended Mode Register
       b7    - RXDESEL - Asynchronous Start Bit Edge Detection Select
                       - on the RXDn pin is detected as the start bit.
       b6    - BGDM - Baud Rate Generator Double-Speed Mode Select
                    - Baud rate generator outputs the clock with normal frequency. 
       b5    - NFEN - Digital Noise Filter Function Enable
                    - Noise cancellation function for the RXDn input signal is disabled.
       b4    - ABCS - Asynchronous Mode Base Clock Select
                    - Selects 16 base clock cycles for 1-bit period.
       b3    - ABCSE - Asynchronous Mode Extended Base Clock Select1
                     - The number of clocks in one bit period is determined 
                       by the combination of BGDM and ABCS in the SEMR register.
       b2    - BRME - Bit Rate Modulation Enable - Bit rate modulation function is disabled.
       b1:b0 - Reserved - This bit is read as 0. The write value should be 0. */
    p_regs->semr = 0x00;

    /* SPMR - SPI Mode Register
       b7   - CKPH - Clock Phase Select - Clock is not delayed
       b6   - CKPOL - Clock Polarity Select - Clock polarity is not inverted.
       b5   - Reserved  - This bit is read as 0. The write value should be 0.
       b4   - MFF - Mode Fault Flag - No mode fault error
       b3   - Reserved - This bit is read as 0. The write value should be 0.
       b2   - MSS - Master Slave Select - Transmission is through
                                          the TXDn pin and reception is through the RXDn pin (master mode).
       b1   - CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled).
       b0   - SSE - SSn Pin Function Enable - SSn pin function is disabled */
    p_regs->spmr = 0x00;
    p_regs->mddr = 0xFF;
    p_regs->flow_mode = USART_FLOW_CTS_DISABLE;
}/* End of function sci_set_regs_clear() */

/****************************************************************************************************************//**
* @brief        Transmission enable.
* @param[in]    p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: sci_tx_enable */
static void sci_tx_enable(st_usart_resources_t * const p_usart)
{

    /* TIE - Transmit Interrupt Enable - A TXI interrupt request is enabled */
    /* TE - Transmit Enable - Serial transmission is enabled */
    p_usart->reg->SCR |= 0xA0;
    
    p_usart->pin_set();
    
    p_usart->info->flags |= USART_FLAG_TX_ENABLED;

    if ((USART_MODE_SMC == p_usart->info->mode) &&
        (0U == (p_usart->info->flags & USART_FLAG_RX_ENABLED)))
    {
        /* In the case of a smart card, reception is permitted for parity reception */
        sci_rx_enable(p_usart);
    }

}/* End of function sci_tx_enable() */

/****************************************************************************************************************//**
* @brief        Transmission disable.
* @param[in]    p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: sci_tx_disable */
static void sci_tx_disable(st_usart_resources_t * const p_usart)
{

    /* If reception is also prohibited, cancel the terminal setting */
    if (0U == (p_usart->info->flags & USART_FLAG_RX_ENABLED))
    {
        p_usart->pin_clr();
    }

    /* TIE - Transmit Interrupt Enable - A TXI interrupt request is disabled */
    p_usart->reg->SCR_b.TIE = 0;

    /* TE - Transmit Enable - Serial transmission is disabled */
    p_usart->reg->SCR_b.TE  = 0;

    /* Since transmission is forcibly canceled, internal parameters are also initialized */
    p_usart->info->tx_status.tx_num       = 0;
    p_usart->info->tx_status.send_active  = 0;
    
    while (0 != p_usart->reg->SCR_b.TIE)
    {
        /* TIE clear check */
        __NOP();
    }
    sci_transmit_stop(p_usart);
    p_usart->info->flags &= (~USART_FLAG_TX_ENABLED);
}/* End of function sci_tx_disable() */


/****************************************************************************************************************//**
 * @brief      Reception enable.
 * @param[in]   p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: sci_rx_enable */
static void sci_rx_enable(st_usart_resources_t * const p_usart)
{
    
    p_usart->pin_set();

     /* RE - Receive Enable - Serial reception is enabled */
    p_usart->reg->SCR_b.RE  = 1;

    /* RIE - Receive Interrupt Enable - RXI and ERI interrupt requests are enable */
    p_usart->reg->SCR_b.RIE = 1;
    p_usart->info->flags |= USART_FLAG_RX_ENABLED;

    /* RXI and ERI interrupt enable for NVIC */
    R_NVIC_EnableIRQ(p_usart->rxi_irq);
    R_NVIC_EnableIRQ(p_usart->eri_irq);

}/* End of function sci_rx_enable() */

/****************************************************************************************************************//**
* @brief      Reception disable.
* @param[in]  p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: sci_rx_disable */
static void sci_rx_disable(st_usart_resources_t * const p_usart)
{
    /* RE - Receive Enable - Serial reception is disabled */
    p_usart->reg->SCR_b.RE  = 0;

    /* RIE - Receive Interrupt Enable - RXI and ERI interrupt requests are disabled */
    p_usart->reg->SCR_b.RIE = 0;
    while (0 != p_usart->reg->SCR_b.RIE)
    {
        /* RIE clear check */
        __NOP();
    }

    /* Since reception is forcibly canceled, internal parameters are also initialized */
    p_usart->info->tx_status.rx_num  = 0;
    p_usart->info->rx_status.busy    = 0;
    sci_receive_stop(p_usart);

    sci_disable_irq(p_usart->rxi_irq);
    sci_disable_irq(p_usart->eri_irq);

    /* If transmit is also prohibited, cancel the terminal setting */
    if (0U == (p_usart->info->flags & USART_FLAG_TX_ENABLED))
    {
        p_usart->pin_clr();
    }
    sci_clear_irq_req(p_usart->rxi_irq);
    sci_clear_irq_req(p_usart->eri_irq);
    p_usart->info->flags &= (~USART_FLAG_RX_ENABLED);
}/* End of function sci_rx_disable() */

/****************************************************************************************************************//**
 * @brief      Reception and Transmission enable.
 * @param[in]   p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: sci_tx_rx_enable */
static void sci_tx_rx_enable(st_usart_resources_t * const p_usart)
{
    /* RE - Receive Enable - Serial reception is enabled */
    /* SCR   - Serial Control Register for Non-Smart Card Interface Mode
       b7    - TIE  - Transmit Interrupt Enable - A TXI interrupt request is enabled
       b6    - RIE  - Receive Interrupt Enable - RXI and ERI interrupt requests are enabled
       b5    - TE   - Transmit Enable - Serial transmission is enabled
       b4    - RE   - Receive Enable - Serial transmission is enabled */
    p_usart->reg->SCR  |= 0xF0;
    
    p_usart->pin_set();
    
    p_usart->info->flags |= (USART_FLAG_RX_ENABLED | USART_FLAG_TX_ENABLED);

    /* RXI and ERI interrupt enable for NVIC */
    R_NVIC_EnableIRQ(p_usart->rxi_irq);
    R_NVIC_EnableIRQ(p_usart->eri_irq);

}/* End of function sci_tx_rx_enable() */

/****************************************************************************************************************//**
 * @brief      Reception and Transmission disable.
 * @param[in]   p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: sci_tx_rx_disable */
static void sci_tx_rx_disable(st_usart_resources_t * const p_usart)
{
    p_usart->pin_clr();
    
    /* RE - Receive Enable - Serial reception is enabled */
    /* SCR   - Serial Control Register for Non-Smart Card Interface Mode
       b7    - TIE  - Transmit Interrupt Enable - A TXI interrupt request is disabled
       b6    - RIE  - Receive Interrupt Enable - RXI and ERI interrupt requests ared disabled
       b5    - TE   - Transmit Enable - Serial transmission is disabled
       b4    - RE   - Receive Enable - Serial transmission is disabled
       b2    - TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled */
    p_usart->reg->SCR  &= 0x0F;
    while (0x00 != (p_usart->reg->SCR & 0xC0))
    {
        /* clear check */
        __NOP();
    }

    /* Since transmission / reception is forcibly canceled, internal parameters are also initialized */
    p_usart->info->tx_status.rx_num       = 0;
    p_usart->info->rx_status.busy         = 0;
    p_usart->info->tx_status.tx_num       = 0;
    p_usart->info->tx_status.send_active  = 0;
    
    sci_transmit_stop(p_usart);
    sci_receive_stop(p_usart);
   
    sci_disable_irq(p_usart->rxi_irq);
    sci_disable_irq(p_usart->eri_irq);    
    sci_disable_irq(p_usart->txi_irq);
    
    sci_clear_irq_req(p_usart->txi_irq);
    sci_clear_irq_req(p_usart->rxi_irq);
    sci_clear_irq_req(p_usart->eri_irq);
    p_usart->info->flags &= (~(USART_FLAG_RX_ENABLED | USART_FLAG_TX_ENABLED));
    
}/* End of function sci_tx_rx_disable() */

/****************************************************************************************************************//**
* @brief        check_tx_available
* @param[in]    p_flag    Pointer to initialize flag
* @param[in]    p_usart    Pointer to USART resources
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: check_tx_available */
static int32_t check_tx_available(int16_t * const p_flag, st_usart_resources_t * const p_usart)
{
    if (NULL == p_usart->tx_dma_drv) // @suppress("Cast comment")
    {
        /* When sending by interrupt */
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_usart->txi_irq)
        {
            /* Register txi interrupt handler */
            if ((-1) == R_SYS_IrqEventLinkSet(p_usart->txi_irq, p_usart->txi_iesr_val, p_usart->txi_callback))
            {
                return (ARM_DRIVER_ERROR);
            }
    
            /* TXI priority setting */
            R_NVIC_SetPriority(p_usart->txi_irq, p_usart->txi_priority);
            if (R_NVIC_GetPriority (p_usart->txi_irq) != p_usart->txi_priority)
            {
                return (ARM_DRIVER_ERROR);
            }
            (*p_flag) |= USART_FLAG_TX_AVAILABLE;
        }
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == p_usart->tx_dma_drv)
    {
        /* When sending by DTC */
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_usart->txi_irq)
        {
            return (ARM_DRIVER_ERROR);
        }

        /* Register txi interrupt handler */
        if ((-1) == R_SYS_IrqEventLinkSet(p_usart->txi_irq, p_usart->txi_iesr_val, p_usart->txi_callback))
        {
            return (ARM_DRIVER_ERROR);
        }

        /* TXI priority setting */
        R_NVIC_SetPriority(p_usart->txi_irq, p_usart->txi_priority);
        if (R_NVIC_GetPriority (p_usart->txi_irq) != p_usart->txi_priority)
        {
            return (ARM_DRIVER_ERROR);
        }

        /* When using the DMAC / DTC driver at transmission */
        if (p_usart->tx_dma_drv->Open () != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }
        (*p_flag) |= USART_FLAG_TX_AVAILABLE;
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        /* When sending by DMAC */
        if (p_usart->tx_dma_drv->Open () != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }
        if (p_usart->tx_dma_drv->InterruptEnable (0, NULL) != DMA_OK) // @suppress("Cast comment")
        {
            return (ARM_DRIVER_ERROR);
        }
        (*p_flag) |= USART_FLAG_TX_AVAILABLE;
    }
#endif
        
    
    return (ARM_DRIVER_OK);
}/* End of function check_tx_available() */

/****************************************************************************************************************//**
* @brief        check_rx_available
* @param[in]    p_flag    Pointer to initialize flag
* @param[in]    p_usart    Pointer to USART resources
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: check_rx_available */
static int32_t check_rx_available(int16_t * const p_flag, st_usart_resources_t * const p_usart)
{
    if ((SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_usart->rxi_irq) || // @suppress("Cast comment")
        (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_usart->eri_irq)) // @suppress("Cast comment")
    {
        if (NULL == p_usart->rx_dma_drv) // @suppress("Cast comment")
        {
            return (ARM_DRIVER_OK);
        }
#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
        else
        {
            return (ARM_DRIVER_ERROR);
        }
#endif
    }

    /* Register rxi interrupt handler */
    if ((-1) == R_SYS_IrqEventLinkSet(p_usart->rxi_irq, p_usart->rxi_iesr_val, p_usart->rxi_callback))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* RXI priority setting */
    R_NVIC_SetPriority(p_usart->rxi_irq, p_usart->rxi_priority);
    if (R_NVIC_GetPriority (p_usart->rxi_irq) != p_usart->rxi_priority)
    {
        return (ARM_DRIVER_ERROR);
    }
        
    /* Register eri interrupt handler */
    if ((-1) == R_SYS_IrqEventLinkSet(p_usart->eri_irq, p_usart->eri_iesr_val, p_usart->eri_callback))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* ERI priority setting */
    R_NVIC_SetPriority(p_usart->eri_irq, p_usart->eri_priority);
    if (R_NVIC_GetPriority (p_usart->eri_irq) != p_usart->eri_priority)
    {
        return (ARM_DRIVER_ERROR);
    }

#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
    if (NULL != p_usart->rx_dma_drv) // @suppress("Cast comment")
    {
        if (p_usart->rx_dma_drv->Open () != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }
        
#if (0 != USART_PRV_USED_RX_DMAC_DRV)
#if (0 != USART_PRV_USED_RX_DTC_DRV)
        if ((&Driver_DTC) != p_usart->rx_dma_drv)
#endif
        {
            if (p_usart->rx_dma_drv->InterruptEnable (0, NULL) != DMA_OK) // @suppress("Cast comment")
            {
                return (ARM_DRIVER_ERROR);
            }
        }
#endif
    }
#endif
    (*p_flag) |= USART_FLAG_RX_AVAILABLE;
    return (ARM_DRIVER_OK);
}/* End of function check_rx_available() */

/****************************************************************************************************************//**
* @brief    Transmit stop.
* @param[in,out]    p_usart        Pointer to USART resource data
********************************************************************************************************************/
/* Function Name: sci_transmit_stop */
static void sci_transmit_stop(st_usart_resources_t const * const p_usart)
{

    if (NULL == p_usart->tx_dma_drv) // @suppress("Cast comment")
    {
        /* When receiving by interrupt */
        sci_disable_irq(p_usart->txi_irq);
        sci_clear_irq_req(p_usart->txi_irq);
    }
#if (0 != USART_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == p_usart->tx_dma_drv)
    {
        /* When receiving by DTC */
        p_usart->tx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &(p_usart->txi_irq));
        sci_disable_irq(p_usart->txi_irq);
        sci_clear_irq_req(p_usart->txi_irq);
    }
#endif
#if (0 != USART_PRV_USED_TX_DMAC_DRV)
    else
    {
        /* When receiving by DMAC */
        p_usart->tx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        p_usart->tx_dma_drv->InterruptDisable();
    }
#endif
}/* End of function sci_transmit_stop() */

/****************************************************************************************************************//**
* @brief    Reception stop.
* @param[in,out]    p_usart        Pointer to USART resource data
********************************************************************************************************************/
/* Function Name: sci_receive_stop */
static void sci_receive_stop(st_usart_resources_t const * const p_usart)
{
    volatile uint16_t dummy;
    if (NULL == p_usart->rx_dma_drv) // @suppress("Cast comment")
    {   /* @suppress("Empty compound statement") */ /* @suppress("Block comment") */
        /* When receiving by interrupt */
        /* No action to be performed */
    }
#if (0 != USART_PRV_USED_RX_DTC_DRV)
    else if ((&Driver_DTC) == p_usart->rx_dma_drv)
    {
        /* When receiving by DTC */
        p_usart->rx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &(p_usart->rxi_irq));
    }
#endif
#if (0 != USART_PRV_USED_RX_DMAC_DRV)
    else
    {
        /* When receiving by DMAC */
        p_usart->rx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        p_usart->rx_dma_drv->InterruptDisable();

        (void)R_SYS_IrqEventLinkSet(p_usart->rxi_irq, p_usart->rxi_iesr_val, p_usart->rxi_callback); // @suppress("Cast comment")
        R_NVIC_EnableIRQ(p_usart->rxi_irq);

        /* Dummy read */
        dummy = p_usart->reg->RDRHL;
    }
#endif
}/* End of function sci_receive_stop() */

#if (0 != USART_PRV_USED_DMAC_DTC_DRV)
/****************************************************************************************************************//**
* @brief    dma config data initialize.
* @param[in,out]    p_cfg        Pointer to DMA config data
********************************************************************************************************************/
/* Function Name: dma_config_init */
static void dma_config_init(st_dma_transfer_data_cfg_t *p_cfg)
{
    p_cfg->block_size = 0;
    p_cfg->offset     = 0;
    p_cfg->src_extended_repeat  = 0;
    p_cfg->dest_extended_repeat = 0;
}/* End of function dma_config_init() */
#endif

/****************************************************************************************************************//**
* @brief      txi interrupt
* @param[in]  p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: txi_handler */
static void txi_handler(st_usart_resources_t * const p_usart)
{
    if (USART_SIZE_9 == p_usart->info->data_size)
    {
        /* 9 bit mode is asynchronous only.                                                                  */
        /* It is unnecessary to judge at the 9-bit time because the start-stop synchronization is confirmed. */
        /* TDRHL - Transmit Data Register HL */
        p_usart->reg->TDRHL = ((uint16_t*)(p_usart->info->tx_status.tx_buf))[p_usart->info->tx_status.tx_cnt];
    }
    else
    {
        if ((USART_MODE_SYNC != p_usart->info->mode) ||
           (USART_SYNC_RX_MODE != p_usart->info->tx_status.sync_mode))
        {
            /* TDR - Transmit Data Register */
            p_usart->reg->TDR = ((uint8_t*)(p_usart->info->tx_status.tx_buf))[p_usart->info->tx_status.tx_cnt];
        }
        else
        {
            /* TDR - Transmit Data Register */
            p_usart->reg->TDR = ((uint8_t)(p_usart->info->tx_status.tx_def_val));
        }
    }
    
    p_usart->info->tx_status.tx_cnt++;
    
    if (p_usart->info->tx_status.tx_cnt >= p_usart->info->tx_status.tx_num)
    {
        /* TXI interrupt disable for NVIC */
        sci_disable_irq(p_usart->txi_irq);
        p_usart->info->tx_status.send_active = 0;

        if ((USART_MODE_SYNC != p_usart->info->mode) ||
           (USART_SYNC_TX_MODE == p_usart->info->tx_status.sync_mode))
        {
            if (NULL != p_usart->info->cb_event) // @suppress("Cast comment")
            {
                /* Execution of callback function */
                p_usart->info->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
            }
        }
    }
}/* End of function txi_handler() */

#if (0 != USART_PRV_USED_TX_DTC_DRV)
/****************************************************************************************************************//**
* @brief      txi interrupt for DTC
* @param[in]  p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: txi_dtc_handler */
static void txi_dtc_handler(st_usart_resources_t * const p_usart)
{
    if (0 == p_usart->info->tx_status.tx_cnt)
    {
        if (USART_SIZE_9 == p_usart->info->data_size)
        {
            /* 9 bit mode is asynchronous only.                                                                  */
            /* It is unnecessary to judge at the 9-bit time because the start-stop synchronization is confirmed. */
            /* TDRHL - Transmit Data Register HL */
            p_usart->reg->TDRHL = ((uint16_t*)(p_usart->info->tx_status.tx_buf))[0];
        }
        else
        {
            if ((USART_MODE_SYNC != p_usart->info->mode) ||
               (USART_SYNC_RX_MODE != p_usart->info->tx_status.sync_mode))
            {
                /* TDR - Transmit Data Register */
                p_usart->reg->TDR = ((uint8_t*)(p_usart->info->tx_status.tx_buf))[0];
            }
            else
            {
                /* TDR - Transmit Data Register */
                p_usart->reg->TDR = ((uint8_t)(p_usart->info->tx_status.tx_def_val));
            }
        }
        
    }
    
    if ((0 != p_usart->info->tx_status.tx_cnt) || (1 == p_usart->info->tx_status.tx_num))
    {
        p_usart->info->tx_status.tx_cnt = 1;
        /* TXI interrupt disable  */
        p_usart->tx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &(p_usart->txi_irq));
        sci_disable_irq(p_usart->txi_irq);
        p_usart->info->tx_status.send_active = 0;

        if ((USART_MODE_SYNC != p_usart->info->mode) ||
           (USART_SYNC_TX_MODE == p_usart->info->tx_status.sync_mode))
        {
            if (NULL != p_usart->info->cb_event) // @suppress("Cast comment")
            {
                /* Execution of callback function */
                p_usart->info->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
            }
        }
    }
    else
    {
        p_usart->info->tx_status.tx_cnt = 1;
    }
}/* End of function txi_dtc_handler() */
#endif

#if (0 != USART_PRV_USED_TX_DMAC_DRV)
/****************************************************************************************************************//**
* @brief      txi interrupt for DMAC
* @param[in]  p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: txi_dmac_handler */
static void txi_dmac_handler(st_usart_resources_t * const p_usart)
{
    /* TXI interrupt disable  */
    p_usart->tx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
    p_usart->tx_dma_drv->InterruptDisable();
    p_usart->info->tx_status.send_active = 0;

    if ((USART_MODE_SYNC != p_usart->info->mode) ||
       (USART_SYNC_TX_MODE == p_usart->info->tx_status.sync_mode))
    {
        if (NULL != p_usart->info->cb_event) // @suppress("Cast comment")
        {
            /* Execution of callback function */
            p_usart->info->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
        }
    }
}/* End of function txi_dmac_handler() */
#endif

/****************************************************************************************************************//**
* @brief      rxi interrupt
* @param[in]  p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: rxi_handler */
static void rxi_handler(st_usart_resources_t * const p_usart)
{
    volatile uint16_t dummy;
    if (1 == p_usart->info->rx_status.busy)
    {
        if (USART_SIZE_9 == p_usart->info->data_size)
        {
            /* Acquire value from reception 9 bit data register */
            ((uint16_t*)(p_usart->info->tx_status.rx_buf))[p_usart->info->tx_status.rx_cnt]
                                                                                = p_usart->reg->RDRHL;
        }
        else
        {
            /* Get value from received data register */
            ((uint8_t*)(p_usart->info->tx_status.rx_buf))[p_usart->info->tx_status.rx_cnt] = p_usart->reg->RDR;
        }

        p_usart->info->tx_status.rx_cnt++;        
        if (p_usart->info->tx_status.rx_cnt >= p_usart->info->tx_status.rx_num)
        {
            p_usart->info->rx_status.busy = 0;
            if (NULL != p_usart->info->cb_event) // @suppress("Cast comment")
            {
                if (USART_SYNC_TX_RX_MODE == p_usart->info->tx_status.sync_mode)
                {
                    /* Execution of callback function */
                    p_usart->info->cb_event(ARM_USART_EVENT_TRANSFER_COMPLETE);
                }
                else
                {
                    /* Execution of callback function */
                    p_usart->info->cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
                }
            }
        }
    }
    else
    {
        /* Dummy read */
        dummy = p_usart->reg->RDRHL;
    }
}/* End of function rxi_handler() */

#if (0 != USART_PRV_USED_RX_DMAC_DTC_DRV)
/****************************************************************************************************************//**
* @brief      rxi interrupt for DTC/DMAC
* @param[in]  p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: rxi_dmac_handler */
static void rxi_dmac_handler(st_usart_resources_t * const p_usart)
{
    volatile uint16_t dummy;
    if (1 == p_usart->info->rx_status.busy)
    {
        p_usart->info->rx_status.busy = 0;
        sci_receive_stop(p_usart);
        if (NULL != p_usart->info->cb_event) // @suppress("Cast comment")
        {
            if (USART_SYNC_TX_RX_MODE == p_usart->info->tx_status.sync_mode)
            {
                /* Execution of callback function */
                p_usart->info->cb_event(ARM_USART_EVENT_TRANSFER_COMPLETE);
            }
            else
            {
                /* Execution of callback function */
                p_usart->info->cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
            }
        }
    }
    else
    {
        /* Dummy read */
        dummy = p_usart->reg->RDRHL;
    }
}/* End of function rxi_dmac_handler() */
#endif

/****************************************************************************************************************//**
* @brief        eri interrupt
* @param[in]    p_usart    Pointer to USART resources
**********************************************************************************************************************/
/* Function Name: eri_handler */
static void eri_handler(st_usart_resources_t * const p_usart) // @suppress("Function length")
{
    uint8_t reg;
    uint32_t event = 0;
    volatile uint16_t dummy;
    
    switch(p_usart->info->mode)
    {
        case USART_MODE_SYNC:
        {
            /* In the case of clock synchronization,              */
            /* transmission is temporarily disabled and           */
            /* transmission / reception processing is interrupted */
            sci_transmit_stop(p_usart);
            p_usart->info->tx_status.send_active = 0;
            p_usart->info->tx_status.tx_num      = 0;
            p_usart->info->rx_status.busy        = 0;
            sci_receive_stop(p_usart);
            reg = p_usart->reg->SCR;
            p_usart->reg->SCR &= 0x0F;
            p_usart->reg->SCR = reg;
        }
        break;
        
        case USART_MODE_SMC:
        {
            /* In the case of smart card mode, when an parity error occurs,  */
            /* cancel the error and continue transmission                    */
            if (1 == p_usart->reg->SSR_SMCI_b.ORER)
            {
                sci_receive_stop(p_usart);
                p_usart->info->rx_status.busy        = 0;
            }
        }
        break;

        /* The default case is intentionally combined.  */
        case USART_MODE_ASYNC:
        default:
        {
            sci_receive_stop(p_usart);
            p_usart->info->rx_status.busy        = 0;
        }
        break;

    }
    reg = p_usart->reg->SSR;
    
    if (0 != (reg & USART_RCVR_ERR_MASK))
    {
        if (0 != (reg & USART_SSR_ORER_MASK))
        {
            event |= ARM_USART_EVENT_RX_OVERFLOW;
            p_usart->info->rx_status.overflow = 1;
        }
        if (0 != (reg & USART_SSR_FER_MASK))
        {
            if (USART_MODE_SMC != p_usart->info->mode)
            {
                event |= ARM_USART_EVENT_RX_FRAMING_ERROR;
                p_usart->info->rx_status.framing_error = 1;
            }
        }
        if (0 != (reg & USART_SSR_PER_MASK))
        {
            event |= ARM_USART_EVENT_RX_PARITY_ERROR;
            p_usart->info->rx_status.parity_error = 1;
        }

        /* Clear error condition */
        while (0 != (p_usart->reg->SSR & USART_RCVR_ERR_MASK))
        {
            if (USART_SIZE_9 == p_usart->info->data_size)
            {
                /* Acquire value from reception 9 bit data register */
                dummy = p_usart->reg->RDRHL;
            }
            else
            {
                /* Get value from received data register */
                dummy = (uint16_t)(p_usart->reg->RDR);
            }

            reg                    = p_usart->reg->SSR;
            reg                   &= (~USART_RCVR_ERR_MASK);
            reg                   |= USART_SSR_CLR_MASK;

            /* SSR - Serial Status Register for Non-Smart Card Interface Mode and non-FIFO Mode
               b7  - TDRE - Transmit Data Empty Flag
               b6  - RDRF - Receive Data Full Flag
               b5  - ORER - Overrun Error Flag
               b4  - FER - Framing Error Flag
               b3  - PER - Parity Error Flag
               b2  - TEND - Transmit End Flag
               b1  - MPB - Multi-Processor
               b0  - MPBT - Multi-Processor Bit Transfer  */
            p_usart->reg->SSR        = reg;

            if (0 != (p_usart->reg->SSR & USART_RCVR_ERR_MASK))
            {
                /* read and Compare */
                __NOP();
            }
            sci_clear_irq_req(p_usart->eri_irq);
        }

        if ((0 != event) && (NULL != p_usart->info->cb_event)) // @suppress("Cast comment")
        {
            /* Execution of callback function */
            p_usart->info->cb_event(event);
        }
    }
}/* End of function eri_handler() */

/** SCI0 */
#if (0 != R_SCI0_ENABLE)
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver USART channel 0. */
ARM_DRIVER_USART Driver_USART0 DATA_LOCATION_PRV_DRIVER_USART = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    USART0_Initialize,
    USART0_Uninitialize,
    USART0_PowerControl,
    USART0_Send,
    USART0_Receive,
    USART0_Transfer,
    USART0_GetTxCount,
    USART0_GetRxCount,
    USART0_Control,
    USART0_GetStatus,
    USART0_SetModemControl,
    USART0_GetModemStatus
};
#endif   /* IF(0 != R_SCI0_ENABLE) */

/** SCI1 */
#if (0 != R_SCI1_ENABLE)
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver USART channel 1. */
ARM_DRIVER_USART Driver_USART1 DATA_LOCATION_PRV_DRIVER_USART = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    USART1_Initialize,
    USART1_Uninitialize,
    USART1_PowerControl,
    USART1_Send,
    USART1_Receive,
    USART1_Transfer,
    USART1_GetTxCount,
    USART1_GetRxCount,
    USART1_Control,
    USART1_GetStatus,
    USART1_SetModemControl,
    USART1_GetModemStatus
};
#endif   /* IF(0 != R_SCI1_ENABLE) */

/** SCI2 */
#if (0 != R_SCI2_ENABLE)
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver USART channel 2. */
ARM_DRIVER_USART Driver_USART2 DATA_LOCATION_PRV_DRIVER_USART = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    USART2_Initialize,
    USART2_Uninitialize,
    USART2_PowerControl,
    USART2_Send,
    USART2_Receive,
    USART2_Transfer,
    USART2_GetTxCount,
    USART2_GetRxCount,
    USART2_Control,
    USART2_GetStatus,
    USART2_SetModemControl,
    USART2_GetModemStatus
};
#endif   /* IF(0 != R_SCI1_ENABLE) */

/** SCI3 */
#if (0 != R_SCI3_ENABLE)
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver USART channel 3. */
ARM_DRIVER_USART Driver_USART3 DATA_LOCATION_PRV_DRIVER_USART = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    USART3_Initialize,
    USART3_Uninitialize,
    USART3_PowerControl,
    USART3_Send,
    USART3_Receive,
    USART3_Transfer,
    USART3_GetTxCount,
    USART3_GetRxCount,
    USART3_Control,
    USART3_GetStatus,
    USART3_SetModemControl,
    USART3_GetModemStatus
};
#endif   /* IF(0 != R_SCI3_ENABLE) */

/** SCI4 */
#if (0 != R_SCI4_ENABLE)
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver USART channel 4. */
ARM_DRIVER_USART Driver_USART4 DATA_LOCATION_PRV_DRIVER_USART = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    USART4_Initialize,
    USART4_Uninitialize,
    USART4_PowerControl,
    USART4_Send,
    USART4_Receive,
    USART4_Transfer,
    USART4_GetTxCount,
    USART4_GetRxCount,
    USART4_Control,
    USART4_GetStatus,
    USART4_SetModemControl,
    USART4_GetModemStatus
};
#endif   /* IF(0 != R_SCI4_ENABLE) */

/** SCI5 */
#if (0 != R_SCI5_ENABLE)
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver USART channel 5. */
ARM_DRIVER_USART Driver_USART5 DATA_LOCATION_PRV_DRIVER_USART = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    USART5_Initialize,
    USART5_Uninitialize,
    USART5_PowerControl,
    USART5_Send,
    USART5_Receive,
    USART5_Transfer,
    USART5_GetTxCount,
    USART5_GetRxCount,
    USART5_Control,
    USART5_GetStatus,
    USART5_SetModemControl,
    USART5_GetModemStatus
};
#endif   /* IF(0 != R_SCI5_ENABLE) */

/** SCI9 */
#if (0 != R_SCI9_ENABLE)
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver USART channel 9. */
ARM_DRIVER_USART Driver_USART9 DATA_LOCATION_PRV_DRIVER_USART = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    USART9_Initialize,
    USART9_Uninitialize,
    USART9_PowerControl,
    USART9_Send,
    USART9_Receive,
    USART9_Transfer,
    USART9_GetTxCount,
    USART9_GetRxCount,
    USART9_Control,
    USART9_GetStatus,
    USART9_SetModemControl,
    USART9_GetModemStatus
};
#endif   /* IF(0 != R_SCI9_ENABLE) */

/******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_usart)
 **********************************************************************************************************************/
 
 /* End of file (r_usart_cmsis_api.c) */


