/***********************************************************************************************************************
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
* Copyright (C) 2018-2020 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_i2c_cmsis_api.c
* Version      : 0.80
* Description  : CMSIS-Driver for I2C.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 19.02.2020 0.80     Change of ACKWP bit protection setting method
**********************************************************************************************************************/

/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_iic
 * @{
 *********************************************************************************************************************/
/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <r_i2c_cmsis_api.h>

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define ARM_I2C_PRV_DRV_VERSION    (ARM_DRIVER_VERSION_MAJOR_MINOR(0, 80)) /*!< driver version */
#define IIC_PRV_OUT_9CLOCK    (9)
/******************************************************************************
 Local Typedef definitions
 *****************************************************************************/

/******************************************************************************
 Exported global variables (to be accessed by other files)
 *****************************************************************************/
 
/******************************************************************************
 Private global variables and functions
 *****************************************************************************/
#if (RIIC_CFG_SECTION_ARM_I2C_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_VERSION             __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_VERSION
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_GET_CAPABILITES == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_CAPABILITIES        __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_CAPABILITIES
#endif

#if ((RIIC_CFG_SECTION_ARM_I2C_UNINITIALIZE    == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_CONTROL         == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_SCI_CALCULATION_DATA       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_SCI_CALCULATION_DATA
#endif

#if ((RIIC_CFG_SECTION_ARM_I2C_INITIALIZE      == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_UNINITIALIZE    == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_MASTER_TRANSMIT == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_MASTER_RECEIVE  == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_SLAVE_TRANSMIT  == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_SLAVE_RECEIVE   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_GET_DATA_COUNT  == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_CONTROL         == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_GET_STATUS       == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_IIC_TXI_INTERRUPT       == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_IIC_TEI_INTERRUPT       == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_IIC_RXI_INTERRUPT       == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_IIC_EEI_INTERRUPT       == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_RIIC_RESOURCES        __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_RIIC_RESOURCES
#endif

#if (RIIC_CFG_SECTION_IIC_TXI_INTERRUPT == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_IIC_TXI_INTERRUPT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_IIC_TXI_INTERRUPT
#endif

#if (RIIC_CFG_SECTION_IIC_TEI_INTERRUPT == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_IIC_TEI_INTERRUPT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_IIC_TEI_INTERRUPT
#endif

#if (RIIC_CFG_SECTION_IIC_RXI_INTERRUPT == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_IIC_RXI_INTERRUPT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_IIC_RXI_INTERRUPT
#endif

#if (RIIC_CFG_SECTION_IIC_EEI_INTERRUPT == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_IIC_EEI_INTERRUPT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_IIC_EEI_INTERRUPT
#endif

#if ((RIIC_CFG_SECTION_ARM_I2C_UNINITIALIZE    == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_CONTROL         == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_PRV_RIIC_BPS_CALC      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_RIIC_BPS_CALC
#endif

#if ((RIIC_CFG_SECTION_ARM_I2C_UNINITIALIZE    == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_CONTROL         == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_PRV_RIIC_SETTING      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_RIIC_SETTING
#endif

#if ((RIIC_CFG_SECTION_ARM_I2C_SLAVE_TRANSMIT  == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_SLAVE_RECEIVE   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_CONTROL         == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_PRV_RIIC_RESET      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_RIIC_RESET
#endif

#if ((RIIC_CFG_SECTION_ARM_I2C_GET_VERSION     == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_GET_CAPABILITES == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_INITIALIZE      == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_UNINITIALIZE    == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_MASTER_TRANSMIT == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_MASTER_RECEIVE  == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_SLAVE_TRANSMIT  == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_SLAVE_RECEIVE   == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_GET_DATA_COUNT  == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_CONTROL         == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_GET_STATUS       == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_DRIVER_I2C        __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_I2C
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_GET_VERSION       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_GET_VERSION
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_GET_CAPABILITES == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_GET_CAPABILITES       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_GET_CAPABILITES
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_INITIALIZE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_INITIALIZE
#endif

#if ((RIIC_CFG_SECTION_ARM_I2C_UNINITIALIZE    == SYSTEM_SECTION_RAM_FUNC) || \
     (RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL   == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_PRV_ARM_I2C_UNINITIALIZE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_UNINITIALIZE
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_POWER_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_POWER_CONTROL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_POWER_CONTROL
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_MASTER_TRANSMIT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_MASTER_TRANSMIT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_MASTER_TRANSMIT
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_MASTER_RECEIVE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_MASTER_RECEIVE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_MASTER_RECEIVE
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_SLAVE_TRANSMIT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_SLAVE_TRANSMIT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_SLAVE_TRANSMIT
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_SLAVE_RECEIVE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_SLAVE_RECEIVE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_SLAVE_RECEIVE
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_GET_DATA_COUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_GET_DATA_COUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_GET_DATA_COUNT
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_CONTROL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_CONTROL
#endif

#if (RIIC_CFG_SECTION_ARM_I2C_GET_STATUS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ARM_I2C_GET_STATUS      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_ARM_I2C_GET_STATUS
#endif

/** Driver Version */
static const ARM_DRIVER_VERSION gs_driver_version DATA_LOCATION_PRV_DRIVER_VERSION =
{
    ARM_I2C_API_VERSION,               /*!< CMSIS-Driver Version */
    ARM_I2C_PRV_DRV_VERSION            /*!< Driver-SourceCode Version */
};

/* Driver Capabilities */
static const ARM_I2C_CAPABILITIES gs_driver_capabilities DATA_LOCATION_PRV_DRIVER_CAPABILITIES =
{
    0  /*!< supports 10-bit addressing */
};

#if (0 != RIIC_BUS_SPEED_CAL_ENABLE)
/* Definition used when calculating bit rate */
static const double gs_scl100k_up_time DATA_LOCATION_PRV_SCI_CALCULATION_DATA = RIIC_STAD_SCL_UP_TIME;
static const double gs_scl100k_down_time DATA_LOCATION_PRV_SCI_CALCULATION_DATA = RIIC_STAD_SCL_DOWN_TIME;
static const double gs_scl400k_up_time DATA_LOCATION_PRV_SCI_CALCULATION_DATA = RIIC_FAST_SCL_UP_TIME;
static const double gs_scl400k_down_time DATA_LOCATION_PRV_SCI_CALCULATION_DATA = RIIC_FAST_SCL_DOWN_TIME;
static const double gs_scl1m_up_time DATA_LOCATION_PRV_SCI_CALCULATION_DATA = 120E-9;
static const double gs_scl1m_down_time DATA_LOCATION_PRV_SCI_CALCULATION_DATA = 120E-9;
static const uint8_t gs_d_cks[RIIC_MAX_DIV] DATA_LOCATION_PRV_SCI_CALCULATION_DATA =
    { 1, 2, 4, 8, 16, 32, 64, 128 }; /* divider array of RIIC clock  */
#endif

static ARM_DRIVER_VERSION ARM_I2C_GetVersion(void) FUNC_LOCATION_PRV_ARM_I2C_GET_VERSION;
static ARM_I2C_CAPABILITIES ARM_I2C_GetCapabilities(void) FUNC_LOCATION_PRV_ARM_I2C_GET_CAPABILITES;
static int32_t ARM_I2C_Initialize(ARM_I2C_SignalEvent_t cb_event, st_i2c_resources_t *p_i2c) 
                               FUNC_LOCATION_PRV_ARM_I2C_INITIALIZE;
static int32_t ARM_I2C_Uninitialize(st_i2c_resources_t *p_i2c) FUNC_LOCATION_PRV_ARM_I2C_UNINITIALIZE;
static int32_t ARM_I2C_PowerControl(ARM_POWER_STATE state, st_i2c_resources_t *p_i2c) 
                               FUNC_LOCATION_PRV_ARM_I2C_POWER_CONTROL;
static int32_t ARM_I2C_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, 
                               bool xfer_pending, st_i2c_resources_t *p_i2c) 
                               FUNC_LOCATION_PRV_ARM_I2C_MASTER_TRANSMIT;
static int32_t ARM_I2C_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, 
                               bool xfer_pending, st_i2c_resources_t *p_i2c) FUNC_LOCATION_PRV_ARM_I2C_MASTER_RECEIVE;
static int32_t ARM_I2C_SlaveTransmit(const uint8_t *data, uint32_t num, st_i2c_resources_t *p_i2c) 
                              FUNC_LOCATION_PRV_ARM_I2C_SLAVE_TRANSMIT;
static int32_t ARM_I2C_SlaveReceive(uint8_t *data, uint32_t num, st_i2c_resources_t *p_i2c) 
                              FUNC_LOCATION_PRV_ARM_I2C_SLAVE_RECEIVE;
static int32_t ARM_I2C_GetDataCount(st_i2c_resources_t *p_i2c) FUNC_LOCATION_PRV_ARM_I2C_GET_DATA_COUNT;
static int32_t ARM_I2C_Control(uint32_t control, uint32_t arg, st_i2c_resources_t *p_i2c) 
                              FUNC_LOCATION_PRV_ARM_I2C_CONTROL;
static ARM_I2C_STATUS ARM_I2C_GetStatus(st_i2c_resources_t *p_i2c) FUNC_LOCATION_PRV_ARM_I2C_GET_STATUS;

static void iic_txi_interrupt(st_i2c_resources_t *p_i2c) STATIC_FUNC_LOCATION_PRV_IIC_TXI_INTERRUPT;
static void iic_tei_interrupt(st_i2c_resources_t *p_i2c) STATIC_FUNC_LOCATION_PRV_IIC_TEI_INTERRUPT;
static void iic_rxi_interrupt(st_i2c_resources_t *p_i2c) STATIC_FUNC_LOCATION_PRV_IIC_RXI_INTERRUPT;
static void iic_eei_interrupt(st_i2c_resources_t *p_i2c) STATIC_FUNC_LOCATION_PRV_IIC_EEI_INTERRUPT;
static int32_t riic_bps_calc (uint16_t kbps, st_i2c_reg_buf_t *reg_val) STATIC_FUNC_LOCATION_PRV_RIIC_BPS_CALC;
static int32_t riic_setting(uint16_t bps, st_i2c_resources_t *p_i2c) STATIC_FUNC_LOCATION_PRV_RIIC_SETTING;
static void riic_reset(st_i2c_resources_t *p_i2c, bool en_recv) STATIC_FUNC_LOCATION_PRV_RIIC_RESET;



/** RIIC0 */
#if (0 != R_RIIC0_ENABLE)
static int32_t RIIC0_Initialize(ARM_I2C_SignalEvent_t cb_event) FUNC_LOCATION_PRV_ARM_I2C_INITIALIZE;
static int32_t RIIC0_Uninitialize(void) FUNC_LOCATION_PRV_ARM_I2C_UNINITIALIZE;
static int32_t RIIC0_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_PRV_ARM_I2C_POWER_CONTROL;
static int32_t RIIC0_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
                                    FUNC_LOCATION_PRV_ARM_I2C_MASTER_TRANSMIT;
static int32_t RIIC0_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) 
                                   FUNC_LOCATION_PRV_ARM_I2C_MASTER_RECEIVE;
static int32_t RIIC0_SlaveTransmit(const uint8_t *data, uint32_t num) FUNC_LOCATION_PRV_ARM_I2C_SLAVE_TRANSMIT;
static int32_t RIIC0_SlaveReceive(uint8_t *data, uint32_t num) FUNC_LOCATION_PRV_ARM_I2C_SLAVE_RECEIVE;
static int32_t RIIC0_GetDataCount(void) FUNC_LOCATION_PRV_ARM_I2C_GET_DATA_COUNT;
static int32_t RIIC0_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_PRV_ARM_I2C_CONTROL;
static ARM_I2C_STATUS RIIC0_GetStatus(void) FUNC_LOCATION_PRV_ARM_I2C_GET_STATUS;
static void iic0_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_TXI_INTERRUPT;
static void iic0_tei_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_TEI_INTERRUPT;
static void iic0_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_RXI_INTERRUPT;
static void iic0_eei_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_EEI_INTERRUPT;

static st_i2c_info_t gs_riic0_info = {0};
static st_i2c_transfer_info_t gs_riic0_xfer_info = {0};

/** IIC0 Resources */
static st_i2c_resources_t gs_riic0_resources DATA_LOCATION_PRV_RIIC_RESOURCES =
{
  (IIC0_Type*)IIC0, // @suppress("Cast comment")
  R_RIIC_Pinset_CH0,
  R_RIIC_Pinclr_CH0,
  &gs_riic0_info,
  &gs_riic0_xfer_info,
  SYSTEM_LOCK_RIIC0,
  LPM_MSTP_RIIC0,
  SYSTEM_CFG_EVENT_NUMBER_IIC0_TXI, // @suppress("Cast comment")
  SYSTEM_CFG_EVENT_NUMBER_IIC0_TEI, // @suppress("Cast comment")
  SYSTEM_CFG_EVENT_NUMBER_IIC0_RXI, // @suppress("Cast comment")
  SYSTEM_CFG_EVENT_NUMBER_IIC0_EEI, // @suppress("Cast comment")
  0x00000009,
  0x00000008,
  0x0000000A,
  0x00000008,
  RIIC0_TXI_PRIORITY,
  RIIC0_TEI_PRIORITY,
  RIIC0_RXI_PRIORITY,
  RIIC0_EEI_PRIORITY,
  iic0_txi_interrupt,
  iic0_tei_interrupt,
  iic0_rxi_interrupt,
  iic0_eei_interrupt
};

/****************************************************************************************************************//**
 * @brief IIC initialize method for user applications to initialize the IIC channel0. User applications should call
 * this method through the Driver Instance Driver_IIC0.Initialize().
 *
 * @param[in] cb_event   Callback event for IIC.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_Initialize */
static int32_t RIIC0_Initialize(ARM_I2C_SignalEvent_t cb_event) // @suppress("Source line ordering") @suppress("Non-API function naming")
{
    return (ARM_I2C_Initialize(cb_event, &gs_riic0_resources));
}/* End of function RIIC0_Initialize() */

/****************************************************************************************************************//**
 * @brief IIC uninitialize method for user applications to initialize the SCI channel0. User applications should call
 * this method through the Driver Instance Driver_IIC0.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_Uninitialize */
static int32_t RIIC0_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_I2C_Uninitialize(&gs_riic0_resources));
}/* End of function RIIC0_Uninitialize() */

/****************************************************************************************************************//**
 * @brief IIC power control method for user applications to control the power of SCI channel0. User applications
 * should call this method through the Driver Instance Driver_IIC0.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_PowerControl */
static int32_t RIIC0_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_I2C_PowerControl(state, &gs_riic0_resources));
}/* End of function RIIC0_PowerControl() */

/****************************************************************************************************************//**
 * @brief IIC Master data transmission method for user applications to send data over IIC channel0. 
 *        User applications should
 * call this method through the Driver Instance Driver_IIC0.Send().
 * @param[in]        addr                 Slave address (7-bit or 10-bit) 
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_MasterTransmit */
static int32_t RIIC0_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) // @suppress("Non-API function naming")
{
    return (ARM_I2C_MasterTransmit(addr, data, num, xfer_pending, &gs_riic0_resources));

}/* End of function RIIC0_MasterTransmit() */

/****************************************************************************************************************//**
 * @brief IIC Master data reception method for user applications to receive data over IIC channel0. 
 *        User applications should
 * call this method through the Driver Instance Driver_IIC0.Receive().
 * @param[in]        addr                 Slave address (7-bit or 10-bit) 
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_I2C_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_MasterReceive */
static int32_t RIIC0_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) // @suppress("Non-API function naming")
{
    return (ARM_I2C_MasterReceive(addr, data, num, xfer_pending, &gs_riic0_resources));
}/* End of function RIIC0_MasterReceive() */

/****************************************************************************************************************//**
 * @brief IIC Slave data transmission method for user applications to send data over IIC channel0. User applications
 *         should
 * call this method through the Driver Instance Driver_IIC0.Send().
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Master
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_SlaveTransmit */
static int32_t RIIC0_SlaveTransmit(const uint8_t *data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_I2C_SlaveTransmit(data, num, &gs_riic0_resources));
}/* End of function RIIC0_SlaveTransmit() */

/****************************************************************************************************************//**
 * @brief IIC Slave data reception method for user applications to receive data over IIC channel0. 
 *        User applications should
 * call this method through the Driver Instance Driver_IIC0.Receive().
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Master
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_SlaveReceive */
static int32_t RIIC0_SlaveReceive(uint8_t *data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_I2C_SlaveReceive(data, num, &gs_riic0_resources));
}/* End of function RIIC0_SlaveReceive() */

/****************************************************************************************************************//**
 * @brief IIC data transmission count getter method for user applications to get the transmission counter value for
 * the TX IIC channel0. User applications should call this method through the Driver Instance
 * Driver_RIICS0.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_GetDataCount */
static int32_t RIIC0_GetDataCount(void) // @suppress("Non-API function naming")
{
    return (ARM_I2C_GetDataCount(&gs_riic0_resources));
}/* End of function RIIC0_GetDataCount() */

/****************************************************************************************************************//**
 * @brief IIC driver control method for user applications to set IIC configuration for SCI channel0. User 
 * applications should call this method through the Driver Instance Driver_IIC0.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_Control */
static int32_t RIIC0_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_I2C_Control(control, arg, &gs_riic0_resources));
}/* End of function RIIC0_Control() */

/****************************************************************************************************************//**
 * @brief IIC status getter method for user applications to get the driver status for SCI channel0. User applications
 * should call this method through the Driver Instance Driver_IIC0.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC0_GetStatus */
static ARM_I2C_STATUS RIIC0_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_I2C_GetStatus(&gs_riic0_resources));
}/* End of function RIIC0_GetStatus() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC0 Master/Slave transmission.
 *******************************************************************************************************************/
/* Function Name : iic0_txi_interrupt */
static void iic0_txi_interrupt(void) // @suppress("ISR declaration") @suppress("Function definition ordering")
{
    iic_txi_interrupt(&gs_riic0_resources);
}/* End of function iic0_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC0 Master/Slave transmission completion.
 *******************************************************************************************************************/
/* Function Name : iic0_tei_interrupt */
static void iic0_tei_interrupt(void) // @suppress("ISR declaration")
{
    iic_tei_interrupt(&gs_riic0_resources);
}/* End of function iic0_tei_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC Master/Slave reception data full.
 *******************************************************************************************************************/
/* Function Name : iic0_rxi_interrupt */
static void iic0_rxi_interrupt(void) // @suppress("ISR declaration")
{
    iic_rxi_interrupt(&gs_riic0_resources);
}/* End of function iic0_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC0 Master/Slave transmission event/err.
 *******************************************************************************************************************/
/* Function Name : iic0_eei_interrupt */
static void iic0_eei_interrupt(void) // @suppress("ISR declaration")
{
    iic_eei_interrupt(&gs_riic0_resources);
}/* End of function iic0_eei_interrupt() */

#endif /* (0 != R_RIIC0_ENABLE) */

/** RIIC1 */
#if (0 != R_RIIC1_ENABLE)

static int32_t RIIC1_Initialize(ARM_I2C_SignalEvent_t cb_event) FUNC_LOCATION_PRV_ARM_I2C_INITIALIZE;
static int32_t RIIC1_Uninitialize(void) FUNC_LOCATION_PRV_ARM_I2C_UNINITIALIZE;
static int32_t RIIC1_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_PRV_ARM_I2C_POWER_CONTROL;
static int32_t RIIC1_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) 
               FUNC_LOCATION_PRV_ARM_I2C_MASTER_TRANSMIT;
static int32_t RIIC1_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) 
               FUNC_LOCATION_PRV_ARM_I2C_MASTER_RECEIVE;
static int32_t RIIC1_SlaveTransmit(const uint8_t *data, uint32_t num) FUNC_LOCATION_PRV_ARM_I2C_SLAVE_TRANSMIT;
static int32_t RIIC1_SlaveReceive(uint8_t *data, uint32_t num) FUNC_LOCATION_PRV_ARM_I2C_SLAVE_RECEIVE;
static int32_t RIIC1_GetDataCount(void) FUNC_LOCATION_PRV_ARM_I2C_GET_DATA_COUNT;
static int32_t RIIC1_Control(uint32_t control, uint32_t arg) FUNC_LOCATION_PRV_ARM_I2C_CONTROL;
static ARM_I2C_STATUS RIIC1_GetStatus(void) FUNC_LOCATION_PRV_ARM_I2C_GET_STATUS;
static void iic1_txi_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_TXI_INTERRUPT;
static void iic1_tei_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_TEI_INTERRUPT;
static void iic1_rxi_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_RXI_INTERRUPT;
static void iic1_eei_interrupt(void) STATIC_FUNC_LOCATION_PRV_IIC_EEI_INTERRUPT;

static st_i2c_info_t gs_riic1_info = {0};
static st_i2c_transfer_info_t gs_riic1_xfer_info = {0};

/** IIC1 Resources */
static st_i2c_resources_t gs_riic1_resources DATA_LOCATION_PRV_RIIC_RESOURCES =
{
  (IIC0_Type*)IIC1, // @suppress("Cast comment")
  R_RIIC_Pinset_CH1,
  R_RIIC_Pinclr_CH1,
  &gs_riic1_info,
  &gs_riic1_xfer_info,
  SYSTEM_LOCK_RIIC1,
  LPM_MSTP_RIIC1,
  SYSTEM_CFG_EVENT_NUMBER_IIC1_TXI, // @suppress("Cast comment")
  SYSTEM_CFG_EVENT_NUMBER_IIC1_TEI, // @suppress("Cast comment")
  SYSTEM_CFG_EVENT_NUMBER_IIC1_RXI, // @suppress("Cast comment")
  SYSTEM_CFG_EVENT_NUMBER_IIC1_EEI, // @suppress("Cast comment")
  0x00000015,
  0x00000014,
  0x00000016,
  0x00000013,
  RIIC1_TXI_PRIORITY,
  RIIC1_TEI_PRIORITY,
  RIIC1_RXI_PRIORITY,
  RIIC1_EEI_PRIORITY,
  iic1_txi_interrupt,
  iic1_tei_interrupt,
  iic1_rxi_interrupt,
  iic1_eei_interrupt
};

/****************************************************************************************************************//**
 * @brief IIC initialize method for user applications to initialize the IIC channel0. User applications should call
 * this method through the Driver Instance Driver_IIC1.Initialize().
 *
 * @param[in] cb_event   Callback event for IIC.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_Initialize */
static int32_t RIIC1_Initialize(ARM_I2C_SignalEvent_t cb_event) // @suppress("Non-API function naming")
{
    return (ARM_I2C_Initialize(cb_event, &gs_riic1_resources));
}/* End of function RIIC1_Initialize() */

/****************************************************************************************************************//**
 * @brief IIC uninitialize method for user applications to initialize the SCI channel0. User applications should call
 * this method through the Driver Instance Driver_IIC1.Uninitialize().
 *
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_Uninitialize */
static int32_t RIIC1_Uninitialize(void) // @suppress("Non-API function naming")
{
    return (ARM_I2C_Uninitialize(&gs_riic1_resources));
}/* End of function RIIC1_Uninitialize() */

/****************************************************************************************************************//**
 * @brief IIC power control method for user applications to control the power of SCI channel0. User applications
 * should call this method through the Driver Instance Driver_IIC1.PowerControl().
 * @param[in]    ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock), ARM_POWER_LOW(No operation)
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_PowerControl */
static int32_t RIIC1_PowerControl(ARM_POWER_STATE state) // @suppress("Non-API function naming")
{
    return (ARM_I2C_PowerControl(state, &gs_riic1_resources));
}/* End of function RIIC1_PowerControl() */

/****************************************************************************************************************//**
 * @brief IIC Master data transmission method for user applications to send data over IIC channel0. 
 *        User applications should
 * call this method through the Driver Instance Driver_IIC1.Send().
 * @param[in]        addr                 Slave address (7-bit or 10-bit) 
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_MasterTransmit */
static int32_t RIIC1_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) // @suppress("Non-API function naming")
{
    return (ARM_I2C_MasterTransmit(addr, data, num, xfer_pending, &gs_riic1_resources));

}/* End of function RIIC1_MasterTransmit() */

/****************************************************************************************************************//**
 * @brief IIC Master data reception method for user applications to receive data over IIC channel0. 
 *        User applications should
 * call this method through the Driver Instance Driver_IIC1.Receive().
 * @param[in]        addr                 Slave address (7-bit or 10-bit) 
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_I2C_USART_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_MasterReceive */
static int32_t RIIC1_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) // @suppress("Non-API function naming")
{
    return (ARM_I2C_MasterReceive(addr, data, num, xfer_pending, &gs_riic1_resources));
}/* End of function RIIC1_MasterReceive() */

/****************************************************************************************************************//**
 * @brief IIC Slave data transmission method for user applications to send data over IIC channel0. User applications
 *         should
 * call this method through the Driver Instance Driver_IIC1.Send().
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Master
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_SlaveTransmit */
static int32_t RIIC1_SlaveTransmit(const uint8_t *data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_I2C_SlaveTransmit(data, num, &gs_riic1_resources));
}/* End of function RIIC1_SlaveTransmit() */

/****************************************************************************************************************//**
 * @brief IIC Slave data reception method for user applications to receive data over IIC channel0. 
 *        User applications should
 * call this method through the Driver Instance Driver_IIC1.Receive().
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Master
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @retval See @ref Common_Error_Codes or @ref CMSIS_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_SlaveReceive */
static int32_t RIIC1_SlaveReceive(uint8_t *data, uint32_t num) // @suppress("Non-API function naming")
{
    return (ARM_I2C_SlaveReceive(data, num, &gs_riic1_resources));
}/* End of function RIIC1_SlaveReceive() */

/****************************************************************************************************************//**
 * @brief IIC data transmission count getter method for user applications to get the transmission counter value for
 * the TX IIC channel0. User applications should call this method through the Driver Instance
 * Driver_RIICS0.GetTxCount().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_GetDataCount */
static int32_t RIIC1_GetDataCount(void) // @suppress("Non-API function naming")
{
    return (ARM_I2C_GetDataCount(&gs_riic1_resources));
}/* End of function RIIC1_GetDataCount() */

/****************************************************************************************************************//**
 * @brief IIC driver control method for user applications to set IIC configuration for SCI channel0. User 
 * applications should call this method through the Driver Instance Driver_IIC1.Control().
 * @param[in]  control  Contol code.
 * @param[in]  arg      Argument of Contol code.
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_Control */
static int32_t RIIC1_Control(uint32_t control, uint32_t arg) // @suppress("Non-API function naming")
{
    return (ARM_I2C_Control(control, arg, &gs_riic1_resources));
}/* End of function RIIC1_Control() */

/****************************************************************************************************************//**
 * @brief IIC status getter method for user applications to get the driver status for SCI channel0. User applications
 * should call this method through the Driver Instance Driver_IIC1.GetStatus().
 * @retval See @ref Common_Error_Codes or @ref CMSIS_Driver_I2C_Specific_Error_Code for other possible return codes
 *         or causes.
 *******************************************************************************************************************/
/* Function Name : RIIC1_GetStatus */
static ARM_I2C_STATUS RIIC1_GetStatus(void) // @suppress("Non-API function naming")
{
    return (ARM_I2C_GetStatus(&gs_riic1_resources));
}/* End of function RIIC1_GetStatus() */


/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC1 Master/Slave transmission.
 *******************************************************************************************************************/
/* Function Name : iic1_txi_interrupt */
static void iic1_txi_interrupt(void) // @suppress("ISR declaration")
{
    iic_txi_interrupt(&gs_riic1_resources);
}/* End of function iic1_txi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC1 Master/Slave transmission completion.
 *******************************************************************************************************************/
/* Function Name : iic1_tei_interrupt */
static void iic1_tei_interrupt(void) // @suppress("ISR declaration")
{
    iic_tei_interrupt(&gs_riic1_resources);
}/* End of function iic1_tei_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC Master/Slave reception data full.
 *******************************************************************************************************************/
/* Function Name : iic1_rxi_interrupt */
static void iic1_rxi_interrupt(void) // @suppress("ISR declaration")
{
    iic_rxi_interrupt(&gs_riic1_resources);
}/* End of function iic1_rxi_interrupt() */

/****************************************************************************************************************//**
 * @brief Interrupt handler for IIC1 Master/Slave transmission event/err.
 *******************************************************************************************************************/
/* Function Name : iic1_eei_interrupt */
static void iic1_eei_interrupt(void) // @suppress("ISR declaration")
{
    iic_eei_interrupt(&gs_riic1_resources);
}/* End of function iic1_eei_interrupt() */

#endif /* (0 != R_RIIC1_ENABLE) */



/****************************************************************************************************************//**
 * @brief The implementation of CMSIS-Driver I2C Get Version API for the I2C peripheral on RE Family.
 * @retval Driver version number
 *******************************************************************************************************************/
/* Function Name : ARM_I2C_GetVersion */
static ARM_DRIVER_VERSION ARM_I2C_GetVersion(void) // @suppress("Non-API function naming")
{
    return (gs_driver_version);
}/* End of function ARM_I2C_GetVersion() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver I2C Get Capabilities API for the I2C peripheral on RE Family.
 * @retval      gs_driver_capabilities
 *              The data fields of the struct @ref ARM_I2C_CAPABILITIES encodes the driver capabilities.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_GetCapabilities */
static ARM_I2C_CAPABILITIES ARM_I2C_GetCapabilities(void) // @suppress("Non-API function naming")
{
    return (gs_driver_capabilities);
}/* End of function ARM_I2C_GetCapabilities() */

/*****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver I2C Initialization API for the I2C peripheral on RE Family.
 * @param[in]        cb_event  Pointer to \ref ARM_I2C_SignalEvent
 * @param[in,out]    i2c     Pointer to I2C resource data
 * @retval      ARM_DRIVER_OK  Initialization was successful.
 * @retval      ARM_DRIVER_ERROR   Specified I2C mode is not supported.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_Initialize */
static int32_t ARM_I2C_Initialize(ARM_I2C_SignalEvent_t cb_event, st_i2c_resources_t *p_i2c) // @suppress("Non-API function naming")
{
    if (0x00 != (p_i2c->info->flags & RIIC_FLAG_INITIALIZED))
    {
        /* Target i2c is already initialized */
        return (ARM_DRIVER_OK);
    }

    /* Register txi interrupt handler */
    if ((-1) == R_SYS_IrqEventLinkSet(p_i2c->txi_irq, p_i2c->txi_iesr_val, p_i2c->txi_callback))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* TXI priority setting */
    R_NVIC_SetPriority(p_i2c->txi_irq, p_i2c->txi_priority);
    if (R_NVIC_GetPriority(p_i2c->txi_irq) != p_i2c->txi_priority)
    {
        return (ARM_DRIVER_ERROR);
    }
    
    /* Register tei interrupt handler */
    if ((-1) == R_SYS_IrqEventLinkSet(p_i2c->tei_irq, p_i2c->tei_iesr_val, p_i2c->tei_callback))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* TEI priority setting */
    R_NVIC_SetPriority(p_i2c->tei_irq, p_i2c->tei_priority);
    if (R_NVIC_GetPriority(p_i2c->tei_irq) != p_i2c->tei_priority)
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Register rxi interrupt handler */
    if ((-1) == R_SYS_IrqEventLinkSet(p_i2c->rxi_irq, p_i2c->rxi_iesr_val, p_i2c->rxi_callback))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* RXI priority setting */
    R_NVIC_SetPriority(p_i2c->rxi_irq, p_i2c->rxi_priority);
    if (R_NVIC_GetPriority(p_i2c->rxi_irq) != p_i2c->rxi_priority)
    {
        return (ARM_DRIVER_ERROR);
    }

    /* Register eei interrupt handler */
    if ((-1) == R_SYS_IrqEventLinkSet(p_i2c->eei_irq, p_i2c->eei_iesr_val, p_i2c->eei_callback))
    {
        return (ARM_DRIVER_ERROR);
    }

    /* EEI priority setting */
    R_NVIC_SetPriority(p_i2c->eei_irq, p_i2c->eei_priority);
    if (R_NVIC_GetPriority(p_i2c->eei_irq) != p_i2c->eei_priority)
    {
        return (ARM_DRIVER_ERROR);
    }
    
    /* Lock I2C resource */
    if (0 != R_SYS_ResourceLock(p_i2c->lock_id))
    {
        return (ARM_DRIVER_ERROR);
    }
    
    /* Initialize i2c driver */
    p_i2c->info->cb_event = cb_event;
    
    /* Clear driver status */
    p_i2c->info->status.arbitration_lost = 0;
    p_i2c->info->status.busy             = 0;
    p_i2c->info->status.bus_error        = 0;
    p_i2c->info->status.direction        = RIIC_RECIVER;
    p_i2c->info->status.general_call     = 0;
    p_i2c->info->status.mode             = RIIC_MODE_SLAVE;
    p_i2c->info->state                   = RIIC_STATE_START;
    p_i2c->info->nack                    = RIIC_NACK_NONE;
    p_i2c->info->pending                 = false;
    p_i2c->info->own_sla                 = RIIC_DEFAULT_ADDR; // @suppress("Cast comment")
    p_i2c->info->bps                     = RIIC_DEFAULT_BPS; // @suppress("Cast comment")
    p_i2c->info->flags                   = RIIC_FLAG_INITIALIZED;
    p_i2c->pin_set();
    return (ARM_DRIVER_OK);
}/* End of function ARM_I2C_Initialize() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver I2C Uninitialization API for the I2C peripheral on RE Family.
 * @param[in]   p_i2c        Pointer to I2C resource data
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_Uninitialize */
static int32_t ARM_I2C_Uninitialize(st_i2c_resources_t *p_i2c) // @suppress("Non-API function naming")
{
    if (0x00 == (p_i2c->info->flags & RIIC_FLAG_INITIALIZED))
    {
        return (ARM_DRIVER_OK);
    }
    
    
    /* If it is in the power on state, turn off the power and release the lock */
    if (0U != (p_i2c->info->flags & RIIC_FLAG_POWERED)) 
    {
        ARM_I2C_PowerControl(ARM_POWER_OFF, p_i2c);
    }

    p_i2c->pin_clr();
    p_i2c->info->flags = 0;

    /* Unlock I2C resource */
    R_SYS_ResourceUnlock(p_i2c->lock_id);

    return (ARM_DRIVER_OK);

}/* End of function ARM_I2C_Uninitialize() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver I2C Power Control API for the I2C peripheral on RE Family.
 * @param[in]   state  ARM_POWER_OFF(Stop module clock), ARM_POWER_FULL(Supply module clock)
 * @param[in]   p_i2c  Pointer to I2C resource data
 * @retval      ARM_DRIVER_OK                Power control operation was successful.
 * @retval      ARM_DRIVER_ERROR             Specified I2C mode is not supported.
 * @retval      ARM_DRIVER_ERROR_UNSUPPORTED Power mode specified is not supported.
 * @note Power state ARM_POWER_LOW is not supported.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_PowerControl */
static int32_t ARM_I2C_PowerControl(ARM_POWER_STATE state, st_i2c_resources_t *p_i2c) // @suppress("Non-API function naming")
{
    
    int32_t result = ARM_DRIVER_OK;
    volatile uint8_t uctmp;
    
    switch (state)
    {
        case ARM_POWER_OFF:
        {
            if (0U == (p_i2c->info->flags & RIIC_FLAG_INITIALIZED)) 
            {
                result = ARM_DRIVER_ERROR;
                break;
            }
            
            /* Disable interrupts */
            R_NVIC_DisableIRQ(p_i2c->txi_irq);
            R_NVIC_DisableIRQ(p_i2c->rxi_irq);
            R_NVIC_DisableIRQ(p_i2c->tei_irq);
            R_NVIC_DisableIRQ(p_i2c->eei_irq);


            /* ICE - I2C Bus Interface Enable - Disable. */
            p_i2c->reg->ICCR1_b.ICE              = 0;

            /* IICRST - I2C Bus Interface Internal Reset - Initiates the RIIC reset or internal reset. */
            p_i2c->reg->ICCR1_b.IICRST           = 1;

            p_i2c->info->status.arbitration_lost = 0;
            p_i2c->info->status.busy             = 0;
            p_i2c->info->status.bus_error        = 0;
            p_i2c->info->status.direction        = RIIC_RECIVER;
            p_i2c->info->status.general_call     = 0;
            p_i2c->info->status.mode             = RIIC_MODE_SLAVE;
            p_i2c->info->nack                    = RIIC_NACK_NONE;
            p_i2c->info->pending                 = false;

            /* IICRST - I2C Bus Interface Internal Reset - Releases the RIIC reset or internal reset. */
            p_i2c->reg->ICCR1_b.IICRST = 0;

            (void)R_LPM_ModuleStop(p_i2c->mstp_id); // @suppress("Cast comment")
            p_i2c->info->flags &= (~RIIC_FLAG_POWERED);
        }
        break;
    
    
        case ARM_POWER_FULL:
        {
            if (0U == (p_i2c->info->flags & RIIC_FLAG_INITIALIZED)) 
            {
                result = ARM_DRIVER_ERROR;
                break;
            }
            if (0U != (p_i2c->info->flags & RIIC_FLAG_POWERED)) 
            {
                break;
            }
            
            /* Release module stop */
            if (0 != R_LPM_ModuleStart(p_i2c->mstp_id))
            {
                result = ARM_DRIVER_ERROR;
                break;
            }
            result = riic_setting(p_i2c->info->bps, p_i2c);
            if (ARM_DRIVER_OK == result)
            {
                p_i2c->info->flags |= RIIC_FLAG_POWERED;
            }
            else
            {
                /* Set to module stop */
                (void) R_LPM_ModuleStop(p_i2c->mstp_id);
            }
        }
        break;
    
        case ARM_POWER_LOW:
        default:
        {
            result = ARM_DRIVER_ERROR_UNSUPPORTED;
        }
        break;
    }
    
    return (result);
}/* End of function ARM_I2C_PowerControl() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver I2C Master Data Transmission API for the I2C peripheral
 *              on RE Family.
 *
 *              This function ARM_I2C_MasterTransmit transmits data as Master to the selected Slave.
 * @param[in]        addr                 Slave address (7-bit or 10-bit) 
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Stop condition will not be generated 
 * @param[in,out]    p_i2c                Pointer to I2C resource data
 * @retval      ARM_DRIVER_OK        Data transmission was successful.
 * @retval      ARM_DRIVER_ERROR     Specified I2C mode is not supported.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_MasterTransmit */
static int32_t ARM_I2C_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending, // @suppress("Non-API function naming") @suppress("Function length")
                               st_i2c_resources_t *p_i2c)
{
    uint8_t icmr3_tmp;

    if (0U == (p_i2c->info->flags & RIIC_FLAG_POWERED)) 
    {
        return (ARM_DRIVER_ERROR);
    }
    
    /* If the transmission size is other than 0 and there is no data, an error is returned */
    if ((0 < num) && (NULL == data))
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    if (1 == p_i2c->info->status.busy)
    {
        return (ARM_DRIVER_ERROR_BUSY);
    }
    if (0x80 == (p_i2c->reg->ICCR2 & 0xC0))
    {
        /* In the case of bus busy also in master mode, it is judged that the previous stop condition is not output.
           (xfer_pending = true) */
        return (ARM_DRIVER_ERROR_BUSY);
    }

    /* Clear driver status */
    p_i2c->info->status.busy             = 1;
    p_i2c->info->status.arbitration_lost = 0;
    p_i2c->info->status.bus_error        = 0;
    p_i2c->info->status.direction        = RIIC_TRANSMITTER;
    p_i2c->info->status.general_call     = 0;
    p_i2c->info->status.mode             = RIIC_MODE_MASTER;
    p_i2c->info->nack                    = RIIC_NACK_NONE;
    p_i2c->info->pending                 = xfer_pending;
    p_i2c->info->state                   = RIIC_STATE_START;
    
    p_i2c->xfer_info->cnt                = 0;
    p_i2c->xfer_info->tx_num             = num;
    p_i2c->xfer_info->rx_num             = 0;
    p_i2c->xfer_info->tx_buf             = data;
    p_i2c->xfer_info->f_tx_end           = false;
    if (RIIC_ADDR_NONE != addr)
    {
        p_i2c->xfer_info->sla = (((addr<<1) | RIIC_WRITE) & 0x000000FF);
    }
    else
    {
        p_i2c->xfer_info->sla = RIIC_ADDR_NONE;
    }
    
    /* Set ACK transmission */
    if (0 == p_i2c->reg->ICMR3_b.ACKWP)
    {
        /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is enabled. */
        p_i2c->reg->ICMR3_b.ACKWP = 1;
    }
    
    /* ACKBT - Transmit Acknowledge - 0 is sent as the acknowledge bit (ACK transmission). */
    /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled. */
    icmr3_tmp = p_i2c->reg->ICMR3;
    icmr3_tmp &= 0xE7;
    p_i2c->reg->ICMR3 = icmr3_tmp;
    
    /* ICSR2  - I2C Bus Status Register 2 
       b7   TDRE - Transmit Data Empty Flag
       b6   TEND - Transmit End Flag
       b5   RDRF - Receive Data Full Flag
       b4   NACKF - NACK Detection Flag - NACK is not detected.
       b3   STOP - Stop Condition Detection Flag - Stop condition is not detected.
       b2   START - Start Condition Detection Flag - Start condition is not detected.
       b1   AL - Arbitration-Lost Flag - Arbitration is not lost.
       b0   TMOF - Timeout Detection Flag - Timeout is not detected.  */
    p_i2c->reg->ICSR2 &= 0x00;
    iic_interrupt_clear(p_i2c);
    
    /* ICIER  - I2C Bus Interrupt Enable Register
       b7   TIE - Transmit Data Empty Interrupt Request Enable - enabled.
       b6   TEIE - Transmit End Interrupt Request Enable - disabled.
       b5   RIE - Receive Data Full Interrupt Request Enable - enabled.
       b4   NAKIE - NACK Reception Interrupt Request Enable - enabled.
       b3   SPIE - Stop Condition Detection Interrupt Request Enable - disabled.
       b2   STIE - Start Condition Detection Interrupt Request Enable - disabled.
       b1   ALIE - Arbitration-Lost Interrupt Request Enable - disabled.
       b0   TMOIE - Timeout Interrupt Request Enable - disabled.  */
    p_i2c->reg->ICIER = 0xB2;
    
    if (0xC0 != (p_i2c->reg->ICCR2 & 0xC0))
    {
        /* ST - Start Condition Issuance Request - Requests to issue a start condition. */
        p_i2c->reg->ICCR2_b.ST = 1;
    }
    else
    {
        /* RS - Restart Condition Issuance Request - Requests to issue a restart condition. */
        p_i2c->reg->ICCR2_b.RS = 1;
    }
    R_NVIC_EnableIRQ(p_i2c->txi_irq);
    R_NVIC_EnableIRQ(p_i2c->tei_irq);
    R_NVIC_EnableIRQ(p_i2c->eei_irq);

    return (ARM_DRIVER_OK);
}/* End of function ARM_I2C_MasterTransmit() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver I2C Master Data Reception API for the I2C peripheral on RE Family.
 *
 *              This function ARM_I2C_MasterReceive is used to receive data as Master from the selected Slave.
 * @param[in]        addr                 Slave address (7-bit or 10-bit) 
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in]        xfer_pending         Transfer operation is pending - Only false can be used 
 * @param[in,out]    i2c                  Pointer to I2C resource data
 * @retval      ARM_DRIVER_OK        Data transmission was successful.
 * @retval      ARM_DRIVER_ERROR     Specified I2C mode is not supported.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_MasterReceive */
static int32_t ARM_I2C_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending, // @suppress("Function length") @suppress("Non-API function naming")
                               st_i2c_resources_t *p_i2c)
{
    uint8_t icmr3_tmp;

    if (0U == (p_i2c->info->flags & RIIC_FLAG_POWERED)) 
    {
        return (ARM_DRIVER_ERROR);
    }
    
    /* Master reception mode can not be used while operating in pending mode */
    if (true == xfer_pending)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* Address specification can not be used in master receive mode */
    if (RIIC_ADDR_NONE == addr)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* When the reception size is 0, it returns an error */
    if (0 == num)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* It returns an error if the data buffer is NULL */
    if (NULL == data)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* If busy, return error */
    if (1 == p_i2c->info->status.busy)
    {
        return (ARM_DRIVER_ERROR_BUSY);
    }
    if (0x80 == (p_i2c->reg->ICCR2 & 0xC0))
    {
        /* In the case of bus busy also in master mode, it is judged that the previous stop condition is not output.
           (xfer_pending = true) */
        return (ARM_DRIVER_ERROR_BUSY);
    }

    /* Clear driver status */
    p_i2c->info->status.busy             = 1;
    p_i2c->info->status.arbitration_lost = 0;
    p_i2c->info->status.bus_error        = 0;
    p_i2c->info->status.direction        = RIIC_RECIVER;
    p_i2c->info->status.general_call     = 0;
    p_i2c->info->status.mode             = RIIC_MODE_MASTER;
    p_i2c->info->nack                    = RIIC_NACK_NONE;
    p_i2c->info->pending                 = xfer_pending;
    p_i2c->info->state                   = RIIC_STATE_START;
    
    p_i2c->xfer_info->cnt                = 0;
    p_i2c->xfer_info->tx_num             = num;
    p_i2c->xfer_info->rx_num             = num;
    p_i2c->xfer_info->rx_buf             = data;
    p_i2c->xfer_info->sla                = (((addr<<1) | RIIC_READ) & 0x000000FF);
    p_i2c->xfer_info->f_tx_end           = false;
    
    if (0 == p_i2c->reg->ICMR3_b.ACKWP)
    {
        /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is enabled. */
        p_i2c->reg->ICMR3_b.ACKWP = 1;
    }

    /* ACKBT - Transmit Acknowledge - 0 is sent as the acknowledge bit (ACK transmission). */
    /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled. */
    icmr3_tmp = p_i2c->reg->ICMR3;
    icmr3_tmp &= 0xE7;
    p_i2c->reg->ICMR3 = icmr3_tmp;

    /* ICSR2  - I2C Bus Status Register 2 
       b7   TDRE - Transmit Data Empty Flag
       b6   TEND - Transmit End Flag
       b5   RDRF - Receive Data Full Flag
       b4   NACKF - NACK Detection Flag - NACK is not detected.
       b3   STOP - Stop Condition Detection Flag - Stop condition is not detected.
       b2   START - Start Condition Detection Flag - Start condition is not detected.
       b1   AL - Arbitration-Lost Flag - Arbitration is not lost.
       b0   TMOF - Timeout Detection Flag - Timeout is not detected.  */
    p_i2c->reg->ICSR2 &= 0x00;
    iic_interrupt_clear(p_i2c);
    
    /* ICIER  - I2C Bus Interrupt Enable Register
       b7   TIE - Transmit Data Empty Interrupt Request Enable - enabled.
       b6   TEIE - Transmit End Interrupt Request Enable - disabled.
       b5   RIE - Receive Data Full Interrupt Request Enable - enabled.
       b4   NAKIE - NACK Reception Interrupt Request Enable - enabled.
       b3   SPIE - Stop Condition Detection Interrupt Request Enable - disabled.
       b2   STIE - Start Condition Detection Interrupt Request Enable - disabled.
       b1   ALIE - Arbitration-Lost Interrupt Request Enable - disabled.
       b0   TMOIE - Timeout Interrupt Request Enable - disabled.  */
    p_i2c->reg->ICIER = 0xB2;
    if (0xC0 != (p_i2c->reg->ICCR2 & 0xC0))
    {
        /* ST - Start Condition Issuance Request - Requests to issue a start condition. */
        p_i2c->reg->ICCR2_b.ST = 1;
    }
    else
    {
        /* RS - Restart Condition Issuance Request - Requests to issue a restart condition. */
        p_i2c->reg->ICCR2_b.RS = 1;
    }
    R_NVIC_EnableIRQ(p_i2c->txi_irq);
    R_NVIC_EnableIRQ(p_i2c->rxi_irq);
    R_NVIC_EnableIRQ(p_i2c->eei_irq);
    
    return (ARM_DRIVER_OK);
}/* End of function ARM_I2C_MasterReceive() */

/****************************************************************************************************************//**
 * @brief       The implementation of CMSIS-Driver I2C Slave Data Transmission API for the I2C peripheral
 *              on RE Family.
 *
 *              This function ARM_I2C_SlaveTransmit is used to transmit data as Slave to the Master.
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in,out]    p_i2c                Pointer to I2C resource data
 * @retval      ARM_DRIVER_OK        Data transmission was successful.
 * @retval      ARM_DRIVER_ERROR     Specified I2C mode is not supported.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_SlaveTransmit */
static int32_t ARM_I2C_SlaveTransmit(const uint8_t *data, uint32_t num, st_i2c_resources_t *p_i2c) // @suppress("Non-API function naming")
{
    if (0U == (p_i2c->info->flags & RIIC_FLAG_POWERED)) 
    {
        return (ARM_DRIVER_ERROR);
    }
    
    /* When the reception size is 0, it returns an error */
    if (0 == num)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* It returns an error if the data buffer is NULL */
    if (NULL == data)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* If busy, return error */
    if (1 == p_i2c->info->status.busy)
    {
        return (ARM_DRIVER_ERROR_BUSY);
    }
    
    /* Clear driver status */
    p_i2c->info->status.busy             = 1;
    p_i2c->info->status.arbitration_lost = 0;
    p_i2c->info->status.bus_error        = 0;
    p_i2c->info->status.direction        = RIIC_TRANSMITTER;
    p_i2c->info->status.general_call     = 0;
    p_i2c->info->status.mode             = RIIC_MODE_SLAVE;
    p_i2c->info->nack                    = RIIC_NACK_NONE;
    p_i2c->info->pending                 = 0;
    p_i2c->info->state                   = RIIC_STATE_START;
    
    p_i2c->xfer_info->cnt                = 0;
    p_i2c->xfer_info->tx_num             = num;
    p_i2c->xfer_info->rx_num             = 0;
    p_i2c->xfer_info->tx_buf             = data;
    p_i2c->xfer_info->f_tx_end           = false;
    
    iic_interrupt_clear(p_i2c);
    riic_reset(p_i2c, RIIC_RECV_ENABLE);
    
    R_NVIC_EnableIRQ(p_i2c->txi_irq);
    R_NVIC_EnableIRQ(p_i2c->rxi_irq);
    R_NVIC_EnableIRQ(p_i2c->tei_irq);
    R_NVIC_EnableIRQ(p_i2c->eei_irq);
    return (ARM_DRIVER_OK);
}/* End of function ARM_I2C_SlaveTransmit() */

/****************************************************************************************************************//**
 *  @brief       The implementation of CMSIS-Driver I2C Slave Data Reception API for the I2C peripheral on RE Family.
 *
 *               This function ARM_I2C_SlaveReceive receives data as Slave from the Master.
 * @param[in]        data                 Pointer to buffer with data to transmit to I2C Slave 
 * @param[in]        num                  Number of data to transmit 
 * @param[in,out]    p_i2c                Pointer to I2C resource data
 * @retval      ARM_DRIVER_OK        Data transmission was successful.
 * @retval      ARM_DRIVER_ERROR     Specified I2C mode is not supported.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_SlaveReceive */
static int32_t ARM_I2C_SlaveReceive(uint8_t *data, uint32_t num, st_i2c_resources_t *p_i2c) // @suppress("Non-API function naming")
{
    if (0U == (p_i2c->info->flags & RIIC_FLAG_POWERED)) 
    {
        return (ARM_DRIVER_ERROR);
    }
    
    /* When the reception size is 0, it returns an error */
    if (0 == num)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* It returns an error if the data buffer is NULL */
    if (NULL == data)
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }
    
    /* If busy, return error */
    if (1 == p_i2c->info->status.busy)
    {
        return (ARM_DRIVER_ERROR_BUSY);
    }

    /* Clear driver status */
    p_i2c->info->status.busy             = 1;
    p_i2c->info->status.arbitration_lost = 0;
    p_i2c->info->status.bus_error        = 0;
    p_i2c->info->status.direction        = RIIC_RECIVER;
    p_i2c->info->status.general_call     = 0;
    p_i2c->info->status.mode             = RIIC_MODE_SLAVE;
    p_i2c->info->nack                    = RIIC_NACK_NONE;
    p_i2c->info->pending                 = 0;
    p_i2c->info->state                   = RIIC_STATE_START;
    
    p_i2c->xfer_info->cnt                = 0;
    p_i2c->xfer_info->tx_num             = 0;
    p_i2c->xfer_info->rx_num             = num;
    p_i2c->xfer_info->rx_buf             = data;
    p_i2c->xfer_info->f_tx_end           = false;
    
    iic_interrupt_clear(p_i2c);
    riic_reset(p_i2c, RIIC_RECV_ENABLE);
    R_NVIC_EnableIRQ(p_i2c->rxi_irq);
    R_NVIC_EnableIRQ(p_i2c->txi_irq);
    R_NVIC_EnableIRQ(p_i2c->eei_irq);
    
    return (ARM_DRIVER_OK);
}/* End of function ARM_I2C_SlaveReceive() */

/****************************************************************************************************************//**
* @brief    Get transmitted data count.
* @retval   number of data items transmitted
**********************************************************************************************************************/
/* Function Name : ARM_I2C_GetDataCount */
static int32_t ARM_I2C_GetDataCount(st_i2c_resources_t *p_i2c) // @suppress("Non-API function naming")
{
  return (p_i2c->xfer_info->cnt);
}/* End of function ARM_I2C_GetDataCount() */

/****************************************************************************************************************//**
 * @brief       CMSIS-Driver I2C Control API implementation for the I2C peripheral on RE Family.
 * @param[in]        control      Operation command
 * @param[in]        arg          Argument of operation command
 * @param[in,out]    p_i2c        Pointer to a I2C resource structure
 * @retval      ARM_DRIVER_OK     Specified contol operation was successful.
 * @retval      ARM_DRIVER_ERROR  Specified command is not supported.
 * @retval      ARM_DRIVER_ERROR_UNSUPPORTED Specified I2C mode is not supported.
**********************************************************************************************************************/
/* Function Name : ARM_I2C_Control */
static int32_t ARM_I2C_Control(uint32_t control, uint32_t arg, st_i2c_resources_t *p_i2c) // @suppress("Function length") @suppress("Non-API function naming")
{
    int32_t result = ARM_DRIVER_OK;
    uint8_t i;
    uint8_t timeout;
    uint8_t icmr3_tmp;

    if (0U == (p_i2c->info->flags & RIIC_FLAG_POWERED)) 
    {
        return (ARM_DRIVER_ERROR);
    }
    
    switch (control)
    {
        /* Set own address command */
        case ARM_I2C_OWN_ADDRESS:
        {
            p_i2c->info->own_sla = arg;
        }
        break;
        
        /* Bus speed change command */
        case ARM_I2C_BUS_SPEED:
        {
            switch (arg)
            {
                /* Standard speed */
                case ARM_I2C_BUS_SPEED_STANDARD:
                {
                    if (0 == p_i2c->info->status.busy)
                    {
                        result = riic_setting(RIIC_STAD_SPPED_MAX, p_i2c); // @suppress("Cast comment")
                    }
                    else
                    {
                        result = ARM_DRIVER_ERROR_BUSY;
                    }
                }
                break;
                
                /* Fast speed */
                case ARM_I2C_BUS_SPEED_FAST:
                {
                    if (0 == p_i2c->info->status.busy)
                    {
                        result = riic_setting(RIIC_FAST_SPPED_MAX, p_i2c); // @suppress("Cast comment")
                    }
                    else
                    {
                        result = ARM_DRIVER_ERROR_BUSY;
                    }
                }
                break;

                /* Fast + and high speed mode are not supported */
                case ARM_I2C_BUS_SPEED_FAST_PLUS:
                case ARM_I2C_BUS_SPEED_HIGH:
                default:
                {
                    return (ARM_DRIVER_ERROR_UNSUPPORTED);
                }
            }
        }
        break;
        
        /* Bus clear command */
        case ARM_I2C_BUS_CLEAR:
        {
            if (1 == p_i2c->reg->ICCR2_b.MST)
            {
                /* In master mode, clock 9 clocks are output */
                
                /* MALE - Master Arbitration-Lost Detection Enable - Disabled. */
                p_i2c->reg->ICFER_b.MALE = 0;
                
                for (i = 0; i < IIC_PRV_OUT_9CLOCK; i++)
                {
                    timeout = 12;     /* Timeout: 12us */

                    /* CLO - Extra SCL Clock Cycle Output - Outputs an extra SCL clock cycle.. */
                    p_i2c->reg->ICCR1_b.CLO = 1;
                    while(1 == p_i2c->reg->ICCR1_b.CLO)
                    {
                        if ( 0 == (timeout --) )
                        {
                            /* timeout */
                            break;
                        }
                        R_SYS_SoftwareDelay(1, SYSTEM_DELAY_UNITS_MICROSECONDS);     /* 1us wait */
                    }
                    
                }

                /* MALE - Master Arbitration-Lost Detection Enable - Enabled. */
                p_i2c->reg->ICFER_b.MALE = 1;
                
            }
            else if (0 == p_i2c->reg->ICCR2_b.BBSY)
            {
                /* If it is not in the communication state, set 9 clocks after setting it as master */
                
                /* MALE - Master Arbitration-Lost Detection Enable - Disabled. */
                p_i2c->reg->ICFER_b.MALE = 0;

                /* ST - Start Condition Issuance Request - Requests to issue a start condition. */
                p_i2c->reg->ICCR2_b.ST   = 1;
                
                timeout = 12;     /* Timeout: 12us */
                while(1 == p_i2c->reg->ICCR2_b.ST)
                {
                    if ( 0 == (timeout --) )
                    {
                        /* timeout */
                        break;
                    }
                    R_SYS_SoftwareDelay(1, SYSTEM_DELAY_UNITS_MICROSECONDS);     /* 1us wait */
                }

                /* Wait for 400 kbps "L" output min time (1.3 us) or more */
                R_SYS_SoftwareDelay(2, SYSTEM_DELAY_UNITS_MICROSECONDS);
                
                for (i = 0; i < IIC_PRV_OUT_9CLOCK; i++)
                {
                    /* CLO - Extra SCL Clock Cycle Output - Outputs an extra SCL clock cycle.. */
                    p_i2c->reg->ICCR1_b.CLO = 1;
                    timeout = 12;     /* Timeout: 12us */
                    while(1 == p_i2c->reg->ICCR1_b.CLO)
                    {
                        if ( 0 == (timeout --) )
                        {
                            /* timeout */
                            break;
                        }
                        R_SYS_SoftwareDelay(1, SYSTEM_DELAY_UNITS_MICROSECONDS);     /* 1us wait */
                    }
                    
                }

                /* SP - Stop Condition Issuance Request - Requests to issue a stop condition. */
                p_i2c->reg->ICCR2_b.SP    = 1;
                timeout = 12;
                while(1 == p_i2c->reg->ICCR2_b.SP)
                {
                    if ( 0 == (timeout --) )
                    {
                        /* timeout */
                        break;
                    }
                    R_SYS_SoftwareDelay(1, SYSTEM_DELAY_UNITS_MICROSECONDS);     /* 1us wait */
                }

                /* MALE - Master Arbitration-Lost Detection Enable - Enabled. */
                p_i2c->reg->ICFER_b.MALE = 1;
                
            }
            else
            {
                if (1 == p_i2c->reg->ICCR2_b.TRS)
                {
                    /* ICDRT - I2C Bus Transmit Data Register. */
                    p_i2c->reg->ICDRT = 0xFF;
                }
                else
                {
                    /* In the case of slave reception, set it as a NACK response */
                    if (0 == p_i2c->reg->ICMR3_b.ACKWP)
                    {
                        /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is enabled. */
                        p_i2c->reg->ICMR3_b.ACKWP = 1;
                    }

                    /* ACKBT - Transmit Acknowledge - 1 sent as the acknowledge bit (NACK transmission). */
                    /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled. */
                    icmr3_tmp = p_i2c->reg->ICMR3;
                    icmr3_tmp &= 0xE7;
                    icmr3_tmp |= 0x08;
                    p_i2c->reg->ICMR3 = icmr3_tmp;

                }
            }
        }
        break;
    
        /* Abort transfer command */
        case ARM_I2C_ABORT_TRANSFER:
        {
            /* In the case of the master, I 2 C reset is performed after outputting the stop condition */
            iic_interrupt_clear(p_i2c);
            if (1 == p_i2c->reg->ICCR2_b.MST)
            {
                timeout = 12;     /* Timeout: 12us */

                /* SP - Stop Condition Issuance Request - Requests to issue a stop condition. */
                p_i2c->reg->ICCR2_b.SP = 1;
                while(1 == p_i2c->reg->ICCR2_b.SP)
                {
                    if ( 0 == (timeout --) )
                    {
                        /* timeout */
                        break;
                    }
                    R_SYS_SoftwareDelay(1, SYSTEM_DELAY_UNITS_MICROSECONDS);     /* 1us wait */
                }
            }
            p_i2c->info->status.busy = 0;
            riic_reset(p_i2c, RIIC_RECV_DISABLE);
        }
        break;
    
        default:
        {
            return (ARM_DRIVER_ERROR_UNSUPPORTED);
        }
        break;
    }
    return (result);
}/* End of function ARM_I2C_Control() */

/****************************************************************************************************************//**
 *  @brief       Get IIC Status.
 *  @param[in]   p_i2c  Pointer to a I2C resource structure
 *  @retval      ARM_I2C_STATUS \ref ARM_I2C_STATUS
**********************************************************************************************************************/
/* Function Name : ARM_I2C_GetStatus */
static ARM_I2C_STATUS ARM_I2C_GetStatus(st_i2c_resources_t *p_i2c) // @suppress("Non-API function naming")
{
    return (p_i2c->info->status);
}/* End of function ARM_I2C_GetStatus() */

/****************************************************************************************************************//**
 * @brief       Calculation for ICBRH and ICBRL registers value and CKS bits value from setting bps of the argument.
 * @param[in]        reg_val        Pointer to IIC regster value
 * @param[in]        kbps           bit rate(kHz)
 * @retval      RIIC_SUCCESS        Successful operation, communication state
 * @retval      RIIC_ERR_OTHER      Other error
**********************************************************************************************************************/
/* Function Name : riic_bps_calc */
static int32_t riic_bps_calc (uint16_t kbps, st_i2c_reg_buf_t *reg_val) // @suppress("Function length")
{

#if (0 != RIIC_BUS_SPEED_CAL_ENABLE)
    double pclk_val;

    double bps = (double) (kbps * 1000); // @suppress("Cast comment")
    double l_time; /* H Width period */
    double h_time; /* L Width period */

    double calc_val; /* Using for calculation buffer */

    double scl_up_time;
    double scl_down_time;
    volatile uint8_t cnt;
    uint32_t pclkb_freq;
    uint8_t reg_offset;
    uint8_t reg_set_buff;
    
    (void)R_SYS_PeripheralClockFreqGet(&pclkb_freq); /* Store pclk frequency */ // @suppress("Cast comment")
    pclk_val = (double)pclkb_freq;
    
    /* Set Rise up time and down time */
    if (kbps > RIIC_FAST_SPPED_MAX)
    {
        /* When bps more than 400Kbps[Fast mode plus] */
        scl_up_time = gs_scl1m_up_time;
        scl_down_time = gs_scl1m_down_time;
    }
    else if (kbps > RIIC_STAD_SPPED_MAX) // @suppress("Cast comment")
    {
        /* When bps more than 100Kbps[Fast mode] */
        scl_up_time = gs_scl400k_up_time;
        scl_down_time = gs_scl400k_down_time;
    }
    else
    {
        /* When bps less than 100Kbps[Standard mode] */
        scl_up_time = gs_scl100k_up_time;
        scl_down_time = gs_scl100k_down_time;
    }

    /* Calculation for ICBRH and ICBRL registers value */

    /* Calculation L width time */
    l_time = (1 / (2 * bps)); /* Harf period of frequency */
    h_time = l_time;

    /* Check I2C mode of Speed */
    if (kbps > RIIC_FAST_SPPED_MAX)
    {
        /* 400kbps over */
        /* Check L width */
        if (l_time < 0.5E-6)
        {
            /* Wnen L width less than 0.5us */
            /* Subtract Rise up and down time for SCL from H/L width */
            l_time = 0.5E-6;
            h_time = (((1 / bps) - l_time) - scl_up_time) - scl_down_time;
        }
        else
        {
            /* Subtract Rise up and down time for SCL from H/L width */
            l_time -= scl_down_time;
            h_time -= scl_up_time;
        }
    }
    else if (kbps > RIIC_STAD_SPPED_MAX) // @suppress("Cast comment")
    {
        /* 100kbps over */
        /* Check L width */
        if (l_time < 1.3E-6)
        {
            /* Wnen L width less than 1.3us */
            /* Subtract Rise up and down time for SCL from H/L width */
            l_time = 1.3E-6;
            h_time = (((1 / bps) - l_time) - scl_up_time) - scl_down_time;
        }
        else
        {
            /* Subtract Rise up and down time for SCL from H/L width */
            l_time -= scl_down_time;
            h_time -= scl_up_time;
        }
    }
    else
    {
        /* less than 100kbps */
        l_time -= scl_down_time;
        h_time -= scl_up_time;
    }

    /*************** Calculation ICBRL value ***********************/
    /* Calculating until calc_val is less than 32 */
    for (calc_val = 0xFF, cnt = 0; RIIC_ICBR_MAX < calc_val; cnt++)
    {
        calc_val = l_time; /* Set L width time */

        /* Check the range of divider of CKS */
        if (cnt >= RIIC_MAX_DIV)
        {
            /* Cannot set bps */
            return (ARM_DRIVER_ERROR);
        }

        calc_val = (calc_val / (gs_d_cks[cnt] / pclk_val));/* Calculattion ICBRL value */
        calc_val = calc_val + 0.5; /* round off */
    }

    /***********************************************************************************************/
    /* Calculate offset for setting to ICBRL, ICBRH                                                */
    /* + 3 for fIIC = PCLK, +2 for fIIC = PCLK / 2 or less                                         */
    /* When the noise filter is effective, the number of stages of the noise filter is also added  */
    /***********************************************************************************************/
    if (0 == (cnt - 1))
    {
        reg_offset = 3 + RIIC_NOISE_FILTER;
    }
    else
    {
        reg_offset = 2 + RIIC_NOISE_FILTER;
    }
    
    reg_set_buff = (uint8_t)calc_val; // @suppress("Cast comment")
    if (reg_set_buff > reg_offset)
    {
        reg_set_buff -= reg_offset;
    }
    else
    {
        reg_set_buff = 0;
    }

    /* store ICMR1 value to avoid CKS bit. */
    reg_val->cks   = (((cnt - 1) << 4) & 0x70);
    reg_val->icbrl = (reg_set_buff | 0xE0);

    /*************** Calculation ICBRH value ***********************/
    calc_val = h_time; /* Set H width */
    calc_val = (calc_val / (gs_d_cks[cnt - 1] / pclk_val)); /* Calculattion ICBRH value */
    calc_val = (uint8_t) (calc_val + 0.5); /* round off */

    /* If the calculated value is less than reg_offset, it rounded up to 0. */
    reg_set_buff = (uint8_t)calc_val;
    if (reg_set_buff > reg_offset)
    {
        reg_set_buff -= reg_offset;
    }
    else
    {
        reg_set_buff = 0;
    }

    reg_val->icbrh = (reg_set_buff | 0xE0);

#else
    switch (kbps)
    {
        /* Standard speed setting */
        case RIIC_STAD_SPPED_MAX:
            reg_val->cks   = (RIIC_STAD_CKS << 4) & 0x70;
            reg_val->icbrl = RIIC_STAD_ICBRL & 0x1F;
            reg_val->icbrh = RIIC_STAD_ICBRH & 0x1F;
        break;

        /* Fast speed setting */
        case RIIC_FAST_SPPED_MAX:
            reg_val->cks   = (RIIC_FAST_CKS << 4) & 0x70;
            reg_val->icbrl = RIIC_FAST_ICBRL & 0x1F;
            reg_val->icbrh = RIIC_FAST_ICBRH & 0x1F;
        break;
        
        default:
            return ARM_DRIVER_ERROR;
        break;
    }
#endif

    return (ARM_DRIVER_OK);

} /* End of function riic_bps_calc() */

/****************************************************************************************************************//**
* @brief        Setting to IIC register and bit rate
*
*               This function set to IIC register and bit rate.
*               If the bit rate calculation is successful, set the IIC register and return ARM_DRIVER_OK.
*               If the bit rate calculation fails, return ARM_DRIVER_ERROR.
* @param[in]    bps    bitrate
* @param[in]    p_i2c    Pointer to I2c resources
* @retval      ARM_DRIVER_OK     Setting to I2C was successful.
* @retval      ARM_DRIVER_ERROR  Specified baudrate is not supported.
**********************************************************************************************************************/
/* Function Name : riic_setting */
static int32_t riic_setting(uint16_t bps, st_i2c_resources_t *p_i2c) // @suppress("Function length")
{
    int32_t result = ARM_DRIVER_OK;
    st_i2c_reg_buf_t reg_set;
    uint8_t uctmp;
    
    result = riic_bps_calc(bps, &reg_set);
    
    if (ARM_DRIVER_OK == result)
    {
        /* ICE - I2C Bus Interface Enable - Disable. */
        p_i2c->reg->ICCR1_b.ICE    = 0;

        /* IICRST - I2C Bus Interface Internal Reset - Initiates the RIIC reset or internal reset. */
        p_i2c->reg->ICCR1_b.IICRST = 1;

        /* ICE - I2C Bus Interface Enable - Enable. */
        p_i2c->reg->ICCR1_b.ICE    = 1;

        /* SARL0  - Slave Address Register L0 
           b7:b1  SVA[6:0] - 7-Bit Address/10-Bit Address Lower Bits
           b0     SVA0 - 10-Bit Address LSB  */
        p_i2c->reg->SARL0 = (uint8_t)((p_i2c->info->own_sla)<<1);

        /* SARU0  - Slave Address Register U0 
           b7:b3  Reserved - These bits are read as 0. The write value should be 0.
           b2:b1  SVA[1:0] - 10-Bit Address Upper Bits
           b0     FS - 7-Bit/10-Bit Address Format Select - The 7-bit address format is selected.  */
        p_i2c->reg->SARU0 = 0x00;                         /* 7bit mode                                */

        /* ICSER  - I2C Bus Status Enable Register
           b7   HOAE - Host Address Enable - Host address detection is disabled.
           b6   Reserved - This bit is read as 0. The write value should be 0.
           b5   DIDE - Device-ID Address Detection Enable - Device-ID address detection is disable.
           b4   Reserved - This bit is read as 0. The write value should be 0.
           b3   GCAE - General Call Address Enable - General call address detection is disabled.
           b2   SAR2E - Slave Address Register 2 Enable - Slave address in SARL2 and SARU2 is disabled.
           b1   SAR1E - Slave Address Register 1 Enable - Slave address in SARL1 and SARU1 is disabled.
           b0   SAR0E - Slave Address Register 0 Enable - Slave address in SARL0 and SARU0 is disabled.*/
        p_i2c->reg->ICSER = 0x00;
        uctmp = (uint8_t) ((uint8_t) (p_i2c->reg->ICMR1) & (0x8F)); // @suppress("Cast comment")
        
        /* ICMR1  - I2C Bus Mode Register 1
           b6:b4   CKS[2:0] - Internal Reference Clock Select. */
        p_i2c->reg->ICMR1 = (uint8_t) ((uctmp) | reg_set.cks);

        /* ICBRL  - I2C Bus Bit Rate Low-Level Register
           b4:b0   BRL[4:0] - Bit Rate Low-Level Period - Low-level period of SCL clock.
           b7:b5   Reserved - These bits are read as 1. The write value should be 1. */
        p_i2c->reg->ICBRL = reg_set.icbrl;

        /* ICBRH  - I2C Bus Bit Rate High-Level Register
           b4:b0   BRH[4:0] - Bit Rate High-Level Period - High-level period of SCL clock.
           b7:b5   Reserved - These bits are read as 1. The write value should be 1. */
        p_i2c->reg->ICBRH = reg_set.icbrh;

        /* ICMR2  - I2C Bus Mode Register 2
           b7    DLCS - SDA Output Delay Counter - The internal reference clock (fIIC) is selected 
                                                   as the clock source of the SDA output delay counter.
           b6:b4 SDDL[2:0] - SDA Output Delay Counter - No output delay
           b3    Reserved - This bit is read as 0. The write value should be 0.
           b2    TMOH - Timeout H Count Control - Count is enabled while the SCLn line is at a high level.
           b1    TMOL - Timeout L Count Control - Count is enabled while the SCLn line is at a low level.
           b0    TMOS - Timeout Detection Time Select - Long mode is selected.  */
        p_i2c->reg->ICMR2 = 0x06;

        /* ICMR3  - I2C Bus Mode Register 3
           b7    SMBS - SMBus/I2C Bus Select - The I2C bus is selected.
           b6    WAIT - WAIT - No WAIT
           b5    RDRFS - RDRF Flag Set Timing Select - The RDRF flag is set at the rising edge of 
                                                       the ninth SCL clock cycle.
           b4    ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled.
           b2    ACKBT - Transmit Acknowledge - A 0 is sent as the acknowledge bit (ACK transmission).
           b2    ACKBR - Receive Acknowledge - 
           b1:b0 NF[1:0] - Noise Filter Stage Select      */
        p_i2c->reg->ICMR3 = RIIC_ICMR3_DEF;

        /* ICFER  - I2C Bus Function Enable Register
           b7 FMPE - Fast-Mode Plus Enable - No Fm+ slope control circuit is used for the SCLn pin and SDAn pin.
           b6 SCLE - SCL Synchronous Circuit Enable - An SCL synchronous circuit is used.
           b5 NFE  - Digital Noise Filter Circuit Enable
           b4 NACKE - NACK Reception Transfer Suspension Enable - Transfer operation is suspended during NACK reception
           b3 SALE - Slave Arbitration-Lost Detection Enable - Slave arbitration-lost detection is enabled.
           b2 NALE - NACK Transmission Arbitration-Lost Detection Enable - NACK transmission arbitration-lost detection
                                                                           is disabled.
           b1 MALE - Master Arbitration-Lost Detection Enable - Master arbitration-lost detection is enabled.
           b0 TMOE - Timeout Function Enable - The timeout function is disabled.       */
        p_i2c->reg->ICFER = RIIC_ICFER_DEF;

        /* ICIER  - I2C Bus Interrupt Enable Register
           b7   TIE - Transmit Data Empty Interrupt Request Enable - enabled.
           b6   TEIE - Transmit End Interrupt Request Enable - disabled.
           b5   RIE - Receive Data Full Interrupt Request Enable - enabled.
           b4   NAKIE - NACK Reception Interrupt Request Enable - enabled.
           b3   SPIE - Stop Condition Detection Interrupt Request Enable - disabled.
           b2   STIE - Start Condition Detection Interrupt Request Enable - disabled.
           b1   ALIE - Arbitration-Lost Interrupt Request Enable - disabled.
           b0   TMOIE - Timeout Interrupt Request Enable - disabled.  */
        p_i2c->reg->ICIER = 0xB2;

        /* IICRST - I2C Bus Interface Internal Reset - Releases the RIIC reset or internal reset. */
        p_i2c->reg->ICCR1_b.IICRST = 0;
        p_i2c->info->bps  = bps;
    }
    return (result);
}/* End of function riic_setting() */

/****************************************************************************************************************//**
 * @brief        Reset to I2C register.
 *
 *               This function reset to I2C register.
 *               Back up the bit rate setting and write back after I2C reset.
 * @param[in]    p_i2c      Pointer to I2c resources
 * @param[in]    en_recv    RECV state (enable / disable)
**********************************************************************************************************************/
/* Function Name : riic_reset */
static void riic_reset(st_i2c_resources_t *p_i2c, bool en_recv) // @suppress("Function length")
{
    st_i2c_reg_buf_t reg_buckup;
    
    /* After I2C reset, the registers are initialized, so take a backup and write back */
    reg_buckup.cks   = p_i2c->reg->ICMR1;
    reg_buckup.icbrl = p_i2c->reg->ICBRL;
    reg_buckup.icbrh = p_i2c->reg->ICBRH;
    
    /* ICE - I2C Bus Interface Enable - Disable. */
    p_i2c->reg->ICCR1_b.ICE    = 0;

    /* IICRST - I2C Bus Interface Internal Reset - Initiates the RIIC reset or internal reset. */
    p_i2c->reg->ICCR1_b.IICRST = 1;

    /* ICE - I2C Bus Interface Enable - Enable. */
    p_i2c->reg->ICCR1_b.ICE    = 1;

    /* SARL0  - Slave Address Register L0 
       b7:b1  SVA[6:0] - 7-Bit Address/10-Bit Address Lower Bits
       b0     SVA0 - 10-Bit Address LSB  */
    p_i2c->reg->SARL0 = (uint8_t)(((p_i2c->info->own_sla)<<1)&0xFF);

    /* SARU0  - Slave Address Register U0 
       b7:b3  Reserved - These bits are read as 0. The write value should be 0.
       b2:b1  SVA[1:0] - 10-Bit Address Upper Bits
       b0     FS - 7-Bit/10-Bit Address Format Select - The 7-bit address format is selected.  */
    p_i2c->reg->SARU0 = 0x00;
    if (RIIC_RECV_ENABLE == en_recv)
    {
        if (0 == (p_i2c->info->own_sla & ARM_I2C_ADDRESS_GC))
        {
            /* ICSER  - I2C Bus Status Enable Register
               b7   HOAE - Host Address Enable - Host address detection is disabled.
               b6   Reserved - This bit is read as 0. The write value should be 0.
               b5   DIDE - Device-ID Address Detection Enable - Device-ID address detection is disable.
               b4   Reserved - This bit is read as 0. The write value should be 0.
               b3   GCAE - General Call Address Enable - General call address detection is disabled.
               b2   SAR2E - Slave Address Register 2 Enable - Slave address in SARL2 and SARU2 is disabled.
               b1   SAR1E - Slave Address Register 1 Enable - Slave address in SARL1 and SARU1 is disabled.
               b0   SAR0E - Slave Address Register 0 Enable - Slave address in SARL0 and SARU0 is enabled.*/
            p_i2c->reg->ICSER = 0x01;
        }
        else
        {
            /* ICSER  - I2C Bus Status Enable Register
               b7   HOAE - Host Address Enable - Host address detection is disabled.
               b6   Reserved - This bit is read as 0. The write value should be 0.
               b5   DIDE - Device-ID Address Detection Enable - Device-ID address detection is disable.
               b4   Reserved - This bit is read as 0. The write value should be 0.
               b3   GCAE - General Call Address Enable - General call address detection is enabled.
               b2   SAR2E - Slave Address Register 2 Enable - Slave address in SARL2 and SARU2 is disabled.
               b1   SAR1E - Slave Address Register 1 Enable - Slave address in SARL1 and SARU1 is disabled.
               b0   SAR0E - Slave Address Register 0 Enable - Slave address in SARL0 and SARU0 is enabled.*/
            p_i2c->reg->ICSER = 0x09;
        }
    }
    else
    {
        /* ICSER  - I2C Bus Status Enable Register
           b7   HOAE - Host Address Enable - Host address detection is disabled.
           b6   Reserved - This bit is read as 0. The write value should be 0.
           b5   DIDE - Device-ID Address Detection Enable - Device-ID address detection is disable.
           b4   Reserved - This bit is read as 0. The write value should be 0.
           b3   GCAE - General Call Address Enable - General call address detection is disabled.
           b2   SAR2E - Slave Address Register 2 Enable - Slave address in SARL2 and SARU2 is disabled.
           b1   SAR1E - Slave Address Register 1 Enable - Slave address in SARL1 and SARU1 is disabled.
           b0   SAR0E - Slave Address Register 0 Enable - Slave address in SARL0 and SARU0 is disabled.*/
        p_i2c->reg->ICSER = 0x00;
    }

    /* ICMR1  - I2C Bus Mode Register 1
       b6:b4   CKS[2:0] - Internal Reference Clock Select. */
    p_i2c->reg->ICMR1 = reg_buckup.cks;               /* Set ICMR1                                */

    /* ICBRL  - I2C Bus Bit Rate Low-Level Register
       b4:b0   BRL[4:0] - Bit Rate Low-Level Period - Low-level period of SCL clock.
       b7:b5   Reserved - These bits are read as 1. The write value should be 1. */
    p_i2c->reg->ICBRL = reg_buckup.icbrl;             /* Set ICBRL                                */

    /* ICBRH  - I2C Bus Bit Rate High-Level Register
       b4:b0   BRH[4:0] - Bit Rate High-Level Period - High-level period of SCL clock.
       b7:b5   Reserved - These bits are read as 1. The write value should be 1. */
    p_i2c->reg->ICBRH = reg_buckup.icbrh;             /* Set ICBRH                                */

    /* ICMR2  - I2C Bus Mode Register 2
       b7    DLCS - SDA Output Delay Counter - The internal reference clock (fIIC) is selected 
                                               as the clock source of the SDA output delay counter.
       b6:b4 SDDL[2:0] - SDA Output Delay Counter - No output delay
       b3    Reserved - This bit is read as 0. The write value should be 0.
       b2    TMOH - Timeout H Count Control - Count is enabled while the SCLn line is at a high level.
       b1    TMOL - Timeout L Count Control - Count is enabled while the SCLn line is at a low level.
       b0    TMOS - Timeout Detection Time Select - Long mode is selected.  */
    p_i2c->reg->ICMR2 = 0x06;

    /* ICMR3  - I2C Bus Mode Register 3
       b7    SMBS - SMBus/I2C Bus Select - The I2C bus is selected.
       b6    WAIT - WAIT - No WAIT
       b5    RDRFS - RDRF Flag Set Timing Select - The RDRF flag is set at the rising edge of 
                                                   the ninth SCL clock cycle.
       b4    ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled.
       b2    ACKBT - Transmit Acknowledge - A 0 is sent as the acknowledge bit (ACK transmission).
       b2    ACKBR - Receive Acknowledge - 
       b1:b0 NF[1:0] - Noise Filter Stage Select      */
    p_i2c->reg->ICMR3 = RIIC_ICMR3_DEF;

    /* ICFER  - I2C Bus Function Enable Register
       b7 FMPE - Fast-Mode Plus Enable - No Fm+ slope control circuit is used for the SCLn pin and SDAn pin.
       b6 SCLE - SCL Synchronous Circuit Enable - An SCL synchronous circuit is used.
       b5 NFE  - Digital Noise Filter Circuit Enable
       b4 NACKE - NACK Reception Transfer Suspension Enable - Transfer operation is suspended during NACK reception
       b3 SALE - Slave Arbitration-Lost Detection Enable - Slave arbitration-lost detection is enabled.
       b2 NALE - NACK Transmission Arbitration-Lost Detection Enable - NACK transmission arbitration-lost detection
                                                                       is disabled.
       b1 MALE - Master Arbitration-Lost Detection Enable - Master arbitration-lost detection is enabled.
       b0 TMOE - Timeout Function Enable - The timeout function is disabled.       */
    p_i2c->reg->ICFER = RIIC_ICFER_DEF;

    /* ICIER  - I2C Bus Interrupt Enable Register
       b7   TIE - Transmit Data Empty Interrupt Request Enable - enabled.
       b6   TEIE - Transmit End Interrupt Request Enable - disabled.
       b5   RIE - Receive Data Full Interrupt Request Enable - enabled.
       b4   NAKIE - NACK Reception Interrupt Request Enable - enabled.
       b3   SPIE - Stop Condition Detection Interrupt Request Enable - disabled.
       b2   STIE - Start Condition Detection Interrupt Request Enable - disabled.
       b1   ALIE - Arbitration-Lost Interrupt Request Enable - disabled.
       b0   TMOIE - Timeout Interrupt Request Enable - disabled.  */
    p_i2c->reg->ICIER = 0xB2;

    /* IICRST - I2C Bus Interface Internal Reset - Releases the RIIC reset or internal reset. */
    p_i2c->reg->ICCR1_b.IICRST = 0;
    
    return;
}/* End of function riic_reset() */

/************************************************************************/
/* Interrupt handler                                                    */
/************************************************************************/
/****************************************************************************************************************//**
* @brief      Interrupt handler for IIC Master/Slave transmission.
*
*             TXI interrupt processing.
* @param[in]  p_i2c    Pointer to I2C resources
**********************************************************************************************************************/
/* Function Name : iic_txi_interrupt */
static void iic_txi_interrupt(st_i2c_resources_t *p_i2c) // @suppress("ISR declaration") @suppress("Function length")
{
    uint32_t event  = 0;
    uint8_t timeout;
    
    if (RIIC_MODE_SLAVE == p_i2c->info->status.mode)
    {
        if (RIIC_STATE_START == p_i2c->info->state)
        {
            /* Slave and first transmission */
            event |= ARM_I2C_EVENT_SLAVE_TRANSMIT;

            /* STOP - Stop Condition Detection Flag - Stop condition is not detected.   */
            p_i2c->reg->ICSR2_b.STOP = 0;

            /* SPIE - Stop Condition Detection Interrupt Request Enable - enabled.   */
            p_i2c->reg->ICIER_b.SPIE = 1;
        }
        if (0x00 != p_i2c->reg->ICSER)
        {
            /* ICSER  - I2C Bus Status Enable Register
               b7   HOAE - Host Address Enable - Host address detection is disabled.
               b6   Reserved - This bit is read as 0. The write value should be 0.
               b5   DIDE - Device-ID Address Detection Enable - Device-ID address detection is disable.
               b4   Reserved - This bit is read as 0. The write value should be 0.
               b3   GCAE - General Call Address Enable - General call address detection is disabled.
               b2   SAR2E - Slave Address Register 2 Enable - Slave address in SARL2 and SARU2 is disabled.
               b1   SAR1E - Slave Address Register 1 Enable - Slave address in SARL1 and SARU1 is disabled.
               b0   SAR0E - Slave Address Register 0 Enable - Slave address in SARL0 and SARU0 is disabled.*/
            p_i2c->reg->ICSER = 0x00;
        }
        if (RIIC_TRANSMITTER == p_i2c->info->status.direction)
        {
            /* Slave transmission has no address output,                        */
            /* it changes to the transmission state for data transmission first */
            p_i2c->info->state = RIIC_STATE_SEND;
        }
        else
        {
            /* If a transmission request is received while waiting for slave reception, wait for stop condition */
            /* SALE - Slave Arbitration-Lost Detection Enable - Slave arbitration-lost detection is enabled. */
            p_i2c->reg->ICFER_b.SALE = 0;
            p_i2c->info->state = RIIC_STATE_STOP;
        }
    }

    switch (p_i2c->info->state)
    {
        case RIIC_STATE_START:
        {
            /* STOP - Stop Condition Detection Flag - Stop condition is not detected.   */
            p_i2c->reg->ICSR2_b.STOP = 0;

            /* SPIE - Stop Condition Detection Interrupt Request Enable - enabled.   */
            p_i2c->reg->ICIER_b.SPIE = 1;

            /* Address output */
            if (RIIC_ADDR_NONE != p_i2c->xfer_info->sla)
            {
                /* ICDRT - I2C Bus Transmit Data Register. */
                p_i2c->reg->ICDRT  = (uint8_t)p_i2c->xfer_info->sla;
                p_i2c->info->state = RIIC_STATE_ADDR;
            }
            else
            {
                timeout = 20;                        /* Timeout: 20us */
                while (1 == p_i2c->reg->ICCR1_b.SCLI)
                {
                    /* Wait until SCL is L level */
                    if ( 0 == (timeout --) )
                    {
                        /* timeout */
                        break;
                    }
                    R_SYS_SoftwareDelay(1, SYSTEM_DELAY_UNITS_MICROSECONDS);     /* 1us wait */
                }

                /* SP - Stop Condition Issuance Request - Requests to issue a stop condition. */
                p_i2c->reg->ICCR2_b.SP     = 1;
                p_i2c->info->state         = RIIC_STATE_STOP;
                p_i2c->xfer_info->f_tx_end = true;
            }
        }
        break;
        case RIIC_STATE_ADDR:
        {
            if (RIIC_RECIVER == p_i2c->info->status.direction)
            {
                /* In the case of reception mode, exit without doing anything (data part is not written) */
                break;
            }
            if (0 != p_i2c->xfer_info->cnt)
            {
                /* When the data count is 0, it is determined that the output has not been completed yet, 
                   and it remains as RIIC_STATE_ADDR */
                p_i2c->info->state = RIIC_STATE_SEND;
            }
        }
        /* Perform the following processing @suppress("Block comment") */
        case RIIC_STATE_SEND:
        {
            if (p_i2c->xfer_info->cnt < p_i2c->xfer_info->tx_num)
            {
                /*  */
                p_i2c->reg->ICDRT = p_i2c->xfer_info->tx_buf[p_i2c->xfer_info->cnt];
                p_i2c->xfer_info->cnt++;
            }
            else
            {
                /* TEIE - Transmit End Interrupt Request Enable - enabled. */
                p_i2c->reg->ICIER_b.TEIE = 1;

                /* TIE - Transmit Data Empty Interrupt Request Enable - disabled. */
                p_i2c->reg->ICIER_b.TIE = 0;
            }
        }
        break;

        case RIIC_STATE_STOP:
        {
            /* ICDRT - I2C Bus Transmit Data Register. */
            p_i2c->reg->ICDRT = 0xFF;
        }
        break;

        case RIIC_STATE_RECV:
        {
            __NOP();
        }
        break;

        default:
        {
            event |= ARM_I2C_EVENT_UND_ERROR;
            p_i2c->info->state = RIIC_STATE_STOP;

            /* ICDRT - I2C Bus Transmit Data Register. */
            p_i2c->reg->ICDRT  = 0xFF;
        }
        break;
    }
    
    /* Callback function */
    if ((NULL != p_i2c->info->cb_event) && (0 != event))
    {
        p_i2c->info->cb_event(event);
    }
    
}/* End of function iic_txi_interrupt() */

/****************************************************************************************************************//**
* @brief      Interrupt handler for IIC Master/Slave transmission completion.
*
*             TEI interrupt processing.
* @param[in]  p_i2c    Pointer to I2C resources
**********************************************************************************************************************/
/* Function Name : iic_tei_interrupt */
static void iic_tei_interrupt(st_i2c_resources_t *p_i2c) // @suppress("ISR declaration")
{
    uint32_t event       = 0;
    volatile uint8_t dummy;
    
    if (RIIC_MODE_MASTER == p_i2c->info->status.mode)
    {
        /* TEIE - Transmit End Interrupt Request Enable - disabled. */
        p_i2c->reg->ICIER_b.TEIE = 0;
        dummy = p_i2c->reg->ICIER;                                /* Wait two clock cycles of the target module clock */
        R_SYS_IrqStatusClear(p_i2c->tei_irq);
        R_NVIC_ClearPendingIRQ(p_i2c->tei_irq);

        /************************************************************************************************/
        /* When the interrupt priority order is EEI <TEI, when NACK occurs at the time of final data    */
        /* transmission, a TEI interrupt occurs first.                                                  */
        /* Since NACK judgment and stop condition output are performed by EEI interruption processing   */
        /* inherently, in case of NACK detection, do not process anything and exit.                     */
        /************************************************************************************************/
        if (1 != p_i2c->reg->ICSR2_b.NACKF)
        {
            if (true == p_i2c->info->pending)
            {
                /* Without stop bit output */
                p_i2c->info->status.busy  = 0;
                event |= ARM_I2C_EVENT_TRANSFER_DONE;
                iic_interrupt_clear(p_i2c);

                /* Callback function */
                if ((NULL != p_i2c->info->cb_event) && (0 != event))
                {
                    p_i2c->info->cb_event(event);
                }
            }
            else
            {
                /* SP - Stop Condition Issuance Request - Requests to issue a stop condition. */
                p_i2c->reg->ICCR2_b.SP     = 1;
                p_i2c->info->state         = RIIC_STATE_STOP;
                p_i2c->xfer_info->f_tx_end = true;
            }
        }
    }
    else
    {
        /* Dummy Write(SCL release) */
        p_i2c->reg->ICDRT = 0xFF;
        dummy = p_i2c->reg->ICIER;
        R_SYS_IrqStatusClear(p_i2c->tei_irq);
        R_NVIC_ClearPendingIRQ(p_i2c->tei_irq);
        p_i2c->info->state = RIIC_STATE_STOP;
        p_i2c->xfer_info->f_tx_end = true;
    }
    
    
}/* End of function iic_tei_interrupt() */

/****************************************************************************************************************//**
* @brief      Interrupt handler for IIC Master/Slave reception data full.
*
*             Receive interrupt processing.
*             
* @param[in]  p_i2c    Pointer to I2C resources
**********************************************************************************************************************/
/* Function Name : iic_rxi_interrupt */
static void iic_rxi_interrupt(st_i2c_resources_t *p_i2c) // @suppress("Function length") // @suppress("ISR declaration")
{
    volatile uint8_t dummy;
    bool flg_end_receive = 0;
    uint32_t event       = 0;
    uint8_t icmr3_tmp;
    
    /* General call event */
    if (1 == p_i2c->reg->ICSR1_b.GCA)
    {
        /* GCAE - General Call Address Enable - General call address detection is disabled. */
        p_i2c->reg->ICSR1_b.GCA = 0;
        p_i2c->info->status.general_call = 1;
    }

    if (0x00 != p_i2c->reg->ICSER)
    {
        /* ICSER  - I2C Bus Status Enable Register
           b7   HOAE - Host Address Enable - Host address detection is disabled.
           b6   Reserved - This bit is read as 0. The write value should be 0.
           b5   DIDE - Device-ID Address Detection Enable - Device-ID address detection is disable.
           b4   Reserved - This bit is read as 0. The write value should be 0.
           b3   GCAE - General Call Address Enable - General call address detection is disabled.
           b2   SAR2E - Slave Address Register 2 Enable - Slave address in SARL2 and SARU2 is disabled.
           b1   SAR1E - Slave Address Register 1 Enable - Slave address in SARL1 and SARU1 is disabled.
           b0   SAR0E - Slave Address Register 0 Enable - Slave address in SARL0 and SARU0 is disabled.*/
        p_i2c->reg->ICSER = 0x00;
    }
    
    switch (p_i2c->info->state)
    {
        case RIIC_STATE_RECV:
        {
            if (RIIC_MODE_MASTER == p_i2c->info->status.mode)
            {
                /* The next next data is the last or the reception size is 2 bytes or less? */
                if ((p_i2c->xfer_info->cnt + 3) == p_i2c->xfer_info->rx_num)
                {
                    /* WAIT - WAIT - WAIT */
                    p_i2c->reg->ICMR3_b.WAIT = 1;
                }
        
                /* Next data = Final? */
                if ((p_i2c->xfer_info->cnt + 2) == p_i2c->xfer_info->rx_num)
                {
                    if (0 == p_i2c->reg->ICMR3_b.ACKWP)
                    {
                        /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is enabled. */
                        p_i2c->reg->ICMR3_b.ACKWP = 1;
                    }

                    /* ACKBT - Transmit Acknowledge - 1 sent as the acknowledge bit (NACK transmission). */
                    /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled. */
                    icmr3_tmp = p_i2c->reg->ICMR3;
                    icmr3_tmp &= 0xE7;
                    icmr3_tmp |= 0x08;
                    p_i2c->reg->ICMR3 = icmr3_tmp;

                }

                /* Final data reception complete */
                if ((p_i2c->xfer_info->cnt + 1) >= p_i2c->xfer_info->rx_num)
                {
                    /* SP - Stop Condition Issuance Request - Requests to issue a stop condition. */
                    p_i2c->reg->ICCR2_b.SP   = 1;
                    flg_end_receive        = 1;
                }
        
            }
            else
            {
                /* Final data reception complete */
                if ((p_i2c->xfer_info->cnt + 1) >= p_i2c->xfer_info->rx_num)
                {
                    if (0 == p_i2c->reg->ICMR3_b.ACKWP)
                    {
                        /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is enabled. */
                        p_i2c->reg->ICMR3_b.ACKWP = 1;
                    }

                    /* ACKBT - Transmit Acknowledge - 1 sent as the acknowledge bit (NACK transmission). */
                    /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled. */
                    icmr3_tmp = p_i2c->reg->ICMR3;
                    icmr3_tmp &= 0xE7;
                    icmr3_tmp |= 0x08;
                    p_i2c->reg->ICMR3 = icmr3_tmp;

                    p_i2c->info->state = RIIC_STATE_STOP;
                }
            }
            p_i2c->xfer_info->rx_buf[p_i2c->xfer_info->cnt] = p_i2c->reg->ICDRR;
            p_i2c->xfer_info->cnt++;
        }
        break;
        
        case RIIC_STATE_START:
        {
            if (RIIC_MODE_SLAVE == p_i2c->info->status.mode)
            {
                event |= ARM_I2C_EVENT_SLAVE_RECEIVE;

                /* STOP - Stop Condition Detection Flag - Stop condition is not detected.   */
                p_i2c->reg->ICSR2_b.STOP = 0;

                /* SPIE - Stop Condition Detection Interrupt Request Enable - enabled.   */
                p_i2c->reg->ICIER_b.SPIE = 1;
            }
            if (RIIC_RECIVER == p_i2c->info->status.direction)
            {
                p_i2c->info->state = RIIC_STATE_RECV;
            }
            else
            {
                /* When the slave transmission start function is executed, but a read signal arrives from the master, 
                   an error is returned on the assumption that a receive event has occurred in the event    */
                event |= ARM_I2C_EVENT_SLAVE_RECEIVE;
                if (0 == p_i2c->reg->ICMR3_b.ACKWP)
                {
                    /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is enabled. */
                    p_i2c->reg->ICMR3_b.ACKWP = 1;
                }

                /* ACKBT - Transmit Acknowledge - 1 sent as the acknowledge bit (NACK transmission). */
                /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled. */
                icmr3_tmp = p_i2c->reg->ICMR3;
                icmr3_tmp &= 0xE7;
                icmr3_tmp |= 0x08;
                p_i2c->reg->ICMR3 = icmr3_tmp;

                p_i2c->info->state = RIIC_STATE_STOP;
            }
            dummy = p_i2c->reg->ICDRR;
        }
        break;

        case RIIC_STATE_ADDR:
        {
            /* Receive size is 2 bytes or less? */
            if (2 >= p_i2c->xfer_info->rx_num)
            {
                /* WAIT - WAIT - WAIT */
                p_i2c->reg->ICMR3_b.WAIT = 1;
            }

            /* Receive size less than 1 byte? */
            if (1 >= p_i2c->xfer_info->rx_num)
            {
                if (0 == p_i2c->reg->ICMR3_b.ACKWP)
                {
                    /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is enabled. */
                    p_i2c->reg->ICMR3_b.ACKWP = 1;
                }

                /* ACKBT - Transmit Acknowledge - 1 sent as the acknowledge bit (NACK transmission). */
                /* ACKWP - ACKBT Write Protect - Modification of the ACKBT bit is disabled. */
                icmr3_tmp = p_i2c->reg->ICMR3;
                icmr3_tmp &= 0xE7;
                icmr3_tmp |= 0x08;
                p_i2c->reg->ICMR3 = icmr3_tmp;
            }

            p_i2c->info->state = RIIC_STATE_RECV;
            dummy = p_i2c->reg->ICDRR;
        }
        break;
        
        case RIIC_STATE_SEND:
        case RIIC_STATE_STOP:
        default:
        {
            p_i2c->info->state = RIIC_STATE_STOP;
            dummy = p_i2c->reg->ICDRR;
        }
        break;
    }
    
    if (1 == flg_end_receive)
    {
        p_i2c->info->state = RIIC_STATE_STOP;

        /* WAIT - WAIT - No WAIT */
        p_i2c->reg->ICMR3_b.WAIT = 0;
    }

    /* Callback function */
    if ((NULL != p_i2c->info->cb_event) && (0 != event))
    {
        p_i2c->info->cb_event(event);
    }
}/* End of function iic_rxi_interrupt() */

/****************************************************************************************************************//**
* @brief      Interrupt handler for IIC Master/Slave transmission event/err.
*
*             communication error / event occurred processing.
*             Set an interrupt cause event and call the call back function.
* @param[in]  p_i2c    Pointer to I2C resources
**********************************************************************************************************************/
/* Function Name : iic_eei_interrupt */
static void iic_eei_interrupt(st_i2c_resources_t *p_i2c) // @suppress("ISR declaration") @suppress("Function length")
{
    uint32_t event  = 0;
    volatile uint8_t dummy;
    
    /* Arbitration lost event? */
    if (1 == p_i2c->reg->ICSR2_b.AL)
    {
        /* AL - Arbitration-Lost Flag - Arbitration is not lost. */
        p_i2c->reg->ICSR2_b.AL = 0;

        /* Wait two clock cycles of the target module clock */
        dummy = p_i2c->reg->ICSR2;
        R_SYS_IrqStatusClear(p_i2c->eei_irq);
        R_NVIC_ClearPendingIRQ(p_i2c->eei_irq);

        /* When an arbitration error occurs, it ends without waiting for a stop condition. */
        /* Together with incomplete completion event generation/                           */
        event |= (ARM_I2C_EVENT_ARBITRATION_LOST | 
                  ARM_I2C_EVENT_TRANSFER_DONE    |
                  ARM_I2C_EVENT_TRANSFER_INCOMPLETE);
        p_i2c->info->status.busy             = 0;
        p_i2c->info->status.arbitration_lost = 1;
        p_i2c->info->status.mode             = RIIC_MODE_SLAVE;
        iic_interrupt_clear(p_i2c);
    }
    
    
    if ( (1 == p_i2c->reg->ICSR2_b.NACKF) && (1 == p_i2c->reg->ICIER_b.NAKIE))
    {
        if (RIIC_MODE_MASTER == p_i2c->info->status.mode)
        {
            /* Disable TXI and RXI interrupts when NACK is received */
            R_NVIC_DisableIRQ(p_i2c->txi_irq);
            R_NVIC_DisableIRQ(p_i2c->rxi_irq);
            R_NVIC_DisableIRQ(p_i2c->tei_irq);
            if (RIIC_RECIVER == p_i2c->info->status.direction)
            {
                /* In the case of master reception, only NACK has address NACK */
                p_i2c->info->nack = RIIC_NACK_ADDR;
                dummy = p_i2c->reg->ICDRR;                  /* ICDRR register dummy read */
            }
            else
            {
                if (RIIC_STATE_ADDR == p_i2c->info->state)
                {
                    /* In case of NACK output at address transmission */
                    p_i2c->info->nack = RIIC_NACK_ADDR;
                }
                else
                {
                    /* In the case of NACK output upon data transmission */
                    p_i2c->info->nack = RIIC_NACK_DATA;
                }
            }

            /* SP - Stop Condition Issuance Request - Requests to issue a stop condition. */
            p_i2c->reg->ICCR2_b.SP = 1;

            /* NAKIE - NACK Reception Interrupt Request Enable - disable. */
            p_i2c->reg->ICIER_b.NAKIE = 0;
        }
        else
        {
            R_NVIC_DisableIRQ(p_i2c->txi_irq);

            /* ICDRT - I2C Bus Transmit Data Register. */
            p_i2c->reg->ICDRT = 0xFF;
            dummy = p_i2c->reg->ICDRR;

            /* SALE - Slave Arbitration-Lost Detection Enable - Slave arbitration-lost detection is enabled. */
            p_i2c->reg->ICFER_b.SALE = 0;

            /* NACKF - NACK Detection Flag - NACK is not detected. */
            p_i2c->reg->ICSR2_b.NACKF = 0;
        }
        p_i2c->info->state = RIIC_STATE_STOP;
        dummy = p_i2c->reg->ICSR2;                      /* Wait two clock cycles of the target module clock */
        R_SYS_IrqStatusClear(p_i2c->eei_irq);
        R_NVIC_ClearPendingIRQ (p_i2c->eei_irq);

    }
    
    if (1 == p_i2c->reg->ICSR2_b.STOP)
    {
        /* NACKF - NACK Detection Flag - NACK is not detected. */
        p_i2c->reg->ICSR2_b.NACKF = 0;

        /* STOP - Stop Condition Detection Flag - Stop condition is not detected.   */
        p_i2c->reg->ICSR2_b.STOP  = 0;
        dummy = p_i2c->reg->ICSR2;                      /* Wait two clock cycles of the target module clock */
        R_SYS_IrqStatusClear(p_i2c->eei_irq);
        R_NVIC_ClearPendingIRQ (p_i2c->eei_irq);
        if (1 == p_i2c->info->status.busy)
        {
            p_i2c->info->status.busy  = 0;
            event |= ARM_I2C_EVENT_TRANSFER_DONE;
            if (RIIC_TRANSMITTER == p_i2c->info->status.direction)
            {
                if (true != p_i2c->xfer_info->f_tx_end)
                {
                    event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
                }
            }
            else
            {
                if (p_i2c->xfer_info->cnt < p_i2c->xfer_info->rx_num)
                {
                    event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
                }
            }
            
            switch (p_i2c->info->nack)
            {
                case RIIC_NACK_ADDR:
                {
                    /* In case of address NACK, address NACK occurrence event setting */
                    event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
                    event |= ARM_I2C_EVENT_ADDRESS_NACK;
                }
                break;

                case RIIC_NACK_DATA:
                {
                    /* In case of data NACK, address NACK occurrence event setting */
                    event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
                }
                break;
                
                default:
                {
                    __NOP();
                }
                break;
            }
            
            if (1 == p_i2c->info->status.general_call)
            {
                event |= ARM_I2C_EVENT_GENERAL_CALL;
            }
            iic_interrupt_clear(p_i2c);
        }
    }
    
    /* Callback function */
    if ((NULL != p_i2c->info->cb_event) && (0 != event))
    {
        p_i2c->info->cb_event(event);
    }
    
}/* End of function iic_eei_interrupt() */


/* End I2C Interface */
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver I2C channel 0. */
#if (0 != R_RIIC0_ENABLE)
ARM_DRIVER_I2C Driver_I2C0 DATA_LOCATION_PRV_DRIVER_I2C =
{
    ARM_I2C_GetVersion,
    ARM_I2C_GetCapabilities,
    RIIC0_Initialize,
    RIIC0_Uninitialize,
    RIIC0_PowerControl,
    RIIC0_MasterTransmit,
    RIIC0_MasterReceive,
    RIIC0_SlaveTransmit,
    RIIC0_SlaveReceive,
    RIIC0_GetDataCount,
    RIIC0_Control,
    RIIC0_GetStatus
};
#endif

/* End I2C Interface */
/** @brief The Driver Instance of ACCESS STRUCT for the CMSIS-Driver I2C channel 0. */
#if (0 != R_RIIC1_ENABLE)
ARM_DRIVER_I2C Driver_I2C1 DATA_LOCATION_PRV_DRIVER_I2C =
{
    ARM_I2C_GetVersion,
    ARM_I2C_GetCapabilities,
    RIIC1_Initialize,
    RIIC1_Uninitialize,
    RIIC1_PowerControl,
    RIIC1_MasterTransmit,
    RIIC1_MasterReceive,
    RIIC1_SlaveTransmit,
    RIIC1_SlaveReceive,
    RIIC1_GetDataCount,
    RIIC1_Control,
    RIIC1_GetStatus
};
#endif

/******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_iic)
 *********************************************************************************************************************/
/* End of file (r_i2c_cmsis.c) */
