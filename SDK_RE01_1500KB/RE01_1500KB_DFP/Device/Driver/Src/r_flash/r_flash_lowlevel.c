/********************************************************************************************************************
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
********************************************************************************************************************/
/*******************************************************************************************************************
* File Name    : r_flash_lowlevel.c
* Version      : 1.00
* Description  : This module implements the Flash API control commands for the RE01 Group(1500KB products)
********************************************************************************************************************/
/*******************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.10.2018 0.51    First Release
*         : 29.11.2018 0.60    +Update E017.h to version 0.7
*                              +Update MAX_ICLK_FREQ_HZ and MIN_ICLK_FREQ_HZ
*                              +Update Max Programming Time
*                              +Update Max Erasure Time
*         : 22.07.2019 1.00    Change the product name to RE Family
*                              - Change the include file names
*                              - Update comments
********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup grp_device_hal_flash
 * @{
 **********************************************************************************************************************/

/********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
********************************************************************************************************************/
#include "r_flash_lowlevel.h"
#include "RE01_1500KB.h"
#include "r_flash_re01_1500kb.h"
#include "r_flash_cfg.h"
#include "r_system_api.h"
#include "r_system_cfg.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct {
    union {
        __IOM  uint8_t  COM8;
    };
} FLASHACI_Type8;

typedef struct {
    union {
        __IOM  uint16_t  COM16;
    };
} FLASHACI_Type16;

/** Flash States */
typedef enum
{
    FLASH_UNINITIALIZED = 0,
    FLASH_INITIALIZATION,
    FLASH_READY,
    FLASH_WRITING,
    FLASH_ERASING,
    FLASH_BLANKCHECK,
    FLASH_GETSTATUS,
    FLASH_LOCK_BIT
} flash_states_t;

/** Flash current operation */
typedef enum
{
    FLASH_CUR_INVALID,                  ///< Invalid
    FLASH_CUR_IDLE,                     ///< Flash idle
    FLASH_CUR_CF_ERASE,                 ///< Code Flash Erasing
    FLASH_CUR_CF_WRITE,                 ///< Code Flash Writing
    FLASH_CUR_CF_BGO_WRITE,             ///< Code Flash Writing (BGO)
    FLASH_CUR_CF_BGO_ERASE,             ///< Code Flash Erasing (BGO)
    FLASH_CUR_DF_ERASE,                 ///< Data Flash Erasing
    FLASH_CUR_DF_BGO_ERASE,             ///< Data Flash Erasing (BGO)
    FLASH_CUR_DF_WRITE,                 ///< Data Flash Writing
    FLASH_CUR_DF_BLANKCHECK,            ///< Data Flash BlankCheck
    FLASH_CUR_DF_BGO_BLANKCHECK,        ///< Data Flash BlankCheck (BGO)
    FLASH_CUR_CF_BLANKCHECK,            ///< Code Flash BlankCheck
    FLASH_CUR_CF_TOGGLE_STARTUPAREA,    ///< Code Flash Toggle Startup Area
    FLASH_CUR_CF_ACCESSWINDOW,          ///< Code Flash Set Access Window
    FLASH_CUR_DF_BGO_WRITE,             ///< Data Flash Writing (BGO)
    FLASH_CUR_CF_BGO_READ_LOCKBIT,      ///< UNSUPPORTED
    FLASH_CUR_CF_BGO_WRITE_LOCKBIT,     ///< UNSUPPORTED
    FLASH_CUR_FCU_INIT,                 ///< Flash Initializing
    FLASH_CUR_LOCKBIT_SET,              ///< UNSUPPORTED
    FLASH_CUR_STOP
} flash_cur_t;

typedef struct
{
    uint32_t    src_addr;               ///< Source address for operation in progress
    uint32_t    dest_addr;              ///< Destination address for operation in progress
    uint32_t    total_count;            ///< Total number of bytes to write/erase
    uint32_t    current_count;          ///< Bytes of total completed
    flash_cur_t current_operation;      ///< Operation in progress, ie. FLASH_CUR_CF_ERASE
    uint16_t    min_pgm_size;           ///< As defined for the part, usually differs for CF and DF
    uint32_t    wait_cnt;               ///< Worst case wait time for operation completion
    bool        bgo_enabled_df;         ///< BGO enable for Data Flash
    bool        bgo_enabled_cf;         ///< BGO enable for Code Flash
} current_param_t;
/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
/*! @name REGISTER_VALUE
 * Registers value
 */
/* @{ */
#define FCU_FWEPROR_FLWE_PROHIBIT  (0x00)       ///< Prohibits programming and erasure of the code flash
#define FCU_FWEPROR_FLWE_PERMIT    (0x01)       ///< Permits programming and erasure of the code flash
#define FCU_FSUINITR_KEY           (0x2D00)     ///< Flash Sequencer Set-Up Initialization Register Key
#define FCU_FSUINITR_SUINIT_RETAIN (0x0000)     ///< Flash Sequencer Set-Up registers keep their current values
#define FCU_FSUINITR_SUINIT_INIT   (0x0001)     ///< Flash Sequencer Set-Up registers are initialized
#define FCU_FENTRYR_KEY            (0xAA00)     ///< Flash P/E Mode Entry Register Key
#define FCU_FENTRYR_READ           (0x0000)     ///< Read mode
#define FCU_FENTRYR_CODE           (0x0001)     ///< Code flash is in P/E mode
#define FCU_FENTRYR_DATA           (0x0080)     ///< Data flash is in P/E mode
#define FCU_FPCKAR_KEY             (0x1E00)     ///< Flash Sequencer Processing Clock Notification Register Key
#define FCU_FSUACR_KEY             (0x6600)     ///< Flash Start-Up Area Control Register Key
/* @} */

/*! @name EVENT_LINK_SETTING
 * Event Link Setting Register Value
 */
/* @{ */
#define FCU_FIFERR_IELS            (0x03)
#define FCU_FRDYI_IELS             (0x03)
/* @} */

/*! @name FACI_COMMAND_LIST
 * FACI Commands
 */
/* @{ */
#define FCU_FACI_PROGRAM_CMD1             (0xE8)
#define FCU_FACI_PROGRAM_CMD2             (0xD0)
#define FCU_FACI_BLOCK_ERASE_CMD1         (0x20)
#define FCU_FACI_BLOCK_ERASE_CMD2         (0xD0)
#define FCU_FACI_SUSPEND_CMD              (0xB0)
#define FCU_FACI_RESUME_CMD               (0xD0)
#define FCU_FACI_STATUS_CLEAR_CMD         (0x50)
#define FCU_FACI_FORCED_STOP_CMD          (0xB3)
#define FCU_FACI_BLANK_CHECK_CMD1         (0x71)
#define FCU_FACI_BLANK_CHECK_CMD2         (0xD0)
#define FCU_FACI_CONFIG_CMD1              (0x40)
#define FCU_FACI_CONFIG_CMD2              (0x08)
#define FCU_FACI_CONFIG_CMD3              (0xD0)
#define FCU_FACI_LOCK_BIT_PROGRAM_CMD1    (0x77)
#define FCU_FACI_LOCK_BIT_PROGRAM_CMD2    (0xD0)
#define FCU_FACI_LOCK_BIT_READ_CMD1       (0x71)
#define FCU_FACI_LOCK_BIT_READ_CMD2       (0xD0)
/* @} */
 
#define FACI_COMMAND                      (0x407E0000UL)  ///< FACI command-issuing area
#define FCU_CONFIG_REG_ADDR               (0x0000A160UL)  ///< Address Used by Configuration Set Command for FAW

#define FLASH_FENTRYR_TIMEOUT   (2)       ///< The number of loops to wait for FENTRYR timeout.

#define FLASH_FRDY_CMD_TIMEOUT  (50000)   ///< The maximum timeout for commands (not in spec yet)

#define FLASH_FCU_INT_ENABLE    FLASH->FAEINT = 0x90;   \
                                FLASH->FRDYIE = 0x01;   ///< Enable FCU interrupts
#define FLASH_FCU_INT_DISABLE   FLASH->FAEINT = 0x00;   \
                                FLASH->FRDYIE = 0x00;   ///< Disable FCU interrupts

#define FACI   ((FLASHACI_Type8*)  FACI_COMMAND)
#define FACID  ((FLASHACI_Type16*) FACI_COMMAND)

#define MAX_ICLK_FREQ_HZ (32000000)
#define MIN_ICLK_FREQ_HZ (1000000)

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Structure that holds the parameters for current operations */
current_param_t g_current_parameters = {
                                         0,     /*!< Source Address */
                                         0,     /*!< Destination Address */
                                         0,     /*!< Total Count */
                                         0,     /*!< Current Count */
                                         FLASH_CUR_INVALID, /*!< Current Operation */
                                         0,     /*!< Minimum Program Size */
                                         0,      /*!< Wait Count for current operation */
                                         false, /*!< DF BGO Disabled */
                                         false, /*!< CF BGO Disabled */
                                        };                 ///< Structure that holds the parameters for current operations

static flash_states_t g_flash_state = FLASH_UNINITIALIZED; ///< Flash state
static uint32_t g_iclk_hz;                                 ///< ICLK (Hz)
static int32_t g_flash_lock;                               ///< Flash Lock Flag
static bool    g_driver_opened = false;                    ///< Flash Driver Opened Flag

/* Internal functions. */
void (*flash_ready_isr_handler)(void *) = FLASH_NO_FUNC;                   ///< Function pointer for Flash Ready ISR
void (*flash_error_isr_handler)(void *) = FLASH_NO_FUNC;                   ///< Function pointer for Flash Error ISR

flash_int_cb_args_t g_flash_int_ready_cb_args;             ///< Callback argument structure for flash READY interrupt
flash_int_cb_args_t g_flash_int_error_cb_args;             ///< Callback argument structure for flash ERROR interrupt

#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_INIT  __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_INIT
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_CLOCKCONFIG __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_CLOCKCONFIG
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_RESET  __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_RESET
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_STOP  __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_STOP
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_INTERRUPTCONFIG __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_INTERRUPTCONFIG
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_WRITEFAWREG __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_WRITEFAWREG
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_PEMODEENTER __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_PEMODEENTER
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_PEMODEEXIT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_PEMODEEXIT
#endif
#if (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_R_FLASH_ERASE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_ERASE
#endif
#if (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_R_FLASH_WRITE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_WRITE
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_LOCKSTATE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_LOCKSTATE
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_GETSTATUS __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_GETSTATUS
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_RELEASESTATE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_RELEASESTATE
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_SWAPFUN __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_SWAPFUN
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_SOFTWARELOCK __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_SOFTWARELOCK
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_SOFTWAREUNLOCK __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_SOFTWAREUNLOCK
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_TOGGLESTARTUPAREA __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_TOGGLESTARTUPAREA
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_GETCURRENTSTARTUPAREA __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_GETCURRENTSTARTUPAREA
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_GETCURRENTSWAPSTATE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_GETCURRENTSWAPSTATE
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_SETCURRENTSWAPSTATE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_SETCURRENTSWAPSTATE
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_SETACCESSWINDOW __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_SETACCESSWINDOW
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_GETACCESSWINDOW __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_GETACCESSWINDOW
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_READYISR  __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_READYISR
#endif
#if ((FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC) ||\
    (FLASH_CFG_SECTION_R_FLASH_GETVERSION == SYSTEM_SECTION_RAM_FUNC))
#define STATIC_FUNC_LOCATION_R_FLASH_ERRORISR  __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_R_FLASH_ERRORISR
#endif

static flash_err_t flash_init () STATIC_FUNC_LOCATION_R_FLASH_INIT;
static flash_err_t flash_clock_config (uint32_t) STATIC_FUNC_LOCATION_R_FLASH_CLOCKCONFIG;
static flash_err_t flash_reset () STATIC_FUNC_LOCATION_R_FLASH_RESET;
static flash_err_t flash_stop () STATIC_FUNC_LOCATION_R_FLASH_STOP;
static flash_err_t flash_interrupt_config (bool state, void *pCallback) STATIC_FUNC_LOCATION_R_FLASH_INTERRUPTCONFIG;

static flash_err_t flash_write_faw_reg (fawreg_t faw) STATIC_FUNC_LOCATION_R_FLASH_WRITEFAWREG;
static flash_err_t flash_pe_mode_enter (flash_type_t flash_type) STATIC_FUNC_LOCATION_R_FLASH_PEMODEENTER;
static flash_err_t flash_pe_mode_exit () STATIC_FUNC_LOCATION_R_FLASH_PEMODEEXIT;
static flash_err_t flash_erase (uint32_t block_address, uint32_t block_count) STATIC_FUNC_LOCATION_R_FLASH_ERASE;
static flash_err_t flash_write (uint32_t *src_start_address, uint32_t * dest_start_address, uint32_t *num_bytes,
        flash_program_mode_t pgm_mode) STATIC_FUNC_LOCATION_R_FLASH_WRITE;
static flash_err_t flash_lock_state (flash_states_t new_state) STATIC_FUNC_LOCATION_R_FLASH_LOCKSTATE;
static flash_err_t flash_get_status (void) STATIC_FUNC_LOCATION_R_FLASH_GETSTATUS;
static void flash_release_state (void) STATIC_FUNC_LOCATION_R_FLASH_RELEASESTATE;
static void swapfun (int32_t * restrict fp, int32_t * restrict sp) STATIC_FUNC_LOCATION_R_FLASH_SWAPFUN;
static bool flash_software_lock (int32_t * const plock) STATIC_FUNC_LOCATION_R_FLASH_SOFTWARELOCK;
static bool flash_software_unlock (int32_t * const plock) STATIC_FUNC_LOCATION_R_FLASH_SOFTWAREUNLOCK;
static flash_err_t flash_toggle_startup_area (void) STATIC_FUNC_LOCATION_R_FLASH_TOGGLESTARTUPAREA;
static uint8_t flash_get_current_startup_area (void) STATIC_FUNC_LOCATION_R_FLASH_GETCURRENTSTARTUPAREA;
static uint8_t flash_get_current_swap_state (void) STATIC_FUNC_LOCATION_R_FLASH_GETCURRENTSWAPSTATE;
static void flash_set_current_swap_state (uint8_t value) STATIC_FUNC_LOCATION_R_FLASH_SETCURRENTSWAPSTATE;
#ifndef CHIP_VERSION_WS1
static flash_err_t flash_set_access_window (flash_access_window_config_t *pAccessInfo)
                   STATIC_FUNC_LOCATION_R_FLASH_SETACCESSWINDOW;
#endif
static flash_err_t flash_get_access_window (flash_access_window_config_t *pAccessInfo)
                   STATIC_FUNC_LOCATION_R_FLASH_GETACCESSWINDOW;
static void flash_ready_isr (void) STATIC_FUNC_LOCATION_R_FLASH_READYISR;
static void flash_error_isr (void) STATIC_FUNC_LOCATION_R_FLASH_ERRORISR;

/*******************************************************************************************************************//**
 * @brief                         Function will initialize the flash.
 * @retval FLASH_SUCCESS          API initialized successfully.
 * @retval FLASH_ERR_BUSY         Another flash operation in progress, try again later.
 * @retval FLASH_ERR_ALREADY_OPEN Open() has been called twice without an intermediate Close().
 **********************************************************************************************************************/
flash_err_t flash_api_open (void)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Allow Initialization if not initialized or
     * if no operation is ongoing and re-initialization is desired */
    if ((FLASH_UNINITIALIZED == g_flash_state) || (FLASH_READY == g_flash_state))
    {
        if (FLASH_SUCCESS != flash_lock_state(FLASH_INITIALIZATION))
        {
            /* Another operation is on-going */
            return FLASH_ERR_BUSY;          // The program never enter to this path. 
        }
    }

    if (g_driver_opened == true)
    {
        /* API already initialized */
        flash_release_state();
        return FLASH_ERR_ALREADY_OPEN;
    }

    /*Initialize the FCU*/
    err = flash_init();
    if (FLASH_SUCCESS != err)
    {
        return err;             // The program never enter to this path. 
    }

    g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_INITIALIZED;

    /* Set the parameters struct based on the config file settings */
    g_current_parameters.bgo_enabled_cf = FLASH_CFG_CODE_FLASH_BGO;

    /*Interrupts are enabled by default on this MCU.
     * Disable Interrupts*/
    flash_interrupt_config(false, 0);
    /* Release state so other operations can be performed. */
    flash_release_state();
    g_driver_opened = true;
    /* Flash is ready */
    g_flash_state = FLASH_READY;
    return FLASH_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                 Function disables flash interrupts and marks driver as closed.
 * @retval FLASH_SUCCESS  API closed successfully.
 * @retval FLASH_ERR_BUSY Another operation is ongoing.
 **********************************************************************************************************************/
flash_err_t flash_api_close(void)
{
    /* Lock driver */
    if (FLASH_SUCCESS != flash_lock_state(FLASH_UNINITIALIZED))
    {
        return FLASH_ERR_BUSY;
    }

    /* Disable interrupts */
    flash_interrupt_config(false, NULL);

    /* Show driver as closed and release hold on lock.
     * Do not use flash_release_state() because we do not want to set state to FLASH_READY.
     */
    g_driver_opened = false;
    flash_software_unlock(&g_flash_lock);

    return FLASH_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                       Function will write to the specified Code Flash memory area.
 *                              Note that this function does not insure that the block(s) being written are blank.
 * @param[in] src_address       Source buffer address (data being written to Flash)
 * @param[in] dest_address      Destination address.
 * @param[in] num_bytes         Number of bytes to be written.
 *                              It must be multiple of the minimum programming size (8 bytes or 256 bytes).
 * @param[in] pgm_mode          Programming mode (8 bytes or 256 bytes).
 * @retval FLASH_SUCCESS        Write completed successfully; successfully initialized in case of BGO mode.
 * @retval FLASH_ERR_BYTES      Number of bytes exceeds max range or is 0 or is not a valid multiple of the minimum
 *                              programming size for the specified flash
 * @retval FLASH_ERR_OVERFLOW   Address + number of bytes for this operation went past the end of memory 
 * @retval FLASH_ERR_ADDRESS    src/dest address is an invalid Code Flash block start address or
 *                              address is not aligned with the minimum programming size for the Code Flash.
 *                              The src address HAS to be a RAM address since ROM cannot be accessed during Code Flash
 *                              programming
 * @retval FLASH_ERR_BUSY       Flash peripheral is busy with another operation or not initialized
 * @retval FLASH_ERR_CMD_LOCKED The FCU entered a command locked state and cannot process the operation.
 *                              A RESET was performed on the FCU to recover from this state.
 * @retval FLASH_ERR_FAILURE    Flash Write operation failed for some other reason. This can be a result of trying to
 *                              write to an area that has been protected via access control.
 **********************************************************************************************************************/
flash_err_t flash_api_write (uint32_t src_address, uint32_t dest_address, uint32_t num_bytes,
        flash_program_mode_t pgm_mode)
{
    flash_err_t err = FLASH_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((FLASH_8_BYTES_PGM != pgm_mode) && (FLASH_256_BYTES_PGM != pgm_mode))
    {
        return FLASH_ERR_FAILURE; // The program never enter to this path. 
    }
    if ((num_bytes == 0) || ((FLASH_8_BYTES_PGM == pgm_mode) && (num_bytes % FLASH_CF_MIN_PGM_SIZE_8_BYTES != 0))
            || ((FLASH_256_BYTES_PGM == pgm_mode) && (num_bytes % FLASH_CF_MIN_PGM_SIZE_256_BYTES != 0)))
    {
        return FLASH_ERR_BYTES;
    }

    if (((FLASH_8_BYTES_PGM == pgm_mode) && (dest_address % FLASH_CF_MIN_PGM_SIZE_8_BYTES != 0))
            || ((FLASH_256_BYTES_PGM == pgm_mode) && (dest_address % FLASH_CF_MIN_PGM_SIZE_256_BYTES != 0)))
    {
        return FLASH_ERR_ADDRESS;
    }

    if (dest_address + num_bytes > FLASH_NUM_BLOCKS_CF * FLASH_CF_BLOCK_SIZE)
    {
        return FLASH_ERR_OVERFLOW;
    }
#endif

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    /*Check if callback set*/
    if (FLASH_NO_FUNC == flash_ready_isr_handler || NULL == flash_ready_isr_handler)
    {
        return FLASH_ERR_FAILURE;
    }
#endif

    /*Check if API is busy*/
    if (g_flash_state != FLASH_READY)
    {
        /* API not initialized or busy with another operation*/
        return FLASH_ERR_BUSY;
    }
    /*Grab the current flash state*/
    if (FLASH_SUCCESS != flash_lock_state(FLASH_WRITING))
    {
        /* API busy with another operation*/
        return FLASH_ERR_BUSY;          // The program never enter to this path. 
    }

    /*No errors in parameters. Enter Code Flash PE mode*/
    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        /*If there is an error, then reset the FCU: This will clear error flags and exit the P/E mode*/
        flash_reset();
        /*Release the lock*/
        flash_release_state();
        return err;
    }

    if (g_current_parameters.bgo_enabled_cf == true)
    {
        g_current_parameters.current_operation = FLASH_CUR_CF_BGO_WRITE;
    }
    else
    {
        g_current_parameters.current_operation = FLASH_CUR_CF_WRITE;
    }

    /*Write the data*/
    err = flash_write(&src_address, &dest_address, &num_bytes, pgm_mode);
    if (FLASH_SUCCESS != err)
    {
        /*If there is an error, then reset the FCU: This will clear error flags and exit the P/E mode*/
        flash_reset();
        /*Release the lock*/
        flash_release_state();
        return err;
    }

    /*If in non-BGO mode, then current operation is completed.
     * Exit from PE mode and return status*/
    if ((g_current_parameters.current_operation == FLASH_CUR_CF_WRITE)
            || (g_current_parameters.current_operation == FLASH_CUR_DF_WRITE))
    {
        /*Return to read mode*/
        err = flash_pe_mode_exit();
        if (FLASH_SUCCESS != err)
        {
            /*Reset the FCU: This will stop any existing processes and exit PE mode*/
            flash_reset();
        }
        /*Release lock and reset the state to idle*/
        flash_release_state();
    }

    /*If BGO is enabled, then
     *return result of started Write process*/
    return err;
}

/*******************************************************************************************************************//**
 * @brief                         Function will erase the specified Code Flash blocks.
 * @param[in] block_start_address Start address of the first block.
 * @param[in] num_blocks          Number of blocks to erase.
 * @retval FLASH_SUCCESS          Erase completed successfully; successfully initialized in case of BGO mode.
 * @retval FLASH_ERR_BLOCKS       Number of blocks exceeds max range or is 0
 * @retval FLASH_ERR_OVERFLOW     Address + number of bytes for this operation went past the end of memory area
 * @retval FLASH_ERR_ADDRESS      Start address is an invalid Code Flash block start address
 * @retval FLASH_ERR_BUSY         Flash peripheral is busy with another operation
 * @retval FLASH_ERR_FAILURE      Flash Write operation failed for some other reason. This can be a result of trying
 *                                to erase an area that has been protected via access control.
 **********************************************************************************************************************/
flash_err_t flash_api_erase (uint32_t block_start_address, uint32_t num_blocks)
{
    flash_err_t err = FLASH_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((num_blocks == 0) || (num_blocks > FLASH_NUM_BLOCKS_CF))
    {
        return FLASH_ERR_BLOCKS;
    }

    if (block_start_address % FLASH_CF_BLOCK_SIZE != 0)
    {
        return FLASH_ERR_ADDRESS;
    }

    if ((block_start_address/FLASH_CF_BLOCK_SIZE) + num_blocks > FLASH_NUM_BLOCKS_CF)
    {
        return FLASH_ERR_OVERFLOW;
    }
#endif

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    /*Check if callback set*/
    if (FLASH_NO_FUNC == flash_ready_isr_handler || NULL == flash_ready_isr_handler) {
        return FLASH_ERR_FAILURE;
    }
#endif

    /*Check if API is busy*/
    if (g_flash_state != FLASH_READY)
    {
        /* API not initialized or busy with another operation*/
        return FLASH_ERR_BUSY;
    }

    /*Grab the current flash state*/
    if (FLASH_SUCCESS != flash_lock_state(FLASH_ERASING))
    {
        /* API busy with another operation*/
        return FLASH_ERR_BUSY;          // The program never enter to this path. 
    }

    /*Check address and num_blocks validity and switch to Code Flash P/E mode*/
    if (block_start_address < FLASH_ROM_SIZE_BYTES) /* Flash memory 1.5 MB: 0x00000000 - 0x0017FFFF */
    {
        if ((num_blocks > FLASH_NUM_BLOCKS_CF) || (num_blocks <= 0))
        {
            err = FLASH_ERR_BLOCKS;
        }
        else
        {
            /*No errors in parameters. Enter Code Flash PE mode*/
            err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
            if (g_current_parameters.bgo_enabled_cf == true)
            {
                g_current_parameters.current_operation = FLASH_CUR_CF_BGO_ERASE;
            }
            else
            {
                g_current_parameters.current_operation = FLASH_CUR_CF_ERASE;
            }
        }
    }
    else
    {
        err = FLASH_ERR_ADDRESS;
    }

    if (FLASH_SUCCESS != err)
    {
        flash_release_state();
        return err;
    }

    /* Access Window protection not checked here
     * User Should configure it before calling function*/

    /*Erase the Blocks*/
    err = flash_erase((uint32_t)block_start_address, num_blocks);
    if (FLASH_SUCCESS != err)
    {
        /*If there is an error, then reset the FCU: This will clear error flags and exit the P/E mode*/
        flash_reset();
        flash_release_state();
        return err;
    }

    /*If in non-BGO mode, then current operation is completed.
     * Exit from PE mode and return status*/
    if ((g_current_parameters.current_operation == FLASH_CUR_CF_ERASE)
            || (g_current_parameters.current_operation == FLASH_CUR_DF_ERASE))
    {
        /*Return to read mode*/
        err = flash_pe_mode_exit();
        if (FLASH_SUCCESS != err)
        {
            /*Reset the FCU: This will stop any existing processes and exit PE mode*/
            flash_reset();
        }
        /*Release lock and reset the state to Idle*/
        flash_release_state();
    }
    /*If BGO is enabled, then return result of started Erase process*/
    return err;
}

/*******************************************************************************************************************//**
 * @brief                        This function supports additional configuration operations.
 *                               The supported commands are listed in the flash_cmd_t enum.
 * @param[in] cmd                command.
 * @param[in] pcfg               Pointer to configuration.
 *                               This argument can be NULL for commands that do not require a configuration.@n
 *                               Command                                | Argument
 *                               -------------------------------------- | --------
 *                               FLASH_CMD_RESET                        | NULL
 *                               FLASH_CMD_STATUS_GET                   | NULL
 *                               FLASH_CMD_SET_BGO_CALLBACK             | flash_interrupt_config_t *
 *                               FLASH_CMD_SWAPFLAG_TOGGLE              | NULL
 *                               FLASH_CMD_SWAPFLAG_GET                 | uint8_t *
 *                               FLASH_CMD_SWAPSTATE_GET                | uint8_t *
 *                               FLASH_CMD_SWAPSTATE_SET                | uint8_t *
 *                               FLASH_CMD_ACCESSWINDOW_SET             | flash_access_window_config_t *
 *                               FLASH_CMD_ACCESSWINDOW_GET             | flash_access_window_config_t *
 *                               FLASH_CMD_CONFIG_CLOCK                 | uint32_t *
 * @retval FLASH_SUCCESS         Operation was completed successfully
 * @retval FLASH_ERR_FAILURE     Callback function not set or Hardware operation failed
 * @retval FLASH_ERR_ADRRESS     Address is an invalid Code Flash block start address
 * @retval FLASH_ERR_BUSY        Flash peripheral is busy with another operation or not initialized
 * @retval FLASH_ERR_NULL_PTR    Pointer was NULL for a command that expects a configuration structure
 * @retval FLASH_ERR_CMD_LOCKED  The FCU was in a command locked state and has been reset
 * @retval FLASH_ERR_ACCESSW     Access window error: Incorrect area specified
 * @retval FLASH_ERR_FREQUENCY   Illegal Frequency parameter passed for FLASH_CMD_CONFIG_CLOCK command
 * @retval FLASH_ERR_UNSUPPORTED Unrecognized commands
 **********************************************************************************************************************/
flash_err_t flash_api_control (flash_cmd_t cmd, void *pcfg)
{
    flash_err_t err = FLASH_SUCCESS;
    uint8_t *pSwapInfo = pcfg;
    flash_access_window_config_t *pAccessInfo = pcfg;
    uint32_t *plocal_pcfg;

    /*If the command is to reset the FCU, then no attempt is made to grab the lock
     * before executing the reset since the assumption is that the RESET command
     * is used to terminate any existing operation*/
    switch (cmd)
    {
        case FLASH_CMD_RESET:
            err = flash_reset();
            flash_release_state();
            return err;
    }

    /*Check if API is busy*/
    if (g_flash_state != FLASH_READY)
    {
        /* API not initialized or busy with another operation*/
        return FLASH_ERR_BUSY;
    }

    switch (cmd)
    {
        /* If Code Flash BGO is enabled, then allow user to set a callback isr */
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
        case FLASH_CMD_SET_BGO_CALLBACK:
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
            if ((pcfg == NULL) || (pcfg == FLASH_NO_PTR))
            {
                return FLASH_ERR_NULL_PTR;
            }
#endif
            err = flash_interrupt_config(true, pcfg);
        break;
#endif

        case FLASH_CMD_SWAPFLAG_TOGGLE: /* Inverts the start-up program swap flag */
#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1))
            if ((FLASH_NO_FUNC == flash_ready_isr_handler))
            {
                return FLASH_ERR_FAILURE;
            }
#endif
            /*Grab the current flash state*/
            if (FLASH_SUCCESS != flash_lock_state(FLASH_WRITING))
            {
                /* API busy with another operation*/
                return FLASH_ERR_BUSY;      // The program never enter to this path.     
            }
            err = flash_toggle_startup_area();
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
            flash_release_state();
#endif
        break;

        case FLASH_CMD_SWAPFLAG_GET: /* Returns the the current value of the start-up program swap flag. */
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
            if ((!pSwapInfo) || (pSwapInfo == FLASH_NO_PTR))
            {
                return FLASH_ERR_NULL_PTR;
            }
#endif
            *pSwapInfo = flash_get_current_startup_area();
        break;

        case FLASH_CMD_SWAPSTATE_GET:
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
            if ((!pSwapInfo) || (pSwapInfo == FLASH_NO_PTR))
            {
                return FLASH_ERR_NULL_PTR;
            }
#endif
            /* Retrieves the current physical state of the start-up program area(which one is active).. */
            *pSwapInfo = flash_get_current_swap_state();
        break;

        case FLASH_CMD_SWAPSTATE_SET:
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
            if ((!pSwapInfo) || (pSwapInfo == FLASH_NO_PTR))
            {
                return FLASH_ERR_NULL_PTR;
            }

            if (*pSwapInfo != FLASH_SAS_EXTRA && *pSwapInfo != FLASH_SAS_DEFAULT && *pSwapInfo != FLASH_SAS_ALTERNATE
                    && *pSwapInfo != FLASH_SAS_SWITCH_AREA)
            {
                return FLASH_ERR_PARAM;
            }
#endif
            /* Swaps the start-up program areas without setting the start-up program swap flag. */
            flash_set_current_swap_state(*pSwapInfo);
        break;

        case FLASH_CMD_ACCESSWINDOW_SET:
#ifdef CHIP_VERSION_WS1 // WS1 limitation
            return FLASH_ERR_UNSUPPORTED;
#else
            /* Works on full blocks only. Provide code block numbers or any address within a block */
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
            if ((pAccessInfo == 0) || (pAccessInfo == FLASH_NO_PTR))
            {
                return FLASH_ERR_NULL_PTR;
            }

            if ((pAccessInfo->start_addr > FLASH_ROM_SIZE_BYTES || pAccessInfo->end_addr > FLASH_ROM_SIZE_BYTES))
            {
                return (FLASH_ERR_ACCESSW);
            }
#endif
#if ((FLASH_CFG_CODE_FLASH_ENABLE == 1) && (FLASH_CFG_CODE_FLASH_BGO == 1))
            if ((FLASH_NO_FUNC == flash_ready_isr_handler))
            {
                return FLASH_ERR_FAILURE;
            }
#endif
            /*Grab the current flash state*/
            if (FLASH_SUCCESS != flash_lock_state(FLASH_WRITING))
            {
                /* API busy with another operation*/
                return FLASH_ERR_BUSY;      // The program never enter to this path. 
            }

            err = flash_set_access_window(pAccessInfo);
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
            flash_release_state();
#endif
#endif /* CHIP_VERSION_WS1 */
        break;

        case FLASH_CMD_ACCESSWINDOW_GET:
            if ((pAccessInfo == 0) || (pAccessInfo == FLASH_NO_PTR))
            {
                return FLASH_ERR_NULL_PTR;
            }
            err = flash_get_access_window(pAccessInfo);
        break;

        case FLASH_CMD_STATUS_GET:
            err = flash_get_status();
        break;

        case FLASH_CMD_CONFIG_CLOCK:
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
            if ((pcfg == NULL) || (pcfg == FLASH_NO_PTR))
            {
                return FLASH_ERR_NULL_PTR;
            }
#endif
            plocal_pcfg = pcfg;
            if ((MIN_ICLK_FREQ_HZ <= *plocal_pcfg) && (*plocal_pcfg <= MAX_ICLK_FREQ_HZ))
            {
                err = flash_clock_config(*plocal_pcfg);
            }
            else
            {
                err = FLASH_ERR_FREQUENCY;
            }
        break;

        default:
            err = FLASH_ERR_UNSUPPORTED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief                    Function will initialize the FCU and set the FCU Clock based on the current ICLK frequency
 * @retval FLASH_SUCCESS     Peripheral Initialized successfully.
 **********************************************************************************************************************/
static flash_err_t flash_init ()
{
    flash_err_t err = FLASH_SUCCESS;

    g_current_parameters.current_operation = FLASH_CUR_FCU_INIT;
    /*Allow Access to the Flash registers*/
    FLASH->FWEPROR = FCU_FWEPROR_FLWE_PERMIT;

    /*Set the Clock*/
    R_SYS_SystemClockFreqGet(&g_iclk_hz);
    flash_clock_config(g_iclk_hz);
    return err;
}

/*******************************************************************************************************************//**
 * @brief                     Function to enable or disable flash interrupts.
 * @param[in] state           Enable/Disable Interrupts
 * @param[in] pcfg            Callback function that needs to be called from the ISR in case of an error or after
 *                            operation is complete.
 *                            This argument can be a NULL if interrupts are being disabled (non-BGO mode)
 * @retval FLASH_SUCCESS      Interrupts configured successfully.
 * @retval FLASH_ERR_PARAM    Illegal parameter
 * @retval FLASH_ERR_NULL_PTR Invalid callback function pointer passed when enabling interrupts
 **********************************************************************************************************************/
static flash_err_t flash_interrupt_config (bool state, void *pcfg)
{
    flash_interrupt_config_t *int_cfg = pcfg;

    if (true == state)
    {
#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
        if ((int_cfg->int_priority < 1) || (int_cfg->int_priority > 15))
        {
            return FLASH_ERR_PARAM;
        }

        if ((NULL == int_cfg->pcallback) || (FLASH_NO_FUNC == int_cfg->pcallback))
        {
            return FLASH_ERR_NULL_PTR;
        }
#endif
        /* Assign the callback if not NULL*/
        flash_ready_isr_handler = int_cfg->pcallback;
        flash_error_isr_handler = int_cfg->pcallback;

        /* Enable interrupts in FCU */
        FLASH_FCU_INT_ENABLE;

        /* Clear Flash Ready Interrupt Request */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI);
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI);

        /* Setup IRQ Event Link for FCU_FRDYI */
        if (-1 == R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI, FCU_FRDYI_IELS, flash_ready_isr))
        {
            return FLASH_ERR_PARAM;         // The program never enter to this path. 
        }
        /* Set Flash Ready Interrupt Priority */
        R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI, int_cfg->int_priority);
        if (int_cfg->int_priority != R_NVIC_GetPriority(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI))
        {
            return FLASH_ERR_PARAM;         // The program never enter to this path. 
        }
        /* Enable Flash Ready Interrupt */
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI);

#ifdef FLASH_HAS_ERR_ISR
        /* Clear Flash Error Interrupt Request */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR);
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR);

        /* Setup IRQ Event Link for FCU_FIFERR */
        if (-1 == R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR, FCU_FIFERR_IELS, flash_error_isr))
        {
            return FLASH_ERR_PARAM;         // The program never enter to this path. 
        }
        /* Set Flash Error Interrupt Priority */
        R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR, int_cfg->int_priority);
        if (int_cfg->int_priority != R_NVIC_GetPriority(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR))
        {
            return FLASH_ERR_PARAM;         // The program never enter to this path. 
        }
        /* Enable Flash Error Interrupt */
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR);
#endif
    }
    else if (false == state)
    {
        /* Disable Flash FCU interrupt */
        FLASH_FCU_INT_DISABLE;

        /* Clear any pending Flash Ready Interrupt Request */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI);
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI);

#ifdef FLASH_HAS_ERR_ISR
        /* Clear any pending Flash Error Interrupt Request */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR);
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR);
#endif
    }
    else
    {
        return FLASH_ERR_PARAM;         // The program never enter to this path. 
    }

    return FLASH_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                         Function sets the internal FCU register to the current ICLK frequency
 * @param[in] cur_fclk            Specify current ICLK frequency
 * @retval FLASH_SUCCESS          Operation successfully.
 **********************************************************************************************************************/
static flash_err_t flash_clock_config (uint32_t cur_fclk)
{
    uint32_t speed_mhz = 0;
    /* Convert clock from Hz to MHz and round it up to a whole number */
    speed_mhz = cur_fclk / 1000000;
    if ((cur_fclk % 1000000) != 0)
    {
        speed_mhz++;    // must round up to nearest MHz
    }
    /* Set FCU clock */
    FLASH->FPCKAR = (uint16_t)(FCU_FPCKAR_KEY) + (uint16_t)speed_mhz;

    return FLASH_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                         Function switches the peripheral to P/E mode for Code Flash
 * @param[in] flash_type          Specify Code Flash or Data Flash. Only support Code Flash.
 * @retval FLASH_SUCCESS          Switched successfully.
 * @retval FLASH_ERR_PARAM        Illegal parameter passed
 * @retval FLASH_ERR_FAILURE      Unable to Switch to P/E Mode.
 **********************************************************************************************************************/
static flash_err_t flash_pe_mode_enter (flash_type_t flash_type)
{
    int count = 0;
    flash_err_t err = FLASH_SUCCESS;

    if (flash_type == FLASH_TYPE_DATA_FLASH)
    {
        // The program never enter to this path
        FLASH->FENTRYR = FCU_FENTRYR_KEY | FCU_FENTRYR_DATA;        //Transition to DF P/E mode
        if (FLASH->FENTRYR == FCU_FENTRYR_DATA)
        {
            err = FLASH_SUCCESS;
        }
        else
        {
            err = FLASH_ERR_FAILURE;
        }
    }
    else if (flash_type == FLASH_TYPE_CODE_FLASH)
    {
        FLASH->FENTRYR = FCU_FENTRYR_KEY | FCU_FENTRYR_CODE;        //Transition to CF P/E mode
        while ((FCU_FENTRYR_CODE != FLASH->FENTRYR) && (count++ < FLASH_FENTRYR_TIMEOUT))
        {
            /* Do nothing */
        }

        if (FCU_FENTRYR_CODE == FLASH->FENTRYR)
        {
            err = FLASH_SUCCESS;
        }
        else
        {
            err = FLASH_ERR_FAILURE;
        }
    }
    else
    {
        err = FLASH_ERR_PARAM;              // The program never enter to this path. 
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief                         Function switches the peripheral from P/E mode for Code Flash to Read mode
 * @retval FLASH_SUCCESS          Switched successfully.
 * @retval FLASH_ERR_TIMEOUT      Operation timed out. Ongoing flash operation failed.
 * @retval FLASH_ERR_CMD_LOCKED   Peripheral in locked state. Operation failed.
 * @retval FLASH_ERR_FAILURE      Operation failed for some other reason.
 **********************************************************************************************************************/
static flash_err_t flash_pe_mode_exit ()
{
    int count = 0;
    flash_err_t err = FLASH_SUCCESS;
    flash_err_t temp_err = FLASH_SUCCESS;
    /* Timeout counter. */
    volatile uint32_t wait_cnt = FLASH_FRDY_CMD_TIMEOUT;

    /* Read FRDY bit until it has been set to 1 indicating that the current
     * operation is complete.*/
    while (1 != FLASH->FSTATR_b.FRDY)
    {
        /* Wait until FRDY is 1 unless timeout occurs. */
        if (wait_cnt-- <= 0)
        {
            /* if FRDY is not set to 1 after max timeout, return timeout status*/
            return FLASH_ERR_TIMEOUT;
        }
    }

    /*Check if Command Lock bit is set*/
    if (0 != FLASH->FASTAT_b.CMDLK)
    {
        /*Issue a Status Clear to clear Command Locked state*/
        FACI->COM8 = (uint8_t) FCU_FACI_STATUS_CLEAR_CMD;
        temp_err = FLASH_ERR_CMD_LOCKED;
    }

    /*Transition to Read mode*/
    FLASH->FENTRYR = FCU_FENTRYR_KEY | FCU_FENTRYR_READ;
    while ((FCU_FENTRYR_READ != FLASH->FENTRYR) && (count++ < FLASH_FENTRYR_TIMEOUT))
    {
        /* Do nothing */
    }

    if (FCU_FENTRYR_READ == FLASH->FENTRYR)
    {
        err = FLASH_SUCCESS;
    }
    else
    {
        err = FLASH_ERR_FAILURE;
    }

    /*If a command locked state was detected earlier, then return that error*/
    if (FLASH_ERR_CMD_LOCKED == temp_err)
    {
        return temp_err;
    }
    else
    {
        return err;
    }
}

/*******************************************************************************************************************//**
 * @brief                         Function issue the STOP command and check the state of the Command Lock bit.
 * @retval FLASH_SUCCESS          Stop issued successfully.
 * @retval FLASH_ERR_TIMEOUT      Timed out
 * @retval FLASH_ERR_CMD_LOCKED   Peripheral in locked state
 **********************************************************************************************************************/
static flash_err_t flash_stop ()
{
    /* Timeout counter. */
    volatile uint32_t wait_cnt = FLASH_FRDY_CMD_TIMEOUT;

    FLASH_FCU_INT_DISABLE;

    /*Issue stop command to flash command area*/
    FACI->COM8 = (uint8_t) FCU_FACI_FORCED_STOP_CMD;

    /* Read FRDY bit until it has been set to 1 indicating that the current
     * operation is complete.*/
    while (1 != FLASH->FSTATR_b.FRDY)
    {
        /* Wait until FRDY is 1 unless timeout occurs. */
        if (0 >= wait_cnt--)
        {
            /* This should not happen normally.
             * FRDY should get set in 15-20 ICLK cycles on STOP command*/
            return FLASH_ERR_TIMEOUT;
        }
    }
    /*Check that Command Lock bit is cleared*/
    if (0 != FLASH->FASTAT_b.CMDLK)
    {
        return FLASH_ERR_CMD_LOCKED;
    }

    return FLASH_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                         Function will erase the specified Code Flash blocks.
 * @param[in] block_address       Start address of the first block.
 * @param[in] num_blocks          Number of blocks to erase.
 * @retval FLASH_SUCCESS          Erase completed successfully; successfully initialized in case of BGO mode.
 * @retval FLASH_ERR_ACCESSW      Erase command has been issued against the area protected by the access window.
 * @retval FLASH_ERR_SECURITY     An security error occurred.
 * @retval FLASH_ERR_CMD_LOCKED   The FCU entered a command locked state and cannot process the operation.
 * @retval FLASH_ERR_FAILURE      Flash Write operation failed for some other reason.
 **********************************************************************************************************************/
static flash_err_t flash_erase (uint32_t block_address, uint32_t num_blocks)
{
    flash_err_t err = FLASH_SUCCESS;

    /*Set current operation parameters */
    g_current_parameters.dest_addr = block_address;
    g_current_parameters.total_count = num_blocks;
    g_current_parameters.wait_cnt = WAIT_MAX_ERASE_CF * (g_iclk_hz / 1000000);

    for (g_current_parameters.current_count = 0;
            g_current_parameters.current_count < g_current_parameters.total_count;
            g_current_parameters.current_count++)
    {
        /*Set block start address*/
        FLASH->FSADDR = g_current_parameters.dest_addr;
        /*Issue two part Block Erase commands*/
        FACI->COM8 = FCU_FACI_BLOCK_ERASE_CMD1;
        FACI->COM8 = FCU_FACI_BLOCK_ERASE_CMD2;

        /*if in BGO mode, exit here; remaining processing if any will be done in ISR*/
        if ((g_current_parameters.current_operation == FLASH_CUR_CF_BGO_ERASE)
                || (g_current_parameters.current_operation == FLASH_CUR_DF_BGO_ERASE))
        {
            return err;
        }
        /* Read FRDY bit until it has been set to 1 indicating that the current
         * operation is complete.*/
        while (1 != FLASH->FSTATR_b.FRDY)
        {
            /* Wait until FRDY is 1 unless timeout occurs. */
            if (g_current_parameters.wait_cnt-- <= 0)
            {
                /* if FRDY is not set to 1 after max timeout, issue the stop command*/
                flash_stop();
                return FLASH_ERR_TIMEOUT;
            }
        }

        /*Check if there were any errors
         * Check if Command Lock bit is set*/
        if (0 != FLASH->FASTAT_b.CMDLK)
        {
            if ((FLASH->FSTATR_b.ILGCOMERR) || (FLASH->FSTATR_b.ILGLERR))
            {
                err = FLASH_ERR_ACCESSW;
            }
            else if (FLASH->FSTATR_b.SECERR)
            {
                err = FLASH_ERR_SECURITY;
            }
            else if ((FLASH->FSTATR_b.PRGERR) || (FLASH->FSTATR_b.ERSERR))
            {
                err = FLASH_ERR_FAILURE;
            }
            else
            {
                err = FLASH_ERR_CMD_LOCKED;
            }
            return err;
        }

        /* If the current mode is Code Flash PE, increment to the next block
         * starting address by adding 4K
         * Also set the timeout */
        if (FCU_FENTRYR_DATA == FLASH->FENTRYR)
        {
#ifndef FLASH_NO_DATA_FLASH
            g_current_parameters.dest_addr += FLASH_DF_BLOCK_SIZE;
            g_current_parameters.wait_cnt = WAIT_MAX_ERASE_DF;
#endif
        }
        else if (FCU_FENTRYR_CODE == FLASH->FENTRYR)
        {
            g_current_parameters.dest_addr += FLASH_CF_BLOCK_SIZE;
            g_current_parameters.wait_cnt = WAIT_MAX_ERASE_CF * (g_iclk_hz / 1000000);
        }
        else
        {
            //should never get here
            return FLASH_ERR_FAILURE;       // The program never enter to this path. 
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief                        Function will write to the specified Code Flash memory area.
 *                               Note that this function does not insure that the block(s) being written are blank.
 * @param[in] src_start_address  Source buffer address (data being written to Flash)
 * @param[in] dest_start_address Destination address.
 * @param[in] num_bytes          Number of bytes to be written.
 *                               It must be multiple of the minimum programming size (8 bytes or 256 bytes).
 * @param[in] pgm_mode           Programming mode (8 bytes or 256 bytes).
 * @retval FLASH_SUCCESS         Write completed successfully; successfully initialized in case of BGO mode.
 * @retval FLASH_ERR_BYTES       Number of bytes exceeds max range or is 0 or is not a valid multiple of the minimum
 *                               programming size for the specified flash
 * @retval FLASH_ERR_ADRRESS     src/dest address is an invalid Code Flash block start address or
 *                               address is not aligned with the minimum programming size for the Code Flash.
 *                               The src address HAS to be a RAM address since ROM cannot be accessed during Code Flash
 *                               programming
 * @retval FLASH_ERR_BUSY        Flash peripheral is busy with another operation or not initialized
 * @retval FLASH_ERR_CMD_LOCKED  The FCU entered a command locked state and cannot process the operation.
 *                               A RESET was performed on the FCU to recover from this state.
 * @retval FLASH_ERR_FAILURE     Flash Write operation failed for some other reason. This can be a result of trying to
 *                               write to an area that has been protected via access control.
 **********************************************************************************************************************/
static flash_err_t flash_write (uint32_t *src_start_address, uint32_t * dest_start_address, uint32_t *num_bytes,
        flash_program_mode_t pgm_mode)
{
    flash_err_t err = FLASH_SUCCESS;

    g_current_parameters.total_count = (*num_bytes) >> 1; //Since two bytes will be written at a time
    g_current_parameters.dest_addr = *dest_start_address;
    g_current_parameters.src_addr = *src_start_address;
    g_current_parameters.current_count = 0;
    g_current_parameters.wait_cnt = WAIT_MAX_ROM_WRITE * (g_iclk_hz / 1000000);
    if (FLASH_8_BYTES_PGM == pgm_mode)
    {
        g_current_parameters.min_pgm_size = FLASH_CF_MIN_PGM_SIZE_8_BYTES >> 1; //Since two bytes will be written at a time
    }
    else
    {
        g_current_parameters.min_pgm_size = FLASH_CF_MIN_PGM_SIZE_256_BYTES >> 1; //Since two bytes will be written at a time
    }

    uint16_t tmp = 0;
    /* Iterate through the number of data bytes */
    while (g_current_parameters.total_count > 0)
    {
        /*Set block start address*/
        FLASH->FSADDR = g_current_parameters.dest_addr;
        /*Issue two part Write commands*/
        FACI->COM8 = FCU_FACI_PROGRAM_CMD1;
        FACI->COM8 = g_current_parameters.min_pgm_size;
        /*Write one line (256 bytes for CF)*/
        while (g_current_parameters.current_count++ < g_current_parameters.min_pgm_size)
        {
            /* Wait for data buffer empty state */
            while (FLASH->FSTATR_b.DBFULL)
            {
                /* Do nothing */
            }
            /* Copy data from source address to destination area */
            tmp = *(uint16_t *)g_current_parameters.src_addr;
            g_current_parameters.src_addr += 2;
            g_current_parameters.dest_addr += 2;
            g_current_parameters.total_count--;
            FACID->COM16 = tmp;
        }
        /*if in BGO mode, exit here; remaining processing if any will be done in ISR*/
        if ((g_current_parameters.current_operation == FLASH_CUR_CF_BGO_WRITE)
            || (g_current_parameters.current_operation == FLASH_CUR_DF_BGO_WRITE))
        {
            return err;
        }

        /*Reset line count*/
        g_current_parameters.current_count = 0;

        /*Issue write end command*/
        FACI->COM8 = FCU_FACI_PROGRAM_CMD2;

        /* Read FRDY bit until it has been set to 1 indicating that the current
         * operation is complete.*/

        while (1 != FLASH->FSTATR_b.FRDY)
        {
            /* Wait until FRDY is 1 unless timeout occurs. */
            if (g_current_parameters.wait_cnt-- <= 0)
            {
                /* if FRDY is not set to 1 after max timeout, issue the stop command*/
                flash_stop();
                return FLASH_ERR_TIMEOUT;
            }
        }
        g_current_parameters.wait_cnt = WAIT_MAX_ROM_WRITE * (g_iclk_hz / 1000000);
        /*Check if there were any errors
         * Check if Command Lock bit is set*/
        if (0 != FLASH->FASTAT_b.CMDLK)
        {
            err = FLASH_ERR_CMD_LOCKED;
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief                 Function resets the Flash peripheral
 * @retval FLASH_SUCCESS  Flash Peripheral successfully reset.
 * @note                  This function will reset the peripheral by stopping any ongoing operations,
 *                        clearing the DFAE and CFAE flags and changing the PE mode to Read mode.
 **********************************************************************************************************************/
static flash_err_t flash_reset ()
{
    /*Issue a Flash Stop to stop any ongoing operation*/
    flash_stop();

    /*Read and clear any DF or CF access violation errors*/
    if (FLASH->FASTAT_b.CFAE == 1)
    {
        FLASH->FASTAT_b.CFAE = 0;
    }
#ifndef FLASH_NO_DATA_FLASH
    if (FLASH->FASTAT_b.DFAE == 1)
    {
        FLASH->FASTAT_b.DFAE = 0;
    }
#endif

    /* Possible FLASH_CMD_RESET is called when no outstanding command is in progress.
     * In that case, enter pe mode so flash_stop() can write to the sequencer.
     */
    if (g_flash_state == FLASH_READY)
    {
        flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    }

    /*Issue a Flash Stop again for a complete RESET*/
    flash_stop();

    /*Transition to Read mode*/
    FLASH->FENTRYR = FCU_FENTRYR_KEY | FCU_FENTRYR_READ;

    return FLASH_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                 Returns the current state of the flash
 * @retval FLASH_SUCCESS  Flash is ready to use
 * @retval FLASH_ERR_BUSY Flash is busy with another operation or is uninitialized
 **********************************************************************************************************************/
static flash_err_t flash_get_status (void)
{
    /* Return flash status */
    if (g_flash_state == FLASH_READY)
    {
        return FLASH_SUCCESS;
    }
    else
    {
        return FLASH_ERR_BUSY;          // The program never enter to this path. 
    }
}

/*******************************************************************************************************************//**
 * @brief                 Attempt to grab the flash state to perform an operation
 * @param[in] new_state   Which state to attempt to transition to
 * @retval FLASH_SUCCESS  State was grabbed
 * @retval FLASH_ERR_BUSY Flash is busy with another operation
 **********************************************************************************************************************/
static flash_err_t flash_lock_state (flash_states_t new_state)
{
    /* Check to see if lock was successfully taken */
    if (flash_software_lock(&g_flash_lock) == true)
    {
        /* Lock taken, we can change state */
        g_flash_state = new_state;

        /* Return success */
        return FLASH_SUCCESS;
    }
    else
    {
        /* Another operation is on-going */
        return FLASH_ERR_BUSY;
    }
}

/*******************************************************************************************************************//**
 * @brief Release state so another flash operation can take place
 **********************************************************************************************************************/
static void flash_release_state (void)
{
    /* Done with current operation */
    g_flash_state = FLASH_READY;

    /* Release hold on lock */
    flash_software_unlock(&g_flash_lock);
}

/*******************************************************************************************************************//**
 * @brief                     Swap registers value
 * @param[in,out] fp          First parameter
 * @param[in,out] sp          Second parameter
 **********************************************************************************************************************/
static void swapfun (int32_t * restrict fp, int32_t * restrict sp)
{
    uint32_t temp = *fp;
    *fp = *sp;
    *sp = temp;
}

/*******************************************************************************************************************//**
 * @brief                     Attempt to acquire the lock that has been sent in.
 * @param[out] plock          Pointer to lock state.
 * @retval true               Lock was acquired.
 * @retval false              Lock was not acquired.
 **********************************************************************************************************************/
static bool flash_software_lock (int32_t * const plock)
{
    bool ret = false;

    /* Variable used in trying to acquire lock. Using the xchg instruction makes this atomic */
    int32_t is_locked = true;

    swapfun(&is_locked, plock);

    /* Check to see if semaphore was successfully taken */
    if (is_locked == false)
    {
        /* Lock obtained, return success. */
        ret = true;
    }
    else
    {
        /* Lock was not obtained, another task already has it. */
    }

    return ret;
}

/*******************************************************************************************************************//**
 * @brief                     Release hold on lock.
 * @param[out] plock          Pointer to lock state.
 * @retval true               Lock was released.
 * @retval false              Lock was not released.
 **********************************************************************************************************************/
static bool flash_software_unlock (int32_t * const plock)
{
    /* Set lock back to unlocked. */
    *plock = false;

    return true;
}

/*******************************************************************************************************************//**
 * @brief                     Check the current start-up area setting and specifies the area currently not used as
 *                            the start-up area.
 * @retval FLASH_SUCCESS      Switched successfully.
 * @retval FLASH_ERR_FAILURE  Unable to Switch to P/E Mode.
 * @retval FLASH_ERR_PARAM    Illegal parameter passed
 **********************************************************************************************************************/
static flash_err_t flash_toggle_startup_area (void)
{
    flash_err_t err = FLASH_SUCCESS;
    fawreg_t faw;

    g_current_parameters.current_operation = FLASH_CUR_CF_TOGGLE_STARTUPAREA;
    faw.LONG = FLASH->FAWMON;
    faw.BIT.BTFLG ^= 1;
    err = flash_write_faw_reg(faw);

    return err;
}

/*******************************************************************************************************************//**
 * @brief                    Return which startup area (Default or Alternate) is active
 * @retval 0                 Alternate area
 * @retval 1                 Default area
 **********************************************************************************************************************/
static uint8_t flash_get_current_startup_area (void)
{
    fawreg_t faw;

    faw.LONG = FLASH->FAWMON;

    return (faw.BIT.BTFLG);
}

/*******************************************************************************************************************//**
 * @brief                    Return which startup area (Default or Alternate) is active
 * @retval 0                 The start-up area is selected according to the start-up area settings of the extra area.
 * @retval 2                 The start-up area is switched to the default area temporarily.
 * @retval 3                 The start-up area is switched to the alternate area temporarily.
 **********************************************************************************************************************/
static uint8_t flash_get_current_swap_state (void)
{
    return (FLASH->FSUACR_b.SAS);
}

/*******************************************************************************************************************//**
 * @brief                    Setting for switching the start-up area
 * @param[in] value          Value for SAS bits; switch startup area if value = SAS_SWITCH_AREA
 **********************************************************************************************************************/
static void flash_set_current_swap_state (uint8_t value)
{
    uint8_t sas_flag;
    uint16_t reg_value;

    if (FLASH_SAS_SWITCH_AREA == value) /* to switch startup areas */
    {
        if (FLASH_SAS_SWAPFLG == FLASH->FSUACR_b.SAS)
        {
            if (1 == FLASH->FAWMON_b.BTFLG)     // 1 = default
            {
                sas_flag = FLASH_SAS_ALTERNATE;
            }
            else
            {
                sas_flag = FLASH_SAS_DEFAULT;
            }
        }
        else
        {
            if (FLASH_SAS_ALTERNATE == FLASH->FSUACR_b.SAS)
            {
                sas_flag = FLASH_SAS_DEFAULT;
            }
            else
            {
                sas_flag = FLASH_SAS_ALTERNATE;
            }
        }
    }
    else
    {
        sas_flag = value; /* to set SAS to desired area */
    }

    reg_value = FCU_FSUACR_KEY | (uint16_t)sas_flag;
    FLASH->FSUACR = reg_value;

    while (sas_flag != FLASH->FSUACR_b.SAS)
    {
        /* Confirm that the written value can be read correctly. */
    }
}

/*******************************************************************************************************************//**
 * @brief                Return the read address form of the current access window area setting
 * @retval FLASH_SUCCESS Operation was completed successfully
 **********************************************************************************************************************/
static flash_err_t flash_get_access_window (flash_access_window_config_t *pAccessInfo)
{
    pAccessInfo->start_addr = (FLASH->FAWMON_b.FAWS << 12);
    pAccessInfo->end_addr = (FLASH->FAWMON_b.FAWE << 12);

    return FLASH_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                    Specifies the setting for the access window.
 * @param[in] pAccessInfo    Start and end address of Access Window Setting.
 *                           The end address should be one beyond the actual last byte to allow write access for.
 * @retval FLASH_SUCCESS     Operation was completed successfully
 * @retval FLASH_ERR_ACCESSW AccessWindow setting error
 * @retval FLASH_ERR_ADDRESS Address is invalid
 **********************************************************************************************************************/
#ifndef CHIP_VERSION_WS1
static flash_err_t flash_set_access_window (flash_access_window_config_t *pAccessInfo)
{
    flash_err_t err = FLASH_SUCCESS;
    fawreg_t faw;

    g_current_parameters.current_operation = FLASH_CUR_CF_ACCESSWINDOW;

    faw.LONG = FLASH->FAWMON;

    faw.BIT.FAWS = (pAccessInfo->start_addr & 0x00FFF000) >> 12;
    faw.BIT.FAWE = (pAccessInfo->end_addr & 0x00FFF000) >> 12;

    err = flash_write_faw_reg(faw);

    return err;
}
#endif /* CHIP_VERSION_WS1 */

/*******************************************************************************************************************//**
 * @brief                    Writes the contents of the argument to the FAW register.
 * @param[in] start_addr     Start address of Access Window Setting
 * @param[in] end_addr       End address of Access Window Setting.
 * @retval FLASH_SUCCESS     Command executed successfully
 * @retval FLASH_ERR_ACCESSW AccessWindow setting error
 **********************************************************************************************************************/
static flash_err_t flash_write_faw_reg (fawreg_t faw)
{
    flash_err_t err = FLASH_SUCCESS;

    err = flash_pe_mode_enter(FLASH_TYPE_CODE_FLASH);
    if (FLASH_SUCCESS != err)
    {
        return (err);
    }

    faw.BIT.FSPR = 1; // p/e enabled (allow rewrite of flash; 0 locks chip forever)

#ifdef __BIG    // Big endian
    uint32_t swapped;
    swapped = (faw.LONG << 16) | ((faw.LONG >> 16) & 0x0000FFFF);
    faw.LONG = swapped;
#endif

    FLASH->FSADDR_b.FSADDR = FCU_CONFIG_REG_ADDR;
    FACI->COM8 = (uint8_t)FCU_FACI_CONFIG_CMD1;
    FACI->COM8 = (uint8_t)FCU_FACI_CONFIG_CMD2;

    FACID->COM16 = 0xFFFF;                                   // WD0
    FACID->COM16 = 0xFFFF;                                   // WD1
    FACID->COM16 = (uint16_t)(faw.LONG & 0x000FFFF);         // WD2
    FACID->COM16 = (uint16_t)((faw.LONG >> 16) & 0x000FFFF); // WD3
    FACID->COM16 = 0xFFFF;                                   // WD4
    FACID->COM16 = 0xFFFF;                                   // WD5
    FACID->COM16 = 0xFFFF;                                   // WD6
    FACID->COM16 = 0xFFFF;                                   // WD7
    FACI->COM8 = (uint8_t)FCU_FACI_CONFIG_CMD3;

#if (FLASH_CFG_CODE_FLASH_BGO == 0)
    g_current_parameters.wait_cnt = FLASH_FRDY_CMD_TIMEOUT;
    while (1 != FLASH->FSTATR_b.FRDY)
    {
        /* Wait until FRDY is 1 unless timeout occurs. */
        if (g_current_parameters.wait_cnt-- <= 0)
        {
            /* if FRDY is not set to 1 after max timeout, issue the stop command*/
            flash_stop();
            return FLASH_ERR_TIMEOUT;
        }
    }

    flash_pe_mode_exit();
#endif

    return err;
}

/*******************************************************************************************************************//**
 * @brief ISR for Flash Ready Interrupt
 **********************************************************************************************************************/
static void flash_ready_isr (void)
{
    uint16_t tmp;
    if ((FLASH_CUR_DF_BGO_WRITE == g_current_parameters.current_operation)
     || (FLASH_CUR_CF_BGO_WRITE == g_current_parameters.current_operation))
    {
        /* Resume P/E */
        FACI->COM8 = (uint8_t) FCU_FACI_PROGRAM_CMD2;

        /* If there are still bytes to write */
        if (g_current_parameters.total_count > 0)
        {
            g_current_parameters.current_count = 0;

            /* Setup fcu command */
            FLASH->FSADDR = g_current_parameters.dest_addr;
            FACI->COM8 = (uint8_t) FCU_FACI_PROGRAM_CMD1;
            FACI->COM8 = (uint8_t) g_current_parameters.min_pgm_size;

            /* MINIMUM FLASH WRITE SIZE LOOP (2 BYTES AT A TIME DUE TO FCU BUFFER SIZE) */
            while (g_current_parameters.current_count++ < g_current_parameters.min_pgm_size)
            {
                /* Copy data from source address to destination area */
                tmp = *(uint16_t *)g_current_parameters.src_addr;
                g_current_parameters.src_addr += 2;
                g_current_parameters.dest_addr += 2;
                g_current_parameters.total_count--;
                FACID->COM16 = tmp;

                /* Wait for fcu buffer empty state */
                while (FLASH->FSTATR_b.DBFULL)
                {
                    /* Do nothing */
                }
            }
            /* Exit ISR until next FRDY interrupt to continue operation (write next min size) */
            return;
        }
        /* Done writing all bytes */
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_WRITE_COMPLETE;
        }
    }


    else if ((FLASH_CUR_DF_BGO_ERASE == g_current_parameters.current_operation)
        || (FLASH_CUR_CF_BGO_ERASE == g_current_parameters.current_operation))
    {
        g_current_parameters.current_count++;
        if (g_current_parameters.current_count < g_current_parameters.total_count)
        {
            /* Increment to next block address */
#ifndef FLASH_NO_DATA_FLASH
            /* DATA FLASH */
            if (FLASH->FENTRYR_b.FENTRYD == 1)
            {
                g_current_parameters.dest_addr += FLASH_DF_BLOCK_SIZE;
            }
#endif

#if (FLASH_CFG_CODE_FLASH_ENABLE == 1)
            /* CODE FLASH */
            if (FLASH->FENTRYR_b.FENTRYC == 1)
            {
                g_current_parameters.dest_addr += FLASH_CF_BLOCK_SIZE;
            }
#endif

            /* Init flash register and write fcu erase block command */
            FLASH->FSADDR = g_current_parameters.dest_addr;
            FACI->COM8 = (uint8_t) FCU_FACI_BLOCK_ERASE_CMD1;
            FACI->COM8 = (uint8_t) FCU_FACI_BLOCK_ERASE_CMD2;

            /* Exit ISR until next FRDY interrupt to continue operation (erase next block) */
            return;
        }
        /* Done erasing all blocks */
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_ERASE_COMPLETE;
        }
    }


#ifndef FLASH_NO_BLANK_CHECK
    else if (FLASH_CUR_DF_BGO_BLANKCHECK == g_current_parameters.current_operation)
    {
        if (FLASH->FBCSTAT_b.BCST == 0x01)
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_NOT_BLANK;
        }
        else
        {
            g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_BLANK;
        }
    }
#endif

#if (FLASH_HAS_CF_ACCESS_WINDOW && FLASH_CFG_CODE_FLASH_ENABLE)
    else if (FLASH_CUR_CF_ACCESSWINDOW == g_current_parameters.current_operation)
    {
        g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_SET_ACCESSWINDOW;
    }
#endif

#if (FLASH_HAS_BOOT_SWAP && FLASH_CFG_CODE_FLASH_ENABLE)
    else if (FLASH_CUR_CF_TOGGLE_STARTUPAREA == g_current_parameters.current_operation)
    {
        g_flash_int_ready_cb_args.event = FLASH_INT_EVENT_TOGGLE_STARTUPAREA;
    }
#endif

    if (g_current_parameters.current_operation != FLASH_CUR_STOP)
    {
        flash_pe_mode_exit();
        flash_release_state();

        if ((FLASH_NO_FUNC != flash_ready_isr_handler)
            && (NULL != flash_ready_isr_handler))
        {
            flash_ready_isr_handler((void *)&g_flash_int_ready_cb_args);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief ISR for Flash Error Interrupt
 **********************************************************************************************************************/
static void flash_error_isr (void)
{
    /* Check if Command Lock bit is set */
    if (1 == FLASH->FASTAT_b.CMDLK)
    {
#ifndef FLASH_NO_DATA_FLASH
        if (FLASH.FASTAT.BIT.DFAE)
        {
            g_flash_int_error_cb_args.event = FLASH_INT_EVENT_ERR_DF_ACCESS;  // tried to access invalid address
        }
#endif
        if ((FLASH->FASTAT_b.CFAE) || (FLASH->FSTATR_b.ILGLERR))
        {
            g_flash_int_error_cb_args.event = FLASH_INT_EVENT_ERR_CF_ACCESS;  // tried to access invalid address
        }
        else if (FLASH->FSTATR_b.SECERR)           // access window protected
        {
            g_flash_int_error_cb_args.event = FLASH_INT_EVENT_ERR_SECURITY;
        }
        else if ((FLASH->FSTATR_b.PRGERR) || (FLASH->FSTATR_b.ERSERR))
        {
            g_flash_int_error_cb_args.event = FLASH_INT_EVENT_ERR_FAILURE;
        }
        else
        {
            g_flash_int_error_cb_args.event = FLASH_INT_EVENT_ERR_CMD_LOCKED;
        }
    }

    /* Reset the FCU: This will stop any existing processes and exit PE mode */
    flash_reset();

    /* Clear any pending Flash Ready interrupt request */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI);
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FRDYI);

    /* Clear any pending Flash Error interrupt request */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR);
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_FCU_FIFERR);

    flash_release_state();

    if ((FLASH_NO_FUNC != flash_error_isr_handler) && (NULL != flash_error_isr_handler))
    {
        flash_error_isr_handler((void *)&g_flash_int_error_cb_args);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_flash)
 **********************************************************************************************************************/

/* End of File */

