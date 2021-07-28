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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_lpm_cfg.h
 * Version      : 1.00
 * Description  : Device HAL LPM (Low Power Mode)
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 18.12.2019 0.40     First release
 *           04.03.2020 0.80     Changed the version number.
 *           27.05.2020 1.00     Updated the version number.
 *********************************************************************************************************************/

/******************************************************************************************************************//**
 @addgroup grp_device_hal_lpm
 @{
 *********************************************************************************************************************/
#include "r_system_cfg.h"
#include "r_system_api.h"

#ifndef R_LPM_CFG_H
#define R_LPM_CFG_H

/**********************************************************************************************************************
Configuration Options
**********************************************************************************************************************/

/******************************************************************************************************************//**
 * @brief Parameter check enable@n
 *        0 = Disable@n
 *        1 = Enable (Default)@n
 *********************************************************************************************************************/
#define LPM_CFG_PARAM_CHECKING_ENABLE   (1)

/******************************************************************************************************************//**
 * @brief Entering the critical section by masking interrupt@n
 *        0 = Disable@n
 *        1 = Enable (default)@n
 *        If this setting is enabled, it masks the interrupt by the R_LPM API and enters the critical section.
 *        If not, the user need to use R_SYS_EnterCriticalSection() or R_SYS_ExitCriticalSection() to 
 *        enter the critical section in the user application.
 *********************************************************************************************************************/
#define LPM_CFG_ENTER_CRITICAL_SECTION_ENABLE    (1)

/******************************************************************************************************************//**
 * @brief Register protection enable setting@n
 *        0 = Disable@n
 *        1 = Enable (default)@n
 *        If this setting is enabled, register protection is enable and disable by the R_LPM API.
 *        If not, the user need to use R_SYS_RegisterProtectEnable() or R_SYS_RegisterProtectDisable() to 
 *        perform protection settings in the user application.
 *********************************************************************************************************************/
#define LPM_CFG_REGISTER_PROTECTION_ENABLE       (1)

/******************************************************************************************************************//**
 * @name R_LPM_API_LOCATION_CONFIG
 *       Definition of R_LPM API location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define LPM_CFG_SECTION_R_LPM_GETVERSION                        (SYSTEM_SECTION_CODE)       /*!<  R_LPM_GetVersion() section */
#define LPM_CFG_SECTION_R_LPM_INITIALIZE                        (SYSTEM_SECTION_CODE)       /*!<  R_LPM_Initialize() section */
#define LPM_CFG_SECTION_R_LPM_MODULESTART                       (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_ModuleStart() section */
#define LPM_CFG_SECTION_R_LPM_MODULESTOP                        (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_ModuleStop() section */
#define LPM_CFG_SECTION_R_LPM_FASTMODULESTART                   (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_FastModuleStart() section */
#define LPM_CFG_SECTION_R_LPM_FASTMODULESTOP                    (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_FastModuleStop() section */
#define LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEALLPWONSET         (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_PowerSupplyModeAllpwonSet() section */
#define LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEEXFPWONSET         (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_PowerSupplyModeExfpwonSet() section */
#define LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEMINPWONSET         (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_PowerSupplyModeMinpwonSet() section */
#define LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEGET                (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_PowerSupplyModeGet() section */
#define LPM_CFG_SECTION_R_LPM_BACKBIASMODEENABLE                (SYSTEM_SECTION_CODE)       /*!<  R_LPM_BackBiasModeEnable() section */
#define LPM_CFG_SECTION_R_LPM_BACKBIASMODEDISABLE               (SYSTEM_SECTION_CODE)       /*!<  R_LPM_BackBiasModeDisable() section */
#define LPM_CFG_SECTION_R_LPM_BACKBIASMODEENABLESTATUSGET       (SYSTEM_SECTION_CODE)       /*!<  R_LPM_BackBiasModeEnableStatusGet() section */
#define LPM_CFG_SECTION_R_LPM_BACKBIASMODEENTRY                 (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_BackBiasModeEntry() section */
#define LPM_CFG_SECTION_R_LPM_BACKBIASMODEEXIT                  (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_BackBiasModeExit() section */
#define LPM_CFG_SECTION_R_LPM_BACKBIASMODEGET                   (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_BackBiasModeGet() section */
#define LPM_CFG_SECTION_R_LPM_SLEEPMODEENTRY                    (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_SleepModeEntry() section */
#define LPM_CFG_SECTION_R_LPM_SSTBYMODEENTRY                    (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_SSTBYModeEntry() section */
#define LPM_CFG_SECTION_R_LPM_SSTBYMODESETUP                    (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_SSTBYModeSetup() section */
#define LPM_CFG_SECTION_R_LPM_DSTBYMODEENTRY                    (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_DSTBYModeEntry() section */
#define LPM_CFG_SECTION_R_LPM_DSTBYMODESETUP                    (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_DSTBYModeSetup() section */
#define LPM_CFG_SECTION_R_LPM_DSTBYRESETSTATUSGET               (SYSTEM_SECTION_CODE)       /*!<  R_LPM_DSTBYResetStatusGet() section */
#define LPM_CFG_SECTION_R_LPM_RAMRETENTIONSET                   (SYSTEM_SECTION_CODE)       /*!<  R_LPM_RamRetentionSet() section */
#define LPM_CFG_SECTION_R_LPM_SNOOZESET                         (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_SnoozeSet() section */
#define LPM_CFG_SECTION_R_LPM_IOPOWERSUPPLYMODESET              (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_IOPowerSupplyModeSet() section */
#define LPM_CFG_SECTION_R_LPM_IOPOWERSUPPLYMODEGET              (SYSTEM_SECTION_RAM_FUNC)   /*!<  R_LPM_IOPowerSupplyModeGet() section */

/* @} */

/******************************************************************************************************************//**
 * @} (end ingroup grp_device_hal_lpm)
 *********************************************************************************************************************/

#endif /* R_LPM_CFG_H */
/* End of file (r_lpm_cfg.h) */

