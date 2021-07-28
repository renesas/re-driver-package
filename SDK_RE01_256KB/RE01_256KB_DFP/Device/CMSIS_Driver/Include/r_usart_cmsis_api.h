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
* File Name    : r_usart_cmsis_api.h
* Version      : 1.20
* Description  : CMSIS-Driver for USART
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
*         : 05.01.2021 1.20     Add sci_close_dma() function
**********************************************************************************************************************/

/******************************************************************************************************************//**
 @ingroup grp_cmsis_drv_imp
 @defgroup grp_cmsis_drv_impl_usart CMSIS-Driver USART Implementation

 @section sec_cmsis_driver_usart_summary CMSIS-Driver USART Summary
 This is the CMSIS-Driver USART implementation which drives the SCI peripheral on RE Family via the CMSIS-Driver USART
 interface.
 Use the instance @ref Driver_USART0 to acccess to SCI peripheral channel 0. Do not use ARM_USART_XXX APIs,
 or USART0_XXX functions directly. For instance, when accessing the USART channel 0, use the USART Driver Instance 
 Driver_USART0, which is an instance of 'Access Struct' @ref ARM_DRIVER_USART for USART channel 0.
 Following is a code example to open USART channel 0 and power up the UART peripheral.
 @code
 ...

 Driver_USART0.Initialize();                     // Initialize the USART channel 0

 Driver_USART0.PowerControl(ARM_POWER_FULL);     // Power up the USART peripheral
 ...
 @endcode

 @section CMSIS_Driver_USART_Specific_Error_Code CMSIS-Driver USART Specific Error Code
 See ARM_USART_ERROR_XXX in Driver_USART.h for the detail.

 @{
 *********************************************************************************************************************/
/***********************************************************
Includes <System Includes> , "Project Includes"
***********************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

#include "RE01_256KB.h"
#include "Pin.h"
#include "R_Driver_USART.h"
#include "r_usart_cfg.h"
#include "r_system_api.h"
#include "r_system_cfg.h"
#include "r_lpm_api.h"

#if (USART_PRV_USED_DMAC_DRV != 0)
#include "r_dmac_api.h"
#endif
#if (USART_PRV_USED_DTC_DRV != 0)
#include "r_dtc_api.h"
#endif
/******************************************************************************
 Macro definitions
 *****************************************************************************/
#ifndef R_USART_CMSIS_H
#define R_USART_CMSIS_H
#define R_SCI0_ENABLE              (1)            /*!< SCI0 Enable */
#define R_SCI1_ENABLE              (1)            /*!< SCI1 Enable */
#define R_SCI2_ENABLE              (1)            /*!< SCI2 Enable */
#define R_SCI3_ENABLE              (1)            /*!< SCI3 Enable */
#define R_SCI4_ENABLE              (1)            /*!< SCI4 Enable */
#define R_SCI5_ENABLE              (1)            /*!< SCI5 Enable */
#define R_SCI9_ENABLE              (1)            /*!< SCI9 Enable */

/**************************************************/
/* Local Definition                               */
/**************************************************/
/* USART data size */
#define USART_SIZE_7              (0)             /*!< 7bit data */
#define USART_SIZE_8              (1)             /*!< 8bit data */
#define USART_SIZE_9              (2)             /*!< 9bit data */

/* SCI SSR register receiver error masks */
#define USART_SSR_ORER_MASK         (0x20U)   /*!< Mask bit for the ORER bit (Overflow error) in the SCI SSR register */
#define USART_SSR_FER_MASK          (0x10U)   /*!< Mask bit for the FER bit (Framing error) in the SCI SSR register  */
#define USART_SSR_PER_MASK          (0x08U)   /*!< Mask bit for the PER bit (Parity error) in the SCI SSR register */
#define USART_RCVR_ERR_MASK         (USART_SSR_ORER_MASK | USART_SSR_FER_MASK | USART_SSR_PER_MASK)    /*!< Mask */
#define USART_SSR_CLR_MASK          (0xC0U)       /*!< SSR register cleare mask (11000000b) */

#define USART_DIVISORS_SET_BIT(mask, value, reg, bit) \
        (reg = ((value & (1 << mask)) == (1 << mask)) ? (reg | (1 << bit)) : (reg & (~(1 << bit))))    /*!< Macro function */ /* @suppress("Macro parameter")  @suppress("Macro expansion parameter") */

/* USART flags */
#define USART_FLAG_INITIALIZED      (1U << 0)    /*!< initialized */
#define USART_FLAG_TX_AVAILABLE     (1U << 1)    /*!< Transmit available */
#define USART_FLAG_RX_AVAILABLE     (1U << 2)    /*!< Receive available */
#define USART_FLAG_POWERED          (1U << 3)    /*!< powered */
#define USART_FLAG_CONFIGURED       (1U << 4)    /*!< configured */
#define USART_FLAG_TX_ENABLED       (1U << 5)    /*!< Transmit enable */
#define USART_FLAG_RX_ENABLED       (1U << 6)    /*!< Receive enable */

/* IESR Values by channel */
#define USART_TXI0_IESR_VAL         (0x00000010)
#define USART_RXI0_IESR_VAL         (0x00000010)
#define USART_ERI0_IESR_VAL         (0x00000010)

#define USART_TXI0_DMAC_SOURCE_ID   (0x76)
#define USART_RXI0_DMAC_SOURCE_ID   (0x75)
#define USART_TXI1_DMAC_SOURCE_ID   (0x7C)
#define USART_RXI1_DMAC_SOURCE_ID   (0x7B)
#define USART_TXI2_DMAC_SOURCE_ID   (0x81)
#define USART_RXI2_DMAC_SOURCE_ID   (0x80)
#define USART_TXI3_DMAC_SOURCE_ID   (0x86)
#define USART_RXI3_DMAC_SOURCE_ID   (0x85)
#define USART_TXI4_DMAC_SOURCE_ID   (0x8B)
#define USART_RXI4_DMAC_SOURCE_ID   (0x8A)
#define USART_TXI5_DMAC_SOURCE_ID   (0x90)
#define USART_RXI5_DMAC_SOURCE_ID   (0x8F)
#define USART_TXI9_DMAC_SOURCE_ID   (0x95)
#define USART_RXI9_DMAC_SOURCE_ID   (0x94)

#define USART_TXI1_IESR_VAL         (0x0000001A)
#define USART_RXI1_IESR_VAL         (0x0000001B)
#define USART_ERI1_IESR_VAL         (0x0000001A)

#define USART_TXI2_IESR_VAL         (0x0000001A)
#define USART_RXI2_IESR_VAL         (0x0000001A)
#define USART_ERI2_IESR_VAL         (0x00000019)

#define USART_TXI3_IESR_VAL         (0x0000001C)
#define USART_RXI3_IESR_VAL         (0x0000001C)
#define USART_ERI3_IESR_VAL         (0x0000001B)

#define USART_TXI4_IESR_VAL         (0x0000001B)
#define USART_RXI4_IESR_VAL         (0x0000001B)
#define USART_ERI4_IESR_VAL         (0x0000001A)

#define USART_TXI5_IESR_VAL         (0x0000001D)
#define USART_RXI5_IESR_VAL         (0x0000001D)
#define USART_ERI5_IESR_VAL         (0x0000001C)

#define USART_TXI9_IESR_VAL         (0x0000001C)
#define USART_RXI9_IESR_VAL         (0x0000001C)
#define USART_ERI9_IESR_VAL         (0x0000001C)

/******************************************************************************
 Global Typedef definitions
 *****************************************************************************/
 /** @brief Enum for CMSIS-Driver USART Flow Mode Status */
 typedef enum /* @suppress("Source line ordering") */
{
    USART_FLOW_CTS_DISABLE = 0,                      /*!< CTS disable */
    USART_FLOW_CTS_ENABLE  = 1,                      /*!< CTS enable */
    USART_FLOW_RTS_ENABLE  = 2                       /*!< RTS enable */
} e_usart_flow_t;

/** @brief Enum for CMSIS-Driver USART mode */
typedef enum
{
    USART_MODE_ASYNC = 0,                       /*!< UART (Asynchronous); arg = Baudrate */
    USART_MODE_SYNC  = 1,                       /*!< Synchronous ; arg = Baudrate(Master), external clock signal(Slave) */
    USART_MODE_SMC   = 2                        /*!< UART Smart Card; arg = Baudrate */
} e_usart_mode_t;

/** @brief Synchronous transmit / receive mode */
typedef enum
{
    USART_SYNC_TX_MODE       = 0,               /*!< Synchronous transmit mode */
    USART_SYNC_RX_MODE       = 1,               /*!< Synchronous receive mode */
    USART_SYNC_TX_RX_MODE    = 2                /*!< Synchronous transmit/receive mode */
} e_usart_sync_t;

 /** @brief Enum for CMSIS-Driver USART base clock */
 typedef enum
{
    USART_BASE_PCLKA = 0,                       /*!< Base clock is PCLKA */
    USART_BASE_PCLKB = 1,                       /*!< Base clock is PCLKB */
} e_usart_base_clk_t;

/** @brief Structure for CMSIS-Driver USART Data Reception Status */
typedef struct
{
    uint8_t         busy;                       /*!< Receiver busy flag */
    uint8_t         overflow;                   /*!< Receive data overflow detected */
    uint8_t         framing_error;              /*!< Receive data framing error detected */
    uint8_t         parity_error;               /*!< Receive data parity error detected */
} st_usart_rx_status_t;

/** @brief Structure for CMSIS-Driver USART Data Transmission Status */
typedef struct
{
    uint32_t            rx_num;                 /*!< Total number of data to be received */
    uint32_t            tx_num;                 /*!< Total number of data to be send */
    void                *rx_buf;                /*!< Pointer to in data buffer */
    void                *tx_buf;                /*!< Pointer to out data buffer */
    uint32_t            rx_cnt;                 /*!< Number of data received */
    uint32_t            tx_cnt;                 /*!< Number of data sent */
    uint16_t            tx_def_val;             /*!< Transmit default value */
    uint8_t             rx_dump_val;            /*!< Receive dump value */
    uint8_t             send_active;            /*!< Send active flag */
    e_usart_sync_t      sync_mode;              /*!< Synchronous mode */
} st_usart_transfer_info_t;

/** @brief Structure to save USART settings and statuses */
typedef struct
{
    ARM_USART_SignalEvent_t    cb_event;        /*!< Event callback */
    st_usart_rx_status_t       rx_status;       /*!< Receive Status */
    st_usart_transfer_info_t   tx_status;       /*!< Transfer Status */
    e_usart_mode_t             mode;            /*!< USART mode */
    uint8_t                    data_size;       /*!< Transmit size */
    e_usart_flow_t             flow_mode;       /*!< Flow mode */
    uint16_t                   flags;           /*!< USART Driver flags */
    uint32_t                   baudrate;        /*!< Baudrate */
} st_usart_info_t;

/** @brief Structure to save contexts for a USART channel */
typedef struct
{
    volatile SCI2_Type      *reg;               /*!< Base pointer to SCI registers */
    r_pinset_t              pin_set;            /*!< USART pin set function */
    r_pinclr_t              pin_clr;            /*!< USART pin clear function */
    st_usart_info_t         *info;              /*!< Usart infomation */
    uint16_t                *cts_port;          /*!< Pointer to CTS port for software polling */
    uint8_t                 cts_pin_no;         /*!< Pointer to CTS pin number for software polling */
    uint16_t                *rts_port;           /*!< Pointer to RTS port for software control */
    uint8_t                 rts_pin_no;         /*!< Pointer to RTS pin number for software polling */
    e_usart_base_clk_t      pclk;               /*!< Base clock */
    e_system_mcu_lock_t     lock_id;            /*!< Module lock id */
    e_lpm_mstp_t            mstp_id;            /*!< MSTP id */
    IRQn_Type               txi_irq;            /*!< TXI IRQ Number */
    IRQn_Type               rxi_irq;            /*!< RXI IRQ Number */
    IRQn_Type               eri_irq;            /*!< ERI IRQ Number */
    uint32_t                txi_iesr_val;
    uint32_t                rxi_iesr_val;
    uint32_t                eri_iesr_val;
    uint32_t                txi_priority;
    uint32_t                rxi_priority;
    uint32_t                eri_priority;
#if (USART_PRV_USED_DMAC_DTC_DRV != 0)
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
#if (USART_PRV_USED_DTC_DRV != 0)
    st_dma_transfer_data_t  *tx_dtc_info;
    st_dma_transfer_data_t  *rx_dtc_info;
#endif
    system_int_cb_t         txi_callback;
    system_int_cb_t         rxi_callback;
    system_int_cb_t         eri_callback;    
} const st_usart_resources_t;

/** @brief Structure to save register setting values specific for the SCI peripheral */
typedef struct
{
    uint8_t                smr;                /*!< Set value of SMR register */
    uint8_t                scr;                /*!< Set value of SCR register */
    uint8_t                brr;                /*!< Set value of BRR register */
    uint8_t                scmr;               /*!< Set value of SCMR register */
    uint8_t                semr;               /*!< Set value of SEMR register */
    uint8_t                spmr;               /*!< Set value of SPMR register */
    uint8_t                mddr;               /*!< Set value of MDDR register */
    e_usart_flow_t         flow_mode;          /*!< Set flow mode */
    uint8_t                data_size;          /*!< Set data size */
} st_sci_reg_set_t;

/** @brief Structure to save baudrate setting specific for the SCI peripheral */
typedef struct
{
    int64_t     divisor;                       /*!< clock divisor */
    uint16_t    reg_value;                     /*!< register setting value */
} st_baud_divisor_t;


/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/
#if (USART_CFG_SECTION_ARM_USART_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_GET_VERSION      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_GET_VERSION
#endif

#if (USART_CFG_SECTION_ARM_USART_GET_CAPABILITES == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_GET_CAPABILITES      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_GET_CAPABILITES
#endif


#if (USART_CFG_SECTION_ARM_USART_INITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_INITIALIZE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_INITIALIZE
#endif

#if (USART_CFG_SECTION_ARM_USART_UNINITIALIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_UNINITIALIZE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_UNINITIALIZE
#endif

#if (USART_CFG_SECTION_ARM_USART_POWER_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_POWER_CONTROL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_POWER_CONTROL
#endif

#if (USART_CFG_SECTION_ARM_USART_SEND == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_SEND      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_SEND
#endif

#if (USART_CFG_SECTION_ARM_USART_RECEIVE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_RECEIVE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_RECEIVE
#endif

#if (USART_CFG_SECTION_ARM_USART_TRANSFER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_TRANSFER      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_TRANSFER
#endif

#if (USART_CFG_SECTION_ARM_USART_GET_TX_COUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_GET_TX_COUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_GET_TX_COUNT
#endif

#if (USART_CFG_SECTION_ARM_USART_GET_RX_COUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_GET_RX_COUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_GET_RX_COUNT
#endif

#if (USART_CFG_SECTION_ARM_USART_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_CONTROL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_CONTROL
#endif

#if (USART_CFG_SECTION_ARM_USART_GET_STATUS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_GET_STATUS      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_GET_STATUS
#endif

#if (USART_CFG_SECTION_ARM_USART_GET_MODEM_STATUS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS
#endif

#if (USART_CFG_SECTION_ARM_USART_SET_MODEM_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL
#endif


ARM_DRIVER_VERSION ARM_USART_GetVersion(void) FUNC_LOCATION_ARM_USART_GET_VERSION;
ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void) FUNC_LOCATION_ARM_USART_GET_CAPABILITES;
int32_t ARM_USART_Initialize(ARM_USART_SignalEvent_t cb_event, st_usart_resources_t * const p_usart) 
                             FUNC_LOCATION_ARM_USART_INITIALIZE;
int32_t ARM_USART_Uninitialize(st_usart_resources_t * const p_usart) FUNC_LOCATION_ARM_USART_UNINITIALIZE;
int32_t ARM_USART_PowerControl(ARM_POWER_STATE state, st_usart_resources_t * const p_usart) 
                              FUNC_LOCATION_ARM_USART_POWER_CONTROL;
int32_t ARM_USART_Send(void const * const p_data, uint32_t num, st_usart_resources_t * const p_usart)
                              FUNC_LOCATION_ARM_USART_SEND;
int32_t ARM_USART_Receive(void * const p_data, uint32_t num, st_usart_resources_t * const p_usart)
                         FUNC_LOCATION_ARM_USART_RECEIVE;
int32_t ARM_USART_Transfer(void const * const p_data_out,
                           void * const p_data_in, uint32_t num, st_usart_resources_t * const p_usart)
                           FUNC_LOCATION_ARM_USART_TRANSFER;
uint32_t ARM_USART_GetTxCount(st_usart_resources_t const * const p_usart) FUNC_LOCATION_ARM_USART_GET_TX_COUNT;
uint32_t ARM_USART_GetRxCount(st_usart_resources_t const * const p_usart) FUNC_LOCATION_ARM_USART_GET_RX_COUNT;
int32_t ARM_USART_Control(uint32_t control, uint32_t arg, st_usart_resources_t const * const p_usart)
                          FUNC_LOCATION_ARM_USART_CONTROL;
ARM_USART_STATUS ARM_USART_GetStatus(st_usart_resources_t const * const p_usart)
                                     FUNC_LOCATION_ARM_USART_GET_STATUS;
int32_t ARM_USART_SetModemControl(ARM_USART_MODEM_CONTROL control, st_usart_resources_t const * const p_usart)
                                  FUNC_LOCATION_ARM_USART_SET_MODEM_CONTROL;
ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus(st_usart_resources_t const * const p_usart)
                                                FUNC_LOCATION_ARM_USART_GET_MODEM_STATUS;

#if (0 != USART_PRV_USED_DMAC_DRV)
extern DRIVER_DMA Driver_DMAC0;  // @suppress("Source line ordering")
extern DRIVER_DMA Driver_DMAC1; 
extern DRIVER_DMA Driver_DMAC2; 
extern DRIVER_DMA Driver_DMAC3; 
#endif
#if (0 != USART_PRV_USED_DTC_DRV)
extern DRIVER_DMA Driver_DTC; 
 #endif

/****************************************************************************************************************//**
 * @brief       Inline function to clear bits.
 * @param[in,out]   p_reg   Pointer to USART register setting buffer
 * @param[in]       bit     Position of the bit to be clear.
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void sci_bit_clear_8(uint8_t * const p_reg, uint8_t bit) // @suppress("Function description comment")
{
    (*p_reg) &= (~(1 << bit));
  
}/* End of function sci_bit_clear_8() */

/****************************************************************************************************************//**
 * @brief       Inline function to set bits
 * @param[in,out]   p_reg      Pointer to USART register setting buffer
 * @param[in]       bit        Position of the bit to be set.
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void sci_bit_set_8(uint8_t * const p_reg, uint8_t bit) // @suppress("Function description comment")
{
    (*p_reg) |= (1 << bit);
}/* End of function sci_bit_set_8() */

/****************************************************************************************************************//**
 * @brief       Check and clear interrupt request
 * @param[in]   irq_num      Interrupt vect number
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void sci_clear_irq_req(IRQn_Type irq_num) // @suppress("Function description comment")
{
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > irq_num) // @suppress("Cast comment")
    {
        R_SYS_IrqStatusClear(irq_num);
        R_NVIC_ClearPendingIRQ(irq_num);
    }
}/* End of function sci_clear_irq_req() */

/****************************************************************************************************************//**
 * @brief       Check and disable interrupt
 * @param[in]   irq_num      Interrupt vect number
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void sci_disable_irq(IRQn_Type irq_num) // @suppress("Function description comment")
{
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > irq_num) // @suppress("Cast comment")
    {
        R_NVIC_DisableIRQ(irq_num);
    }
}/* End of function sci_disable_irq() */

#if (0 != USART_PRV_USED_DMAC_DTC_DRV)
/****************************************************************************************************************//**
 * @param[in]   drv     Pointer to DMA Driver
 * @param[in]   irq     DMA factors to be closed
 * @brief       DMA driver closed
 *******************************************************************************************************************/
/* Function Name : sci_close_dma */
__STATIC_FORCEINLINE void sci_close_dma(DRIVER_DMA* drv, uint16_t irq)
{
    volatile uint64_t act_src = 0;
    if (NULL != drv) // @suppress("Cast comment")
    {
#if (0 != USART_PRV_USED_DTC_DRV)
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
}/* End of function sci_close_dma */
#endif

#ifdef  __cplusplus
}
#endif

#endif /* R_USART_CMSIS_H */
/******************************************************************************************************************//**
 * @} (end defgroup grp_cmsis_drv_impl_usart)
 *********************************************************************************************************************/
/* End of file (r_usart_cmsis_api.h) */

