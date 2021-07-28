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
 * File Name    : r_dmac_cfg.h
 * Version      : 1.40
 * Description  : HAL-Driver for DMAC
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.11.2018 0.51     First Release
 *         : 04.12.2018 0.60     Update comments
 *                               Adapted to GSCE coding rules.
 *         : 02.07.2019 1.00     Add R_DMAC_GetTransferByte function
 *         : 05.01.2021 1.40     Add R_DMAC_GetAvailabilitySrc and R_DMAC_Release function
 ***********************************************************************************************************************/

#ifndef R_DMAC_CFG_H
#define R_DMAC_CFG_H
/******************************************************************************************************************//**
 @addtogroup grp_device_hal_dmac
 @{
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
#define DMAC_CFG_PARAM_CHECKING_ENABLE      (1)             ///< 0:Disable Parameter Check 1:Enable Parameter Check

#define DMAC0_INT_PRIORITY                  (3)             ///< DMAC0 interrupt priority value
    ///  (set to 0 to 3, 0 is highest priority.)
#define DMAC1_INT_PRIORITY                  (3)             ///< DMAC1 interrupt priority value
    ///  (set to 0 to 3, 0 is highest priority.)
#define DMAC2_INT_PRIORITY                  (3)             ///< DMAC2 interrupt priority value
    ///  (set to 0 to 3, 0 is highest priority.)
#define DMAC3_INT_PRIORITY                  (3)             ///< DMAC3 interrupt priority value
    ///  (set to 0 to 3, 0 is highest priority.)

/******************************************************************************************************************//**
 * @name R_DMAC_API_LOCATION_CONFIG
 *       Definition of R_DMAC API location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define DMAC_CFG_R_DMAC_GET_VERSION          (SYSTEM_SECTION_CODE)         ///< R_DMAC_GetVersion() section
#define DMAC_CFG_R_DMAC_OPEN                 (SYSTEM_SECTION_CODE)         ///< R_DMAC_Open() section
#define DMAC_CFG_R_DMAC_CLOSE                (SYSTEM_SECTION_CODE)         ///< R_DMAC_Close() section
#define DMAC_CFG_R_DMAC_CREATE               (SYSTEM_SECTION_CODE)         ///< R_DMAC_Create() section
#define DMAC_CFG_R_DMAC_CONTROL              (SYSTEM_SECTION_CODE)         ///< R_DMAC_Control() section
#define DMAC_CFG_R_DMAC_GET_STATE            (SYSTEM_SECTION_CODE)         ///< R_DMAC_GetState() section
#define DMAC_CFG_R_DMAC_CLEAR_STATE          (SYSTEM_SECTION_CODE)         ///< R_DMAC_ClearState() section
#define DMAC_CFG_R_DMAC_GET_TRANSFER_BYTE    (SYSTEM_SECTION_CODE)         ///< R_DMAC_GetTransferByte() section
#define DMAC_CFG_R_DMAC_INTERRUPT_ENABLE     (SYSTEM_SECTION_CODE)         ///< R_DMAC_InterruptEnable() section
#define DMAC_CFG_R_DMAC_INTERRUPT_DISABLE    (SYSTEM_SECTION_CODE)         ///< R_DMAC_InterruptDisable() section
#define DMAC_CFG_R_DMAC_GET_AVAILABILITY_SRC (SYSTEM_SECTION_CODE)         ///< R_DMAC_GetAvailabilitySrc() section
#define DMAC_CFG_R_DMAC_RELEASE              (SYSTEM_SECTION_CODE)         ///< R_DMAC_Release() section
#define DMAC_CFG_DMAC_INTERRUPT_HANDLER      (SYSTEM_SECTION_RAM_FUNC)     ///< dmac_interrupt_handler() section
/* @} */

#ifdef  __cplusplus
}
#endif
/*******************************************************************************************************************//**
 * @} (end addtogroup grp_hal_drv_dmac)
 **********************************************************************************************************************/

#endif /* R_DMAC_CFG_H */

/* End of file (r_dmac_cfg.h) */

