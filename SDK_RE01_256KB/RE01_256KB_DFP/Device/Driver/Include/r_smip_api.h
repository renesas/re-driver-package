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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_smip_api.c
* Version      : 0.40
* Description  : Serial MIP-LCD API.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
**********************************************************************************************************************/
/******************************************************************************************************************//**
 @ingroup grp_device_hal
 @defgroup grp_device_hal_smip Device HAL SMIP Implementation
 @brief The SMIP Middleware implementation for the HAL-Driver SMIP interface on RE Family.

 @section sec_hal_driver_smip_summary SMIP-Middleware Summary on RE Family.
 When using SMIP-Middleware, call the R_SMIP_XXX function directly.
 
 @code
 ...

    R_SMIP_Open( 1, 500000, &g_smip_tbl_lcd_info[SMIP_TYPE_KYOCERA], callback );    // Open SMIP
    R_SMIP_PowerOn();                                                               // Execution of power on sequence
    R_SMIP_Send(0, 256, disp_image);                             // Send the image information and display the screen.

 ...
 @endcode

 @section HAL_Driver_SMIP_Specific_Error_Code HAL-Driver SMIP Specific Error Code
 See DMA_ERROR_XXX in r_smip_api.h for the detail.

 @{
 *********************************************************************************************************************/
/***********************************************************
Includes <System Includes> , "Project Includes"
***********************************************************/
#include "RE01_256KB.h"
#include "Driver_SPI.h"
#include "Pin.h"
#include "r_system_api.h"
#include "r_system_cfg.h"
#include "r_lpm_api.h"
#include "r_smip_cfg.h"


/******************************************************************************
 Macro definitions
*****************************************************************************/
#ifndef R_SMIP_CMSIS_H
#define R_SMIP_CMSIS_H


/******************************************************************************
 Global Typedef definitions
*****************************************************************************/
/** SMIP API error codes */
typedef enum /* @suppress("Source line ordering") */
{
    SMIP_OK = 0,                                    /*!< Operation succeeded */
    SMIP_ERROR,                                     /*!< Unspecified error */
    SMIP_ERROR_BUSY,                                /*!< Driver is busy */
    SMIP_ERROR_PARAMETER,                           /*!< Parameter error */
    SMIP_ERROR_MODE,                                /*!< Mode error */
    SMIP_ERROR_LOCKED,                              /*!< This Module is already locked */
    SMIP_ERROR_TIMEOUT,                             /*!< Timeout error */
    SMIP_ERROR_SYSTEM_SETTING,                      /*!< error for System Driver (R_System) setting */
    SMIP_ERROR_POWER_SEQUENCE                       /*!< Power on/off sequence error */
} e_smip_err_t;

/** SMIP LCD codes */
typedef enum
{
    SMIP_TYPE_KYOCERA = 0,                          /*!< KYOCERA MIP-LCD 256(H)x256(V) monochrome */
    SMIP_TYPE_JDI,                                  /*!< JDI MIP-LCD 176(H)xRGBx176(V)  */
    SMIP_TYPE_SHARP,                                /*!< Sharp MIP-LCD 128(H)x128(V) monochrome */
} e_smip_lcd_type_t;


/** @brief Transmission status */
typedef struct {        
    uint8_t                 type;
    uint8_t                 mode_size;
    uint8_t                 mode_dmy_size;
    uint8_t                 address_size;
    uint8_t                 end_dummy_size;
    uint8_t                 first_addr;
    uint16_t                disp_width;
    uint16_t                disp_line;
    uint16_t                vcom_clk;
    uint16_t                t_memory_init;
    uint16_t                t_com_init;
    uint16_t                ts_scs;
    uint16_t                th_scs;
    uint16_t                tw_scsl;
    uint16_t                ts_vcom;
    uint16_t                th_vcom;
    const uint8_t           *cmd_tbl;
} st_smip_cfg_t;        

typedef void (*smip_cb_event_t) (uint32_t event);         /*!< Pointer to SMIP interrupt callback function. */

/** SMIP API callback events */
#define SMIP_EVENT_SEND_COMPLETE    (1UL << 0)      /*!< Send compleate */
#define SMIP_EVENT_SEND_ERROR       (1UL << 1)      /*!< Send error */

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/
#if (SMIP_CFG_SECTION_R_SMIP_GETVERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_GETVERSION       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_GETVERSION
#endif

#if (SMIP_CFG_SECTION_R_SMIP_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_OPEN       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_OPEN
#endif

#if (SMIP_CFG_SECTION_R_SMIP_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_CLOSE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_CLOSE
#endif

#if (SMIP_CFG_SECTION_R_SMIP_POWER_ON == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_POWER_ON       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_POWER_ON
#endif

#if (SMIP_CFG_SECTION_R_SMIP_POWER_OFF == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_POWER_OFF       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_POWER_OFF
#endif

#if (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_ALL_ZERO       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_ALL_ZERO
#endif

#if (SMIP_CFG_SECTION_R_SMIP_ALL_ONE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_ALL_ONE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_ALL_ONE
#endif

#if (SMIP_CFG_SECTION_R_SMIP_SEND == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_SEND       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SEND
#endif

#if (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_SEND_COLOR       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SEND_COLOR
#endif

#if (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_SEND_COMMAND       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SEND_COMMAND
#endif

#if (SMIP_CFG_SECTION_R_SMIP_SUSPEND == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_SUSPEND       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SUSPEND
#endif

#if (SMIP_CFG_SECTION_R_SMIP_RESUME == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_RESUME       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_RESUME
#endif

#if (SMIP_CFG_SECTION_R_SMIP_RECONFIG_SPI_SPPED == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_RECONFIG_SPI_SPPED       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_RECONFIG_SPI_SPPED
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_OPEN                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CLOSE               == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_TBL_LCD_INFO       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_TBL_LCD_INFO
#endif

uint32_t R_SMIP_GetVersion(void) FUNC_LOCATION_R_SMIP_GETVERSION; // @suppress("Source line ordering")
e_smip_err_t R_SMIP_Open(uint8_t ch, uint32_t sclk, const st_smip_cfg_t *p_info, 
                                                    smip_cb_event_t cb_event) FUNC_LOCATION_R_SMIP_OPEN;
e_smip_err_t R_SMIP_Close(void) FUNC_LOCATION_R_SMIP_CLOSE;
e_smip_err_t R_SMIP_PowerOn(void) FUNC_LOCATION_R_SMIP_POWER_ON;
e_smip_err_t R_SMIP_PowerOff(void) FUNC_LOCATION_R_SMIP_POWER_OFF;
e_smip_err_t R_SMIP_AllZero(void) FUNC_LOCATION_R_SMIP_ALL_ZERO;
e_smip_err_t R_SMIP_AllOne(void) FUNC_LOCATION_R_SMIP_ALL_ONE;
e_smip_err_t R_SMIP_Send(uint16_t st, uint16_t line_num, const uint8_t *data) FUNC_LOCATION_R_SMIP_SEND;
e_smip_err_t R_SMIP_SendColor(uint16_t st, uint16_t line_num, const uint8_t *data) FUNC_LOCATION_R_SMIP_SEND_COLOR;
e_smip_err_t R_SMIP_SendCommand(uint8_t cmd) FUNC_LOCATION_R_SMIP_SEND_COMMAND;
e_smip_err_t R_SMIP_Suspend(void) FUNC_LOCATION_R_SMIP_SUSPEND;
e_smip_err_t R_SMIP_Resume(uint32_t sclk) FUNC_LOCATION_R_SMIP_RESUME;
e_smip_err_t R_SMIP_ReconfigSpiSpeed(uint32_t sclk) FUNC_LOCATION_R_SMIP_RECONFIG_SPI_SPPED;

extern const st_smip_cfg_t g_smip_tbl_lcd_info[3] DATA_LOCATION_PRV_TBL_LCD_INFO;


#endif /* R_SPI_CMSIS_H */
/******************************************************************************************************************//**
 * @} (end defgroup grp_device_hal_smip)
 *********************************************************************************************************************/

/* End of file (r_smip_api.h) */
