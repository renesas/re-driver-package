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
* File Name    : r_spi_cmsis.c
* Version      : 1.20
* Description  : CMSIS-Driver for SPI.
**********************************************************************************************************************/
/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 02.27.2020 0.80     Fixed bug where MSB/LSB settings were reversed.
*                               Upgrade version to 0.80
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
*         : 05.01.2021 1.20     Fixed DTC Close processing when using DTC.
*                               If there are other DTC factors that can be used, close will be skipped.
**********************************************************************************************************************/

/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_spi
 * @{
 *********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <r_spi_cmsis_api.h>
/******************************************************************************
 Macro definitions
*****************************************************************************/
#define SPI_PRV_SPCMD0_SPB_OFFSET       (8)         /*!< SPCMD0 SPB offset */
#define SPI_PRV_SPCMD0_SPB_CLR_MASK     (0xF0FF)    /*!< SPCMD0 SPB clear mask */
#define SPI_PRV_SPCMD0_SPB_20BIT        (0x0000)    /*!< Data Length : 20bit */
#define SPI_PRV_SPCMD0_SPB_24BIT        (0x0100)    /*!< Data Length : 24bit */
#define SPI_PRV_SPCMD0_SPB_32BIT        (0x0200)    /*!< Data Length : 32bit */
#define SPI_PRV_SPCMD0_SPB_8BIT         (0x0400)    /*!< Data Length : 8bit */

#define SPI_PRV_EXEC_SEND               (0x00)      /*!< EXEC state : Send */
#define SPI_PRV_EXEC_RECEIVE            (0x01)      /*!< EXEC state : Receive */
#define SPI_PRV_EXEC_TRANSFER           (0x02)      /*!< EXEC state : Transfer */

#define SPI_PRV_MASK_BRDV               (0xFFF3)    /*!< SPCMD0 BRDV mask */

#define ARM_SPI_PRV_DRV_VERSION         (ARM_DRIVER_VERSION_MAJOR_MINOR(1, 20)) /*!< driver version */
#define SPI_PRV_BASE_BIT_MASK           (0xFFFFFFFE)

/******************************************************************************
 Private global variables and functions
*****************************************************************************/
#if (SPI_CFG_ARM_SPI_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_VERSION             __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_VERSION
#endif

#if (SPI_CFG_ARM_SPI_GET_CAPABILITIES == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_CAPABILITIES        __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_CAPABILITIES
#endif

#if (SPI_CFG_ARM_SPI_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_SPI_BRDV_TBL        __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_SPI_BRDV_TBL
#endif

#if (SPI_CFG_ARM_SPI_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_MASTER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_MASTER
#endif

#if (SPI_CFG_ARM_SPI_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_SLAVE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_SLAVE
#endif

#if (SPI_CFG_ARM_SPI_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_COMMON      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_COMMON
#endif

#if (SPI_CFG_ARM_SPI_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_RSPI_BAUD_SET      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_RSPI_BAUD_SET
#endif

#if (SPI_CFG_ARM_SPI_UNINITIALIZE == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_CONTROL      == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_REGS_CLEAR      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_SET_REGS_CLEAR
#endif

#if (SPI_CFG_ARM_SPI_UNINITIALIZE == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_CONTROL      == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_INIT_REGISTER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_INIT_REGISTER
#endif

#if (SPI_CFG_R_SPRI_HANDLER == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_R_SPII_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_R_SPRI_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_R_SPRI_HANDLER
#endif

#if (SPI_CFG_R_SPTI_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_R_SPTI_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_R_SPTI_HANDLER
#endif

#if (SPI_CFG_R_SPII_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_R_SPII_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_R_SPII_HANDLER
#endif

#if (SPI_CFG_R_SPEI_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_R_SPEI_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_R_SPEI_HANDLER
#endif

#if (SPI_CFG_R_SPTEND_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_R_SPTEND_HANDLER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_R_SPTEND_HANDLER
#endif

#if (SPI_CFG_R_SPEI_HANDLER       == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_R_SPRI_HANDLER       == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_R_SPII_HANDLER       == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_R_SPTEND_HANDLER     == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_UNINITIALIZE == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_CONTROL      == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_POWERCONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_INTERRUPT_DISABLE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_INTERRUPT_DISABLE
#endif
#if (SPI_CFG_ARM_SPI_SEND == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_RECEIVE == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_TRANSFER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_IR_FLAG_CLEAR      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_IR_FLAG_CLEAR
#endif     
#if (SPI_CFG_ARM_SPI_INITIALIZE   == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_UNINITIALIZE == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_POWERCONTROL == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_SEND         == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_RECEIVE      == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_TRANSFER     == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_GET_DATACOUNT == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_CONTROL      == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_GET_STATUS    == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_R_SPRI_HANDLER       == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_R_SPTI_HANDLER       == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_R_SPII_HANDLER       == SYSTEM_SECTION_RAM_FUNC ) || \
    (SPI_CFG_R_SPEI_HANDLER       == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_SPI_RESOURCES        __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_SPI_RESOURCES
#endif

#if (SPI_CFG_ARM_SPI_INITIALIZE       == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_UNINITIALIZE     == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_POWERCONTROL     == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_SEND             == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_RECEIVE          == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_TRANSFER         == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_GET_DATACOUNT    == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_CONTROL          == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_ARM_SPI_GET_STATUS       == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_GET_VERSION      == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_GET_CAPABILITIES == SYSTEM_SECTION_RAM_FUNC)    
#define DATA_LOCATION_PRV_DRIVER_SPI        __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_SPI
#endif

#if (SPI_CFG_ARM_SPI_INITIALIZE   == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_CHECK_TX_AVAILABLE       __attribute__ ((section(".ramfunc"))) // @suppress("Macro expansion")
#else
#define STATIC_FUNC_LOCATION_PRV_CHECK_TX_AVAILABLE
#endif

#if (SPI_CFG_ARM_SPI_INITIALIZE   == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_CHECK_RX_AVAILABLE       __attribute__ ((section(".ramfunc"))) // @suppress("Macro expansion")
#else
#define STATIC_FUNC_LOCATION_PRV_CHECK_RX_AVAILABLE
#endif

#if (SPI_CFG_ARM_SPI_CONTROL  == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_R_SPRI_HANDLER   == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_R_SPII_HANDLER   == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_R_SPEI_HANDLER   == SYSTEM_SECTION_RAM_FUNC)  || \
    (SPI_CFG_R_SPTEND_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_TRANSMIT_STOP       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_TRANSMIT_STOP
#endif
      
#if (SPI_CFG_ARM_SPI_SEND         == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_RECEIVE      == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_TRANSFER     == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_SEND_SETTING        __attribute__ ((section(".ramfunc"))) // @suppress("Macro expansion")
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_SEND_SETTING
#endif

#if (SPI_CFG_ARM_SPI_RECEIVE      == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_TRANSFER     == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_SPI_RECEIVE_SETTING     __attribute__ ((section(".ramfunc"))) // @suppress("Macro expansion")
#else
#define STATIC_FUNC_LOCATION_PRV_SPI_RECEIVE_SETTING
#endif

#if (SPI_CFG_ARM_SPI_SEND         == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_RECEIVE      == SYSTEM_SECTION_RAM_FUNC) || \
    (SPI_CFG_ARM_SPI_TRANSFER     == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_DMA_CONFIG_INIT         __attribute__ ((section(".ramfunc"))) // @suppress("Macro expansion")
#else
#define STATIC_FUNC_LOCATION_PRV_DMA_CONFIG_INIT
#endif

static int32_t spi_set_init_master(uint32_t control, uint32_t baudrate, st_spi_reg_buf_t * const p_spi_regs) 
                                   STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_MASTER;
static int32_t spi_set_init_slave(uint32_t control, st_spi_reg_buf_t * const p_spi_regs) 
                                  STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_SLAVE;
static int32_t spi_set_init_common(uint32_t control, st_spi_reg_buf_t * const p_spi_regs)
                            STATIC_FUNC_LOCATION_PRV_SPI_SET_INIT_COMMON;
static int32_t rspi_baud_set(st_spi_reg_buf_t * const p_spi_regs, uint32_t bps_target) 
                             STATIC_FUNC_LOCATION_PRV_RSPI_BAUD_SET;
static void spi_set_regs_clear(st_spi_reg_buf_t * const p_regs) STATIC_FUNC_LOCATION_PRV_SPI_SET_REGS_CLEAR;
static void spi_init_register(st_spi_reg_buf_t const * const p_spi_regs, st_spi_resources_t * const p_spi) 
                              STATIC_FUNC_LOCATION_PRV_SPI_INIT_REGISTER;
static void r_spri_handler(st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_R_SPRI_HANDLER;
static void r_spti_handler(st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_R_SPTI_HANDLER;
static void r_spii_handler(st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_R_SPII_HANDLER;
static void r_spei_handler(st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_R_SPEI_HANDLER;
static void r_sptend_handler(st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_R_SPTEND_HANDLER;
static void spi_interrupt_disable(st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_SPI_INTERRUPT_DISABLE;
static int32_t spi_ir_flag_clear(st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_SPI_IR_FLAG_CLEAR;

static int32_t check_tx_available(int16_t * const p_flag, st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_CHECK_TX_AVAILABLE;
static int32_t check_rx_available(int16_t * const p_flag, st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_CHECK_RX_AVAILABLE;
static void spi_transmit_stop(st_spi_resources_t const * const p_spi) STATIC_FUNC_LOCATION_PRV_SPI_TRANSMIT_STOP;
static int32_t spi_send_setting(void const * const p_data, uint32_t num, bool dummy_flag, st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_SPI_SEND_SETTING;
static int32_t spi_receive_setting(void const * const p_data, uint32_t num, st_spi_resources_t * const p_spi) STATIC_FUNC_LOCATION_PRV_SPI_RECEIVE_SETTING;

#if (0 != SPI_PRV_USED_DMAC_DTC_DRV)
static void dma_config_init(st_dma_transfer_data_cfg_t *p_cfg) STATIC_FUNC_LOCATION_PRV_DMA_CONFIG_INIT;
#endif

/** Driver Version */
static const ARM_DRIVER_VERSION gs_driver_version DATA_LOCATION_PRV_DRIVER_VERSION =
{
    ARM_SPI_API_VERSION,
    ARM_SPI_PRV_DRV_VERSION
};

/** Driver Capabilities */
static const ARM_SPI_CAPABILITIES gs_driver_capabilities DATA_LOCATION_PRV_DRIVER_CAPABILITIES = {
    0, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface */
    0  /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};

/*                                                           No-Div,  Div-2,  Div-4,  Div-8 */
static const uint16_t gs_spi_brdv_tbl[4] DATA_LOCATION_PRV_SPI_BRDV_TBL = {0x0000, 0x0004, 0x0008, 0x000C};

/****************************************************************************************************************//**
 * @brief The implementation of CMSIS-Driver SPI Get Version API for the SPI peripheral on RE Family.
 * @retval Driver version number
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_GetVersion */
ARM_DRIVER_VERSION ARM_SPI_GetVersion(void) //@suppress("Source line ordering")  @suppress("Non-API function naming")
{
 return (gs_driver_version);
}/* End of function ARM_SPI_GetVersion() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver SPI Get Capabilities API for the SPI peripheral on RE Family.
 * @retval      ARM_SPI_CAPABILITIES @ref ARM_SPI_CAPABILITIES
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_GetCapabilities */
ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities(void) //@suppress("Non-API function naming")
{
  return (gs_driver_capabilities);
}/* End of function ARM_SPI_GetCapabilities() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver SPI Initialization API for the SPI peripheral on RE Family.
 * @param[in]        cb_event  Pointer to \ref ARM_SPI_SignalEvent
 * @param[in,out]    p_spi     Pointer to SPI resource data
 * @retval      ARM_DRIVER_OK  Initialization was successful.
 * @retval      ARM_DRIVER_ERROR  Specified SPI mode is not supported.
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_Initialize */
int32_t ARM_SPI_Initialize(ARM_SPI_SignalEvent_t cb_event, st_spi_resources_t * const p_spi) // @suppress("Non-API function naming")
{
    int32_t result    = ARM_DRIVER_OK;
    int16_t init_flag = SPI_FLAG_INITIALIZED;
    if (0x00 != (p_spi->info->flags & SPI_FLAG_INITIALIZED))
    {
        /* Target spi is already initialized */
        return (ARM_DRIVER_OK);
    }
    
    result = check_tx_available(&init_flag, p_spi);
    if (ARM_DRIVER_OK == result)
    {
        result = check_rx_available(&init_flag, p_spi);
    }
    if (SPI_FLAG_INITIALIZED == init_flag)
    {
        result = ARM_DRIVER_ERROR;
    }
    
    /* Lock SPI resource */
    if (ARM_DRIVER_OK == result)
    {
        if (0 != R_SYS_ResourceLock(p_spi->lock_id))
        {
            result = ARM_DRIVER_ERROR;
        }
    }
    
    if (ARM_DRIVER_OK == result)
    {
    /* Set callback function */
        p_spi->info->cb_event  = cb_event;
        
        /* Clear SPI status */
        p_spi->info->status.busy          = 0;
        p_spi->info->status.data_lost     = 0;
        p_spi->info->status.mode_fault    = 0;
        p_spi->xfer->exec_state           = SPI_PRV_EXEC_RECEIVE;
        p_spi->xfer->tx_cnt               = 0;
        p_spi->xfer->rx_cnt               = 0;
        
        /* Clear TX status */
        p_spi->xfer->tx_def_val = 0xFFFFFFFF;
        
        /* SPI is initialized */
        p_spi->info->flags                = init_flag;
    }
    else
    {
#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        /* Close tx dma driver */
        spi_close_dma(p_spi->tx_dma_drv, p_spi->tx_dma_source);
#endif
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        /* Close rx dma driver */
        spi_close_dma(p_spi->rx_dma_drv, p_spi->rx_dma_source);
#endif
    }

    return (result);
}/* End of function ARM_SPI_Initialize() */


/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver SPI Uninitialization API for the SPI peripheral on RE Family.
 * @param[in]   p_spi        Pointer to SPI resource data
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
 * @retval      ARM_DRIVER_ERROR    Specified SPI mode is not supported.
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_Uninitialize */
int32_t ARM_SPI_Uninitialize(st_spi_resources_t * const p_spi) // @suppress("Non-API function naming")
{
    st_spi_reg_buf_t p_spi_regs; // @suppress("Local variable naming")

    if (0x00 == (p_spi->info->flags & SPI_FLAG_INITIALIZED))
    {
        /* Target spi is not initialized */
        return (ARM_DRIVER_OK);
    }
    
    if (0 == (p_spi->info->flags & SPI_FLAG_POWERED))
    {
            /* Release module stop */
          if (0 != R_LPM_ModuleStart(p_spi->mstp_id))
          {
              return (ARM_DRIVER_ERROR);
          }
    }

    spi_set_regs_clear(&p_spi_regs);          /* Initialize the buffer for register setting */
    spi_interrupt_disable(p_spi);
    spi_init_register(&p_spi_regs, p_spi);    /* Reset register */
    p_spi->info->status.busy = 0U;
    p_spi->pin_clr();                         /* Clear pin */
    p_spi->info->flags = 0;
    
    (void) R_LPM_ModuleStop(p_spi->mstp_id); // @suppress("Cast comment")

#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        /* Close tx dma driver */
        spi_close_dma(p_spi->tx_dma_drv, p_spi->tx_dma_source);
#endif
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        /* Close rx dma driver */
        spi_close_dma(p_spi->rx_dma_drv, p_spi->rx_dma_source);
#endif

    /* Unlock SPI resource */
    R_SYS_ResourceUnlock(p_spi->lock_id);

    return (ARM_DRIVER_OK);
}/* End of function ARM_SPI_Uninitialize() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver SPI Power Control API for the SPI peripheral on RE Family.
 * @param[in]   state  ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock)
 * @param[in]   p_spi  Pointer to SPI resource data
 * @retval      ARM_DRIVER_OK                Power control operation was successful.
 * @retval      ARM_DRIVER_ERROR             Specified SPI mode is not supported.
 * @retval      ARM_DRIVER_ERROR_UNSUPPORTED Power mode specified is not supported.
 * @note Power state ARM_POWER_LOW is not supported.
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_PowerControl */
int32_t ARM_SPI_PowerControl(ARM_POWER_STATE state, st_spi_resources_t * const p_spi) // @suppress("Non-API function naming")
{
    int32_t result = ARM_DRIVER_OK;
    switch (state)
    {
        case ARM_POWER_OFF:
        {
            if (0 == (p_spi->info->flags & SPI_FLAG_INITIALIZED))
            {
                result = ARM_DRIVER_ERROR;
                break;
            }

            spi_interrupt_disable(p_spi);
            p_spi->info->status.busy = 0U;
            p_spi->pin_clr();                         /* Clear pin */

            /* Set module stop */
            (void) R_LPM_ModuleStop(p_spi->mstp_id);
            
            p_spi->info->flags &= (~SPI_FLAG_POWERED);
        }
        break;
    
        case ARM_POWER_FULL:
        {
            if (0 == (p_spi->info->flags & SPI_FLAG_INITIALIZED))
            {
                result = ARM_DRIVER_ERROR;
                break;
            }
            if (1 == (p_spi->info->flags & SPI_FLAG_POWERED))
            {
                break;
            }

            /* Release module stop */
            if ( 0 != R_LPM_ModuleStart(p_spi->mstp_id))
            {
                result = ARM_DRIVER_ERROR;
                break;
            }
            p_spi->info->flags |= SPI_FLAG_POWERED;
        }
        break;
        case ARM_POWER_LOW:
        default:
        {
            return (ARM_DRIVER_ERROR_UNSUPPORTED);
        }
        break;
    }
    
    return (result);
    
}/* End of function ARM_SPI_PowerControl() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver SPI Data Transmission API for the SPI peripheral on RE Family.
 * @param[in]        p_data        Pointer to output data
 * @param[in]        num           Number of data to transmit
 * @param[in,out]    p_spi         Pointer to SPI resource data
 * @retval      ARM_DRIVER_OK    Data transmission was successful.
 * @retval      ARM_DRIVER_ERROR Specified SPI mode is not supported.
 * @retval      ARM_DRIVER_ERROR_PARAMETER An invalid parameter is specified.
 * @note        In master operation, this function can not be used without the setting below.
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPII
 *               In slave operation, this function can not be used without the setting below.
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPTEND
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_Send */
int32_t ARM_SPI_Send(void const * const p_data, uint32_t num, st_spi_resources_t * const p_spi) //@suppress("Function length") @suppress("Non-API function naming")
{
    volatile uint8_t dummy;
    int32_t result = ARM_DRIVER_OK;

    /* Setting value error */
    if ((NULL == p_data) || (0U == num))
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }

    /* Power supply confirmation */
    if (0U == (p_spi->info->flags & SPI_FLAG_POWERED))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Setting incomplete */
    if (0U == (SPI_FLAG_CONFIGURED & p_spi->info->flags))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Busy state */
    if (1U == p_spi->info->status.busy)
    {
        return (ARM_DRIVER_ERROR_BUSY);
    }
    if (ARM_SPI_MODE_MASTER == (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
    {
        /* When operating in master mode, the SPI_FLAG_MASTER_AVAILABLE flag is required */
        if (0U == (p_spi->info->flags & SPI_FLAG_MASTER_SEND_AVAILABLE))
        {
            /* If a MasterSend can not be used, an error is returned */
            return (ARM_DRIVER_ERROR);
        }
    }
    else
    {
        /* When operating in slave mode, the SPI_FLAG_SLAVE_SEND_AVAILABLE flag is required */
        if (0U == (p_spi->info->flags & SPI_FLAG_SLAVE_SEND_AVAILABLE))
        {
            /* If a SlaveSend can not be used, an error is returned */
            return (ARM_DRIVER_ERROR);
        }
    }
    
    result = spi_ir_flag_clear(p_spi);
    
    if (ARM_DRIVER_OK == result)
    {
        result = spi_send_setting(p_data, num, false,  p_spi);
    }
    
    if (ARM_DRIVER_OK == result)
    {
    
        /* Sending / receiving flag set */
        p_spi->info->status.busy = 1U;
        
        /* Clear error flag */
        p_spi->info->status.data_lost = 0U;
        p_spi->info->status.mode_fault = 0U;
        
        /* Initialize transmission / reception information */
        p_spi->xfer->rx_buf = NULL;
        p_spi->xfer->tx_buf = (uint8_t *)p_data; // @suppress("Cast comment")
        p_spi->xfer->rx_cnt     = 0U;
        p_spi->xfer->tx_cnt     = 0U;
        p_spi->xfer->exec_state = SPI_PRV_EXEC_SEND;
        p_spi->xfer->num    = num;

        /* Set serial communication only for transmission */
        dummy = p_spi->reg->SPSR;                     /* dummy read */
        
        /* SPSR - SPI Status Register
            b4  - UDRF - Underrun Error Flag - Neither a mode fault error nor an underrun error occurs
            b3  - PERF - Parity Error Flag - No parity error occurs
            b2  - MODF - Mode Fault Error Flag - Neither a mode fault error nor an underrun error occurs
            b1  - IDLNF - SPI Idle Flag - SPI is in the idle state
            b0  - OVRF - Overrun Error Flag - No overrun error occurs */
        p_spi->reg->SPSR = 0xA0;                      /* Clear OVRF,IDLNF,MODF,PERF,UDRF */
        
        /* Clear interrupt request */
        /* SPIIE - SPI Idle Interrupt Enable - Disables the generation of idle interrupt requests */
        p_spi->reg->SPCR2_b.SPIIE = 0;                /* Clear SPIIE */
        
        if (ARM_SPI_MODE_SLAVE == (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
        {
               /* SPCR - SPI Control Register
               b6   - SPE - SPI Function Enable - Enables the SPI function
               b5   - SPTIE - Transmit Buffer Empty Interrupt Enable
               b4   - SPEIE - SPI Error Interrupt Enable - Enables the generation of SPI error interrupt requests
                    - Enables the generation of transmit buffer empty interrupt requests
               b1   - TXMD - Transmit operation mode select - Select Send only serial communication  */
            p_spi->reg->SPCR |= 0x72;                     /* Set SPE,SPTIE */
            R_NVIC_EnableIRQ(p_spi->spei_irq);
        }
        else
        {
            /* SPCR - SPI Control Register
               b6   - SPE - SPI Function Enable - Enables the SPI function
               b5   - SPTIE - Transmit Buffer Empty Interrupt Enable
                    - Enables the generation of transmit buffer empty interrupt requests
               b1   - TXMD - Transmit operation mode select - Select Send only serial communication  */
            p_spi->reg->SPCR |= 0x62;                     /* Set SPE,SPTIE */
        }

        if ((NULL == p_spi->tx_dma_drv)// @suppress("Cast comment")
#if (0 != SPI_PRV_USED_TX_DTC_DRV)
         || ((&Driver_DTC) == p_spi->tx_dma_drv)
#endif
           )
        {
            /* When using interrupt or DTC. */
            R_NVIC_EnableIRQ(p_spi->spti_irq);
        }
    }

#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
    else
    {
        /* Close tx dma driver */
        spi_close_dma(p_spi->tx_dma_drv, p_spi->tx_dma_source);
    }
#endif
    return (result);
}/* End of function ARM_SPI_Send() */

/****************************************************************************************************************//**
 *  @brief       The implementation of CMSIS-Driver SPI Data Reception API for the SPI peripheral on RE Family.
 *  @param[in]        p_data       Pointer to input data
 *  @param[in]        num          Number of data to receive
 *  @param[in,out]    p_spi        Pointer to SPI resource data
 *  @retval      ARM_DRIVER_OK     Data reception was successful.
 *  @retval      ARM_DRIVER_ERROR  Specified SPI mode is not supported.
 *  @retval      ARM_DRIVER_ERROR_PARAMETER An illegal parameter is specified.
 *  @note        In master operation, this function can not be used without the setting below.
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPRI
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPII
 *               In slave operation, this function can not be used without the setting below.
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPRI
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_Receive */
int32_t ARM_SPI_Receive(void const * const p_data, uint32_t num, st_spi_resources_t * const p_spi) // @suppress("Function length") @suppress("Non-API function naming")
{
    volatile uint8_t dummy;
    int32_t result = ARM_DRIVER_OK;

    /* Setting value error */
    if ((NULL == p_data) || (0U == num))
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }

    /* Power supply confirmation */
    if (0U == (p_spi->info->flags & SPI_FLAG_POWERED))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Setting incomplete */
    if (0U == (SPI_FLAG_CONFIGURED & p_spi->info->flags))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Busy state */
    if (1U == p_spi->info->status.busy)
    {
        return (ARM_DRIVER_ERROR_BUSY);
    }

    if (ARM_SPI_MODE_MASTER == (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
    {
        if (0U == (p_spi->info->flags & SPI_FLAG_MASTER_RECEIVE_AVAILABLE))
        {
            /* If a MasterReceive can not be used, an error is returned */
            return (ARM_DRIVER_ERROR);
        }
    }
    else
    {
        if (0U == (p_spi->info->flags & SPI_FLAG_SLAVE_RECEIVE_AVAILABLE))
        {
            /* If a SlaveReceive can not be used, an error is returned */
            return (ARM_DRIVER_ERROR);
        }
    }
    
    result = spi_ir_flag_clear(p_spi);
    if (ARM_DRIVER_OK == result)
    {
        /* When receiving, the transmission operation also operates at the same time. */
        result = spi_send_setting(&(p_spi->xfer->tx_def_val), num, true, p_spi);
        if (ARM_DRIVER_OK == result)
        {
            result = spi_receive_setting(p_data, num, p_spi);
        }
    }
    
    if (ARM_DRIVER_OK == result)
    {
        /* Sending / receiving flag set */
        p_spi->info->status.busy = 1U;
        
        /* Clear error flag */
        p_spi->info->status.data_lost  = 0U;
        p_spi->info->status.mode_fault = 0U;
        
        /* Initialize transmission / reception information */
        p_spi->xfer->rx_buf = (uint8_t *)p_data;
        p_spi->xfer->tx_buf = NULL; // @suppress("Cast comment")
        p_spi->xfer->rx_cnt     = 0U;
        p_spi->xfer->tx_cnt     = 0U;
        p_spi->xfer->exec_state = SPI_PRV_EXEC_RECEIVE;
        p_spi->xfer->num    = num;

        dummy = p_spi->reg->SPSR;                     /* dummy read */
        
        /* SPSR - SPI Status Register
            b4  - UDRF - Underrun Error Flag - Neither a mode fault error nor an underrun error occurs
            b3  - PERF - Parity Error Flag - No parity error occurs
            b2  - MODF - Mode Fault Error Flag - Neither a mode fault error nor an underrun error occurs
            b1  - IDLNF - SPI Idle Flag - SPI is in the idle state
            b0  - OVRF - Overrun Error Flag - No overrun error occurs */
        p_spi->reg->SPSR = 0xA0;                      /* Clear OVRF,IDLNF,MODF,PERF,UDRF */
        
        /* Clear interrupt request */
        /* SPIIE - SPI Idle Interrupt Enable - Disables the generation of idle interrupt requests */
        p_spi->reg->SPCR2_b.SPIIE = 0;                /* Clear SPIIE */
        
        /* SPCR - SPI Control Register
           b7   - SPRIE - SPI Receive Buffer Full Interrupt Enable
                        - Enables the generation of SPI receive buffer full interrupt requests
           b6   - SPE - SPI Function Enable - Enables the SPI function
           b5   - SPTIE - Transmit Buffer Empty Interrupt Enable
                        - Enables the generation of transmit buffer empty interrupt requests 
           b4   - SPEIE - SPI Error Interrupt Enable - Enables the generation of SPI error interrupt requests
           b1   - TXMD - Transmit operation mode select - Select full duplex synchronous serial communication */
        p_spi->reg->SPCR_b.TXMD = 0;
        p_spi->reg->SPCR |= 0xF0;                     /* Set SPE,SPEIE,SPTIE,SPRIE */
        

        /* Receive operation */
        if ((NULL == p_spi->rx_dma_drv)
#if (0 != SPI_PRV_USED_RX_DTC_DRV)
         || ((&Driver_DTC) == p_spi->rx_dma_drv) 
#endif
        ) // @suppress("Cast comment")
        {
            /* When using interrupt or DTC. */
            R_NVIC_EnableIRQ(p_spi->spri_irq);
        }
        
        /* Send operation */
        if ((NULL == p_spi->tx_dma_drv)

#if (0 != SPI_PRV_USED_TX_DTC_DRV)
         || ((&Driver_DTC) == p_spi->tx_dma_drv)
#endif
           ) // @suppress("Cast comment")
        {
            /* When using interrupt or DTC. */
            R_NVIC_EnableIRQ(p_spi->spti_irq);
        }
        R_NVIC_EnableIRQ(p_spi->spei_irq);
    }
    else
    {
#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        /* Close tx dma driver */
        spi_close_dma(p_spi->tx_dma_drv, p_spi->tx_dma_source);
#endif
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        /* Close rx dma driver */
        spi_close_dma(p_spi->rx_dma_drv, p_spi->rx_dma_source);
#endif
    }

    return (result);
}/* End of function ARM_SPI_Receive() */

/****************************************************************************************************************//**
 *  @brief       The implementation of CMSIS-Driver SPI Data Transfer API for the SPI peripheral on RE Family.
 *  @param[in]        p_data_out     Pointer to output data
 *  @param[in]        p_data_in      Pointer to iunput data
 *  @param[in]        num            Number of data to transfer
 *  @param[in,out]    p_spi          Pointer to SPI resource data
 *  @retval      ARM_DRIVER_OK     Data transfer was successful.
 *  @retval      ARM_DRIVER_ERROR  Specified SPI mode is not supported.
 *  @retval      ARM_DRIVER_ERROR_PARAMETER Illegal parameter is specified.
 *  @note        In master operation, this function can not be used without the setting below.
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPRI
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPII
 *               In slave operation, this function can not be used without the setting below.
 *               - SYSTEM_CFG_EVENT_NUMBER_SPIn_SPRI
 *******************************************************************************************************************/
/* Function Name: ARM_SPI_Transfer */
int32_t ARM_SPI_Transfer(void const * const p_data_out, void const * const p_data_in, uint32_t num, st_spi_resources_t * const p_spi) // @suppress("Function length") @suppress("Non-API function naming")
{
    volatile uint8_t dummy;
    int32_t result = ARM_DRIVER_OK;

    /* Setting value error */
    if ((NULL == p_data_in) || (NULL == p_data_out) || (0U == num))
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }

    /* Power supply confirmation */
    if (0U == (p_spi->info->flags & SPI_FLAG_POWERED))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Setting incomplete */
    if (0U == (SPI_FLAG_CONFIGURED & p_spi->info->flags))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Busy state */
    if (1U == p_spi->info->status.busy)
    {
        return (ARM_DRIVER_ERROR_BUSY);
    }
    if (ARM_SPI_MODE_MASTER == (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
    {
        if (0U == (p_spi->info->flags & SPI_FLAG_MASTER_RECEIVE_AVAILABLE))
        {
            /* If a MasterReceive can not be used, an error is returned */
            return (ARM_DRIVER_ERROR);
        }
    }
    else
    {
        if (0U == (p_spi->info->flags & SPI_FLAG_SLAVE_RECEIVE_AVAILABLE))
        {
            /* If a SlaveReceve can not be used, an error is returned */
            return (ARM_DRIVER_ERROR);
        }
    }
    
    result = spi_ir_flag_clear(p_spi);
    if (ARM_DRIVER_OK == result)
    {
        /* When receiving, the transmission operation also operates at the same time. */
        result = spi_send_setting(p_data_out, num, false, p_spi);
        if (ARM_DRIVER_OK == result)
        {
            result = spi_receive_setting(p_data_in, num, p_spi);
        }
    }
    
    if (ARM_DRIVER_OK == result)
    {
        /* Sending / receiving flag set */
        p_spi->info->status.busy = 1U;
        
        /* Clear error flag */
        p_spi->info->status.data_lost  = 0U;
        p_spi->info->status.mode_fault = 0U;
        
        /* Initialize transmission / reception information */
        p_spi->xfer->rx_buf = (uint8_t *)p_data_in;
        p_spi->xfer->tx_buf = (uint8_t *)p_data_out; // @suppress("Cast comment")
        p_spi->xfer->rx_cnt = 0U;
        p_spi->xfer->tx_cnt = 0U;
        p_spi->xfer->exec_state = SPI_PRV_EXEC_TRANSFER;
        p_spi->xfer->num    = num;
        
        /* When transmission / reception is completed */
        dummy = p_spi->reg->SPSR;                     /* dummy read */
        
        /* SPSR - SPI Status Register
            b4  - UDRF - Underrun Error Flag - Neither a mode fault error nor an underrun error occurs
            b3  - PERF - Parity Error Flag - No parity error occurs
            b2  - MODF - Mode Fault Error Flag - Neither a mode fault error nor an underrun error occurs
            b1  - IDLNF - SPI Idle Flag - SPI is in the idle state
            b0  - OVRF - Overrun Error Flag - No overrun error occurs */
        p_spi->reg->SPSR = 0xA0;                      /* Clear OVRF,IDLNF,MODF,PERF,UDRF */
        
        /* SPIIE - SPI Idle Interrupt Enable - Disables the generation of idle interrupt requests */
        p_spi->reg->SPCR2_b.SPIIE = 0;                /* Clear SPIIE */
        
        /* TXMD - Transmit operation mode select - Select full duplex synchronous serial communication */
        p_spi->reg->SPCR_b.TXMD = 0;
        
        /* Clear interrupt request */
        /* SPCR - SPI Control Register
           b7   - SPRIE - SPI Receive Buffer Full Interrupt Enable
                        - Enables the generation of SPI receive buffer full interrupt requests
           b6   - SPE - SPI Function Enable - Enables the SPI function
           b5   - SPTIE - Transmit Buffer Empty Interrupt Enable
                        - Enables the generation of transmit buffer empty interrupt requests 
           b4   - SPEIE - SPI Error Interrupt Enable - Enables the generation of SPI error interrupt requests*/
        p_spi->reg->SPCR |= 0xF0;                     /* Set SPE,SPEIE,SPTIE,SPRIE */

        /* Receive operation */
        if ((NULL == p_spi->rx_dma_drv)
#if (0 != SPI_PRV_USED_RX_DTC_DRV)
         || ((&Driver_DTC) == p_spi->rx_dma_drv)
#endif
              ) // @suppress("Cast comment")
        {
            /* Receive Start */
            R_NVIC_EnableIRQ(p_spi->spri_irq);
            
        }
        
        /* Send operation */
        if ((NULL == p_spi->tx_dma_drv)
#if (0 != SPI_PRV_USED_TX_DTC_DRV)
         || ((&Driver_DTC) == p_spi->tx_dma_drv)
#endif
              ) // @suppress("Cast comment")
        {
            /* Receive Start */
            R_NVIC_EnableIRQ(p_spi->spti_irq);
        }
        R_NVIC_EnableIRQ(p_spi->spei_irq);
    }
    else
    {
#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        /* Close tx dma driver */
        spi_close_dma(p_spi->tx_dma_drv, p_spi->tx_dma_source);
#endif
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        /* Close rx dma driver */
        spi_close_dma(p_spi->rx_dma_drv, p_spi->rx_dma_source);
#endif
    }
    return (result);
}/* End of function ARM_SPI_Transfer() */

/****************************************************************************************************************//**
* @brief    Get transmitted data count.
* @param[in]    p_spi          Pointer to a SPI resource structure
* @retval   number of data items transmitted
********************************************************************************************************************/
/* Function Name: ARM_SPI_GetDataCount */
uint32_t ARM_SPI_GetDataCount(st_spi_resources_t const * const p_spi) // @suppress("Non-API function naming")
{
    uint32_t retval = 0;
    
    if (SPI_PRV_EXEC_SEND == p_spi->xfer->exec_state)
    {
        if (NULL == p_spi->tx_dma_drv) // @suppress("Cast comment")
        {
            retval = p_spi->xfer->tx_cnt;
        }
#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        else
        {
            (void)p_spi->tx_dma_drv->GetTransferByte(p_spi->tx_dma_source, &retval);
        }
#endif
    }
    else
    {
        if (NULL == p_spi->rx_dma_drv) // @suppress("Cast comment")
        {
            retval = p_spi->xfer->rx_cnt;
        }
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        else
        {
            (void)p_spi->rx_dma_drv->GetTransferByte(p_spi->rx_dma_source, &retval);
        }
#endif
    }
    
    return (retval);
}/* End of function ARM_SPI_GetDataCount() */

/****************************************************************************************************************//**
 * @brief       CMSIS-Driver SPI Control API implementation for the SPI peripheral on RE Family.
 * @param[in]        control      Operation command
 * @param[in]        arg          Argument of operation command
 * @param[in,out]    p_spi        Pointer to a SPI resource structure
 * @retval      ARM_DRIVER_OK     Specified contol operation was successful.
 * @retval      ARM_DRIVER_ERROR  Specified command is not supported.
 * @retval      ARM_SPI_ERROR_MODE Specified SPI mode is not supported.
********************************************************************************************************************/
/* Function Name: ARM_SPI_Control */
int32_t ARM_SPI_Control(uint32_t control, uint32_t arg, st_spi_resources_t * const p_spi) // @suppress("Function length") @suppress("Non-API function naming")
{
    int32_t result = ARM_DRIVER_OK;
    st_spi_reg_buf_t spi_regs;

    if (0U == (p_spi->info->flags & SPI_FLAG_POWERED))
    {
        /* SPI not powered */
        return (ARM_DRIVER_ERROR);
    }
    
    if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_ABORT_TRANSFER)
    {
        spi_transmit_stop(p_spi);

        /* Clear bussy */
        p_spi->info->status.busy = 0U;

        /* Enable SPI */
        result = ARM_DRIVER_OK;
    }
    else
    {
        if (1 == p_spi->info->status.busy)
        {
            result = ARM_DRIVER_ERROR_BUSY;
        }
        else
        {
            spi_set_regs_clear(&spi_regs);                   /* Initialize the buffer for register setting */
            
            /* Set control mode */
            switch (control & ARM_SPI_CONTROL_Msk)
            {
                case ARM_SPI_MODE_INACTIVE:                  /* SPI Inactive */
                {
                    /* Disable SPI */
                    /* SPE - SPI Function Enable - Disables the SPI function*/
                    p_spi->reg->SPCR_b.SPE = 0;                       /* Internal reset */
                    p_spi->info->mode = ARM_SPI_MODE_INACTIVE;        /* Set to inactive */
                    p_spi->info->flags &= (~SPI_FLAG_CONFIGURED);       /* Clear Configured flag */
                    spi_init_register(&spi_regs, p_spi);              /* Reset register */
                    result = ARM_DRIVER_OK;
                }
                break;
            
                case ARM_SPI_MODE_MASTER:               /* SPI Master (Output on MOSI, Input on MISO); */
                {
                                                        /* arg = Bus Speed in bpss                     */
                    /* Set Maste mode */
                    /* MSTR - SPI Master/Slave Mode Select - Master mode */
                    spi_bit_set_8(&spi_regs.spcr, 3);
                    result = spi_set_init_master(control, arg, &spi_regs);
                    
                    if (ARM_DRIVER_OK == result)
                    {
                        spi_init_register(&spi_regs, p_spi);
                        p_spi->info->mode &= (~ARM_SPI_CONTROL_Msk);
                        p_spi->info->mode = spi_regs.mode | ARM_SPI_MODE_MASTER;
                        p_spi->info->flags |= SPI_FLAG_CONFIGURED;
                    }
                }
                break;
            
                case ARM_SPI_MODE_SLAVE:                     /* SPI Slave  (Output on MISO, Input on MOSI) */
                {
                    /* Set Slave mode */
                    /* MSTR - SPI Master/Slave Mode Select - Slave mode */
                    spi_bit_clear_8(&spi_regs.spcr, 3);
                    result = spi_set_init_slave(control, &spi_regs);
                    
                    if (ARM_DRIVER_OK == result)
                    {
                        spi_init_register(&spi_regs, p_spi);
                        p_spi->info->mode &= (~ARM_SPI_CONTROL_Msk);
                        p_spi->info->mode |= ARM_SPI_MODE_SLAVE;
                        p_spi->info->flags |= SPI_FLAG_CONFIGURED;
                    }
                }
                break;
            
                case ARM_SPI_SET_BUS_SPEED:                  /* Set Bus Speed in bps; arg = value */
                {
                    if ((0 != (p_spi->info->flags & SPI_FLAG_CONFIGURED))
                     &&(ARM_SPI_MODE_MASTER == (p_spi->info->mode & ARM_SPI_CONTROL_Msk)))
                    {
                        spi_regs.spcmd0   = p_spi->reg->SPCMD0;    /* Get current SPCMD0 register value */
                        result = rspi_baud_set(&spi_regs, arg);
                        if (ARM_DRIVER_OK == result)
                        {
                            /* SPE - SPI Function Enable - Disables the SPI function*/
                            p_spi->reg->SPCR_b.SPE = 0;                /* Internal reset */

                            /* SPBR - SPI Bit Rate Register - Depends on variables */
                            p_spi->reg->SPBR       = spi_regs.spbr;    /* Set bitrate */

                            /* SPCMD0 - SPI Command Registers 0 - Depends on variables  */
                            p_spi->reg->SPCMD0     = spi_regs.spcmd0;  /* Set bitrate */

                            p_spi->info->bps = spi_regs.bps;
                        }
                    }
                    else
                    {
                        result = ARM_DRIVER_ERROR;
                    }
                }
                break;
            
                case ARM_SPI_GET_BUS_SPEED:                 /* Get Bus Speed in bps */
                {
                    result = p_spi->info->bps;
                }
                break;
            
                case ARM_SPI_SET_DEFAULT_TX_VALUE:          /* Set default Transmit value; arg = value */
                {
                    p_spi->xfer->tx_def_val = arg;
                }
                break;
                case ARM_SPI_CONTROL_SS:                    /* Control Slave Select; arg = 0:inactive, 1:active */
                {
                    if ((ARM_SPI_MODE_MASTER != (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
                    || (ARM_SPI_SS_MASTER_SW != (p_spi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk))
                    || (NULL == p_spi->ss_pin)) // @suppress("Cast comment")
                    {
                        result = ARM_DRIVER_ERROR;
                    }
                    else
                    {
                        if (ARM_SPI_SS_ACTIVE == arg)
                        {
                          (*(p_spi->ss_pin)) &= (~(1 << p_spi->ss_pin_pos));
                        }
                        else
                        {
                            (*(p_spi->ss_pin)) |= (1 << p_spi->ss_pin_pos);
                        }
                    }
                }
                break;
            
                /* The default case is intentionally combined.  */
                /* Not Supported */
                case ARM_SPI_MODE_MASTER_SIMPLEX:       /* SPI Master (Output/Input on MOSI); arg = Bus Speed in bps */
                case ARM_SPI_MODE_SLAVE_SIMPLEX:        /* SPI Slave  (Output/Input on MISO) */
                default:
                {
                    result = ARM_SPI_ERROR_MODE;
                }
                break;
            }
        }
    }
    
    return (result);
}/* End of function ARM_SPI_Control() */

/****************************************************************************************************************//**
 *  @brief       Get SPI Status
 *  @param[in]   p_spi      Pointer to a SPI resource structure
 *  @retval      ARM_SPI_STATUS
**********************************************************************************************************************/
/* Function Name: ARM_SPI_GetStatus */
ARM_SPI_STATUS ARM_SPI_GetStatus(st_spi_resources_t const * const p_spi) // @suppress("Non-API function naming")
{
    return (p_spi->info->status);
}/* End of function ARM_SPI_GetStatus() */

/* End SPI Interface */

/****************************************************************************************************************//**
 * @brief        Clear SPI register setting buffer
 * @param[in]    p_regs   Pointer to SPI register setting buffer.
 *******************************************************************************************************************/
/* Function Name: spi_set_regs_clear */
static void spi_set_regs_clear(st_spi_reg_buf_t * const p_regs)
{
    p_regs->mode       = 0x00000000;
    p_regs->spcmd0     = 0x070D;
    p_regs->spcr       = 0x00;
    p_regs->spbr       = 0xFF;
    p_regs->data_bits  = 8;
    p_regs->bps        = 0;
}/* End of function spi_set_regs_clear() */

/****************************************************************************************************************//**
* @brief      Function to set Master mode.
* @param[in]  control      Operation command
* @param[in]  boudrate     bit rate
* @param[in]  p_spi_regs   Pointer to SPI resources
* @retval     ARM_DRIVER_OK          Specified contol operation was successful.
* @retval     ARM_SPI_ERROR_SS_MODE  The specified slave select mode is not supported.
**********************************************************************************************************************/
/* Function Name: spi_set_init_master */
static int32_t spi_set_init_master(uint32_t control, uint32_t baudrate, st_spi_reg_buf_t * const p_spi_regs)
{
    int32_t result = ARM_DRIVER_OK;
    
    switch(control & ARM_SPI_SS_MASTER_MODE_Msk)
    {
        case ARM_SPI_SS_MASTER_UNUSED:        /* SPI Slave Select when Master: Not used (default) */
        {
            p_spi_regs->mode |=  ARM_SPI_SS_MASTER_UNUSED;

            /* SPCR Set 3Line */
            /* SPMS - SPI Mode Select - Clock synchronous operation (3-wire method) */
            spi_bit_set_8(&p_spi_regs->spcr, 0);
        }
        break;
        case ARM_SPI_SS_MASTER_SW:            /* SPI Slave Select when Master: Software controlled */
        {
            p_spi_regs->mode |=  ARM_SPI_SS_MASTER_SW;

            /* SPCR Set 3Line */
            /* SPMS - SPI Mode Select - Clock synchronous operation (3-wire method) */
            spi_bit_set_8(&p_spi_regs->spcr, 0);
        }
        break;
        case ARM_SPI_SS_MASTER_HW_OUTPUT:     /* SPI Slave Select when Master: Hardware controlled Output */
        {
            p_spi_regs->mode |=  ARM_SPI_SS_MASTER_HW_OUTPUT;

            /* SPMS - SPI Mode Select - SPI operation (4-wire method) */
            spi_bit_clear_8(&p_spi_regs->spcr, 0);
        }
        break;

        /* The default case is intentionally combined.  */
        case ARM_SPI_SS_MASTER_HW_INPUT:      /* SPI Slave Select when Master: Hardware monitored Input */
        default:
        {
            result = ARM_SPI_ERROR_SS_MODE;
        }
        break;
    }
    if (ARM_DRIVER_OK == result)
    {
        /* Setting communication speed */
        result= rspi_baud_set(p_spi_regs, baudrate);
    }
    if (ARM_DRIVER_OK == result)
    {
        /* Master / Slave common settings */
        result= spi_set_init_common(control, p_spi_regs);
    }
    return (result);
}/* End of function spi_set_init_master() */

/****************************************************************************************************************//**
* @brief      Function to set Slave mode.
* @param[in]  p_spi_regs   Pointer to SPI register setting buffer
* @param[in]  control      Operation command
* @retval     ARM_DRIVER_OK          Specified contol operation was successful.
* @retval     ARM_SPI_ERROR_SS_MODE  Specified slave select mode is not supported.
**********************************************************************************************************************/
/* Function Name: spi_set_init_slave */
static int32_t spi_set_init_slave(uint32_t control, st_spi_reg_buf_t * const p_spi_regs)
{
    int32_t result = ARM_DRIVER_OK;
    switch(control & ARM_SPI_SS_SLAVE_MODE_Msk)
    {
        case ARM_SPI_SS_SLAVE_HW:    /* SPI Slave Select when Slave: Hardware monitored (default) */
        {
            p_spi_regs->mode |=  ARM_SPI_SS_SLAVE_HW;

            /* SPMS - SPI Mode Select - SPI operation (4-wire method) */
            spi_bit_clear_8(&p_spi_regs->spcr, 0);
        }
        break;
        case ARM_SPI_SS_SLAVE_SW:    /* SPI Slave Select when Slave: Software controlled */
        {
            /* SPCR Set 3Line */
            p_spi_regs->mode |=  ARM_SPI_SS_SLAVE_SW;

            /* SPMS - SPI Mode Select -  Clock synchronous operation (3-wire method) */
            spi_bit_set_8(&p_spi_regs->spcr, 0);
        }
        break;
        default:
        {
            return (ARM_SPI_ERROR_SS_MODE);
        }
        break;
    }

    /* Master / Slave common settings */
    result = spi_set_init_common(control, p_spi_regs);
    return (result);
}/* End of function spi_set_init_slave() */


/****************************************************************************************************************//**
* @brief      This function performs common setting between master mode and slave mode.
* @param[in]  control   SPI frame format setting command
* @param[in]  p_spi_regs   Pointer to SPI register setting buffer
* @retval     ARM_DRIVER_OK                Specified contol operation was successful.
* @retval     ARM_SPI_ERROR_FRAME_FORMAT   Specified frame format is not supported.
* @retval     ARM_SPI_ERROR_BIT_ORDER      Specified bit order is not supported.
* @note       ARM_SPI_TI_SSI and ARM_SPI_MICROWIRE are not supported.
*             Synchronous in Slave mode, operation with "CPHA = 0" is prohibited.
*             If this setting is requested, ARM_SPI_ERROR_FRAME_FORMAT is returned.
**********************************************************************************************************************/
/* Function Name: spi_set_init_common */
static int32_t spi_set_init_common(uint32_t control, st_spi_reg_buf_t * const p_spi_regs) // @suppress("Function length")
{
    int32_t result = ARM_DRIVER_OK;
    uint16_t set_spb = 0;
    uint8_t data_bit;
    
    /* SPI Frame Format */
    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA0:
        {
            if (ARM_SPI_SS_SLAVE_SW == (control & ARM_SPI_SS_SLAVE_MODE_Msk))
            {
                /* Synchronous operation in Slave mode can not be used with CPHA = 0. */
                result = ARM_SPI_ERROR_FRAME_FORMAT;
                break;
            }

            /* CPHA - RSPCK Phase Setting - Data sampling on odd edge, data variation on even edge */
            spi_bit_clear_16(&p_spi_regs->spcmd0, 0);       /* Set CPHA = 0 */

            /* CPOL - RSPCK Polarity Setting - RSPCK is low when idle */
            spi_bit_clear_16(&p_spi_regs->spcmd0, 1);       /* Set CPOL = 0 */
        }
        break;
        
        case ARM_SPI_CPOL0_CPHA1:
        {
            /* CPHA - RSPCK Phase Setting - Data variation on odd edge, data sampling on even edge */
            spi_bit_set_16(&p_spi_regs->spcmd0, 0);         /* Set CPHA = 1 */

            /* CPOL - RSPCK Polarity Setting - RSPCK is low when idle */
            spi_bit_clear_16(&p_spi_regs->spcmd0, 1);       /* Set CPOL = 0 */
        }
        break;
        
        case ARM_SPI_CPOL1_CPHA0:
        {
            if (ARM_SPI_SS_SLAVE_SW == (control & ARM_SPI_SS_SLAVE_MODE_Msk))
            {
                /* Synchronous operation in Slave mode can not be used with CPHA = 0. */
                result = ARM_SPI_ERROR_FRAME_FORMAT;
                break;
            }

            /* CPHA - RSPCK Phase Setting - Data sampling on odd edge, data variation on even edge */
            spi_bit_clear_16(&p_spi_regs->spcmd0, 0);       /* Set CPHA = 0 */

            /* CPOL - RSPCK Polarity Setting - RSPCK is high when idle */
            spi_bit_set_16(&p_spi_regs->spcmd0, 1);         /* Set CPOL = 1 */
        }
        break;
        
        case ARM_SPI_CPOL1_CPHA1:
        {
            /* CPHA - RSPCK Phase Setting - Data variation on odd edge, data sampling on even edge */
            spi_bit_set_16(&p_spi_regs->spcmd0, 0);         /* Set CPHA = 1 */

            /* CPOL - RSPCK Polarity Setting - RSPCK is high when idle */
            spi_bit_set_16(&p_spi_regs->spcmd0, 1);         /* Set CPOL = 1 */
        }
        break;

        /* Unsupported */
        /* The default case is intentionally combined.  */
        case ARM_SPI_TI_SSI:
        case ARM_SPI_MICROWIRE:
        default:
        {
            result = ARM_SPI_ERROR_FRAME_FORMAT;
        }
        break;
    }
    switch(control & ARM_SPI_BIT_ORDER_Msk)
    {
        case ARM_SPI_MSB_LSB:
        {
            /* LSBF - SPI LSB First - MSB first */
            spi_bit_clear_16(&p_spi_regs->spcmd0, 12);       /* Set LSBF = 0 */
        }
        break;

        case ARM_SPI_LSB_MSB:
        {
            /* LSBF - SPI LSB First - LSB first */
            spi_bit_set_16(&p_spi_regs->spcmd0, 12);         /* Set LSBF = 1 */
        }
        break;

        default:
        {
            result = ARM_SPI_ERROR_BIT_ORDER;
        }
        break;
    }
    
    /* Set Data Bits */
    data_bit = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);

    p_spi_regs->spcmd0 &= SPI_PRV_SPCMD0_SPB_CLR_MASK;
    switch (data_bit)
    {
        case 20:
        {
            /* SPCMD0.SPB = "0000b" */
            /* SPB - SPI Data Length Setting - 20bit */
            p_spi_regs->spcmd0 |= SPI_PRV_SPCMD0_SPB_20BIT;
            p_spi_regs->data_bits = data_bit;
        }
        break;
        
        case 24:
        {
            /* SPCMD0.SPB = "0001b" */
            /* SPB - SPI Data Length Setting - 24bit */
            p_spi_regs->spcmd0 |= SPI_PRV_SPCMD0_SPB_24BIT;
            p_spi_regs->data_bits = data_bit;
        }
        break;

        case 32:
        {
            /* SPCMD0.SPB = "0010b" */
            /* SPB - SPI Data Length Setting - 32bit */
            p_spi_regs->spcmd0 |= SPI_PRV_SPCMD0_SPB_32BIT;
            p_spi_regs->data_bits = data_bit;
        }
        break;

        case 8:
        {
            /* SPCMD0.SPB = "0100b" */
            /* SPB - SPI Data Length Setting - 8bit */
            p_spi_regs->spcmd0 |= SPI_PRV_SPCMD0_SPB_8BIT;
            p_spi_regs->data_bits = data_bit;
        }
        break;
        
        default:
        {
            if ((8U < data_bit) && (16U >= data_bit))
            {
                /* When SPI Data Length is from 9 bits to 16 bits,
                   SPB setting value is "Data Length - 1". */
                set_spb = (uint16_t)(data_bit - 1);
                set_spb <<= SPI_PRV_SPCMD0_SPB_OFFSET;

                /* SPB - SPI Data Length Setting - Depends on variables */
                p_spi_regs->spcmd0 |= set_spb;
                p_spi_regs->data_bits = data_bit;
            }
            else
            {
                result = ARM_SPI_ERROR_DATA_BITS;
            }
        }
        break;
        
    }
    return (result);

}/* End of function spi_set_init_common() */

/****************************************************************************************************************//**
* @brief        Determines the SPI channel SPBR register setting for the requested baud rate.
*
*                Returns the actual bit rate that the setting will achieve which may differ from requested.
*                If the requested bit rate cannot be exactly achieved, the next lower bit rate setting will be applied.
*               If successful, applies the calculated setting to the SPBR register.
* @param[in]    p_spi_regs     Pointer to SPI register setting buffer
* @param[in]    bps_target     Target baud
* @retval       ARM_DRIVER_ERROR baud_bps_target too high for the PCLK
*               ARM_DRIVER_OK Bit rate calculation was successful.
* @note         Target baud must be >= PCLK/4 to get anything out.
*               Does not track dynamically changing PCLK. 
**********************************************************************************************************************/
/* Function Name: rspi_baud_set */
static int32_t rspi_baud_set(st_spi_reg_buf_t * const p_spi_regs, uint32_t bps_target)
{
    uint32_t    bps_calc = 0;
    int32_t     freq = 0;        //Frequency(PCLK)
    int32_t     spbr_value;      //n term in equation(SPBR)
    int32_t     div;             //N term in equation(SPCMD0.BRDV)
    int32_t     result = ARM_DRIVER_OK;
    

    (void)R_SYS_SystemClockFreqGet((uint32_t*)&freq); // @suppress("Cast comment")

    /* Get the register settings for requested baud rate. */
    if ((freq / bps_target) < 2)
    {
        return (ARM_DRIVER_ERROR);   /* baud_bps_target too high for the PCLK. */
    }

    /*
     * From Hardware manual: Bit rate = freq / (2(spbr_value + 1)(2^div))
     * where:
     *      freq = PCLK, spbr_value = SPBR setting, div = BRDV bits
     * Solving for spbr_value:
     *      spbr_value = (((freq/(2^div))/2) / bps) - 1
     *
     */

    div = 0;
    while (div <= 3)
    {
        spbr_value = ((freq >> (div + 1)) / (int32_t)bps_target) - 1;  /* Solve for SPBR setting. */ // @suppress("Cast comment")

        if ((spbr_value >= 0) && (spbr_value <= 0xff)) /* Must be <= SPBR register max value. Must not be negative*/
        {
            /* Now plug spbr_valueback into the formula for BPS and check it. */
            bps_calc = (uint32_t)(freq / (2 * ((spbr_value + 1) << div)));
    
            if (bps_calc > bps_target)
            {
                spbr_value += 1;
            }
            if (spbr_value <= 0xff)
            {
                p_spi_regs->spbr = spbr_value;                         /* Save to bus speed. */
                p_spi_regs->spcmd0 &= SPI_PRV_MASK_BRDV;
                p_spi_regs->spcmd0 |= gs_spi_brdv_tbl[div];               /* Store bitrate divided setteing  */
                p_spi_regs->bps = bps_calc;                            /* Store bus speed */
                break;
            }
        }
        div++;
    }
    if (div > 3)
    {
        result = ARM_DRIVER_ERROR;
    }
    return (result);    /* Return the actual BPS rate achieved. */
}/* end of function rspi_baud_set(). */

/****************************************************************************************************************//**
 * @brief       Register initialization function
 * @param[in]   p_spi_regs   Pointer to SPI register setting buffer
 * @param[in]   p_spi        Pointer to SPI resource data
 *******************************************************************************************************************/
/* Function Name: spi_init_register */
static void spi_init_register(st_spi_reg_buf_t const * const p_spi_regs, st_spi_resources_t * const p_spi) // @suppress("Function length")
{
    volatile uint8_t dummy;
    
    /* SPE - SPI Function Enable - Disables the SPI function*/
    p_spi->reg->SPCR_b.SPE    = 0;                          /* SPI internal reset */

    /* Register setting */
    /* SSLP    - SPI Slave Select Polarity Register
       b7 : b4 - Resaeved
       b3      - SSL3P - SSL3 Signal Polarity Setting - SSL3 signal is active low
       b2      - SSL2P - SSL2 Signal Polarity Setting - SSL2 signal is active low
       b1      - SSL1P - SSL1 Signal Polarity Setting - SSL1 signal is active low
       b0      - SSL0P - SSL0 Signal Polarity Setting - SSL0 signal is active low */
    p_spi->reg->SSLP          = 0x00;                       /* SSL0 Low active */

    /* SPPCR - SPI Pin Control Register
       b5    - MOIFE - MOSI Idle Value Fixing Enable
                     - MOSI output value equals final data from previous transfer
       b4    - MOIFV - MOSI Idle Fixed Value 0
                     - The level output on the MOSIn pin during MOSI idling corresponds to low
       b1    - SPLP2 - SPI Loopback 2 - Normal mode
       b0    - SPLP  - SPI Loopback - Normal mode */
    p_spi->reg->SPPCR         = 0x00;                       /* Normal mode */

    /* SPBR - SPI Bit Rate Register - Depends on variables */
    p_spi->reg->SPBR          = p_spi_regs->spbr;           /* Set bitrate */

    /* SPDCR - SPI Data Control Register
       b6    - SPBYT  - SPI Byte Access Specification
                      - SPDR is accessed in words or longwords (the SPLW bit is enabled)
       b5    - SPLW   - SPI Longword Access/Word Access Specification
                      - SPDR is accessed in longwords
       b4    - SPRDTD - SPI Receive/Transmit Data Select
                      - SPDR values are read from the receive buffer
       b1:b0 - SPFC   - Number of Frames Specification - 1 frame */
    p_spi->reg->SPDCR         = 0x00;                       /*  */
    
    /* SPCKD - SPI Clock Delay Register - 1 RSPCK */
    p_spi->reg->SPCKD         = 0x00;

    /* SSLND - SPI Slave Select Negation Delay Register
       b2:b0  - SLNDL - 1 RSPCK */
    p_spi->reg->SSLND         = 0x00;

    /* SPND - SPI Next-Access Delay Register
       b2:b0 - SPNDL - SPI Next-Access Delay Setting - 1 RSPCK + 2 PCLK */
    p_spi->reg->SPND          = 0x00;

    /* SPCR2 - SPI Control Register 2
       b4    - SCKASE - RSPCK Auto-Stop Function Enable - Disables the RSPCK auto-stop function
       b3    - PTE - Parity Self-Diagnosis
                   - Disables the self-diagnosis function of the parity circuit
       b2    - SPIIE - SPI Idle Interrupt Enable
                     - Disables the generation of idle interrupt requests
       b1    - SPOE - Parity Mode - Selects even parity for use in transmission and reception
       b0    - SPPE - Parity Enable - Does not add the parity bit to transmit data
                                      and does not check the parity bit of receive data */
    p_spi->reg->SPCR2         = 0x00;

    /* SPSCR - SPI Sequence Control Register
       b2:b0 - SPSLN - SPI Sequence Length Specification */
    p_spi->reg->SPSCR         = 0x00;

    /* SPCMD0 - SPI Command Registers 0 - Depends on argument
       b15    - SCKDEN - RSPCK Delay Setting Enable
       b14    - SLNDEN - SSL Negation Delay Setting Enable
       b13    - SPNDEN - SPI Next-Access Delay Enable
       b12    - LSBF - SPI LSB First
       b11:b8 - SPB - SPI Data Length Setting
       b7     - SSLKP - SSL Signal Level Keeping
       b6     - Resaeved
       b5:b4  - SSLA - SSL Signal Assertion Setting
       b3:b2  - BRDV - Bit Rate Division Setting
       b1     - CPOL - RSPCK Polarity Setting
       b0     - CPHA - RSPCK Phase Setting */
    p_spi->reg->SPCMD0        = p_spi_regs->spcmd0;

    /* SPSR - SPI Status Register
        b4  - UDRF - Underrun Error Flag - Neither a mode fault error nor an underrun error occurs
        b3  - PERF - Parity Error Flag - No parity error occurs
        b2  - MODF - Mode Fault Error Flag - Neither a mode fault error nor an underrun error occurs
        b1  - IDLNF - SPI Idle Flag - SPI is in the idle state
        b0  - OVRF - Overrun Error Flag - No overrun error occurs */
    p_spi->reg->SPSR = 0xA0;                  /* Clear SPE,OVRF,IDLNF,MODF,PERF,UDRF */
    dummy            = p_spi->reg->SPDR_HA;
    p_spi->pin_set();                                       /* Setting of input / output terminal */

    /* SPCR - SPI Control Register - Depends on argument
       b7   - SPRIE - SPI Receive Buffer Full Interrupt Enable
       b6   - SPE - SPI Function Enable
       b5   - SPTIE - Transmit Buffer Empty Interrupt Enable
       b4   - SPEIE - SPI Error Interrupt Enable 
       b3   - MSTR  - SPI Master/Slave Mode Select 
       b2   - MODFEN - Mode Fault Error Detection Enable 
       b1   - TXMD - Communications Operating Mode Select
       b0   - SPMS - SPI Mode Select */
    p_spi->reg->SPCR          = p_spi_regs->spcr;
    dummy                     = p_spi->reg->SPCR;           /* Dummy read spcr register */
    p_spi->xfer->data_bits    = p_spi_regs->data_bits;      /* Memory bit size */
    p_spi->info->bps          = p_spi_regs->bps;
    
}/* End of function spi_init_register() */


/****************************************************************************************************************//**
* @brief      Common processing at the end of transfer.
* @param[in]  p_spi    Pointer to SPI resources
**********************************************************************************************************************/
/* Function Name: spi_interrupt_disable */
static void spi_interrupt_disable(st_spi_resources_t * const p_spi)
{
   /* SPCR - SPI Control Register
      b7   - SPRIE  - SPI Receive Buffer Full Interrupt Enable
                    - Disables the generation of SPI receive buffer full interrupt requests
      b6   - SPE - SPI Function Enable - Disables the SPI function
      b5   - SPTIE - Transmit Buffer Empty Interrupt Enable
                   - Disables the generation of transmit buffer empty interrupt requests
      b4   - SPEIE - SPI Error Interrupt Enable - Disables the generation of SPI error interrupt requests */
    p_spi->reg->SPCR  &= 0x0F;                /* Clear SPRIE,SPTIE,SPEIE */
    p_spi->reg->SPCR2_b.SPIIE = 0;            /* Clear SPIIE */
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->sptend_irq)
    {
        R_SYS_IrqStatusClear(p_spi->sptend_irq);
        R_NVIC_ClearPendingIRQ(p_spi->sptend_irq);
        R_NVIC_DisableIRQ(p_spi->sptend_irq);
    }
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spii_irq) // @suppress("Cast comment")
    {
        R_SYS_IrqStatusClear(p_spi->spii_irq);
        R_NVIC_ClearPendingIRQ(p_spi->spii_irq);
        R_NVIC_DisableIRQ(p_spi->spii_irq);
    }
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spri_irq) // @suppress("Cast comment")
    {
        R_SYS_IrqStatusClear(p_spi->spri_irq);
        R_NVIC_ClearPendingIRQ(p_spi->spri_irq);
        R_NVIC_DisableIRQ(p_spi->spri_irq);
    }
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spei_irq) // @suppress("Cast comment")
    {
        R_SYS_IrqStatusClear(p_spi->spei_irq);
        R_NVIC_ClearPendingIRQ(p_spi->spei_irq);
        R_NVIC_DisableIRQ(p_spi->spei_irq);
    }
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spti_irq) // @suppress("Cast comment")
    {
        R_SYS_IrqStatusClear(p_spi->spti_irq);
        R_NVIC_ClearPendingIRQ(p_spi->spti_irq);
        R_NVIC_DisableIRQ(p_spi->spti_irq);
    }
}/* End of function spi_interrupt_disable() */

/****************************************************************************************************************//**
* @brief      Common processing at the end of transfer.
* @param[in]  p_spi    Pointer to SPI resources
**********************************************************************************************************************/
/* Function Name: spi_ir_flag_clear */
static int32_t spi_ir_flag_clear(st_spi_resources_t * const p_spi)
{
    int32_t result         = ARM_DRIVER_OK;
    uint64_t time_out_cnt  = 0;
    
    p_spi->reg->SPCR_b.SPE = 0;
    p_spi->reg->SPCR_b.SPRIE = 0;
    p_spi->reg->SPCR_b.SPTIE = 0;
    
    while (0 != p_spi->reg->SPCR_b.SPRIE)
    {
        time_out_cnt++;
        if (SYSTEM_CFG_API_TIMEOUT_COUNT <= time_out_cnt)
        {
            result = ARM_DRIVER_ERROR_TIMEOUT;
            break;
        }
    }
    while (0 != p_spi->reg->SPCR_b.SPTIE)
    {
        time_out_cnt++;
        if (SYSTEM_CFG_API_TIMEOUT_COUNT <= time_out_cnt)
        {
            result = ARM_DRIVER_ERROR_TIMEOUT;
            break;
        }
    }
    if (ARM_DRIVER_OK == result)
    {
        /* Check SPRI valid. */
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spri_irq)
        {
            R_SYS_IrqStatusClear(p_spi->spri_irq);
            R_NVIC_ClearPendingIRQ(p_spi->spri_irq);   
        }

        /* Check SPTI valid. */
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spti_irq)
        {   
            R_SYS_IrqStatusClear(p_spi->spti_irq);
            R_NVIC_ClearPendingIRQ(p_spi->spti_irq);
        }
    } 
    return (result);
}/* End of function spi_ir_flag_clear() */
    

/****************************************************************************************************************//**
* @brief        check_tx_available
* @param[in]    p_flag    Pointer to initialize flag
* @param[in]    p_spi    Pointer to SPI resources
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: check_tx_available */
static int32_t check_tx_available(int16_t * const p_flag, st_spi_resources_t * const p_spi)
{

#if (0 != SPI_PRV_USED_TX_DMAC_DRV)
    if ((NULL != p_spi->tx_dma_drv)// @suppress("Cast comment")
#if (0 != SPI_PRV_USED_TX_DTC_DRV)
         && ((&Driver_DTC) != p_spi->tx_dma_drv)
#endif
         )
    {
        /* When sending by DMAC */
        if (p_spi->tx_dma_drv->Open () != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }
        if (p_spi->tx_dma_drv->InterruptEnable (0, NULL) != DMA_OK) // @suppress("Cast comment")
        {
            return (ARM_DRIVER_ERROR);
        }
    }
    else
#endif
    {
        /* When sending by interrupt */
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spti_irq)
        {
            /* Register spti interrupt handler */
            if ((-1) == R_SYS_IrqEventLinkSet(p_spi->spti_irq, p_spi->spti_iesr_val, p_spi->spti_callback))
            {
                return (ARM_DRIVER_ERROR);
            }
    
            /* SPTI priority setting */
            R_NVIC_SetPriority(p_spi->spti_irq, p_spi->spti_priority);
            if (R_NVIC_GetPriority (p_spi->spti_irq) != p_spi->spti_priority)
            {
                return (ARM_DRIVER_ERROR);
            }
#if (0 != SPI_PRV_USED_TX_DTC_DRV)
            if ((&Driver_DTC) == p_spi->tx_dma_drv)
            {
                /* When using the DMAC / DTC driver at transmission */
                if (p_spi->tx_dma_drv->Open () != DMA_OK)
                {
                    return (ARM_DRIVER_ERROR);
                }
            }
#endif
        }
        else
        {
            return (ARM_DRIVER_ERROR);
        }
    }
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spii_irq) // @suppress("Cast comment")
    {
        /* Register SPII interrupt handler */
        if ((-1) == R_SYS_IrqEventLinkSet(p_spi->spii_irq, p_spi->spii_iesr_val, p_spi->spii_callback))
        {
            return (ARM_DRIVER_ERROR);
        }
        R_NVIC_SetPriority(p_spi->spii_irq, p_spi->spii_priority);
        if (R_NVIC_GetPriority (p_spi->spii_irq) != p_spi->spii_priority)
        {
            return (ARM_DRIVER_ERROR);
        }
        (*p_flag) |= SPI_FLAG_MASTER_SEND_AVAILABLE;
    }
    if ((SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->sptend_irq) && // @suppress("Cast comment")
       ((SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spei_irq))) // @suppress("Cast comment")
    {
        /* Register SPTEND interrupt handler */
        if ((-1) == R_SYS_IrqEventLinkSet(p_spi->sptend_irq, p_spi->sptend_iesr_val, p_spi->sptend_callback))
        {
            return (ARM_DRIVER_ERROR);
        }
        R_NVIC_SetPriority(p_spi->sptend_irq, p_spi->sptend_priority);
        if (R_NVIC_GetPriority (p_spi->sptend_irq) != p_spi->sptend_priority)
        {
            return (ARM_DRIVER_ERROR);
        }
        
        /* Register SPEI interrupt handler */
        if ((-1) == R_SYS_IrqEventLinkSet(p_spi->spei_irq, p_spi->spei_iesr_val, p_spi->spei_callback))
        {
            return (ARM_DRIVER_ERROR);
        }
        R_NVIC_SetPriority(p_spi->spei_irq, p_spi->spei_priority);
        if (R_NVIC_GetPriority (p_spi->spei_irq) != p_spi->spei_priority)
        {
            return (ARM_DRIVER_ERROR);
        }
        
        (*p_flag) |= SPI_FLAG_SLAVE_SEND_AVAILABLE;
    }
    return (ARM_DRIVER_OK);
}/* End of function check_tx_available() */


/****************************************************************************************************************//**
* @brief        check_rx_available
* @param[in]    p_flag    Pointer to initialize flag
* @param[in]    p_spi    Pointer to SPI resources
* @retval       execution_status
**********************************************************************************************************************/
/* Function Name: check_rx_available */
static int32_t check_rx_available(int16_t * const p_flag, st_spi_resources_t * const p_spi)
{
    bool rx_available = true;
    
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spei_irq) // @suppress("Cast comment")
    {
        if ((-1) == R_SYS_IrqEventLinkSet(p_spi->spei_irq, p_spi->spei_iesr_val, p_spi->spei_callback))
        {
            return (ARM_DRIVER_ERROR);
        }
        R_NVIC_SetPriority(p_spi->spei_irq, p_spi->spei_priority);
        if (R_NVIC_GetPriority (p_spi->spei_irq) != p_spi->spei_priority)
        {
            return (ARM_DRIVER_ERROR);
        }
    }
    else
    {
        rx_available = false;
    }
    
    if (true == rx_available)
    {

#if (0 != SPI_PRV_USED_RX_DMAC_DRV)
        if ((NULL != p_spi->rx_dma_drv)
#if (0 != SPI_PRV_USED_RX_DTC_DRV)
              && ((&Driver_DTC) != p_spi->rx_dma_drv)
#endif
              ) // @suppress("Cast comment")
        {
            /* When sending by DMAC */
            if (p_spi->rx_dma_drv->Open () != DMA_OK)
            {
                return (ARM_DRIVER_ERROR);
            }
            if (p_spi->rx_dma_drv->InterruptEnable (0, NULL) == DMA_OK) // @suppress("Cast comment")
            {
                (*p_flag) |= SPI_FLAG_SLAVE_RECEIVE_AVAILABLE;
            }
        }
        else
#endif
        {
            /* When sending by interrupt */
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spri_irq)
            {
                /* Register spri interrupt handler */
                if ((-1) == R_SYS_IrqEventLinkSet(p_spi->spri_irq, p_spi->spri_iesr_val, p_spi->spri_callback))
                {
                    return (ARM_DRIVER_ERROR);
                }

                /* SPTI priority setting */
                R_NVIC_SetPriority(p_spi->spri_irq, p_spi->spri_priority);
                if (R_NVIC_GetPriority (p_spi->spri_irq) != p_spi->spri_priority)
                {
                    return (ARM_DRIVER_ERROR);
                }   
#if (0 != SPI_PRV_USED_RX_DTC_DRV)         
                if ((&Driver_DTC) == p_spi->rx_dma_drv)
                {
                    /* When using the DMAC / DTC driver at transmission */
                    if (p_spi->rx_dma_drv->Open () != DMA_OK) // @suppress("If statement nesting")
                    {
                        return (ARM_DRIVER_ERROR);
                    }
                }
#endif
                (*p_flag) |= SPI_FLAG_SLAVE_RECEIVE_AVAILABLE;
            }
            else
            {
                rx_available = false;
            }
        }
    }

    /* If Send/Receive is possible and SPEI is enabled, master Receive is possible. */
    if (true == rx_available)
    {
        if (0 != ((*p_flag) & SPI_FLAG_MASTER_SEND_AVAILABLE))
        {
            (*p_flag) |= SPI_FLAG_MASTER_RECEIVE_AVAILABLE;
        }
    }

#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
    else
    {
        if (NULL != p_spi->rx_dma_drv) // @suppress("Cast comment")
        {
            return (ARM_DRIVER_ERROR);
        }
    }
#endif

    return (ARM_DRIVER_OK);
}/* End of function check_rx_available() */

/****************************************************************************************************************//**
* @brief    Transmit stop.
* @param[in,out]    p_spi        Pointer to SPI resource data
********************************************************************************************************************/
/* Function Name: spi_transmit_stop */
static void spi_transmit_stop(st_spi_resources_t const * const p_spi)
{
    spi_interrupt_disable(p_spi);

    if (NULL == p_spi->tx_dma_drv) // @suppress("Cast comment")
    {   /* @suppress("Empty compound statement") */ /* @suppress("Block comment") */
        /* When receiving by interrupt */
        /* Interrupts are disabled, so do not do anything here. */
    }
#if (0 != SPI_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == p_spi->tx_dma_drv)
    {
        /* When receiving by DTC */
        p_spi->tx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &p_spi->spti_irq);
    }
#endif
#if(0 != SPI_PRV_USED_TX_DMAC_DRV)
    else
    {
        /* When receiving by DMAC */
        p_spi->tx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        p_spi->tx_dma_drv->InterruptDisable();
    }
#endif
    if (NULL == p_spi->rx_dma_drv) // @suppress("Cast comment")
    {   /* @suppress("Empty compound statement") */ /* @suppress("Block comment") */
        /* When receiving by interrupt */
        /* No action to be performed */
    }
#if(0 != SPI_PRV_USED_RX_DTC_DRV)
    else if ((&Driver_DTC) == p_spi->rx_dma_drv)
    {
        /* When receiving by DTC */
        p_spi->rx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &p_spi->spri_irq);
    }
#endif
#if(0 != SPI_PRV_USED_RX_DMAC_DRV)
    else
    {
        /* When receiving by DMAC */
        p_spi->rx_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        p_spi->rx_dma_drv->InterruptDisable();
    }
#endif
}/* End of function spi_transmit_stop() */


/****************************************************************************************************************//**
* @brief    Make necessary settings for transmission.
* @param[in]        p_data       Pointer to output data
* @param[in]        num          Number of data to transmit
* @param[in,out]    dummy_flag   Dummy transmission flag - Dummy transmission : true / Normal transmission : false
* @param[in,out]    p_spi        Pointer to SPI resource data
********************************************************************************************************************/
/* Function Name: spi_send_setting */
static int32_t spi_send_setting(void const * const p_data, uint32_t num, bool dummy_flag, st_spi_resources_t * const p_spi)
{

#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
    st_dma_transfer_data_cfg_t dma_config;
    
    /* Initialize DTC/DMAC Config Data */
    dma_config_init(&dma_config);
    dma_config.src_addr = (uint32_t)p_data; // @suppress("Cast comment")
    dma_config.dest_addr = (uint32_t)(&(p_spi->reg->SPDR)); // @suppress("Cast comment")
    dma_config.transfer_count = num;
    
    if (true != dummy_flag)
    {
        dma_config.mode = DMA_MODE_NORMAL | DMA_SRC_INCR | DMA_DEST_FIXED;
    }
    else
    {
        dma_config.mode = DMA_MODE_NORMAL | DMA_SRC_FIXED | DMA_DEST_FIXED;
    }
#endif

    /* Switch the buffer size according to the bit size */
    if (p_spi->xfer->data_bits > 16)
    {
#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        /* When the bit size is 17 bits or more */
        dma_config.mode |=  DMA_SIZE_LONG;
#endif
        p_spi->reg->SPDCR_b.SPBYT = 0;
        p_spi->reg->SPDCR_b.SPLW  = 1;
    }
    else if (p_spi->xfer->data_bits > 8)
    {
        /* When the bit size is from 9 bits to 15 bits. */

#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        dma_config.mode |= DMA_SIZE_WORD;
#endif
        p_spi->reg->SPDCR_b.SPBYT = 0;
        p_spi->reg->SPDCR_b.SPLW  = 0;
    }
    else
    {
        /* When the bit size is 8 bits or less. */
#if (0 != SPI_PRV_USED_TX_DMAC_DTC_DRV)
        dma_config.mode |= DMA_SIZE_BYTE;
#endif
        p_spi->reg->SPDCR_b.SPBYT = 1;
    }
    
    if (NULL == p_spi->tx_dma_drv) // @suppress("Cast comment")
    {   /* @suppress("Empty compound statement") */ /* @suppress("Block comment") */
        /* When sending by interrupt */
        /* Enable NVIC interrupt by Send / Receive / Transfer function, no processing here. */
    }
#if(0 != SPI_PRV_USED_TX_DTC_DRV)
    else if ((&Driver_DTC) == p_spi->tx_dma_drv)
    {
        /* When sending by DTC */
        /* DTC transfer setting */
        dma_config.p_transfer_data = p_spi->tx_dtc_info;
        if (p_spi->tx_dma_drv->Create (p_spi->tx_dma_source, &dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }

        /* SPTI interrupt is not enabled, transfer does not start even when the start command is executed. */
        (void)p_spi->tx_dma_drv->Control(DMA_CMD_START, NULL);
    }
#endif
#if(0 != SPI_PRV_USED_TX_DMAC_DRV)
    else
    {
        /* When sending by DMAC */
        if (p_spi->tx_dma_drv->Create (p_spi->tx_dma_source, &dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }

        /* DMAC Start */
        (void)p_spi->tx_dma_drv->InterruptEnable(DMA_INT_COMPLETE, p_spi->spti_callback);
        (void)p_spi->tx_dma_drv->Control(DMA_CMD_START, NULL); // @suppress("Cast comment")
    }
#endif
    return (ARM_DRIVER_OK);
}/* End of function spi_send_setting() */

/****************************************************************************************************************//**
* @brief    Make necessary settings for reception.
* @brief    Make necessary settings for transmission.
* @param[in]        p_data       Pointer to input data
* @param[in]        num          Number of data to transmit
* @param[in,out]    p_spi        Pointer to SPI resource data
********************************************************************************************************************/
/* Function Name: spi_receive_setting */
static int32_t spi_receive_setting(void const * const p_data, uint32_t num, st_spi_resources_t * const p_spi)
{

#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
    st_dma_transfer_data_cfg_t dma_config;
    
    /* Initialize DTC/DMAC Config Data */
    dma_config_init(&dma_config);
    dma_config.src_addr  = (uint32_t)(&(p_spi->reg->SPDR)); // @suppress("Cast comment")
    dma_config.dest_addr = (uint32_t)p_data; // @suppress("Cast comment")
    dma_config.transfer_count = num;
#endif
    /* Switch the buffer size according to the bit size */
    if (p_spi->xfer->data_bits > 16)
    {
        /* When the bit size is 17 bits or more */
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_LONG | DMA_SRC_FIXED | DMA_DEST_INCR;
#endif
        p_spi->reg->SPDCR_b.SPBYT = 0;
        p_spi->reg->SPDCR_b.SPLW  = 1;
    }
    else if (p_spi->xfer->data_bits > 8)
    {
        /* When the bit size is from 9 bits to 15 bits. */
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_WORD | DMA_SRC_FIXED | DMA_DEST_INCR;
#endif
        p_spi->reg->SPDCR_b.SPBYT = 0;
        p_spi->reg->SPDCR_b.SPLW  = 0;
    }
    else
    {
        /* When the bit size is 8 bits or less. */
#if (0 != SPI_PRV_USED_RX_DMAC_DTC_DRV)
        dma_config.mode = DMA_MODE_NORMAL | DMA_SIZE_BYTE | DMA_SRC_FIXED | DMA_DEST_INCR;
#endif
        p_spi->reg->SPDCR_b.SPBYT = 1;
    }

    if (NULL == p_spi->rx_dma_drv) // @suppress("Cast comment")
    {   /* @suppress("Empty compound statement") */ /* @suppress("Block comment") */
        /* When sending by interrupt */
        /* Enable NVIC interrupt by Send / Receive / Transfer function, no processing here. */
    }
#if(0 != SPI_PRV_USED_RX_DTC_DRV)
    else if ((&Driver_DTC) == p_spi->rx_dma_drv)
    {
        /* When sending by DTC */
        /* DTC transfer setting */
        dma_config.p_transfer_data = p_spi->rx_dtc_info;
        if (p_spi->rx_dma_drv->Create (p_spi->rx_dma_source, &dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }

        /* DTC Start */
        p_spi->rx_dma_drv->Control(DMA_CMD_START, NULL);
    }
#endif
#if(0 != SPI_PRV_USED_RX_DMAC_DRV)
    else
    {
        /* DMAC transfer setting */
        if (p_spi->rx_dma_drv->Create (p_spi->rx_dma_source, &dma_config) != DMA_OK)
        {
            return (ARM_DRIVER_ERROR);
        }

         /* DMAC Start */
        if (ARM_SPI_MODE_SLAVE == (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
        {
           (void)p_spi->rx_dma_drv->InterruptEnable(DMA_INT_COMPLETE, p_spi->spri_callback); // @suppress("Cast comment")
        }
        (void)p_spi->rx_dma_drv->Control(DMA_CMD_START, NULL); // @suppress("Cast comment")
    }
#endif
    return (ARM_DRIVER_OK);
}/* End of function spi_receive_setting() */

#if (0 != SPI_PRV_USED_DMAC_DTC_DRV)
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
* @brief      Receive interrupt handling function.
* @param[in]  p_spi    Pointer to SPI resources
**********************************************************************************************************************/
/* Function Name: r_spri_handler */
static void r_spri_handler(st_spi_resources_t * const p_spi)
{

    uint32_t mask;
    volatile uint32_t dummy;
    if ((NULL == p_spi->rx_dma_drv) && // @suppress("Cast comment")
        (p_spi->xfer->rx_cnt < p_spi->xfer->num))
    {
        /* data bit mask */
        mask = ~(SPI_PRV_BASE_BIT_MASK << (p_spi->xfer->data_bits - 1));

        /* Switch the buffer size according to the bit size */
        if (p_spi->xfer->data_bits > 16)
        {
            ((uint32_t*)(p_spi->xfer->rx_buf))[p_spi->xfer->rx_cnt] = (p_spi->reg->SPDR & mask); // @suppress("Cast comment")
        }
        else if (p_spi->xfer->data_bits > 8)
        {
            ((uint16_t*)(p_spi->xfer->rx_buf))[p_spi->xfer->rx_cnt] = (p_spi->reg->SPDR_HA & mask); // @suppress("Cast comment")
        }
        else
        {
            ((uint8_t*)(p_spi->xfer->rx_buf))[p_spi->xfer->rx_cnt] = p_spi->reg->SPDR_BY;
        }
        
        p_spi->xfer->rx_cnt++;
    }
    else
    {
        if (p_spi->xfer->data_bits > 16)
        {
            dummy = p_spi->reg->SPDR;
        }
        else if (p_spi->xfer->data_bits > 8)
        {
            dummy = p_spi->reg->SPDR_HA;
        }
        else
        {
            dummy = p_spi->reg->SPDR_BY;
        }        
    }
    
    /* Final data read complete? */
    if ((p_spi->xfer->rx_cnt >= p_spi->xfer->num) ||
        (NULL != p_spi->rx_dma_drv)) // @suppress("Cast comment")
    {
        if (ARM_SPI_MODE_SLAVE == (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
        {
            p_spi->info->status.busy = 0;
            spi_transmit_stop(p_spi);
            
            if (NULL != p_spi->info->cb_event) // @suppress("Cast comment")
            {
                p_spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
            }
        }
        else
        {
            if ((NULL == p_spi->rx_dma_drv)// @suppress("Cast comment")
#if (0 != SPI_PRV_USED_RX_DTC_DRV)
                  || ((&Driver_DTC) == p_spi->rx_dma_drv)
#endif
                  )
            {
                /* SPCR - SPI Control Register
                   b7   - SPRIE  - SPI Receive Buffer Full Interrupt Enable
                                 - Disables the generation of SPI receive buffer full interrupt requests */
                p_spi->reg->SPCR_b.SPRIE = 0;
                R_NVIC_DisableIRQ(p_spi->spri_irq);
            }
        }
    }

}/* End of function r_spri_handler() */

/****************************************************************************************************************//**
* @brief      Transmit interrupt handling function
* @param[in]  p_spi    Pointer to SPI resources
**********************************************************************************************************************/
/* Function Name: r_spti_handler */
static void r_spti_handler(st_spi_resources_t * const p_spi)
{
    if (1 == p_spi->reg->SPSR_b.UDRF)
    {
        /* In the case of an underrun error, the subsequent processing is not executed. */
        return;
    }
    if ((NULL == p_spi->tx_dma_drv) && // @suppress("Cast comment")
        (p_spi->xfer->tx_cnt < p_spi->xfer->num))
    {
        if (SPI_PRV_EXEC_RECEIVE == p_spi->xfer->exec_state)
        {
            /* In the case of reception operation, dummy data write */
            /* SPDR - SPI Data Register - Depends on variables */
            /* Switch the buffer size according to the bit size */
            if (p_spi->xfer->data_bits > 16)
            {
                /* SPDR - SPI Data Register - Depends on variables(32bit) */
                p_spi->reg->SPDR    = (uint32_t)p_spi->xfer->tx_def_val;
            }
            else if (p_spi->xfer->data_bits > 8)
            {
                /* SPDR - SPI Data Register - Depends on variables(16bit) */
                p_spi->reg->SPDR_HA = (uint16_t)p_spi->xfer->tx_def_val;
            }
            else
            {
                /* SPDR - SPI Data Register - Depends on variables(8bit) */
                p_spi->reg->SPDR_BY = (uint8_t)p_spi->xfer->tx_def_val;
            }
        }
        else
        {
            /* Switch the buffer size according to the bit size */
            if (p_spi->xfer->data_bits > 16)
            {
                /* SPDR - SPI Data Register - Depends on variables(32bit) */
                p_spi->reg->SPDR = ((uint32_t*)(p_spi->xfer->tx_buf))[p_spi->xfer->tx_cnt];
            }
            else if (p_spi->xfer->data_bits > 8)
            {
                /* SPDR - SPI Data Register - Depends on variables(16bit) */
                p_spi->reg->SPDR_HA = ((uint16_t*)(p_spi->xfer->tx_buf))[p_spi->xfer->tx_cnt];
            }
            else
            {
                /* SPDR - SPI Data Register - Depends on variables(8bit) */
                p_spi->reg->SPDR_BY = ((uint8_t*)(p_spi->xfer->tx_buf))[p_spi->xfer->tx_cnt];
            }
        }
        p_spi->xfer->tx_cnt++;
    }
    else
    {
        if ((NULL == p_spi->tx_dma_drv)// @suppress("Cast comment")
#if (0 != SPI_PRV_USED_TX_DTC_DRV)
              || ((&Driver_DTC) == p_spi->tx_dma_drv)
#endif
              )
        {
        /* SPTIE - Transmit Buffer Empty Interrupt Enable
             - Disables the generation of transmit buffer empty interrupt requests */
            p_spi->reg->SPCR_b.SPTIE = 0;         /* Disable SPI spti interrupt */
            R_NVIC_DisableIRQ(p_spi->spti_irq);
        }

#if (0 != SPI_PRV_USED_TX_DMAC_DRV)
        else
        {
            (void)p_spi->tx_dma_drv->InterruptDisable(); // @suppress("Cast comment")
        }
#endif

        if (ARM_SPI_MODE_SLAVE == (p_spi->info->mode & ARM_SPI_CONTROL_Msk))
        {
            if (SPI_PRV_EXEC_SEND == p_spi->xfer->exec_state)
            {
                R_NVIC_ClearPendingIRQ(p_spi->sptend_irq);
                R_SYS_IrqStatusClear(p_spi->sptend_irq);
                R_NVIC_EnableIRQ(p_spi->sptend_irq);
            }
        }
        else
        {
            /* SPIIE - SPI Idle Interrupt Enable - Enable the generation of idle interrupt requests */
            p_spi->reg->SPCR2_b.SPIIE = 1;        /* Enable SPI idle interrupt */
            R_NVIC_EnableIRQ(p_spi->spii_irq);
        }
    }
}/* End of function r_spti_handler() */

/****************************************************************************************************************//**
* @brief      Idle interrupt function
* @param[in]  p_spi    Pointer to SPI resources
**********************************************************************************************************************/
/* Function Name: r_spii_handler */
static void r_spii_handler(st_spi_resources_t * const p_spi)
{
    uint8_t ir_flg;

    /* In case of overrun error, disable SPII interrupt and exit. */
    if (0 == p_spi->reg->SPSR_b.OVRF)
    {
        p_spi->info->status.busy = 0;

        /* Check if SPRI is valid. */
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_spi->spri_irq)
        {
            R_SYS_IrqStatusGet(p_spi->spri_irq, &ir_flg);
            if (0 != ir_flg)
            {
                /* In order to receive data that has not been read, SPRI function is called. */
                r_spri_handler(p_spi);
            }
        }
        spi_transmit_stop(p_spi);
        if (NULL != p_spi->info->cb_event) // @suppress("Cast comment")
        {
            p_spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
        }
    }
    else
    {
        R_NVIC_DisableIRQ(p_spi->spii_irq);
    }
}/* End of function r_spii_handler() */

/****************************************************************************************************************//**
* @brief      Reception error interrupt processing
* @param[in]  p_spi    Pointer to SPI resources
**********************************************************************************************************************/
/* Function Name: r_spei_handler */
static void r_spei_handler(st_spi_resources_t * const p_spi)
{
    uint32_t event = 0;
    volatile uint32_t dummy;
    
    /* Underrun error? */
    if (1U == p_spi->reg->SPSR_b.UDRF)
    {
        p_spi->info->status.data_lost = 1;
        event |= ARM_SPI_EVENT_DATA_LOST;
    }

    /* Overrun error? */
    if (1U == p_spi->reg->SPSR_b.OVRF)
    {
        p_spi->info->status.data_lost = 1;
        event |= ARM_SPI_EVENT_DATA_LOST;
    }

    p_spi->info->status.busy = 0;
    spi_transmit_stop(p_spi);

    /* SPSR - SPI Status Register
        b4  - UDRF - Underrun Error Flag - Neither a mode fault error nor an underrun error occurs
        b3  - PERF - Parity Error Flag - No parity error occurs
        b2  - MODF - Mode Fault Error Flag - Neither a mode fault error nor an underrun error occurs
        b1  - IDLNF - SPI Idle Flag - SPI is in the idle state
        b0  - OVRF - Overrun Error Flag - No overrun error occurs */
    p_spi->reg->SPSR = 0xA0;                  /* Clear SPE,OVRF,IDLNF,MODF,PERF,UDRF */

    dummy = p_spi->reg->SPDR;   /* Dummy read the value remaining in SPDR. */

    /* Callback function */
    if ((NULL != p_spi->info->cb_event) && (0 != event))
    {
        p_spi->info->cb_event(event);
    }

}/* End of function r_spei_handler() */

/****************************************************************************************************************//**
* @brief      send completely interrupt processing
* @param[in]  p_spi    Pointer to SPI resources
**********************************************************************************************************************/
/* Function Name: r_sptend_handler */
static void r_sptend_handler(st_spi_resources_t * const p_spi)
{
    spi_transmit_stop(p_spi);
    p_spi->info->status.busy = 0;
    if (NULL != p_spi->info->cb_event) // @suppress("Cast comment")
    {
        p_spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
    }
}/* End of function r_sptend_handler() */

/* SPI0 */
#if (0 != R_SPI0_ENABLE)
static int32_t SPI0_Initialize(ARM_SPI_SignalEvent_t cb_event) FUNC_LOCATION_ARM_SPI_INITIALIZE;
static int32_t SPI0_Uninitialize(void) FUNC_LOCATION_ARM_SPI_UNINITIALIZE;
static int32_t SPI0_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_SPI_POWERCONTROL;
static int32_t SPI0_Send (void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_SPI_SEND;
static int32_t SPI0_Receive (void * p_data, uint32_t num) FUNC_LOCATION_ARM_SPI_RECEIVE;
static int32_t SPI0_Transfer(const void *p_data_out, void *p_data_in, uint32_t num) FUNC_LOCATION_ARM_SPI_TRANSFER;
static uint32_t SPI0_GetDataCount(void) FUNC_LOCATION_ARM_SPI_GET_DATACOUNT;
static int32_t SPI0_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_SPI_CONTROL;
static ARM_SPI_STATUS SPI0_GetStatus(void) FUNC_LOCATION_ARM_SPI_GET_STATUS;
static void spi0_spti_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPTI_HANDLER;
static void spi0_spri_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPRI_HANDLER;
static void spi0_spii_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPII_HANDLER;
static void spi0_spei_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPEI_HANDLER;
static void spi0_sptend_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPTEND_HANDLER;

static st_spi_info_t gs_spi0_info = {0};
static st_spi_transfer_info_t gs_spi0_xfer_info = {0};

#if (SPI_USED_DTC == SPI0_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  spi0_tx_dtc_info;
#endif
#if (SPI_USED_DTC == SPI0_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  spi0_rx_dtc_info;
#endif

/* SPI0 Resources */
static st_spi_resources_t gs_spi0_resources DATA_LOCATION_PRV_SPI_RESOURCES =
{
    (SPI0_Type*)SPI0, // @suppress("Cast comment")
    R_RSPI_Pinset_CH0,                 /* pin_set */
    R_RSPI_Pinclr_CH0,                 /* pin_clr */
#ifdef SPI0_SS_PORT
    &SPI0_SS_PORT,
#else
    NULL, // @suppress("Cast comment")
#endif
    SPI0_SS_PIN,
    &gs_spi0_info,
    &gs_spi0_xfer_info,
    SYSTEM_LOCK_SPI0,
    LPM_MSTP_SPI0,
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI0_SPTI, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI0_SPRI, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI0_SPII, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI0_SPEI, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI0_SPTEND, // @suppress("Cast comment")
    0x000000011,
    0x000000012,
    0x000000010,
    0x000000011,
    0x000000011,
    SPI0_SPTI_PRIORITY,
    SPI0_SPRI_PRIORITY,
    SPI0_SPII_PRIORITY,
    SPI0_SPEI_PRIORITY,
    SPI0_SPTEND_PRIORITY,
#if (SPI_USED_DMAC0 == SPI0_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    SPI_SPTI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC1 == SPI0_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    SPI_SPTI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC2 == SPI0_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    SPI_SPTI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC3 == SPI0_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    SPI_SPTI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DTC == SPI0_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SPI0_SPTI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SPI_USED_DMAC0 == SPI0_RECEIVE_CONTROL)
    &Driver_DMAC0,
    SPI_SPRI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC1 == SPI0_RECEIVE_CONTROL)
    &Driver_DMAC1,
    SPI_SPRI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC2 == SPI0_RECEIVE_CONTROL)
    &Driver_DMAC2,
    SPI_SPRI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC3 == SPI0_RECEIVE_CONTROL)
    &Driver_DMAC3,
    SPI_SPRI0_DMAC_SOURCE_ID,
#elif (SPI_USED_DTC == SPI0_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SPI0_SPRI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SPI_PRV_USED_DTC_DRV != 0)
#if (SPI_USED_DTC == SPI0_TRANSMIT_CONTROL)
    &spi0_tx_dtc_info,
#else
    NULL, // @suppress("Cast comment")
#endif
#if (SPI_USED_DTC == SPI0_RECEIVE_CONTROL)
    &spi0_rx_dtc_info,
#else
    NULL, // @suppress("Cast comment")
#endif
#endif
    spi0_spti_interrupt,
    spi0_spri_interrupt,
    spi0_spii_interrupt,
    spi0_spei_interrupt,
    spi0_sptend_interrupt
};

/****************************************************************************************************************//**
 * @brief SPI initialize method for user applications to initialize the SPI channel0. User applications should call
 * this method through the Driver Instance Driver_SPI0.Initialize().
 *
 * @param[in] cb_event   Callback event for SPI.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_Initialize */
static int32_t SPI0_Initialize(ARM_SPI_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Initialize(cb_event, &gs_spi0_resources));
}/* End of function SPI0_Initialize() */

/****************************************************************************************************************//**
 * @brief SPI uninitialize method for user applications to initialize the SPI channel0. User applications should call
 * this method through the Driver Instance Driver_SPI0.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_Uninitialize */
static int32_t SPI0_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Uninitialize(&gs_spi0_resources));
}/* End of function SPI0_Uninitialize() */

/****************************************************************************************************************//**
 * @brief SPI power control method for user applications to control the power of SPI channel0. User applications
 * should call this method through the Driver Instance Driver_SPI0.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_PowerControl */
static int32_t SPI0_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_SPI_PowerControl(state, &gs_spi0_resources));
}/* End of function SPI0_PowerControl() */

/****************************************************************************************************************//**
 * @brief SPI data transmission method for user applications to send data over SPI channel0. User applications should
 * call this method through the Driver Instance Driver_SPI0.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_Send */
static int32_t SPI0_Send (void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Send(p_data, num, &gs_spi0_resources));
}/* End of function SPI0_Send() */

/****************************************************************************************************************//**
 * @brief SPI data reception method for user applications to receive data over SPI channel0. User applications should
 * call this method through the Driver Instance Driver_SPI0.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_Receive */
static int32_t SPI0_Receive (void * p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Receive(p_data, num, &gs_spi0_resources));
}/* End of function SPI0_Receive() */

/****************************************************************************************************************//**
 * @brief SPI data tranmission method for user applications to send and receive data over SPI channel0 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_SPI0.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/*  Function Name: SPI0_Transfer */
static int32_t SPI0_Transfer(const void *p_data_out, void *p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Transfer(p_data_out, p_data_in, num, &gs_spi0_resources));
}/* End of function SPI0_Transfer() */

/****************************************************************************************************************//**
 * @brief SPI data transmission count getter method for user applications to get the transmission counter value for
 * the TX channel on SPI channel0. User applications should call this method through the Driver Instance
 * Driver_SPI0.GetCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_GetDataCount */
static uint32_t SPI0_GetDataCount(void) // @suppress("Non-API function naming")
{
    return (ARM_SPI_GetDataCount(&gs_spi0_resources));
}/* End of function SPI0_GetDataCount() */

/****************************************************************************************************************//**
 * @brief SPI driver control method for user applications to set SPI configuration for SPI channel0. User 
 * applications should call this method through the Driver Instance Driver_SPI0.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_Control */
static int32_t SPI0_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Control(control, arg, &gs_spi0_resources));
}/* End of function SPI0_Control() */

/****************************************************************************************************************//**
 * @brief SPI status getter method for user applications to get the driver status for SPI channel0. User applications
 * should call this method through the Driver Instance Driver_SPI0.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI0_GetStatus */
static ARM_SPI_STATUS SPI0_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_SPI_GetStatus(&gs_spi0_resources));
}/* End of function SPI0_GetStatus() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI0
 *******************************************************************************************************************/
/* Function Name: spi0_spti_interrupt */
static void spi0_spti_interrupt(void) // @suppress("Function definition ordering") @suppress("ISR declaration")
{
    r_spti_handler(&gs_spi0_resources);
}/* End of function spi0_spti_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI0
 *******************************************************************************************************************/
/* Function Name: spi0_spri_interrupt */
static void spi0_spri_interrupt(void) // @suppress("ISR declaration")
{
    r_spri_handler(&gs_spi0_resources);
}/* End of function spi0_spri_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI0
 *******************************************************************************************************************/
/* Function Name: spi0_spii_interrupt */
static void spi0_spii_interrupt(void) // @suppress("ISR declaration")
{
    r_spii_handler(&gs_spi0_resources);
}/* End of function spi0_spii_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI0
 *******************************************************************************************************************/
/* Function Name: spi0_spei_interrupt */
static void spi0_spei_interrupt(void) // @suppress("ISR declaration")
{
    r_spei_handler(&gs_spi0_resources);
}/* End of function spi0_spei_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI0
 *******************************************************************************************************************/
/* Function Name: spi0_sptend_interrupt */
static void spi0_sptend_interrupt(void) // @suppress("ISR declaration")
{
    r_sptend_handler(&gs_spi0_resources);
}/* End of function spi0_sptend_interrupt() */

ARM_DRIVER_SPI Driver_SPI0 DATA_LOCATION_PRV_DRIVER_SPI =
{
    ARM_SPI_GetVersion,
    ARM_SPI_GetCapabilities,
    SPI0_Initialize,
    SPI0_Uninitialize,
    SPI0_PowerControl,
    SPI0_Send,
    SPI0_Receive,
    SPI0_Transfer,
    SPI0_GetDataCount,
    SPI0_Control,
    SPI0_GetStatus
};

#endif /* (0 != R_SPI0_ENABLE) */

/* SPI1 */
#if (0 != R_SPI1_ENABLE)
static int32_t SPI1_Initialize(ARM_SPI_SignalEvent_t cb_event) FUNC_LOCATION_ARM_SPI_INITIALIZE;
static int32_t SPI1_Uninitialize(void) FUNC_LOCATION_ARM_SPI_UNINITIALIZE;
static int32_t SPI1_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_ARM_SPI_POWERCONTROL;
static int32_t SPI1_Send (void const * const p_data, uint32_t num) FUNC_LOCATION_ARM_SPI_SEND;
static int32_t SPI1_Receive (void * p_data, uint32_t num) FUNC_LOCATION_ARM_SPI_RECEIVE;
static int32_t SPI1_Transfer(const void *p_data_out, void *p_data_in, uint32_t num) FUNC_LOCATION_ARM_SPI_TRANSFER;
static uint32_t SPI1_GetDataCount(void) FUNC_LOCATION_ARM_SPI_GET_DATACOUNT;
static int32_t SPI1_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_ARM_SPI_CONTROL;
static ARM_SPI_STATUS SPI1_GetStatus(void) FUNC_LOCATION_ARM_SPI_GET_STATUS;
static void spi1_spti_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPTI_HANDLER;
static void spi1_spri_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPRI_HANDLER;
static void spi1_spii_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPII_HANDLER;
static void spi1_spei_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPEI_HANDLER;
static void spi1_sptend_interrupt(void) STATIC_FUNC_LOCATION_PRV_R_SPTEND_HANDLER;

static st_spi_info_t gs_spi1_info = {0};
static st_spi_transfer_info_t gs_spi1_xfer_info = {0};

#if (SPI_USED_DTC == SPI1_TRANSMIT_CONTROL)
  static  st_dma_transfer_data_t  spi1_tx_dtc_info;
#endif
#if (SPI_USED_DTC == SPI1_RECEIVE_CONTROL)
  static  st_dma_transfer_data_t  spi1_rx_dtc_info;
#endif

/* SPI1 Resources */
static st_spi_resources_t gs_spi1_resources DATA_LOCATION_PRV_SPI_RESOURCES =
{
    (SPI0_Type*)SPI1, // @suppress("Cast comment")
    R_RSPI_Pinset_CH1,                 /* pin_set */
    R_RSPI_Pinclr_CH1,                 /* pin_clr */
#ifdef SPI1_SS_PORT
    &SPI1_SS_PORT,
#else
    NULL, // @suppress("Cast comment")
#endif
    SPI1_SS_PIN,
    &gs_spi1_info,
    &gs_spi1_xfer_info,
    SYSTEM_LOCK_SPI1,
    LPM_MSTP_SPI1,
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI1_SPTI, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI1_SPRI, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI1_SPII, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI1_SPEI, // @suppress("Cast comment")
    (IRQn_Type)SYSTEM_CFG_EVENT_NUMBER_SPI1_SPTEND, // @suppress("Cast comment")
    0x00000001D,
    0x00000001D,
    0x00000001C,
    0x00000001D,
    0x00000001D,
    SPI1_SPTI_PRIORITY,
    SPI1_SPRI_PRIORITY,
    SPI1_SPII_PRIORITY,
    SPI1_SPEI_PRIORITY,
    SPI1_SPTEND_PRIORITY,
#if (SPI_USED_DMAC0 == SPI1_TRANSMIT_CONTROL)
    &Driver_DMAC0,
    SPI_SPTI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC1 == SPI1_TRANSMIT_CONTROL)
    &Driver_DMAC1,
    SPI_SPTI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC2 == SPI1_TRANSMIT_CONTROL)
    &Driver_DMAC2,
    SPI_SPTI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC3 == SPI1_TRANSMIT_CONTROL)
    &Driver_DMAC3,
    SPI_SPTI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DTC == SPI1_TRANSMIT_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SPI1_SPTI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SPI_USED_DMAC0 == SPI1_RECEIVE_CONTROL)
    &Driver_DMAC0,
    SPI_SPRI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC1 == SPI1_RECEIVE_CONTROL)
    &Driver_DMAC1,
    SPI_SPRI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC2 == SPI1_RECEIVE_CONTROL)
    &Driver_DMAC2,
    SPI_SPRI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DMAC3 == SPI1_RECEIVE_CONTROL)
    &Driver_DMAC3,
    SPI_SPRI1_DMAC_SOURCE_ID,
#elif (SPI_USED_DTC == SPI1_RECEIVE_CONTROL)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_SPI1_SPRI,
#else
    NULL, // @suppress("Cast comment")
    0,
#endif
#if (SPI_PRV_USED_DTC_DRV != 0)
#if (SPI_USED_DTC == SPI1_TRANSMIT_CONTROL)
    &spi1_tx_dtc_info,
#else
    NULL, // @suppress("Cast comment")
#endif
#if (SPI_USED_DTC == SPI1_RECEIVE_CONTROL)
    &spi1_rx_dtc_info,
#else
    NULL, // @suppress("Cast comment")
#endif
#endif

    spi1_spti_interrupt,
    spi1_spri_interrupt,
    spi1_spii_interrupt,
    spi1_spei_interrupt,
    spi1_sptend_interrupt
};

/****************************************************************************************************************//**
 * @brief SPI initialize method for user applications to initialize the SPI channel1. User applications should call
 * this method through the Driver Instance Driver_SPI1.Initialize().
 *
 * @param[in] cb_event   Callback event for SPI.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_Initialize */
static int32_t SPI1_Initialize(ARM_SPI_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Initialize(cb_event, &gs_spi1_resources));
}/* End of function SPI1_Initialize() */

/****************************************************************************************************************//**
 * @brief SPI uninitialize method for user applications to initialize the SPI channel1. User applications should call
 * this method through the Driver Instance Driver_SPI1.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_Uninitialize */
static int32_t SPI1_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Uninitialize(&gs_spi1_resources));
}/* End of function SPI1_Uninitialize() */

/****************************************************************************************************************//**
 * @brief SPI power control method for user applications to control the power of SPI channel1. User applications
 * should call this method through the Driver Instance Driver_SPI1.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_PowerControl */
static int32_t SPI1_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_SPI_PowerControl(state, &gs_spi1_resources));
}/* End of function SPI1_PowerControl() */

/****************************************************************************************************************//**
 * @brief SPI data transmission method for user applications to send data over SPI channel1. User applications should
 * call this method through the Driver Instance Driver_SPI1.Send().
 * @param[in]       p_data    Pointer to output data
 * @param[in]       num     Number of data to transmit
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_Send */
static int32_t SPI1_Send (void const * const p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Send(p_data, num, &gs_spi1_resources));
}/* End of function SPI1_Send() */

/****************************************************************************************************************//**
 * @brief SPI data reception method for user applications to receive data over SPI channel1. User applications should
 * call this method through the Driver Instance Driver_SPI1.Receive().
 * @param[in]       p_data    Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_Receive */
static int32_t SPI1_Receive (void * p_data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Receive(p_data, num, &gs_spi1_resources));
}/* End of function SPI1_Receive() */

/****************************************************************************************************************//**
 * @brief SPI data tranmission method for user applications to send and receive data over SPI channel1 in the clock
 * synchronous mode. User applications should call this method through the Driver Instance Driver_SPI1.Transfer().
 * @param[in]       p_data_out    Pointer to output data
 * @param[in]       p_data_in     Pointer to input data
 * @param[in]       num     Number of data to receive
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_Transfer */
static int32_t SPI1_Transfer(const void *p_data_out, void *p_data_in, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Transfer(p_data_out, p_data_in, num, &gs_spi1_resources));
}/* End of function SPI1_Transfer() */

/****************************************************************************************************************//**
 * @brief SPI data transmission count getter method for user applications to get the transmission counter value
 * on SPI channel0. User applications should call this method through the Driver Instance
 * Driver_SPI1.GetCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_GetDataCount */
static uint32_t SPI1_GetDataCount(void) // @suppress("Non-API function naming")
{
    return (ARM_SPI_GetDataCount(&gs_spi1_resources));
}/* End of function SPI1_GetDataCount() */

/****************************************************************************************************************//**
 * @brief SPI driver control method for user applications to set SPI configuration for SPI channel1. User 
 * applications should call this method through the Driver Instance Driver_SPI1.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_Control */
static int32_t SPI1_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_SPI_Control(control, arg, &gs_spi1_resources));
}/* End of function SPI1_Control() */

/****************************************************************************************************************//**
 * @brief SPI status getter method for user applications to get the driver status for SPI channel1. User applications
 * should call this method through the Driver Instance Driver_SPI1.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_SPI_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name: SPI1_GetStatus */
static ARM_SPI_STATUS SPI1_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_SPI_GetStatus(&gs_spi1_resources));
}/* End of function SPI1_GetStatus() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI1
 *******************************************************************************************************************/
/* Function Name: spi1_spti_interrupt */
static void spi1_spti_interrupt(void) // @suppress("ISR declaration")
{
    r_spti_handler(&gs_spi1_resources);
}/* End of function spi1_spti_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI1
 *******************************************************************************************************************/
/* Function Name: spi1_spri_interrupt */
static void spi1_spri_interrupt(void) // @suppress("ISR declaration")
{
    r_spri_handler(&gs_spi1_resources);
}/* End of function spi1_spri_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI1
 *******************************************************************************************************************/
/* Function Name: spi1_spii_interrupt */
static void spi1_spii_interrupt(void) // @suppress("ISR declaration")
{
    r_spii_handler(&gs_spi1_resources);
}/* End of function spi1_spii_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI1
 *******************************************************************************************************************/
/* Function Name: spi1_spei_interrupt */
static void spi1_spei_interrupt(void) // @suppress("ISR declaration")
{
    r_spei_handler(&gs_spi1_resources);
}/* End of function spi1_spei_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for SPI1
 *******************************************************************************************************************/
/* Function Name: spi1_sptend_interrupt */
static void spi1_sptend_interrupt(void) // @suppress("ISR declaration")
{
    r_sptend_handler(&gs_spi1_resources);
}/* End of function spi1_sptend_interrupt() */

ARM_DRIVER_SPI Driver_SPI1 DATA_LOCATION_PRV_DRIVER_SPI =
{
    ARM_SPI_GetVersion,
    ARM_SPI_GetCapabilities,
    SPI1_Initialize,
    SPI1_Uninitialize,
    SPI1_PowerControl,
    SPI1_Send,
    SPI1_Receive,
    SPI1_Transfer,
    SPI1_GetDataCount,
    SPI1_Control,
    SPI1_GetStatus
};

#endif /* (0 != R_SPI1_ENABLE) */

/******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_spi)
 *********************************************************************************************************************/
 /* End of file (r_spi_cmsis_api.c) */
