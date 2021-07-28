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
* File Name    : r_dmac_api.h
* Version      : 1.40
* Description  : HAL Driver for DMAC
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.11.2018 0.51     First Release
*         : 04.12.2018 0.60     Update comments
*                               Adapted to GSCE coding rules.
*                               - Modified to include dmac_mode_info_t,st_dmac_resources_t definition 
*                                 in r_dmac_api.c
*                               Changed file name to r_dmac_api.h
*         : 07.03.2019 0.70     Upgrade version to 0.70
*         : 14.05.2019 0.72     Upgrade version to 0.72
*         : 02.07.2019 1.00     IO register header name change(E017.h -> RE01_1500KB.h)
*                               Upgrade version to 1.00
*         : 05.01.2021 1.40     Upgrade version to 1.40
***********************************************************************************************************************/

/******************************************************************************************************************//**
 @ingroup grp_device_hal
 @defgroup grp_device_hal_dmac Device HAL DMAC Implementation
 @brief The DMAC driver implementation for the HAL-Driver DMAC interface to drive the DMAC peripheral on RE Family.

 @section sec_hal_driver_dmac_summary DMAC-Driver DMAC Summary
 This is the HAL-Driver DMAC implementation which drives the DMAC peripheral on RE Family via the DMAC-Driver interface.
 Use the instance @ref Driver_DMAC to acccess to DMAC peripheral. Do not use R_DMAC_XXX APIs directly. 
 @code
 ...

 Driver_DMAC0->Open();                  // Open DMAC driver

 ...
 @endcode

 @section HAL_Driver_DMAC_Specific_Error_Code HAL-Driver DMAC Specific Error Code
 See DMA_ERROR_XXX in r_dma_common_api.h for the detail.

 @{
 *********************************************************************************************************************/

#ifndef R_DMAC_H
#define R_DMAC_H

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
#include "RE01_1500KB.h"
#include "r_lpm_api.h"
#include "r_system_api.h"
#include "r_dma_common_api.h"
#include "r_dmac_cfg.h"


/******************************************************************************
Macro definitions
*******************************************************************************/
/** Version Number of API. */
#define DMAC_VERSION_MAJOR              (1)         /*!< Major Version */
#define DMAC_VERSION_MINOR              (40)        /*!< Minor Version */

#define DMA_CLR_STATE_ESIF              (1U<<1)         /*!< Clear DMSTS.ESIF flag */
#define DMA_CLR_STATE_DTIF              (1U<<2)         /*!< Clear DMSTS.DTIF flag */
#define DMA_CLR_STATE_SOFT_REQ          (1U<<3)         /*!< Clear DMREQ.SWREQ flag */

#define DMAC_TRANSFER_REQUEST_SOFTWARE  (-1)            /*!< Use this definition when specifying a software trigger  
                                                             as a DMA transfer factor                                */

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/

#ifdef  __cplusplus
}
#endif

#endif /* R_DMAC_H */

/******************************************************************************************************************//**
 * @} (end defgroup grp_device_hal_dmac)
 *********************************************************************************************************************/
/* End of file (r_dmac_api.h) */
