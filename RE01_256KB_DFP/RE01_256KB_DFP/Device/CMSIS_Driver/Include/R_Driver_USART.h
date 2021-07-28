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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : R_Driver_USART.h
* Version      : 0.40
* Description  : CMSIS-Driver for USART
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
**********************************************************************************************************************/

#ifndef R_DRIVER_USART_H
#define R_DRIVER_USART_H
/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_usart
 * @{
 *********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif
#include "Driver_USART.h"

/******************************************************************************
 Macro definitions
 *****************************************************************************/
/*----- USART Control Codes: Miscellaneous Controls  -----*/
#define ARM_USART_CONTROL_TX_RX      (0x1BUL << ARM_USART_CONTROL_Pos)  ///< Transmitter and Receiver; arg: 
                                                                        ///  0=disabled, 1=enabled

#ifdef  __cplusplus
}
#endif

/******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_usart)
 *********************************************************************************************************************/
#endif /** R_DRIVER_USART_H */
/* End of file (R_Driver_USART.h) */

