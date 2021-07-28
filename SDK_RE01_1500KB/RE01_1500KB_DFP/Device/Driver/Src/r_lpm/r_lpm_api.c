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
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_lpm_api.c
 * Version      : 1.01
 * Description  : Device HAL LPM (Low Power Mode)
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 22.08.2018 0.50     First release
 *         : 14.09.2018 0.51     Implement measures to speed up processing.
 *                               R_LPM_PowerSupplyModeSet() changed function to separate function for each mode.
 *                                 - ALLPWON: R_LPM_PowerSupplyModeAllpwonSet()
 *                                 - EXFPWON: R_LPM_PowerSupplyModeExfpwonSet()
 *                                 - MINPWON: R_LPM_PowerSupplyModeMinpwonSet()
 *                               Change the error condition, transition from ALLPWON mode to MINPWON mode and
 *                               transition from MINPWON mode to ALLPWON mode are allowed.
 *                               Delete status wait processing in R_LPM_BackBiasModeEnable ()function.
 *                               Added R_LPM_BackBiasModeEnableStatusGet() function.
 *                               R_LPM_PowerSupplyModeAllpwonSet() function changed to return an error if the operation
 *                               clock exceeds 4 MHz.
 *                               Change the type of members included in the structure.
 *                                 structure: st_lpm_sstby_cfg_t , member: wup      , type: uint32_t <- int32_t
 *                                 structure: st_lpm_dstby_cfg_t , member: wup      , type: uint32_t <- int32_t
 *                                 structure: st_lpm_dstby_cfg_t , member: wup_edge , type: uint32_t <- int32_t
 *                                 structure: st_lpm_snooze_cfg_t, member: req      , type: uint32_t <- int32_t
 *                                 structure: st_lpm_snooze_cfg_t, member: snz_sstby, type: uint32_t <- int32_t
 *                               Change the type of the input parameter of the function.
 *                                 function: R_LPM_RamRetentionSet(), param: area, type: uint32_t <- int32_t
 *                                 function: R_LPM_IOPowerSupplyModeSet(), param: uint32_t vocr <- int32_t vocr_value
 *                               Change the return type of a function.
 *                                 function: R_LPM_IOPowerSupplyModeGet(), type: uint32_t <- uint8_t
 *                               Partly compatible with GSCE rules.
 *         : 19.12.2018 0.60     Change the type of the input parameter of the function.
 *                                 function: R_LPM_RamRetentionSet(), param: area, type: uint8_t <- uint32_t
 *                               Change the return type of a function.
 *                                 function: R_LPM_IOPowerSupplyModeGet(), type: uint8_t <- uin32_t
 *                               Delete SLCD.
 *                               Adapted to GSCE coding rules.
 *                                - Changed file name to r_lpm_api.c
 *                                - Changed include file name to r_system_api.h
 *         : 27.02.2019 0.61     Added R_LPM_GetVersion() function.
 *         : 24.04.2019 0.70     Change the comment for Doxygen.
 *         : 22.05.2019 0.72     Changed the comment for Doxygen.
 *                               Added LPM_MSTP_DIL to the member of gs_lpm_module_stop.
 *                               Added internal voltage discharge process in R_LPM_BackBiasModeEntry() function.
 *         : 24.07.2019 1.00     Changed the series name.
 *                               Added ISO2 reset release wait.
 *                               Correct a typo in a comment.
 *         : 13.11.2019 1.01     Fixed wrong RAM allocation condition for r_lpm_wait_release_reset_iso2() function.
 *                               Fixed wrong RAM allocation condition for r_lpm_check_locking_sw() function.
 *                               Delete unnecessary code.
 *                               Fixed error in definition bit length.
 *                               Insert comments in unpassed code.
 *********************************************************************************************************************/

/******************************************************************************************************************//**
 * @addtogroup grp_device_hal_lpm
 * @{
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "RE01_1500KB.h"
#include "r_system_api.h"
#include "r_lpm_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/**
 * @defgroup internal
 */
/* @{ */
#define R_LPM_PRV_OK                      (0)
#define R_LPM_PRV_ERROR                   (-1)

#define R_LPM_PRV_MSTP_EXCEPTION          (1U << 5)
#define R_LPM_PRV_MSTP_REG(x)             ((x) << 6)
#define R_LPM_PRV_MSTP_BIT(x)             (x)
#define R_LPM_PRV_MSTPCRA                 (0U)
#define R_LPM_PRV_MSTPCRB                 (1U)
#define R_LPM_PRV_MSTPCRC                 (2U)
#define R_LPM_PRV_MSTPCRD                 (3U)

#define R_LPM_PRV_DMAC_NUM_CHANNELS       (4UL)
#define R_LPM_PRV_GPT32_NUM_CHANNELS      (2UL)
#define R_LPM_PRV_GPT16_NUM_CHANNELS      (4UL)

#define R_LPM_PRV_SYSTEM_PWSTF_PMOD_MSK   (SYSTEM_PWSTF_ALLPWONM_Msk | SYSTEM_PWSTF_EXFPWONM_Msk | SYSTEM_PWSTF_MINPWONM_Msk)

#define R_LPM_PRV_SHIFTNUM_DPSIFR1        (8U)
#define R_LPM_PRV_SHIFTNUM_DPSIER1        (8U)
#define R_LPM_PRV_SHIFTNUM_DPSIEGR1       (8U)

#define R_LPM_PRV_FLAG_NOT_INITILIZED     (0U)
#define R_LPM_PRV_FLAG_INITILIZED         (1U)
#define R_LPM_PRV_FLAG_SNZ_DISABLE        (0U)
#define R_LPM_PRV_FLAG_SNZ_ENABLE         (1U)

#define R_LPM_PRV_ALLPWON_ENTRY_MAX_FREQ  (4000000U)    /* 4MHz */

#define R_LPM_PRV_REG_PWSTCR_SSTBY_DSTBY  (0x00U)
#define R_LPM_PRV_REG_PWSTCR_ALLPWON      (0x01U)
#define R_LPM_PRV_REG_PWSTCR_EXFPWON      (0x02U)
#define R_LPM_PRV_REG_PWSTCR_MINPWON      (0x03U)
#define R_LPM_PRV_REG_PWSTCR_NORMAL       (0x04U)
#define R_LPM_PRV_REG_PWSTCR_BOOST        (0x05U)
#define R_LPM_PRV_REG_PWSTCR_VBB          (0x06U)

#define R_LPM_PRV_REG_SNZCR_SNZOFF        (0x00U)

#define R_LPM_PRV_REG_SOSCCR_SOSCON       (0x00U)
#define R_LPM_PRV_REG_LOCOCR_LOCOON       (0x00U)

#define R_LPM_PRV_REG_DPSIFR0_ALLCLEAR    (0x00U)
#define R_LPM_PRV_REG_DPSIFR1_ALLCLEAR    (0x00U)

#define R_LPM_PRV_REG_DPSBYCR_DSTBYOFF    (0x00U)

#define R_LPM_PRV_REG_SCKSCR_LOCO         (0x02U)
#define R_LPM_PRV_REG_SCKSCR_SOSC         (0x04U)

#define R_LPM_PRV_REG_VBBCR_VBBOFF        (0x00U)

#define R_LPM_PRV_REG_SOPCCR_SUBSPEED     (0x01U)

#define R_LPM_PRV_REG_VBBST_DISABLE       (0x00U)
#define R_LPM_PRV_REG_VBBST_ENABLE        (0x01U)

#define R_LPM_PRV_REG_SBYCR_SLEEP         (0x0000U)

/** Define the location of gs_lpm_module_stop. */
#if (LPM_CFG_SECTION_R_LPM_MODULESTART == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_MODULESTOP == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_IOPOWERSUPPLYMODESET == SYSTEM_SECTION_RAM_FUNC)
#define R_LPM_PRV_STATIC_DATA_LOCATION_G_LPM_MODULESTOP     __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define R_LPM_PRV_STATIC_DATA_LOCATION_G_LPM_MODULESTOP
#endif

/** Define the location of gsp_lpm_mstp. */
#if (LPM_CFG_SECTION_R_LPM_MODULESTART == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_MODULESTOP == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_IOPOWERSUPPLYMODESET == SYSTEM_SECTION_RAM_FUNC)
#define R_LPM_PRV_STATIC_DATA_LOCATION_G_LPM_MSTP     __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define R_LPM_PRV_STATIC_DATA_LOCATION_G_LPM_MSTP
#endif

/** Define the location of r_lpm_check_locking_sw() function. */
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
#if (LPM_CFG_SECTION_R_LPM_MODULESTOP == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_FASTMODULESTOP == SYSTEM_SECTION_RAM_FUNC)
#define R_LPM_PRV_STATIC_FUNC_LOCATION_R_LPM_CHECKLOCKINGSW     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define R_LPM_PRV_STATIC_FUNC_LOCATION_R_LPM_CHECKLOCKINGSW
#endif
#endif

#if (LPM_CFG_REGISTER_PROTECTION_ENABLE == 1)
#define R_LPM_PRV_REGISTER_PROTECT_ENABLE(x)   (R_SYS_RegisterProtectEnable(x))
#define R_LPM_PRV_REGISTER_PROTECT_DISABLE(x)  (R_SYS_RegisterProtectDisable(x))
#else
#define R_LPM_PRV_REGISTER_PROTECT_ENABLE(x)
#define R_LPM_PRV_REGISTER_PROTECT_DISABLE(x)
#endif /* (LPM_CFG_REGISTER_PROTECTION_ENABLE == 1) */

#if (LPM_CFG_ENTER_CRITICAL_SECTION_ENABLE == 1)
#define R_LPM_PRV_ENTER_CRITICAL_SECTION   (R_SYS_EnterCriticalSection())
#define R_LPM_PRV_EXIT_CRITICAL_SECTION    (R_SYS_ExitCriticalSection())
#else
#define R_LPM_PRV_ENTER_CRITICAL_SECTION
#define R_LPM_PRV_EXIT_CRITICAL_SECTION
#endif /* (LPM_CFG_ENTER_CRITICAL_SECTION_ENABLE == 1) */

/* @} */

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/**
 * @addtogroup internal
 */
/* @{ */
/** @brief Definition of module stop bit. */
typedef struct st_lpm_mstp_bit
{
    uint8_t bit         : 5;    /*!< Which bit in MSTP register */
    uint8_t exception   : 1;    /*!< Special processing required */
    uint8_t reg         : 2;    /*!< Which MSTP register */
} st_lpm_mstp_bit_t;


/** @brief Definition of module stop data. */
typedef union u_lpm_mstp_data
{
    uint8_t byte;               /*!< Byte access for comparison */
    st_lpm_mstp_bit_t bit;
} u_lpm_mstp_data_t;

/* @} */

/**********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/
/**
 * @addtogroup internal
 */
/* @{ */
/** Module stop bit definitions for each e_lpm_mstp_t enum value. */
static const uint8_t gs_lpm_module_stop[] R_LPM_PRV_STATIC_DATA_LOCATION_G_LPM_MODULESTOP =
{
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRA) | R_LPM_PRV_MSTP_BIT(22U) | R_LPM_PRV_MSTP_EXCEPTION,     /* LPM_MSTP_DTC_DMAC */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(5U),                           /* LPM_MSTP_IRDA */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(6U),                           /* LPM_MSTP_QSPI */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(8U),                           /* LPM_MSTP_RIIC1 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(9U),                           /* LPM_MSTP_RIIC0 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(11U) | R_LPM_PRV_MSTP_EXCEPTION,     /* LPM_MSTP_USB */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(18U),                          /* LPM_MSTP_SPI1 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(19U),                          /* LPM_MSTP_SPI0 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(22U),                          /* LPM_MSTP_SCI9 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(26U),                          /* LPM_MSTP_SCI5 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(27U),                          /* LPM_MSTP_SCI4 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(28U),                          /* LPM_MSTP_SCI3 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(29U),                          /* LPM_MSTP_SCI2 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(30U),                          /* LPM_MSTP_SCI1 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRB) | R_LPM_PRV_MSTP_BIT(31U),                          /* LPM_MSTP_SCI0 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(0U),                           /* LPM_MSTP_CAC */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(1U),                           /* LPM_MSTP_CRC */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(10U),                          /* LPM_MSTP_LED */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(13U),                          /* LPM_MSTP_DOC */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(14U),                          /* LPM_MSTP_ELC */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(15U),                          /* LPM_MSTP_DIV */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(22U),                          /* LPM_MSTP_DIL */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(25U),                          /* LPM_MSTP_MLCD */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(26U),                          /* LPM_MSTP_GDT */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRC) | R_LPM_PRV_MSTP_BIT(31U),                          /* LPM_MSTP_TSIP_LITE */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(0U),                           /* LPM_MSTP_LST */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(1U),                           /* LPM_MSTP_TMR */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(2U),                           /* LPM_MSTP_AGT1 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(3U),                           /* LPM_MSTP_AGT0 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(4U),                           /* LPM_MSTP_LPG */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(5U) | R_LPM_PRV_MSTP_EXCEPTION,      /* LPM_MSTP_GPT320_321 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(6U) | R_LPM_PRV_MSTP_EXCEPTION,      /* LPM_MSTP_GPT162_165 */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(7U),                           /* LPM_MSTP_CCC */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(8U),                           /* LPM_MSTP_MTDV */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(14U),                          /* LPM_MSTP_POEG */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(16U),                          /* LPM_MSTP_S14AD */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(17U),                          /* LPM_MSTP_VREF */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(20U),                          /* LPM_MSTP_R12DA */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(22U),                          /* LPM_MSTP_TEMPS */
    R_LPM_PRV_MSTP_REG(R_LPM_PRV_MSTPCRD) | R_LPM_PRV_MSTP_BIT(28U)                           /* LPM_MSTP_ACMP */
};

/** Module stop register addresses. */
static volatile uint32_t * const gsp_lpm_mstp[] R_LPM_PRV_STATIC_DATA_LOCATION_G_LPM_MSTP =
{
    &(SYSTEM->MSTPCRA),
    &(MSTP->MSTPCRB),
    &(MSTP->MSTPCRC),
    &(MSTP->MSTPCRD)
};

/** Flag to prevent the calling more than once from application. */
static volatile uint8_t gs_lpm_init_flg = R_LPM_PRV_FLAG_NOT_INITILIZED;

static volatile uint8_t gs_lpm_snz_flag = R_LPM_PRV_FLAG_SNZ_DISABLE;
static volatile uint16_t gs_lpm_sbycr_ssbymp_retpm = 0U;
static volatile uint8_t gs_lpm_snzcr_snzdtcen_rxdregen = 0U;
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
static volatile e_lpm_power_supply_mode_t gs_lpm_sstby_start_ope = LPM_POWER_SUPPLY_MODE_ALLPWON;
#endif

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
static bool r_lpm_check_locking_sw(e_lpm_mstp_t module) R_LPM_PRV_STATIC_FUNC_LOCATION_R_LPM_CHECKLOCKINGSW;
#endif /* LPM_CFG_PARAM_CHECKING_ENABLE */

#if defined(__GNUC__)
#if (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEEXFPWONSET == SYSTEM_SECTION_RAM_FUNC) ||\
    (LPM_CFG_SECTION_R_LPM_POWERSUPPLYMODEALLPWONSET == SYSTEM_SECTION_RAM_FUNC)
void r_lpm_wait_release_reset_iso2(void)  __attribute__ ((weak, section(".ramfunc")));
#else
void r_lpm_wait_release_reset_iso2(void)  __attribute__ ((weak));
#endif
#endif /* __GNUC__ */

/* @} */

/******************************************************************************************************************//**
 * @brief This function gets the version number of R_LPM.
 * @retval uint32_t Version number
 *********************************************************************************************************************/
uint32_t R_LPM_GetVersion (void)
{
    uint32_t const version = ((LPM_VERSION_MAJOR << 16) | LPM_VERSION_MINOR);

    return (version);
}/* End of function R_LPM_GetVersion() */

/******************************************************************************************************************//**
 * @brief This function initialize r_lpm function.
 *********************************************************************************************************************/
void R_LPM_Initialize (void)
{
    if (R_LPM_PRV_FLAG_NOT_INITILIZED == gs_lpm_init_flg)
    {
        gs_lpm_init_flg                 = R_LPM_PRV_FLAG_INITILIZED;
        gs_lpm_snz_flag                 = R_LPM_PRV_FLAG_SNZ_DISABLE;
        gs_lpm_sbycr_ssbymp_retpm       = 0U;
        gs_lpm_snzcr_snzdtcen_rxdregen  = 0U;
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
        gs_lpm_sstby_start_ope          = LPM_POWER_SUPPLY_MODE_ALLPWON;
#endif
    }
    return ;
} /* End of function R_LPM_Initialize() */

/******************************************************************************************************************//**
 * @brief This function starts module (cancel module stop). Starting a module enables clocks to the peripheral and 
 * allows registers to set.
 * @param[in] module     Defined by e_lpm_mstp_t.
 * @retval 0             Success : The specified module is started.
 * @retval -1            Error   : The specified module is not started. An error occurs if the following conditions are
 * detected.
 * - The input parameter "module" exceeds the definition range of e_lpm_mstp_t.
 * - LPM_MSTP_USB is specified for input parameter "module" and it is not operating in BOOST mode.
 * @details This function uses the R_SYS_EnterCriticalSection() function and R_SYS_ExitCriticalSection() function of
 * the R_SYSTEM driver when "1" is set in LPM_CFG_ENTER_CRITICAL_SECTION_ENABLE. Before this function is executed, it
 * is necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_ModuleStart (e_lpm_mstp_t module)
{
    u_lpm_mstp_data_t data = {0};
    volatile uint32_t * p_mstp_base = 0;
    uint32_t mask = 0UL;

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (module >= LPM_MSTP_NUM)
    {
        return R_LPM_PRV_ERROR;
    }
#endif

    data.byte = gs_lpm_module_stop[module];
    p_mstp_base = gsp_lpm_mstp[data.bit.reg];
    mask = (uint32_t)(1UL << (data.bit.bit));

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (LPM_MSTP_USB == module)
    {
        if (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_BOOSTM_Msk))
        {
            /* Prohibited to cancel the USB module stop state except boost mode. */
            return R_LPM_PRV_ERROR;
        }
    }
#endif

    /* Clear MSTP bit if stop parameter is false. */
    mask = ~mask;

    R_LPM_PRV_ENTER_CRITICAL_SECTION;

    (*p_mstp_base) &= mask;

    R_LPM_PRV_EXIT_CRITICAL_SECTION;

    return R_LPM_PRV_OK;
} /* End of function R_LPM_ModuleStart() */

/******************************************************************************************************************//**
 * @brief This function stops module (enter module stop). Stopping a module disables clocks to the peripheral to save
 * power.
 * @param[in] module     Defined by e_lpm_mstp_t.
 * @retval 0             Success : The specified module is stopped.
 * @retval -1            Error   : The specified module is not stopped. An error occurs if the following conditions are
 * detected.
 * - The input parameter "module" exceeds the definition range of e_lpm_mstp_t.
 * - Other peripheral modules sharing the module stop bit with the module specified by the input parameter "module" are
 * in operation. Whether a peripheral module is in operation or not is determined by the execution state of
 * the R_SYS_ResourceLock function.
 * @details Before this function is executed, it is necessary to execute the R_SYS_Initialize() function. This function
 * uses the R_SYS_ResourceLock() function and R_SYS_ResourceUnLock() function of the R_SYSTEM driver. This function 
 * uses the R_SYS_EnterCriticalSection() function and R_SYS_ExitCriticalSection() function of the R_SYSTEM driver when
 * "1" is set in LPM_CFG_ENTER_CRITICAL_SECTION_ENABLE.
 *********************************************************************************************************************/
int32_t R_LPM_ModuleStop (e_lpm_mstp_t module)
{
    u_lpm_mstp_data_t data = {0};
    volatile uint32_t * p_mstp_base = 0;
    uint32_t mask = 0UL;

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (module >= LPM_MSTP_NUM)
    {
        return R_LPM_PRV_ERROR;
    }
#endif

    data.byte = gs_lpm_module_stop[module];
    p_mstp_base = gsp_lpm_mstp[data.bit.reg];
    mask = (uint32_t)(1UL << (data.bit.bit));

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (data.bit.exception)
    {
        if (r_lpm_check_locking_sw(module) != true)
        {
            /* Module stop bit is shared, so module is not stopped because other modules are running. */
            return R_LPM_PRV_ERROR;
        }
    }
#endif

    R_LPM_PRV_ENTER_CRITICAL_SECTION;

    /* Set MSTP bit if stop parameter is true and the bit is not shared with other channels. */
    (*p_mstp_base) |= mask;

    R_LPM_PRV_EXIT_CRITICAL_SECTION;

    return R_LPM_PRV_OK;
} /* End of function R_LPM_ModuleStop() */

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
/******************************************************************************************************************//**
 * @brief When one module stop bit controls multiple IP stops, make sure that the module other than the specified 
 * module is used.
 * @param[in] module     Defined by e_lpm_mstp_t.
 * @retval true          The module can be stopped because a module other than the specified module is not used.
 * @retval false         The module can not be stopped because a module other than the specified module is being used.
 *********************************************************************************************************************/
static bool r_lpm_check_locking_sw (e_lpm_mstp_t module)
{
    uint32_t channel     = 0U;
    uint32_t channel_max = 0U;
    uint32_t lock_num    = 0U;
    e_system_mcu_lock_t mcu_lock = SYSTEM_LOCK_NUM;

    switch (module)
    {
        case LPM_MSTP_DTC_DMAC:
        {
            channel_max = R_LPM_PRV_DMAC_NUM_CHANNELS;
            mcu_lock = SYSTEM_LOCK_DMAC0;
        }
        break;
        case LPM_MSTP_GPT320_321:
        {
            channel_max = R_LPM_PRV_GPT32_NUM_CHANNELS;
            mcu_lock = SYSTEM_LOCK_GPT320;
        }
        break;

        case LPM_MSTP_GPT162_165:
        {
            channel_max = R_LPM_PRV_GPT16_NUM_CHANNELS;
            mcu_lock = SYSTEM_LOCK_GPT162;
        }
        break;

        case LPM_MSTP_USB:
        {
            return true;
        }
        break;

        default:
        {
            return false; // Usually not passed due to argument error check from driver
        }
        break;
    }

    for (channel = 0U; channel < channel_max; channel++)
    {
        if ((-1) == R_SYS_ResourceLock((e_system_mcu_lock_t)(mcu_lock + channel)))
        {
            lock_num++;
        }
        else
        {
            R_SYS_ResourceUnlock((e_system_mcu_lock_t)(mcu_lock + channel));
        }
    }

    if (LPM_MSTP_DTC_DMAC == module)
    {
        if ((-1) == R_SYS_ResourceLock(SYSTEM_LOCK_DTC))
        {
            lock_num++;
        }
        else
        {
            R_SYS_ResourceUnlock(SYSTEM_LOCK_DTC);
        }
    }

    if (0U != lock_num)
    {
        return false;
    }

    return true;

} /* End of function r_lpm_check_locking_sw() */
#endif /* LPM_CFG_PARAM_CHECKING_ENABLE */

/******************************************************************************************************************//**
 * @brief This function starts modules (cancel module stop). Starting modules enable clocks to the peripheral and allows
 * registers to be set.
 * @param[in] module_a   Defined by @ref LPM_FAST_MSTPT_A.
 * @param[in] module_b   Defined by @ref LPM_FAST_MSTPT_B.
 * @param[in] module_c   Defined by @ref LPM_FAST_MSTPT_C.
 * @param[in] module_d   Defined by @ref LPM_FAST_MSTPT_D.
 * @retval 0             Success : The specified modules are started.
 * @retval -1            Error   : The specified modules are not started. An error occurs if the following condition 
 * is detected.
 * - LPM_MSTP_USB is specified for input parameter "module_b" and it is not operating in BOOST mode.
 * @details This function can start multiple modules. Start the module set in the input parameters "module_a",
 * "module_b", "module_c" and "module_d". Multiple modules can be set for each input parameter by logical OR. \n\n
 * This function uses the R_SYS_EnterCriticalSection() function and R_SYS_ExitCriticalSection() function of the 
 * R_SYSTEM driver when "1" is set in LPM_CFG_ENTER_CRITICAL_SECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_FastModuleStart (uint32_t module_a, uint32_t module_b, uint32_t module_c, uint32_t module_d)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (0U != (module_b & LPM_FAST_MSTP_B_USB))
    {
        if (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_BOOSTM_Msk))
        {
            return R_LPM_PRV_ERROR;
        }
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_LPM_PRV_ENTER_CRITICAL_SECTION;

    SYSTEM->MSTPCRA &= (~(module_a & LPM_FAST_MSTP_A_ALL));
    MSTP->MSTPCRB   &= (~(module_b & LPM_FAST_MSTP_B_ALL));
    MSTP->MSTPCRC   &= (~(module_c & LPM_FAST_MSTP_C_ALL));
    MSTP->MSTPCRD   &= (~(module_d & LPM_FAST_MSTP_D_ALL));

    R_LPM_PRV_EXIT_CRITICAL_SECTION;

    return R_LPM_PRV_OK;
} /* End of function R_LPM_FastModuleStart() */

/******************************************************************************************************************//**
 * @brief This function stops modules (enter module stop). Stopping modules disable clocks to the peripheral to save
 * power.
 * @param[in] module_a   Defined by @ref LPM_FAST_MSTPT_A.
 * @param[in] module_b   Defined by @ref LPM_FAST_MSTPT_B.
 * @param[in] module_c   Defined by @ref LPM_FAST_MSTPT_C.
 * @param[in] module_d   Defined by @ref LPM_FAST_MSTPT_D.
 * @retval 0             Success : The specified modules are stopped.
 * @retval -1            Error   : The specified modules are not stopped. An error occurs if the following condition
 * is detected.
 * - Other peripheral modules sharing the module stop bit with the module set for the input parameter "module_a",
 * "module_b", "module_c" and "module_d" are in operation. Whether a peripheral module is in operation or not is
 * determined by the execution state of the R_SYS_ResourceLock function.
 * @details This function can stop multiple modules. Stop the module set in the input parameters "module_a", 
 * "module_b", "module_c" and "module_d". Multiple factors can be set for each input parameter by logical OR. \n\n
 * Before this function is executed, it is necessary to execute the R_SYS_Initialize() function. This function uses
 * the R_SYS_ResourceLock() function and R_SYS_ResourceUnLock() function of the R_SYSTEM driver. This function uses
 * the R_SYS_EnterCriticalSection() function and R_SYS_ExitCriticalSection() function of the R_SYSTEM driver when "1"
 * is set in LPM_CFG_ENTER_CRITICAL_SECTION_ENABLE.
 *********************************************************************************************************************/
int32_t R_LPM_FastModuleStop (uint32_t module_a, uint32_t module_b, uint32_t module_c, uint32_t module_d)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (0U != (module_a & LPM_FAST_MSTP_A_DTC_DMAC))     // DTC or DMAC
    {
        if (r_lpm_check_locking_sw(LPM_MSTP_DTC_DMAC) != true)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    if (0U != (module_d & LPM_FAST_MSTP_D_GPT320_321))      // GPT320 or GPT321
    {
        if (r_lpm_check_locking_sw(LPM_MSTP_GPT320_321) != true)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    if (0U != (module_d & LPM_FAST_MSTP_D_GPT162_165))      // GPT162, GPT163, GPT164 or GPT165
    {
        if (r_lpm_check_locking_sw(LPM_MSTP_GPT162_165) != true)
        {
            return R_LPM_PRV_ERROR;
        }
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    R_LPM_PRV_ENTER_CRITICAL_SECTION;

    SYSTEM->MSTPCRA |= (module_a & LPM_FAST_MSTP_A_ALL);
    MSTP->MSTPCRB   |= (module_b & LPM_FAST_MSTP_B_ALL);
    MSTP->MSTPCRC   |= (module_c & LPM_FAST_MSTP_C_ALL);
    MSTP->MSTPCRD   |= (module_d & LPM_FAST_MSTP_D_ALL);

    R_LPM_PRV_EXIT_CRITICAL_SECTION;
    
    return R_LPM_PRV_OK;
} /* End of function R_LPM_FastModuleStop() */

/******************************************************************************************************************//**
 * @brief This function supports power mode transition to ALLPWON.
 * @retval 0             Success : Transition to the specified power supply mode.
 * @retval -1            Error   : Not transition to the specified power supply mode. An error occurs if the following
 * conditions are detected.
 * - Power supply mode transition can not be set to hardware.
 * - It is operating with a clock exceeding 4 MHz.
 * @details When this function is executed in the MINPWON mode state, this function makes a transition from the MINPWON
 * mode to the EXFPWON mode and then makes a transition from the EXFPWON mode to the ALLPWON mode.\n\n
 * This function executes the r_lpm_wait_release_reset_iso2 () function after transition from the MINPWON mode to the
 * ALLPWON mode in order to wait for reset release of the ISO2 area stopped in the MINPWON mode. This process is
 * required when the PCLKB division setting is 4 to 64 division. The r_lpm_wait_release_reset_iso2 () function is
 * implemented as a WEAK function in the R_LPM driver. It can disable the corresponding function in R_LPM driver by
 * implementing the non-weak function with the same name.\n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of
 * the R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_PowerSupplyModeAllpwonSet (void) // @suppress("Function length")
{
    uint8_t   current_mode = (SYSTEM->PWSTF & (uint8_t)R_LPM_PRV_SYSTEM_PWSTF_PMOD_MSK);

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    uint32_t freq_hz = 0U;

    R_SYS_SystemClockFreqGet(&freq_hz);
    if (R_LPM_PRV_ALLPWON_ENTRY_MAX_FREQ < freq_hz)
    {
        return R_LPM_PRV_ERROR;
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    if (SYSTEM_PWSTF_MINPWONM_Msk == current_mode)
    {
        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the software standby mode */
        SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
        SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;

        /* Disable the snooze mode */
        SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;

        /* Set the exf power supply mode */
        SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_EXFPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_EXFPWON != SYSTEM->PWSTCR)
        {
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to EXF power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_EXFPWONM_Msk));

        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the all power supply mode */
        SYSTEM->PWSTCR = R_LPM_PRV_REG_PWSTCR_ALLPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_ALLPWON != SYSTEM->PWSTCR)
        {
            r_lpm_wait_release_reset_iso2();
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to ALL power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_ALLPWONM_Msk));

        r_lpm_wait_release_reset_iso2();

    }
    else if (SYSTEM_PWSTF_EXFPWONM_Msk == current_mode)
    {
        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the software standby mode */
        SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
        SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;

        /* Disable the snooze mode */
        SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;

        /* Set the all power supply mode */
        SYSTEM->PWSTCR = R_LPM_PRV_REG_PWSTCR_ALLPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_ALLPWON != SYSTEM->PWSTCR)
        {
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to ALL power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_ALLPWONM_Msk));
    }
    else
    {
        ;    // do nothing
    }

    return R_LPM_PRV_OK;
} /* End of function R_LPM_PowerSupplyModeAllpwonSet() */

/******************************************************************************************************************//**
 * @brief This function supports power mode transition to EXFPWON.
 * @retval 0             Success : Transition to the specified power supply mode.
 * @retval -1            Error   : Not transition to the specified power supply mode. An error occurs if the following
 * conditions are detected.
 * - It is operating in BOOST mode.
 * - Power supply mode transition can not be set to hardware.
 * @details This function executes the r_lpm_wait_release_reset_iso2 () function after transition from the MINPWON mode
 * to the EXFPWON mode in order to wait for reset release of the ISO2 area stopped in the MINPWON mode. This process
 * is required when the PCLKB division setting is 4 to 64 division. The r_lpm_wait_release_reset_iso2 () function is
 * implemented as a WEAK function in the R_LPM driver. It can disable the corresponding function in R_LPM driver by
 * implementing the non-weak function with the same name.\n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function 
 * of the R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it
 * is necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_PowerSupplyModeExfpwonSet (void)
{
    uint8_t   current_mode = (SYSTEM->PWSTF & (uint8_t)R_LPM_PRV_SYSTEM_PWSTF_PMOD_MSK);

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)

    if (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_BOOSTM_Msk))
    {
        return R_LPM_PRV_ERROR;
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    if (SYSTEM_PWSTF_ALLPWONM_Msk == current_mode)
    {
        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the software standby mode */
        SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
        SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;

        /* Disable the snooze mode */
        SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;

        /* Set the exf power supply mode */
        SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_EXFPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_EXFPWON != SYSTEM->PWSTCR)
        {
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to EXF power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_EXFPWONM_Msk));

    }
    else if (SYSTEM_PWSTF_MINPWONM_Msk == current_mode)
    {
        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the software standby mode */
        SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
        SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;

        /* Disable the snooze mode */
        SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;

        /* Set the exf power supply mode */
        SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_EXFPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_EXFPWON != SYSTEM->PWSTCR)
        {
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to EXF power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_EXFPWONM_Msk));

        r_lpm_wait_release_reset_iso2();

    }
    else
    {
        ;    // do nothing
    }

    return R_LPM_PRV_OK;
} /* End of function R_LPM_PowerSupplyModeExfpwonSet() */

/******************************************************************************************************************//**
 * @brief This function supports power mode transition to MINPWON.
 * @retval 0             Success : Transition to the specified power supply mode.
 * @retval -1            Error   : Not transition to the specified power supply mode. An error occurs if the following
 * conditions are detected.
 * - It is operating in BOOST mode.
 * - Power supply mode transition can not be set to hardware.
 * @details When this function is executed in the state of ALLPWON mode, this function makes a transition to ALLPWON
 * mode or EXFPWON mode and then makes transition from EXFPWON mode to MINPWON mode.\n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of
 * the R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_PowerSupplyModeMinpwonSet (void) // @suppress("Function length")
{
    uint8_t   current_mode = (SYSTEM->PWSTF & (uint8_t)R_LPM_PRV_SYSTEM_PWSTF_PMOD_MSK);

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)

    if (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_BOOSTM_Msk))
    {
        return R_LPM_PRV_ERROR;
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    if (SYSTEM_PWSTF_ALLPWONM_Msk == current_mode)
    {
        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the software standby mode */
        SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
        SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;

        /* Disable the snooze mode */
        SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;

        /* Set the exf power supply mode */
        SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_EXFPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_EXFPWON != SYSTEM->PWSTCR)
        {
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to EXF power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_EXFPWONM_Msk));

        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the min power supply mode */
        SYSTEM->PWSTCR = R_LPM_PRV_REG_PWSTCR_MINPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_MINPWON != SYSTEM->PWSTCR)
        {
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to MIN power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_MINPWONM_Msk));

    }
    else if (SYSTEM_PWSTF_EXFPWONM_Msk == current_mode)
    {

        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Set the software standby mode */
        SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
        SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;

        /* Disable the snooze mode */
        SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;

        /* Set the min power supply mode */
        SYSTEM->PWSTCR = R_LPM_PRV_REG_PWSTCR_MINPWON;

        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

        /* Confirm that settings are accepted */
        if (R_LPM_PRV_REG_PWSTCR_MINPWON != SYSTEM->PWSTCR)
        {
            return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
        }

        /* Execute WFE instruction */
        /* Wait the transition to MIN power supply mode */
        do
        {
            __WFE();
        }
        while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_MINPWONM_Msk));
    }
    else
    {
        ;    // do nothing
    }

    return R_LPM_PRV_OK;
} /* End of function R_LPM_PowerSupplyModeMinpwonSet() */

/******************************************************************************************************************//**
 * @brief This function gets the state of power supply mode.
 * @retval LPM_POWER_SUPPLY_MODE_ALLPWON   Operating in ALLPWON mode
 * @retval LPM_POWER_SUPPLY_MODE_EXFPWON   Operating in EXFPWON mode
 * @retval LPM_POWER_SUPPLY_MODE_MINPWON   Operating in MINPWON mode
 *********************************************************************************************************************/
e_lpm_power_supply_mode_t R_LPM_PowerSupplyModeGet (void)
{
    return (e_lpm_power_supply_mode_t)(SYSTEM->PWSTF & (uint8_t)R_LPM_PRV_SYSTEM_PWSTF_PMOD_MSK);
} /* End of function R_LPM_PowerSupplyModeGet() */

/******************************************************************************************************************//**
 * @brief This function is the WEAK function. Confirm that the reset of ISO2 is completed by waiting for the write to
 * the ISO2 register to be reflected.
 *********************************************************************************************************************/
#if defined(__ICCARM__)
__weak void r_lpm_wait_release_reset_iso2 (void)
#elif defined(__GNUC__)
void r_lpm_wait_release_reset_iso2 (void)
#endif
{
    do
    {
        S14AD->ADEDDMY0 = 1;
    }
    while (1U != S14AD->ADEDDMY0);

    do
    {
        S14AD->ADEDDMY0 = 0;
    }
    while (0U != S14AD->ADEDDMY0);

    return ;
} /* End of function r_lpm_wait_release_reset_iso2() */

/******************************************************************************************************************//**
 * @brief This function sets up Back Bias control. 
 * @param[in] clock      Set the clock for Back Bias control.
 * @retval 0             Success : Setup of Back Bias control succeeded.
 * @retval -1            Error   : Setup of Back Bias control failed. An error occurs if the following conditions are
 * detected.
 * - Specified clock is not oscillating.
 * - The input parameter "clock" exceeds the definition range of e_lpm_vbb_clock_t.
 * @details Before this function is executed, the clock used in Back Bias mode must be oscillated. This function sets
 * enable of Back Bias control. But this function does not wait for setup completion. Therefore, after executing this
 * function, before executing Entry to Back Bias mode, it is necessary to execute R_LPM_BackBiasModeEnableStatusGet()
 * function to confirm that setup is completed.\n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_BackBiasModeEnable (e_lpm_vbb_clock_t clock)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (LPM_VBB_CLOCK_SOSC < clock)
    {
        return R_LPM_PRV_ERROR;
    }
    if (LPM_VBB_CLOCK_SOSC == clock)
    {
        if (R_LPM_PRV_REG_SOSCCR_SOSCON != SYSTEM->SOSCCR)
        {
            return R_LPM_PRV_ERROR;
           }
    }
    if (LPM_VBB_CLOCK_LOCO == clock)
    {
        if (R_LPM_PRV_REG_LOCOCR_LOCOON != SYSTEM->LOCOCR)
        {
            return R_LPM_PRV_ERROR;
        }
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->VBBCR = (uint8_t)(SYSTEM_VBBCR_VBBEN_Msk | clock);

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    return R_LPM_PRV_OK;
} /* End of function R_LPM_BackBiasModeEnable() */

/******************************************************************************************************************//**
 * @brief This function cancels the setup state of Back Bias mode. 
 * @retval 0             Success : Back Bias mode cancel succeeded.
 * @retval -1            Error   : Back Bias mode cancel failed. An error occurs if the following condition is
 * detected.
 * - Operating in Back Bias mode.
 * @details Before this function is executed, operation in Back Bias mode must be stopped. \n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_BackBiasModeDisable (void)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_VBBM_Msk))
    {
        return R_LPM_PRV_ERROR;
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->VBBCR = R_LPM_PRV_REG_VBBCR_VBBOFF;

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    return R_LPM_PRV_OK;
} /* End of function R_LPM_BackBiasModeDisable() */

/******************************************************************************************************************//**
 * @brief This function gets the setup status of Back Bias control.  
 * @retval LPM_VBB_DISABLED   Back Bias control setup is not complete.
 * @retval LPM_VBB_ENABLED    Back Bias control setup is completed.
 * @details After executing the R_LPM_BackBiasModeEnable() function, please confirm that setup is completed with this
 * function before entering Back Bias mode.
 *********************************************************************************************************************/
e_lpm_vbb_enable_status_t R_LPM_BackBiasModeEnableStatusGet (void)
{
    return (e_lpm_vbb_enable_status_t)SYSTEM->VBBST;
} /* End of function R_LPM_BackBiasModeEnableStatusGet() */

/******************************************************************************************************************//**
 * @brief This function transits to Back Bias mode.
 * @retval 0             Success : Back Bias mode transition succeeded.
 * @retval -1            Error   : Back Bias mode transition failed. An error occurs if the following conditions are
 * detected.
 * - Back Bias control setup is incomplete.
 * - Operating in BOOST mode.
 * - It is not operating in Subosc-Speed mode.
 * - The specified clock in setup is not set to ICLK.
 * - Back Bias mode transition can not be set to hardware.
 * @details Before this function is executed, it is necessary to execute the R_LPM_BackBiasModeEnable() function to
 * complete the setup. Before this function is executed, ICLK must be switched to the clock specified by the
 * R_LPM_BackBiasModeEnable() function. Before this function is executed, it is necessary to execute the
 * R_SYS_32kHzSpeedModeSet() function and set Subosc-Speed mode.\n\n
 * This function uses the R_SYS_SoftwareDelay() function and the local function of the R_SYSTEM driver. Before this
 * function is executed, it is necessary to execute the R_SYS_Initialize() function.
 * @note There is following restriction when using the WS1 device.
 * - After executing the transition to Software Standby mode in Normal operation mode with SOSC-Speed, if you change to
 * Back Bias mode without changing the speed mode, you can not recover from Software Standby mode. Before switching to
 * the Back Bias mode, processing to set SOSC-Speed mode again after switching to Low-Speed is performed. This function
 * uses the R_SYS_LowSpeedModeSet() function and R_SYS_32kHzSpeedModeSet() function of the R_SYSTEM driver.
 * (Restriction No.60)
 *********************************************************************************************************************/
int32_t R_LPM_BackBiasModeEntry (void)
{
    bool boost_flag = false;

#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (R_LPM_PRV_REG_VBBST_DISABLE == SYSTEM->VBBST)
    {
        return R_LPM_PRV_ERROR;
    }
    if (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_BOOSTM_Msk))
    {
        return R_LPM_PRV_ERROR;
    }
    if (R_LPM_PRV_REG_SOPCCR_SUBSPEED != SYSTEM->SOPCCR)
    {
        return R_LPM_PRV_ERROR;
    }
    if (0U == (SYSTEM->VBBCR & SYSTEM_VBBCR_CLKSEL_Msk))
    {
        if (R_LPM_PRV_REG_SCKSCR_LOCO != SYSTEM->SCKSCR)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    if (0U != (SYSTEM->VBBCR & SYSTEM_VBBCR_CLKSEL_Msk))
    {
        if (R_LPM_PRV_REG_SCKSCR_SOSC != SYSTEM->SCKSCR)
        {
            return R_LPM_PRV_ERROR;
        }
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */
    if (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_VBBM_Msk))
    {
        return R_LPM_PRV_OK;       // not error
    }

#ifdef CHIP_VERSION_WS1             // WS1 limitation No.60
    if (SYSTEM_PWSTF_ALLPWONM_Msk == (SYSTEM->PWSTF & R_LPM_PRV_SYSTEM_PWSTF_PMOD_MSK))
    {
        R_SYS_LowSpeedModeSet();
        R_SYS_32kHzSpeedModeSet();
    }
#endif /* CHIP_VERSION_WS1 */

    /* Execute internal voltage discharge when transitioning from BOOST */
    r_sys_BoostFlagGet( &boost_flag );
    if (true == boost_flag)
    {
        r_sys_BoostFlagClr();
        
        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
        SYSTEM->VBBCR_b.IVDIS = 1;
        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
        
        /* Wait 1 ms for internal voltage discharge to complete */
        R_SYS_SoftwareDelay(1, SYSTEM_DELAY_UNITS_MILLISECONDS);
        
        R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
        SYSTEM->VBBCR_b.IVDIS = 0;
        R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);
    }

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
    SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;
    SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;
    SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_VBB;

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    if (R_LPM_PRV_REG_PWSTCR_VBB != SYSTEM->PWSTCR)
    {
        return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
    }

    /* Execute WFE instruction */
    /* Wait the transition to VBB mode */
    do
    {
        __WFE();
    }
    while (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_VBBM_Msk));

    return R_LPM_PRV_OK;
} /* End of function R_LPM_BackBiasModeEntry() */

/******************************************************************************************************************//**
 * @brief This function transits from Back Bias mode to Normal mode.
 * @retval 0             Success : Transition from Back Bias mode to Normal mode succeeded.
 * @retval -1            Error   : Transition from Back Bias mode to Normal mode failed. An error occurs if the 
 * following condition is detected.
 * - Normal mode transition can not be set to hardware.
 * @details This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function
 * of the R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it
 * is necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
int32_t R_LPM_BackBiasModeExit (void)
{
    if (0U == (SYSTEM->PWSTF & SYSTEM_PWSTF_VBBM_Msk))
    {
        return R_LPM_PRV_OK;       // not error
    }

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
    SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;
    SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;
    SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_NORMAL;

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    if (R_LPM_PRV_REG_PWSTCR_NORMAL != SYSTEM->PWSTCR)
    {
        return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
    }

    /* Execute WFE instruction */
    /* Wait the transition to NORMAL mode */
    do
    {
        __WFE();
    }
    while (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_VBBM_Msk));

    return R_LPM_PRV_OK;
} /* End of function R_LPM_BackBiasModeExit() */

/******************************************************************************************************************//**
 * @brief This function gets the operating mode, Back Bias mode or Normal mode.
 * @retval LPM_BACKBIAS_MODE_NORMAL  Operating in Normal mode.
 * @retval LPM_BACKBIAS_MODE_VBB     Operating in Back Bias mode.
 *********************************************************************************************************************/
e_lpm_backbias_mode_t R_LPM_BackBiasModeGet (void)
{
    return (e_lpm_backbias_mode_t)SYSTEM->PWSTF_b.VBBM;
} /* End of function R_LPM_BackBiasModeGet() */

/******************************************************************************************************************//**
 * @brief This function transitions to Software Standby mode.
 * @retval 0             Success : Software Standby mode transition succeeded.
 * @retval -1            Error   : Software Standby mode transition failed. An error occurs if the following conditions
 * are detected.
 * - Operating in BOOST mode.
 * - For the state of the current power supply mode, the state of the power supply mode before the transition to the 
 * Software Standby mode set by the R_LPM_SSTBYModeSetup () function is not appropriate.
 * @details In this function, it transitions to Software Standby mode. This function terminates when returning from
 * Software Standby mode. Before this function is executed, it is necessary to execute the 
 * R_LPM_SSTBYModeSetup() function and set the operation conditions in Software Standby mode.\n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 * @note There is following restriction when using the WS1 device.
 * - If snooze is enabled in Subosc-Speed mode and Back Bias operation mode, LPM_SSTBY_TO_OPE_EXFPWON_TO_ALLPWON can
 * not be set as the power supply mode when returning from Software Standby mode, so it returns an error.
 *********************************************************************************************************************/
int32_t R_LPM_SSTBYModeEntry (void)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    
    uint8_t   current_mode = (SYSTEM->PWSTF & (uint8_t)R_LPM_PRV_SYSTEM_PWSTF_PMOD_MSK);

    if (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_BOOSTM_Msk))
    {
        return R_LPM_PRV_ERROR;
    }
    if (SYSTEM_PWSTF_ALLPWONM_Msk == current_mode)
    {
        if (LPM_POWER_SUPPLY_MODE_ALLPWON != gs_lpm_sstby_start_ope)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    else if (SYSTEM_PWSTF_EXFPWONM_Msk == current_mode)
    {
        if (LPM_POWER_SUPPLY_MODE_EXFPWON != gs_lpm_sstby_start_ope)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    else if (SYSTEM_PWSTF_MINPWONM_Msk == current_mode)
    {
        if (LPM_POWER_SUPPLY_MODE_MINPWON != gs_lpm_sstby_start_ope)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    else
    {
        return R_LPM_PRV_ERROR; // Usually not passed due to unexpected hardware status check
    }

#ifdef CHIP_VERSION_WS1             // WS1 limitation No.34, No.35 and No.36
    if (R_LPM_PRV_FLAG_SNZ_ENABLE == gs_lpm_snz_flag)
    {
        if (0U != (SYSTEM->SOPCCR & SYSTEM_SOPCCR_SOPCM_Msk))
        {
            if (SYSTEM_PWSTF_MINPWONM_Msk != current_mode)
            {
                if (0U == gs_lpm_sbycr_ssbymp_retpm)
                {
                    return R_LPM_PRV_ERROR;
                }
            }
        }
    }
#endif /* CHIP_VERSION_WS1 */

#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    if (R_LPM_PRV_FLAG_SNZ_DISABLE != gs_lpm_snz_flag)
    {
        SYSTEM->SNZCR   = R_LPM_PRV_REG_SNZCR_SNZOFF;
        SYSTEM->SNZCR   = ((uint8_t)SYSTEM_SNZCR_SNZE_Msk | gs_lpm_snzcr_snzdtcen_rxdregen);
    }
    SYSTEM->SBYCR   = ((uint16_t)SYSTEM_SBYCR_SSBY_Msk | gs_lpm_sbycr_ssbymp_retpm);
    SYSTEM->DPSBYCR = R_LPM_PRV_REG_DPSBYCR_DSTBYOFF;
    SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_SSTBY_DSTBY;

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    __WFI();

    return R_LPM_PRV_OK;
} /* End of function R_LPM_SSTBYModeEntry() */

/******************************************************************************************************************//**
 * @brief This function sets operating conditions in Software Standby mode.
 * @param[in] p_cfg      Set pointer of the structure storing the operating condition of Software Standby mode.
 * - p_cfg->ope_sstby sets the power supply mode when transition to Software Standby mode.
 * - p_cfg->sstby_ope sets the power supply mode when returning from Software Standby mode.
 * - p_cfg->wup is defined by @ref LPM_SSTBY_WUP. Set the wakeup event from Software Standby mode to this parameter. 
 * Multiple wakeup event can be set to input parameter by logical OR.
 * @retval 0             Success : Setting of operating condition in Software Standby mode succeeded.
 * @retval -1            Error   : Setting of operating condition in Software Standby mode failed. An error occurs if
 * the following conditions are detected.
 * - The input parameter "p_cfg" is NULL.
 * - The input parameter "p_cfg->ope_sstby" exceeds the definition range of e_lpm_ope_to_sstby_power_supply_mode_t.
 * - The input parameter "p_cfg->sstby_ope" exceeds the definition range of e_lpm_sstby_to_ope_power_supply_mode_t.
 * - The power supply mode after transition to Software Standby mode does not match the power supply mode before
 * transition from Software Standby mode to operating mode.
 * @details Setting of power supply mode must be same both after transition to Software Standby mode and before
 * transition to Operation from Software Standby mode. \n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 * @sa @ref configuration_of_sstby
 *********************************************************************************************************************/
int32_t R_LPM_SSTBYModeSetup (st_lpm_sstby_cfg_t * p_cfg)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (NULL == p_cfg)
    {
        return R_LPM_PRV_ERROR;
    }
    if (LPM_OPE_TO_SSTBY_MINPWON_TO_MINPWON < p_cfg->ope_sstby)
    {
        return R_LPM_PRV_ERROR;
    }
    if (LPM_SSTBY_TO_OPE_MINPWON_TO_MINPWON < p_cfg->sstby_ope)
    {
        return R_LPM_PRV_ERROR;
    }

    if ((LPM_OPE_TO_SSTBY_ALLPWON_TO_EXFPWON == p_cfg->ope_sstby) ||
        (LPM_OPE_TO_SSTBY_EXFPWON_TO_EXFPWON == p_cfg->ope_sstby))
    {
        if ((LPM_SSTBY_TO_OPE_EXFPWON_TO_ALLPWON != p_cfg->sstby_ope) &&
            (LPM_SSTBY_TO_OPE_EXFPWON_TO_EXFPWON != p_cfg->sstby_ope))
        {
            return R_LPM_PRV_ERROR;
        }
    }
    else
    {
        if (LPM_SSTBY_TO_OPE_MINPWON_TO_MINPWON != p_cfg->sstby_ope)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    
    if ((LPM_OPE_TO_SSTBY_ALLPWON_TO_EXFPWON == p_cfg->ope_sstby) ||
        (LPM_OPE_TO_SSTBY_ALLPWON_TO_MINPWON == p_cfg->ope_sstby))
    {
        gs_lpm_sstby_start_ope = LPM_POWER_SUPPLY_MODE_ALLPWON;
    }
    else if ((LPM_OPE_TO_SSTBY_EXFPWON_TO_EXFPWON == p_cfg->ope_sstby) ||
             (LPM_OPE_TO_SSTBY_EXFPWON_TO_MINPWON == p_cfg->ope_sstby))
    {
        gs_lpm_sstby_start_ope = LPM_POWER_SUPPLY_MODE_EXFPWON;
    }
    else
    {
        gs_lpm_sstby_start_ope = LPM_POWER_SUPPLY_MODE_MINPWON;
    }

#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    if ((LPM_OPE_TO_SSTBY_ALLPWON_TO_MINPWON == p_cfg->ope_sstby) ||
        (LPM_OPE_TO_SSTBY_EXFPWON_TO_MINPWON == p_cfg->ope_sstby) ||
        (LPM_OPE_TO_SSTBY_MINPWON_TO_MINPWON == p_cfg->ope_sstby))
    {
        gs_lpm_sbycr_ssbymp_retpm = (uint16_t)SYSTEM_SBYCR_SSBYMP_Msk;
    }
    else
    {
        if (LPM_SSTBY_TO_OPE_EXFPWON_TO_ALLPWON != p_cfg->sstby_ope)
        {
            gs_lpm_sbycr_ssbymp_retpm = (uint16_t)SYSTEM_SBYCR_RETPM_Msk;
        }
        else
        {
            gs_lpm_sbycr_ssbymp_retpm = 0U;
        }
    }

    ICU->WUPEN = (LPM_SSTBY_WUP_ALL & p_cfg->wup);

    return R_LPM_PRV_OK;
} /* End of function R_LPM_SSTBYModeSetup() */

/******************************************************************************************************************//**
 * @brief This function transitions to Deep Software Standby mode.
 * @param[in] io         Set Pin States when Deep Software Standby Mode is canceled. 
 * @retval 0             Success : Deep Software Standby mode transition succeeded.
 * @retval -1            Error   : Deep Software Standby mode transition failed. An error occurs if the following
 * conditions are detected.
 * - The input parameter "io" exceeds the definition range of e_lpm_dstby_io_t.
 * - Operating in BOOST mode.
 * @details In this function, it transitions to Deep Software Standby mode. Reset is canceled when recovered from Deep
 * Software Standby mode. Before this function is executed, it is necessary to execute the 
 * R_LPM_DSTBYModeSetup() function and set the operation conditions in Deep Software Standby mode. \n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 * @note There are following restrictions when using the WS1 device.
 * - If CCC is not set as a wakeup event from Deep Software Standby mode using the R_LPM_DSTBYModeSetup()
 * function, or if the SYOCDCR.DBGEN bit is set to "1", return from Deep Software Standby mode is not possible. Then it
 * returns an error. (Restriction No.51 and 52)
 * - In on-chip debugging state, it can not transit to Deep Software Standby mode. (Restriction No.1)
 *********************************************************************************************************************/
int32_t R_LPM_DSTBYModeEntry (e_lpm_dstby_io_t io)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((LPM_DSTBY_IO_CLEAR != io) && (LPM_DSTBY_IO_KEEP != io))
    {
        return R_LPM_PRV_ERROR;
    }
    if (0U != (SYSTEM->PWSTF & SYSTEM_PWSTF_BOOSTM_Msk))
    {
        return R_LPM_PRV_ERROR;
    }

#ifdef CHIP_VERSION_WS1             // WS1 limitation No.1, No.51 and No.52
    if (0U == ((uint8_t)(LPM_DSTBY_WUP_CCC >> R_LPM_PRV_SHIFTNUM_DPSIER1) & SYSTEM->DPSIER1))
    {
        return R_LPM_PRV_ERROR;
    }
#endif /* CHIP_VERSION_WS1 */

#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->SBYCR   = (uint16_t)SYSTEM_SBYCR_SSBY_Msk;
    SYSTEM->DPSBYCR = (uint8_t)(SYSTEM_DPSBYCR_DPSBY_Msk | io);
    SYSTEM->PWSTCR  = R_LPM_PRV_REG_PWSTCR_SSTBY_DSTBY;
    SYSTEM->DPSIFR0 = R_LPM_PRV_REG_DPSIFR0_ALLCLEAR;
    SYSTEM->DPSIFR1 = R_LPM_PRV_REG_DPSIFR1_ALLCLEAR;

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    __WFI();

    return R_LPM_PRV_OK;
} /* End of function R_LPM_DSTBYModeEntry() */

/******************************************************************************************************************//**
 * @brief This function sets operating conditions in Deep Software Standby mode.
 * @param[in] p_cfg      Pointer of the structure storing the operating condition of Deep Software Standby mode.
 * - p_cfg->wup is defined by @ref LPM_DSTBY_WUP. Set the wakeup event from Deep Software Standby mode to this 
 * parameter. Multiple wakeup event can be set to input parameter by logical OR.
 * - p_cfg->wup_edge is defined by @ref LPM_DSTBY_WUP_EDGE. Set this parameter to use the rising edge of interrupt or
 * VCC >= Vdet 1 as the wakeup event from Deep Software Standby mode. This parameter can be set for multiple event by
 * logical OR.
 * @retval 0             Success : Setting of operating condition in Deep Software Standby mode succeeded.
 * @retval -1            Error   : Setting of operating condition in Deep Software Standby mode failed. An error occurs
 * if the following condition is detected.
 * - The input parameter "p_cfg" is NULL.
 * @details This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function
 * of the R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it
 * is necessary to execute the R_SYS_Initialize() function.
 * @sa @ref configuration_of_dstby
 *********************************************************************************************************************/
int32_t R_LPM_DSTBYModeSetup (st_lpm_dstby_cfg_t * p_cfg)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (NULL == p_cfg)
    {
        return R_LPM_PRV_ERROR;
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->DPSIER0  = (uint8_t)(0xFFUL & LPM_DSTBY_WUP_ALL & p_cfg->wup);
    SYSTEM->DPSIER1  = (uint8_t)(0xFFUL & ((LPM_DSTBY_WUP_ALL & p_cfg->wup) >> R_LPM_PRV_SHIFTNUM_DPSIER1));

    SYSTEM->DPSIEGR0 = (uint8_t)(0xFFUL & LPM_DSTBY_WUP_EDGE_ALL & p_cfg->wup_edge);
    SYSTEM->DPSIEGR1 = (uint8_t)(0xFFUL & ((LPM_DSTBY_WUP_EDGE_ALL & p_cfg->wup_edge) >> R_LPM_PRV_SHIFTNUM_DPSIEGR1));

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    return R_LPM_PRV_OK;
} /* End of function R_LPM_DSTBYModeSetup() */

/******************************************************************************************************************//**
 * @brief This function transitions to Sleep mode.
 * @details In this function, it transitions to Sleep mode. This function terminates when returning from Sleep mode.\n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the 
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is 
 * necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
void R_LPM_SleepModeEntry (void)
{
    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->SBYCR = R_LPM_PRV_REG_SBYCR_SLEEP;

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    __WFI();

  return ;
} /* End of function R_LPM_SleepModeEntry() */

/******************************************************************************************************************//**
 * @brief This function sets the RAM retention area in Software Standby mode.
 * @param[in] area       Defined by @ref LPM_RAM_RETENTION_AREA. Set the RAM area to be retention during Software 
 * Standby mode to this parameter. Multiple areas can be set to input parameter by logical OR.
 * @details When this input parameter is used to set retention area, it sets corresponding RAMSn bit of RAMSDCR to 
 * 0 (retention). Note that this input parameter and the value of RAMSDCR register are inverted.\n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 *********************************************************************************************************************/
void R_LPM_RamRetentionSet (uint32_t area)
{
    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    SYSTEM->RAMSDCR = (uint8_t)~(LPM_RAM_RETENTION_ALL & area);

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    return ;
} /* End of function R_LPM_RamRetentionSet() */

/******************************************************************************************************************//**
 * @brief This function sets operating conditions in Snooze mode.
 * @param[in] p_cfg      Pointer of the structure storing the operating condition of Deep Software Standby mode.
 * - p_cfg->enable sets whether to enable or disable transition from Software Standby mode to Snooze.
 * - p_cfg->req is defined by @ref LPM_SNZ_REQ. Set the event to transition from Software Standby mode to Snooze mode 
 * to this parameter. Multiple event can be set to input parameter by logical OR.
 * - p_cfg->snz_sstby is defined by @ref LPM_SNZ_SSTBY_REQ. Set the event to transition from Snooze to Software Standby
 * mode to this parameter. Multiple event can be set to input parameter by logical OR.
 * - Set the return event from Snooze mode to p_cfg->wup. This setting set to the snooze cancel event. (ICU_SNZCANCEL)
 * - p_cfg->dtc_enable sets whether or not DTC operation during Snooze operation is enabled or disabled.
 * - p_cfg->req_rx 0 sets whether or not Snooze transition occurs when the falling edge of RXD 0 is detected during
 * Software Standby mode.
 * @retval 0             Success : Setting of operating condition in Snooze mode succeeded.
 * @retval -1            Error   : Setting of operating condition in Snooze mode failed. An error occurs if the
 * following conditions are detected.
 * - The input parameter "p_cfg" is NULL.
 * - The input parameter "p_cfg->enable" exceeds the definition range of e_lpm_snz_enable_t.
 * - The input parameter "p_cfg->req_rx0" exceeds the definition range of e_lpm_snz_rx0_falling_edge_t.
 * - The input parameter "p_cfg->dtc_enable" exceeds the definition range of e_lpm_snz_dtc_enable_t.
 * - The same event is set for the return factor from Snooze and transition factor from Snooze to Software Standby mode
 * (For DTC and ADC)
 * @details This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function
 * of the R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it
 * is necessary to execute the R_SYS_Initialize() function.
 * @sa @ref configuration_of_snooze
 *********************************************************************************************************************/
int32_t R_LPM_SnoozeSet (st_lpm_snooze_cfg_t * p_cfg)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    if (NULL == p_cfg)
    {
        return R_LPM_PRV_ERROR;
    }
    if (LPM_SNZ_ENABLE < p_cfg->enable)
    {
        return R_LPM_PRV_ERROR;
    }
    if (LPM_RX0_FALLING_EDGE_ENABLE < p_cfg->req_rx0)
    {
        return R_LPM_PRV_ERROR;
    }
    if ((LPM_SNZ_DTC_DISABLE != p_cfg->dtc_enable) && (LPM_SNZ_DTC_ENABLE != p_cfg->dtc_enable))
    {
        return R_LPM_PRV_ERROR;
    }
    if (LPM_SNZ_WUP_DTC == p_cfg->wup)
    {
        if (LPM_SNZ_SSTBY_DTCZ== p_cfg->snz_sstby)
        {
            return R_LPM_PRV_ERROR;
        }
        if (LPM_SNZ_SSTBY_DTCN== p_cfg->snz_sstby)
        {
            return R_LPM_PRV_ERROR;
        }
    }
    if ((LPM_SNZ_WUP_ADC140_WCMPM == p_cfg->wup) && (LPM_SNZ_SSTBY_AD0MAT== p_cfg->snz_sstby))
    {
        return R_LPM_PRV_ERROR;
    }
    if ((LPM_SNZ_WUP_ADC140_WCMPUM == p_cfg->wup) && (LPM_SNZ_SSTBY_AD0UMT== p_cfg->snz_sstby))
    {
        return R_LPM_PRV_ERROR;
    }

#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    if (LPM_SNZ_ENABLE == p_cfg->enable)
    {
        gs_lpm_snz_flag                 = R_LPM_PRV_FLAG_SNZ_ENABLE;
        SYSTEM->SNZREQCR                = (LPM_SNZ_REQ_ALL & p_cfg->req);
        ICU->SELSR0                     = (uint16_t)p_cfg->wup;
        SYSTEM->SNZEDCR                 = (uint8_t)(LPM_SNZ_SSTBY_ALL & p_cfg->snz_sstby);
        gs_lpm_snzcr_snzdtcen_rxdregen  = ((uint8_t)p_cfg->req_rx0 | (uint8_t)p_cfg->dtc_enable);
    }
    else
    {
        gs_lpm_snz_flag                 = R_LPM_PRV_FLAG_SNZ_DISABLE;
        SYSTEM->SNZCR                   = R_LPM_PRV_REG_SNZCR_SNZOFF;
        gs_lpm_snzcr_snzdtcen_rxdregen  = 0U;
    }

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    return R_LPM_PRV_OK;

} /* End of function R_LPM_SnoozeSet() */

/******************************************************************************************************************//**
 * @brief This function acquires the Deep Software Standby mode cancelling factor.
 * @retval LPM_DSTBY_RESET_IRQ0DS  When the value of bit field is 1, Deep Software Standby was released by IRQ0-DS pin.
 * @retval LPM_DSTBY_RESET_IRQ1DS  When the value of bit field is 1, Deep Software Standby was released by IRQ1-DS pin.
 * @retval LPM_DSTBY_RESET_IRQ2DS  When the value of bit field is 1, Deep Software Standby was released by IRQ2-DS pin.
 * @retval LPM_DSTBY_RESET_IRQ3DS  When the value of bit field is 1, Deep Software Standby was released by IRQ3-DS pin.
 * @retval LPM_DSTBY_RESET_LVD1    When the value of bit field is 1, Deep Software Standby was released by voltage
 * monitor 1 signal (LVD1).
 * @retval LPM_DSTBY_RESET_LVDBAT  When the value of bit field is 1, Deep Software Standby was released by voltage
 * monitor BAT signal (LVDBAT).
 * @retval LPM_DSTBY_RESET_NMI     When the value of bit field is 1, Deep Software Standby was released by NMI pin.
 * @retval LPM_DSTBY_RESET_CCC     When the value of bit field is 1, Deep Software Standby was released by CCC interval
 * intterupt singnal.
 * @details Return value is defined by @ref LPM_DSTBY_RESET.
 *********************************************************************************************************************/
uint32_t R_LPM_DSTBYResetStatusGet (void)
{
    uint32_t reg0 = ((uint32_t)(SYSTEM->DPSIFR0) & 0x00FFUL);
    uint32_t reg1 = (((uint32_t)(SYSTEM->DPSIFR1) << R_LPM_PRV_SHIFTNUM_DPSIFR1) & 0xFF00UL);

    return (reg0 | reg1);

} /* End of function R_LPM_DSTBYResetStatusGet() */

/******************************************************************************************************************//**
 * @brief If power cannot be supplied in each IO power supply domain (AVCC0, AVCC1, IOVCC0, IOVCC1, IOVCC1, IOVCC2, 
 * USBVCC, MTDV), an undefined signal is generated to activate the function to prevent propagation to other domains to
 * which power is supplied.
 * @param[in] vocr Defined by @ref LPM_IOPOWER_SUPPLY. Set the IO power supply domains that are not supplied power to
 * this parameter. Enables the function to suppress the propagation of undefined signals generated in the IO power
 * supply domain set to vocr to other domains. Multiple IO power supply domains can be set to input parameter by
 * logical OR.
 * @retval 0             Success : Setting of the undefined signal propagation suppression function of IO power supply
 * domain succeeded.
 * @retval -1            Error   : Setting of the undefined signal propagation suppression function of IO power supply
 * domain failed. An error occurs if the following condition is detected.
 * - A module belonging to the IO power supply domain specified by the input parameter "vocr" is not transitioned to
 * the module stop state.
 * @details Before this function is executed, it is necessary to execute the R_LPM_ModuleStop() function or the 
 * R_LPM_FastModuleStop() function to stop the module belonging to the specified IO power supply domain. \n\n
 * This function uses the R_SYS_RegisterProtectEnable() function and R_SYS_RegisterProtectDisable() function of the
 * R_SYSTEM driver when "1" is set in LPM_CFG_REGISTER_PROTECTION_ENABLE. Before this function is executed, it is
 * necessary to execute the R_SYS_Initialize() function.
 * @note There is following restriction when using the WS1 device.
 * - VOCR b7 bit definition is inverted. (Restriction No.58)
 *********************************************************************************************************************/
int32_t R_LPM_IOPowerSupplyModeSet (uint8_t vocr)
{
#if (LPM_CFG_PARAM_CHECKING_ENABLE == 1)
    u_lpm_mstp_data_t data = {0};
    volatile uint32_t * p_mstp_base = 0;
    uint32_t mask = 0UL;

    /* Turn off the power supply of AVCC0 power supply module must be in a module stopped state. */
    if (LPM_IOPOWER_SUPPLY_AVCC0 & vocr)
    {
        /* Check the module stop of S14AD, TEMPS and VREF. */
        data.byte = gs_lpm_module_stop[LPM_MSTP_S14AD];
        p_mstp_base = gsp_lpm_mstp[data.bit.reg];
        mask = (1U << (data.bit.bit));
        if ((mask & (*p_mstp_base)) != mask)
        {
            return R_LPM_PRV_ERROR;      /* The module stop is not stopping. */
        }
        data.byte = gs_lpm_module_stop[LPM_MSTP_TEMPS];
        p_mstp_base = gsp_lpm_mstp[data.bit.reg];
        mask = (1U << (data.bit.bit));
        if ((mask & (*p_mstp_base)) != mask)
        {
            return R_LPM_PRV_ERROR;      /* The module stop is not stopping. */
        }
        data.byte = gs_lpm_module_stop[LPM_MSTP_VREF];
        p_mstp_base = gsp_lpm_mstp[data.bit.reg];
        mask = (1U << (data.bit.bit));
        if ((mask & (*p_mstp_base)) != mask)
        {
            return R_LPM_PRV_ERROR;      /* The module stop is not stopping. */
        }
    }

    /* Turn off the power supply of AVCC1 power supply module must be in a module stopped state. */
    if (LPM_IOPOWER_SUPPLY_AVCC1 & vocr)
    {
        /* Check the module stop of R12DA and ACMP. */
        data.byte = gs_lpm_module_stop[LPM_MSTP_R12DA];
        p_mstp_base = gsp_lpm_mstp[data.bit.reg];
        mask = (1U << (data.bit.bit));
        if ((mask & (*p_mstp_base)) != mask)
        {
            return R_LPM_PRV_ERROR;      /* The module stop is not stopping. */
        }
        data.byte = gs_lpm_module_stop[LPM_MSTP_ACMP];
        p_mstp_base = gsp_lpm_mstp[data.bit.reg];
        mask = (1U << (data.bit.bit));
        if ((mask & (*p_mstp_base)) != mask)
        {
            return R_LPM_PRV_ERROR;      /* The module stop is not stopping. */
        }
    }

    /* Turn off the power supply of USBVCC power supply module must be in a module stopped state. */
    if (LPM_IOPOWER_SUPPLY_USBVCC & vocr)
    {
        /* Check the module stop of USB. */
        data.byte = gs_lpm_module_stop[LPM_MSTP_USB];
        p_mstp_base = gsp_lpm_mstp[data.bit.reg];
        mask = (1U << (data.bit.bit));
        if ((mask & (*p_mstp_base)) != mask)
        {
            return R_LPM_PRV_ERROR;      /* The module stop is not stopping. */
        }
    }

    /* Turn off the power supply of MTDV power supply module must be in a module stopped state. */
    if (LPM_IOPOWER_SUPPLY_MTDV & vocr)
    {
        /* Check the module stop of MTDV. */
        data.byte = gs_lpm_module_stop[LPM_MSTP_MTDV];
        p_mstp_base = gsp_lpm_mstp[data.bit.reg];
        mask = (1U << (data.bit.bit));
        if ((mask & (*p_mstp_base)) != mask)
        {
            return R_LPM_PRV_ERROR;      /* The module stop is not stopping. */
        }
    }
#endif /* (LPM_CFG_PARAM_CHECKING_ENABLE == 1) */

#ifdef CHIP_VERSION_WS1             // WS1 limitation No.58
    vocr ^= SYSTEM_VOCR_VPMCTL_Msk;
#endif /* CHIP_VERSION_WS1 */

    R_LPM_PRV_REGISTER_PROTECT_DISABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    /* Set VOCR register. */
    SYSTEM->VOCR = LPM_IOPOWER_SUPPLY_ALL & vocr;

    R_LPM_PRV_REGISTER_PROTECT_ENABLE(SYSTEM_REG_PROTECT_OM_LPC_BATT);

    return R_LPM_PRV_OK;
} /* End of function R_LPM_IOPowerSupplyModeSet() */

/******************************************************************************************************************//**
 * @brief This function acquires the IO power supply domains which undefined signal propagation suppression function is
 * enabled.
 * @retval When the bit field below the return value is '1', the undefined signal propagation suppression function of
 * the corresponding IO power supply domain is enabled.
 *  - LPM_IOPOWER_SUPPLY_AVCC0   : S14AD, TEMPS and VREF
 *  - LPM_IOPOWER_SUPPLY_AVCC1   : R12DA and ACMP
 *  - LPM_IOPOWER_SUPPLY_IOVCC0  : PORT8
 *  - LPM_IOPOWER_SUPPLY_IOVCC1  : PORT3, PORT6, PORT7 and P202-P204
 *  - LPM_IOPOWER_SUPPLY_IOVCC2  : PORT1
 *  - LPM_IOPOWER_SUPPLY_IOVCC3  : P010-P015 and PORT5
 *  - LPM_IOPOWER_SUPPLY_USBVCC  : USB
 *  - LPM_IOPOWER_SUPPLY_MTDV    : MTDV
 * @details Return value is defined by @ref LPM_IOPOWER_SUPPLY.
 * @note There is following restriction when using the WS1 device.
 * - VOCR b7 bit definition is inverted. (Restriction No.58)
 *********************************************************************************************************************/
uint8_t R_LPM_IOPowerSupplyModeGet (void)
{
    uint8_t vocr = SYSTEM->VOCR & LPM_IOPOWER_SUPPLY_ALL;

#ifdef CHIP_VERSION_WS1             // WS1 limitation No.58
    vocr ^= SYSTEM_VOCR_VPMCTL_Msk;
#endif /* CHIP_VERSION_WS1 */

    return vocr;
} /* End of function R_LPM_IOPowerSupplyModeGet() */

/******************************************************************************************************************//**
 * @} (end addtogroup Device HAL LPM (Low Power Mode))
 *********************************************************************************************************************/

/* End of file (r_lpm.c) */

