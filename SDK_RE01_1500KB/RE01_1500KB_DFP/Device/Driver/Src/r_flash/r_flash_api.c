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
* File Name    : r_flash_api.c
* Version      : 1.00
* Description  : This module implements the FLASH API
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.10.2018 0.51    First Release
*         : 29.11.2018 0.60    Update E017.h to version 0.7
*         : 22.07.2019 1.00    Change the product name to RE Family
*                              - Change the include file names
*                              - Update comments
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup grp_device_hal_flash
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Configuration for this package. */
#include "r_flash_api.h"

#include "RE01_1500KB.h"
#include "r_flash_cfg.h"
#include "r_flash_re01_1500kb.h"
#include "r_flash_cfg.h"

/* Public interface header file for this package. */
#include <stdlib.h>

#include "r_flash_lowlevel.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
External functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief                         Function will initialize the flash peripheral.
 * @retval FLASH_SUCCESS          API initialized successfully.
 * @retval FLASH_ERR_BUSY         API has already been initialized and another operation is ongoing.
 * @retval FLASH_ERR_ALREADY_OPEN Open() has been called twice without an intermediate Close().
 * @retval FLASH_ERR_FAILURE      Initialization failed.
 *                                A RESET was performed on the Flash circuit to rectify any internal errors
 **********************************************************************************************************************/
flash_err_t R_FLASH_Open (void)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Call the MCU specific control function which handles control commands for the target MCU */
    err = flash_api_open();

    return(err);
}

/*******************************************************************************************************************//**
 * @brief                 Function will close the flash peripheral.
 * @retval FLASH_SUCCESS  API closed successfully.
 * @retval FLASH_ERR_BUSY Another operation is ongoing.
 **********************************************************************************************************************/
flash_err_t R_FLASH_Close (void)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Call the MCU specific control function which handles control commands for the target MCU */
    err = flash_api_close();

    return(err);
}

/*******************************************************************************************************************//**
 * @brief                    Function will write to the specified Code Flash memory area.
 *                           Note that this function does not insure that the block(s) being written are blank.
 * @param[in] src_address    Source buffer address (data being written to Flash)
 * @param[in] dest_address   Destination address.
 * @param[in] num_bytes      Number of bytes to be written.
 *                           It must be multiple of the minimum programming size (8 bytes).
 * @retval FLASH_SUCCESS     Write completed successfully; successfully initialized in case of BGO mode.
 * @retval FLASH_ERR_BYTES   Number of bytes exceeds max range or is 0 or is not a valid multiple of the minimum
 *                           programming size for the specified flash
 * @retval FLASH_ERR_OVERFLOW Address + number of bytes for this operation went past the
 *                           end of memory area
 * @retval FLASH_ERR_ADRRESS src/dest address is an invalid Code Flash block start address or
 *                           address is not aligned with the minimum programming size for the Code Flash.
 *                           The src address HAS to be a RAM address since ROM cannot be accessed during Code Flash
 *                           programming
 * @retval FLASH_ERR_BUSY    Flash peripheral is busy with another operation or not initialized
 * @retval FLASH_ERR_CMD_LOCKED The FCU entered a command locked state and cannot process the operation.
 *                              A RESET was performed on the FCU to recover from this state.
 * @retval FLASH_ERR_FAILURE Flash Write operation failed for some other reason. This can be a result of trying to write
 *                           to an area that has been protected via access control.
 **********************************************************************************************************************/
flash_err_t R_FLASH_Write_8Bytes (uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Call the MCU specific write function which handles control commands for the target MCU */
    err = flash_api_write(src_address, dest_address, num_bytes, FLASH_8_BYTES_PGM);

    return err;
}

/*******************************************************************************************************************//**
 * @brief                    Function will write to the specified Code Flash memory area.
 *                           Note that this function does not insure that the block(s) being written are blank.
 * @param[in] src_address    Source buffer address (data being written to Flash)
 * @param[in] dest_address   Destination address.
 * @param[in] num_bytes      Number of bytes to be written.
 *                           It must be multiple of the minimum programming size (256 bytes).
 * @retval FLASH_SUCCESS     Write completed successfully; successfully initialized in case of BGO mode.
 * @retval FLASH_ERR_BYTES   Number of bytes exceeds max range or is 0 or is not a valid multiple of the minimum
 *                           programming size for the specified flash
 * @retval FLASH_ERR_OVERFLOW Address + number of bytes for this operation went past the
 *                           end of memory area
 * @retval FLASH_ERR_ADRRESS src/dest address is an invalid Code Flash block start address or
 *                           address is not aligned with the minimum programming size for the Code Flash.
 *                           The src address HAS to be a RAM address since ROM cannot be accessed during Code Flash
 *                           programming
 * @retval FLASH_ERR_BUSY    Flash peripheral is busy with another operation or not initialized
 * @retval FLASH_ERR_CMD_LOCKED The FCU entered a command locked state and cannot process the operation.
 *                              A RESET was performed on the FCU to recover from this state.
 * @retval FLASH_ERR_FAILURE Flash Write operation failed for some other reason. This can be a result of trying to write
 *                           to an area that has been protected via access control.
 **********************************************************************************************************************/
flash_err_t R_FLASH_Write_256Bytes (uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
{
    flash_err_t err = FLASH_SUCCESS;
    
    /* Call the MCU specific write function which handles control commands for the target MCU */
    err = flash_api_write(src_address, dest_address, num_bytes, FLASH_256_BYTES_PGM);
    
    return err;
}

/*******************************************************************************************************************//**
 * @brief                         Function will erase the specified Code Flash blocks.
 * @param[in] block_start_address Start address of the first block.
 * @param[in] num_blocks          Number of blocks to erase.
 * @retval FLASH_SUCCESS          Erase completed successfully; successfully initialized in case of BGO mode.
 * @retval FLASH_ERR_BLOCKS       Number of blocks exceeds max range or is 0
 * @retval FLASH_ERR_ADRRESS      Start address is an invalid Code Flash block start address
 * @retval FLASH_ERR_BUSY         Flash peripheral is busy with another operation
 * @retval FLASH_ERR_FAILURE      Flash Write operation failed for some other reason. This can be a result of trying
 *                                to erase an area that has been protected via access control.
 **********************************************************************************************************************/
flash_err_t R_FLASH_Erase (uint32_t block_start_address, uint32_t num_blocks)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Call the MCU specific write function which handles control commands for the target MCU */
    err = flash_api_erase(block_start_address, num_blocks);

    return err;       
}

/*******************************************************************************************************************//**
 * @brief                         Function will perform a blank check on the specified Flash area.
 * @param[in] address             Start address to perform blank check.
 * @param[in] num_blocks          Number of bytes to perform blank check operation for.
 * @param[out] result             Returns the result of the blank check operation. This field is valid only in
 *                                non-BGO mode operation
 * @retval FLASH_ERR_UNSUPPORTED  Command not supported
 * @note                          RE01 chip does not support Blank Check feature. 
 *                                The API always return FLASH_ERR_UNSUPPORTED.
 **********************************************************************************************************************/
flash_err_t R_FLASH_BlankCheck (uint32_t address, uint32_t num_bytes, flash_res_t *result)
{
    return FLASH_ERR_UNSUPPORTED;
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
 * @retval FLASH_ERR_FREQUENCY   Illegal Frequency parameter passed for FLASH_CMD_CONFIG_CLOCK command
 * @retval FLASH_ERR_UNSUPPORTED Unrecognized commands
 **********************************************************************************************************************/
flash_err_t R_FLASH_Control (flash_cmd_t cmd, void *pcfg)
{
    flash_err_t err = FLASH_SUCCESS;

    /* Call the MCU specific control function which handles control commands for the target MCU */
    err = flash_api_control(cmd, pcfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Returns the current version of this module. The version number is encoded where the top 2 bytes are the
 *         major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25
 *         would be returned as 0x00040019.
 * @retval Version of this module.
 **********************************************************************************************************************/
uint32_t R_FLASH_GetVersion (void)
{
    uint32_t r = 0;
    
    /* These version macros are defined in r_flash.h. */
    r = ((((uint32_t)FLASH_VERSION_MAJOR) << 16) | (uint32_t)FLASH_VERSION_MINOR);
    
    return r;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_flash)
 **********************************************************************************************************************/

/* End of File */
