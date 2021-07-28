/*********************************************************************************************************************
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
* File Name    : r_dmac_api.c
* Version      : 1.20
* Description  : HAL Driver for DMAC.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 05.01.2021 1.20     Add "R_DMAC_GetAvailabilitySrc" and "R_DMAC_Release"
**********************************************************************************************************************/

/******************************************************************************************************************//**
 * @addtogroup grp_device_hal_dmac
 * @{
 *********************************************************************************************************************/
/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
/* Configuration for this package. */
#include <stdint.h>
#include <stdbool.h>
#include <r_dmac_api.h>
#include "r_dmac_cfg.h"

/* Public interface header file for this package. */

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define DMAC_PRV_MIN_COUNT_VAL          (1)             /*!< DMAC transfer min count */
#define DMAC_PRV_MAX_16BITS_COUNT_VAL   (65536)         /*!< DMAC 16bit transfer max count */
#define DMAC_PRV_MAX_8BITS_COUNT_VAL    (256)           /*!< DMAC 8bit transfer max count */
#define DMAC_PRV_MAX_10BITS_COUNT_VAL   (1024)          /*!< DMAC 10bit transfer max count */
#define DMAC_PRV_EVENT_NUM_MIN          (0x01)          /*!< DMAC Event min number */
#define DMAC_PRV_EVENT_NUM_MAX          (0xAA)          /*!< DMAC Event max number */
#define DMAC_PRV_MAX_EXTRA_REP_AREA     (27)            /*!< DMAC Extra repeat area min size */
#define DMAC_PRV_DELSR_IR_POS           (1U << 16)      /*!< DMAC activation request status flag position */

#define DMAC_PRV_MIN_ADDR_OFFSET        (-16777216)     /*!< DMAC Offset register min valise(0xFF00 0000) */
#define DMAC_PRV_MAX_ADDR_OFFSET        (16777215)      /*!< DMAC Offset register max valise(0x00FF FFFF) */
#define DMAC_PRV_NOREF_OFFSET           (0x0F000000)    /*!< DMAC Offset register not refresh */
#define DMAC_PRV_FREERUN                (0xFFFFFFFF)    /*!< DMAC Freerun mode */
#define DMAC_PRV_DMTMD_BYTE             (0)             /*!< DMAC 1byte size valuse */
#define DMAC_PRV_DMTMD_WORD             (1)             /*!< DMAC 2byte size valuse */
#define DMAC_PRV_DMTMD_LONG             (2)             /*!< DMAC 4byte size valuse */

#if (DMAC_CFG_R_DMAC_OPEN              == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CLOSE             == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CREATE            == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CONTROL           == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_GET_STATE         == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CLEAR_STATE       == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_INTERRUPT_ENABLE  == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_INTERRUPT_DISABLE == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_GET_TRANSFER_BYTE == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_DMAC_INTERRUPT_HANDLER   == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DMAC_RESOURCES       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DMAC_RESOURCES
#endif

#if (DMAC_CFG_R_DMAC_OPEN                 == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CLOSE                == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CREATE               == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CONTROL              == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_GET_STATE            == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CLEAR_STATE          == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_INTERRUPT_ENABLE     == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_INTERRUPT_DISABLE    == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_GET_TRANSFER_BYTE    == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_GET_VERSION          == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_RELEASE              == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_GET_AVAILABILITY_SRC == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_DMAC       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_DMAC
#endif

#if (DMAC_CFG_R_DMAC_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_GET_VERSION       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_GET_VERSION
#endif

#if (DMAC_CFG_R_DMAC_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_OPEN       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_OPEN
#endif

#if (DMAC_CFG_R_DMAC_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_CLOSE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_CLOSE
#endif

#if (DMAC_CFG_R_DMAC_CREATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_CREATE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_CREATE
#endif

#if (DMAC_CFG_R_DMAC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_CONTROL       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_CONTROL
#endif

#if (DMAC_CFG_R_DMAC_INTERRUPT_ENABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_ENABLE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_ENABLE
#endif

#if (DMAC_CFG_R_DMAC_INTERRUPT_DISABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_DISABLE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_DISABLE
#endif

#if (DMAC_CFG_R_DMAC_GET_STATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_GET_STATE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_GET_STATE
#endif

#if (DMAC_CFG_R_DMAC_CLEAR_STATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_CLEAR_STATE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_CLEAR_STATE
#endif

#if (DMAC_CFG_R_DMAC_GET_TRANSFER_BYTE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_GET_TRANSFER_BYTE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_GET_TRANSFER_BYTE
#endif

#if (DMAC_CFG_R_DMAC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_DMAC_CMD_REFRESH     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_DMAC_CMD_REFRESH
#endif

#if (DMAC_CFG_R_DMAC_OPEN            == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_DMAC_ACTIVE_SET       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_DMAC_ACTIVE_SET
#endif

#if (DMAC_CFG_R_DMAC_CLOSE            == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_DMAC_ACTIVE_CLEAR       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_DMAC_ACTIVE_CLEAR
#endif

#if (DMAC_CFG_DMAC_INTERRUPT_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_DMAC_INTERRUPT_HANDLER     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_DMAC_INTERRUPT_HANDLER
#endif

#if (DMAC_CFG_R_DMAC_GET_AVAILABILITY_SRC == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_GET_AVAILABILITY_SRC       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_GET_AVAILABILITY_SRC
#endif

#if (DMAC_CFG_R_DMAC_RELEASE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DMAC_RELEASE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DMAC_RELEASE
#endif

/******************************************************************************
 Local Typedef definitions
 *****************************************************************************/
/** DMAC mode infomation */
typedef struct 
{           
    dma_cb_event_t        cb_event;                 /*!< Event callback */
    uint32_t              dmcount;                  /*!< DMAC transfer set count */
    uint8_t               flags;                    /*!< DMA driver flags */
} st_dmac_mode_info_t;

/** DMAC recource data */
typedef struct 
{            
    volatile DMAC0_Type     *reg;                   /*!< Base pointer to DMAC registers */
    volatile uint32_t       *delsr;                 /*!< Pointer to DELSR register */
    e_system_mcu_lock_t     lock_id;                /*!< Module lock id */
    e_lpm_mstp_t            mstp_id;                /*!< MSTP id */
    uint16_t                active_flag;            /*!< DMACn active flag */
    st_dmac_mode_info_t     *info;                  /*!< DMAC mode infomation */
    IRQn_Type               dmac_int_irq;           /*!< DMAC interrupt number */
    uint32_t                dmac_int_iesr_val;      /*!< IESR setting value */
    uint32_t                dmac_int_priority;      /*!< DMAC interrupt priority */
    system_int_cb_t         dmac_int_callback;      /*!< DMAC interrupt callback function */
} const st_dmac_resources_t;            

/******************************************************************************
 Private global variables and functions
 *****************************************************************************/

static e_dma_err_t R_DMAC0_Open(void) FUNC_LOCATION_PRV_R_DMAC_OPEN; 
static e_dma_err_t R_DMAC0_Close(void) FUNC_LOCATION_PRV_R_DMAC_CLOSE; 
static e_dma_err_t R_DMAC0_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg) 
                         FUNC_LOCATION_PRV_R_DMAC_CREATE; 
static e_dma_err_t R_DMAC0_Control(e_dma_cmd_t cmd, void const * const p_arg)
                         FUNC_LOCATION_PRV_R_DMAC_CONTROL; 
static e_dma_err_t R_DMAC0_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
                         FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_ENABLE;
static e_dma_err_t R_DMAC0_InterruptDisable(void) FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_DISABLE;
static e_dma_err_t R_DMAC0_GetState(st_dma_state_t * const state) FUNC_LOCATION_PRV_R_DMAC_GET_STATE;
static e_dma_err_t R_DMAC0_ClearState(uint32_t clr_state) FUNC_LOCATION_PRV_R_DMAC_CLEAR_STATE;
static e_dma_err_t R_DMAC0_GetTransferByte(int16_t const dmac_source, uint32_t *transfer_byte)
                         FUNC_LOCATION_PRV_R_DMAC_GET_TRANSFER_BYTE;
static e_dma_err_t R_DMAC0_GetAvailabilitySrc(uint64_t * const src_info)FUNC_LOCATION_PRV_R_DMAC_GET_AVAILABILITY_SRC;
static e_dma_err_t R_DMAC0_Release(int16_t const DMAC_source)FUNC_LOCATION_PRV_R_DMAC_RELEASE;

static void dmac0_interrupt_handler(void) FUNC_LOCATION_PRV_DMAC_INTERRUPT_HANDLER;

/* DMAC0 mode infomation */
static st_dmac_mode_info_t gs_damc0_info = {0};

uint16_t g_dma_active_flag = 0;               /*!< DMAC Active flag */
/* DMAC0 Resources */
static st_dmac_resources_t gs_dmac0_resources DATA_LOCATION_PRV_DMAC_RESOURCES =
{
    DMAC0,                                      // DMAC0 regster base address // @suppress("Cast comment")
    &(ICU->DELSR0),                             // Pointer to DELSR0 register // @suppress("Cast comment")
    SYSTEM_LOCK_DMAC0,                          // DMAC0 resource lock id
    LPM_MSTP_DTC_DMAC,                          // DMAC mstp id
    DMA_ACTIVE_DMAC0,                            // DMAC0 active flag
    &gs_damc0_info,                                // DMAC0 infomation
    SYSTEM_CFG_EVENT_NUMBER_DMAC0_INT,          // DMAC0 interrupt event number // @suppress("Cast comment")
    0x00000002,                                 // DMAC0 interrupt IELS set value
    DMAC0_INT_PRIORITY,                         // DMAC0 interrupt priority
    dmac0_interrupt_handler                     // DMAC0 interrupt callback handler
};

static e_dma_err_t R_DMAC1_Open(void) FUNC_LOCATION_PRV_R_DMAC_OPEN;
static e_dma_err_t R_DMAC1_Close(void) FUNC_LOCATION_PRV_R_DMAC_CLOSE;
static e_dma_err_t R_DMAC1_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
                         FUNC_LOCATION_PRV_R_DMAC_CREATE;
static e_dma_err_t R_DMAC1_Control(e_dma_cmd_t cmd, void const * const p_arg) FUNC_LOCATION_PRV_R_DMAC_CONTROL;
static e_dma_err_t R_DMAC1_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
                         FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_ENABLE;
static e_dma_err_t R_DMAC1_InterruptDisable(void) FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_DISABLE;
static e_dma_err_t R_DMAC1_GetState(st_dma_state_t * const state) FUNC_LOCATION_PRV_R_DMAC_GET_STATE;
static e_dma_err_t R_DMAC1_ClearState(uint32_t clr_state) FUNC_LOCATION_PRV_R_DMAC_CLEAR_STATE;
static e_dma_err_t R_DMAC1_GetTransferByte(int16_t const dmac_source, uint32_t *transfer_byte)
                         FUNC_LOCATION_PRV_R_DMAC_GET_TRANSFER_BYTE;
static e_dma_err_t R_DMAC1_GetAvailabilitySrc(uint64_t * const src_info)FUNC_LOCATION_PRV_R_DMAC_GET_AVAILABILITY_SRC;
static e_dma_err_t R_DMAC1_Release(int16_t const DMAC_source)FUNC_LOCATION_PRV_R_DMAC_RELEASE;

static void dmac1_interrupt_handler(void) FUNC_LOCATION_PRV_DMAC_INTERRUPT_HANDLER;

/* DMAC1 mode infomation */
static st_dmac_mode_info_t gs_damc1_info = {0};

/* DMAC1 Resources */
static st_dmac_resources_t gs_dmac1_resources DATA_LOCATION_PRV_DMAC_RESOURCES =
{
    (DMAC0_Type*)DMAC1,                         // DMAC1 regster base address // @suppress("Cast comment")
    &(ICU->DELSR1),                             // Pointer to DELSR1 register // @suppress("Cast comment")
    SYSTEM_LOCK_DMAC1,                          // DMAC1 resource lock id
    LPM_MSTP_DTC_DMAC,                          // DMAC mstp id
    DMA_ACTIVE_DMAC1,                            // DMAC1 active flag
    &gs_damc1_info,                                // DMAC1 infomation
    SYSTEM_CFG_EVENT_NUMBER_DMAC1_INT,          // DMAC1 interrupt event number // @suppress("Cast comment")
    0x00000002,                                 // DMAC1 interrupt IELS set value
    DMAC1_INT_PRIORITY,                         // DMAC1 interrupt priority
    dmac1_interrupt_handler                     // DMAC1 interrupt callback handler
};

static e_dma_err_t R_DMAC2_Open(void) FUNC_LOCATION_PRV_R_DMAC_OPEN;
static e_dma_err_t R_DMAC2_Close(void) FUNC_LOCATION_PRV_R_DMAC_CLOSE;
static e_dma_err_t R_DMAC2_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
                         FUNC_LOCATION_PRV_R_DMAC_CREATE;
static e_dma_err_t R_DMAC2_Control(e_dma_cmd_t cmd, void const * const p_arg) FUNC_LOCATION_PRV_R_DMAC_CONTROL;
static e_dma_err_t R_DMAC2_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
                         FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_ENABLE;
static e_dma_err_t R_DMAC2_InterruptDisable(void) FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_DISABLE;
static e_dma_err_t R_DMAC2_GetState(st_dma_state_t * const state) FUNC_LOCATION_PRV_R_DMAC_GET_STATE;
static e_dma_err_t R_DMAC2_ClearState(uint32_t clr_state) FUNC_LOCATION_PRV_R_DMAC_CLEAR_STATE;
static e_dma_err_t R_DMAC2_GetTransferByte(int16_t const dmac_source, uint32_t *transfer_byte)
                         FUNC_LOCATION_PRV_R_DMAC_GET_TRANSFER_BYTE;
static e_dma_err_t R_DMAC2_GetAvailabilitySrc(uint64_t * const src_info)FUNC_LOCATION_PRV_R_DMAC_GET_AVAILABILITY_SRC;
static e_dma_err_t R_DMAC2_Release(int16_t const DMAC_source)FUNC_LOCATION_PRV_R_DMAC_RELEASE;

static void dmac2_interrupt_handler(void) FUNC_LOCATION_PRV_DMAC_INTERRUPT_HANDLER;

/* DMAC2 mode infomation */
static st_dmac_mode_info_t gs_damc2_info = {0};

/* DMAC2 Resources */
static st_dmac_resources_t gs_dmac2_resources DATA_LOCATION_PRV_DMAC_RESOURCES =
{
    (DMAC0_Type*)DMAC2,                         // DMAC2 regster base address // @suppress("Cast comment")
    &(ICU->DELSR2),                             // Pointer to DELSR2 register // @suppress("Cast comment")
    SYSTEM_LOCK_DMAC2,                          // DMAC2 resource lock id
    LPM_MSTP_DTC_DMAC,                          // DMAC mstp id
    DMA_ACTIVE_DMAC2,                            // DMAC2 active flag
    &gs_damc2_info,                                // DMAC2 infomation
    SYSTEM_CFG_EVENT_NUMBER_DMAC2_INT,          // DMAC2 interrupt event number // @suppress("Cast comment")
    0x00000002,                                 // DMAC2 interrupt IELS set value
    DMAC2_INT_PRIORITY,                         // DMAC2 interrupt priority
    dmac2_interrupt_handler                     // DMAC2 interrupt callback handler
};


static e_dma_err_t R_DMAC3_Open(void) FUNC_LOCATION_PRV_R_DMAC_OPEN;
static e_dma_err_t R_DMAC3_Close(void) FUNC_LOCATION_PRV_R_DMAC_CLOSE;
static e_dma_err_t R_DMAC3_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
                         FUNC_LOCATION_PRV_R_DMAC_CREATE;
static e_dma_err_t R_DMAC3_Control(e_dma_cmd_t cmd, void const * const p_arg) FUNC_LOCATION_PRV_R_DMAC_CONTROL;
static e_dma_err_t R_DMAC3_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
                         FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_ENABLE;
static e_dma_err_t R_DMAC3_InterruptDisable(void) FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_DISABLE;
static e_dma_err_t R_DMAC3_GetState(st_dma_state_t * const state) FUNC_LOCATION_PRV_R_DMAC_GET_STATE;
static e_dma_err_t R_DMAC3_ClearState(uint32_t clr_state) FUNC_LOCATION_PRV_R_DMAC_CLEAR_STATE;
static e_dma_err_t R_DMAC3_GetTransferByte(int16_t const dmac_source, uint32_t *transfer_byte)
                         FUNC_LOCATION_PRV_R_DMAC_GET_TRANSFER_BYTE;
static e_dma_err_t R_DMAC3_GetAvailabilitySrc(uint64_t * const src_info)FUNC_LOCATION_PRV_R_DMAC_GET_AVAILABILITY_SRC;
static e_dma_err_t R_DMAC3_Release(int16_t const DMAC_source)FUNC_LOCATION_PRV_R_DMAC_RELEASE;

static void dmac3_interrupt_handler(void) FUNC_LOCATION_PRV_DMAC_INTERRUPT_HANDLER;


/* DMAC3 mode infomation */
static st_dmac_mode_info_t gs_damc3_info = {0};

/* DMAC3 Resources */
static st_dmac_resources_t gs_dmac3_resources DATA_LOCATION_PRV_DMAC_RESOURCES =
{
    (DMAC0_Type*)DMAC3,                         // DMAC3 regster base address // @suppress("Cast comment")
    &(ICU->DELSR3),                             // Pointer to DELSR3 register // @suppress("Cast comment")
    SYSTEM_LOCK_DMAC3,                          // DMAC3 resource lock id
    LPM_MSTP_DTC_DMAC,                          // DMAC mstp id
    DMA_ACTIVE_DMAC3,                            // DMAC3 active flag
    &gs_damc3_info,                                // DMAC3 infomation
    SYSTEM_CFG_EVENT_NUMBER_DMAC3_INT,          // DMAC3 interrupt event number // @suppress("Cast comment")
    0x00000002,                                 // DMAC3 interrupt IELS set value
    DMAC3_INT_PRIORITY,                         // DMAC3 interrupt priority
    dmac3_interrupt_handler                     // DMAC3 interrupt callback handler
};

static void dmac_active_set(uint16_t ch) FUNC_LOCATION_PRV_DMAC_ACTIVE_SET;
static void dmac_active_clear(uint16_t ch) FUNC_LOCATION_PRV_DMAC_ACTIVE_CLEAR;
static e_dma_err_t dmac_cmd_refresh(st_dma_refresh_data_t const * const p_data, st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_DMAC_CMD_REFRESH;
static uint32_t  R_DMAC_GetVersion(void) FUNC_LOCATION_PRV_R_GET_VERSION;
static e_dma_err_t R_DMAC_Open(st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_R_DMAC_OPEN;
static e_dma_err_t R_DMAC_Close(st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_R_DMAC_CLOSE;
static e_dma_err_t R_DMAC_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg,
                          st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_R_DMAC_CREATE;
static e_dma_err_t R_DMAC_Control(e_dma_cmd_t cmd, void const * const p_arg,
                                  st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_R_DMAC_CONTROL;
static e_dma_err_t R_DMAC_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback, 
                          st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_ENABLE;
static e_dma_err_t R_DMAC_InterruptDisable(st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_R_DMAC_INTERRUPT_DISABLE;
static e_dma_err_t R_DMAC_GetState(st_dma_state_t * const state, st_dmac_resources_t * const p_dmac) 
                          FUNC_LOCATION_PRV_R_DMAC_GET_STATE;
static e_dma_err_t R_DMAC_ClearState(uint32_t clr_state, st_dmac_resources_t * const p_dmac) 
                          FUNC_LOCATION_PRV_R_DMAC_CLEAR_STATE;
static e_dma_err_t  R_DMAC_GetTransferByte(uint32_t * transfer_byte, st_dmac_resources_t * const p_dmac)
                          FUNC_LOCATION_PRV_R_DMAC_GET_TRANSFER_BYTE;
static e_dma_err_t R_DMAC_GetAvailabilitySrc(uint64_t * const src_info, st_dmac_resources_t * const p_dmac)FUNC_LOCATION_PRV_R_DMAC_GET_AVAILABILITY_SRC;
static e_dma_err_t R_DMAC_Release(int16_t const dmac_source, st_dmac_resources_t * const p_dmac)FUNC_LOCATION_PRV_R_DMAC_RELEASE;

static void dmac_interrupt_handler(st_dmac_resources_t * const p_dmac) FUNC_LOCATION_PRV_DMAC_INTERRUPT_HANDLER;

/*****************************************************************************************************************//**
 * @brief       Get API verion
 * @retval      uint32_t  Version.
**********************************************************************************************************************/
/* Function Name : R_DMAC_GetVersion */
static uint32_t  R_DMAC_GetVersion(void) // @suppress("Source line ordering")
{
    uint32_t const version = ((DMAC_VERSION_MAJOR << 16) | DMAC_VERSION_MINOR);

    return (version);
}/* End of function R_DMAC_GetVersion() */


/*****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Open API for the DMAC peripheral on RE Family.
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK  Open successful.
 * @retval      DMA_ERROR  Open error.
**********************************************************************************************************************/
/* Function Name : R_DMAC_Open */
static e_dma_err_t R_DMAC_Open(st_dmac_resources_t * const p_dmac)
{
    
    if (DMA_FLAG_OPENED == p_dmac->info->flags)
    {
        return (DMA_ERROR);
    }

    /* Lock DMAC resource */
    if (0 != R_SYS_ResourceLock(p_dmac->lock_id))
    {
        return (DMA_ERROR_LOCKED);
    }
    
    /* Release module stop */
    if (0 != R_LPM_ModuleStart(p_dmac->mstp_id))
    {
        /* Release DMAC resource */
        R_SYS_ResourceUnlock(p_dmac->lock_id);
        return (DMA_ERROR);
    }

    /** Resource initialize */
    p_dmac->info->flags = DMA_FLAG_OPENED;            /* Driver Opened                  */
    p_dmac->info->cb_event = NULL;                    /* Callback function is NULL      */
    p_dmac->info->dmcount = 0;                        /* Normal mode transfer count info clear */
    dmac_active_set(p_dmac->active_flag);
    
    return (DMA_OK);
}/* End of function R_DMAC_Open() */


/****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Close API for the DMAC peripheral on RE Family.
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
**********************************************************************************************************************/
/* Function Name : R_DMAC_Close */
static e_dma_err_t R_DMAC_Close(st_dmac_resources_t * const p_dmac) // @suppress("Function length")
{
    
    if (0 == (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_OK);
    }
    
    /* DELSRn  - DMAC Event Link setting register n
       b31 : b17 -Reserved
       b16       - DMAC activation request status flag
       b15 : b8  - Reserved
       b7  : b0  - DMAC Event Link selection */
    *(p_dmac->delsr) = 0x0000;
    
    /* DMCNT  - DMA Transfer Enable Register
       b7 : b1 - Reserved
       b0      - DTE - DMA Transfer Enable - Disables DMA transfer. */
    p_dmac->reg->DMCNT = 0x00;

    /* DMTMD  - DMA Transfer Mode Register
       b15:b14 - DM[1:0] - Transfer Mode Select - Normal transfer
       b13:b12 - DTS[1:0] - Repeat Area Select - The destination is specified as the repeat area or block area.
       b11:b10 - Reserved
       b9 :b8  - SZ[1:0] - Transfer Data Size Select - 8 bits
       b7 :b2  - Reserved
       b1 :b0  - DCTG[1:0] - Transfer Request Source Select - Software */
    p_dmac->reg->DMTMD = 0x0000;

    /* DMSAR  - DMA Source Address Register
       b31:b0  - Specifies the transfer source start address. */
    p_dmac->reg->DMSAR = 0x00000000;

    /* DMDAR  - DMA Destination Address Register
       b31:b0  - Specifies the transfer destination start address. */
    p_dmac->reg->DMDAR = 0x00000000;
    
    /* DMCRA  - DMA Transfer Count Register
       b31:b26 - Reserved
       b25:b16 - DMCRAH - Upper bits of transfer count.
       b15:b0  - DMCRAL - Upper bits of transfer count. */
    p_dmac->reg->DMCRA = 0x00000000;
    
    /* DMDAR  - DMA Destination Address Register
       b15:b0  - Specifies the number of block transfer operations or repeat transfer operations. */
    p_dmac->reg->DMCRB = 0x0000;

    /* DMINT  - DMA Interrupt Setting Register
       b7 :b5  - Reserved
       b4      - DTIE  - Transfer End Interrupt Enable - Disable
       b3      - ESIE  - Transfer Escape End Interrupt Enable - Disable
       b2      - RPTIE - Repeat Size End Interrupt Enable - Disable
       b1      - SARIE - Source Address Extended Repeat Area Overflow Interrupt Enable - Disable
       b0      - DARIE - Destination Address Extended Repeat Area Overflow Interrupt Enable - Disable */
    p_dmac->reg->DMINT = 0x00;
    
    /* DMAMD  - DMA Address Mode Register
       b15:b14 - SM[1:0] - Source Address Update Mode - Source address is fixed.
       b13     - Reserved
       b12:b8  - SARA[4:0] - Source Address Extended Repeat Area
       b7 :b6  - DM[1:0]   - Destination Address Update Mode - Destination address is fixed.
       b5      - Reserved
       b4 :b0  - DARA[4:0] - Destination Address Extended Repeat Area */
    p_dmac->reg->DMAMD = 0x0000;

    /* DMOFR  - DMA Offset Register
       b31:b0  - Specifies the offset when offset addition is selected as the address update 
                 mode for transfer source or destination. */
    p_dmac->reg->DMOFR = 0x00000000;

    /* DMREQ  - DMA Software Start Register
       b7 :b5  - Reserved
       b4      - CLRS  - DMA Software Start Bit Auto Clear Select - SWREQ bit is cleared after 
                         DMA transfer is started by software.
       b3 :b1  - Reserved
       b0      - SWREQ - DMA Software Start - DMA transfer is not requested. */
    p_dmac->reg->DMREQ = 0x00;
    
    /* DMSTS  - DMA Status Register
       b7      - ACT   - DMA Active Flag
       b6 :b5  - Reserved
       b4      - DTIF  - Transfer End Interrupt Flag
       b3 :b1  - Reserved
       b0      - ESIF  - Transfer Escape End Interrupt Flag */
    p_dmac->reg->DMSTS = 0x00;
    
    dmac_active_clear(p_dmac->active_flag);

    /* Disable DMAC complete interrupt */
    R_NVIC_DisableIRQ(p_dmac->dmac_int_irq);
    R_SYS_IrqStatusClear(p_dmac->dmac_int_irq);
    R_NVIC_ClearPendingIRQ(p_dmac->dmac_int_irq);

    /* Release dmac resource */
    R_SYS_ResourceUnlock(p_dmac->lock_id);

    if (0 == (g_dma_active_flag & DMA_ACTIVE_MASK))
    {
        /* Setting module stop */
        R_LPM_ModuleStop(p_dmac->mstp_id);
    }
    
    /* DMAC infomation flag clear */
    p_dmac->info->flags = 0x00;

    return (DMA_OK);

}/* End of function R_DMAC_Close() */

/****************************************************************************************************************//**
 * @brief       Create DMAC transfer infomation.
 * @param[in]   dmac_source     DMAC transfer trigger source.
 * @param[in]   p_data_cfg      Pointer to DMAC transfer setting infomation.
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK          DMAC transfer infomation create successful.
 * @retval      DMA_ERROR       DMAC transfer infomation create error.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
**********************************************************************************************************************/
/* Function Name : R_DMAC_Create */
static e_dma_err_t R_DMAC_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg, // @suppress("Function length")
                          st_dmac_resources_t * const p_dmac)
{

#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
    /* DMAC open check */
    if (DMA_FLAG_OPENED != (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
    
    /* DMAC transfer trigger check */
    if (((DMAC_PRV_EVENT_NUM_MIN > dmac_source) || (DMAC_PRV_EVENT_NUM_MAX < dmac_source))
        &&(DMAC_TRANSFER_REQUEST_SOFTWARE != dmac_source) )
    {
        return (DMA_ERROR_PARAMETER);
    }

    /* If there is a value in the reserved bit area, an error is returned */
    if (0 != ((p_data_cfg->mode) & DMA_MODE_RASERVED))
    {
        return (DMA_ERROR_PARAMETER);
    }

    /* MODE check */
    switch ((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK)
    {
        case DMA_MODE_NORMAL:
        {
            /* In the case of normal transfer, the setting range of transfer count is 1 to 65535. */
            if (p_data_cfg->transfer_count > (DMAC_PRV_MAX_16BITS_COUNT_VAL-1))
            {
                return (DMA_ERROR_PARAMETER);
            }
            
            /* In case of normal transfer, no minimum value. */
            /* When 0 is set, the free running mode is set. */
        }
        break;
        
        case DMA_MODE_REPEAT:
        case DMA_MODE_BLOCK:
        {
            /* In the case of repeat/block transfer, the setting range of transfer count is 1 to 65536. */
            if ((p_data_cfg->transfer_count < DMAC_PRV_MIN_COUNT_VAL) ||
                (p_data_cfg->transfer_count > DMAC_PRV_MAX_16BITS_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }

            /* In the case of repeat/block transfer, the setting range of block size is 1 to 1024. */
            if ((p_data_cfg->block_size > DMAC_PRV_MAX_10BITS_COUNT_VAL) ||
                (p_data_cfg->block_size < DMAC_PRV_MIN_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        
        default:
        {
            /* Invalid mode */
            return (DMA_ERROR_MODE);
        }
        break;
    }

    
    /* Check data size setting. */
    switch ((p_data_cfg->mode) & DMA_SIZE_MASK)
    {
        case DMA_SIZE_BYTE:
        {
            /* No action to be performed */
        }
        break;

        case DMA_SIZE_WORD:
        {
            /* For word size, bit 0 of the transfer source / transfer destination address must be 0 */
            if ((0 != (p_data_cfg->src_addr & 0x00000001)) ||
                (0 != (p_data_cfg->dest_addr & 0x00000001)))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;

        case DMA_SIZE_LONG:
        {
            /* For long size, bit 1 to 0 of the transfer source / transfer destination address must be 00 */
            if ((0 != (p_data_cfg->src_addr & 0x00000003)) ||
                (0 != (p_data_cfg->dest_addr & 0x00000003)))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        
        default:
        {
            /* Invalid data size */
            return (DMA_ERROR_MODE);
        }
        break;
    }

    /* Check repeat block area */
    switch ((p_data_cfg->mode) & DMA_REPEAT_BLOCK_MASK)
    {
        case DMA_REPEAT_BLOCK_DEST:
        {
            /* When repeat area or block area is set as the transfer destination, */
            /* write 00000b to the DARA [4: 0] bits.                              */
            if ((DMA_MODE_NORMAL != ((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK)) &&
                (0 != p_data_cfg->dest_extended_repeat))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        case DMA_REPEAT_BLOCK_SRC:
        {
            /* When repeat area or block area is set as the transfer source,      */
            /* write 00000b to the SARA [4: 0] bits.                              */
            if ((DMA_MODE_NORMAL != ((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK)) &&
                (0 != p_data_cfg->src_extended_repeat))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        case DMA_REPEAT_BLOCK_NONE:
        {
            /* No action to be performed */
        }
        break;
        
        default:
        {
            /* Invalid repeat block selected */
            return (DMA_ERROR_MODE);
        }
        break;
    }

    /* Check chain transfer mode(DTC only) */
    if (DMA_CHAIN_DISABLE != ((p_data_cfg->mode) & DMA_CHAIN_MODE_MASK))
    {
        return (DMA_ERROR_MODE);
    }
    
    /* Maximum value check of transfer source extended repeat area */
    if (DMAC_PRV_MAX_EXTRA_REP_AREA < p_data_cfg->src_extended_repeat)
    {
        return (DMA_ERROR_PARAMETER);
    }

    /* Maximum value check of transfer destination extended repeat area */
    if (DMAC_PRV_MAX_EXTRA_REP_AREA < p_data_cfg->dest_extended_repeat)
    {
        return (DMA_ERROR_PARAMETER);
    }
    
    /* When offset addition is selected, it is checked whether the offset value is within the range */
    if ((DMA_SRC_ADDR_OFFSET == ((p_data_cfg->mode) & DMA_SRC_ADDRESSING_MASK)) || 
        (DMA_DEST_ADDR_OFFSET == ((p_data_cfg->mode) & DMA_DEST_ADDRESSING_MASK)))
    {
        if ((DMAC_PRV_MAX_ADDR_OFFSET < (p_data_cfg->offset)) ||
            (DMAC_PRV_MIN_ADDR_OFFSET > (p_data_cfg->offset)))
        {
            return (DMA_ERROR_PARAMETER);
        }
    }
    
    /* "source addressing", "destination addressing" are not checked    */
    /*  because there is no error pattern                               */
#endif /* (1 == DMAC_CFG_PARAM_CHECKING_ENABLE) */
    
    /* DELSRn  - DMAC Event Link setting register n
       b15 : b8 - Reserved
       b7  : b0 - DMAC Event Link selection */
    *(p_dmac->delsr) = 0x0000;

    /* DMCNT  - DMA Transfer Enable Register
       b7 : b1 - Reserved
       b0      - DTE - DMA Transfer Enable - Disables DMA transfer. */
    p_dmac->reg->DMCNT_b.DTE = 0;
    
    if (DMAC_TRANSFER_REQUEST_SOFTWARE != dmac_source)
    {
        *(p_dmac->delsr) = dmac_source;
    }

    /* DMAMD  - DMA Address Mode Register
       b15:b14 - SM[1:0] - Source Address Update Mode
       b13     - Reserved
       b12:b8  - SARA[4:0] - Source Address Extended Repeat Area
       b7 :b6  - DM[1:0]   - Destination Address Update Mode
       b5      - Reserved
       b4 :b0  - DARA[4:0] - Destination Address Extended Repeat Area */
    p_dmac->reg->DMAMD_b.SM = ((p_data_cfg->mode) & DMA_SRC_ADDRESSING_MASK)>>10;
    p_dmac->reg->DMAMD_b.DM = ((p_data_cfg->mode) & DMA_DEST_ADDRESSING_MASK)>>2;
    p_dmac->reg->DMAMD_b.SARA = p_data_cfg->src_extended_repeat;
    p_dmac->reg->DMAMD_b.DARA = p_data_cfg->dest_extended_repeat;
    
    
    /* DMTMD  - DMA Transfer Mode Register
       b15:b14 - DM[1:0] - Transfer Mode Select - Normal transfer
       b13:b12 - DTS[1:0] - Repeat Area Select - The destination is specified as the repeat area or block area.
       b11:b10 - Reserved
       b9 :b8  - SZ[1:0] - Transfer Data Size Select - 8 bits
       b7 :b2  - Reserved
       b1 :b0  - DCTG[1:0] - Transfer Request Source Select - Software */
    if (DMAC_TRANSFER_REQUEST_SOFTWARE == dmac_source)
    {
        p_dmac->reg->DMTMD_b.DCTG = 0;
    }
    else
    {
        p_dmac->reg->DMTMD_b.DCTG = 1;
    }
    p_dmac->reg->DMTMD_b.SZ  = ((p_data_cfg->mode) & DMA_SIZE_MASK) >> 12;
    p_dmac->reg->DMTMD_b.MD  = ((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK) >> 14;
    switch ((p_data_cfg->mode) & DMA_REPEAT_BLOCK_MASK)
    {
        case DMA_REPEAT_BLOCK_DEST:
        {
            p_dmac->reg->DMTMD_b.DTS = 0;
        }
        break;
        case DMA_REPEAT_BLOCK_SRC:
        {
            p_dmac->reg->DMTMD_b.DTS = 1;
        }
        break;
        case DMA_REPEAT_BLOCK_NONE:
        {
            p_dmac->reg->DMTMD_b.DTS = 2;
        }
        break;
        default:
        {
            /* No action to be performed */
        }
        break;
    }
    
    /* DMSAR  - DMA Source Address Register
       b31:b0  - Specifies the transfer source start address. */
    p_dmac->reg->DMSAR = p_data_cfg->src_addr;

    /* DMDAR  - DMA Destination Address Register
       b31:b0  - Specifies the transfer destination start address. */
    p_dmac->reg->DMDAR = p_data_cfg->dest_addr;
    
    /* DMCRA  - DMA Transfer Count Register
       b31:b26 - Reserved
       b25:b16 - DMCRAH - Upper bits of transfer count.
       b15:b0  - DMCRAL - Upper bits of transfer count. */
    switch ((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK)
    {
        case DMA_MODE_NORMAL:
        {
            p_dmac->reg->DMCRA_b.DMCRAL = p_data_cfg->transfer_count;
            p_dmac->reg->DMCRA_b.DMCRAH = 0;
            if (0 != p_data_cfg->transfer_count)
            {
                p_dmac->info->dmcount = p_data_cfg->transfer_count;
            }
            else
            {
                /* Set freerun mode */
                p_dmac->info->dmcount = DMAC_PRV_FREERUN;
            }
        }
        break;
        
        case DMA_MODE_REPEAT:
        case DMA_MODE_BLOCK:
        {
            p_dmac->reg->DMCRA_b.DMCRAL = (p_data_cfg->block_size & 0x3FF);
            p_dmac->reg->DMCRA_b.DMCRAH = (p_data_cfg->block_size & 0x3FF);
            p_dmac->reg->DMCRB = (p_data_cfg->transfer_count & 0xFFFF);
            p_dmac->info->dmcount = p_data_cfg->transfer_count;
        }
        break;
        
        default:
        {
            /* No action to be performed */
        }
        break;
    }

    /* DMOFR  - DMA Offset Register
       b31:b0  - Specifies the offset when offset addition is selected as the address update 
                 mode for transfer source or destination. */
    if ((DMA_SRC_ADDR_OFFSET == ((p_data_cfg->mode) & DMA_SRC_ADDRESSING_MASK)) || 
        (DMA_DEST_ADDR_OFFSET == ((p_data_cfg->mode) & DMA_DEST_ADDRESSING_MASK)))
    {
        p_dmac->reg->DMOFR = p_data_cfg->offset;
    }
    else
    {
        p_dmac->reg->DMOFR = 0;
    }
    
    return (DMA_OK);
    
}/* End of function R_DMAC_Create() */


/****************************************************************************************************************//**
 * @brief       In this function, it is possible to start / stop DMAC0 and so on.
 * @param[in]   cmd     DMAC Operation command
 * @param[in]   p_arg   Pointer to Argument by command
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK          DMAC control command accepted.
 * @retval      DMA_ERROR       DMAC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC_Control */
static e_dma_err_t R_DMAC_Control(e_dma_cmd_t cmd, void const * const p_arg, st_dmac_resources_t * const p_dmac) // @suppress("Function length")
{
    e_dma_err_t result = DMA_OK;
    
#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
    
    if (DMA_FLAG_OPENED != (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
#endif
    switch (cmd)
    {
        /* DMAC start */
        case DMA_CMD_START:
        {
            /* DELSRn  - DMAC Event Link setting register n
               b16     - DMAC activation request status flag - false */
            *(p_dmac->delsr) &= ~DMAC_PRV_DELSR_IR_POS;
            /* DMCNT   - DMA Transfer Enable Register
               b0      - DTE - DMA Transfer Enable - Enable */
            p_dmac->reg->DMCNT_b.DTE = 1;
        }
        break;
        
        /* DMAC stop */
        case DMA_CMD_STOP:
        {
            /* DMCNT   - DMA Transfer Enable Register
               b0      - DTE - DMA Transfer Enable - Disable */
            p_dmac->reg->DMCNT_b.DTE = 0;
        }
        break;
        
        /* DMAC Software trigger */
        case DMA_CMD_SOFTWARE_TRIGGER:
        {
            if (1 == p_dmac->reg->DMREQ_b.SWREQ)
            {
                result = DMA_ERROR;
                break;
            }

            /* DMREQ   - DMA Transfer Enable Register
               b0      - SWREQ - DMA Software Start - DMA transfer is requested. */
            p_dmac->reg->DMREQ_b.SWREQ = 0x01;
        }
        break;
        
        /* DMAC Software trigger request clear */
        case DMA_CMD_AUTO_CLEAR_SOFT_REQ:
        {
#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }
#endif
            if (true == (*(uint8_t*)p_arg)) // @suppress("Cast comment")
            {
                /* DMREQ  - DMA Software Start Register
                   b4      - CLRS  - DMA Software Start Bit Auto Clear Select - SWREQ bit is cleared after 
                                     DMA transfer is started by software. */
                p_dmac->reg->DMREQ_b.CLRS = 0;
            }
            else
            {
                /* DMREQ  - DMA Software Start Register
                   b4      - CLRS  - DMA Software Start Bit Auto Clear Select - SWREQ bit is not cleared after 
                                     DMA transfer is started by software. */
                p_dmac->reg->DMREQ_b.CLRS = 1;
            }
        }
        break;
        
        case DMA_CMD_ACT_SRC_DISABLE:
        {
            /* DMCNT   - DMA Transfer Enable Register
               b0      - DTE - DMA Transfer Enable - Disable */

            p_dmac->reg->DMCNT_b.DTE = 0;

            /* DELSRn  - DMAC Event Link setting register n
               b15 : b8 - Reserved
               b7  : b0 - DMAC Event Link selection */
            *(p_dmac->delsr) = 0x0000;
        }
        break;
        case DMA_CMD_REFRESH_DATA:
        {
#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }
#endif
            (((st_dma_refresh_data_t*)p_arg)->keep_dma_req) = false;
            (((st_dma_refresh_data_t*)p_arg)->offset) = DMAC_PRV_NOREF_OFFSET;
            result = dmac_cmd_refresh((st_dma_refresh_data_t*)p_arg, p_dmac);
        }
        break;

        case DMA_CMD_REFRESH_EXTRA:
        {
#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }
            /* Check whether the offset value is within the range */
            if ((DMAC_PRV_MAX_ADDR_OFFSET < ((((st_dma_refresh_data_t*)p_arg)->offset))) ||
                (DMAC_PRV_MIN_ADDR_OFFSET > ((((st_dma_refresh_data_t*)p_arg)->offset))))
            {
                return (DMA_ERROR_PARAMETER);
            }
#endif
            result = dmac_cmd_refresh((st_dma_refresh_data_t*)p_arg, p_dmac);
        }
        break;
        
        case DMA_CMD_ACT_SRC_ENABLE:
        case DMA_CMD_DATA_READ_SKIP_ENABLE:
        case DMA_CMD_CHAIN_TRANSFER_ABORT:
        case DMA_CMD_CHANGING_DATA_FORCIBLY_SET:
        default:
        {
            result = DMA_ERROR;
        }
        break;
        
    }
    return (result);
}/* End of function R_DMAC_Control() */

/****************************************************************************************************************//**
 * @brief       Enable the DMAC transfer complete interrupt
 * @param[in]   int_fact    Interrupt factor
 * @param[in]   p_callback  Callback event for DMAC complete.
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC_InterruptEnable */
static e_dma_err_t R_DMAC_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback,  // @suppress("Function length")
                                 st_dmac_resources_t * const p_dmac)
{
    e_dma_err_t result = DMA_OK;
    
#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
    /* DMAC open check */
    if (DMA_FLAG_OPENED != (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
    
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_dmac->dmac_int_irq) // @suppress("Cast comment")
    {
        return (DMA_ERROR_SYSTEM_SETTING);
    }
    
    if (0 != (int_fact & (~DMA_INT_MASK)))
    {
        return (DMA_ERROR);
    }
    
#endif
    
    /* Event link setting of DMAC transfer completion interrupt */
    if ((-1) == R_SYS_IrqEventLinkSet(p_dmac->dmac_int_irq, p_dmac->dmac_int_iesr_val,
                                    p_dmac->dmac_int_callback))
    {
        result = DMA_ERROR_SYSTEM_SETTING;
    }

    /* DMAC complete interrupt priority setting */
    R_NVIC_SetPriority(p_dmac->dmac_int_irq, p_dmac->dmac_int_priority);
    if (R_NVIC_GetPriority(p_dmac->dmac_int_irq) != p_dmac->dmac_int_priority)
    {
        result = DMA_ERROR_SYSTEM_SETTING;
    }
    
    if ((DMA_OK == result) && (0 != (int_fact & DMA_INT_MASK)))
    {
        /* Check dmac transfer complete interrupt */
        if (0 != (int_fact & DMA_INT_COMPLETE))
        {
        /* DMINT  - DMA Interrupt Setting Register
           b4      - DTIE  - Transfer End Interrupt Enable - Enable */
            p_dmac->reg->DMINT_b.DTIE = 1;
        }
        else
        {
        /* DMINT  - DMA Interrupt Setting Register
           b4      - DTIE  - Transfer End Interrupt Enable - Disable */
            p_dmac->reg->DMINT_b.DTIE = 0;
        }

        /* Check source address extended repeat area overflow interrupt */
        if (0 != (int_fact & DMA_INT_SRC_OVERFLOW))
        {
        /* DMINT  - DMA Interrupt Setting Register
           b1      - SARIE - Source Address Extended Repeat Area Overflow Interrupt Enable - Enable */
            p_dmac->reg->DMINT_b.SARIE = 1;
        }
        else
        {
        /* DMINT  - DMA Interrupt Setting Register
           b1      - SARIE - Source Address Extended Repeat Area Overflow Interrupt Enable - Disable */
               p_dmac->reg->DMINT_b.SARIE = 0;
        }

        /* Check destination address extended repeat area overflow interrupt */
        if (0 != (int_fact & DMA_INT_DEST_OVERFLOW))
        {
        /* DMINT  - DMA Interrupt Setting Register
           b0      - DARIE - Destination Address Extended Repeat Area Overflow Interrupt Enable - Enable */
               p_dmac->reg->DMINT_b.DARIE = 1;
        }
        else
        {
        /* DMINT  - DMA Interrupt Setting Register
           b0      - DARIE - Destination Address Extended Repeat Area Overflow Interrupt Enable - Disable */
            p_dmac->reg->DMINT_b.DARIE = 0;
        }

        /* Check repeat size end interrupt */
        if (0 != (int_fact & DMA_INT_REPEAT_END))
        {
        /* DMINT  - DMA Interrupt Setting Register
           b2      - RPTIE - Repeat Size End Interrupt Enable - Enable */
            p_dmac->reg->DMINT_b.RPTIE = 1;
        }
        else
        {
        /* DMINT  - DMA Interrupt Setting Register
           b2      - RPTIE - Repeat Size End Interrupt Enable - Disable */
            p_dmac->reg->DMINT_b.RPTIE = 0;
        }

        /* Check transfer escape end interrupt */
        if (0 != (int_fact & DMA_INT_ESCAPE_END))
        {
        /* DMINT  - DMA Interrupt Setting Register
           b3      - ESIE  - Transfer Escape End Interrupt Enable - Enable */
            p_dmac->reg->DMINT_b.ESIE = 1;
        }
        else
        {
        /* DMINT  - DMA Interrupt Setting Register
           b3      - ESIE  - Transfer Escape End Interrupt Enable - Disable */
            p_dmac->reg->DMINT_b.ESIE = 0;
        }
        
        /* callback event setting */
        p_dmac->info->cb_event = p_callback;
        
        /* Enable DMAC complete interrupt */
        R_SYS_IrqStatusClear(p_dmac->dmac_int_irq);
        R_NVIC_ClearPendingIRQ(p_dmac->dmac_int_irq);
        R_NVIC_EnableIRQ(p_dmac->dmac_int_irq);
    }

    return (result);
}/* End of function R_DMAC_InterruptEnable() */

/****************************************************************************************************************//**
 * @brief       Disable the DMAC transfer complete interrupt
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC_InterruptDisable */
static e_dma_err_t R_DMAC_InterruptDisable(st_dmac_resources_t * const p_dmac)
{
#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)

    /* DMAC open check */
    if (DMA_FLAG_OPENED != (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
    
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_dmac->dmac_int_irq) // @suppress("Cast comment")
    {
        return (DMA_ERROR_SYSTEM_SETTING);
    }
    

#endif
    
    /* DMINT  - DMA Interrupt Setting Register
       b7 :b5  - Reserved
       b4      - DTIE  - Transfer End Interrupt Enable - Disable
       b3      - ESIE  - Transfer Escape End Interrupt Enable - Disable
       b2      - RPTIE - Repeat Size End Interrupt Enable - Disable
       b1      - SARIE - Source Address Extended Repeat Area Overflow Interrupt Enable - Disable
       b0      - DARIE - Destination Address Extended Repeat Area Overflow Interrupt Enable - Disable */
    p_dmac->reg->DMINT = 0x00;
    
    /* Disable DMAC complete interrupt */
    R_NVIC_DisableIRQ(p_dmac->dmac_int_irq);
    R_SYS_IrqStatusClear(p_dmac->dmac_int_irq);
    R_NVIC_ClearPendingIRQ(p_dmac->dmac_int_irq);

    return (DMA_OK);
}/* End of function R_DMAC_InterruptDisable() */

/****************************************************************************************************************//**
 * @brief       Get DMAC status
 * @param[in,out]   state       Pointer to DMAC status buffer
 * @param[in]   p_dmac          Pointer to DMAC resource data
 * @retval      DMA_OK          Getting DMAC state successfully.
 * @retval      DMA_ERROR       Getting DMAC state error.
**********************************************************************************************************************/
/* Function Name : R_DMAC_GetState */
static e_dma_err_t R_DMAC_GetState(st_dma_state_t * const state, st_dmac_resources_t * const p_dmac)
{

#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
    /* Open check */
    if (DMA_FLAG_OPENED != (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
    
#endif
    /* Get DMAC Active flag */
    state->in_progress = p_dmac->reg->DMSTS_b.ACT;
    
    /* Get Transfer escape interrupt flag */
    state->esif_stat = p_dmac->reg->DMSTS_b.ESIF;
    
    /* Get Transfer end interrupt flag */
    state->dtif_stat = p_dmac->reg->DMSTS_b.DTIF;

    /* Get Software request flag */
    state->soft_req_stat = p_dmac->reg->DMREQ_b.SWREQ;
    return (DMA_OK);
    
}/* End of function R_DMAC_GetState() */

/****************************************************************************************************************//**
 * @brief       Clear DMAC status
 * @param[in]   clr_state       Target clearing status
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK          Clear DMAC state successfully.
 * @retval      DMA_ERROR       Clear DMAC state error.
**********************************************************************************************************************/
/* Function Name : R_DMAC_ClearState */
static e_dma_err_t R_DMAC_ClearState(uint32_t clr_state, st_dmac_resources_t * const p_dmac)
{

#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
    /* Open check */
    if (DMA_FLAG_OPENED != (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
    
#endif
    
    if (0 != (DMA_CLR_STATE_ESIF & clr_state))
    {
        /* DMSTS  - DMA Status Register
           b0      - ESIF  - Transfer Escape End Interrupt Flag - A transfer escape end interrupt has not 
                                                                  been generated. */
        p_dmac->reg->DMSTS_b.ESIF = 0;
    }
    
    if (0 != (DMA_CLR_STATE_DTIF & clr_state))
    {
        /* DMSTS  - DMA Status Register
           b4      - DTIF  - Transfer End Interrupt Flag - A transfer end interrupt has not been generated. */
        p_dmac->reg->DMSTS_b.DTIF = 0;
    }

    if (0 != (DMA_CLR_STATE_SOFT_REQ & clr_state))
    {

        /* DMREQ  - DMA Software Start Register
           b0      - SWREQ - DMA Software Start - DMA transfer is not requested. */
        p_dmac->reg->DMREQ_b.SWREQ = 0;
    }
    
    return (DMA_OK);
    
}/* End of function R_DMAC_ClearState() */

/*****************************************************************************************************************//**
 * @brief       Get transfer byte
 * @param[in,out]   transfer_byte       Pointer to transfer byte buffer
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK          Getting DMAC transfer byte successfully.
 * @retval      DMA_ERROR       Getting DMAC transfer byte error.
**********************************************************************************************************************/
/* Function Name : R_DMAC_GetTransferByte */
static e_dma_err_t  R_DMAC_GetTransferByte(uint32_t * transfer_byte, st_dmac_resources_t * const p_dmac) // @suppress("Source line ordering")
{
    uint32_t retval = 0;
    uint32_t dmtmd_sz = 0;

#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
    /* Open check */
    if (DMA_FLAG_OPENED != (p_dmac->info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
    
#endif

    /* Check data size */
    switch (p_dmac->reg->DMTMD_b.SZ)
    {
        case DMAC_PRV_DMTMD_BYTE:
        {
            dmtmd_sz = 1;   /* 1byte */
        }
        break;

        case DMAC_PRV_DMTMD_WORD:
        {
            dmtmd_sz = 2;   /* 2byte */
        }
        break;

        case DMAC_PRV_DMTMD_LONG:
        {
            dmtmd_sz = 4;   /* 4byte */
        }
        break;
        
        default:
        {
            /* Invalid data size */
            return (DMA_ERROR_MODE);
        }
        break;
    }
    
    
    switch ((p_dmac->reg->DMTMD) & DMA_TRANSFER_MODE_MASK)
    {
        case DMA_MODE_NORMAL:
        {
            uint32_t transfer_counter = 0;
            
            if (DMAC_PRV_FREERUN == p_dmac->info->dmcount)
            {
                /* Free running mode (The counter is not working) */
                return (DMA_ERROR);
            }
            
            transfer_counter = (p_dmac->info->dmcount) - (p_dmac->reg->DMCRA_b.DMCRAL);
            retval = dmtmd_sz * transfer_counter;
        }
        break;
        
        case DMA_MODE_REPEAT:
        case DMA_MODE_BLOCK:
        {
            uint16_t repeat_size = (p_dmac->reg->DMCRA_b.DMCRAH) & 0x3FF;
            uint16_t transfer_counter = p_dmac->reg->DMCRA_b.DMCRAL & 0x3FF;
            uint32_t transfer_times = p_dmac->reg->DMCRB;
            
            if (0 == repeat_size)
            {
                repeat_size = 1024;
            }
            
            if (0 == transfer_counter)
            {
                transfer_counter = 1024;
            }
            
            if ((0 == transfer_times) && (1 == p_dmac->reg->DMCNT_b.DTE))
            {
                transfer_times = 65536;
            }
            
            retval = dmtmd_sz * (repeat_size - transfer_counter);
            retval += dmtmd_sz * (repeat_size * (p_dmac->info->dmcount - transfer_times));
            
        }
        break;
        
        default:
        {
            return (DMA_ERROR_MODE);
        }
        break;
    }
        
    *transfer_byte = retval;
    
    return (DMA_OK);
}/* End of function R_DMAC_GetTransferByte() */

/****************************************************************************************************************//**
 * @brief       Get DTC available source(DMAC Not Used)
 * @param[out]  src_info     Pointer to DMAC available source data buffer
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC. 
**********************************************************************************************************************/
/* Function Name : R_DMAC_GetAvailabilitySrc */
static e_dma_err_t R_DMAC_GetAvailabilitySrc(uint64_t * const src_info, st_dmac_resources_t * const p_dmac)
{
    return DMA_ERROR_UNSUPPORTED;
}/* End of function R_DMAC_GetAvailabilitySrc() */

/****************************************************************************************************************//**
 * @brief       Release DTC transfer source infomation(DMAC Not Used).
 * @param[in]   dmac_source      DMAC transfer trigger source.
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC.
**********************************************************************************************************************/
/* Function Name : R_DMAC_Release */
static e_dma_err_t R_DMAC_Release(int16_t const dmac_source, st_dmac_resources_t * const p_dmac)
{
    return DMA_ERROR_UNSUPPORTED;
}

/****************************************************************************************************************//**
 * @brief       Set dmac active channel 
 * @param[in]   ch  Set dmac active channel
**********************************************************************************************************************/
/* Function Name : dmac_active_set */
static void dmac_active_set(uint16_t ch)
{
    g_dma_active_flag |= ch;

    /* DMAST  - DMACA Module Activation Register
       b7 :b1  - Reserved
       b0      - DMST - DMAC Operation Enable - DMAC activation is enabled. */
    DMA->DMAST_b.DMST = 1;
}/* End of function dmac_active_set() */

/****************************************************************************************************************//**
 * @brief       Clear dmac active channel
 * @param[in]   ch  Clear dmac active channel
**********************************************************************************************************************/
/* Function Name : dmac_active_clear */
static void dmac_active_clear(uint16_t ch)
{
    g_dma_active_flag &= (~ch);
    if (0 == (g_dma_active_flag & DMA_ACTIVE_DMAC_MASK))
    {
        /* DMAST  - DMACA Module Activation Register
           b7 :b1  - Reserved
           b0      - DMST - DMAC Operation Enable - DMAC activation is disabled. */
        DMA->DMAST_b.DMST = 0;
    }
}/* End of function dmac_active_clear() */


/****************************************************************************************************************//**
 * @brief       Refresh the DMA transfer information 
 * @param[in]   p_data   Pointer to refresh data
 * @param[in,out]    p_dmac     Pointer to DMAC resource data
 * @retval      DMA_OK          DMAC control command accepted.
 * @retval      DMA_ERROR       DMAC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
**********************************************************************************************************************/
/* Function Name : dmac_cmd_refresh */
static e_dma_err_t dmac_cmd_refresh(st_dma_refresh_data_t const * const p_data, st_dmac_resources_t * const p_dmac)
{
    /* MODE check */
    uint16_t mode = p_dmac->reg->DMTMD;

#if (1 == DMAC_CFG_PARAM_CHECKING_ENABLE)
    switch (mode & DMA_TRANSFER_MODE_MASK)
    {
        case DMA_MODE_NORMAL:
        {
            /* In the case of normal transfer, the setting range of transfer count is 1 to 65535. */
            if (p_data->transfer_count > (DMAC_PRV_MAX_16BITS_COUNT_VAL-1))
            {
                return (DMA_ERROR_PARAMETER);
            }
            
            /* In case of normal transfer, no minimum value. */
            /* When 0 is set, the free running mode is set. */
        }
        break;
        
        case DMA_MODE_REPEAT:
        case DMA_MODE_BLOCK:
        {
            /* In the case of repeat/block transfer, the setting range of transfer count is 1 to 65536. */
            if ((p_data->transfer_count < DMAC_PRV_MIN_COUNT_VAL) ||
                (p_data->transfer_count > DMAC_PRV_MAX_16BITS_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }

            /* In the case of repeat/block transfer, the setting range of block size is 1 to 1024. */
            if ((p_data->block_size > DMAC_PRV_MAX_10BITS_COUNT_VAL) ||
                (p_data->block_size < DMAC_PRV_MIN_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        
        default:
        {
            /* Invalid mode */
            return (DMA_ERROR_MODE);
        }
        break;
    }
#endif
    /* DMCNT   - DMA Transfer Enable Register
       b0      - DTE - DMA Transfer Enable - Disable */
    p_dmac->reg->DMCNT_b.DTE = 0;
    
    /* Rewriting transfer information */
    if ((uint32_t)NULL != p_data->src_addr)
    {
        /* DMSAR  - DMA Source Address Register
           b31:b0  - Specifies the transfer source start address. */
        p_dmac->reg->DMSAR = p_data->src_addr;
    }
    if ((uint32_t)NULL != p_data->dest_addr)
    {
        /* DMDAR  - DMA Destination Address Register
           b31:b0  - Specifies the transfer destination start address. */
        p_dmac->reg->DMDAR = p_data->dest_addr;
    }
    if (DMAC_PRV_NOREF_OFFSET != p_data->offset)
    {
        /* DMOFR  - DMA Offset Register
           b31:b0  - Specifies the offset when offset addition is selected as the address update 
                     mode for transfer source or destination. */
        p_dmac->reg->DMOFR = p_data->offset;
    }
    switch (mode & DMA_TRANSFER_MODE_MASK)
    {
        case DMA_MODE_NORMAL:
        {
            p_dmac->reg->DMCRA_b.DMCRAL = p_data->transfer_count;
            p_dmac->reg->DMCRA_b.DMCRAH = 0;
            if (0 != p_data->transfer_count)
            {
                p_dmac->info->dmcount = p_data->transfer_count;
            }
            else
            {
                /* Set freerun mode */
                p_dmac->info->dmcount = DMAC_PRV_FREERUN;
            }
        }
        break;
        
        case DMA_MODE_REPEAT:
        case DMA_MODE_BLOCK:
        {
            p_dmac->reg->DMCRA_b.DMCRAL = (p_data->block_size & 0x3FF);
            p_dmac->reg->DMCRA_b.DMCRAH = (p_data->block_size & 0x3FF);
            p_dmac->reg->DMCRB = (p_data->transfer_count & 0xFFFF);
            p_dmac->info->dmcount = p_data->transfer_count;
        }
        break;
        
        default:
        {
            /* No action to be performed */
        }
        break;
    }
    
    /* When auto start is enabled, start transfer */
    if (true == p_data->auto_start)
    {
        if (false == p_data->keep_dma_req)
        {
            /* DELSRn  - DMAC Event Link setting register n
               b16     - DMAC activation request status flag - false */
            *(p_dmac->delsr) &= ~DMAC_PRV_DELSR_IR_POS;
        }
        /* DMCNT   - DMA Transfer Enable Register
           b0      - DTE - DMA Transfer Enable - Enable */
        p_dmac->reg->DMCNT_b.DTE = 1;
    }
    return DMA_OK;
}/* End of function dmac_cmd_refresh() */

/****************************************************************************************************************//**
 * @brief       dmac interrupt function
 * @param[in]    p_dmac     Pointer to DMAC resource data
**********************************************************************************************************************/
/* Function Name : dmac_interrupt_handler */
static void dmac_interrupt_handler(st_dmac_resources_t * const p_dmac)
{
    
    if (NULL != p_dmac->info->cb_event) // @suppress("Cast comment")
    {
        p_dmac->info->cb_event();
    }
}/* End of function dmac_interrupt_handler() */


/* End DMAC Interface */


/****************************************************************************
 * The Driver Instance of ACCESS STRUCT for the HAL-Driver DMAC0 channel.
 ****************************************************************************/
DRIVER_DMA Driver_DMAC0 DATA_LOCATION_PRV_DRIVER_DMAC =
{
    R_DMAC_GetVersion,
    R_DMAC0_Open,
    R_DMAC0_Close,
    R_DMAC0_Create,
    R_DMAC0_Control,
    R_DMAC0_InterruptEnable,
    R_DMAC0_InterruptDisable,
    R_DMAC0_GetState,
    R_DMAC0_ClearState,
    R_DMAC0_GetTransferByte,
    R_DMAC0_GetAvailabilitySrc,
    R_DMAC0_Release
    
};

/*****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Open API for the DMAC0 peripheral on RE Family.
 * @retval      DMA_OK  Open successful.
 * @retval      DMA_ERROR  Open error.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_Open */
static e_dma_err_t R_DMAC0_Open(void)
{
    return (R_DMAC_Open(&gs_dmac0_resources));
}/* End of function R_DMAC0_Open() */

/****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Close API for the DMAC0 peripheral on RE Family.
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_Close */
static e_dma_err_t R_DMAC0_Close(void)
{
    return (R_DMAC_Close(&gs_dmac0_resources));
}/* End of function R_DMAC0_Close() */

/****************************************************************************************************************//**
 * @brief       Create DMAC0 transfer infomation.
 * @param[in]   dmac_source     DMAC transfer trigger source.
 * @param[in]   p_data_cfg      Pointer to DMAC transfer setting infomation.
 * @retval      DMA_OK          DMAC transfer infomation create successful.
 * @retval      DMA_ERROR       DMAC transfer infomation create error.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_Create */
static e_dma_err_t R_DMAC0_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
{
    return (R_DMAC_Create(dmac_source, p_data_cfg, &gs_dmac0_resources));
}/* End of function R_DMAC0_Create() */

/****************************************************************************************************************//**
 * @brief       In this function, it is possible to start / stop DMAC0 and so on.
 * @param[in]   cmd     DMAC Operation command
 * @param[in]   p_arg   Pointer to Argument by command
 * @retval      DMA_OK          DMAC control command accepted.
 * @retval      DMA_ERROR       DMAC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_Control */
static e_dma_err_t R_DMAC0_Control(e_dma_cmd_t cmd, void const * const p_arg)
{
    return (R_DMAC_Control(cmd, p_arg, &gs_dmac0_resources));
}/* End of function R_DMAC0_Control() */

/****************************************************************************************************************//**
 * @brief       Enable the DMAC0 transfer complete interrupt
 * @param[in]   int_fact    Interrupt factor
 * @param[in]   p_callback  Callback event for DMAC complete.
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_InterruptEnable */
static e_dma_err_t R_DMAC0_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
{
    return (R_DMAC_InterruptEnable(int_fact, p_callback, &gs_dmac0_resources));
}/* End of function R_DMAC0_InterruptEnable() */

/****************************************************************************************************************//**
 * @brief       Disable the DMAC0 transfer complete interrupt
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_InterruptDisable */
static e_dma_err_t R_DMAC0_InterruptDisable(void)
{
    return (R_DMAC_InterruptDisable(&gs_dmac0_resources));
}/* End of function R_DMAC0_InterruptDisable() */

/****************************************************************************************************************//**
 * @brief       Get DMAC0 status
 * @param[in,out]   state       Pointer to DMAC status buffer
 * @retval      DMA_OK          Getting DMAC state successfully.
 * @retval      DMA_ERROR       Getting DMAC state error.
 * @note   The operation vector number is fixed to 0 because it is invalid in DMAC
**********************************************************************************************************************/
/* Function Name : R_DMAC0_GetState */
static e_dma_err_t R_DMAC0_GetState(st_dma_state_t * const state)
{
    return (R_DMAC_GetState(state, &gs_dmac0_resources));
}/* End of function R_DMAC0_GetState() */

/****************************************************************************************************************//**
 * @brief       Clear DMAC0 status
 * @param[in]   clr_state       Target clearing status
 * @retval      DMA_OK          Clear DMAC state successfully.
 * @retval      DMA_ERROR       Clear DMAC state error.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_ClearState */
static e_dma_err_t R_DMAC0_ClearState(uint32_t clr_state)
{
    return (R_DMAC_ClearState(clr_state, &gs_dmac0_resources));
}/* End of function R_DMAC0_ClearState() */

/****************************************************************************************************************//**
 * @brief       Get DMAC0 transfer byte
 * @param[in]   dmac_source   DMA transfer activation factor( Not used with DMAC )
 * @param[in,out]   transfer_byte       Pointer to transfer byte buffer
 * @retval      DMA_OK          Getting DMAC0 transfer byte successfully.
 * @retval      DMA_ERROR       Getting DMAC0 transfer byte error.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_GetTransferByte */
static e_dma_err_t R_DMAC0_GetTransferByte(int16_t const dmac_source, uint32_t * transfer_byte)
{
    
    return (R_DMAC_GetTransferByte(transfer_byte, &gs_dmac0_resources));
}/* End of function R_DMAC0_GetTransferByte() */

/****************************************************************************************************************//**
 * @brief       Release DTC transfer source infomation(DMAC Not Used).
 * @param[in]   dmac_source      DMAC transfer trigger source.
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC.
**********************************************************************************************************************/
/* Function Name : R_DMAC0_Release */
static e_dma_err_t R_DMAC0_Release(int16_t const dmac_source)
{
    return (R_DMAC_Release(dmac_source, &gs_dmac0_resources));
}/* End of function R_DMAC0_Release() */

/****************************************************************************************************************//**
 * @brief       Get DTC available source(DMAC Not Used)
 * @param[out]  src_info     Pointer to DMAC available source data buffer
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC. 
**********************************************************************************************************************/
/* Function Name : R_DMAC0_GetAvailabilitySrc */
static e_dma_err_t R_DMAC0_GetAvailabilitySrc(uint64_t * const src_info)
{
    return (R_DMAC_GetAvailabilitySrc(src_info, &gs_dmac0_resources));
}/* End of function R_DMAC0_GetAvailabilitySrc() */

/****************************************************************************************************************//**
 * @brief       dmac0 interrupt function
**********************************************************************************************************************/
/* Function Name : dmac0_interrupt_handler */
static void dmac0_interrupt_handler(void)
{
    dmac_interrupt_handler(&gs_dmac0_resources);
}/* End of function dmac0_interrupt_handler() */



/****************************************************************************
 * The Driver Instance of ACCESS STRUCT for the HAL-Driver DMAC1 channel.
 ****************************************************************************/
DRIVER_DMA Driver_DMAC1 DATA_LOCATION_PRV_DRIVER_DMAC =
{
    R_DMAC_GetVersion,
    R_DMAC1_Open,
    R_DMAC1_Close,
    R_DMAC1_Create,
    R_DMAC1_Control,
    R_DMAC1_InterruptEnable,
    R_DMAC1_InterruptDisable,
    R_DMAC1_GetState,
    R_DMAC1_ClearState,
    R_DMAC1_GetTransferByte,
    R_DMAC1_GetAvailabilitySrc,
    R_DMAC1_Release
};

/*****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Open API for the DMAC1 peripheral on RE Family.
 * @retval      DMA_OK  Open successful.
 * @retval      DMA_ERROR  Open error.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_Open */
static e_dma_err_t R_DMAC1_Open(void)
{
    return (R_DMAC_Open(&gs_dmac1_resources));
}/* End of function R_DMAC1_Open() */

/****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Close API for the DMAC1 peripheral on RE Family.
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_Close */
static e_dma_err_t R_DMAC1_Close(void)
{
    return (R_DMAC_Close(&gs_dmac1_resources));
}/* End of function R_DMAC1_Close() */

/****************************************************************************************************************//**
 * @brief       Create DMAC1 transfer infomation.
 * @param[in]   dmac_source     DMAC transfer trigger source.
 * @param[in]   p_data_cfg      Pointer to DMAC transfer setting infomation.
 * @retval      DMA_OK          DMAC transfer infomation create successful.
 * @retval      DMA_ERROR       DMAC transfer infomation create error.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_Create */
static e_dma_err_t R_DMAC1_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
{
    return (R_DMAC_Create(dmac_source, p_data_cfg, &gs_dmac1_resources));
}/* End of function R_DMAC1_Create() */

/****************************************************************************************************************//**
 * @brief       In this function, it is possible to start / stop DMAC1 and so on.
 * @param[in]   cmd     DMAC Operation command
 * @param[in]   p_arg   Pointer to Argument by command
 * @retval      DMA_OK          DMAC control command accepted.
 * @retval      DMA_ERROR       DMAC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_Control */
static e_dma_err_t R_DMAC1_Control(e_dma_cmd_t cmd, void const * const p_arg)
{
    return (R_DMAC_Control(cmd, p_arg, &gs_dmac1_resources));
}/* End of function R_DMAC1_Control() */

/****************************************************************************************************************//**
 * @brief       Enable the DMAC1 transfer complete interrupt
 * @param[in]   int_fact    Interrupt factor
 * @param[in]   p_callback  Callback event for DMAC complete.
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_InterruptEnable */
static e_dma_err_t R_DMAC1_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
{
    return (R_DMAC_InterruptEnable(int_fact, p_callback, &gs_dmac1_resources));
}/* End of function R_DMAC1_InterruptEnable() */

/****************************************************************************************************************//**
 * @brief       Disable the DMAC1 transfer complete interrupt
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_InterruptDisable */
static e_dma_err_t R_DMAC1_InterruptDisable(void)
{
    return (R_DMAC_InterruptDisable(&gs_dmac1_resources));
}/* End of function R_DMAC1_InterruptDisable() */

/****************************************************************************************************************//**
 * @brief       Get DMAC1 status
 * @param[in,out]   state       Pointer to DMAC status buffer
 * @retval      DMA_OK         e error.
 * @note   The operation vector number is fixed to 0 because it is invalid in DMAC
**********************************************************************************************************************/
/* Function Name : R_DMAC1_GetState */
static e_dma_err_t R_DMAC1_GetState(st_dma_state_t * const state)
{
    return (R_DMAC_GetState(state, &gs_dmac1_resources));
}/* End of function R_DMAC1_GetState() */

/****************************************************************************************************************//**
 * @brief       Clear DMAC1 status
 * @param[in]   clr_state       Target clearing status
 * @retval      DMA_OK          Clear DMAC state successfully.
 * @retval      DMA_ERROR       Clear DMAC state error.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_ClearState */
static e_dma_err_t R_DMAC1_ClearState(uint32_t clr_state)
{
    return (R_DMAC_ClearState(clr_state, &gs_dmac1_resources));
}/* End of function R_DMAC1_ClearState() */

/****************************************************************************************************************//**
 * @brief       Get DMAC1 transfer byte
 * @param[in]   dmac_source   DMA transfer activation factor( Not used with DMAC )
 * @param[in,out]   transfer_byte       Pointer to transfer byte buffer
 * @retval      DMA_OK          Getting DMAC1 transfer byte successfully.
 * @retval      DMA_ERROR       Getting DMAC1 transfer byte error.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_GetTransferByte */
static e_dma_err_t R_DMAC1_GetTransferByte(int16_t const dmac_source, uint32_t * transfer_byte)
{
    
    return (R_DMAC_GetTransferByte(transfer_byte, &gs_dmac1_resources));
}/* End of function R_DMAC0_GetTransferByte() */

/****************************************************************************************************************//**
 * @brief       Release DTC transfer source infomation(DMAC Not Used).
 * @param[in]   dmac_source      DMAC transfer trigger source.
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC.
**********************************************************************************************************************/
/* Function Name : R_DMAC1_Release */
static e_dma_err_t R_DMAC1_Release(int16_t const dmac_source)
{
    return (R_DMAC_Release(dmac_source, &gs_dmac0_resources));
}/* End of function R_DMAC1_Release() */

/****************************************************************************************************************//**
 * @brief       Get DTC available source(DMAC Not Used)
 * @param[out]  src_info     Pointer to DMAC available source data buffer
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC. 
**********************************************************************************************************************/
/* Function Name : R_DMAC1_GetAvailabilitySrc */
static e_dma_err_t R_DMAC1_GetAvailabilitySrc(uint64_t * const src_info)
{
    return (R_DMAC_GetAvailabilitySrc(src_info, &gs_dmac0_resources));
}/* End of function R_DMAC1_GetAvailabilitySrc() */

/****************************************************************************************************************//**
 * @brief       dmac1 interrupt function
**********************************************************************************************************************/
/* Function Name : dmac1_interrupt_handler */
static void dmac1_interrupt_handler(void)
{
    dmac_interrupt_handler(&gs_dmac1_resources);
}/* End of function dmac1_interrupt_handler() */


/****************************************************************************
 * The Driver Instance of ACCESS STRUCT for the HAL-Driver DMAC2 channel.
 ****************************************************************************/
DRIVER_DMA Driver_DMAC2 DATA_LOCATION_PRV_DRIVER_DMAC =
{
    R_DMAC_GetVersion,
    R_DMAC2_Open,
    R_DMAC2_Close,
    R_DMAC2_Create,
    R_DMAC2_Control,
    R_DMAC2_InterruptEnable,
    R_DMAC2_InterruptDisable,
    R_DMAC2_GetState,
    R_DMAC2_ClearState,
    R_DMAC2_GetTransferByte,
    R_DMAC2_GetAvailabilitySrc,
    R_DMAC2_Release
};

/*****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Open API for the DMAC2 peripheral on RE Family.
 * @retval      DMA_OK  Open successful.
 * @retval      DMA_ERROR  Open error.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_Open */
static e_dma_err_t R_DMAC2_Open(void)
{
    return (R_DMAC_Open(&gs_dmac2_resources));
}/* End of function R_DMAC2_Open() */

/****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Close API for the DMAC2 peripheral on RE Family.
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_Close */
static e_dma_err_t R_DMAC2_Close(void)
{
    return (R_DMAC_Close(&gs_dmac2_resources));
}/* End of function R_DMAC2_Close() */

/****************************************************************************************************************//**
 * @brief       Create DMAC2 transfer infomation.
 * @param[in]   dmac_source     DMAC transfer trigger source.
 * @param[in]   p_data_cfg      Pointer to DMAC transfer setting infomation.
 * @retval      DMA_OK          DMAC transfer infomation create successful.
 * @retval      DMA_ERROR       DMAC transfer infomation create error.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_Create */
static e_dma_err_t R_DMAC2_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
{
    return (R_DMAC_Create(dmac_source, p_data_cfg, &gs_dmac2_resources));
}/* End of function R_DMAC2_Create() */

/****************************************************************************************************************//**
 * @brief       In this function, it is possible to start / stop DMAC2 and so on.
 * @param[in]   cmd     DMAC Operation command
 * @param[in]   p_arg   Pointer to Argument by command
 * @retval      DMA_OK          DMAC control command accepted.
 * @retval      DMA_ERROR       DMAC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_Control */
static e_dma_err_t R_DMAC2_Control(e_dma_cmd_t cmd, void const * const p_arg)
{
    return (R_DMAC_Control(cmd, p_arg, &gs_dmac2_resources));
}/* End of function R_DMAC2_Control() */

/****************************************************************************************************************//**
 * @brief       Enable the DMAC2 transfer complete interrupt
 * @param[in]   int_fact    Interrupt factor
 * @param[in]   p_callback  Callback event for DMAC complete.
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_InterruptEnable */
static e_dma_err_t R_DMAC2_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
{
    return (R_DMAC_InterruptEnable(int_fact, p_callback, &gs_dmac2_resources));
}/* End of function R_DMAC2_InterruptEnable() */

/****************************************************************************************************************//**
 * @brief       Disable the DMAC2 transfer complete interrupt
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_InterruptDisable */
static e_dma_err_t R_DMAC2_InterruptDisable(void)
{
    return (R_DMAC_InterruptDisable(&gs_dmac2_resources));
}/* End of function R_DMAC2_InterruptDisable() */

/****************************************************************************************************************//**
 * @brief       Get DMAC2 status
 * @param[in,out]   state       Pointer to DMAC status buffer
 * @retval      DMA_OK          Getting DMAC state successfully.
 * @retval      DMA_ERROR       Getting DMAC state error.
 * @note   The operation vector number is fixed to 0 because it is invalid in DMAC
**********************************************************************************************************************/
/* Function Name : R_DMAC2_GetState */
static e_dma_err_t R_DMAC2_GetState(st_dma_state_t * const state)
{
    return (R_DMAC_GetState(state, &gs_dmac2_resources));
}/* End of function R_DMAC2_GetState() */

/****************************************************************************************************************//**
 * @brief       Clear DMAC2 status
 * @param[in]   clr_state       Target clearing status
 * @retval      DMA_OK          Clear DMAC state successfully.
 * @retval      DMA_ERROR       Clear DMAC state error.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_ClearState */
static e_dma_err_t R_DMAC2_ClearState(uint32_t clr_state)
{
    return (R_DMAC_ClearState(clr_state, &gs_dmac2_resources));
}/* End of function R_DMAC2_ClearState() */

/****************************************************************************************************************//**
 * @brief       Get DMAC2 transfer byte
 * @param[in]   dmac_source   DMA transfer activation factor( Not used with DMAC )
 * @param[in,out]   transfer_byte       Pointer to transfer byte buffer
 * @retval      DMA_OK          Getting DMAC2 transfer byte successfully.
 * @retval      DMA_ERROR       Getting DMAC2 transfer byte error.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_GetTransferByte */
static e_dma_err_t R_DMAC2_GetTransferByte(int16_t const dmac_source, uint32_t * transfer_byte)
{
    
    return (R_DMAC_GetTransferByte(transfer_byte, &gs_dmac2_resources));
}/* End of function R_DMAC0_GetTransferByte() */

/****************************************************************************************************************//**
 * @brief       Release DTC transfer source infomation(DMAC Not Used).
 * @param[in]   dmac_source      DMAC transfer trigger source.
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC.
**********************************************************************************************************************/
/* Function Name : R_DMAC2_Release */
static e_dma_err_t R_DMAC2_Release(int16_t const dmac_source)
{
    return (R_DMAC_Release(dmac_source, &gs_dmac0_resources));
}/* End of function R_DMAC2_Release() */

/****************************************************************************************************************//**
 * @brief       Get DTC available source(DMAC Not Used)
 * @param[out]  src_info     Pointer to DMAC available source data buffer
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC. 
**********************************************************************************************************************/
/* Function Name : R_DMAC2_GetAvailabilitySrc */
static e_dma_err_t R_DMAC2_GetAvailabilitySrc(uint64_t * const src_info)
{
    return (R_DMAC_GetAvailabilitySrc(src_info, &gs_dmac0_resources));
}/* End of function R_DMAC2_GetAvailabilitySrc() */


/****************************************************************************************************************//**
 * @brief       dmac2 interrupt function
**********************************************************************************************************************/
/* Function Name : dmac2_interrupt_handler */
static void dmac2_interrupt_handler(void)
{
    dmac_interrupt_handler(&gs_dmac2_resources);
}/* End of function dmac2_interrupt_handler() */

/****************************************************************************
 * The Driver Instance of ACCESS STRUCT for the HAL-Driver DMAC3 channel.
 ****************************************************************************/
DRIVER_DMA Driver_DMAC3 DATA_LOCATION_PRV_DRIVER_DMAC  =
{
    R_DMAC_GetVersion,
    R_DMAC3_Open,
    R_DMAC3_Close,
    R_DMAC3_Create,
    R_DMAC3_Control,
    R_DMAC3_InterruptEnable,
    R_DMAC3_InterruptDisable,
    R_DMAC3_GetState,
    R_DMAC3_ClearState,
    R_DMAC3_GetTransferByte,
    R_DMAC3_GetAvailabilitySrc,
    R_DMAC3_Release
};

/*****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Open API for the DMAC3 peripheral on RE Family.
 * @retval      DMA_OK  Open successful.
 * @retval      DMA_ERROR  Open error.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_Open */
static e_dma_err_t R_DMAC3_Open(void)
{
    return (R_DMAC_Open(&gs_dmac3_resources));
}/* End of function R_DMAC3_Open() */

/****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DMAC Close API for the DMAC3 peripheral on RE Family.
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_Close */
static e_dma_err_t R_DMAC3_Close(void)
{
    return (R_DMAC_Close(&gs_dmac3_resources));
}/* End of function R_DMAC3_Close() */

/****************************************************************************************************************//**
 * @brief       Create DMAC3 transfer infomation.
 * @param[in]   dmac_source     DMAC transfer trigger source.
 * @param[in]   p_data_cfg      Pointer to DMAC transfer setting infomation.
 * @retval      DMA_OK          DMAC transfer infomation create successful.
 * @retval      DMA_ERROR       DMAC transfer infomation create error.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_Create */
static e_dma_err_t R_DMAC3_Create(int16_t const dmac_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
{
    return (R_DMAC_Create(dmac_source, p_data_cfg, &gs_dmac3_resources));
}/* End of function R_DMAC3_Create() */

/****************************************************************************************************************//**
 * @brief       In this function, it is possible to start / stop DMAC3 and so on.
 * @param[in]   cmd     DMAC Operation command
 * @param[in]   p_arg   Pointer to Argument by command
 * @retval      DMA_OK          DMAC control command accepted.
 * @retval      DMA_ERROR       DMAC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_Control */
static e_dma_err_t R_DMAC3_Control(e_dma_cmd_t cmd, void const * const p_arg)
{
    return (R_DMAC_Control(cmd, p_arg, &gs_dmac3_resources));
}/* End of function R_DMAC3_Control() */

/****************************************************************************************************************//**
 * @brief       Enable the DMAC3 transfer complete interrupt
 * @param[in]   int_fact    Interrupt factor
 * @param[in]   p_callback  Callback event for DMAC complete.
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_InterruptEnable */
static e_dma_err_t R_DMAC3_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
{
    return (R_DMAC_InterruptEnable(int_fact, p_callback, &gs_dmac3_resources));
}/* End of function R_DMAC3_InterruptEnable() */

/****************************************************************************************************************//**
 * @brief       Disable the DMAC3 transfer complete interrupt
 * @retval      DMA_OK          DMAC complete settting successful.
 * @retval      DMA_ERROR       DMAC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DMAC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_InterruptDisable */
static e_dma_err_t R_DMAC3_InterruptDisable(void)
{
    return (R_DMAC_InterruptDisable(&gs_dmac3_resources));
}/* End of function R_DMAC3_InterruptDisable() */

/****************************************************************************************************************//**
 * @brief       Get DMAC3 status
 * @param[in,out]   state       Pointer to DMAC status buffer
 * @retval      DMA_OK          Getting DMAC state successfully.
 * @retval      DMA_ERROR       Getting DMAC state error.
 * @note   The operation vector number is fixed to 0 because it is invalid in DMAC
**********************************************************************************************************************/
/* Function Name : R_DMAC3_GetState */
static e_dma_err_t R_DMAC3_GetState(st_dma_state_t * const state)
{
    return (R_DMAC_GetState(state, &gs_dmac3_resources));
}/* End of function R_DMAC3_GetState() */

/****************************************************************************************************************//**
 * @brief       Clear DMAC3 status
 * @param[in]   clr_state       Target clearing status
 * @retval      DMA_OK          Clear DMAC state successfully.
 * @retval      DMA_ERROR       Clear DMAC state error.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_ClearState */
static e_dma_err_t R_DMAC3_ClearState(uint32_t clr_state)
{
    return (R_DMAC_ClearState(clr_state, &gs_dmac3_resources));
}/* End of function R_DMAC3_ClearState() */

/****************************************************************************************************************//**
 * @brief       Get DMAC3 transfer byte
 * @param[in]   dmac_source   DMA transfer activation factor( Not used with DMAC )
 * @param[in,out]   transfer_byte       Pointer to transfer byte buffer
 * @retval      DMA_OK          Getting DMAC3 transfer byte successfully.
 * @retval      DMA_ERROR       Getting DMAC3 transfer byte error.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_GetTransferByte */
static e_dma_err_t R_DMAC3_GetTransferByte(int16_t const dmac_source, uint32_t * transfer_byte)
{
    
    return (R_DMAC_GetTransferByte(transfer_byte, &gs_dmac3_resources));
}/* End of function R_DMAC0_GetTransferByte() */

/****************************************************************************************************************//**
 * @brief       Release DTC transfer source infomation(DMAC Not Used).
 * @param[in]   dmac_source      DMAC transfer trigger source.
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC.
**********************************************************************************************************************/
/* Function Name : R_DMAC3_Release */
static e_dma_err_t R_DMAC3_Release(int16_t const dmac_source)
{
    return (R_DMAC_Release(dmac_source, &gs_dmac0_resources));
}/* End of function R_DMAC3_Release() */

/****************************************************************************************************************//**
 * @brief       Get DTC available source(DMAC Not Used)
 * @param[out]  src_info     Pointer to DMAC available source data buffer
 * @param[in]   p_dmac       Pointer to DMAC resource data
 * @retval      DMA_ERROR_UNSUPPORTED       Error because it cannot be used with DMAC.
 * @note        This function can only be done with DTC. 
**********************************************************************************************************************/
/* Function Name : R_DMAC3_GetAvailabilitySrc */
static e_dma_err_t R_DMAC3_GetAvailabilitySrc(uint64_t * const src_info)
{
    return (R_DMAC_GetAvailabilitySrc(src_info, &gs_dmac0_resources));
}/* End of function R_DMAC3_GetAvailabilitySrc() */


/****************************************************************************************************************//**
 * @brief       dmac3 interrupt function
**********************************************************************************************************************/
/* Function Name : dmac3_interrupt_handler */
static void dmac3_interrupt_handler(void)
{
    dmac_interrupt_handler(&gs_dmac3_resources);
}/* End of function dmac3_interrupt_handler() */

/******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_dmac)
 *********************************************************************************************************************/
