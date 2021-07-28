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
* Copyright (C) 2018-2019 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_i2c_cmsis.h
* Version      : 0.40
* Description  : CMSIS-Driver for I2C.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
**********************************************************************************************************************/

/******************************************************************************************************************//**
 @ingroup grp_cmsis_drv_imp
 @defgroup grp_cmsis_drv_impl_iic CMSIS-Driver IIC Implementation
 @brief The IIC driver implementation for the CMSIS-Driver IIC interface to drive the IIC peripheral on RE Family.

 @section sec_cmsis_driver_i2c_summary CMSIS-Driver IIC Summary
 This is the CMSIS-Driver IIC implementation which drives the IIC peripheral on RE Family via the CMSIS-Driver I2C
 interface.
 Use the instance @ref Driver_IIC0 to acccess to IIC peripheral channel 0. Do not use ARM_I2C_XXX APIs,
 or IIC0_XXX functions directly. For instance, when accessing the IIC channel 0, use the IIC Driver Instance 
 Driver_IIC0, which is an instance of 'Access Struct' @ref ARM_DRIVER_I2C for IIC channel 0.
 Following is a code example to open IIC channel 0 and power up the UART peripheral.
 @code
 ...
 Driver_RIIC0.Initialize();

 Driver_RIIC0.PowerControl(ARM_POWER_FULL);
 ...
 @endcode

 @section CMSIS_Driver_I2C_Specific_Error_Code CMSIS-Driver I2C Specific Error Code
 See ARM_I2C_ERROR_XXX in Driver_I2C.h for the detail.

 @{
 *********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#include "RE01_256KB.h"
#include "pin.h"
#include "R_Driver_I2C.h"
#include "r_i2c_cfg.h"
#include "r_system_api.h"
#include "r_system_cfg.h"
#include "r_lpm_api.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#ifndef R_I2C_CMSIS_H
#define R_I2C_CMSIS_H

#define R_RIIC0_ENABLE  (1)                                     /*!< RIIC0 enable */
#define R_RIIC1_ENABLE  (1)                                     /*!< RIIC1 enable */

/**************************************************/
/* Local Definition                               */
/**************************************************/
/** RIIC flags */
#define RIIC_FLAG_INITIALIZED       (1U << 0)                    /*!< initialized */
#define RIIC_FLAG_POWERED           (1U << 1)                    /*!< powered */

/** ARM_I2C_GetStatus.mode: 0=Slave, 1=Master. */
#define RIIC_MODE_MASTER (1)                                      /*!< I2C Master Mode */
#define RIIC_MODE_SLAVE  (0)                                      /*!< I2C Slave Mode */

/** ARM_I2C_GetStatus.direction: 0=Transmitter, 1=Reciver. */
#define RIIC_TRANSMITTER    (0)                                   /*!< I2C status : Transmitter */
#define RIIC_RECIVER        (1)                                   /*!< I2C status : Reciver */

#define RIIC_MAX_DIV                ((uint8_t)0x08)             /*!< Maximum dividing stage */
#define RIIC_STAD_SPPED_MAX         ((uint16_t)100)             /*!< Standard mode Maximum transfer rate */
#define RIIC_FAST_SPPED_MAX         ((uint16_t)400)             /*!< Fast mode Maximum transfer rate */
#define RIIC_ICBR_MAX               (32)                        /*!< IIC bus bit rate register maximum setting value */

#define RIIC_DEFAULT_ADDR           ((uint32_t)0x00000000)      /*!< Default own slave address */
#define RIIC_DEFAULT_BPS            (RIIC_STAD_SPPED_MAX)       /*!< Default bus speed */
#define RIIC_WRITE                  (0)                         /*!< Write mode */
#define RIIC_READ                   (1)                         /*!< Read mode */

#define RIIC_RECV_ENABLE            (1)                         /*!< Receive enable */
#define RIIC_RECV_DISABLE           (0)                         /*!< Receive disable */


#if RIIC_NOISE_FILTER == 0
    #define RIIC_ICMR3_DEF          (0x00)
    #define RIIC_ICFER_DEF          (0x5A)
#elif RIIC_NOISE_FILTER <= 4
    #define RIIC_ICMR3_DEF          (0x00 | (RIIC_NOISE_FILTER-1))
    #define RIIC_ICFER_DEF          (0x7A)
#else
    #error "ERROR - Unsupported noise filter chosen in r_i2c_cfg.h"
#endif



/******************************************************************************
 * Local Typedef definitions
 *****************************************************************************/
/**  Enum for IIC Driver State */
typedef enum i2c_driver_state { // @suppress("Source line ordering")
  RIIC_STATE_START,                                             /*!< start state */
  RIIC_STATE_ADDR,                                              /*!< address send state */
  RIIC_STATE_SEND,                                              /*!< data send state */
  RIIC_STATE_RECV,                                              /*!< receive state */
  RIIC_STATE_STOP                                               /*!< stop state */
} e_i2c_driver_state_t;

/**  Enum for IIC Nack State */
typedef enum i2c_nack_state {
  RIIC_NACK_NONE,                                               /*!< NACK not occurred */
  RIIC_NACK_ADDR,                                               /*!< Receive NACK at address send */
  RIIC_NACK_DATA                                                /*!< Receive NACK at data send */
} e_i2c_nack_state_t;

/** @brief  Structure to register buffer */
typedef struct
{
    uint8_t cks;                                /*!< Set value of CKS register */
    uint8_t icbrl;                              /*!< Set value of ICBRL register */
    uint8_t icbrh;                              /*!< Set value of ICBRH register */
} st_i2c_reg_buf_t;

/** @brief Structure to save IIC settings and statuses */
typedef struct
{          
    ARM_I2C_SignalEvent_t   cb_event;           /*!< Event callback */
    ARM_I2C_STATUS          status;             /*!< Status flags */
    uint8_t                 flags;              /*!< I2C Driver flags */
    uint32_t                own_sla;            /*!< Slave address */
    uint16_t                bps;                /*!< Transfer speed */
    e_i2c_driver_state_t    state;              /*!< Transmit state */
    e_i2c_nack_state_t      nack;               /*!< nack state */
    bool                    pending;            /*!< Transfer pending (no STOP) */
} st_i2c_info_t;           

/** @brief Structure for CMSIS-Driver IIC Data Transmission Status */
typedef struct
{          
    uint32_t                sla;                /*!< Target slave address */
    uint32_t                rx_num;             /*!< Total number of rx data */
    uint32_t                tx_num;             /*!< Total number of tx data */
    uint8_t                 *rx_buf;            /*!< Pointer to in data buffer */
    const uint8_t           *tx_buf;            /*!< Pointer to out data buffer */
    uint32_t                cnt;                /*!< Number of data send or received */
    uint8_t                 f_tx_end;           /*!< Send complate flag */
} st_i2c_transfer_info_t ;         

/** @brief Structure to save contexts for an IIC channel */
typedef struct
{            
    volatile IIC0_Type      *reg;               /*!< Base pointer to RIIC registers */
    r_pinset_t              pin_set;            /*!< I2C pin set function */
    r_pinclr_t              pin_clr;            /*!< I2C pin clear function */
    st_i2c_info_t           *info;              /*!< I2C infomation */
    st_i2c_transfer_info_t  *xfer_info;         /*!< I2C transfer infomation */
    e_system_mcu_lock_t     lock_id;            /*!< Module lock id */
    e_lpm_mstp_t            mstp_id;            /*!< MSTP id */
    IRQn_Type               txi_irq;            /*!< TXI IRQ Number */
    IRQn_Type               tei_irq;            /*!< TEI IRQ Number */
    IRQn_Type               rxi_irq;            /*!< RXI IRQ Number */
    IRQn_Type               eei_irq;            /*!< EEI IRQ Number */
    uint32_t                txi_iesr_val;       
    uint32_t                tei_iesr_val;       
    uint32_t                rxi_iesr_val;       
    uint32_t                eei_iesr_val;       
    uint32_t                txi_priority;       
    uint32_t                tei_priority;       
    uint32_t                rxi_priority;       
    uint32_t                eei_priority;       
    system_int_cb_t         txi_callback;
    system_int_cb_t         tei_callback;
    system_int_cb_t         rxi_callback;
    system_int_cb_t         eei_callback;
} const st_i2c_resources_t;            

/*******************************************************************************************************************//**
 * @} (end defgroup grp_cmsis_drv_impl_iic)
 **********************************************************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/

/****************************************************************************************************************//**
 * @brief       Inline function to interrupt all clear.
 * @param[in,out]    p_i2c     Pointer to I2C resource data
 *******************************************************************************************************************/
/* Function Name : iic_interrupt_clear */
__STATIC_FORCEINLINE void iic_interrupt_clear(st_i2c_resources_t *p_i2c)
{
    
    /* Interrupt disable */
    R_NVIC_DisableIRQ(p_i2c->txi_irq);
    R_NVIC_DisableIRQ(p_i2c->rxi_irq);
    R_NVIC_DisableIRQ(p_i2c->tei_irq);
    R_NVIC_DisableIRQ(p_i2c->eei_irq);
    
    /* Pending interrupt request clear */
    R_SYS_IrqStatusClear(p_i2c->txi_irq);
    R_NVIC_ClearPendingIRQ(p_i2c->txi_irq);
    R_SYS_IrqStatusClear(p_i2c->rxi_irq);
    R_NVIC_ClearPendingIRQ(p_i2c->rxi_irq);
    R_SYS_IrqStatusClear(p_i2c->tei_irq);
    R_NVIC_ClearPendingIRQ(p_i2c->tei_irq);
    R_SYS_IrqStatusClear(p_i2c->eei_irq);
    R_NVIC_ClearPendingIRQ(p_i2c->eei_irq);
  
}/* End of function iic_interrupt_clear() */



#ifdef  __cplusplus
}
#endif

#endif /** R_I2C_CMSIS_H */

/******************************************************************************************************************//**
 * @} (end defgroup grp_cmsis_drv_impl_i2c)
 *********************************************************************************************************************/
/* End of file (r_i2c_cmsis.h) */

