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
* File Name    : r_flash_cfg.h
* Version      : 1.00
* Description  : 
**********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 16.12.2019 0.4     First Release (Ported from 1500KB)
*                              - Update comments
*         : 16.1.2020  0.8     Update comments
*         : 25.5.2020  1.0     Update comments
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup grp_device_hal_flash
 * @{
 **********************************************************************************************************************/

#ifndef R_FLASH_CFG_H
#define R_FLASH_CFG_H

#ifdef  __cplusplus
extern "C"
{
#endif

/***********************************************************************************************************************
 Configuration Options
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief FLASH PARAM CHECKING ENABLE@n
 * SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING@n
 * Setting to 1 includes parameter checking; 0 compiles out parameter checking
 **********************************************************************************************************************/
#define FLASH_CFG_PARAM_CHECKING_ENABLE     (1)


/*******************************************************************************************************************//**
 * @brief ENABLE CODE FLASH PROGRAMMING@n
 * If you are only using data flash, set this to 0.
 * Setting to 1 includes code to program the ROM area. When programming ROM,
 * code must be executed from RAM.
 **********************************************************************************************************************/
#define FLASH_CFG_CODE_FLASH_ENABLE (1)

/*******************************************************************************************************************//**
 * @brief ENABLE BGO/NON-BLOCKING CODE FLASH (ROM) OPERATIONS@n
 * Setting this to 0 forces ROM API function to block until completed.
 * Setting to 1 places the module in BGO (background operations) mode. In BGO
 * mode, ROM operations return immediately after the operation has been started.
 * Notification of the operation completion is done via the callback function.
 * When reprogramming ROM, THE RELOCATABLE VECTOR TABLE AND CORRESPONDING
 * INTERRUPT ROUTINES MUST BE IN RAM.
 **********************************************************************************************************************/
#define FLASH_CFG_CODE_FLASH_BGO   (0)

/*******************************************************************************************************************//**
 * @brief MAP FLASH DRIVER TO ROM@n
 * Setting to 1 maps the flash driver to the ROM. (instead of RAM)
 * This mean, user can locate all of flash driver into Flash memory.
 * By using BGO, user can write and erase the block which does not has flash driver.
 **********************************************************************************************************************/
#define FLASH_CFG_CODE_FLASH_RUN_FROM_ROM (0)

#if (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0)
/*******************************************************************************************************************//**
 * @name R_FLASH_API_LOCATION_CONFIG
 *       Definition of R_FLASH API location configuration@n
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *       It takes effect when FLASH_CFG_CODE_FLASH_RUN_FROM_ROM is 0.@n
 *       When FLASH_CFG_CODE_FLASH_RUN_FROM_ROM is 1, all R_FLASH API locate in Flash memory
 **********************************************************************************************************************/
/* @{ */
#define FLASH_CFG_SECTION_R_FLASH_OPEN                        (SYSTEM_SECTION_RAM_FUNC)
#define FLASH_CFG_SECTION_R_FLASH_CLOSE                       (SYSTEM_SECTION_RAM_FUNC)
#define FLASH_CFG_SECTION_R_FLASH_WRITE                       (SYSTEM_SECTION_RAM_FUNC)
#define FLASH_CFG_SECTION_R_FLASH_ERASE                       (SYSTEM_SECTION_RAM_FUNC)
#define FLASH_CFG_SECTION_R_FLASH_BLANKCHECK                  (SYSTEM_SECTION_RAM_FUNC)
#define FLASH_CFG_SECTION_R_FLASH_CONTROL                     (SYSTEM_SECTION_RAM_FUNC) 
#define FLASH_CFG_SECTION_R_FLASH_GETVERSION                  (SYSTEM_SECTION_RAM_FUNC) 
/* @} */
#else
#define FLASH_CFG_SECTION_R_FLASH_OPEN                        (SYSTEM_SECTION_CODE)
#define FLASH_CFG_SECTION_R_FLASH_CLOSE                       (SYSTEM_SECTION_CODE)
#define FLASH_CFG_SECTION_R_FLASH_WRITE                       (SYSTEM_SECTION_CODE)
#define FLASH_CFG_SECTION_R_FLASH_ERASE                       (SYSTEM_SECTION_CODE)
#define FLASH_CFG_SECTION_R_FLASH_BLANKCHECK                  (SYSTEM_SECTION_CODE)
#define FLASH_CFG_SECTION_R_FLASH_CONTROL                     (SYSTEM_SECTION_CODE)
#define FLASH_CFG_SECTION_R_FLASH_GETVERSION                  (SYSTEM_SECTION_CODE)
#endif /* (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 0) */

#ifdef  __cplusplus
}
#endif

/*******************************************************************************************************************//**
 * @} (end ingroup grp_device_hal_flash)
 **********************************************************************************************************************/

#endif /* R_FLASH_CFG_H */
