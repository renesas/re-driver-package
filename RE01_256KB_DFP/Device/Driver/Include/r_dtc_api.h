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
* File Name    : r_dtc_api.h
* Version      : 1.20
* Description  : HAL Driver for DTC
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 04.02.2020 0.80     Upgrade version to 0.80
*         : 27.08.2020 1.10     Upgrade version to 1.10
*         : 05.01.2021 1.20     Upgrade version to 1.20
***********************************************************************************************************************/

/******************************************************************************************************************//**
 @ingroup grp_device_hal
 @defgroup grp_device_hal_dtc Device HAL DTC Implementation
 @brief The DTC driver implementation for the HAL-Driver DTC interface to drive the DTC peripheral on RE Family.

 @section sec_hal_driver_dtc_summary DTC-Driver DTC Summary
 This is the HAL-Driver DTC implementation which drives the DTC peripheral on RE Family via the DTC-Driver interface.
 Use the instance @ref Driver_DTC to acccess to DTC peripheral. Do not use R_DTC_XXX APIs directly. 
 @code
 ...

 Driver_DTC->Open();                  // Open DTC driver

 ...
 @endcode

 @section HAL_Driver_DTC_Specific_Error_Code HAL-Driver DTC Specific Error Code
 See DMA_ERROR_XXX in r_dma_common_api.h for the detail.

 @{
 *********************************************************************************************************************/

#ifndef R_DTC_H
#define R_DTC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "RE01_256KB.h"
#include "r_lpm_api.h"
#include "r_system_api.h"
#include "r_dma_common_api.h"
#include "r_dtc_cfg.h"


/******************************************************************************
Macro definitions
*******************************************************************************/
/** Version Number of API. */
#define DTC_VERSION_MAJOR               (1)         /*!< Major Version */
#define DTC_VERSION_MINOR               (20)        /*!< Minor Version */

/*****************************************************************************
Typedef definitions(DTC only definitions)
******************************************************************************/
/** DTC Transfer infomation data */
typedef struct {
    uint32_t            mra_mrb;                    /*!< DMA Mode Register A and B.    */
    uint32_t            sar;                        /*!< DMA Transfer Source Register.  */
    uint32_t            dar;                        /*!< DMA Transfer Destination Register. */
    uint32_t            cra_crb;                    /*!< DMA Transfer Count Register A and B.  */
}st_dma_transfer_data_t;

/** DMA Chain abort target */
typedef struct st_dma_chain_abort {
    IRQn_Type           act_src;                    /*!< DMA abort target action source.      */
    uint16_t            chain_transfer_nr;          /*!< DMA abort target chain total number. */
}st_dma_chain_abort_t;


/** DMA Forcibly change infomation data */
typedef struct st_dma_chg_data{ 
    IRQn_Type               act_src;                /*!< DMA change target action source.      */
    uint32_t                chain_transfer_nr;      /*!< DMA change target chain numbers.      */
    st_dma_transfer_data_cfg_t cfg_data;            /*!< DMA change infomation data.           */
}st_dma_chg_data_t;

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/

#ifdef  __cplusplus
}
#endif

#endif /* R_DTC_H */

/******************************************************************************************************************//**
 * @} (end defgroup grp_hal_drv_impl_dtc)
 *********************************************************************************************************************/
/* End of file (r_dtc_api.h) */
