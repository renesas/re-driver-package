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
* File Name    : r_spi_cmsis.h
* Version      : 1.20
* Description  : CMSIS-Driver for SPI.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
*         : 05.01.2021 1.20     Add spi_close_dma() function
**********************************************************************************************************************/

/******************************************************************************************************************//**
 @ingroup grp_cmsis_drv_imp
 @defgroup grp_cmsis_drv_impl_spi CMSIS-Driver SPI Implementation
 @brief The SPI driver implementation for the CMSIS-Driver SPI interface to drive the SPI peripheral on RE Family.

 @section sec_cmsis_driver_spi_summary CMSIS-Driver SPI Summary
 This is the CMSIS-Driver SPI implementation which drives the SPI peripheral on RE Family via the CMSIS-Driver SPI
 interface.
 Use the instance @ref Driver_SPI0 to acccess to SPI peripheral channel 0. Do not use ARM_SPI_XXX APIs,
 or SPI0_XXX functions directly. For instance, when accessing the SPI channel 0, use the SPI Driver Instance 
 Driver_SPI0, which is an instance of 'Access Struct' @ref ARM_DRIVER_SPI for SPI channel 0.
 Following is a code example to open SPI channel 0 and power up the SPI peripheral.
 @code
 ...

 Driver_SPI0.Initialize();                  // Initialize the SPI channel 0

 Driver_SPI0.PowerControl(ARM_POWER_FULL);  // Power up the SPI peripheral
 ...
 @endcode

 @section CMSIS_Driver_SPI_Specific_Error_Code CMSIS-Driver SPI Specific Error Code
 See ARM_SPI_ERROR_XXX in Driver_SPI.h for the detail.

 @{
 *********************************************************************************************************************/
 /***********************************************************
Includes <System Includes> , "Project Includes"
***********************************************************/
#include "RE01_256KB.h"
#include "Driver_SPI.h"
#include "Pin.h"
#include "r_spi_cfg.h"
#include "r_system_api.h"
#include "r_system_cfg.h"
#include "r_lpm_api.h"
#if (SPI_PRV_USED_DMAC_DRV != 0)

/* Using the DMAC driver */
#include "r_dmac_api.h"
#endif

#if (SPI_PRV_USED_DTC_DRV != 0)

/* Using the DTC driver */
#include "r_dtc_api.h"
#endif

/******************************************************************************
 Macro definitions
*****************************************************************************/
#ifndef R_SPI_CMSIS_H
#define R_SPI_CMSIS_H

#define R_SPI0_ENABLE                         (1)       /*!< SPI ENABLE */
#define R_SPI1_ENABLE                         (1)       /*!< SPI ENABLE */
#define SPI_FLAG_INITIALIZED                  (1 << 0)  /*!< Initialization flag */
#define SPI_FLAG_POWERED                      (1 << 1)  /*!< Started flag */
#define SPI_FLAG_CONFIGURED                   (1 << 2)  /*!< Set flag */
#define SPI_FLAG_MASTER_SEND_AVAILABLE        (1 << 3)  /*!< Master Send enable */
#define SPI_FLAG_MASTER_RECEIVE_AVAILABLE     (1 << 4)  /*!< Master Receive enable */
#define SPI_FLAG_SLAVE_SEND_AVAILABLE         (1 << 5)  /*!< Slave Send available */
#define SPI_FLAG_SLAVE_RECEIVE_AVAILABLE      (1 << 6)  /*!< Slave Receive available */

#define SPI_SPTI0_DMAC_SOURCE_ID   (0x9A)
#define SPI_SPRI0_DMAC_SOURCE_ID   (0x99)
#define SPI_SPTI1_DMAC_SOURCE_ID   (0x9F)
#define SPI_SPRI1_DMAC_SOURCE_ID   (0x9E)

/******************************************************************************
 Global Typedef definitions
*****************************************************************************/
/** @brief Transmission status */
typedef struct  /* @suppress("Source line ordering") */
{           
    uint32_t                num;                /*!< Total number of transfer */
    void                    *rx_buf;             /*!< Pointer to in data buffer */
    const void              *tx_buf;             /*!< Pointer to out data buffer */
    uint32_t                rx_cnt;             /*!< Number of data received */
    uint32_t                tx_cnt;             /*!< Number of data sent */
    uint32_t                tx_def_val;         /*!< Transmit default value */
    uint8_t                 data_bits;          /*!< Number of data bytes */
    uint8_t                 exec_state;         /*!< Transfer and/or Receive status  */
} st_spi_transfer_info_t;          

/** @brief SPI setting, structure confirmation structure */
typedef struct 
{           
    ARM_SPI_SignalEvent_t   cb_event;           /*!< Event callback */
    ARM_SPI_STATUS          status;             /*!< Status flags */
    uint32_t                mode;               /*!< Current SSP mode */
    uint32_t                bps;                /*!< Bus speed */
    uint8_t                 flags;              /*!< SPI Driver flags */
} st_spi_info_t;

/** @brief Structure that stores channel information of SPI */
typedef struct {            
    volatile SPI0_Type      *reg;               /*!< Base pointer to SPI registers */
    r_pinset_t              pin_set;            /*!< SPI pin set function */
    r_pinclr_t              pin_clr;            /*!< SPI pin clear function */
    volatile uint16_t       *ss_pin;            /*!< Pointer to SS port */
    uint8_t                 ss_pin_pos;         /*!< Pointer to SS port position */
    st_spi_info_t           *info;              /*!< SPI infomation */
    st_spi_transfer_info_t  *xfer;              /*!< SPI transfer infomation */
    e_system_mcu_lock_t     lock_id;            /*!< Module lock id */
    e_lpm_mstp_t            mstp_id;            /*!< MSTP id */
    IRQn_Type               spti_irq;           /*!< SPTI IRQ Number */
    IRQn_Type               spri_irq;           /*!< SPRI RX IRQ Number */
    IRQn_Type               spii_irq;           /*!< SPII IRQ Number */
    IRQn_Type               spei_irq;           /*!< SPEI IRQ Number */
    IRQn_Type               sptend_irq;         /*!< SPEI IRQ Number */
    uint32_t                spti_iesr_val;      /*!< SPTI IESR value */
    uint32_t                spri_iesr_val;      /*!< SPRI IESR value */
    uint32_t                spii_iesr_val;      /*!< SPII IESR value */
    uint32_t                spei_iesr_val;      /*!< SPEI IESR value */
    uint32_t                sptend_iesr_val;    /*!< SPEI IESR value */
    uint32_t                spti_priority;      /*!< SPTI priority level */
    uint32_t                spri_priority;      /*!< SPRI priority level */
    uint32_t                spii_priority;      /*!< SPII priority level */
    uint32_t                spei_priority;      /*!< SPEI priority level */
    uint32_t                sptend_priority;    /*!< SPEI priority level */
#if (SPI_PRV_USED_DMAC_DTC_DRV != 0)
    DRIVER_DMA              *tx_dma_drv;
    uint16_t                tx_dma_source;
    DRIVER_DMA              *rx_dma_drv;
    uint16_t                rx_dma_source;
#else
    void                    *tx_dma_drv;
    uint16_t                tx_dma_source;
    void                    *rx_dma_drv;
    uint16_t                rx_dma_source;
#endif
#if (SPI_PRV_USED_DTC_DRV != 0)
    st_dma_transfer_data_t  *tx_dtc_info;
    st_dma_transfer_data_t  *rx_dtc_info;
#endif
    system_int_cb_t         spti_callback;      /*!< SPTI callback function address */
    system_int_cb_t         spri_callback;      /*!< SPRI callback function address */
    system_int_cb_t         spii_callback;      /*!< SPII callback function address */
    system_int_cb_t         spei_callback;      /*!< SPEI callback function address */
    system_int_cb_t         sptend_callback;    /*!< SPEI callback function address */
} const st_spi_resources_t;

/** @brief For SPI register stored value */
typedef struct {
    int32_t                 mode;               /*!< SPI running mode */
    uint16_t                spcmd0;             /*!< SPCMD0 setting buffer */
    uint8_t                 spcr;               /*!< SPCR setting buffer */
    uint8_t                 spbr;               /*!< SPBR setting buffer */
    uint8_t                 data_bits;          /*!< Data bit size */
    uint32_t                bps;                /*!< Bitrate */
} st_spi_reg_buf_t;

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/
#if (SPI_CFG_ARM_SPI_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_GET_VERSION      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_GET_VERSION
#endif

#if (SPI_CFG_ARM_SPI_GET_CAPABILITIES == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_GET_CAPABILITIES      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_GET_CAPABILITIES
#endif

#if (SPI_CFG_ARM_SPI_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_INITIALIZE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_INITIALIZE
#endif

#if (SPI_CFG_ARM_SPI_UNINITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_UNINITIALIZE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_UNINITIALIZE
#endif

#if (SPI_CFG_ARM_SPI_POWERCONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_POWERCONTROL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_POWERCONTROL
#endif

#if (SPI_CFG_ARM_SPI_SEND == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_SEND      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_SEND
#endif

#if (SPI_CFG_ARM_SPI_RECEIVE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_RECEIVE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_RECEIVE
#endif

#if (SPI_CFG_ARM_SPI_TRANSFER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_TRANSFER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_TRANSFER
#endif

#if (SPI_CFG_ARM_SPI_GET_DATACOUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_GET_DATACOUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_GET_DATACOUNT
#endif

#if (SPI_CFG_ARM_SPI_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_CONTROL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_CONTROL
#endif

#if (SPI_CFG_ARM_SPI_GET_STATUS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_SPI_GET_STATUS      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_SPI_GET_STATUS
#endif

ARM_DRIVER_VERSION ARM_SPI_GetVersion(void) FUNC_LOCATION_ARM_SPI_GET_VERSION;
ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities(void) FUNC_LOCATION_ARM_SPI_GET_CAPABILITIES;
int32_t ARM_SPI_Initialize(ARM_SPI_SignalEvent_t cb_event, st_spi_resources_t * const p_spi) 
                           FUNC_LOCATION_ARM_SPI_INITIALIZE;
int32_t ARM_SPI_Uninitialize(st_spi_resources_t * const p_spi) FUNC_LOCATION_ARM_SPI_UNINITIALIZE;
int32_t ARM_SPI_PowerControl(ARM_POWER_STATE state, st_spi_resources_t * const p_spi) 
                             FUNC_LOCATION_ARM_SPI_POWERCONTROL;
int32_t ARM_SPI_Send(void const * const p_data, uint32_t num, st_spi_resources_t * const p_spi) 
                     FUNC_LOCATION_ARM_SPI_SEND;
int32_t ARM_SPI_Receive(void const * const p_data, uint32_t num, st_spi_resources_t * const p_spi) 
                        FUNC_LOCATION_ARM_SPI_RECEIVE;
int32_t ARM_SPI_Transfer(void const * const p_data_out, void const * const p_data_in, uint32_t num,
                         st_spi_resources_t * const p_spi) FUNC_LOCATION_ARM_SPI_TRANSFER;
uint32_t ARM_SPI_GetDataCount(st_spi_resources_t const * const p_spi) FUNC_LOCATION_ARM_SPI_GET_DATACOUNT;
int32_t ARM_SPI_Control(uint32_t control, uint32_t arg, st_spi_resources_t * const p_spi) 
                        FUNC_LOCATION_ARM_SPI_CONTROL;
ARM_SPI_STATUS ARM_SPI_GetStatus(st_spi_resources_t const * const p_spi) FUNC_LOCATION_ARM_SPI_GET_STATUS;
#if (0 != SPI_PRV_USED_DMAC_DRV)
extern DRIVER_DMA Driver_DMAC0;  // @suppress("Source line ordering")
extern DRIVER_DMA Driver_DMAC1; 
extern DRIVER_DMA Driver_DMAC2; 
extern DRIVER_DMA Driver_DMAC3; 
#endif
#if (0 != SPI_PRV_USED_DTC_DRV)
extern DRIVER_DMA Driver_DTC; 
#endif

/****************************************************************************************************************//**
 * @brief       Inline function to clear bits.
 * @param[in,out]   p_reg   Pointer to SPI register setting buffer
 * @param[in]       bit     Position of the bit to be clear.
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void spi_bit_clear_8(uint8_t * const p_reg, uint8_t bit) // @suppress("Function description comment")
{
  (*p_reg) &= (~(1 << bit));
}/* End of function spi_bit_clear_8() */

/****************************************************************************************************************//**
 * @brief       Inline function to set bits
 * @param[in,out]   p_reg      Pointer to SPI register setting buffer
 * @param[in]       bit        Position of the bit to be set.
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void spi_bit_set_8(uint8_t * const p_reg, uint8_t bit) // @suppress("Function description comment")
{
  (*p_reg) |= (1 << bit);
}/* End of function spi_bit_set_8() */

/****************************************************************************************************************//**
 * @brief       Inline function to clear bits.
 * @param[in,out]   p_reg   Pointer to SPI register setting buffer
 * @param[in]       bit     Position of the bit to be clear.
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void spi_bit_clear_16(uint16_t * const p_reg, uint8_t bit) // @suppress("Function description comment")
{
  (*p_reg) &= (~(1 << bit));
}/* End of function spi_bit_clear_16() */

/****************************************************************************************************************//**
 * @brief       Inline function to set bits
 * @param[in,out]   p_reg      Pointer to SPI register setting buffer
 * @param[in]       bit        Position of the bit to be set.
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void spi_bit_set_16(uint16_t * const p_reg, uint8_t bit) // @suppress("Function description comment")
{
  (*p_reg) |= (1 << bit);
}/* End of function spi_bit_set_16() */

#if (0 != SPI_PRV_USED_DMAC_DTC_DRV)
/****************************************************************************************************************//**
 * @param[in]   drv     Pointer to DMA Driver
 * @param[in]   irq     DMA factors to be closed
 * @brief       DMA driver closed
 *******************************************************************************************************************/
/* Function Name : spi_close_dma */
__STATIC_FORCEINLINE void spi_close_dma(DRIVER_DMA* drv, uint16_t irq)
{
    volatile uint64_t act_src = 0;
    if (NULL != drv) // @suppress("Cast comment")
    {
#if (0 != SPI_PRV_USED_DTC_DRV)
        if ((&Driver_DTC) == drv)
        {
            /* DTC Release */
            (void)drv->Release(irq); // @suppress("Cast comment")
            (void)drv->GetAvailabilitySrc((uint64_t*)&act_src); // @suppress("Cast comment")
        }
#endif
        if (0 == act_src)
        {
            (void)drv->Close(); // @suppress("Cast comment")
        }
    }
}/* End of function spi_close_dma */
#endif

#endif /* R_SPI_CMSIS_H */

/******************************************************************************************************************//**
 * @} (end defgroup grp_cmsis_drv_impl_spi)
 *********************************************************************************************************************/
/* End of file (r_spi_cmsis.h) */
