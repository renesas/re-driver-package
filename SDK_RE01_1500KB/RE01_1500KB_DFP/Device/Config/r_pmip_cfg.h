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
* Copyright (C) 2019-2021 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_pmip_cfg.h
* Version      : 1.40
* Description  : HAL-Driver for PMIP
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.06.2019 0.72    API Release 
*         : 23.07.2019 1.00    Upgrade version to 1.00
*         : 18.02.2020 1.10    Configurable value of full screen rewrite data transmitted during power-on sequence.
*         : 27.08.2020 1.20    Fixed not to include header file when DMA is not used.
*                              Corrected comments according to "User's Manual:Hardware(R01UH0796)"
*         : 12.01.2021 1.40    Add "PMIP_CFG_R_PMIP_RECONFIG"
***********************************************************************************************************************/
#ifndef R_PMIP_CFG_H
#define R_PMIP_CFG_H

/******************************************************************************************************************//**
 * @addtogroup grp_device_hal_pmip
 * @{
 *********************************************************************************************************************/

/******************************************************************************
Includes <System Includes> , "Project Includes"
*******************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

#include "r_system_api.h"

/******************************************************************************
Macro definitions
*******************************************************************************/
/* DMAC transfer enable setting */
/* 0:disable 1:enable */  
#define PMIP_CFG_SUPPORT_DMAC            (1)
/* 0:disable  */  
/* Set "0" because it does not support DTC. */
#define PMIP_CFG_SUPPORT_DTC             (0)

/* horizontal size of MIP-LCD (pixel number) */
#define PMIP_CFG_DISP_WIDTH      (176)        

/* vertical size of MIP-LCD (pixel number) */
#define PMIP_CFG_DISP_HEIGHT     (176)        

/* transmission clock high width (us) */
#define PMIP_CFG_SCLKH           (1)

/* VCOM Output High-Width Setting */
/* 0:500ms  1:1000ms  2:2000ms  3:5000ms */        
#define PMIP_CFG_VCOM_CLK        (0)

/* PMIP configure parameter checking enable */
/* 0:disable 1:enable */        
#define PMIP_CFG_PARAM_CK_EN     (1)

/* enable signal control table selection */
/* 0:Kyocera 1:other */        
#define PMIP_CFG_ENB_TBL         (1)

/* MLCD interrupt priority value */
#define PMIP_CFG_INTERRUPT_LEVEL (0)  /* 0~3, 0 is highest priority */      

/* Value of Initialization data for MIP display when power-on API call   */
/* When Value of "1" is send 0xFF and Value of "0" is send 0x00          */
/* It should set to suit your MIP LCD specification of default value     */
#define PMIP_CFG_DISP_INI_DATA  (1)

/*!< PMIP Function */
#define PMIP_CFG_R_PMIP_GETVERSION                 (SYSTEM_SECTION_CODE)        /*!< R_PMIP_GetVersion                 () section    */
#define PMIP_CFG_R_PMIP_OPEN                       (SYSTEM_SECTION_CODE)        /*!< R_PMIP_Open                       () section    */
#define PMIP_CFG_R_PMIP_CLOSE                      (SYSTEM_SECTION_CODE)        /*!< R_PMIP_Close                      () section    */
#define PMIP_CFG_R_PMIP_POWERON                    (SYSTEM_SECTION_CODE)        /*!< R_PMIP_PowerOn                    () section    */
#define PMIP_CFG_R_PMIP_POWEROFF                   (SYSTEM_SECTION_CODE)        /*!< R_PMIP_PowerOff                   () section    */
#define PMIP_CFG_R_PMIP_CONTROL                    (SYSTEM_SECTION_CODE)        /*!< R_PMIP_Control                    () section    */
#define PMIP_CFG_R_PMIP_ALLZERO                    (SYSTEM_SECTION_CODE)        /*!< R_PMIP_AllZero                    () section    */
#define PMIP_CFG_R_PMIP_ALLONE                     (SYSTEM_SECTION_CODE)        /*!< R_PMIP_AllOne                     () section    */
#define PMIP_CFG_R_PMIP_SENDTRIM                   (SYSTEM_SECTION_CODE)        /*!< R_PMIP_SendTrim                   () section    */
#define PMIP_CFG_R_PMIP_SEND                       (SYSTEM_SECTION_CODE)        /*!< R_PMIP_Send                       () section    */
#define PMIP_CFG_R_PMIP_RECONFIG                   (SYSTEM_SECTION_CODE)        /*!< R_PMIP_Reconfig                   () section    */
#define PMIP_CFG_E_MLCD_ALLSET                     (SYSTEM_SECTION_CODE)        /*!< e_mlcd_allset                     () section    */
#define PMIP_CFG_V_MLCD_CLEAR_REG                  (SYSTEM_SECTION_CODE)        /*!< v_mlcd_clear_reg                  () section    */
#define PMIP_CFG_E_MLCD_PRIORITY_SET               (SYSTEM_SECTION_CODE)        /*!< e_mlcd_priority_set               () section    */
#define PMIP_CFG_V_VCOM_SET                        (SYSTEM_SECTION_CODE)        /*!< v_vocm_set                        () section    */
#define PMIP_CFG_E_MLCD_DMAC_CH_SEL                (SYSTEM_SECTION_CODE)        /*!< e_mlcd_dmac_ch_sel                () section    */
#define PMIP_CFG_E_MLCD_DMAC_NORMAL_TRANS          (SYSTEM_SECTION_CODE)        /*!< e_mlcd_dmac_normal_trans          () section    */
#define PMIP_CFG_E_MLCD_CPU_NORMAL_TRANS           (SYSTEM_SECTION_CODE)        /*!< e_mlcd_cpu_normal_trans           () section    */
#define PMIP_CFG_V_MLCD_NORMAL_DMAC_PRE_SET        (SYSTEM_SECTION_CODE)        /*!< e_normal_dmac_pre_set             () section    */
#define PMIP_CFG_V_MLCD_DMAC_DATA_TRANS            (SYSTEM_SECTION_CODE)        /*!< e_dmac_data_trans                 () section    */
#define PMIP_CFG_V_CPU_TRANS_NORMAL                (SYSTEM_SECTION_CODE)        /*!< v_cpu_trans_normal                () section    */
#define PMIP_CFG_E_SEND_CHECK                      (SYSTEM_SECTION_CODE)        /*!< e_send_check                      () section    */
#define PMIP_CFG_V_ISR_DMAC_NORMAL_TEMI            (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_normal_temi            () section    */
#define PMIP_CFG_V_ISR_DMAC_TEI                    (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_tei                    () section    */
#define PMIP_CFG_V_ISR_CPU_NORMAL_TEMI             (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_normal_temi             () section    */
#define PMIP_CFG_V_ISR_CPU_TEI                     (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_tei                     () section    */

#ifdef  __cplusplus
}
#endif

/******************************************************************************************************************//**
 * @} (end ingroup grp_device_hal_pmip)
 *********************************************************************************************************************/
#endif /* R_PMIP_CFG_H */

/* End of file (r_pmip_cfg.h) */
