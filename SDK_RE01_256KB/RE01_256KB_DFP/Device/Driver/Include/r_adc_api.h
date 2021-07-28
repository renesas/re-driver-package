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
* Copyright (C) 2018-2021 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_adc.h
* Version      : 1.20
* Description  : HAL Driver for ADC
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
*         : 05.01.2021 1.20     Add adc_close_dma() function
***********************************************************************************************************************/
/******************************************************************************************************************//**
 @ingroup grp_device_hal
 @defgroup grp_device_hal_adc Device HAL ADC Implementation
 @brief The ADC driver implementation for the HAL-Driver ADC interface to drive the ADC peripheral on RE Family.

 @section sec_hal_driver_adc_summary ADC-Driver ADC Summary
 This is the HAL-Driver ADC implementation which drives the ADC peripheral on RE Family via the ADC-Driver interface.
 Use the instance @ref Driver_ADC to acccess to ADC peripheral. Do not use R_ADC_XXX APIs directly. 
 @code
 ...
 // Configure AD conversion
 Driver_ADC->Open();                                     // Open ADC driver
 Driver_ADC->Control(AD_CMD_AUTO_READ_REPEAT, arg);      // In case of using DMAC / DTC, this processing is performed. 
                                                            It is unnecessary when not using.
 Driver_ADC->ScanSet(ADC_GROUP_A, groupa_pin, ADC_TRIGER_ADTRG);// Set the terminals and trigger factors of the group to be used
 
 // AD conversion starts with the input of the set trigger
 // To forcibly terminate the AD conversion, call the AD_CMD_STOP_TRIG command to clear the trigger cause.
 Driver_ADC->Control(AD_CMD_STOP_TRIG, 0);
 ...
 
 @endcode
 @note If you forcibly terminate the AD conversion, please set it as follows when resuming.
 @code
 // When DMAC / DTC is not used, reset the trigger cause with the ScanSet function.
 // The procedure for resetting when using DMAC / DTC is as follows.
 // When DMAC / DTC is used, DMAC / DTC setting can be maintained or initialized.
 // Select the resetting method from the following two sample codes.
 ...
 // Sample Code 1
 // Setting method when DMAC / DTC side is not initialized
 // In this method, the DMAC / DTC maintains the previous setting. 
 // The transfer counter also inherits the previous value.
 Driver_ADC->ScanSet(ADC_GROUP_A, groupa_pin, ADC_TRIGER_ADTRG);// Set the terminals and trigger factors of the group to be used
 
 // AD conversion starts with the input of the set trigger
 
 ...
 ...
 // Sample Code 2
 // Setting method when DMAC / DTC side is initialized
 // The DMAC / DTC setting is overwritten with the reset value.
 // The value of the transfer counter is also initialized.
 Driver_ADC->Control(AD_CMD_AUTO_READ_REPEAT, arg);             // Resetting the DMAC / DTC
 Driver_ADC->ScanSet(ADC_GROUP_A, groupa_pin, ADC_TRIGER_ADTRG);// Set the terminals and trigger factors of the group to be used
 
 // AD conversion starts with the input of the set trigger
 
 ...
 
 @endcode
 @section HAL_Driver_ADC_Specific_Error_Code HAL-Driver ADC Specific Error Code
 See ADC_ERROR_XXX in r_adc.h for the detail.

 @{
 *********************************************************************************************************************/

#ifndef R_ADC_H
#define R_ADC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "RE01_256KB.h"
#include "Pin.h"
#include "r_system_api.h"
#include "r_system_cfg.h"
#include "r_lpm_api.h"
#include "r_adc_cfg.h"

#if (ADC_PRV_USED_DMAC_DRV != 0)
/* Using the DMAC driver */
#include "r_dmac_api.h"
#endif

#if (ADC_PRV_USED_DTC_DRV != 0)
/* Using the DTC driver */
#include "r_dtc_api.h"
#endif

/******************************************************************************
Macro definitions
*******************************************************************************/
/** Version Number of API. */
#define ADC_VERSION_MAJOR               (1)                           /*!< Major Version */
#define ADC_VERSION_MINOR               (20)                          /*!< Minor Version */

/** ADC Enable / Disable */
#define ADC_ENABLE                      (1)                           /*!< ADC Enable */
#define ADC_DISABLE                     (0)                           /*!< ADC Disable */

#define ADC_FLAG_OPENED                 (1U << 0)                     /*!< Opened flag  */
#define ADC_FLAG_INITIALIZED            (1U << 1)                     /*!< Initialization flag */
#define ADC_FLAG_CONFIGURATION          (1U << 2)                     /*!< Configured flag */
#define ADC_FLAG_DMA_ADI_OPEN           (1U << 3)                     /*!< DMA Opened flag */
#define ADC_FLAG_DMA_GBADI_OPEN         (1U << 4)                     /*!< DMA Opened flag */
#define ADC_FLAG_DMA_GCADI_OPEN         (1U << 5)                     /*!< DMA Opened flag */
#define ADC_FLAG_DMA_WCMPM_OPEN         (1U << 6)                     /*!< DMA Opened flag */
#define ADC_FLAG_DMA_WCMPUM_OPEN        (1U << 7)                     /*!< DMA Opened flag */

#define ADC_FLAG_DMA_POS                (3)

#define ADC_MODE_POS                    (0)                             /*!< Mode position */
#define ADC_MODE_MASK                   (0x3UL << ADC_MODE_POS )      /*!< Mode mask */

/*----- ADC Control Codes: Mode -----*/
#define ADC_SINGLE_SCAN                 (0U << ADC_MODE_POS)          /*!< Single scan mode */
#define ADC_GROUP_SCAN                  (1U << ADC_MODE_POS)          /*!< Group scan mode */
#define ADC_REPEAT_SCAN                 (2U << ADC_MODE_POS)          /*!< Repeat scan mode */

#define ADC_RESOLUTION_POS              (2)                             /*!< Resolution position */
#define ADC_RESOLUTION_MASK             (0x1UL << ADC_RESOLUTION_POS) /*!< Resolutio position */
/*----- ADC Control Codes: Resolution -----*/
#define ADC_14BIT                       (0U << ADC_RESOLUTION_POS)    /*!< 14bit Resolution */
#define ADC_12BIT                       (1U << ADC_RESOLUTION_POS)    /*!< 12bit Resolution */

#define ADC_FORMAT_POS                  (3)                             /*!< Format position */
#define ADC_FORMAT_MASK                 (0x1UL << ADC_FORMAT_POS)     /*!< Format mask */

/*----- ADC Control Codes: Format -----*/
#define ADC_RIGHT                       (0U << ADC_FORMAT_POS)        /*!< Right-justified */
#define ADC_LEFT                        (1U << ADC_FORMAT_POS)        /*!< Left-justified */

#define ADC_DDA_MASK                    (0x0F)                        /*!< DDA mask */
/******************************************************************************************************************//**
 * @name ADC_MSEL_ANn
 *       These definitions are used to set the terminals to be AD converted.@n
 *          [Example]@n
 *          @code
 *          ...
 *          st_adc_pins_t scanset_pin;
 *
 *          scanset_pin.an_chans = ADC_MSEL_AN06 | ADC_MSEL_AN28;     // AD conversion with AN06 and AN28
 *          Driver_ADC->Open(ADC_SINGLE_SCAN, 10, callback);
 *          Driver_ADC->ScanSet(ADC_GROUP_A, scanset_pin, triger);
 *           ...
 *          @endcode
 *          [Use function]@n
 *          Driver_ADC->ScanSet
 *********************************************************************************************************************/
/* @{ */
/*----- Channel definition -----*/
#define ADC_MSEL_AN00  (1 <<  0)                                      /*!< Select AN00 */
#define ADC_MSEL_AN01  (1 <<  1)                                      /*!< Select AN01 */
#define ADC_MSEL_AN02  (1 <<  2)                                      /*!< Select AN02 */
#define ADC_MSEL_AN03  (1 <<  3)                                      /*!< Select AN03 */
#define ADC_MSEL_AN04  (1 <<  4)                                      /*!< Select AN04 */
#define ADC_MSEL_AN05  (1 <<  5)                                      /*!< Select AN05 */
#define ADC_MSEL_AN06  (1 <<  6)                                      /*!< Select AN06 */
#define ADC_MSEL_AN07  (1 <<  7)                                      /*!< Select AN07 */
#define ADC_MSEL_AN16  (1 << 16)                                      /*!< Select AN16 */
#define ADC_MSEL_AN17  (1 << 17)                                      /*!< Select AN17 */
#define ADC_MSEL_AN20  (1 << 20)                                      /*!< Select AN20 */
#define ADC_MSEL_AN21  (1 << 21)                                      /*!< Select AN21 */
#define ADC_MSEL_VSC_VCC  (1 << 31)                                   /*!< Select VSC_VCC */
/* @} */
/** Definition of disconnection detection */
/** Disconnection Detection Assist (DDA)*/
#define ADC_DDA_OFF       (0x00)    /*!< Disconnection Detection Assist (DDA) OFF */
#define ADC_DDA_DISCHARGE (0x00)    /*!< Discharge mode */
#define ADC_DDA_PRECHARGE (0x10)    /*!< Precharge mode */

/*****************************************************************************
Typedef definitions
******************************************************************************/
/** @brief Typedef definition for callback event function */
typedef void (*adc_cd_event_t) (uint32_t event);   // @suppress("Source line ordering")

/** @brief ADC Driver Error Definition */
typedef enum                    /* ADC API error codes */
{
    ADC_OK = 0,                 /*!< Operation succeeded */
    ADC_ERROR,                  /*!< Unspecified error */
    ADC_ERROR_BUSY,             /*!< Driver is busy */
    ADC_ERROR_PARAMETER,        /*!< Parameter error */
    ADC_ERROR_MODE,             /*!< Mode error */
    ADC_ERROR_LOCKED,           /*!< This Module is already locked */
    ADC_ERROR_SYSTEM_SETTING    /*!< error for System Driver (R_System) setting */
} e_adc_err_t;

/** @brief Definition for AD conversion run state */
typedef enum
{
    ADC_STATE_STOP,             /*!<  A/D stop state                */
    ADC_STATE_RUN               /*!<  A/D Converting                */
} e_adc_state_t;

/** @brief AD conversion state definition */
typedef enum 
{
    ADC_CONV_NOT_POLLING = 0,   /*!< Target group not confirmed            */
    ADC_CONV_INCOMPLETE,        /*!< A/D Conversion incomplete             */
    ADC_CONV_COMPLETE,          /*!< A/D Conversion complete               */
    ADC_CONV_GET_FAILED,        /*!< A/D Conversion result get failed      */
    ADC_CONV_WCMP_DETECT,       /*!<       */
    ADC_CONV_WCMP_NOT_DETECT    /*!< A/D Conversion result get failed      */
} e_adc_conv_state_t;

/** @brief AD conversion event definition  */
typedef enum                        /*!< callback function events */
{
    ADC_EVT_SCAN_COMPLETE,          /*!< normal/Group A scan complete */
    ADC_EVT_SCAN_COMPLETE_GROUPB,   /*!< Group B scan complete */
    ADC_EVT_SCAN_COMPLETE_GROUPC,   /*!< Group C scan complete */
    ADC_EVT_CONDITION_MET,          /*!< 1+ chans/sensors met comparator condition */
    ADC_EVT_CONDITION_METB,         /*!< 1+ chans/sensors met comparator condition */
    ADC_EVT_WINDOW_CMP_MATCH,       /*!< WCMPM Event */
    ADC_EVT_WINDOW_CMP_UNMATCH      /*!< WCMPUM Event */
} e_adc_cb_evt_t;

/** @brief Group definition */
typedef enum e_adc_group
{
    ADC_GROUP_A = 0,                /*!< GroupA */
    ADC_GROUP_B,                    /*!< GroupB */
    ADC_GROUP_C                     /*!< GrouoC */
} e_adc_group_t;

/** @brief AD conversion start trigger definition */
typedef enum
{
    ADC_TRIGER_SOFT,                /*!< Software trigger */
    ADC_TRIGER_TMR,                 /*!< TMR trigger */
    ADC_TRIGER_ELC,                 /*!< ELC trigger */
    ADC_TRIGER_ADTRG,               /*!< ADTRG */
    ADC_TRIGER_LOW_PRIORITY_CONT_SCAN    /*!< Trigger non-selection definition for low priority group operation */
} e_adc_triger_t;

/** @brief AD conversion Control command definition */
typedef enum
{
    AD_CMD_SET_ADD_MODE,                /*!< Set addition mode                                 */
    AD_CMD_SET_DBLTRG,                  /*!< Enable double trigger mode                        */
    AD_CMD_SET_DIAG,                    /*!< Enable self-diagnosis mode                        */
    AD_CMD_SET_ADI_INT,                 /*!< Set interrupt of ADI_INT                          */
    AD_CMD_SET_GROUPB_INT,              /*!< Set interrupt of group B                          */
    AD_CMD_SET_GROUPC_INT,              /*!< Set interrupt of group C                          */
    AD_CMD_SET_WCMPM_INT,               /*!< Set interrupt of WCMPM                          */
    AD_CMD_SET_WCMPUM_INT,              /*!< Set interrupt of WCMPUM                          */
    AD_CMD_SET_AUTO_CLEAR,              /*!< Enable / disable automatic clearing               */
    AD_CMD_SET_SAMPLING_AN000,          /*!< AN000, change the sampling state of self diagnosis  */
    AD_CMD_SET_SAMPLING_AN001,          /*!< Change the sampling state of AN001                 */
    AD_CMD_SET_SAMPLING_AN002,          /*!< Change the sampling state of AN002                 */
    AD_CMD_SET_SAMPLING_AN003,          /*!< Change the sampling state of AN003                 */
    AD_CMD_SET_SAMPLING_AN004,          /*!< Change the sampling state of AN004                 */
    AD_CMD_SET_SAMPLING_AN005,          /*!< Change the sampling state of AN005                 */
    AD_CMD_SET_SAMPLING_AN006,          /*!< Change the sampling state of AN006                 */
    AD_CMD_SET_SAMPLING_AN007,          /*!< Change the sampling state of AN007                 */
    AD_CMD_SET_SAMPLING_AN016_AN017_AN020_AN021_VSC_VCC, /*!< Change the sampling state of AN016 , AN017, AN020, AN021, and VSC_VCC    */
    AD_CMD_SET_SAMPLING_TEMP,           /*!< Change the sampling state of the temperature sensor  */
    AD_CMD_SET_ADNDIS,                  /*!< Disconnection detection assist setting             */
    AD_CMD_SET_GROUP_PRIORITY,          /*!< Group priority setting                             */
    AD_CMD_SET_WINDOWA,                 /*!< Set comparison condition of window A               */
    AD_CMD_SET_WINDOWB,                 /*!< Set comparison condition of window B               */
    AD_CMD_SET_ELC,                     /*!< Set scan end event occurrence condition.           */
    AD_CMD_GET_CMP_RESULT,              /*!< Get window comparison result                       */
    AD_CMD_GET_AD_STATE,                /*!< Get status of AD converter                         */
    AD_CMD_USE_VREFL0,                  /*!< Select VREFL 0 as low potential reference voltage  */
    AD_CMD_USE_VREFH0,                  /*!< Select VREFH 0 as the high potential reference voltage  */
    AD_CMD_SCLK_ENABLE,                 /*!< Sub clock selection                                */
    AD_CMD_CALIBRATION,                 /*!< Performing calibration                             */
    AD_CMD_STOP_TRIG,                   /*!< Stop the A/D conversion and clear the trigger cause */
    AD_CMD_AUTO_READ_NORMAL,            /*!< Read with DMA Normal Transfer */
    AD_CMD_AUTO_READ_BLOCK,             /*!< Read with DMA BLOCK Transfer */
    AD_CMD_AUTO_READ_COMPARE,           /*!< Read compare data with DMA Normal Transfer */
    AD_CMD_AUTO_READ_STOP,              /*!< DMA stop */
    AD_CMD_AUTO_READ_RESTART            /*!< DMA restart */
} e_adc_cmd_t;

/** @brief Definition of self-diagnosis mode */
typedef enum
{
    AD_DIAG_DISABLE,                    /*!< Self-diagnosis mode disable */
    AD_DIAG_0V,                         /*!< Self-diagnosis 0V mode setting */
    AD_DIAG_HARF,                       /*!< Self-diagnosis 1/2 Reference voltage mode setting */
    AD_DIAG_BASE,                       /*!< Self-diagnosis Reference voltage mode setting */
    AD_DIAG_ROTATE                      /*!< Self-diagnosis Rotate mode setting */
} e_adc_diag_t;

/** @brief Definition of addition mode */
typedef enum
{
    ADC_ADD_OFF            = 0,               /*!< addition is turned off for chans/sensors */
    ADC_ADD_2_SAMPLES      = 1,               /*!< addition 2 samples */
    ADC_ADD_4_SAMPLES      = 3,               /*!< addition 3 samples */
    ADC_ADD_16_SAMPLES     = 5,               /*!< addition 5 samples */
    ADC_ADD_AVG_2_SAMPLES  = 0x81,            /*!< average 2 samples */
    ADC_ADD_AVG_4_SAMPLES  = 0x83,            /*!< average 4 samples */
    ADC_ADD_AVG_16_SAMPLES = 0x85             /*!< average 16 samples */
} e_adc_add_t;


/** @brief Define Group Priority */
typedef enum                            
{
    ADC_GSP_PRIORYTY_OFF           = 0x0000,     /*!< Group priority operation off */
    ADC_GSP_WAIT_TRG               = 0x0001,     /*!< Waiting for trigger */
    ADC_GSP_SCAN_BIGIN             = 0x0003,     /*!< Restart from first CH */
    ADC_GSP_SCAN_RESTART           = 0x4003,     /*!< Restart from execution CH */
    ADC_GSP_WAIT_TRG_CONT_SCAN     = 0x8001,     /*!< Waiting for trigger and continue */
    ADC_GSP_SCAN_BIGIN_CONT_SCAN   = 0x8003,     /*!< Restart from first CH and continue */
    ADC_GSP_SCAN_RESTART_CONT_SCAN = 0xC003      /*!< Restart from execution CH and continue */
} e_adc_gsp_t;

/** @brief Defining complex condition settings */
typedef enum                  
{
    ADC_COMB_OR        = 0,              /*!< AD compare OR setting */
    ADC_COMB_EXOR      = 1,              /*!< AD compare EXOR setting */
    ADC_COMB_AND       = 2,              /*!< AD compare AND setting */
    ADC_COMB_NON_EVENT = 3,              /*!< Do not generate compare function ELC event of window B */
    ADC_COMB_OFF       = 0xFF            /*!< Compare function off */
} e_adc_comb_t;

/** @brief Temperature sensor output definition */
typedef enum
{
    ADC_SENSOR_NOTUSE = 0,               /*!< not use temperature sensor */
    ADC_MSEL_TEMP     = (1 << 0)         /*!< use temperature sensor */
} e_adc_msel_sensor_t;

/** @brief Using these definitions, select the conversion result of ANn to read.@n
           [Example]@n
           @code
            ...
           Driver_ADC->Read(ADC_SSEL_AN00);    // Read the conversion result of AN00.
            ...
           @endcode
           [Use function]@n
           Driver_ADC->Read */
typedef enum
{
    ADC_SSEL_AN00    = 0,        /*!< Read ADDR0 */
    ADC_SSEL_AN01    = 1,        /*!< Read ADDR1 */
    ADC_SSEL_AN02    = 2,        /*!< Read ADDR2 */
    ADC_SSEL_AN03    = 3,        /*!< Read ADDR3 */
    ADC_SSEL_AN04    = 4,        /*!< Read ADDR4 */
    ADC_SSEL_AN05    = 5,        /*!< Read ADDR5 */
    ADC_SSEL_AN06    = 6,        /*!< Read ADDR6 */
    ADC_SSEL_AN07    = 7,        /*!< Read ADDR7 */
    ADC_SSEL_AN16    = 16,       /*!< Read ADDR16 */
    ADC_SSEL_AN17    = 17,       /*!< Read ADDR17 */
    ADC_SSEL_AN20    = 20,       /*!< Read ADDR20 */
    ADC_SSEL_AN21    = 21,       /*!< Read ADDR21 */
    ADC_SSEL_VSC_VCC = 31,       /*!< Read ADVSCDR */
    ADC_SSEL_TEMP    = 32,       /*!< Read ADTSDR */
    ADC_SSEL_DBL     = 50,       /*!< Read ADDBLDR */
    ADC_SSEL_DIAG    = 51        /*!< Read ADRD */
} e_adc_ssel_ch_t;


/** @brief Definition method for obtaining results */
typedef enum
{
    ADC_INT_DISABLE,    /*!< Interrupt disabled */
    ADC_INT_POLLING,    /*!< Polling */
    ADC_INT_ENABLE      /*!< Interrupt enabled */

} e_adc_int_method_t;

/** @brief Definition of comparison method */
typedef enum
{
    ADC_CMP_OFF,                            /*!< Do not compare                         */
    ADC_CMP_LEVEL,                          /*!< Level judgment                         */
    ADC_CMP_WINDOW,                         /*!< Window judgment                        */
} e_adc_cmp_mode_t;

/** @brief Definition of Window B comparison condition */
typedef enum
{
    ADC_WINB_LEVEL_BELOW,                        /*!< Below level                        */
    ADC_WINB_LEVEL_ABOVE,                        /*!< Level and above                    */
    ADC_WINB_WINDOW_OUTSIDE,                     /*!< Out of window range                */
    ADC_WINB_WINDOW_BETWEEN                      /*!< In the window area                 */
} e_adc_winb_cond_t;


/** @brief ELC event setting definition */
typedef enum 
{
    ADC_ELC_GROUPA = 0x00,                       /*!< ELC event occurred only in group A  */
    ADC_ELC_GROUPB = 0x01,                       /*!< ELC event occurred only in group B  */
    ADC_ELC_GROUPC = 0x04,                       /*!< ELC event occurred only in group C  */
    ADC_ELC_ALL    = 0x02                        /*!< ELC event occurred all group        */
} e_adc_elc_mode_t;

/** @brief DMA read cmd setting definition */
typedef enum 
{
    ADC_READ_ADI,                         /*!< Select ADI for AD conversion value read event. */
    ADC_READ_GBADI,                       /*!< Select GBADI for AD conversion value read event.  */
    ADC_READ_GCADI,                       /*!< Select GCADI for AD conversion value read event.  */
    ADC_READ_WCMPM,                       /*!< Select GCADI for AD conversion value read event.  */
    ADC_READ_WCMPUM,                      /*!< Select GCADI for AD conversion value read event.  */
} e_adc_dma_event_t;

/** @brief Definition of DMA transfer regster size */
typedef enum
{
    ADC_READ_BYTE,                        /*!< 8bit size  */
    ADC_READ_WORD,                        /*!< 16bit size  */
    ADC_READ_LONG                         /*!< 32bit size  */
} e_adc_read_size;

/** @brief Definition of terminal settings */
typedef struct
{            
    uint32_t              an_chans;           /*!< Enable / disable analog input channel  */
    e_adc_msel_sensor_t   sensor;             /*!< Enable / disable sensor input          */
} st_adc_pins_t;           

/** @brief Typedef definition for compare result of window A  */
typedef struct
{
    uint32_t            an_chans;           /*!< Window A compare result of analog input channel */
    uint8_t             sensor;             /*!< Window A compare result of Temperature sensor channel */
} st_adc_wina_result_t;

/** @brief Definition of window A */
typedef struct 
{
    uint8_t             inten;              /*!< Interrupt enabled                 */
    e_adc_cmp_mode_t    cmp_mode;           /*!< Comparison method                 */
    uint16_t            level1;             /*!< Comparison Level 1                */
    uint16_t            level2;             /*!< Comparison Level 2                */
    st_adc_pins_t       chans;              /*!< Target channel                    */
    st_adc_pins_t       chan_cond;          /*!< Comparative condition by channel  */
} st_adc_wina_t;

/** @brief Definition of window B */
typedef struct 
{           
    uint8_t             inten;              /*!< Interrupt enabled         */
    e_adc_winb_cond_t   winb_cond;          /*!< Comparison condition      */
    e_adc_comb_t        comb;               /*!< Compound condition        */
    uint16_t            level1;             /*!< Comparison Level 1        */
    uint16_t            level2;             /*!< Comparison Level 2        */
    e_adc_ssel_ch_t     channel;            /*!< Target channel            */
} st_adc_winb_t;           

/** @brief Typedef definition of addition mode setting */
typedef struct 
{           
    e_adc_add_t           add_mode;           /*!< Number of additions       */
    st_adc_pins_t         chans;              /*!< Target channel            */
} st_adc_add_mode_t;           

/** @brief Typedef definition of Window compare function setting */
typedef struct 
{           
    st_adc_wina_result_t   wina_result;        /*!< Window A comparison result   */
    uint8_t                winb_result;        /*!< Window B comparison result   */
    uint8_t                comb_result;        /*!< Combination comparison result  */
} st_adc_cmp_result_t;         

/** @brief Typedef definition of AD conversion status */
typedef struct 
{           
    e_adc_state_t           ad_info;            /*!< Operating state of A/D conversion         */
    e_adc_conv_state_t      groupa_info;        /*!< GroupA information  */
    e_adc_conv_state_t      groupb_info;        /*!< GroupB information  */
    e_adc_conv_state_t      groupc_info;        /*!< GroupC information  */
    e_adc_conv_state_t      wcmpm_info;         /*!< WCMPM information  */
    e_adc_conv_state_t      wcmpum_info;        /*!< WCMPUM information  */
} st_adc_status_info_t;

/** @brief Typedef definition of AD mode information and interrupt method */
typedef struct 
{           
    adc_cd_event_t        cb_event;           /*!< Event callback            */
    uint8_t               flags;              /*!< flag  */
    e_adc_int_method_t    groupa_int_method;  /*!< GroupA interrupt method  */
    e_adc_int_method_t    groupb_int_method;  /*!< GroupB interrupt method  */
    e_adc_int_method_t    groupc_int_method;  /*!< GroupC interrupt method  */
    e_adc_int_method_t    wcmpm_int_method;   /*!< WCMPM interrupt method  */
    e_adc_int_method_t    wcmpum_int_method;  /*!< WCMOUM interrupt method  */
} st_adc_mode_info_t;

/** @brief Typedef definition of DMA reading setting information */
typedef struct 
{
    system_int_cb_t       cb_event;          /*!< Event callback */
    e_adc_dma_event_t     dma_fact;          /*!< DMA transfer factor */
    uint32_t              src_addr;          /*!< DMA Transfer source address. */
    uint32_t              dest_addr;         /*!< DMA Transfer destination address. */
    uint32_t              transfer_count;    /*!< DMA Transfer count. */
    uint32_t              block_size;        /*!< DMA Block/repeat size. */
    e_adc_read_size       reg_size;          /*!< Regster size           */
} st_adc_dma_read_info_t;

/** @brief Typedef definition of AD mode information and interrupt method */
typedef struct 
{           
    system_int_cb_t        dma_adi_cb_event;     /*!< Event callback            */
    system_int_cb_t        dma_gbadi_cb_event;   /*!< Event callback            */
    system_int_cb_t        dma_gcadi_cb_event;   /*!< Event callback            */
    system_int_cb_t        dma_wcmpm_cb_event;   /*!< Event callback            */
    system_int_cb_t        dma_wcmpum_cb_event;  /*!< Event callback            */
} st_adc_dma_isr_info_t;


typedef struct
{
    uint32_t            dest_addr;                  /*!< DMA Transfer destination address. */
    uint32_t            transfer_count;             /*!< DMA Transfer count. */
    uint32_t            block_size;                 /*!< DMA Block/repeat size. */
} st_adc_dma_restart;

/** @brief Typedef definition of AD resource */
typedef struct 
{            
    volatile ADC140_Type     *reg;               /*!< Base pointer to AD registers  */
    r_pinset_t              pin_set;            /*!< ADC pin set function          */
    r_pinclr_t              pin_clr;            /*!< ADC pin clear function        */
    st_adc_mode_info_t      *adc_info;          /*!< AD infomation                 */
    st_adc_dma_isr_info_t   *dma_info;          /*!< DMA infomation                */
    e_system_mcu_lock_t     lock_id;            /*!< Module lock id */
    e_lpm_mstp_t            mstp_id;            /*!< MSTP id */
    IRQn_Type               adi_irq;            /*!< adi IRQ Number */
    IRQn_Type               gbadi_irq;          /*!< gbadi IRQ Number */
    IRQn_Type               gcadi_irq;          /*!< gcadi IRQ Number */
    IRQn_Type               cmpai_irq;          /*!< cmpai IRQ Number */
    IRQn_Type               cmpbi_irq;          /*!< cmpbi IRQ Number */
    IRQn_Type               wcmpm_irq;          /*!< wcmpm IRQ Number */
    IRQn_Type               wcmpum_irq;         /*!< wcmpum IRQ Number */
    uint32_t                adi_iesr_val;       /*!< ADI IESR Number */
    uint32_t                gbadi_iesr_val;     /*!< GBADI IESR Number */
    uint32_t                gcadi_iesr_val;     /*!< GCADI IESR Number */
    uint32_t                cmpai_iesr_val;     /*!< CMPAI IESR Number */
    uint32_t                cmpbi_iesr_val;     /*!< CMPBI IESR Number */
    uint32_t                wcmpm_iesr_val;     /*!< WCMPM IESR Number */
    uint32_t                wcmpum_iesr_val;    /*!< WCMPUM IESR Number */
    uint32_t                adi_priority;       /*!< ADI priority level */
    uint32_t                gbadi_priority;     /*!< GBADI priority level */
    uint32_t                gcadi_priority;     /*!< GCADI priority level */
    uint32_t                cmpai_priority;     /*!< CMPAI priority level */
    uint32_t                cmpbi_priority;     /*!< CMPBI priority level */
    uint32_t                wcmpm_priority;     /*!< WCMPM priority level */
    uint32_t                wcmpum_priority;    /*!< WCMPUM priority level */
#if (ADC_PRV_USED_DTC_DMAC_DRV != 0)
    DRIVER_DMA              *adi_dma_drv;       /*!< ADI DMA Driver */
    uint16_t                adi_dma_source;     /*!< ADI DMA Source */
    DRIVER_DMA              *gbadi_dma_drv;
    uint16_t                gbadi_dma_source;
    DRIVER_DMA              *gcadi_dma_drv;
    uint16_t                gcadi_dma_source;
    DRIVER_DMA              *wcmpm_dma_drv;
    uint16_t                wcmpm_dma_source;
    DRIVER_DMA              *wcmpum_dma_drv;
    uint16_t                wcmpum_dma_source;
#endif
    st_adc_dma_restart      *dma_restart_data;
    system_int_cb_t         adi_callback;           /*!< ADI callback function address */
    system_int_cb_t         gbadi_callback;         /*!< GBADI callback function address */
    system_int_cb_t         gcadi_callback;         /*!< GCADI callback function address */
    system_int_cb_t         cmpai_callback;         /*!< CMPAI callback function address */
    system_int_cb_t         cmpbi_callback;         /*!< CMPBI callback function address */
    system_int_cb_t         wcmpm_callback;         /*!< WCMPM callback function address */
    system_int_cb_t         wcmpum_callback;        /*!< WCMPUM callback function address */
    system_int_cb_t         dma_adi_callback;       /*!< ADI callback function address */
    system_int_cb_t         dma_gbadi_callback;     /*!< GBADI callback function address */
    system_int_cb_t         dma_gcadi_callback;     /*!< GCADI callback function address */
    system_int_cb_t         dma_wcmpm_callback;     /*!< WCMPM callback function address */
    system_int_cb_t         dma_wcmpum_callback;    /*!< WCMPUM callback function address */
} const st_adc_resources_t;            

/** @brief  Typedef definition of AD driver */
typedef struct _DRIVER_S14AD {
  int32_t     (*GetVersion)     (void);             /*!< Pointer to \ref R_ADC_GetVersion : Get driver version. */ // @suppress("Data type member function pointer naming")
  e_adc_err_t (*Open)     (uint32_t  mode, // @suppress("Data type member function pointer naming")
            uint8_t   default_sampling_rate,
            adc_cd_event_t const p_callback);      /*!< Pointer to \ref R_ADC_Open : Open ADC interface. */
  e_adc_err_t (*ScanSet)   (e_adc_group_t group,  // @suppress("Data type member function pointer naming")
                            st_adc_pins_t pins , 
                            e_adc_triger_t triger);/*!< Pointer to \ref R_ADC_ScanSet : ScanSet ADC interface. */
  e_adc_err_t (*Start)    (void);                  /*!< Pointer to \ref R_ADC_Start : Start ADC interface. */ // @suppress("Data type member function pointer naming")
  e_adc_err_t (*Stop)     (void);                  /*!< Pointer to \ref R_ADC_Stop : Stop ADC interface. */ // @suppress("Data type member function pointer naming")
  e_adc_err_t (*Control)  (e_adc_cmd_t const cmd, // @suppress("Data type member function pointer naming")
                           void const * const p_args); /*!< Pointer to \ref R_ADC_Control : Control ADC interface. */
  e_adc_err_t (*Read)     (e_adc_ssel_ch_t      reg_id, // @suppress("Data type member function pointer naming")
                           uint16_t * const p_data);    /*!< Pointer to \ref R_ADC_Read : Read ADC interface. */
  e_adc_err_t (*Close)   (void);                   /*!< Pointer to \ref R_ADC_Close : Close ADC interface. */ // @suppress("Data type member function pointer naming")
} const DRIVER_S14AD; // @suppress("Data type naming")


/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/
#if (S14AD_CFG_R_ADC_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_OPEN       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_OPEN
#endif
#if (S14AD_CFG_R_ADC_SCAN_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_SCANSET       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_SCANSET
#endif
#if (S14AD_CFG_R_ADC_START == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_START       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_START
#endif
#if (S14AD_CFG_R_ADC_STOP == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_STOP       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_STOP
#endif
#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_CONTROL       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_CONTROL
#endif
#if (S14AD_CFG_R_ADC_READ == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_READ       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_READ
#endif
#if (S14AD_CFG_R_ADC_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_CLOSE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_CLOSE
#endif
#if (S14AD_CFG_R_ADC_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_ADC_GET_VERSION       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_ADC_GET_VERSION
#endif

e_adc_err_t R_ADC_Open(uint32_t  mode,
                     uint8_t   default_sampling_rate,
                     adc_cd_event_t const p_callback,
                     st_adc_resources_t const * const p_adc) FUNC_LOCATION_R_ADC_OPEN;

e_adc_err_t R_ADC_ScanSet(e_adc_group_t group, 
                          st_adc_pins_t pins , 
                          e_adc_triger_t triger,
                          st_adc_resources_t * const p_adc) FUNC_LOCATION_R_ADC_SCANSET;

e_adc_err_t R_ADC_Start( st_adc_resources_t * const p_adc ) FUNC_LOCATION_R_ADC_START;
e_adc_err_t R_ADC_Stop( st_adc_resources_t * const p_adc ) FUNC_LOCATION_R_ADC_STOP;

e_adc_err_t R_ADC_Control(e_adc_cmd_t const cmd,
                        void const * const p_args,
                        st_adc_resources_t const * const p_adc) FUNC_LOCATION_R_ADC_CONTROL;

e_adc_err_t R_ADC_Read(e_adc_ssel_ch_t      reg_id,
                     uint16_t * const p_data,
                     st_adc_resources_t const * const p_adc) FUNC_LOCATION_R_ADC_READ;

e_adc_err_t   R_ADC_Close(st_adc_resources_t* const p_adc) FUNC_LOCATION_R_ADC_CLOSE;
uint32_t  R_ADC_GetVersion(void) FUNC_LOCATION_R_ADC_GET_VERSION;

#ifdef  __cplusplus
}
#endif

#if (0 != ADC_PRV_USED_DMAC_DRV)
extern DRIVER_DMA Driver_DMAC0;  // @suppress("Source line ordering")
extern DRIVER_DMA Driver_DMAC1; 
extern DRIVER_DMA Driver_DMAC2; 
extern DRIVER_DMA Driver_DMAC3; 
#endif
#if (0 != ADC_PRV_USED_DTC_DRV)
extern DRIVER_DMA Driver_DTC; 
#endif

/****************************************************************************************************************//**
 * @brief       Check and clear interrupt request
 * @param[in]   irq_num      Interrupt vect number
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void adc_clear_irq_req(IRQn_Type irq_num) // @suppress("Function description comment")
{
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > irq_num) // @suppress("Cast comment")
    {
        R_SYS_IrqStatusClear(irq_num);
        R_NVIC_ClearPendingIRQ(irq_num);
    }
}/* End of function adc_clear_irq_req() */

/****************************************************************************************************************//**
 * @brief       Check and disable interrupt
 * @param[in]   irq_num      Interrupt vect number
 *******************************************************************************************************************/
__STATIC_FORCEINLINE void adc_disable_irq(IRQn_Type irq_num) // @suppress("Function description comment")
{
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > irq_num) // @suppress("Cast comment")
    {
        R_NVIC_DisableIRQ(irq_num);
    }
}/* End of function adc_disable_irq() */


#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
/****************************************************************************************************************//**
 * @param[in]   drv     Pointer to DMA Driver
 * @param[in]   irq     DMA factors to be closed
 * @brief       DMA driver closed
 *******************************************************************************************************************/
/* Function Name : adc_close_dma */
__STATIC_FORCEINLINE void adc_close_dma(DRIVER_DMA* drv, uint16_t irq)
{
    volatile uint64_t act_src = 0;
    if (NULL != drv) // @suppress("Cast comment")
    {
#if (0 != ADC_PRV_USED_DTC_DRV)
        if ((&Driver_DTC) == drv)
        {
            /* DTC Release */
            (void)drv->Release(irq); // @suppress("Cast comment")
            (void)drv->GetAvailabilitySrc((uint64_t*)&act_src); // @suppress("Cast comment")
        }
#endif
        if (0 == act_src)
        {
            (void)drv->Close(); // @suppress("Cast comment")
        }
    }
}/* End of function adc_close_dma */
#endif

/******************************************************************************************************************//**
 * @} (end defgroup grp_hal_drv_impl_adc)
 *********************************************************************************************************************/

#endif /* R_ADC_H */
/* End of file (r_adc.h) */
