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
* File Name    : r_flash_lowlevel.h
* Version      : 1.00
* Description  : This file defines the flash API functions for the RE01 Group(1500KB products).
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 04.10.2018 0.51    First Release
*         : 29.11.2018 0.60    Version Update
*         : 22.07.2019 1.00    Change the product name to RE Family.
*                              - Update comments
**********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup grp_device_hal_flash
 * @{
 **********************************************************************************************************************/

#ifndef R_FLASH_LOWLEVEL_HEADER_FILE
#define R_FLASH_LOWLEVEL_HEADER_FILE

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>
#include "r_flash_api.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/** Flash Programming Mode */
typedef enum
{
    FLASH_8_BYTES_PGM = 0,
    FLASH_256_BYTES_PGM
} flash_program_mode_t;

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
#if (FLASH_CFG_SECTION_R_FLASH_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_FLASH_API_OPEN          __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_FLASH_API_OPEN
#endif
#if (FLASH_CFG_SECTION_R_FLASH_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_FLASH_API_CLOSE         __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_FLASH_API_CLOSE
#endif
#if (FLASH_CFG_SECTION_R_FLASH_WRITE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_FLASH_API_WRITE         __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_FLASH_API_WRITE
#endif
#if (FLASH_CFG_SECTION_R_FLASH_ERASE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_FLASH_API_ERASE         __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_FLASH_API_ERASE
#endif
#if (FLASH_CFG_SECTION_R_FLASH_BLANKCHECK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_FLASH_API_BLANKCHECK    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_FLASH_API_BLANKCHECK
#endif
#if (FLASH_CFG_SECTION_R_FLASH_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_FLASH_API_CONTROL       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_FLASH_API_CONTROL
#endif

flash_err_t flash_api_open (void)                                               FUNC_LOCATION_FLASH_API_OPEN;
flash_err_t flash_api_close (void)                                              FUNC_LOCATION_FLASH_API_CLOSE;
flash_err_t flash_api_write (uint32_t flash_addr, uint32_t buffer_addr, uint32_t bytes, flash_program_mode_t pgm_mode)
                                                                                FUNC_LOCATION_FLASH_API_WRITE;
flash_err_t flash_api_erase (uint32_t block_start_address, uint32_t num_blocks) FUNC_LOCATION_FLASH_API_ERASE;
flash_err_t flash_api_blankcheck (uint32_t address, uint32_t num_bytes, flash_res_t *result)
                                                                                FUNC_LOCATION_FLASH_API_BLANKCHECK;
flash_err_t flash_api_control (flash_cmd_t cmd, void *pcfg)                     FUNC_LOCATION_FLASH_API_CONTROL;

/*******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_flash)
 **********************************************************************************************************************/

#endif /* R_FLASH_LOWLEVEL_HEADER_FILE */


