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
* Copyright (C) 2018-2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_flash.h
* Version      : 1.00
* Description  : This module provides the interface file to be included by the user when using the FLASH API
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 04.10.2018 0.51    First Release
*         : 29.11.2018 0.60    Version Update
*         : 22.07.2019 1.00    Change the product name to RE Family
*                              - Change the include file name
*                              - Update comments
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup grp_device_hal
 * @defgroup grp_device_hal_flash Device HAL FLASH Implementation
 * @brief The FLASH driver implementation for the Device HAL driver interface on RE01 Group.
 * @section
 * @code
 * @endcode
 * @section
 * @{
 **********************************************************************************************************************/

#ifndef R_FLASH_INTERFACE_HEADER_FILE
#define R_FLASH_INTERFACE_HEADER_FILE

#include "r_flash_cfg.h"
#include "r_flash_re01_1500kb.h"
#include "r_system_api.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/*! @name FLASH_VERSION
 * Driver Version Number. */
/* @{ */
#define FLASH_VERSION_MAJOR      (1)
#define FLASH_VERSION_MINOR      (00)
/* @} */

#define FLASH_HAS_ISR_CALLBACK   (1)
#define FLASH_HAS_ERR_ISR        (1)
#define FLASH_NO_DATA_FLASH      (1)

#if (FLASH_CFG_CODE_FLASH_ENABLE == 0)
#error "No data flash on this MCU. Set FLASH_CFG_CODE_FLASH_ENABLE to 1 in r_flash_cfg.h."
#endif

#define FLASH_NO_BLANK_CHECK                (1)
#define FLASH_ERASE_ASCENDING_BLOCK_NUMS    (1)
#define FLASH_HAS_BOOT_SWAP                 (1)
#define FLASH_HAS_CF_ACCESS_WINDOW          (1)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/** Return error codes */
typedef enum
{
    FLASH_SUCCESS = 0,      ///< None
    FLASH_ERR_BUSY,         ///< Peripheral Busy
    FLASH_ERR_ACCESSW,      ///< Access window error
    FLASH_ERR_FAILURE,      ///< Operation failure, programming or erasing error due to something other than lock bit
    FLASH_ERR_CMD_LOCKED,   ///< Peripheral in command locked state
    FLASH_ERR_LOCKBIT_SET,  ///< Pgm/Erase error due to lock bit. (UNSUPPORTED)
    FLASH_ERR_FREQUENCY,    ///< Illegal frequency value attempted
    FLASH_ERR_ALIGNED,      ///< The address not aligned correctly for CF or DF
    FLASH_ERR_BOUNDARY,     ///< Writes cannot cross the 1MB boundary on some parts
    FLASH_ERR_OVERFLOW,     ///< 'Address + number of bytes' for this operation went past the end of memory area.
    FLASH_ERR_BYTES,        ///< Invalid number of bytes passed
    FLASH_ERR_ADDRESS,      ///< Invalid address or address not on a programming boundary
    FLASH_ERR_BLOCKS,       ///< The 'number of blocks' argument is invalid
    FLASH_ERR_PARAM,        ///< Illegal parameter
    FLASH_ERR_NULL_PTR,     ///< Received null ptr; missing required argument
    FLASH_ERR_UNSUPPORTED,  ///< Command not supported for this flash type
    FLASH_ERR_SECURITY,     ///< Pgm/Erase error due to part locked (FAW.FSPR)
    FLASH_ERR_TIMEOUT,      ///< Timeout Condition
    FLASH_ERR_ALREADY_OPEN  ///< Open() called twice without intermediate Close()
} flash_err_t;


/** R_FLASH_Control() commands */
typedef enum
{
    FLASH_CMD_RESET,                ///< Kill any ongoing operation and reset the flash controller
    FLASH_CMD_STATUS_GET,           ///< Get flash status; returns FLASH_ERR_BUSY or FLASH_SUCCESS (idle)
    FLASH_CMD_SET_BGO_CALLBACK,     ///< Specify callback function for Flash ISR@n
                                    ///< Arg: flash_interrupt_config_t*
    FLASH_CMD_SWAPFLAG_GET,         ///< Get current selection for the start-up area used at reset@n
                                    ///< Arg: uint8_t* (FLASH_STARTUP_SETTING_xxx)
    FLASH_CMD_SWAPFLAG_TOGGLE,      ///< Swap the start-up area used at reset

    FLASH_CMD_SWAPSTATE_GET,        ///< Get the temporarily active start-up area@n
                                    ///< Arg: uint8_t* (FLASH_SAS_xxx)
    FLASH_CMD_SWAPSTATE_SET,        ///< Set the start-up area for temporary use@n
                                    ///< Arg: uint8_t* (FLASH_SAS_xxx)
    FLASH_CMD_ACCESSWINDOW_SET,     ///< Set the Access Window boundaries for CF@n
                                    ///< Arg: flash_access_window_config_t*
    FLASH_CMD_ACCESSWINDOW_GET,     ///< Get the Access Window boundaries for CF@n
                                    ///< Arg: flash_access_window_config_t*
    FLASH_CMD_CONFIG_CLOCK,         ///< Arg: uint32_t* (ICLK speed)
    FLASH_CMD_END_ENUM
} flash_cmd_t;


/** Result type for certain operations */
typedef enum
{
    FLASH_RES_INVALID,                      ///< Invalid condition
    FLASH_RES_LOCKBIT_STATE_PROTECTED,      ///< Result for FLASH_CMD_LOCKBIT_READ (UNSUPPORTED)
    FLASH_RES_LOCKBIT_STATE_NON_PROTECTED,  ///< Result for FLASH_CMD_LOCKBIT_READ (UNSUPPORTED)
    FLASH_RES_BLANK,                        ///< Result for Blank Check Function
    FLASH_RES_NOT_BLANK                     ///< Result for Blank Check Function
} flash_res_t;


typedef enum
{
    FLASH_TYPE_CODE_FLASH = 0,
    FLASH_TYPE_DATA_FLASH,
    FLASH_TYPE_INVALID
} flash_type_t;


/** Event type for the flash interrupt callback function (where available) */
typedef enum
{
    FLASH_INT_EVENT_INITIALIZED,
    FLASH_INT_EVENT_ERASE_COMPLETE,
    FLASH_INT_EVENT_WRITE_COMPLETE,
    FLASH_INT_EVENT_BLANK,
    FLASH_INT_EVENT_NOT_BLANK,
    FLASH_INT_EVENT_TOGGLE_STARTUPAREA,
    FLASH_INT_EVENT_SET_ACCESSWINDOW,
    FLASH_INT_EVENT_ERR_DF_ACCESS,
    FLASH_INT_EVENT_ERR_CF_ACCESS,
    FLASH_INT_EVENT_ERR_SECURITY,
    FLASH_INT_EVENT_ERR_CMD_LOCKED,
    FLASH_INT_EVENT_ERR_FAILURE,
    FLASH_INT_EVENT_END_ENUM
} flash_interrupt_event_t;


#ifdef FLASH_HAS_ISR_CALLBACK
/** Control() FLASH_CMD_SET_BGO_CALLBACK */
typedef struct
{
    void    (*pcallback)(void *);
    uint8_t int_priority;
} flash_interrupt_config_t;

/** Event type for ISR callback */
typedef struct
{
    flash_interrupt_event_t event;      ///< Which Flash event caused this interrupt
} flash_int_cb_args_t;

#endif


#ifdef FLASH_HAS_BOOT_SWAP
/*! @name FLASH_STARTUP_SETTING
 * Control() FLASH_CMD_SWAPFLAG_GET@n
 * Startup area select for reset
 */
/* @{ */
#define FLASH_STARTUP_SETTING_ALTERNATE     (0) ///< The start-up area is the alternate block (block 1)
#define FLASH_STARTUP_SETTING_DEFAULT       (1) ///< The start-up area is the default block (block 0)
/* @} */

/*! @name FLASH_STARTUP_AREA_SELECT
 * Control() FLASH_CMD_SWAPSTATE_GET/SET@n
 * Startup area select for temporary read/write
 */
/* @{ */
#define FLASH_SAS_SWAPFLG       (0)     ///< The start-up area temporarily set according to the swap flag
#define FLASH_SAS_EXTRA         (0)     ///< (same as above; here for backwards compatibility)
#define FLASH_SAS_DEFAULT       (2)     ///< The start-up area temporarily set to the default area
#define FLASH_SAS_ALTERNATE     (3)     ///< The start-up area temporarily set to the alternate area
#define FLASH_SAS_SWITCH_AREA   (4)     ///< Command to temporarily switch to the other startup area
/* @} */
#endif


#ifdef FLASH_HAS_CF_ACCESS_WINDOW
/** Control() FLASH_CMD_ACCESSWINDOW_SET/GET */
typedef struct
{
    uint32_t start_addr;                ///< start address of code flash Access Window
    uint32_t end_addr;                  ///< end address of code flash Access Window
} flash_access_window_config_t;
#endif

#include "r_flash_lowlevel.h"

typedef struct
{
    IRQn_Type               frdyi_irq;
    IRQn_Type               fiferr_irq;
    uint32_t                frdyi_iesr_val;
    uint32_t                fiferr_iesr_val;
    uint32_t                frdyi_priority;
    uint32_t                fiferr_priority;
    system_int_cb_t         frdyi_callback;
    system_int_cb_t         fiferr_callback;
} const st_flash_resources_t;

/***********************************************************************************************************************
Exported global variables
*******************************l****************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
#if (FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_FLASH_OPEN          __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_FLASH_OPEN
#endif
#if (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_FLASH_CLOSE         __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_FLASH_CLOSE
#endif
#if (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_FLASH_WRITE         __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_FLASH_WRITE
#endif
#if (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_FLASH_ERASE         __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_FLASH_ERASE
#endif
#if (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_FLASH_BLANKCHECK    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_FLASH_BLANKCHECK
#endif
#if (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_FLASH_CONTROL       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_FLASH_CONTROL
#endif
#if (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_FLASH_GETVERSION    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_FLASH_GETVERSION
#endif

flash_err_t R_FLASH_Open (void)                                               FUNC_LOCATION_R_FLASH_OPEN;
flash_err_t R_FLASH_Close (void)                                              FUNC_LOCATION_R_FLASH_CLOSE;
flash_err_t R_FLASH_Write_8Bytes (uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
                                                                              FUNC_LOCATION_R_FLASH_WRITE;
flash_err_t R_FLASH_Write_256Bytes (uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
                                                                              FUNC_LOCATION_R_FLASH_WRITE;
flash_err_t R_FLASH_Erase (uint32_t block_start_address, uint32_t num_blocks) FUNC_LOCATION_R_FLASH_ERASE;
flash_err_t R_FLASH_BlankCheck (uint32_t address, uint32_t num_bytes, flash_res_t *blank_check_result)
                                                                              FUNC_LOCATION_R_FLASH_BLANKCHECK;
flash_err_t R_FLASH_Control (flash_cmd_t cmd, void *pcfg)                     FUNC_LOCATION_R_FLASH_CONTROL;
uint32_t R_FLASH_GetVersion (void)                                            FUNC_LOCATION_R_FLASH_GETVERSION;

/*******************************************************************************************************************//**
 * @} (end defgroup grp_device_hal_flash)
 **********************************************************************************************************************/

#endif /* R_FLASH_INTERFACE_HEADER_FILE */
