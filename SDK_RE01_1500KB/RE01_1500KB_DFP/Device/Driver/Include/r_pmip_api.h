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
* Copyright (C) 2019-2021 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_pmip_api.h
* Version      : 1.40
* Description  : HAL-Driver for PMIP
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.06.2019 0.72    First Release
*         : 23.07.2019 1.00    IO register header name change(E017.h -> RE01_1500KB.h)
*                              Upgrade version to 1.00
*         : 21.11.2019 1.01    Upgrade version to 1.01
*         : 18.02.2020 1.10    Fixed the problem that R_PMIP_Send function is not allocated to RAM
*         : 27.08.2020 1.20    Fixed not to include header file when DMA is not used.
*         : 12.01.2021 1.40    Add R_PMIP_Reconfig().
*                              Add "MLCD_FLAG_RECONFIG" , "PMIP_ERROR_BUSY" and "PMIP_ERROR_RECONFIG"
***********************************************************************************************************************/
#ifndef R_PMIP_API_H
#define R_PMIP_API_H

#ifdef  __cplusplus
extern "C"
{
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "RE01_1500KB.h"
#include "r_lpm_api.h"
#include "r_system_api.h"
#include "r_pmip_cfg.h"
#if (PMIP_CFG_SUPPORT_DMAC == 1)
#include "r_dmac_api.h"
#endif
  
/******************************************************************************************************************//**
 @ingroup  grp_device_hal
 @defgroup grp_device_hal_pmip Device HAL PMIP Implementation
 @brief The PMIP (Memory In Pixel) driver implementation for the Device HAL driver interface on RE Family.
    
 @section 
 @code
 @endcode
 @section 

 @{
 *********************************************************************************************************************/


/******************************************************************************
Macro definitions
*******************************************************************************/
/** Major Version Number of API. */
#define PMIP_VERSION_MAJOR    (1)
/** Minor Version Number of API. */
#define PMIP_VERSION_MINOR    (40)

/** MLCD Driver flags */
#define MLCD_FLAG_OPENED      (1 << 0)
#define MLCD_FLAG_RECONFIG    (1 << 1)

/** MLCD DATA register offset */
#define MLCD_DATA_BUF_OFFSET  (0x20)

/** MLCD transfer end */
#define MLCD_TEND             (1)

/** MLCD transfer empty */
#define MLCD_TEF              (1)

/** MLCD data empty interrupt trigger DMAC */
#define MLCD_TEMI_TO_DMAC_TRG (0xA7)

/** MLCD base address of the register */
#define PMIP_MLCD_BASE        (0x40070200UL)

/** MLCD temi iels value */
#define MLCD_TEMI_IELS_VALUE    (0x1DU)

/** MLCD tei iels value */
#define MLCD_TEI_IELS_VALUE     (0x1EU)

/******************************************************************************
 Exported global function (to be accessed by other files)
******************************************************************************/
#if (PMIP_CFG_R_PMIP_GETVERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_GETVERSION     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_GETVERSION
#endif

#if (PMIP_CFG_R_PMIP_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_OPEN     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_OPEN
#endif

#if (PMIP_CFG_R_PMIP_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_CLOSE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_CLOSE
#endif

#if (PMIP_CFG_R_PMIP_POWERON == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_POWERON     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_POWERON
#endif

#if (PMIP_CFG_R_PMIP_POWEROFF == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_POWEROFF     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_POWEROFF
#endif

#if (PMIP_CFG_R_PMIP_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_CONTROL     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_CONTROL
#endif

#if (PMIP_CFG_R_PMIP_ALLZERO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_ALLZERO     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_ALLZERO
#endif

#if (PMIP_CFG_R_PMIP_ALLONE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_ALLONE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_ALLONE
#endif

#if (PMIP_CFG_R_PMIP_SENDTRIM == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_SENDTRIM     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_SENDTRIM
#endif

#if (PMIP_CFG_R_PMIP_SEND == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_PMIP_SEND     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_PMIP_SEND
#endif

#if (PMIP_CFG_R_PMIP_RECONFIG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_PMIP_RECONFIG     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_PMIP_RECONFIG
#endif

/** @brief PMIP API event error codes */
typedef enum
{
    PMIP_EVENT_SEND_COMPLETE                          = 1,               /*!< normal end   */
    PMIP_EVENT_ERROR_DMAC_TRANS                       = 2,               /*!< abnormal end */

} e_pmip_event_err_t;

/** @brief PMIP API error codes */
typedef enum
{
    PMIP_OK                          = 1,               /*!< successful operation            */
    PMIP_ERROR                       = 2,               /*!< Open/Close failed error         */
    PMIP_ERROR_DTC_NOT_SUPPORT       = 3,               /*!< DTC trans mode is not support   */
    PMIP_ERROR_TRANS_MODE            = 4,               /*!< MLCD Trans mode config failed   */ 
    PMIP_ERROR_DMAC_TRANS            = 5,               /*!< MLCD DMAC transfer failed       */
    PMIP_ERROR_DMAC_CFG              = 6,               /*!< DMAC config failed              */
    PMIP_ERROR_INPUT_CLK_OFFRANGE    = 7,               /*!< MLCD input clock off range      */
    PMIP_ERROR_ENBEG_CFG             = 8,               /*!< MLCD enbeg config failed        */
    PMIP_ERROR_CONTROL_CMD           = 9,               /*!< MLCD command setting error      */
    PMIP_ERROR_SEND_CFG              = 10,              /*!< MLCD send config failed         */
    PMIP_ERROR_SYSTEM_SETTING        = 11,              /*!< MLCD system config failed       */
    PMIP_ERROR_BUSY                  = 12,              /*!< MLCD busy error                 */
    PMIP_ERROR_RECONFIG              = 13               /*!< MLCD reconfig error             */
} e_pmip_err_t;

/*****************************************************************************
Typedef definitions
******************************************************************************/
/** @brief PMIP Driver callback function */
typedef void (*pmip_cb_event_t) (e_pmip_event_err_t event_err_t);                 /*!< Pointer to DMA interrupt callback function. */

/** @brief PMIP transmission mode */
typedef enum    /* transmission method 0:DMAC  1:CPU  2:DTC(Not support yet)*/
{
#if (PMIP_CFG_SUPPORT_DMAC == 1)
    DMAC_TR = 0,   /*!< DMAC mode */
#endif
    CPU_TR  = 1,   /*!< CPU mode */
#if (PMIP_CFG_SUPPORT_DTC == 1)
    DTC_TR  = 2    /*!< DTC mode(Not support yet) */
#endif
} e_trans_mode_t;

/** @brief PMIP trans cmd */
typedef enum    /* trans cmd 0:MODE1  1:MODE2  2:MODE3 */
{
    CMD0 = 0,   /* bit arrangement for data transmission */
    CMD1 = 1,   /* automatic update method of horizontal address */
    CMD2 = 2    /* automatic update method of vertical address */
} e_trans_cmd_t;

/** @brief PMIP enable control table for Kyocera*/
typedef enum
{
     KYOCERA_ENB   =  2,               /*!< us */
     KYOCERA_ENBH  = 10,               /*!< us */
} e_pmip_enb_tbl_kyocera_t;

/** @brief PMIP enable control table for user setting */
typedef enum
{
     OTHER_ENB     =  4,               /*!< us */
     OTHER_ENBH    = 20,               /*!< us */
} e_pmip_enb_tbl_other_t;

/** MLCD mode infomation */
typedef struct 
{           
    pmip_cb_event_t       cb_event;                 /*!< Event callback */
    uint8_t               flags;                    /*!< DMA driver flags */
} st_mlcd_mode_info_t;

/** @brief PMIP recource data */
typedef struct 
{           
    volatile MLCD_Type      *reg_mlcd;                  /*!< Base pointer to PMIP registers      */
    volatile ICU_Type       *reg_icu;                   /*!< Base pointer to ICU registers       */
    volatile MSTP_Type      *reg_mstp;                  /*!< Base pointer to MSTP registers      */
    e_lpm_mstp_t            mstp_id;                    /*!< MSTP id                             */
    uint8_t                 pmip_cfg_interrupt_level;   /*!< PMIP interrupt priority value       */
    st_mlcd_mode_info_t     *info;                      /*!< PMIP Driver flags                   */
} const st_pmip_resources_t; 

/** @brief PMIP trans mode setting */
typedef struct
{
    e_trans_mode_t    mode;   /* transmission method 0:DMAC  1:CPU  2:DTC(Not support yet) */
    uint8_t           sel;    /* dmac channel sel 0:DMAC0  1:DMAC1  2:DMAC2  3:DAMC3 */
} st_transmode_t;

/* PMIP API function */
uint32_t R_PMIP_GetVersion(void)FUNC_LOCATION_R_PMIP_GETVERSION;
e_pmip_err_t R_PMIP_Open(pmip_cb_event_t cb, st_transmode_t * tr_mode)FUNC_LOCATION_R_PMIP_OPEN;
e_pmip_err_t R_PMIP_Close(void)FUNC_LOCATION_R_PMIP_CLOSE;
e_pmip_err_t R_PMIP_PowerOn(void)FUNC_LOCATION_R_PMIP_POWERON;
e_pmip_err_t R_PMIP_PowerOff(void)FUNC_LOCATION_R_PMIP_POWEROFF;
e_pmip_err_t R_PMIP_Control(e_trans_cmd_t cmd, void * set)FUNC_LOCATION_R_PMIP_CONTROL;
e_pmip_err_t R_PMIP_AllZero(void)FUNC_LOCATION_R_PMIP_ALLZERO;
e_pmip_err_t R_PMIP_AllOne(void)FUNC_LOCATION_R_PMIP_ALLONE;
e_pmip_err_t R_PMIP_SendTrim(uint8_t st_h, uint8_t st_v, uint16_t img_size_h, uint8_t trim_size_h, uint16_t size_v, 
                             uint8_t const data[])FUNC_LOCATION_R_PMIP_SENDTRIM;
e_pmip_err_t R_PMIP_Send(uint8_t st_h, uint8_t st_v, uint8_t size_h, uint16_t size_v, 
                         uint8_t const data[]) FUNC_LOCATION_R_PMIP_SEND;
e_pmip_err_t R_PMIP_Reconfig(void)FUNC_LOCATION_PRV_R_PMIP_RECONFIG;
#ifdef  __cplusplus
}
#endif

/******************************************************************************************************************//**
 * @} (end defgroup grp_device_hal_pmip)
 *********************************************************************************************************************/

#endif /* R_PMIP_API_H */
/* End of file (r_pmip_api.h) */
