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
 * File Name    : r_i2c_cfg.h
 * Version      : 0.40
 * Description  : CMSIS-Driver for I2C.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
 **********************************************************************************************************************/

#ifndef R_I2C_CFG_H
#define R_I2C_CFG_H
/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_iic
 * @{
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
#define RIIC_NOISE_FILTER   (2)                     ///< Digital noise filter
                                                    ///< Specify the number of noise filter stages for 0 to 4 IIC cycles
                                                    ///< (When 0 is set, the digital noise filter is invalid)

#define RIIC_BUS_SPEED_CAL_ENABLE  (1)              ///< Bus speed caluclation enable
                                                    ///< When "1" is set, values of ICBRL, ICBRH, ICMR3.CKS are calculated 
                                                    ///< from the specified bus speed

/* Constant for bus speed automatic calculation */                          
#define RIIC_STAD_SCL_UP_TIME      (1000E-9)        ///< SCL rise time in standard mode
#define RIIC_STAD_SCL_DOWN_TIME    (300E-9)         ///< SCL falling time in standard mode
#define RIIC_FAST_SCL_UP_TIME      (300E-9)         ///< SCL rise time in fast mode
#define RIIC_FAST_SCL_DOWN_TIME    (300E-9)         ///< SCL falling time in fast mode
                            
/* Bus speed control register setting value when bus speed calculation is not used */
#define RIIC_STAD_ICBRL            (15)             ///< ICBRL set value in standard mode
#define RIIC_STAD_ICBRH            (12)             ///< ICBRH set value in standard mode
#define RIIC_STAD_CKS              ( 3)             ///< CKS set value in standard mode  
#define RIIC_FAST_ICBRL            (17)             ///< ICBRL set value in fast mode    
#define RIIC_FAST_ICBRH            ( 6)             ///< ICBRH set value in fast mode    
#define RIIC_FAST_CKS              ( 1)             ///< CKS set value in fast mode  

#define RIIC0_TXI_PRIORITY  (3)                     ///< TXI0 priority value(set to 0 to 3)
#define RIIC0_TEI_PRIORITY  (3)                     ///< TEI0 priority value(set to 0 to 3)
#define RIIC0_RXI_PRIORITY  (3)                     ///< RXI0 priority value(set to 0 to 3)
#define RIIC0_EEI_PRIORITY  (3)                     ///< EEI0 priority value(set to 0 to 3)

#define RIIC1_TXI_PRIORITY  (3)                     ///< TXI1 priority value(set to 0 to 3)
#define RIIC1_TEI_PRIORITY  (3)                     ///< TEI1 priority value(set to 0 to 3)
#define RIIC1_RXI_PRIORITY  (3)                     ///< RXI1 priority value(set to 0 to 3)
#define RIIC1_EEI_PRIORITY  (3)                     ///< EEI1 priority value(set to 0 to 3)

/******************************************************************************************************************//**
 * @name R_I2C_API_LOCATION_CONFIG
 *       Definition of R_I2C API location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define RIIC_CFG_SECTION_ARM_I2C_GET_VERSION     (SYSTEM_SECTION_CODE)        ///< ARM_I2C_GetVersion() section
#define RIIC_CFG_SECTION_ARM_I2C_GET_CAPABILITES (SYSTEM_SECTION_CODE)        ///< ARM_I2C_GetCapabilities() section
#define RIIC_CFG_SECTION_ARM_I2C_INITIALIZE      (SYSTEM_SECTION_CODE)        ///< ARM_I2C_Initialize() section
#define RIIC_CFG_SECTION_ARM_I2C_UNINITIALIZE    (SYSTEM_SECTION_CODE)        ///< ARM_I2C_Uninitialize() section
#define RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL   (SYSTEM_SECTION_CODE)        ///< ARM_I2C_PowerControl() section
#define RIIC_CFG_SECTION_ARM_I2C_MASTER_TRANSMIT (SYSTEM_SECTION_CODE)        ///< ARM_I2C_MasterTransmit() section
#define RIIC_CFG_SECTION_ARM_I2C_MASTER_RECEIVE  (SYSTEM_SECTION_CODE)        ///< ARM_I2C_MasterReceive() section
#define RIIC_CFG_SECTION_ARM_I2C_SLAVE_TRANSMIT  (SYSTEM_SECTION_CODE)        ///< ARM_I2C_SlaveTransmit() section
#define RIIC_CFG_SECTION_ARM_I2C_SLAVE_RECEIVE   (SYSTEM_SECTION_CODE)        ///< ARM_I2C_SlaveReceive() section
#define RIIC_CFG_SECTION_ARM_I2C_GET_DATA_COUNT  (SYSTEM_SECTION_CODE)        ///< ARM_I2C_GetDataCount() section
#define RIIC_CFG_SECTION_ARM_I2C_CONTROL         (SYSTEM_SECTION_CODE)        ///< ARM_I2C_Control() section
#define RIIC_CFG_SECTION_ARM_I2C_GET_STATUS      (SYSTEM_SECTION_CODE)        ///< ARM_I2C_GetStatus section
#define RIIC_CFG_SECTION_IIC_TXI_INTERRUPT       (SYSTEM_SECTION_RAM_FUNC)    ///< iic_txi_interrupt() section
#define RIIC_CFG_SECTION_IIC_TEI_INTERRUPT       (SYSTEM_SECTION_RAM_FUNC)    ///< iic_tei_interrupt() section
#define RIIC_CFG_SECTION_IIC_RXI_INTERRUPT       (SYSTEM_SECTION_RAM_FUNC)    ///< iic_rxi_interrupt() section
#define RIIC_CFG_SECTION_IIC_EEI_INTERRUPT       (SYSTEM_SECTION_RAM_FUNC)    ///< iic_eei_interrupt() section
/* @} */

#ifdef  __cplusplus
}
#endif
/*******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_iic)
 **********************************************************************************************************************/

#endif /** R_I2C_CFG_H */

/* End of file (r_i2c_cfg.h) */
