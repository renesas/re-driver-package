/*********************************************************************************************************************
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
* File Name    : r_adc.c
* Version      : 1.40
* Description  : HAL Driver for ADC
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 07.08.2018 0.50     First Release
*         : 04.10.2018 0.51     - Added trigger setting definition "ADC_TRIGER_LOW_PRIORITY_CONT_SCAN" 
*                                 for group priority operation.
*                               - Adjust the level of Start function and Stop function.
*         : 20.12.2018 0.60     - Changed the AN terminal definition and error judgment
*                                 as AN18 and AN19 were deleted in WS2.
*                               - Update comments.
*                               - Fix to clear the trigger setting with adc_cmd_group_ priority () 
*                                 and write back the trigger setting after writing the ADG SPCR register.
*                               - Fix to perform dummy read before clearing status flag with close function.
*                               Adapted to GSCE coding rules.
*                               Changed file name to r_adc_api.c
*         : 19.02.2019 0.70     Fix to be able to use temperature sensor even when VREFH 0 is selected.
*                               Added DMA linkage function
*                               Added ADC_CMPAI_SNOOZE_USE and ADC_CMPBI_SNOOZE_USE definition.
*                               - Changed not to use CMAPAI / CMPBI in SNOOZE mode.
*                               Supports WCMPM / WCMPUM interrupt.
*                               Fixed to skip processing after return value of adc_dma_drv_select function is invalid
*                               in adc_cmd_auto_read_restart function.
*         : 04.06.2019 0.72     r_adc_cfg.h Update comments
*         : 16.07.2019 1.00     Upgrade version to 1.00
*                               Fixed offset calibration process
*         : 11.10.2019 1.01     Fixed an error in the definition name.
*                               Corrected the description mistake of DMACn driver usage judgment formula of
*                               adc_dma_config function.
*         : 01.04.2020 1.20     Fixed not to include header file when DMA is not used.
*         : 05.01.2021 1.40     Fixed DTC Close processing when using DTC.
*                               If there are other DTC factors that can be used, close will be skipped.
**********************************************************************************************************************/

/**********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
**********************************************************************************************************************/
/* Configuration for this package. */
#include <stdint.h>
#include <stdbool.h>
#include "r_adc_api.h"
#include "r_adc_cfg.h"

/* Public interface header file for this package. */

/******************************************************************************************************************//**
 @addtogroup grp_device_hal_adc
 @{
 *********************************************************************************************************************/
/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/
#define ADC_PRV_CHAN_LOW_MASK       (0x007F)                          /*!< ADC register channel lower mask */
#define ADC_PRV_CHAN_HIGH_MASK      (0x01FF3)                         /*!< ADC register channel upper mask */
#define ADC_PRV_CHAN_ALL_MASK       ((ADC_PRV_CHAN_HIGH_MASK << 16) | ADC_PRV_CHAN_LOW_MASK)  /*!< ADC register all channel mask */
#define ADC_PRV_SENSOR_TEMP         (0x01)                            /*!< Temperature sensor channel */
#define ADC_PRV_TRIGER_TMR_VAL      (0x1D)                            /*!< TMR trigger setting value */
#define ADC_PRV_TRIGER_ELC_VAL      (0x30)                            /*!< ELC trigger setting value */
#define ADC_PRV_TRIGER_ADTRG_VAL    (0x00)                            /*!< ADTRG setting value */
#define ADC_PRV_TRIGER_DISABLE_VAL  (0x3F)                            /*!< Trigger disable value */
#define ADC_PRV_SENSOR_MASK         (0x01)                            /*!< Temperature sensor channel mask */

/* Interrupt exclusive control flag define for DMA control */
#define ADC_PRV_ADI_EVENT_USED        (1 << ADC_READ_ADI)             /*!< ADI event is already in use */
#define ADC_PRV_GBADI_EVENT_USED      (1 << ADC_READ_GBADI)           /*!< GBADI event is already in uset */
#define ADC_PRV_GCADI_EVENT_USED      (1 << ADC_READ_GCADI)           /*!< GCADI event is already in use */
#define ADC_PRV_WCMPM_EVENT_USED      (1 << ADC_READ_WCMPM)           /*!< WCMPM event is already in uset */
#define ADC_PRV_WCMPUM_EVENT_USED     (1 << ADC_READ_WCMPUM)          /*!< WCMPUM event is already in use */

#define ADC_PRV_ADI_DMAC_SOURCE_ID    (0x23)                          /*!< DMAC activation factor ID */
#define ADC_PRV_GBADI_DMAC_SOURCE_ID  (0x24)                          /*!< DMAC activation factor ID */
#define ADC_PRV_GCADI_DMAC_SOURCE_ID  (0x29)                          /*!< DMAC activation factor ID */
#define ADC_PRV_WCMPM_DMAC_SOURCE_ID  (0x27)                          /*!< DMAC activation factor ID */
#define ADC_PRV_WCMPUM_DMAC_SOURCE_ID (0x28)                          /*!< DMAC activation factor ID */

/******************************************************************************
 Exported global variables (to be accessed by other files)
 *****************************************************************************/
 
/**********************************************************************************************************************
Typedef definitions
**********************************************************************************************************************/
/** @brief Definition of sampling state */
typedef enum e_adc_sst // @suppress("Source line ordering")
{
    AD_SST_AN000,                   /*!< Set the sampling state of AN000 */
    AD_SST_AN001,                   /*!< Set the sampling state of AN001 */
    AD_SST_AN002,                   /*!< Set the sampling state of AN002 */
    AD_SST_AN003,                   /*!< Set the sampling state of AN003 */
    AD_SST_AN004,                   /*!< Set the sampling state of AN004 */
    AD_SST_AN005,                   /*!< Set the sampling state of AN005 */
    AD_SST_AN006,                   /*!< Set the sampling state of AN006 */
    AD_SST_AN016_AN017_AN020_TO_AN028,          /*!< Set the sampling state of AN016 to AN028 */
    AD_SST_TEMP                     /*!< Set the sampling state of Temperature sensor */
} e_adc_sst_t;

/** @brief DMA read cmd definition */
typedef enum 
{
    ADC_READ_NORMAL,                       /*!< Set Normal mode. */
    ADC_READ_BLOCK,                        /*!< Set Block mode.  */
    ADC_READ_COMPARE,                      /*!< Set Compare mode.  */
} e_adc_dma_cmd_t;

typedef struct
{
    uint8_t                 flg;
#if (ADC_PRV_USED_DTC_DMAC_DRV != 0)
    DRIVER_DMA              *dma_drv;
#endif
    uint16_t                dma_source;
    IRQn_Type               irq;
    uint32_t                iesr;
    uint32_t                priority;
#if (ADC_PRV_USED_DTC_DRV != 0)
    st_dma_transfer_data_t *transfer_info;
#endif
} st_adc_dma_set;

/**********************************************************************************************************************
Private global variables and functions
**********************************************************************************************************************/
static st_adc_mode_info_t    gs_adc0_info = {0};

#if (S14AD_CFG_R_ADC_OPEN      == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_SCAN_SET  == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_START     == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_STOP      == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_CONTROL   == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_READ      == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_CLOSE     == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_ADC_S14ADI0_ISR == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_ADC_GBADI_ISR   == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_ADC_GCADI_ISR   == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_ADC_CMPAI_ISR   == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_ADC_CMPBI_ISR   == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_ADC_WCMPM_ISR   == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_ADC_WCMPUM_ISR  == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_ADC_RESOURCES __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_ADC_RESOURCES
#endif

#if (S14AD_CFG_R_ADC_OPEN        == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_SCAN_SET    == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_START       == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_STOP        == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_CONTROL     == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_READ        == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_CLOSE       == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_ADC __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_ADC
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_ADD_MODE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_ADD_MODE
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_DBLTRG __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_DBLTRG
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_DIAG __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_DIAG
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_CLEAR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_CLEAR
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SAMPLING __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SAMPLING
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_CHARGE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_CHARGE
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_GROUP_PRIORITY __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_GROUP_PRIORITY
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWA __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWA
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWA_SUB __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWA_SUB
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWB __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWB
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_GET_CMP_RESULT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_GET_CMP_RESULT
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_GET_STATE __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_GET_STATE
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_VREFL0 __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_VREFL0
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_VREFH0 __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_VREFH0
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_SCLK __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_SCLK
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_CALIBRATION __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_CALIBRATION
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_ADI __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_ADI
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_GBADI __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_GBADI
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_GCADI __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_GCADI
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WCMPUM __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WCMPUM
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WCMPM __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WCMPM
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_ELC __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_ELC
#endif
      
#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_STOP_TRIGGER __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_STOP_TRIGGER
#endif
      
#if (S14AD_CFG_ADC_S14ADI0_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_S14ADI0_ISR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_S14ADI0_ISR
#endif

#if (S14AD_CFG_ADC_GBADI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_GBADI_ISR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_GBADI_ISR
#endif

#if (S14AD_CFG_ADC_GCADI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_GCADI_ISR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_GCADI_ISR
#endif

#if (S14AD_CFG_ADC_CMPAI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPAI_ISR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPAI_ISR
#endif

#if (S14AD_CFG_ADC_CMPBI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPBI_ISR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPBI_ISR
#endif

#if (S14AD_CFG_ADC_WCMPM_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_ISR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_ISR
#endif

#if (S14AD_CFG_ADC_WCMPUM_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_ISR __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_ISR
#endif

#if (S14AD_CFG_ADC_S14ADI0_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_ADI_INTERRUPT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_ADI_INTERRUPT
#endif

#if (S14AD_CFG_ADC_GBADI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_GBADI_INTERRUPT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_GBADI_INTERRUPT
#endif

#if (S14AD_CFG_ADC_GCADI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_GCADI_INTERRUPT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_GCADI_INTERRUPT
#endif

#if (S14AD_CFG_ADC_CMPAI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPAI_INTERRUPT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPAI_INTERRUPT
#endif

#if (S14AD_CFG_ADC_CMPBI_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPBI_INTERRUPT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMPBI_INTERRUPT
#endif

#if (S14AD_CFG_ADC_WCMPM_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_INTERRUPT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_INTERRUPT
#endif

#if (S14AD_CFG_ADC_WCMPUM_ISR == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_INTERRUPT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_INTERRUPT
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_DMA_CONFIG __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_DMA_CONFIG
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_DMA_DRV_SELECT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_DMA_DRV_SELECT
#endif

#if (S14AD_CFG_R_ADC_OPEN        == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_CLOSE       == SYSTEM_SECTION_RAM_FUNC) || \
    (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_DMA_RAM_INIT __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_DMA_RAM_INIT
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ_RESTART __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ_RESTART
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ_STOP __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ_STOP
#endif

#if (S14AD_CFG_R_ADC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ
#endif


static e_adc_err_t adc_cmd_add_mode(st_adc_add_mode_t const * const p_add_m, st_adc_resources_t * const p_adc) 
                                    STATIC_FUNC_LOCATION_PRV_ADC_CMD_ADD_MODE;
static e_adc_err_t adc_cmd_dbltrg(int8_t const * const p_dbltrg_ch, st_adc_resources_t * const p_adc) 
                                  STATIC_FUNC_LOCATION_PRV_ADC_CMD_DBLTRG;
static e_adc_err_t adc_cmd_diag(e_adc_diag_t const * const p_diag, st_adc_resources_t * const p_adc) 
                                STATIC_FUNC_LOCATION_PRV_ADC_CMD_DIAG;
static e_adc_err_t adc_cmd_auto_clear(uint8_t const * const p_en, st_adc_resources_t * const p_adc) 
                                      STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_CLEAR;
static e_adc_err_t adc_cmd_sampling(e_adc_sst_t sst ,uint8_t const * const p_sst_val, st_adc_resources_t * const p_adc)
                                    STATIC_FUNC_LOCATION_PRV_ADC_CMD_SAMPLING;
static e_adc_err_t adc_cmd_charge(uint8_t const * const p_charge, st_adc_resources_t * const p_adc) 
                                  STATIC_FUNC_LOCATION_PRV_ADC_CMD_CHARGE;
static e_adc_err_t adc_cmd_group_priority(e_adc_gsp_t const * const p_gsp, st_adc_resources_t * const p_adc) 
                                          STATIC_FUNC_LOCATION_PRV_ADC_CMD_GROUP_PRIORITY;
static e_adc_err_t adc_cmd_set_windowa(st_adc_wina_t const * const p_wina, st_adc_resources_t * const p_adc) 
                                       STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWA;
static void adc_cmd_set_windowa_sub(st_adc_wina_t const * const wina, st_adc_resources_t * const p_adc) 
                                    STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWA_SUB;
static e_adc_err_t adc_cmd_set_windowb(st_adc_winb_t const * const p_winb, st_adc_resources_t * const p_adc) 
                                       STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WINDOWB;
static e_adc_err_t adc_cmd_get_cmp_result(st_adc_cmp_result_t * const p_cmp_result, st_adc_resources_t * const p_adc) 
                                          STATIC_FUNC_LOCATION_PRV_ADC_CMD_GET_CMP_RESULT;
static e_adc_err_t adc_cmd_get_state(st_adc_status_info_t * const p_state, st_adc_resources_t const * const p_adc) 
                                     STATIC_FUNC_LOCATION_PRV_ADC_CMD_GET_STATE;
static e_adc_err_t adc_cmd_set_vrefl0(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_VREFL0;
static e_adc_err_t adc_cmd_set_vrefh0(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_VREFH0;
static e_adc_err_t adc_cmd_set_sclk(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_SCLK;
static e_adc_err_t adc_cmd_calibration(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_CMD_CALIBRATION;
static e_adc_err_t adc_cmd_set_adi(e_adc_int_method_t const  * const p_met, st_adc_resources_t * const p_adc) 
                                   STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_ADI;
static e_adc_err_t adc_cmd_set_gbadi(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) 
                                     STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_GBADI;
static e_adc_err_t adc_cmd_set_gcadi(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) 
                                     STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_GCADI;
static e_adc_err_t adc_cmd_set_wcmpm(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) 
                                     STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WCMPM;
static e_adc_err_t adc_cmd_set_wcmpum(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) 
                                     STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_WCMPUM;
static e_adc_err_t adc_cmd_set_elc(e_adc_elc_mode_t const * const p_elc, st_adc_resources_t * const p_adc) 
                                   STATIC_FUNC_LOCATION_PRV_ADC_CMD_SET_ELC;
static e_adc_err_t adc_cmd_stop_trigger(st_adc_resources_t * const p_adc)
                                        STATIC_FUNC_LOCATION_PRV_ADC_CMD_STOP_TRIGGER;
                                        
static void adc_s14adi0_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_S14ADI0_ISR;
static void adc_gbadi_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_GBADI_ISR;
static void adc_gcadi_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_GCADI_ISR;
static void adc_cmpai_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_CMPAI_ISR;
static void adc_cmpbi_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_CMPBI_ISR;
static void adc_wcmpm_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_ISR;
static void adc_wcmpum_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_ISR;

static void adc_dma_adi_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_S14ADI0_ISR;
static void adc_dma_gbadi_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_GBADI_ISR;
static void adc_dma_gcadi_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_GCADI_ISR;
static void adc_dma_wcmpm_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_ISR;
static void adc_dma_wcmpum_isr(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_ISR;

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
static e_adc_err_t adc_dma_config(st_adc_dma_read_info_t const * const p_dma, st_adc_dma_set const * const setting, e_adc_dma_cmd_t cmd, st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_DMA_CONFIG;
static e_adc_err_t adc_dma_drv_select(st_adc_dma_set * const setting, e_adc_dma_event_t event, st_adc_resources_t * const p_adc)STATIC_FUNC_LOCATION_PRV_DMA_DRV_SELECT;
static void adc_dma_ram_init(st_adc_resources_t * const p_adc) STATIC_FUNC_LOCATION_PRV_DMA_RAM_INIT;
static e_adc_err_t adc_cmd_auto_read_restart(e_adc_dma_event_t const * const event, st_adc_resources_t * const p_adc)STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ_RESTART;
static e_adc_err_t adc_cmd_auto_read_stop(e_adc_dma_event_t const * const event, st_adc_resources_t * const p_adc)STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ_STOP;
static e_adc_err_t adc_cmd_auto_read(st_adc_dma_read_info_t const * const p_dma, e_adc_dma_cmd_t cmd,  st_adc_resources_t * const p_adc)STATIC_FUNC_LOCATION_PRV_ADC_CMD_AUTO_READ;
#endif

/* ADC0 */
static int32_t S14AD0_GetVersion(void) FUNC_LOCATION_R_ADC_GET_VERSION;
static e_adc_err_t S14AD0_Open(uint32_t mode, uint8_t default_sampling_rate, adc_cd_event_t const p_callback)
                               FUNC_LOCATION_R_ADC_OPEN;
static e_adc_err_t S14AD0_ScanSet(e_adc_group_t group, st_adc_pins_t pins , e_adc_triger_t triger)
                                  FUNC_LOCATION_R_ADC_SCANSET;
static e_adc_err_t S14AD0_Start(void) FUNC_LOCATION_R_ADC_START;
static e_adc_err_t S14AD0_Stop(void) FUNC_LOCATION_R_ADC_STOP;
static e_adc_err_t S14AD0_Control(e_adc_cmd_t const cmd, void const * const p_args) FUNC_LOCATION_R_ADC_CONTROL;
static e_adc_err_t S14AD0_Read(e_adc_ssel_ch_t sel_ch, uint16_t * const p_data) FUNC_LOCATION_R_ADC_READ;
static e_adc_err_t S14AD0_Close(void) FUNC_LOCATION_R_ADC_CLOSE;

static void adc_adi_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_ADI_INTERRUPT;
static void adc_gbadi_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_GBADI_INTERRUPT;
static void adc_gcadi_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_GCADI_INTERRUPT;
static void adc_cmpai_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_CMPAI_INTERRUPT;
static void adc_cmpbi_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_CMPBI_INTERRUPT;
static void adc_wcmpm_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_INTERRUPT;
static void adc_wcmpum_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_INTERRUPT;

static void adc_dma_adi_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_ADI_INTERRUPT;
static void adc_dma_gbadi_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_GBADI_INTERRUPT;
static void adc_dma_gcadi_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_GCADI_INTERRUPT;
static void adc_dma_wcmpm_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_WCMPM_INTERRUPT;
static void adc_dma_wcmpum_interrupt(void) STATIC_FUNC_LOCATION_PRV_ADC_WCMPUM_INTERRUPT;

#if (S14AD_ADI_CONTROL == S14AD_USED_DTC)
static st_dma_transfer_data_t gs_adc_dtc_adi_info              = {0};
#endif
#if (S14AD_GBADI_CONTROL == S14AD_USED_DTC)
static st_dma_transfer_data_t gs_adc_dtc_gbadi_info            = {0};
#endif
#if (S14AD_GCADI_CONTROL == S14AD_USED_DTC)
static st_dma_transfer_data_t gs_adc_dtc_gcadi_info            = {0};
#endif
#if (S14AD_WCMPM_CONTROL == S14AD_USED_DTC)
static st_dma_transfer_data_t gs_adc_dtc_wcmpm_info            = {0};
#endif
#if (S14AD_WCMPUM_CONTROL == S14AD_USED_DTC)
static st_dma_transfer_data_t gs_adc_dtc_wcmpum_info           = {0};
#endif

static st_adc_dma_isr_info_t gs_dma_info  = {0};

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
static uint8_t gs_event_exclusion_flg = 0;
#endif
static st_adc_dma_restart gs_adc0_dma_restart_data[5];

/* ADC0 Resources */
static st_adc_resources_t gs_adc0_resources DATA_LOCATION_PRV_ADC_RESOURCES =
{
    (S14AD_Type*)S14AD, // @suppress("Cast comment")
    R_S14AD_Pinset,
    R_S14AD_Pinclr,
    &gs_adc0_info,
    &gs_dma_info,
    SYSTEM_LOCK_S14AD,
    LPM_MSTP_S14AD,
    SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_ADC140_GBADI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_ADC140_GCADI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_ADC140_CMPAI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_ADC140_CMPBI, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_ADC140_WCMPM, // @suppress("Cast comment")
    SYSTEM_CFG_EVENT_NUMBER_ADC140_WCMPUM, // @suppress("Cast comment")
    0x00000008,
    0x00000007,
    0x00000007,
    0x00000006,
    0x00000006,
    0x00000009,
    0x00000008,
    S14AD_ADI_PRIORITY,
    S14AD_GBADI_PRIORITY,
    S14AD_GCADI_PRIORITY,
    S14AD_CMPAI_PRIORITY,
    S14AD_CMPBI_PRIORITY,
    S14AD_WCMPM_PRIORITY,
    S14AD_WCMPUM_PRIORITY,
#if (ADC_PRV_USED_DTC_DMAC_DRV != 0)
#if (S14AD_ADI_CONTROL == S14AD_USED_DMAC0)
    &Driver_DMAC0,
    ADC_PRV_ADI_DMAC_SOURCE_ID,
#elif (S14AD_ADI_CONTROL == S14AD_USED_DMAC1)
    &Driver_DMAC1,
    ADC_PRV_ADI_DMAC_SOURCE_ID,
#elif (S14AD_ADI_CONTROL == S14AD_USED_DMAC2)
    &Driver_DMAC2,
    ADC_PRV_ADI_DMAC_SOURCE_ID,
#elif (S14AD_ADI_CONTROL == S14AD_USED_DMAC3)
    &Driver_DMAC3,
    ADC_PRV_ADI_DMAC_SOURCE_ID,
#elif (S14AD_ADI_CONTROL == S14AD_USED_DTC)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI,
#else
    NULL,
    0,
#endif
#if (S14AD_GBADI_CONTROL == S14AD_USED_DMAC0)
    &Driver_DMAC0,
    ADC_PRV_GBADI_DMAC_SOURCE_ID,
#elif (S14AD_GBADI_CONTROL == S14AD_USED_DMAC1)
    &Driver_DMAC1,
    ADC_PRV_GBADI_DMAC_SOURCE_ID,
#elif (S14AD_GBADI_CONTROL == S14AD_USED_DMAC2)
    &Driver_DMAC2,
    ADC_PRV_GBADI_DMAC_SOURCE_ID,
#elif (S14AD_GBADI_CONTROL == S14AD_USED_DMAC3)
    &Driver_DMAC3,
    ADC_PRV_GBADI_DMAC_SOURCE_ID,
#elif (S14AD_GBADI_CONTROL == S14AD_USED_DTC)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_ADC140_GBADI,
#else
    NULL,
    0,
#endif
#if (S14AD_GCADI_CONTROL == S14AD_USED_DMAC0)
    &Driver_DMAC0,
    ADC_PRV_GCADI_DMAC_SOURCE_ID,
#elif (S14AD_GCADI_CONTROL == S14AD_USED_DMAC1)
    &Driver_DMAC1,
    ADC_PRV_GCADI_DMAC_SOURCE_ID,
#elif (S14AD_GCADI_CONTROL == S14AD_USED_DMAC2)
    &Driver_DMAC2,
    ADC_PRV_GCADI_DMAC_SOURCE_ID,
#elif (S14AD_GCADI_CONTROL == S14AD_USED_DMAC3)
    &Driver_DMAC3,
    ADC_PRV_GCADI_DMAC_SOURCE_ID,
#elif (S14AD_GCADI_CONTROL == S14AD_USED_DTC)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_ADC140_GCADI,
#else
    NULL,
    0,
#endif
#if (S14AD_WCMPM_CONTROL == S14AD_USED_DMAC0)
    &Driver_DMAC0,
    ADC_PRV_WCMPM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPM_CONTROL == S14AD_USED_DMAC1)
    &Driver_DMAC1,
    ADC_PRV_WCMPM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPM_CONTROL == S14AD_USED_DMAC2)
    &Driver_DMAC2,
    ADC_PRV_WCMPM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPM_CONTROL == S14AD_USED_DMAC3)
    &Driver_DMAC3,
    ADC_PRV_WCMPM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPM_CONTROL == S14AD_USED_DTC)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_ADC140_WCMPM,
#else
    NULL,
    0,
#endif
#if (S14AD_WCMPUM_CONTROL == S14AD_USED_DMAC0)
    &Driver_DMAC0,
    ADC_PRV_WCMPUM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPUM_CONTROL == S14AD_USED_DMAC1)
    &Driver_DMAC1,
    ADC_PRV_WCMPUM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPUM_CONTROL == S14AD_USED_DMAC2)
    &Driver_DMAC2,
    ADC_PRV_WCMPUM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPUM_CONTROL == S14AD_USED_DMAC3)
    &Driver_DMAC3,
    ADC_PRV_WCMPUM_DMAC_SOURCE_ID,
#elif (S14AD_WCMPUM_CONTROL == S14AD_USED_DTC)
    &Driver_DTC,
    SYSTEM_CFG_EVENT_NUMBER_ADC140_WCMPUM,
#else
    NULL,
    0,
#endif
#endif
    gs_adc0_dma_restart_data,
    adc_adi_interrupt,
    adc_gbadi_interrupt,
    adc_gcadi_interrupt,
    adc_cmpai_interrupt,
    adc_cmpbi_interrupt,
    adc_wcmpm_interrupt,
    adc_wcmpum_interrupt,
    adc_dma_adi_interrupt,
    adc_dma_gbadi_interrupt,
    adc_dma_gcadi_interrupt,
    adc_dma_wcmpm_interrupt,
    adc_dma_wcmpum_interrupt
};


/**************************************************************************//**
* @brief     This function applies power to the A/D peripheral, sets the
*            operational mode, dafault sampling rate, and callback function. 
* @param[in] mode        Operational mode 
* @param[in] default_sampling_rate
*                        Set default value of sampling state. To change the sampling state individually, 
*                        change with the control function.
* @param[in] p_callback  Optional pointer to function called from interrupt when a scan completes
* @param[in] p_adc       Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERR_AD_LOCKED  Resource lock failed.
* @retval    ADC_ERROR_PARAMETER  Mode or default_sampling_rate is illegal value.
* @retval    ADC_ERROR
*******************************************************************************/
/* Function Name : R_ADC_Open */
e_adc_err_t R_ADC_Open(uint32_t mode, uint8_t default_sampling_rate, adc_cd_event_t const p_callback, // @suppress("Function length")  @suppress("Source line ordering")
                     st_adc_resources_t const * const p_adc)
{

#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    switch (mode & ADC_MODE_MASK)
    {
        case ADC_SINGLE_SCAN:
        case ADC_REPEAT_SCAN:
        case ADC_GROUP_SCAN:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
    switch (mode & ADC_RESOLUTION_MASK)
    {
        case ADC_14BIT:
        case ADC_12BIT:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    switch (mode & ADC_FORMAT_MASK)
    {
        case ADC_RIGHT:
        case ADC_LEFT:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    if (default_sampling_rate < 2)
    {
        return (ADC_ERROR_PARAMETER);
    }

#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    if (ADC_FLAG_OPENED == p_adc->adc_info->flags)
    {
        return (ADC_ERROR);
    }

    /* Lock S14AD resource */
    if (0 != R_SYS_ResourceLock(p_adc->lock_id))
    {
        return (ADC_ERROR_LOCKED);
    }
    
    /* Release module stop */
    if (0 != R_LPM_ModuleStart(p_adc->mstp_id))
    {
        R_SYS_ResourceUnlock(p_adc->lock_id);
        return (ADC_ERROR);
    }
    
    p_adc->pin_set();
    
    p_adc->adc_info->flags             = ADC_FLAG_OPENED;
    p_adc->adc_info->groupa_int_method = ADC_INT_DISABLE;
    p_adc->adc_info->groupb_int_method = ADC_INT_DISABLE;
    p_adc->adc_info->groupc_int_method = ADC_INT_DISABLE;
    p_adc->adc_info->wcmpm_int_method  = ADC_INT_DISABLE;
    p_adc->adc_info->wcmpum_int_method = ADC_INT_DISABLE;

    switch(mode & ADC_MODE_MASK)
    {
        case ADC_SINGLE_SCAN:
        {
            /* ADCSR   - A/D Control Register
               b14:13  - Scan Mode Select - Set to single scan mode */
            p_adc->reg->ADCSR = 0x0000;
        }
        break;

        case ADC_GROUP_SCAN:
        {
            /* ADCSR   - A/D Control Register
               b14:13  - Scan Mode Select - Set to group scan mode */
            p_adc->reg->ADCSR = 0x2000;
        }
        break;

        
        case ADC_REPEAT_SCAN:
        {
            /* ADCSR   - A/D Control Register
               b14:13  - ADCS - Scan Mode Select - Set to group scan mode */
            p_adc->reg->ADCSR = 0x4000;
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    
    switch(mode & ADC_RESOLUTION_MASK)
    {
        case ADC_14BIT:
        {
            /* ADPRC - A/D Conversion Resolution Setting - Set to 14bit resolution */
            p_adc->reg->ADCER_b.ADPRC = 3;
        }
        break;

        case ADC_12BIT:
        {
            /* ADPRC - A/D Conversion Resolution Setting - Set to 12bit resolution */
            p_adc->reg->ADCER_b.ADPRC = 0;
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    
    switch(mode & ADC_FORMAT_MASK)
    {
        case ADC_RIGHT:
        {
            /* ADRFMT - A/D Data Register Format Select - Set to right format */
            p_adc->reg->ADCER_b.ADRFMT = 0;
        }
        break;

        case ADC_LEFT:
        {
            /* ADRFMT - A/D Data Register Format Select - Set to left format */
            p_adc->reg->ADCER_b.ADRFMT = 1;
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    
    /* ADSSTRn - The ADSSTRn register sets the sampling time for analog input.(n = 0 ~ 6, L, T)
                 The relationship between the A/D sampling state register
                 and the associated channel is shown below.
                 ADSSTR0 - AN000, Self-Diagnosis
                 ADSSTR1 - AN001
                 ADSSTR2 - AN002
                 ADSSTR3 - AN003
                 ADSSTR4 - AN004
                 ADSSTR5 - AN005
                 ADSSTR6 - AN006
                 ADSSTRL - AN016, AN017, AN020 to AN028
                 ADSSTRT - Temperature sensor output */
    p_adc->reg->ADSSTR0 = default_sampling_rate;
    p_adc->reg->ADSSTR1 = default_sampling_rate;
    p_adc->reg->ADSSTR2 = default_sampling_rate;
    p_adc->reg->ADSSTR3 = default_sampling_rate;
    p_adc->reg->ADSSTR4 = default_sampling_rate;
    p_adc->reg->ADSSTR5 = default_sampling_rate;
    p_adc->reg->ADSSTR6 = default_sampling_rate;
    p_adc->reg->ADSSTRL = default_sampling_rate;
    p_adc->reg->ADSSTRT = default_sampling_rate;
    
    p_adc->adc_info->cb_event = p_callback;       /* Set to call back function */

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    adc_dma_ram_init(p_adc);
#endif
    return (ADC_OK);
}/* End of function R_ADC_Open() */

/**************************************************************************//**
* @brief     For the specified group, set the analog pin to be used and the trigger.
* @param[in]     group    Configration target group
* @param[in]     pins     Set the A/D conversion target pin
* @param[in]     triger   Set to starting A/D conversion trigger
* @param[in,out] p_adc Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    The member of pins(an_chans or sensor) is invalid.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
* @retval    ADC_ERROR
*                    Not opened
*******************************************************************************/
/* Function Name : R_ADC_ScanSet */
e_adc_err_t R_ADC_ScanSet(e_adc_group_t group, st_adc_pins_t pins , e_adc_triger_t triger, // @suppress("Function length")
                     st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)

    /* Check if the specified channel is valid */
    if ((0 != (pins.an_chans & (~ADC_PRV_CHAN_ALL_MASK))) ||
        (0 != (pins.sensor & (~ADC_PRV_SENSOR_MASK))))
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    /* Check if the specified group and trigger are valid */
    switch (group)
    {
        /* Check group A */
        case ADC_GROUP_A:
        {
            switch (triger)
            {
                case ADC_TRIGER_SOFT:
                {
                    /* Software trigger can not be used in group scan mode */
                    if (1 == p_adc->reg->ADCSR_b.ADCS)
                    {
                        return (ADC_ERROR_MODE);
                    }
                }
                break;
                
                case ADC_TRIGER_TMR:
                case ADC_TRIGER_ELC:
                case ADC_TRIGER_ADTRG:
                {
                    __NOP();
                }
                break;

                default:
                {
                    return (ADC_ERROR_PARAMETER);
                }
                break;
            }
        }
        break;

        /* Check group B */
        case ADC_GROUP_B:
        {
            /* Do not use group B except for group scan mode */
            if (1 != p_adc->reg->ADCSR_b.ADCS)
            {
                return (ADC_ERROR_MODE);
            }
            switch (triger)
            {
                case ADC_TRIGER_TMR:
                case ADC_TRIGER_ELC:
                case ADC_TRIGER_LOW_PRIORITY_CONT_SCAN:
                {
                    __NOP();
                }
                break;

                default:
                {
                    return (ADC_ERROR_PARAMETER);
                }
                break;
            }
        }
        break;

        /* Check group C */
        case ADC_GROUP_C:
        {
            /* Do not use group C except in group scan mode */
            if (1 != p_adc->reg->ADCSR_b.ADCS)
            {
                return (ADC_ERROR_MODE);
            }
            switch (triger)
            {
                case ADC_TRIGER_TMR:
                case ADC_TRIGER_ELC:
                case ADC_TRIGER_LOW_PRIORITY_CONT_SCAN:
                {
                    __NOP();
                }
                break;

                default:
                {
                    return (ADC_ERROR_PARAMETER);
                }
                break;
            }
        }
        break;
        
        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    if (0 == (p_adc->adc_info->flags & ADC_FLAG_OPENED))
    {
        return (ADC_ERROR);
    }
    p_adc->adc_info->flags |= ADC_FLAG_INITIALIZED;
    switch (group)
    {
        /* Single scan, continuous scan, or group scan A A/D conversion channel and trigger settings */
        case ADC_GROUP_A:
        {
            /* ADANSA0   - A/D Channel Select Register A0
                           The ANSA000 bit corresponds to AN000 and the ANSA006 bit corresponds to AN006. */
            p_adc->reg->ADANSA0 = (uint16_t)((pins.an_chans) & ADC_PRV_CHAN_LOW_MASK);

            /* ADANSA1   - A/D Channel Select Register A1
                           The ANSA16 bit corresponds to AN016, and the ANSA28 bit corresponds to AN028. */
            p_adc->reg->ADANSA1 = (uint16_t)(((pins.an_chans) >> 16) & ADC_PRV_CHAN_HIGH_MASK);

            /* TSSA - Temperature Sensor Output A/D Conversion Select */
            p_adc->reg->ADEXICR_b.TSSA = (0 != (pins.sensor & ADC_PRV_SENSOR_TEMP)) ? 1 : 0;
            switch (triger)
            {
                /* Set software trigger */
                case ADC_TRIGER_SOFT:
                {
                    /* TRGE - Trigger Start Enable - Disables A/D conversion to be started
                                                     by synchronous or asynchronous trigger. */
                    p_adc->reg->ADCSR_b.TRGE  = 0;
                }
                break;

                /* Set TMR trigger */
                case ADC_TRIGER_TMR:
                {
                     /* TRSA - A/D Conversion Start Trigger Select
                             - Compare match between TCORA and TCNT */
                    p_adc->reg->ADSTRGR_b.TRSA = ADC_PRV_TRIGER_TMR_VAL;

                    /* EXTRG - Trigger Select - A/D conversion is started by synchronous trigger.*/
                    p_adc->reg->ADCSR_b.EXTRG  = 0;

                    /* TRGE - Trigger Start Enable - Enables A/D conversion to be started
                                                     by synchronous or asynchronous trigger. */
                    p_adc->reg->ADCSR_b.TRGE   = 1;
                }
                break;

                /* Set ELC trigger */
                case ADC_TRIGER_ELC:
                {
                    /* TRSA - A/D Conversion Start Trigger Select - ELC */
                    p_adc->reg->ADSTRGR_b.TRSA = ADC_PRV_TRIGER_ELC_VAL;

                    /* EXTRG - Trigger Select - A/D conversion is started by synchronous trigger.*/
                    p_adc->reg->ADCSR_b.EXTRG = 0;

                    /* TRGE - Trigger Start Enable - Enables A/D conversion to be started
                                                     by synchronous or asynchronous trigger. */
                    p_adc->reg->ADCSR_b.TRGE  = 1;
                }
                break;

                /* Set asynchronous trigger(ADTRG) */
                case ADC_TRIGER_ADTRG:
                {
                    /* TRSA - A/D Conversion Start Trigger Select - Trigger input */
                    p_adc->reg->ADSTRGR_b.TRSA = ADC_PRV_TRIGER_ADTRG_VAL;

                    /* TEXTRG - Trigger Select - A/D conversion is started by asynchronous trigger.*/
                    p_adc->reg->ADCSR_b.EXTRG = 1;

                    /* TRGE - Trigger Start Enable - Enables A/D conversion to be started
                                                     by synchronous or asynchronous trigger. */
                    p_adc->reg->ADCSR_b.TRGE  = 1;
                }
                break;

                default:
                {
                    __NOP();
                }
                break;
            }
        }
        break;

        /* Group scan B A/D conversion channel and trigger settings */
        case ADC_GROUP_B:
        {
            /* ADANSB0   - A/D Channel Select Register B0
                           The ANSB000 bit corresponds to AN000 and the ANSB006 bit corresponds to AN006. */
            p_adc->reg->ADANSB0 = (uint16_t)((pins.an_chans) & ADC_PRV_CHAN_LOW_MASK);

            /* ADANSB1   - A/D Channel Select Register B1
                           The ANSB16 bit corresponds to AN016, and the ANSB28 bit corresponds to AN028. */
            p_adc->reg->ADANSB1 = (uint16_t)(((pins.an_chans) >> 16) & ADC_PRV_CHAN_HIGH_MASK);

            /* TSSB - Temperature Sensor Output A/D Conversion Select */
            p_adc->reg->ADEXICR_b.TSSB = (0 != (pins.sensor & ADC_PRV_SENSOR_TEMP)) ? 1 : 0;
            switch (triger)
            {
                /* Set TMR trigger */
                case ADC_TRIGER_TMR:
                {
                     /* TRSB - A/D Conversion Start Trigger Select
                             - Compare match between TCORA and TCNT */
                    p_adc->reg->ADSTRGR_b.TRSB = ADC_PRV_TRIGER_TMR_VAL;
                }
                break;

                /* Set ELC trigger */
                case ADC_TRIGER_ELC:
                {
                    /* TRSB - A/D Conversion Start Trigger Select - ELC */
                    p_adc->reg->ADSTRGR_b.TRSB = ADC_PRV_TRIGER_ELC_VAL;
                }
                break;
                case ADC_TRIGER_LOW_PRIORITY_CONT_SCAN:
                {
                    p_adc->reg->ADSTRGR_b.TRSB = ADC_PRV_TRIGER_DISABLE_VAL;
                }
                break;

                default:
                {
                    __NOP();
                }
                break;
            }
        }
        break;

        /* Group scan C A/D conversion channel and trigger settings */
        case ADC_GROUP_C:
        {
            /* ADANSC0   - A/D Channel Select Register C0
                           The ANSC000 bit corresponds to AN000 and the ANSC006 bit corresponds to AN006. */
            p_adc->reg->ADANSC0 = (uint16_t)((pins.an_chans) & ADC_PRV_CHAN_LOW_MASK);              /* Use lower 16 bits */

            /* ADANSC1   - A/D Channel Select Register C1
                           The ANSC16 bit corresponds to AN016, and the ANSC28 bit corresponds to AN028. */
            p_adc->reg->ADANSC1 = (uint16_t)(((pins.an_chans) >> 16) & ADC_PRV_CHAN_HIGH_MASK);     /* Use upper 16 bits only */

            /* TSSC - Group C Temperature Sensor Output A/D Conversion Select */
            p_adc->reg->ADGCEXCR_b.TSSC = (0 != (pins.sensor & ADC_PRV_SENSOR_TEMP)) ? 1 : 0;
            switch (triger)
            {
                /* Set TMR trigger */
                case ADC_TRIGER_TMR:
                {
                 /* TRSC - A/D Conversion Start Trigger Select
                         - Compare match between TCORA and TCNT */
                    p_adc->reg->ADGCTRGR_b.TRSC = ADC_PRV_TRIGER_TMR_VAL;
                }
                break;

                /* Set ELC trigger */
                case ADC_TRIGER_ELC:
                {
                    /* TRSB - A/D Conversion Start Trigger Select - ELC */
                    p_adc->reg->ADGCTRGR_b.TRSC = ADC_PRV_TRIGER_ELC_VAL;
                }
                break;
                case ADC_TRIGER_LOW_PRIORITY_CONT_SCAN:
                {
                    p_adc->reg->ADGCTRGR_b.TRSC = ADC_PRV_TRIGER_DISABLE_VAL;
                }
                break;

                default:
                {
                    __NOP();
                }
                break;
            }

            /* If there is no terminal setting, group C is unused */
            if ((0 != (pins.an_chans & ADC_PRV_CHAN_ALL_MASK)) || ((0 != (pins.sensor & ADC_PRV_SENSOR_MASK))))
            {
                /* GRCE - Group C A/D Conversion Operation Enable - Group C is used */
                p_adc->reg->ADGCTRGR_b.GRCE = 1;
            }
            else
            {
                /* GRCE - Group C A/D Conversion Operation Enable - Group C is not used */
                p_adc->reg->ADGCTRGR_b.GRCE = 0;
            }
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    
    return (ADC_OK);
}/* End of function R_ADC_ScanSet() */

/**************************************************************************//**
* @brief     This function starts A/D.
*                By setting the ADST bit to 1, the A/D is started.
* @param[in,out]     p_adc     Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR The AD driver is not initialized.
*******************************************************************************/
/* Function Name : R_ADC_Start */
e_adc_err_t R_ADC_Start(st_adc_resources_t * const p_adc)
{
    if (0 == (p_adc->adc_info->flags & ADC_FLAG_INITIALIZED))
    {                                  
        return (ADC_ERROR);
    }

    /* ADST - A/D Conversion Start - Starts A/D conversion process. */
    p_adc->reg->ADCSR_b.ADST = 1;
    return (ADC_OK);
}/* End of function R_ADC_Start() */

/**************************************************************************//**
* @brief     This function stops A/D.
*                By setting the ADST bit to 0, the A/D is stopped.
* @param[in,out]     p_adc     Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR The AD driver is not initialized.
* @note      In R_ADC_Stop function, ADCSR.ADST is cleared to 0 only.@n
*            To stop A/D conversion when an asynchronous trigger or a synchronous trigger is selected
*            as the condition for starting A/D conversion, use the AD_CMD_STOP_TRIG command
*            with the Control function.
*******************************************************************************/
/* Function Name : R_ADC_Stop */
e_adc_err_t R_ADC_Stop(st_adc_resources_t * const p_adc)
{
    if (0 == (p_adc->adc_info->flags & ADC_FLAG_INITIALIZED))
    {                                  
        return (ADC_ERROR);
    }

    /* ADST - A/D Conversion Start - Stop A/D conversion process. */
    p_adc->reg->ADCSR_b.ADST = 0;

    return (ADC_OK);
}/* End of function R_ADC_Stop() */

/**************************************************************************//**
* @brief     This function provides commands for enabling channels and
*                sensors and for runtime operations. These include enabling/
*                disabling trigger sources and interrupts, initiating a
*                software trigger, and checking for scan completion.
*                Commands that can be controlled by this function include 
*                addition mode, double trigger mode, self-diagnosis,
*                group n interrupt, sampling state, etc...
* @param[in]       cmd ADC Operation command
* @param[in,out]  p_args Pointer to Argument by command
* @param[in,out]  p_adc   Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER  Parameter Error
* @retval    ADC_ERROR_MODE  Mode error
* @retval    ADC_ERROR_SYSTEM_SETTING  System error
* @retval    ADC_ERROR  Unspecified error
* @retval    ADC_ERROR_BUSY  Driver is busy
*******************************************************************************/
/* Function Name : R_ADC_Control */
e_adc_err_t R_ADC_Control(e_adc_cmd_t const     cmd, // @suppress("Function length")
                         void const * const      p_args,
                         st_adc_resources_t const * const p_adc)
{
    e_adc_err_t result = ADC_OK;
    
    if (0 == (p_adc->adc_info->flags & ADC_FLAG_OPENED))
    {
        return (ADC_ERROR);
    }

    switch (cmd)
    {
        case AD_CMD_SET_ADD_MODE:                   /* Set addition mode                                */
        {
            result = adc_cmd_add_mode((st_adc_add_mode_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_DBLTRG:                     /* Enable double trigger mode                       */
        {
            result = adc_cmd_dbltrg((int8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_DIAG:                       /* Enable self-diagnosis mode                       */
        {
            result = adc_cmd_diag((e_adc_diag_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_ADI_INT:                    /* Set interrupt of ADI_INT                         */
        {
            result = adc_cmd_set_adi((e_adc_int_method_t *)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_GROUPB_INT:                 /* Set interrupt of group B                         */
        {
            result = adc_cmd_set_gbadi((e_adc_int_method_t *)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_GROUPC_INT:                 /* Set interrupt of group C                         */
        {
            result = adc_cmd_set_gcadi((e_adc_int_method_t *)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_WCMPM_INT:                 /* Set interrupt of group B                         */
        {
            result = adc_cmd_set_wcmpm((e_adc_int_method_t *)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_WCMPUM_INT:                 /* Set interrupt of group C                         */
        {
            result = adc_cmd_set_wcmpum((e_adc_int_method_t *)p_args, p_adc); // @suppress("Cast comment")
        }
        break;
        
        case AD_CMD_SET_AUTO_CLEAR:                 /* Enable / disable automatic clearing              */
        {
            result = adc_cmd_auto_clear((uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN000:             /* AN000 : change the sampling state of self diagnosis      */
        {
            result = adc_cmd_sampling(AD_SST_AN000, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN001:             /* Change the sampling state of AN001                */
        {
            result = adc_cmd_sampling(AD_SST_AN001, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN002:             /* Change the sampling state of AN002                */
        {
            result = adc_cmd_sampling(AD_SST_AN002, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN003:             /* Change the sampling state of AN003                */
        {
            result = adc_cmd_sampling(AD_SST_AN003, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN004:             /* Change the sampling state of AN004                */
        {
            result = adc_cmd_sampling(AD_SST_AN004, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN005:             /* Change the sampling state of AN005                */
        {
            result = adc_cmd_sampling(AD_SST_AN005, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN006:             /* Change the sampling state of AN006                */
        {
            result = adc_cmd_sampling(AD_SST_AN006, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_AN016_AN017_AN020_TO_AN028:/* Change the sampling state of AN016 to AN028    */
        {
            result = adc_cmd_sampling(AD_SST_AN016_AN017_AN020_TO_AN028, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_SAMPLING_TEMP:              /* Change the sampling state of the temperature sensor  */
        {
            result = adc_cmd_sampling(AD_SST_TEMP, (uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_ADNDIS:                     /* Disconnection detection assist setting            */
        {
            result = adc_cmd_charge((uint8_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_GROUP_PRIORITY:             /* Group priority setting                            */
        {
            result = adc_cmd_group_priority((e_adc_gsp_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_WINDOWA:                    /* Set comparison condition of window A              */
        {
            result = adc_cmd_set_windowa((st_adc_wina_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_SET_WINDOWB:                    /* Set comparison condition of window B              */
        {
            result = adc_cmd_set_windowb((st_adc_winb_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_GET_CMP_RESULT:                 /* Get window comparison result                      */
        {
            result = adc_cmd_get_cmp_result((st_adc_cmp_result_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_GET_AD_STATE:                   /* Get status of AD converter                        */
        {
            result = adc_cmd_get_state((st_adc_status_info_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_USE_VREFL0:                     /* Select VREFL 0 as low potential reference voltage     */
        {
            result = adc_cmd_set_vrefl0(p_adc);
        }
        break;

        case AD_CMD_USE_VREFH0:                     /* Select VREFH 0 as the high potential reference voltage */
        {
            result = adc_cmd_set_vrefh0(p_adc);
        }
        break;

        case AD_CMD_SCLK_ENABLE:                    /* Sub clock selection                               */
        {
            result = adc_cmd_set_sclk(p_adc);
        }
        break;

        case AD_CMD_CALIBRATION:                    /* Performing calibration                            */
        {
            result = adc_cmd_calibration(p_adc);
        }
        break;

        case AD_CMD_SET_ELC:                        /* Scan end event generation condition setting */
        {
            result = adc_cmd_set_elc((e_adc_elc_mode_t*)p_args, p_adc); // @suppress("Cast comment")
        }
        break;

        case AD_CMD_STOP_TRIG:                    /* Stop the A / D conversion, clear the trigger */
        {
            result = adc_cmd_stop_trigger(p_adc);
        }
        break;

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
        case AD_CMD_AUTO_READ_NORMAL:                   /* Reading the AD conversion value in DMA normal transfer */
        {
            result = adc_cmd_auto_read((st_adc_dma_read_info_t*)p_args, ADC_READ_NORMAL, p_adc);
        }
        break;
        
        case AD_CMD_AUTO_READ_BLOCK:                    /* Reading the AD conversion value in DMA block transfer */
        {
            result = adc_cmd_auto_read((st_adc_dma_read_info_t*)p_args, ADC_READ_BLOCK, p_adc);
        }
        break;
        
        case AD_CMD_AUTO_READ_COMPARE:                  /* Reading by window compare event */
        {
            result = adc_cmd_auto_read((st_adc_dma_read_info_t*)p_args, ADC_READ_COMPARE, p_adc);
        }
        break;
        
        case AD_CMD_AUTO_READ_STOP:                    /* Stop the A / D conversion, clear the trigger */
        {
            result = adc_cmd_auto_read_stop((e_adc_dma_event_t*)p_args, p_adc);
        }
        break;
        
        case AD_CMD_AUTO_READ_RESTART:                    /* Stop the A / D conversion, clear the trigger */
        {
            result = adc_cmd_auto_read_restart((e_adc_dma_event_t*)p_args, p_adc);
        }
        break;
#endif
        default:
        {
            result = ADC_ERROR_PARAMETER;
        }
        break;
    }
    return (result);
}/* End of function R_ADC_Control() */


/**************************************************************************//**
* @brief     This function reads conversion results from a single channel,
*                sensor, or the double trigger register.
* @param[in]         sel_ch   Set reading channel
* @param[in,out]    p_data   Pointer to reading data storage
* @param[in,out]    p_adc    Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    sel_ch is illegal value.
*******************************************************************************/
/* Function Name : R_ADC_Read */
e_adc_err_t R_ADC_Read(e_adc_ssel_ch_t  sel_ch,
                     uint16_t * const   p_data,
                     st_adc_resources_t const * const p_adc)
{
    uint16_t *p_dregs;

#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_data) // @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    switch (sel_ch)
    {
        case ADC_SSEL_AN00:
        case ADC_SSEL_AN01:
        case ADC_SSEL_AN02:
        case ADC_SSEL_AN03:
        case ADC_SSEL_AN04:
        case ADC_SSEL_AN05:
        case ADC_SSEL_AN06:
        case ADC_SSEL_AN16:
        case ADC_SSEL_AN17:
        case ADC_SSEL_AN20:
        case ADC_SSEL_AN21:
        case ADC_SSEL_AN22:
        case ADC_SSEL_AN23:
        case ADC_SSEL_AN24:
        case ADC_SSEL_AN25:
        case ADC_SSEL_AN26:
        case ADC_SSEL_AN27:
        case ADC_SSEL_AN28:
        case ADC_SSEL_TEMP:
        case ADC_SSEL_DBL:
        case ADC_SSEL_DIAG:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    
    switch (sel_ch)
    {
        case ADC_SSEL_TEMP:
        {
            p_dregs = (uint16_t*)(&(p_adc->reg->ADTSDR)); // @suppress("Cast comment")
        }
        break;

        case ADC_SSEL_DBL:
        {
            p_dregs = (uint16_t*)(&(p_adc->reg->ADDBLDR)); // @suppress("Cast comment")
        }
        break;
        
        case ADC_SSEL_DIAG:
        {
            p_dregs = (uint16_t*)(&(p_adc->reg->ADRD)); // @suppress("Cast comment")
        }
        break;
        
        default:
        {
            p_dregs = (uint16_t*)(&(p_adc->reg->ADDR0));      /* Base address set                                 */ // @suppress("Cast comment")
            p_dregs += sel_ch;                                /* Offset setting from base address (- setting also)*/
        }
        break;
    }
    *p_data = *p_dregs;

    return (ADC_OK);
}/* End of function R_ADC_Read() */


/**************************************************************************//**
* @brief     Stop the operation of the A/D converter and restore 
*                the register to the value after resetting.
* @param[in,out]     p_adc     Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR Not opened
*******************************************************************************/
/* Function Name : R_ADC_Close */
e_adc_err_t R_ADC_Close(st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
    volatile uint16_t dummy;
    if (0 == (p_adc->adc_info->flags & ADC_FLAG_OPENED))
    {
        return (ADC_OK);
    }
    
    /* Interrupt initialization */
    adc_disable_irq(p_adc->adi_irq);
    adc_disable_irq(p_adc->gbadi_irq);
    adc_disable_irq(p_adc->gcadi_irq);
    adc_disable_irq(p_adc->cmpai_irq);
    adc_disable_irq(p_adc->cmpbi_irq);
    adc_disable_irq(p_adc->wcmpm_irq);
    adc_disable_irq(p_adc->wcmpum_irq);
    
    adc_clear_irq_req(p_adc->adi_irq);
    adc_clear_irq_req(p_adc->gbadi_irq);
    adc_clear_irq_req(p_adc->gcadi_irq);
    adc_clear_irq_req(p_adc->cmpai_irq);
    adc_clear_irq_req(p_adc->cmpbi_irq);
    adc_clear_irq_req(p_adc->wcmpm_irq);
    adc_clear_irq_req(p_adc->wcmpum_irq);
    
    
    /* Status Flag DummyRead */
    dummy = p_adc->reg->ADCMPSR0;
    dummy = p_adc->reg->ADCMPSR1;
    dummy = p_adc->reg->ADCMPSER;
    dummy = p_adc->reg->ADCMPBSR;
    
    /* Register initialization */
    p_adc->reg->ADCSR             = 0x0000;
    p_adc->reg->ADANSA0           = 0x0000;
    p_adc->reg->ADANSA1           = 0x0000;
    p_adc->reg->ADANSB0           = 0x0000;
    p_adc->reg->ADANSB1           = 0x0000;
    p_adc->reg->ADANSC0           = 0x0000;
    p_adc->reg->ADANSC1           = 0x0000;
    p_adc->reg->ADSCS0            = 0x00;
    p_adc->reg->ADSCS1            = 0x01;
    p_adc->reg->ADSCS2            = 0x02;
    p_adc->reg->ADSCS3            = 0x03;
    p_adc->reg->ADSCS4            = 0x04;
    p_adc->reg->ADSCS5            = 0x05;
    p_adc->reg->ADSCS6            = 0x06;
    p_adc->reg->ADSCS16           = 0x10;
    p_adc->reg->ADSCS17           = 0x11;
    p_adc->reg->ADSCS20           = 0x14;
    p_adc->reg->ADSCS21           = 0x15;
    p_adc->reg->ADSCS22           = 0x16;
    p_adc->reg->ADSCS23           = 0x17;
    p_adc->reg->ADSCS24           = 0x18;
    p_adc->reg->ADSCS25           = 0x19;
    p_adc->reg->ADSCS26           = 0x1A;
    p_adc->reg->ADSCS27           = 0x1B;
    p_adc->reg->ADSCS28           = 0x1C;
    p_adc->reg->ADADS0            = 0x0000;
    p_adc->reg->ADADS1            = 0x0000;
    p_adc->reg->ADADC             = 0x00;
    p_adc->reg->ADCER             = 0x0006;
    p_adc->reg->ADSTRGR           = 0x0000;
    p_adc->reg->ADEXICR           = 0x0000;
    p_adc->reg->ADGCEXCR          = 0x00;
    p_adc->reg->ADGCTRGR          = 0x00;
    p_adc->reg->ADSSTR0           = 0x0F;
    p_adc->reg->ADSSTR1           = 0x0F;
    p_adc->reg->ADSSTR2           = 0x0F;
    p_adc->reg->ADSSTR3           = 0x0F;
    p_adc->reg->ADSSTR4           = 0x0F;
    p_adc->reg->ADSSTR5           = 0x0F;
    p_adc->reg->ADSSTR6           = 0x0F;
    p_adc->reg->ADSSTRL           = 0x0F;
    p_adc->reg->ADSSTRT           = 0x0F;
    p_adc->reg->ADDISCR           = 0x00;
    p_adc->reg->ADGSPCR           = 0x00;
    p_adc->reg->ADCMPCR           = 0x0000;
    p_adc->reg->ADCMPANSR0        = 0x0000;
    p_adc->reg->ADCMPANSR1        = 0x0000;
    p_adc->reg->ADCMPANSER        = 0x00;
    p_adc->reg->ADCMPLR0          = 0x0000;
    p_adc->reg->ADCMPLR1          = 0x0000;
    p_adc->reg->ADCMPLER          = 0x00;
    p_adc->reg->ADCMPDR0          = 0x0000;
    p_adc->reg->ADCMPDR1          = 0x0000;
    p_adc->reg->ADWINLLB          = 0x0000;
    p_adc->reg->ADWINULB          = 0x0000;
    p_adc->reg->ADCMPSR0          = 0x0000;
    p_adc->reg->ADCMPSR1          = 0x0000;
    p_adc->reg->ADCMPSER          = 0x0000;
    p_adc->reg->ADCMPBNSR         = 0x00;
    p_adc->reg->ADCMPBSR          = 0x00;
    p_adc->reg->ADHVREFCNT        = 0x00;
    p_adc->reg->ADSCLKCR          = 0x00;
    p_adc->reg->ADCALC            = 0x40;
    p_adc->reg->ADELCCR           = 0x00;
    
    /* ADST - A/D Conversion Start - Stops A/D conversion process. */
    p_adc->reg->ADCSR_b.ADST = 0;
    
    /* Set ModuleStpp */
    if (0 != R_LPM_ModuleStop(p_adc->mstp_id))
    {
        result = ADC_ERROR;
    }

    /* Resource lock release */
    R_SYS_ResourceUnlock(p_adc->lock_id);
 
    /* Clear flag */
    p_adc->adc_info->flags = 0x00;
    
    /* Clear pin setting */
    p_adc->pin_clr();


#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    /* DMA RAM and flag clear */
    adc_dma_ram_init(p_adc);
    
    /* DMA Close */
    adc_close_dma(p_adc->adi_dma_drv, p_adc->adi_dma_source);
    adc_close_dma(p_adc->gbadi_dma_drv, p_adc->gbadi_dma_source);
    adc_close_dma(p_adc->gcadi_dma_drv, p_adc->gcadi_dma_source);
    adc_close_dma(p_adc->wcmpm_dma_drv, p_adc->wcmpm_dma_source);
    adc_close_dma(p_adc->wcmpum_dma_drv, p_adc->wcmpum_dma_source);

#endif
    return (result);
}/* End of function R_ADC_Close() */


/*************************************************************************//**
* @brief     Returns the version of this module. The version number is
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.

* @retval    version number
******************************************************************************/
/* Function Name : R_ADC_GetVersion */
uint32_t  R_ADC_GetVersion(void)
{
    uint32_t const version = (ADC_VERSION_MAJOR << 16) | ADC_VERSION_MINOR;

    return (version);
}/* End of function R_ADC_GetVersion() */

/*************************************************************************//**
* @brief     This function sets addition mode.
* @param[in,out]     p_adc    Pointer to ADC resource
* @param[in]          p_add_m  Pointer to addition mode setting
* @retval    ADC_OK   Successful
* @retval    ADC_ERROR_PARAMETER    Pointer is invalid.
*                    The member of add_m(add_mode) is illegal value.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
******************************************************************************/
/* Function Name : adc_cmd_add_mode */
static e_adc_err_t adc_cmd_add_mode(st_adc_add_mode_t const * const p_add_m, st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_add_m)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    switch (p_add_m->add_mode)
    {
        case ADC_ADD_OFF:
        case ADC_ADD_2_SAMPLES:
        case ADC_ADD_4_SAMPLES:
        case ADC_ADD_AVG_2_SAMPLES:
        case ADC_ADD_AVG_4_SAMPLES:
        case ADC_ADD_AVG_16_SAMPLES:
        break; // @suppress("Switch statement")

        case ADC_ADD_16_SAMPLES:
        {
            /* In the case of 14 bit resolution, the 16 addition mode can not be used */
            if (3 == p_adc->reg->ADCER_b.ADPRC)
            {
                return (ADC_ERROR_MODE);
            }
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }

    /* Can not be used when self-diagnosis mode is enabled */
    if (1 == p_adc->reg->ADCER_b.DIAGM)
    {
        return (ADC_ERROR_MODE);
    }

    /* Check if the specified channel is valid */
    if ((0 != (p_add_m->chans.an_chans & (~ADC_PRV_CHAN_ALL_MASK))) ||
        (0 != (p_add_m->chans.sensor & (~ADC_PRV_SENSOR_MASK))))
    {
        return (ADC_ERROR_PARAMETER);
    }
#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */

    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    /* ADADC - A/D-Converted Value Addition/Average Count Select Register
       b7    - AVEE - Average Mode Enable - Depends on argument
       b6:b3 - Reserved
       b2:b0 - Addition Count Select - Depends on argument */
    p_adc->reg->ADADC  = p_add_m->add_mode;

    /* ADADS0 - A/D-Converted Value Addition/Average Function Channel Select Register 0
       b15:b8 - Reserver
       b7 :b0 - A/D-Converted Value Addition/Average Channel Select - Depends on argument */
    p_adc->reg->ADADS0 = (p_add_m->chans.an_chans) & ADC_PRV_CHAN_LOW_MASK;

    /* ADADS1 - A/D-Converted Value Addition/Average Function Channel Select Register 1
       b15:b5 - Reserved
       b5 :b0 - A/D-Converted Value Addition/Average Channel Select - Depends on argument */
    p_adc->reg->ADADS1 = ((p_add_m->chans.an_chans) >> 16) & ADC_PRV_CHAN_HIGH_MASK;
    
    /* Add / average mode setting of temperature sensor. */
    if(0 == p_add_m->chans.sensor)
    {
        /* Temperature sensor not selected */
        p_adc->reg->ADEXICR_b.TSSAD = 0;
    }
    else
    {
        /* Temperature sensor selected */
        p_adc->reg->ADEXICR_b.TSSAD = 1;
    }
    return (ADC_OK);
}/* End of function adc_cmd_add_mode() */

/*************************************************************************//**
* @brief     This function performs double trigger setting.
*                When the double trigger channel is set, double trigger function is enabled.
*                Set the DBLE bit to 1 when double trigger is enabled, and set the DBLE bit to 0 if invalid.
* @param[in]        p_dbltrg_ch    Pointer to the double trigger channel
* param[in,out]    p_adc          Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    The member of p_adc(register setting value) is illegal value.
* @retval    ADC_ERROR_MODE
*                    The operation mode is invalid.
* @retval    ADC_ERROR_BUSY
*                    Driver is busy
******************************************************************************/
/* Function Name : adc_cmd_dbltrg */
static e_adc_err_t adc_cmd_dbltrg(int8_t const * const p_dbltrg_ch, st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_dbltrg_ch)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    if (((*p_dbltrg_ch) < (-1))
     || (((*p_dbltrg_ch) > 6) && ((*p_dbltrg_ch) < 16))
     || (18 == (*p_dbltrg_ch))
     || (19 == (*p_dbltrg_ch))
     || ((*p_dbltrg_ch) > 28))
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    /* Since it can not be set in continuous scan mode, it returns an error */
    if (2 == p_adc->reg->ADCSR_b.ADCS)
    {
        return (ADC_ERROR_MODE);
    }
    
    /* In the case of software trigger, it returns an error */
    if (0 == p_adc->reg->ADCSR_b.TRGE)
    {
        return (ADC_ERROR_MODE);
    }
    
    /* It can not be used when using self-diagnosis */
    if (1 == p_adc->reg->ADCER_b.DIAGM)
    {
        return (ADC_ERROR_MODE);
    }
    
    /* Temperature sensor output Return error if used */
    if (1 == p_adc->reg->ADEXICR_b.TSSA)
    {
        return (ADC_ERROR_MODE);
    }
    
    /* It can not be set when the compare function is enabled */
    if (1 == p_adc->reg->ADCMPCR_b.CMPAE)
    {
        return (ADC_ERROR_MODE);
    }
    
#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    if ((*p_dbltrg_ch) >= 0)
    {
        /* Double trigger enable setting */
        /* DBLENS - Double Trigger Channel Select - Depends on argument */
        p_adc->reg->ADCSR_b.DBLANS = *p_dbltrg_ch;

        /* DBLE   - Selects double trigger mode. */
        p_adc->reg->ADCSR_b.DBLE = 1;
    }
    else
    {
        /* Double trigger invalid setting */
        /* DBLE - Selects double trigger mode. */
        p_adc->reg->ADCSR_b.DBLE = 0;
    }
    
    return (ADC_OK);
    
}/* End of function adc_cmd_dbltrg() */


/*************************************************************************//**
* @brief     Function to set self diagnosis.
*                In double trigger mode it can not be set and returns an error.
* @param             p_diag  Self-diagnostic function setting value.
* @param[in,out]    p_adc   Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    p_diag or p_adc is illegal value.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
******************************************************************************/
/* Function Name : adc_cmd_diag */
static e_adc_err_t adc_cmd_diag(e_adc_diag_t const * const p_diag, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_diag)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_diag)
    {
        case AD_DIAG_DISABLE:
        case AD_DIAG_0V:
        {
            __NOP();
        }
        break;
        
        case AD_DIAG_HARF:
        case AD_DIAG_BASE:
        case AD_DIAG_ROTATE:
        {
            /* Can not be used when VREFH 0 is selected */
            if (1 == p_adc->reg->ADHVREFCNT_b.HVSEL)
            {
                return (ADC_ERROR_MODE);
            }
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
    /* Can not be used when double trigger mode is enabled */
    if (1 == p_adc->reg->ADCSR_b.DBLE)
    {
        return (ADC_ERROR_MODE);
    }

    /* Can not be used when addition mode is enabled */
    if (0 != p_adc->reg->ADADC_b.ADC)
    {
        return (ADC_ERROR_MODE);
    }
    
    /* It can not be set when disconnection detection function is enabled */
    if (0 != ((p_adc->reg->ADDISCR)&0x0F))
    {
        return (ADC_ERROR_MODE);
    }

    /* It can not be set when the compare function is enabled */
    if (1 == p_adc->reg->ADCMPCR_b.CMPAE)
    {
        return (ADC_ERROR_MODE);
    }

#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }
    
    switch (*p_diag)
    {
        case AD_DIAG_DISABLE:
        {
            /* Self-diagnosis prohibition */
            /* ADCER - A/D Control Extended Register
               b11   - DIAGM - Self-Diagnosis Enable - Disables self-diagnosis of 12-bit A/D converter. */
            p_adc->reg->ADCER_b.DIAGM = 0;
        }
        break;

        case AD_DIAG_0V:
        {
            /* Self-diagnosis 0 V fixed */
            /* ADCER - A/D Control Extended Register
               b11   - DIAGM - Self-Diagnosis Enable - Enables self-diagnosis of 12-bit A/D converter.
               b10   - DIAGLD - Self-Diagnosis Mode Select - Fixed mode for self-diagnosis voltage
               b9:b8 - DIAGVAL - Self-Diagnosis Conversion Voltage Select
                               - Uses the voltage of 0 V for self-diagnosis. */
            p_adc->reg->ADCER_b.DIAGLD  = 1;
            p_adc->reg->ADCER_b.DIAGVAL = 1;
            p_adc->reg->ADCER_b.DIAGM   = 1;
        }
        break;

        case AD_DIAG_HARF:
        {
            /* Self-diagnostic reference voltage / 2 fixed */
            /* ADCER - A/D Control Extended Register
               b11   - DIAGM - Self-Diagnosis Enable - Enables self-diagnosis of 12-bit A/D converter.
               b10   - DIAGLD - Self-Diagnosis Mode Select - Fixed mode for self-diagnosis voltage
               b9:b8 - DIAGVAL - Self-Diagnosis Conversion Voltage Select
                               - Uses the voltage of reference power supply * 1/2 for self-diagnosis. */
            p_adc->reg->ADCER_b.DIAGLD  = 1;
            p_adc->reg->ADCER_b.DIAGVAL = 2;
            p_adc->reg->ADCER_b.DIAGM   = 1;
        }
        break;

        case AD_DIAG_BASE:
        {
            /* Self-diagnostic reference voltage fixed */
            /* ADCER - A/D Control Extended Register
               b11   - DIAGM - Self-Diagnosis Enable - Enables self-diagnosis of 12-bit A/D converter.
               b10   - DIAGLD - Self-Diagnosis Mode Select - Fixed mode for self-diagnosis voltage
               b9:b8 - DIAGVAL - Self-Diagnosis Conversion Voltage Select
                               - Uses the voltage of reference power supply for self-diagnosis. */
            p_adc->reg->ADCER_b.DIAGLD  = 1;
            p_adc->reg->ADCER_b.DIAGVAL = 3;
            p_adc->reg->ADCER_b.DIAGM   = 1;
        }
        break;

        case AD_DIAG_ROTATE:
        {
            /* Self-diagnosis rotate mode */
            /* ADCER - A/D Control Extended Register
               b11   - DIAGM - Self-Diagnosis Enable - Enables self-diagnosis of 12-bit A/D converter.
               b10   - DIAGLD - Self-Diagnosis Mode Select - Rotation mode for self-diagnosis voltage
               b9:b8 - DIAGVAL - Self-Diagnosis Conversion Voltage Select
                               - Uses the voltage of 0 V for self-diagnosis. */
            p_adc->reg->ADCER_b.DIAGLD  = 0;
            p_adc->reg->ADCER_b.DIAGVAL = 1;
            p_adc->reg->ADCER_b.DIAGM   = 1;
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    return (ADC_OK);
}/* End of function adc_cmd_diag() */

/*************************************************************************//**
* @brief     Set enable / disable of Automatic Clearing.
*                If Automatic Clearing is enable, set ACE to 1, otherwise set ACE to 0.
* @param[in]         p_en    Set pointer to Automatic Clearing setting value.
* @param[in,out]    p_adc  Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_en is illegal value.
******************************************************************************/
/* Function Name : adc_cmd_auto_clear */
static e_adc_err_t adc_cmd_auto_clear(uint8_t const * const p_en, st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_en)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_en)
    {
        case ADC_ENABLE:
        case ADC_DISABLE:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
#endif
    
    if (ADC_ENABLE == (*p_en))
    {
        /* ACE - A/D Data Register Automatic Clearing Enable - Enables automatic clearing. */
        p_adc->reg->ADCER_b.ACE = 1;
    }
    else
    {
        /* ACE - A/D Data Register Automatic Clearing Enable - Disables automatic clearing. */
        p_adc->reg->ADCER_b.ACE = 0;
    }
    return (ADC_OK);
}/* End of function adc_cmd_auto_clear() */


/*************************************************************************//**
* @brief     This function sets the sampling state.
*                Sets the sampling time for the selected channel.
* @param[in]        sst  It is the setting value of the sampling state.
* @param[in]        p_sst_val   Pointer to the setting value of sampling time.
* param[in,out]    p_adc    Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    Mode or default_sampling_rate is illegal value.
******************************************************************************/
/* Function Name : adc_cmd_sampling */
static e_adc_err_t adc_cmd_sampling(e_adc_sst_t sst ,uint8_t const * const p_sst_val, st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_sst_val)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    if ((*p_sst_val) < 2)
    {
        return (ADC_ERROR_PARAMETER);
    }
    
#endif
    
    switch(sst)
    {
        /* ADSSTRn - A/D Sampling State Register n (n = 0 ~ 6, L, T)
                     The ADSSTRn register sets the sampling time for analog input.
                     The set value depends on the argument.  */
        case AD_SST_AN000:
        {
            p_adc->reg->ADSSTR0 = *p_sst_val;
        }
        break;

        case AD_SST_AN001:
        {
            p_adc->reg->ADSSTR1 = *p_sst_val;
        }
        break;

        case AD_SST_AN002:
        {
            p_adc->reg->ADSSTR2 = *p_sst_val;
        }
        break;

        case AD_SST_AN003:
        {
            p_adc->reg->ADSSTR3 = *p_sst_val;
        }
        break;

        case AD_SST_AN004:
        {
            p_adc->reg->ADSSTR4 = *p_sst_val;
        }
        break;

        case AD_SST_AN005:
        {
            p_adc->reg->ADSSTR5 = *p_sst_val;
        }
        break;

        case AD_SST_AN006:
        {
            p_adc->reg->ADSSTR6 = *p_sst_val;
        }
        break;

        case AD_SST_AN016_AN017_AN020_TO_AN028:
        {
            p_adc->reg->ADSSTRL = *p_sst_val;
        }
        break;

        case AD_SST_TEMP:
        {
            p_adc->reg->ADSSTRT = *p_sst_val;
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    } 
    return (ADC_OK);
}/* End of function adc_cmd_sampling() */

/*************************************************************************//**
* @brief     This function sets disconnection detection assist.
*                Set the value of the argument (p_charge) in the ADDISCR register.
* @param[in]         p_charge   Pointer to A/D Disconnection Detection Assist setting value.
* @param[in,out]    p_adc      Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_charge is illegal value.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
******************************************************************************/
/* Function Name : adc_cmd_charge */
static e_adc_err_t adc_cmd_charge(uint8_t const * const p_charge, st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_charge)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch ((*p_charge) & (~ADC_DDA_MASK))
    {
        /* Except for the charge period, except for discharge or pre-jersey, it is determined as NG */
        case ADC_DDA_OFF:

        /* ADC_DDA_OFF and ADC_DDA_DISCHARGE have the same value */
    /*  case ADC_DDA_DISCHARGE: */
        case ADC_DDA_PRECHARGE:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
    /* Charge period 1 is prohibited */
    if (1 == ((*p_charge) & ADC_DDA_MASK))
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    /* Invalid when the temperature sensor is subject to A / D conversion */
    if ((1 == p_adc->reg->ADEXICR_b.TSSA) ||
        (1 == p_adc->reg->ADEXICR_b.TSSB) ||
        (1 == p_adc->reg->ADGCEXCR_b.TSSC))
    {
        return (ADC_ERROR_MODE);
    }
    
    /* It can not be used when using self-diagnosis */
    if (1 == p_adc->reg->ADCER_b.DIAGM)
    {
        return (ADC_ERROR_MODE);
    }
    
#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    /* ADDISCR - A/D Disconnection Detection Control Register
       b7:b5   - Reserved
       b4:b0   - ADNDIS - A/D Disconnection Detection Assist Setting - Depends on argument */
    p_adc->reg->ADDISCR = *p_charge;
    return (ADC_OK);
}/* End of function adc_cmd_charge() */


/*************************************************************************//**
* @brief     This function sets the group scan priority operation.
*                Set the group scan priority operation value to the ADGSPCR register.
* @param[in]    p_gsp     Pointer to group scan priority operation setting value.
* param[in,out]    p_adc   Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_gsp is illegal value.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
* @retval    ADC_ERROR_BUSY
*                    Driver is busy
******************************************************************************/
/* Function Name : adc_cmd_group_priority */
static e_adc_err_t adc_cmd_group_priority(e_adc_gsp_t const * const p_gsp, st_adc_resources_t * const p_adc)
{
    uint16_t adstrgr;
    uint8_t  trsc;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_gsp)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    /* It is not used except in group scan mode */
    if (1 != p_adc->reg->ADCSR_b.ADCS)
    {
        return (ADC_ERROR_MODE);
    }
    
    switch (*p_gsp)
    {
        case ADC_GSP_PRIORYTY_OFF:  
        case ADC_GSP_WAIT_TRG:
        case ADC_GSP_SCAN_BIGIN:
        case ADC_GSP_SCAN_RESTART:
        case ADC_GSP_WAIT_TRG_CONT_SCAN:
        case ADC_GSP_SCAN_BIGIN_CONT_SCAN:
        case ADC_GSP_SCAN_RESTART_CONT_SCAN:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }
    
    adstrgr = p_adc->reg->ADSTRGR;           // The register value is held in the RAM.
    trsc    = p_adc->reg->ADGCTRGR_b.TRSC;   // The register value is held in the RAM.
    p_adc->reg->ADSTRGR            = 0x3F3F; //Initialize the trigger setting.
    p_adc->reg->ADGCTRGR_b.TRSC    = 0x3F;   //Initialize the trigger setting.
    
    /* ADGSPCR - A/D Group Scan Priority Control Register
       b15     - GBRP - Single Scan Continuous Start - Depends on argument
       b14     - LGRRS - Restart Channel Select - Depends on argument
       b13:b2  - Reserved
       b1      - GBRSCN - Low-Priority Group Restart Setting - Depends on argument
       b0      - PGS - Group Priority Control Setting - Depends on argument */
    p_adc->reg->ADGSPCR = *p_gsp;
    
    p_adc->reg->ADSTRGR            = adstrgr;  // Write back the register value
    p_adc->reg->ADGCTRGR_b.TRSC    = trsc;     // Write back the register value
    
    return (ADC_OK);
}/* End of function adc_cmd_group_priority() */


/*************************************************************************//**
* @brief     This function sets the compare window A.
*                Set window function enable / disable according to compare mode.
* @param[in]        p_wina  Pointer to the window A function setting value.
* param[in,out]    p_adc   Pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_wina is illegal value.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
* @retval    ADC_ERROR_BUSY
*                    Driver is busy
******************************************************************************/
/* Function Name : adc_cmd_set_windowa */
static e_adc_err_t adc_cmd_set_windowa(st_adc_wina_t const * const p_wina, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)

    if (NULL == p_wina)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (p_wina->cmp_mode)
    {
        case ADC_CMP_OFF:
        {
            /* There is no problem unconditionally. */
            __NOP();
        }
        break;

        case ADC_CMP_WINDOW:
        {
            /* Determining the window only */
            if (p_wina->level1 == p_wina->level2)
            {
                /* If lower and upper are identical, an error is returned */
                return (ADC_ERROR_PARAMETER);
            }
        }

        /* No break. It also carries out processing of ADC_CMP_LEVEL. This was done to satisfy code coverage. */
        case ADC_CMP_LEVEL:
        {
            /* Window, level judgment common */
            if ((0x0000 == p_wina->chans.an_chans) && (0x00 == p_wina->chans.sensor))
            {
                /* Since there is no terminal setting, an error is returned */
                return (ADC_ERROR_PARAMETER);
            }
            
            if ((~(p_wina->chans.an_chans)) & p_wina->chan_cond.an_chans)
            {
                /* If a range is specified for an unspecified channel, an error is returned */
                return (ADC_ERROR_PARAMETER);
            }
            
            if ((~(p_wina->chans.sensor)) & p_wina->chan_cond.sensor)
            {
                /* If a range is specified for a sensor not specified, an error is returned */
                return (ADC_ERROR_PARAMETER);
            }
        }
        break;
        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }

    /* When Window B is enabled */
    if (1 == p_adc->reg->ADCMPCR_b.CMPBE)
    {
        /* Can not be used when sensor is used in window B */
        if (0x20 == p_adc->reg->ADCMPBNSR_b.CMPCHB)
        {
            return (ADC_ERROR_MODE);
        }
        
        /* The channel used in Window B can not be used. */
        if (0 != (p_wina->chans.an_chans & (1 << p_adc->reg->ADCMPBNSR_b.CMPCHB)))
        {
            return (ADC_ERROR_MODE);
        }
    }
    
    /* Can not be used when self-diagnosis mode is enabled */
    if (1 == p_adc->reg->ADCER_b.DIAGM)
    {
        return (ADC_ERROR_MODE);
    }

    /* Can not be used when double trigger mode is enabled */
    if (1 == p_adc->reg->ADCSR_b.DBLE)
    {
        return (ADC_ERROR_MODE);
    }

#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }
    

    
    switch (p_wina->cmp_mode)
    {
        case (ADC_CMP_OFF):
    {
            /* CMPAE  - Comparison Window A Enable - Comparison window A disabled */
            p_adc->reg->ADCMPCR_b.CMPAE = 0;

            /* CMPAIE - Comparison Window A Interrupt Enable
                      - Comparison interrupt by a match with the comparison condition (windowA) is disabled */
            p_adc->reg->ADCMPCR_b.CMPAIE = 0;

            /* CMPAE  - Comparison Window B Enable - Comparison window A disabled */
            p_adc->reg->ADCMPCR_b.CMPBE = 0;

            /* CMPAIE - Comparison Window B Interrupt Enable
                      - Comparison interrupt by a match with the comparison condition (windowA) is disabled */
            p_adc->reg->ADCMPCR_b.CMPBIE = 0;
    }
        break;

        case ADC_CMP_LEVEL:
        {
#if (0 == ADC_CMPAI_SNOOZE_USE)
             if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_adc->cmpai_irq)
            {
                 /* CMPAI Interrupt handler setting */
                if ((-1) == R_SYS_IrqEventLinkSet(p_adc->cmpai_irq, p_adc->cmpai_iesr_val, p_adc->cmpai_callback))
                {
                    result = ADC_ERROR_SYSTEM_SETTING;
                    break;
                }
    
                /* CMPAI Priority setting */
                R_NVIC_SetPriority(p_adc->cmpai_irq, p_adc->cmpai_priority);
                if ( R_NVIC_GetPriority(p_adc->cmpai_irq) != p_adc->cmpai_priority )
                {
                    result = ADC_ERROR_SYSTEM_SETTING;
                    break;
                }
            }
            else 
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }
#endif 
            /* ADCMPDR0 - A/D Comparison Function Window A Lower Level Setting Register
                          The set value depends on the argument. */
            p_adc->reg->ADCMPDR0 = p_wina->level1;

            /* WCMPE - Window Function Setting - The window function is disabled. */
            p_adc->reg->ADCMPCR_b.WCMPE = 0;
            adc_cmd_set_windowa_sub(p_wina, p_adc);
        }
        break;

            
        case ADC_CMP_WINDOW:
        {
#if (0 == ADC_CMPAI_SNOOZE_USE)
             if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_adc->cmpai_irq)
            {
                 /* CMPAI Interrupt handler setting */
                if ((-1) == R_SYS_IrqEventLinkSet(p_adc->cmpai_irq, p_adc->cmpai_iesr_val, p_adc->cmpai_callback))
                {
                    result = ADC_ERROR_SYSTEM_SETTING;
                    break;
                }
    
                /* CMPAI Priority setting */
                R_NVIC_SetPriority(p_adc->cmpai_irq, p_adc->cmpai_priority);
                if ( R_NVIC_GetPriority(p_adc->cmpai_irq) != p_adc->cmpai_priority )
                {
                    result = ADC_ERROR_SYSTEM_SETTING;
                    break;
                }
            }
            else 
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }
#endif 
            if (p_wina->level1 < p_wina->level2)
            {
               /* ADCMPDR0 - A/D Comparison Function Window A Lower Level Setting Register
                             The set value depends on the argument. */
                p_adc->reg->ADCMPDR0 = p_wina->level1;

               /* ADCMPDR1 - A/D Comparison Function Window A Upper Level Setting Register
                             The set value depends on the argument. */
                p_adc->reg->ADCMPDR1 = p_wina->level2;
            }
            else
            {
               /* ADCMPDR0 - A/D Comparison Function Window A Lower Level Setting Register
                             The set value depends on the argument. */
                p_adc->reg->ADCMPDR0 = p_wina->level2;

               /* ADCMPDR1 - A/D Comparison Function Window A Upper Level Setting Register
                             The set value depends on the argument. */
                p_adc->reg->ADCMPDR1 = p_wina->level1;
            }

            /* WCMPE - Window Function Setting - The window function is enabled */
            p_adc->reg->ADCMPCR_b.WCMPE = 1;
            adc_cmd_set_windowa_sub(p_wina, p_adc);
        }
        break;
        
    default:
    {
        __NOP();
    }
    break;
    }
    
    return (result);
}/* End of function adc_cmd_set_windowa() */

/*************************************************************************//**
* @brief     This function performs sub setting of the compare window A.
*                Set the comparison target pins and set comparison conditions.
* @param[in]         p_wina Pointer to the window A function setting value.
* @param[in,out]    p_adc
*                    Set pointer to ADC resource

******************************************************************************/
/* Function Name : adc_cmd_set_windowa_sub */
static void adc_cmd_set_windowa_sub(st_adc_wina_t const * const p_wina, st_adc_resources_t * const p_adc)
{
    /* Setting of comparison target terminal */
    /* ADCMPANSR0 - A/D Comparison Function Window A Channel Select Register 0
       b15:b8     - Reserved
       b7 :b0     - CMPCHA00n - Comparison Window A Channel Select - Depends on argument */
    p_adc->reg->ADCMPANSR0 = (uint16_t)((p_wina->chans.an_chans)&0xFFFF);

    /* ADCMPANSR1 - A/D Comparison Function Window A Channel Select Register 1
       b15:b5     - Reserved
       b5 :b0     - CMPCHA10n - Comparison Window A Channel Select - Depends on argument */
    p_adc->reg->ADCMPANSR1 = (uint16_t)(((p_wina->chans.an_chans) >> 16)&0xFFFF);

    /* ADCMPANSER - A/D Comparison Function Window A Channel Select Register 1
       b7:b1     - Reserved
       b0        - CMPCHA10n - Temperature Sensor Output Compare Select - Depends on argument */
    p_adc->reg->ADCMPANSER = p_wina->chans.sensor;
    
    /* Setting comparison conditions */
    /* ADCMPLR0 - A/D Comparison Function Window A Comparison Condition Setting Register 0
       b15:b8   - Reserved
       b7 :b0   - CMPLCHA00n - Comparison Window A Comparison Condition Select
                             - Depends on argument */
    p_adc->reg->ADCMPLR0 = (uint16_t)((p_wina->chan_cond.an_chans)&0xFFFF);

    /* ADCMPLR0 - A/D Comparison Function Window A Comparison Condition Setting Register 1
       b15:b5   - Reserved
       b4 :b0   - CMPLCHA10n - Comparison Window A Comparison Condition Select - Depends on argument*/
    p_adc->reg->ADCMPLR1 = (uint16_t)(((p_wina->chan_cond.an_chans) >> 16)&0xFFFF);

    /* ADCMPLER - A/D Comparison Function Window A Extended Input Comparison Condition
                  Setting Register
       b7:b1    - Reserved
       b0       - CMPLTSA - Compare window A Temperature Sensor Output Comparison Condition Select
                          - Depends on argument */ 
    p_adc->reg->ADCMPLER = p_wina->chan_cond.sensor;

    /* CMPAE - Comparison Window A Enable - Comparison window A enabled */
    p_adc->reg->ADCMPCR_b.CMPAE = 1;

    /* CMPAIE - Comparison Window A Interrupt Enable
              - Comparison interrupt by a match with the comparison condition(window A) is disabled */
    p_adc->reg->ADCMPCR_b.CMPAIE = 0;

#if (0 == ADC_CMPAI_SNOOZE_USE)
    if (ADC_ENABLE == p_wina->inten)
    {
        /* Enable NVIC interrupt */
        R_NVIC_EnableIRQ(p_adc->cmpai_irq);

        /* CMPAIE - Comparison Window A Interrupt Enable
              - Comparison interrupt by a match with the comparison condition(window A) is enable */
        p_adc->reg->ADCMPCR_b.CMPAIE = 1;
    }
    else
    {
    /* CMPAIE - Comparison Window A Interrupt Enable
              - Comparison interrupt by a match with the comparison condition(window A) is disabled */
        p_adc->reg->ADCMPCR_b.CMPAIE = 0;

        /* Disable NVIC interrupt */
        adc_disable_irq(p_adc->cmpai_irq);
    }
#endif
}/* End of function adc_cmd_set_windowa_sub() */


/*************************************************************************//**
* @brief     This function sets the compare window B.
*                Set window function enable / disable according to compare mode.
* @param[in]         p_winb   Pointer to the window A function setting value.
* @param[in,out]    p_adc    Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_winb is illegal value.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
* @retval    ADC_ERROR_BUSY
*                    Driver is busy
* @retval    ADC_ERROR_SYSTEM_SETTING
*                    System error
******************************************************************************/
/* Function Name : adc_cmd_set_windowb */
static e_adc_err_t adc_cmd_set_windowb(st_adc_winb_t const * const p_winb, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_winb)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (p_winb->comb)
    {
        case ADC_COMB_OFF:
        {
            /* In the case of prohibit setting, there is no problem with no condition. */
            __NOP();
        }
        break;

        case ADC_COMB_OR:
        case ADC_COMB_EXOR:
        case ADC_COMB_AND:
        {
            switch (p_winb->winb_cond)
            {
                case ADC_WINB_LEVEL_BELOW:
                case ADC_WINB_LEVEL_ABOVE:
                case ADC_WINB_WINDOW_OUTSIDE:
                case ADC_WINB_WINDOW_BETWEEN:
                {
                    __NOP();
                }
                break;
        
                default:
                {
                    return (ADC_ERROR_PARAMETER);
                }
                break;
            }
            switch(p_winb->channel)
            {
                /* Invalid arg */
                case ADC_SSEL_DBL:
                case ADC_SSEL_DIAG:
                {
                    return (ADC_ERROR_PARAMETER);
                }
                break;
                
                case ADC_SSEL_TEMP:
                break; // @suppress("Switch statement")
                
                default:
                {
                    /* The channel used in Window A can not be used. */
                    if (p_winb->channel < 7)
                    {
                        /* Check AN000 to AD006 */
                        if (0 != (p_adc->reg->ADCMPANSR0 & (1 << p_winb->channel)))
                        {
                            return (ADC_ERROR_MODE);
                        }
                    }
                    else if ((p_winb->channel > 15) && (p_winb->channel < 29))
                    {
                        /* Check AN016 to AD028 */
                        if (0 != (p_adc->reg->ADCMPANSR1 & (1 << (p_winb->channel-16))))
                        {
                            return (ADC_ERROR_MODE);
                        }
                    }
                    else
                    {
                        return (ADC_ERROR_PARAMETER);
                    }
                }
                break;
            }

            /* If the compare window A is in the disabled state, it returns an error */
            if (0 == p_adc->reg->ADCMPCR_b.CMPAE)
            {
                return (ADC_ERROR_MODE);
            }

            /* In cases other than single scan mode, event output operation can not be used */
            if (0 != p_adc->reg->ADCSR_b.ADCS)
            {
                return (ADC_ERROR_MODE);
            }
        }
        break;
        case ADC_COMB_NON_EVENT:
        {
            /* If the compare window A is in the disabled state, it returns an error */
            if (0 == p_adc->reg->ADCMPCR_b.CMPAE)
            {
                return (ADC_ERROR_MODE);
            }

            /* In cases other than single scan mode, event output operation can not be used */
            if (0 != p_adc->reg->ADCSR_b.ADCS)
            {
                return (ADC_ERROR_MODE);
            }
        }
        break;
        
        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }

    /* Can not be used when sensor is used in window A */
    if (p_adc->reg->ADCMPANSER_b.CMPTSA)
    {
        return (ADC_ERROR_MODE);
    }
        
#endif /* (1 == ADC_CFG_PARAM_CHECKING_ENABLE) */
    
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    switch (p_winb->comb)
    {
        case ADC_COMB_OR:
        case ADC_COMB_EXOR:
        case ADC_COMB_AND:
        {
#if (0 == ADC_CMPBI_SNOOZE_USE)
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED > p_adc->cmpbi_irq)
            {
                /* CMPBI Interrupt handler setting */
                if ((-1) == R_SYS_IrqEventLinkSet(p_adc->cmpbi_irq, p_adc->cmpbi_iesr_val, p_adc->cmpbi_callback))
                {
                    result = ADC_ERROR_SYSTEM_SETTING;
                    break;
                }
    
                /* CMPBI Priority setting */
                R_NVIC_SetPriority(p_adc->cmpbi_irq, p_adc->cmpbi_priority);
                if ( R_NVIC_GetPriority(p_adc->cmpbi_irq) != p_adc->cmpbi_priority )
                {
                    result = ADC_ERROR_SYSTEM_SETTING;
                    break;
                }
            }
            else 
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }
#endif 

            /* CMPCHB - Comparison Window B Channel Select - Depends on argument */
            p_adc->reg->ADCMPBNSR_b.CMPCHB = p_winb->channel;

            /* CMPAB - Window A/B Complex Conditions Setting - Depends on argument */
            p_adc->reg->ADCMPCR_b.CMPAB    = p_winb->comb;
            
            switch (p_winb->winb_cond)
            {
                case ADC_WINB_LEVEL_BELOW:
                case ADC_WINB_WINDOW_OUTSIDE:
                {
                    /* CMPLB - Comparison Window B Channel Select
                             - A/D converted value < ADWINLLB register value
                               or ADWINULB register value < A/D converted value  */
                    p_adc->reg->ADCMPBNSR_b.CMPLB = 0;
                }
                break;
        
                case ADC_WINB_LEVEL_ABOVE:
                case ADC_WINB_WINDOW_BETWEEN:
                {
                    /* CMPLB - Comparison Window B Channel Select
                             - ADWINLLB register value < A/D converted value < ADWINULB register value */
                    p_adc->reg->ADCMPBNSR_b.CMPLB = 1;
                }
                break;
        
                default:
                {
                    __NOP();
                }
                break;
            }

            switch (p_winb->winb_cond)
            {
                case ADC_WINB_LEVEL_BELOW:
                case ADC_WINB_LEVEL_ABOVE:
                {
                    /* ADWINLLB - A/D Comparison Function Window B Lower Level Setting Register
                                  The set value depends on the argument. */
                    p_adc->reg->ADWINLLB = p_winb->level1;
                }
                break;
        
                case ADC_WINB_WINDOW_OUTSIDE:
                case ADC_WINB_WINDOW_BETWEEN:
                {
                    if (p_winb->level1 < p_winb->level2)
                    {
                        /* ADWINLLB - A/D Comparison Function Window B Lower Level Setting Register
                                      The set value depends on the argument. */
                        p_adc->reg->ADWINLLB = p_winb->level1;

                        /* ADWINULB - A/D Comparison Function Window B Upper Level Setting Register
                                      The set value depends on the argument. */
                        p_adc->reg->ADWINULB = p_winb->level2;
                    }
                    else
                    {
                        /* ADWINLLB - A/D Comparison Function Window B Lower Level Setting Register
                                      The set value depends on the argument. */
                        p_adc->reg->ADWINLLB = p_winb->level2;

                        /* ADWINULB - A/D Comparison Function Window B Upper Level Setting Register
                                      The set value depends on the argument. */
                        p_adc->reg->ADWINULB = p_winb->level1;
                    }
                }
                break;
        
                default:
                {
                    __NOP();
                }
                break;
            }


#if (0 == ADC_CMPBI_SNOOZE_USE)
            if (ADC_ENABLE == p_winb->inten)
            {
                R_NVIC_EnableIRQ(p_adc->cmpbi_irq);

                /* CMPBIE - Comparison Window B Interrupt Enable
                          - Comparison interrupt by a match with the comparison condition (window B) is enabled */
                p_adc->reg->ADCMPCR_b.CMPBIE = 1;
            }
            else
            {
                /* CMPBIE - Comparison Window B Interrupt Enable
                          - Comparison interrupt by a match with the comparison condition (window B) is disabled */
                p_adc->reg->ADCMPCR_b.CMPBIE = 0;

                /* Disable NVIC interrupt */
                adc_disable_irq(p_adc->cmpbi_irq);
            }
#endif
            /* CMPBE - Comparison Window B Enable - Comparison window B enabled */
            p_adc->reg->ADCMPCR_b.CMPBE  = 1;
        }
        break;
        case ADC_COMB_NON_EVENT:
        {
            /* CMPBE - Comparison Window B Enable - Comparison window B enabled */
            p_adc->reg->ADCMPCR_b.CMPBE   = 1;

            /* ADWINLLB - A/D Comparison Function Window B Upper Level Setting Register
                                              The set value depends on the argument. */
            p_adc->reg->ADWINLLB          = 0x0000;

            /* ADWINLLB - A/D Comparison Function Window B Lower Level Setting Register
                                              The set value depends on the argument. */
            p_adc->reg->ADWINULB          = 0x0000;

            /* CMPLB - Comparison Window B Channel Select - not use */
            p_adc->reg->ADCMPBNSR         = 0x3F;

            /* CMPAB - Window A/B Complex Conditions Setting - OR */
            p_adc->reg->ADCMPCR_b.CMPAB   = 0x0;

            /* WCMPE - Window Function Setting - The window function is enabled */
            p_adc->reg->ADCMPCR_b.WCMPE   = 1;

            /* CMPLB - Comparison Window B Channel Select
                     - ADWINLLB register value < A/D converted value < ADWINULB register value */
            p_adc->reg->ADCMPBNSR_b.CMPLB = 1;
        }
        break;
        case ADC_COMB_OFF:
        {
            /* CMPBE - Comparison Window B Enable - Comparison window B disabled */
            p_adc->reg->ADCMPCR_b.CMPBE  = 0;

            /* CMPBIE - Comparison Window B Interrupt Enable
                      - Comparison interrupt by a match with the comparison condition (window B) is disabled */
            p_adc->reg->ADCMPCR_b.CMPBIE = 0;
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    
    return (result);
}/* End of function adc_cmd_set_windowb() */


/*************************************************************************//**
* @brief     This function gets the window comparison result.
* @param[in]         p_cmp_result   Pointer to result storage destination.
* @param[in,out]    p_adc          Set pointer to ADC resource.
* @retval    ADC_OK
******************************************************************************/
/* Function Name : adc_cmd_get_cmp_result */
static e_adc_err_t adc_cmd_get_cmp_result(st_adc_cmp_result_t * const p_cmp_result, st_adc_resources_t * const p_adc)
{
    uint32_t cmp_res_tmp;
    
    /* Check window A compare result */
    cmp_res_tmp = p_adc->reg->ADCMPSR1;
    cmp_res_tmp <<= 16;
    cmp_res_tmp |= (uint32_t)(p_adc->reg->ADCMPSR0); // @suppress("Cast comment")
    p_cmp_result->wina_result.an_chans = cmp_res_tmp;
    p_cmp_result->wina_result.sensor = p_adc->reg->ADCMPSER;
    
    /* Check window B compare result  */
    p_cmp_result->winb_result = p_adc->reg->ADCMPBSR;

    /* ADCMPBSR - A/D Comparison Function Window B Channel Status Register
       b7:b1    - Reserved
       b0       - Comparison Window B Flag - The comparison condition is not satisfied. */
    p_adc->reg->ADCMPBSR = 0x00;    /* Result clear */
    p_adc->reg->ADCMPSR0 = 0x00;
    p_adc->reg->ADCMPSR1 = 0x00;
    p_adc->reg->ADCMPSER = 0x00;
    
    /* Check combination result */
    p_cmp_result->comb_result = p_adc->reg->ADWINMON;

    return (ADC_OK);
}/* End of function adc_cmd_get_cmp_result() */

/*************************************************************************//**
* @brief     This function gets the state of the AD converter.
* @param[in]         p_state     Pointer to state storage destination
* @param[in,out]    p_adc       Set pointer to ADC resource
* @retval    ADC_OK
******************************************************************************/
/* Function Name : adc_cmd_get_state */
static e_adc_err_t adc_cmd_get_state(st_adc_status_info_t * const p_state, st_adc_resources_t const * const p_adc)
{
    uint8_t chk_ir;
    p_state->ad_info = (1 == p_adc->reg->ADCSR_b.ADST) ? ADC_STATE_RUN : ADC_STATE_STOP;
    if (ADC_INT_POLLING == p_adc->adc_info->groupa_int_method)
    {
    
        if (0 == R_SYS_IrqStatusGet(p_adc->adi_irq, &chk_ir))
        {
            if (1 == chk_ir)
            {
                p_state->groupa_info = ADC_CONV_COMPLETE;
                (void)adc_clear_irq_req(p_adc->adi_irq); // @suppress("Cast comment")
            }
            else
            {
                p_state->groupa_info = ADC_CONV_INCOMPLETE;
            }
        }
        else
        {
            p_state->groupa_info = ADC_CONV_GET_FAILED;
        }
    }
    else
    {
        p_state->groupa_info = ADC_CONV_NOT_POLLING;
    }
    
    if (ADC_INT_POLLING == p_adc->adc_info->groupb_int_method)
    {
        if (0 == R_SYS_IrqStatusGet(p_adc->gbadi_irq, &chk_ir))
        {
            if (1 == chk_ir)
            {
                p_state->groupb_info = ADC_CONV_COMPLETE;
                (void)adc_clear_irq_req(p_adc->gbadi_irq); // @suppress("Cast comment")
            }
            else
            {
                p_state->groupb_info = ADC_CONV_INCOMPLETE;
            }
        }
        else
        {
            p_state->groupb_info = ADC_CONV_GET_FAILED;
        }
    }
    else
    {
        p_state->groupb_info = ADC_CONV_NOT_POLLING;
    }
    
    if (ADC_INT_POLLING == p_adc->adc_info->groupc_int_method)
    {
        if (0 == R_SYS_IrqStatusGet(p_adc->gcadi_irq, &chk_ir))
        {
            if(1 == chk_ir)
            {
                p_state->groupc_info = ADC_CONV_COMPLETE;
                (void)adc_clear_irq_req(p_adc->gcadi_irq); // @suppress("Cast comment")
            }
            else
            {
                p_state->groupc_info = ADC_CONV_INCOMPLETE;
            }
        }
        else
        {
            p_state->groupc_info = ADC_CONV_GET_FAILED;
        }

    }
    else
    {
        p_state->groupc_info = ADC_CONV_NOT_POLLING;
    }
    
    if (ADC_INT_POLLING == p_adc->adc_info->wcmpm_int_method)
    {
        if (0 == R_SYS_IrqStatusGet(p_adc->wcmpm_irq, &chk_ir))
        {
            if(1 == chk_ir)
            {
                p_state->wcmpm_info = ADC_CONV_WCMP_DETECT;
                (void)adc_clear_irq_req(p_adc->wcmpm_irq); // @suppress("Cast comment")
            }
            else
            {
                p_state->wcmpm_info = ADC_CONV_WCMP_NOT_DETECT;
            }
        }
        else
        {
            p_state->wcmpm_info = ADC_CONV_GET_FAILED;
        }

    }
    else
    {
        p_state->wcmpm_info = ADC_CONV_NOT_POLLING;
    }
    
    if (ADC_INT_POLLING == p_adc->adc_info->wcmpum_int_method)
    {
        if (0 == R_SYS_IrqStatusGet(p_adc->wcmpum_irq, &chk_ir))
        {
            if(1 == chk_ir)
            {
                p_state->wcmpum_info = ADC_CONV_WCMP_DETECT;
                (void)adc_clear_irq_req(p_adc->wcmpum_irq); // @suppress("Cast comment")
            }
            else
            {
                p_state->wcmpum_info = ADC_CONV_WCMP_NOT_DETECT;
            }
        }
        else
        {
            p_state->wcmpum_info = ADC_CONV_GET_FAILED;
        }

    }
    else
    {
        p_state->wcmpum_info = ADC_CONV_NOT_POLLING;
    }

    return (ADC_OK);
}/* End of function adc_cmd_get_state() */

/*************************************************************************//**
* @brief     This function sets VREFL0 to the low voltage reference voltage.
* @param[in,out]     p_adc    Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_BUSY
*                    Driver is busy
******************************************************************************/
/* Function Name : adc_cmd_set_vrefl0 */
static e_adc_err_t adc_cmd_set_vrefl0(st_adc_resources_t * const p_adc)
{
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    /* LVSEL - Low reference voltage select - VREFL0 */
    p_adc->reg->ADHVREFCNT_b.LVSEL = 1;
    
    return (ADC_OK);
}/* End of function adc_cmd_set_vrefl0() */

/*************************************************************************//**
* @brief     This function sets VREFH0 to the high potential reference voltage.
* @param[in,out]     p_adc    Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_BUSY
*                    Driver is busy
* @retval    ADC_ERROR_PARAMETER
*                    p_adc is invalid.
* @retval    ADC_ERROR_MODE
*                    Can not be used in the current mode.
******************************************************************************/
/* Function Name : adc_cmd_set_vrefh0 */
static e_adc_err_t adc_cmd_set_vrefh0(st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (1 == p_adc->reg->ADCER_b.DIAGM)
    {
        if ((0 == p_adc->reg->ADCER_b.DIAGLD)
          ||(2 == p_adc->reg->ADCER_b.DIAGVAL)
          ||(3 == p_adc->reg->ADCER_b.DIAGVAL))
        {
            return (ADC_ERROR_MODE);
        }
    }
    
#endif
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    /* HVSEL - High reference voltage select - 11b */
    p_adc->reg->ADHVREFCNT_b.HVSEL = 3;

    /* HVSEL - High reference voltage select - VREFH0 */
    p_adc->reg->ADHVREFCNT_b.HVSEL = 1;
    
    
    return (ADC_OK);
}/* End of function adc_cmd_set_vrefh0() */

/*************************************************************************//**
* @brief     This is a function to set the sub clock.
*                Set the SCLKEN bit to 1 to set the subclock mode.
* @param[in,out]     p_adc     Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_BUSY
*                    Trigger is enabled.
* @retval    ADC_ERROR
*                    ADCLK is not less than 32.768 kHz.
******************************************************************************/
/* Function Name : adc_cmd_set_sclk */
static e_adc_err_t adc_cmd_set_sclk(st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    uint32_t freq = 0;
    R_SYS_PeripheralClockFreqGet(&freq);

    /* Acquire ADCLK and OK only when it is 32768 or less. */
    if (32768 < freq)
    {
        return (ADC_ERROR);
    }
#endif
    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    /* SCLKEN - Sub Clock Select - Use subclock */
    p_adc->reg->ADSCLKCR_b.SCLKEN = 1;
    
    return (ADC_OK);
}/* End of function adc_cmd_set_sclk() */

/*************************************************************************//**
* @brief     This function performs calibration of ADC.
*                The values of ADCSR, ADCER, and ADSSTR0 are retained
*                and are set again after calibration.
* @param[in,out]     p_adc    Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR
*                    It was executed in an invalid state (ADC not opened).
* @retval    ADC_ERROR_BUSY
*                    Driver is busy
******************************************************************************/
/* Function Name : adc_cmd_calibration */
static e_adc_err_t adc_cmd_calibration(st_adc_resources_t * const p_adc)
{
    uint16_t tmp_adcsr;
    uint16_t tmp_adcer;
    uint8_t  tmp_adsstr;
    
    if (0 != (p_adc->adc_info->flags & ADC_FLAG_INITIALIZED))
    {
        /* Calibration can only be performed before the "ScanSet" function call. */
        return (ADC_ERROR);
    }

    /* Can not be set except when the ADCSR.ADST bit is 0 */
    if (1 == p_adc->reg->ADCSR_b.ADST)
    {
        return (ADC_ERROR_BUSY);
    }

    tmp_adcsr  = p_adc->reg->ADCSR;
    tmp_adcer  = p_adc->reg->ADCER;
    tmp_adsstr = p_adc->reg->ADSSTR0;
    
    /* Register initialization */
    p_adc->reg->ADSSTR1 = 0x0F;
    p_adc->reg->ADSSTR2 = 0x0F;
    p_adc->reg->ADSSTR3 = 0x0F;
    p_adc->reg->ADSSTR4 = 0x0F;
    p_adc->reg->ADSSTR5 = 0x0F;
    p_adc->reg->ADSSTR6 = 0x0F;
    p_adc->reg->ADSSTRL = 0x0F;
    p_adc->reg->ADSSTRT = 0x0F;
    
    /* ADCSR   - A/D Control Register
       b15     - ADST - A/D Conversion Start - Stops A/D conversion process.
       b14:b13 - ADCS - Scan Mode Select - Single scan mode
       b12     - ADIE - Scan End Interrupt Enable - Disables interrupt generation upon scan completion.
       b9      - TRGE - Trigger Start Enable - Disables A/D conversion to be started by synchronous or
                        asynchronous trigger.
       b8      - EXTRG - Trigger Select - A/D conversion is started by synchronous trigger.
       b7      - DBLE - Double Trigger Mode Select - Deselects double trigger mode.
       b6      - GBADIE - Group B Scan End Interrupt Enable
               - Disables interrupt generation upon group B scan completion.
       b4:b0   - DBLANS - Double Trigger Channel Select - Double Trigger Enabled Channels : AN000 */
    p_adc->reg->ADCSR   = 0x0000;

    /* ADCER   - A/D Control Extended Register
       b15     - ADRFMT - A/D Data Register Format Select
               - Flush-right is selected for the A/D data register format.
       b11     - DIAGM - Self-Diagnosis Enable - Disables self-diagnosis of 12-bit A/D converter.
       b10     - DIAGLD - Self-Diagnosis Mode Select - Rotation mode for self-diagnosis voltage
       b9:b8   - DIAGVAL - Self-Diagnosis Conversion Voltage Select
               - Setting prohibited in self-diagnosis voltage fixed mode
       b5      - ACE - A/D Data Register Automatic Clearing Enable - Disables automatic clearing.
       b2:b0   - ADPRC - A/D Conversion Resolution Setting
               - Perform A/D conversion with setting for 12-bit resolution */
    p_adc->reg->ADCER   = 0x0006;

    /* ADSSTRn - The ADSSTRn register sets the sampling time for analog input.
                 ADSSTR0 - AN000 */
    p_adc->reg->ADSSTR0 = 0xFF;

    /* ADC - Addition Count Select - 1-time conversion */
    p_adc->reg->ADADC_b.ADC = 0;

    /* AVEE - Average Mode Enable - Enable */
    p_adc->reg->ADADC_b.AVEE = 1;
    
    /* CALST - Performing calibration - Perform calibration at next A / D conversion start. */
    p_adc->reg->ADCALC = 0xC0;

    /* ADST - A/D Conversion Start - Starts A/D conversion process. */
    p_adc->reg->ADCSR_b.ADST   = 1;
    
    
    while (1 == p_adc->reg->ADCSR_b.ADST)
    {
        /* wait calibration */
        __NOP();
    }
    
    p_adc->reg->ADCSR   = tmp_adcsr;
    p_adc->reg->ADCER   = tmp_adcer;
    p_adc->reg->ADSSTR0 = tmp_adsstr;
    p_adc->reg->ADSSTR1 = tmp_adsstr;
    p_adc->reg->ADSSTR2 = tmp_adsstr;
    p_adc->reg->ADSSTR3 = tmp_adsstr;
    p_adc->reg->ADSSTR4 = tmp_adsstr;
    p_adc->reg->ADSSTR5 = tmp_adsstr;
    p_adc->reg->ADSSTR6 = tmp_adsstr;
    p_adc->reg->ADSSTRL = tmp_adsstr;
    p_adc->reg->ADSSTRT = tmp_adsstr;

    /* AVEE - Average Mode Enable - Disable */
    p_adc->reg->ADADC_b.AVEE = 0;
    
    return (ADC_OK);
}/* End of function adc_cmd_calibration() */

/*************************************************************************//**
* @brief     This function sets the groupA scan end interrupt.
*                Set the interrupt enable / disable by operating the ADIE bit.
* @param[in,out]  p_adc   Pointer to ADC resource
* @param[in]       p_met   Pointer to ADC interrupt method setting value.
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_met is invalid.
* @retval    ADC_ERROR_SYSTEM_SETTING
*                    System error
******************************************************************************/
/* Function Name : adc_cmd_set_adi */
static e_adc_err_t adc_cmd_set_adi(e_adc_int_method_t const  * const p_met, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_met)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        case ADC_INT_POLLING:
        case ADC_INT_ENABLE:
        break; // @suppress("Switch statement")

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    if (0 != (p_adc->adc_info->flags & ( 1 << (ADC_READ_ADI + ADC_FLAG_DMA_POS))))
    {
        return (ADC_ERROR);
    }
#endif
#endif

    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        {
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_adc->adi_irq) // @suppress("Cast comment")
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* ADIE - Scan End Interrupt Enable - Disables interrupt generation upon scan completion. */
            p_adc->reg->ADCSR_b.ADIE = 0;

            /* NVIC interrupt disabled */
            adc_disable_irq(p_adc->adi_irq);
            p_adc->adc_info->groupa_int_method = *p_met;

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to false */
            gs_event_exclusion_flg &= ~ADC_PRV_ADI_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_POLLING:
        {
            /* ADI Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->adi_irq, p_adc->adi_iesr_val, p_adc->adi_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* ADI Priority setting */
            R_NVIC_SetPriority(p_adc->adi_irq, p_adc->adi_priority);
            if ( R_NVIC_GetPriority(p_adc->adi_irq) != p_adc->adi_priority )
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* NVIC interrupt disabled */
             adc_disable_irq(p_adc->adi_irq);

            /* ADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
            p_adc->reg->ADCSR_b.ADIE = 1;
            p_adc->adc_info->groupa_int_method = *p_met;    

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_ADI_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_ENABLE:
        {
            /* ADI Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->adi_irq, p_adc->adi_iesr_val, p_adc->adi_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* ADI Priority setting */
            R_NVIC_SetPriority(p_adc->adi_irq, p_adc->adi_priority);
            if ( R_NVIC_GetPriority(p_adc->adi_irq) != p_adc->adi_priority )
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* ADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
            p_adc->reg->ADCSR_b.ADIE = 1;

            /* Enable NVIC interrupt */
            R_NVIC_EnableIRQ(p_adc->adi_irq);
            p_adc->adc_info->groupa_int_method = *p_met;

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_ADI_EVENT_USED;
#endif
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    return (result);
}/* End of function adc_cmd_set_adi() */

/*************************************************************************//**
* @brief     This function sets the groupB scan end interrupt.
*                Set the interrupt enable / disable by operating the GBADIE bit.
* @param[in,out]    p_adc   Pointer to ADC resource
*                    p_met   Pointer to ADC interrupt method setting value.
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_met is invalid.
* @retval    ADC_ERROR_SYSTEM_SETTING
*                    System error
******************************************************************************/
/* Function Name : adc_cmd_set_gbadi */
static e_adc_err_t adc_cmd_set_gbadi(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_met)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        case ADC_INT_POLLING:
        case ADC_INT_ENABLE:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    if (0 != (p_adc->adc_info->flags & ( 1 << (ADC_READ_GBADI + ADC_FLAG_DMA_POS))))
    {
        return (ADC_ERROR);
    }
#endif
#endif

    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        {
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_adc->gbadi_irq) // @suppress("Cast comment")
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GBADIE - Group B Scan End Interrupt Disable - Disables interrupt generation upon scan completion. */
            p_adc->reg->ADCSR_b.GBADIE = 0;

            /* NVIC interrupt disabled */
            adc_disable_irq(p_adc->gbadi_irq);
            p_adc->adc_info->groupb_int_method = *p_met;
        
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to false */
            gs_event_exclusion_flg &= ~ADC_PRV_GBADI_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_POLLING:
        {
            /* GBADI Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->gbadi_irq, p_adc->gbadi_iesr_val, p_adc->gbadi_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GBADI Priority setting */
            R_NVIC_SetPriority(p_adc->gbadi_irq, p_adc->gbadi_priority);
            if ( R_NVIC_GetPriority(p_adc->gbadi_irq) != p_adc->gbadi_priority )
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* NVIC interrupt disabled */
            adc_disable_irq(p_adc->gbadi_irq);

            /* GBADIE - Group B Scan Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
            p_adc->reg->ADCSR_b.GBADIE = 1;
            p_adc->adc_info->groupb_int_method = *p_met;
                        
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_GBADI_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_ENABLE:
        {
            /* GBADI Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->gbadi_irq, p_adc->gbadi_iesr_val, p_adc->gbadi_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GBADI Priority setting */
            R_NVIC_SetPriority(p_adc->gbadi_irq, p_adc->gbadi_priority);
            if ( R_NVIC_GetPriority(p_adc->gbadi_irq) != p_adc->gbadi_priority )
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GBADIE - Group B Scan Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
            p_adc->reg->ADCSR_b.GBADIE = 1;

            /* Enable NVIC interrupt */
            R_NVIC_EnableIRQ(p_adc->gbadi_irq);
            p_adc->adc_info->groupb_int_method = *p_met;
            
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_GBADI_EVENT_USED;
#endif
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    return (result);
}/* End of function adc_cmd_set_gbadi() */

/*************************************************************************//**
* @brief     This function sets the groupC scan end interrupt.
*                Set the interrupt enable / disable by operating the GCADIE bit.
* @param[in,out]    p_adc   Pointer to ADC resource
* @param[in]         p_met   Pointer to ADC interrupt method setting value.
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_met is illegal value.
* @retval    ADC_ERROR_SYSTEM_SETTING
*                    System error
******************************************************************************/
/* Function Name : adc_cmd_set_gcadi */
static e_adc_err_t adc_cmd_set_gcadi(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_met)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        case ADC_INT_POLLING:
        case ADC_INT_ENABLE:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    if (0 != (p_adc->adc_info->flags & ( 1 << (ADC_READ_GCADI + ADC_FLAG_DMA_POS))))
    {
        return (ADC_ERROR);
    }
#endif
#endif

    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        {
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_adc->gcadi_irq) // @suppress("Cast comment")
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GCADIE - Group C Scan End Interrupt Enable - Disables interrupt generation upon scan completion. */
            p_adc->reg->ADGCTRGR_b.GCADIE = 0;

            /* NVIC interrupt disabled */
            adc_disable_irq(p_adc->gcadi_irq);
            p_adc->adc_info->groupc_int_method = *p_met;
        
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to false */
            gs_event_exclusion_flg &= ~ADC_PRV_GCADI_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_POLLING:
        {
            /* GCADI Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->gcadi_irq, p_adc->gcadi_iesr_val, p_adc->gcadi_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GCADI Priority setting */
            R_NVIC_SetPriority(p_adc->gcadi_irq, p_adc->gcadi_priority);
            if ( R_NVIC_GetPriority(p_adc->gcadi_irq) != p_adc->gcadi_priority)
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* NVIC interrupt disabled */
             adc_disable_irq(p_adc->gcadi_irq);

            /* GCADIE - Group C Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
            p_adc->reg->ADGCTRGR_b.GCADIE = 1;
            p_adc->adc_info->groupc_int_method = *p_met;
            
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_GCADI_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_ENABLE:
        {
          /* GCADI Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->gcadi_irq, p_adc->gcadi_iesr_val, p_adc->gcadi_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GCADI Priority setting */
            R_NVIC_SetPriority(p_adc->gcadi_irq, p_adc->gcadi_priority);
            if ( R_NVIC_GetPriority(p_adc->gcadi_irq) != p_adc->gcadi_priority )
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* GCADIE - Group C Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
            p_adc->reg->ADGCTRGR_b.GCADIE = 1;

            /* Enable NVIC interrupt */
            R_NVIC_EnableIRQ(p_adc->gcadi_irq);
            p_adc->adc_info->groupc_int_method = *p_met;
            
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_GCADI_EVENT_USED;
#endif
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    return (result);
}/* End of function adc_cmd_set_gcadi() */

/*************************************************************************//**
* @brief     Set WCMPM interrupt permission / polling / prohibition setting.
* @param[in,out]    p_adc   Pointer to ADC resource
* @param[in]        p_met   Pointer to ADC interrupt method setting value.
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_met is illegal value.
* @retval    ADC_ERROR_SYSTEM_SETTING
*                    System error
******************************************************************************/
/* Function Name : adc_cmd_set_wcmpm */
static e_adc_err_t adc_cmd_set_wcmpm(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_met)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        case ADC_INT_POLLING:
        case ADC_INT_ENABLE:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    if (0 != (p_adc->adc_info->flags & ( 1 << (ADC_READ_WCMPM + ADC_FLAG_DMA_POS))))
    {
        return (ADC_ERROR);
    }
#endif
#endif

    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        {
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_adc->wcmpm_irq) // @suppress("Cast comment")
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }
            
            /* NVIC interrupt disabled */
            adc_disable_irq(p_adc->wcmpm_irq);
            p_adc->adc_info->wcmpm_int_method = *p_met;
        
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to false */
            gs_event_exclusion_flg &= ~ADC_PRV_WCMPM_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_POLLING:
        {
            /* WCMPM Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->wcmpm_irq, p_adc->wcmpm_iesr_val, p_adc->wcmpm_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* WCMPM Priority setting */
            R_NVIC_SetPriority(p_adc->wcmpm_irq, p_adc->wcmpm_priority);
            if ( R_NVIC_GetPriority(p_adc->wcmpm_irq) != p_adc->wcmpm_priority)
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* NVIC interrupt disabled */
             adc_disable_irq(p_adc->wcmpm_irq);

            p_adc->adc_info->wcmpm_int_method = *p_met;
            
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_WCMPM_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_ENABLE:
        {
          /* WCMPM Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->wcmpm_irq, p_adc->wcmpm_iesr_val, p_adc->wcmpm_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* WCMPM Priority setting */
            R_NVIC_SetPriority(p_adc->wcmpm_irq, p_adc->wcmpm_priority);
            if ( R_NVIC_GetPriority(p_adc->wcmpm_irq) != p_adc->wcmpm_priority )
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* Enable NVIC interrupt */
            R_NVIC_EnableIRQ(p_adc->wcmpm_irq);
            p_adc->adc_info->wcmpm_int_method = *p_met;
            
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_WCMPM_EVENT_USED;
#endif
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    return (result);
}/* End of function adc_cmd_set_wcmpm() */

/*************************************************************************//**
* @brief     Set WCMPUM interrupt permission / polling / prohibition setting.
* @param[in,out]    p_adc   Pointer to ADC resource
* @param[in]        p_met   Pointer to ADC interrupt method setting value.
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER
*                    Pointer is invalid.
*                    p_met is illegal value.
* @retval    ADC_ERROR_SYSTEM_SETTING
*                    System error
******************************************************************************/
/* Function Name : adc_cmd_set_wcmpum */
static e_adc_err_t adc_cmd_set_wcmpum(e_adc_int_method_t const * const p_met, st_adc_resources_t * const p_adc) // @suppress("Function length")
{
    e_adc_err_t result = ADC_OK;
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_met)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        case ADC_INT_POLLING:
        case ADC_INT_ENABLE:
        {
            __NOP();
        }
        break;

        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break;
    }
    
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    if (0 != (p_adc->adc_info->flags & ( 1 << (ADC_READ_WCMPUM + ADC_FLAG_DMA_POS))))
    {
        return (ADC_ERROR);
    }
#endif
#endif

    switch (*p_met)
    {
        case ADC_INT_DISABLE:
        {
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_adc->wcmpum_irq) // @suppress("Cast comment")
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }
            
            /* NVIC interrupt disabled */
            adc_disable_irq(p_adc->wcmpum_irq);
            p_adc->adc_info->wcmpum_int_method = *p_met;
        
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to false */
            gs_event_exclusion_flg &= ~ADC_PRV_WCMPUM_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_POLLING:
        {
            /* WCMPUM Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->wcmpum_irq, p_adc->wcmpum_iesr_val, p_adc->wcmpum_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* WCMPUM Priority setting */
            R_NVIC_SetPriority(p_adc->wcmpum_irq, p_adc->wcmpum_priority);
            if ( R_NVIC_GetPriority(p_adc->wcmpum_irq) != p_adc->wcmpum_priority)
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* NVIC interrupt disabled */
             adc_disable_irq(p_adc->wcmpum_irq);

            p_adc->adc_info->wcmpum_int_method = *p_met;
            
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_WCMPUM_EVENT_USED;
#endif
        }
        break;

        case ADC_INT_ENABLE:
        {
          /* WCMPUM Interrupt handler setting */
            if ((-1) == R_SYS_IrqEventLinkSet(p_adc->wcmpum_irq, p_adc->wcmpum_iesr_val, p_adc->wcmpum_callback))
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* WCMPUM Priority setting */
            R_NVIC_SetPriority(p_adc->wcmpum_irq, p_adc->wcmpum_priority);
            if ( R_NVIC_GetPriority(p_adc->wcmpum_irq) != p_adc->wcmpum_priority )
            {
                result = ADC_ERROR_SYSTEM_SETTING;
                break;
            }

            /* Enable NVIC interrupt */
            R_NVIC_EnableIRQ(p_adc->wcmpum_irq);
            p_adc->adc_info->wcmpum_int_method = *p_met;
            
#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
            /* Set the DMA exclusive control flag to true */
            gs_event_exclusion_flg |= ADC_PRV_WCMPUM_EVENT_USED;
#endif
        }
        break;

        default:
        {
            __NOP();
        }
        break;
    }
    return (result);
}/* End of function adc_cmd_set_wcmpum() */

/*************************************************************************//**
* @brief     This function sets the event occurrence condition of scan end event (ELC).
* @param[in]         p_elc  Set pointer to ELC mode
* @param[in,out]    p_adc  Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_BUSY
*                    Trigger is enabled.
* @retval    ADC_ERROR ADCLK is not less than 32.768 kHz.
******************************************************************************/
/* Function Name : adc_cmd_set_elc */
static e_adc_err_t adc_cmd_set_elc(e_adc_elc_mode_t const * const p_elc, st_adc_resources_t * const p_adc)
{
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_elc)// @suppress("Cast comment")
    {
        return (ADC_ERROR_PARAMETER);
    }
    
    switch (*p_elc)
        {
            case ADC_ELC_GROUPA:
            case ADC_ELC_GROUPB:
            case ADC_ELC_GROUPC:
            case ADC_ELC_ALL:
            {
                __NOP();
            }
            break;
            default:
            {
                return (ADC_ERROR_PARAMETER);
            }
            break;
        }
#endif


    /* ADELCCR - A / D event link control register
       b3      - GCELC - Group C event link control
       b1:b0   - ELCC - Event link control */
    p_adc->reg->ADELCCR = *p_elc;
    
    return (ADC_OK);
}/* End of function adc_cmd_set_elc() */

/*************************************************************************//**
* @brief     Stop the A / D conversion and clear the trigger cause
* @param[in,out]     p_adc  Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @note    When this function is executed, the following items are cleared.
*          To re-execute the A / D conversion, 
*          set the trigger again with the "R_ADC_ScanSet" function.
*                - ADCSR.ADST
*                - ADGSPCR.PGS
*                - ADSTRGR
*                - ADGCTRGR
*                - All interrupt enable bit
*                - ADCSR
******************************************************************************/
/* Function Name : adc_cmd_stop_trigger */
static e_adc_err_t adc_cmd_stop_trigger(st_adc_resources_t * const p_adc)
{
    volatile uint16_t dummy;
    
    if (0 == (p_adc->adc_info->flags & ADC_FLAG_INITIALIZED))
    {                                  
        return (ADC_ERROR);
    }
    
    /* Stop trigger event and clear interrupt enable */
    p_adc->reg->ADGSPCR_b.PGS     = 0;
    p_adc->reg->ADSTRGR           = 0x3F3F;
    p_adc->reg->ADGCTRGR_b.TRSC   = 0x3F;
    p_adc->reg->ADGCTRGR_b.GCADIE = 0;
    p_adc->reg->ADCSR            &= 0xEFBF;     /* Clear ADIE and GBADIE */
    
    /* ADST - A/D Conversion Start - Stops A/D conversion process. */
    p_adc->reg->ADCSR_b.ADST = 0;
    
    /* wait 2ADCLK(Wait due to dummy read of A/D register) */
    dummy = p_adc->reg->ADCSR;
    dummy = p_adc->reg->ADCSR;

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
    if(NULL != p_adc->adi_dma_drv)
    {
        if (S14AD_ADI_CONTROL != S14AD_USED_DTC)
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        }
        else
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &p_adc->adi_dma_source);
        }
    }
    if(NULL != p_adc->gbadi_dma_drv)
    {
        if (S14AD_GBADI_CONTROL != S14AD_USED_DTC)
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        }
        else
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &p_adc->gbadi_dma_source);
        }
    }   
    if(NULL != p_adc->gcadi_dma_drv)
    {
        if (S14AD_GCADI_CONTROL != S14AD_USED_DTC)
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        }
        else
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &p_adc->gcadi_dma_source);
        }
    }
    if(NULL != p_adc->wcmpm_dma_drv)
    {
        if (S14AD_WCMPM_CONTROL != S14AD_USED_DTC)
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        }
        else
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &p_adc->wcmpm_dma_source);
        }
    }
    if(NULL != p_adc->wcmpum_dma_drv)
    {
        if (S14AD_WCMPUM_CONTROL != S14AD_USED_DTC)
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, NULL);
        }
        else
        {
            p_adc->adi_dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &p_adc->wcmpum_dma_source);
        }
    }
#endif
    
    return (ADC_OK);
}/* End of function adc_cmd_stop_trigger() */

#if (0 != ADC_PRV_USED_DTC_DMAC_DRV)
/*************************************************************************//**
* @brief     Setting of reading AD conversion value by DMA control.
* @param[in]         p_dma  Set pointer to dma argment
* @param[in]         cmd    command
* @param[in,out]     p_adc  Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER The argument value is invalid.
* @retval    ADC_ERROR_SYSTEM_SETTING System setting is invalid.
******************************************************************************/
/* Function Name : adc_cmd_auto_read */
static e_adc_err_t adc_cmd_auto_read(st_adc_dma_read_info_t const * const p_dma, e_adc_dma_cmd_t cmd,  st_adc_resources_t * const p_adc)
{
    e_adc_err_t    result  = ADC_OK;
    st_adc_dma_set dma_set = {0};
    
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    /* DMA source event used? */
    if (0 != (gs_event_exclusion_flg & (1 << p_dma->dma_fact)))
    {
        /* Is not using DMA control? */
        if (0 == (p_adc->adc_info->flags & ( 1 << (p_dma->dma_fact + ADC_FLAG_DMA_POS))))
        {
            /* If it is already used by interrupt control, it return ADC_ERROR. */
            return (ADC_ERROR);
        }
    }
    
    switch(p_dma->dma_fact)
    {
        case ADC_READ_ADI:
        case ADC_READ_GBADI:
        case ADC_READ_GCADI:
        {
            if (ADC_READ_COMPARE == cmd)
            {
                /* In cases the ADC_READ_COMPARE command, ADI, GBADI and GCADI are invalid. */
                return (ADC_ERROR_PARAMETER);
            }
        }
        break;
        
        case ADC_READ_WCMPM:
        case ADC_READ_WCMPUM:
        {
            if (ADC_READ_COMPARE != cmd)
            {
                /* In cases other than the ADC_READ_COMPARE command, WCMPM and WCMPUM are invalid. */
                return (ADC_ERROR_PARAMETER);
            }
            switch (p_dma->reg_size)
            {
                case ADC_READ_BYTE:
                case ADC_READ_WORD:
                case ADC_READ_LONG:
                {
                    __NOP();
                }
                break;
                default:
                {
                    if (ADC_READ_COMPARE == cmd)
                    {
                        /* In the case of compare specification, register size specification is mandatory. */
                        return (ADC_ERROR_PARAMETER);
                    }
                }
                break;
            }
        }
        break;
        
        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break; 
    }
    
#endif

    result = adc_dma_drv_select(&dma_set, p_dma->dma_fact, p_adc);
    if (ADC_OK == result)
    {
        result = adc_dma_config(p_dma, &dma_set, cmd, p_adc);
    }
        
    if (ADC_OK == result)
    {
        /* Retain information for restart */
        p_adc->dma_restart_data[p_dma->dma_fact].dest_addr      = p_dma->dest_addr;
        p_adc->dma_restart_data[p_dma->dma_fact].transfer_count = p_dma->transfer_count;
        
        if (ADC_READ_BLOCK == cmd)
        {
            p_adc->dma_restart_data[p_dma->dma_fact].block_size     = p_dma->block_size;
        }
        else
        {
            p_adc->dma_restart_data[p_dma->dma_fact].block_size     = 0;
        }

        dma_set.dma_drv->Control(DMA_CMD_START, NULL);

        switch (p_dma->dma_fact)
        {
            case ADC_READ_ADI:
            {
                /* ADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
                p_adc->reg->ADCSR_b.ADIE = 1;
#if (0 != ADC_PRV_USED_DTC_DRV)
                /* In case of DTC control, hold callback function. */
                p_adc->dma_info->dma_adi_cb_event   = p_dma->cb_event;
#endif
            }
            break;
            case ADC_READ_GBADI:
            {
                /* GBADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
                p_adc->reg->ADCSR_b.GBADIE = 1;
#if (0 != ADC_PRV_USED_DTC_DRV)
                /* In case of DTC control, hold callback function. */
                p_adc->dma_info->dma_gbadi_cb_event = p_dma->cb_event;
#endif
            }
            break;
            case ADC_READ_GCADI:
            {
                /* GCADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
                p_adc->reg->ADGCTRGR_b.GCADIE = 1;
#if (0 != ADC_PRV_USED_DTC_DRV)
                /* In case of DTC control, hold callback function. */
                p_adc->dma_info->dma_gcadi_cb_event = p_dma->cb_event;
#endif
            }
            break;
            case ADC_READ_WCMPM:
            {
#if (0 != ADC_PRV_USED_DTC_DRV)
                /* In case of DTC control, hold callback function. */
                p_adc->dma_info->dma_wcmpm_cb_event = p_dma->cb_event;
#endif
                __NOP();
            }
            break;
            case ADC_READ_WCMPUM:
            {
#if (0 != ADC_PRV_USED_DTC_DRV)
                /* In case of DTC control, hold callback function. */
                p_adc->dma_info->dma_wcmpum_cb_event = p_dma->cb_event;
#endif
                __NOP();
            }
            break;
            default:
            {
                __NOP();
            }
            break;
        }

        /* Since the DMA control setting is completed, an exclusive control flag is set. */
        gs_event_exclusion_flg |= dma_set.flg;
    }
    return (result);
}/* End of function adc_cmd_auto_read() */

/*************************************************************************//**
* @brief     Stop DMA transfer
* @param[in]         event  DMA activation factor information
* @param[in,out]     p_adc  Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER The argument value is invalid.
******************************************************************************/
/* Function Name : adc_cmd_auto_read_stop */
static e_adc_err_t adc_cmd_auto_read_stop(e_adc_dma_event_t const * const event, st_adc_resources_t * const p_adc)
{
    st_adc_dma_set dma_set = {0};
    e_adc_err_t result     = ADC_OK;
    
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    switch(*event)
    {
        case ADC_READ_ADI:
        case ADC_READ_GBADI:
        case ADC_READ_GCADI:
        case ADC_READ_WCMPM:
        case ADC_READ_WCMPUM:
        {
            __NOP();
        }
        break;
        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break; 
    }
#endif

    if (0 != (p_adc->adc_info->flags & ( 1 << (*event + ADC_FLAG_DMA_POS))))
    {
        result = adc_dma_drv_select(&dma_set, *event, p_adc);
        
        if (ADC_OK == result)
        {
            switch (*event)
            {
                case ADC_READ_ADI:
                {
                    /* ADIE - Scan End Interrupt Enable - Disables interrupt generation upon scan completion. */
                    p_adc->reg->ADCSR_b.ADIE      = 0;
#if (0 != ADC_PRV_USED_DTC_DRV)
                    adc_clear_irq_req(p_adc->adi_irq);
#endif
                    
                }
                break;
                case ADC_READ_GBADI:
                {
                    /* GBADIE - Scan End Interrupt Enable - Disables interrupt generation upon scan completion. */
                    p_adc->reg->ADCSR_b.GBADIE    = 0;
#if (0 != ADC_PRV_USED_DTC_DRV)
                    adc_clear_irq_req(p_adc->gbadi_irq);
#endif
                }
                break;
                case ADC_READ_GCADI:
                {
                    /* GCADIE - Scan End Interrupt Enable - Disables interrupt generation upon scan completion. */
                    p_adc->reg->ADGCTRGR_b.GCADIE = 0;
#if (0 != ADC_PRV_USED_DTC_DRV)
                    adc_clear_irq_req(p_adc->gcadi_irq);
#endif
                }
                break;
                case ADC_READ_WCMPM:
                {
#if (0 != ADC_PRV_USED_DTC_DRV)
                    adc_disable_irq(p_adc->wcmpm_irq);
                    adc_clear_irq_req(p_adc->wcmpm_irq);
#endif
                    __NOP();
                }
                break;
                case ADC_READ_WCMPUM:
                {
#if (0 != ADC_PRV_USED_DTC_DRV)
                    adc_disable_irq(p_adc->wcmpum_irq);
                    adc_clear_irq_req(p_adc->wcmpum_irq);
#endif
                    __NOP();
                }
                break;
                default:
                {
                    __NOP();
                }
            break;
            }

#if (0 != ADC_PRV_USED_DTC_DRV)
            if ((&Driver_DTC) == dma_set.dma_drv)
            {
                (void)dma_set.dma_drv->Control(DMA_CMD_ACT_SRC_DISABLE, &dma_set.dma_source);
            }
            else
#endif
            {
                (void)dma_set.dma_drv->Control(DMA_CMD_STOP, NULL);
            }
        }
    }
    return (result);
}/* End of function adc_cmd_auto_read_stop() */

/*************************************************************************//**
* @brief     Stop DMA restart
* @param[in]         event   DMA activation factor information
* @param[in,out]     p_adc  Set pointer to ADC resource
* @retval    ADC_OK    Successful
* @retval    ADC_ERROR_PARAMETER The argument value is invalid.
******************************************************************************/
/* Function Name : adc_cmd_auto_read_restart */
static e_adc_err_t adc_cmd_auto_read_restart(e_adc_dma_event_t const * const event, st_adc_resources_t * const p_adc)
{
    st_adc_dma_set dma_set     = {0};
    st_dma_refresh_data_t refresh;
    e_adc_err_t result         = ADC_OK;
    
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
    switch(*event)
    {
        case ADC_READ_ADI:
        case ADC_READ_GBADI:
        case ADC_READ_GCADI:
        {
            __NOP();
        }
        break;
        case ADC_READ_WCMPM:
        {
            __NOP();
#if (0 != ADC_PRV_USED_DTC_DRV)
             if (&Driver_DTC == p_adc->wcmpm_dma_drv)
             {
                 if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_adc->wcmpm_irq)
                 {
                    return ADC_ERROR_SYSTEM_SETTING;
                 }
             }
#endif
        }
        break;
        case ADC_READ_WCMPUM:
        {
            __NOP();
#if (0 != ADC_PRV_USED_DTC_DRV)
            if (&Driver_DTC == p_adc->wcmpum_dma_drv)
            {
                if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= p_adc->wcmpum_irq)
                {
                    return ADC_ERROR_SYSTEM_SETTING;
                }
            }

#endif
        }
        break;
        default:
        {
            return (ADC_ERROR_PARAMETER);
        }
        break; 
    }
#endif
    
    if ((uint32_t)NULL != (p_adc->dma_restart_data[*event].dest_addr))
    {   
        result = adc_dma_drv_select(&dma_set, *event, p_adc);
        
        if (ADC_OK == result)
        {
            refresh.act_src           = (IRQn_Type)dma_set.dma_source;
            refresh.chain_transfer_nr = 0;
            refresh.src_addr          = (uint32_t)NULL;
            refresh.dest_addr         = (uint32_t)p_adc->dma_restart_data[*event].dest_addr;
            refresh.transfer_count    = p_adc->dma_restart_data[*event].transfer_count;
            refresh.block_size        = p_adc->dma_restart_data[*event].block_size;
            refresh.auto_start        = true;
            
            switch (*event)
            {
                case ADC_READ_ADI:
                {
#if (0 != ADC_PRV_USED_DTC_DRV)
                    /* ADIE - Scan End Interrupt Disable - Enables interrupt generation upon scan completion. */
                    p_adc->reg->ADCSR_b.ADIE = 0;
                    adc_clear_irq_req(p_adc->adi_irq);
#endif
                    (void)dma_set.dma_drv->Control(DMA_CMD_REFRESH_DATA, &refresh);
                    /* ADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
                    p_adc->reg->ADCSR_b.ADIE = 1;
                }
                break;
                case ADC_READ_GBADI:
                {
#if (0 != ADC_PRV_USED_DTC_DRV)
                    /* ADIE - Scan End Interrupt Disable - Enables interrupt generation upon scan completion. */
                    p_adc->reg->ADCSR_b.GBADIE = 0;
                    adc_clear_irq_req(p_adc->gbadi_irq);
#endif
                    (void)dma_set.dma_drv->Control(DMA_CMD_REFRESH_DATA, &refresh);
                    /* GBADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
                    p_adc->reg->ADCSR_b.GBADIE = 1;
                }
                break;
                case ADC_READ_GCADI:
                {
#if (0 != ADC_PRV_USED_DTC_DRV)
                    /* ADIE - Scan End Interrupt Disable - Enables interrupt generation upon scan completion. */
                    p_adc->reg->ADGCTRGR_b.GCADIE = 0;
                    adc_clear_irq_req(p_adc->gcadi_irq);
#endif
                    (void)dma_set.dma_drv->Control(DMA_CMD_REFRESH_DATA, &refresh);
                    /* GCADIE - Scan End Interrupt Enable - Enables interrupt generation upon scan completion. */
                    p_adc->reg->ADGCTRGR_b.GCADIE = 1;
                }
                break;
                case ADC_READ_WCMPM:
                {
#if (0 != ADC_PRV_USED_DTC_DRV)
                     if (&Driver_DTC == p_adc->wcmpm_dma_drv)
                     {
                         (void)R_SYS_IrqEventLinkSet(p_adc->wcmpm_irq, 0x00, NULL);
                         adc_clear_irq_req(p_adc->wcmpm_irq);
                     }
#endif
                    (void)dma_set.dma_drv->Control(DMA_CMD_REFRESH_DATA, &refresh);
#if (0 != ADC_PRV_USED_DTC_DRV)
                     if (&Driver_DTC == p_adc->wcmpm_dma_drv)
                     {
                        if (NULL != p_adc->dma_info->dma_wcmpm_cb_event)
                        {
                            R_NVIC_EnableIRQ(p_adc->wcmpm_irq);
                        }
                        (void)R_SYS_IrqEventLinkSet(p_adc->wcmpm_irq, p_adc->wcmpm_iesr_val, p_adc->dma_wcmpm_callback);
                     }
#endif
                }
                break;
                case ADC_READ_WCMPUM:
                {
#if (0 != ADC_PRV_USED_DTC_DRV)
                     if (&Driver_DTC == p_adc->wcmpum_dma_drv)
                     {
                         (void)R_SYS_IrqEventLinkSet(p_adc->wcmpum_irq, 0x00, NULL);
                         adc_clear_irq_req(p_adc->wcmpum_irq);
                     }
#endif
                    (void)dma_set.dma_drv->Control(DMA_CMD_REFRESH_DATA, &refresh);
#if (0 != ADC_PRV_USED_DTC_DRV)
                     if (&Driver_DTC == p_adc->wcmpum_dma_drv)
                     {
                        if (NULL != p_adc->dma_info->dma_wcmpum_cb_event)
                        {
                            R_NVIC_EnableIRQ(p_adc->wcmpum_irq);
                        }
                        (void)R_SYS_IrqEventLinkSet(p_adc->wcmpum_irq, p_adc->wcmpum_iesr_val, p_adc->dma_wcmpum_callback);
                     }
#endif
                }
                break;
                default:
                {
                    __NOP();
                }
                break;
            }
        }
    }
    else 
    {
        result = ADC_ERROR;
    }
    return (result);
}/* End of function adc_cmd_auto_read_restart() */


/*************************************************************************//**
* @brief     Clear the restart information buffer and flag used in the DMA control.
******************************************************************************/
/* Function Name : adc_dma_ram_init */
static void adc_dma_ram_init(st_adc_resources_t * const p_adc)
{
    uint8_t i = 0;
    
    for (i = 0; i < 5; i++)
    {
        p_adc->dma_restart_data[i].dest_addr      = 0;
        p_adc->dma_restart_data[i].transfer_count = 0;
        p_adc->dma_restart_data[i].block_size     = 0;
    }
    
    gs_event_exclusion_flg = 0;

}/* End of function adc_dma_ram_init() */


/****************************************************************************************************************//**
* @brief    dma driver selected.
* @param[out]        setting      Pointer to DMA setting storage location
* @param[in]         event   DMA activation factor information
* @param[in,out]     p_adc  Set pointer to ADC resource
********************************************************************************************************************/
/* Function Name: adc_dma_drv_select */
static e_adc_err_t adc_dma_drv_select(st_adc_dma_set * const setting, e_adc_dma_event_t event, st_adc_resources_t * const p_adc)
{
    e_adc_err_t result = ADC_OK;
    
    /* Switch to the driver according to the activation factor. */
    switch (event)
    {
        case ADC_READ_ADI:
        {
            setting->dma_drv       = p_adc->adi_dma_drv;
            setting->dma_source    = p_adc->adi_dma_source;
            setting->flg           = ADC_PRV_ADI_EVENT_USED;
            setting->iesr          = p_adc->adi_iesr_val;
            setting->irq           = p_adc->adi_irq;
            setting->priority      = p_adc->adi_priority;
#if (S14AD_ADI_CONTROL == S14AD_USED_DTC)
            setting->transfer_info = &gs_adc_dtc_adi_info;
#endif
        }
        break;
        
        case ADC_READ_GBADI:
        {
            setting->dma_drv       = p_adc->gbadi_dma_drv;
            setting->dma_source    = p_adc->gbadi_dma_source;
            setting->flg           = ADC_PRV_GBADI_EVENT_USED;
            setting->iesr          = p_adc->gbadi_iesr_val;
            setting->irq           = p_adc->gbadi_irq;
            setting->priority      = p_adc->gbadi_priority;
#if (S14AD_GBADI_CONTROL == S14AD_USED_DTC)
            setting->transfer_info = &gs_adc_dtc_gbadi_info;
#endif
        }
        break;
        
        case ADC_READ_GCADI:
        {
            setting->dma_drv       = p_adc->gcadi_dma_drv;
            setting->dma_source    = p_adc->gcadi_dma_source;
            setting->flg           = ADC_PRV_GCADI_EVENT_USED;
            setting->iesr          = p_adc->gcadi_iesr_val;
            setting->irq           = p_adc->gcadi_irq;
            setting->priority      = p_adc->gcadi_priority;
#if (S14AD_GCADI_CONTROL == S14AD_USED_DTC)
            setting->transfer_info = &gs_adc_dtc_gcadi_info;
#endif
        }
        break;
        
        case ADC_READ_WCMPM:
        {
            setting->dma_drv       = p_adc->wcmpm_dma_drv;
            setting->dma_source    = p_adc->wcmpm_dma_source;
            setting->flg           = ADC_PRV_WCMPM_EVENT_USED;
            setting->iesr          = p_adc->wcmpm_iesr_val;
            setting->irq           = p_adc->wcmpm_irq;
            setting->priority      = p_adc->wcmpm_priority;
#if (S14AD_WCMPM_CONTROL == S14AD_USED_DTC)
            setting->transfer_info = &gs_adc_dtc_wcmpm_info;
#endif
        }
        break;
        
        case ADC_READ_WCMPUM:
        {
            setting->dma_drv       = p_adc->wcmpum_dma_drv;
            setting->dma_source    = p_adc->wcmpum_dma_source;
            setting->flg           = ADC_PRV_WCMPUM_EVENT_USED;
            setting->iesr          = p_adc->wcmpum_iesr_val;
            setting->irq           = p_adc->wcmpum_irq;
            setting->priority      = p_adc->wcmpum_priority;
#if (S14AD_WCMPUM_CONTROL == S14AD_USED_DTC)
            setting->transfer_info = &gs_adc_dtc_wcmpum_info;
#endif
        }
        break;
        
        default:
        {
            __NOP();
        }
        break;
    }

    /* Error when DMA driver is NULL */
    if (NULL == setting->dma_drv)
    {
        result = ADC_ERROR_SYSTEM_SETTING;
    }
    
    return (result);
}/* End of function adc_dma_drv_select() */

/****************************************************************************************************************//**
* @brief    dma configuration.
* @param[in]       setting  Pointer to DMA setting storage location
* @param[in]       p_dma    Set pointer to dma argment
* @param[in]       cmd      command
* @param[in,out]   p_adc    Set pointer to ADC resource
********************************************************************************************************************/
/* Function Name: adc_dma_config */
static e_adc_err_t adc_dma_config(st_adc_dma_read_info_t const * const p_dma, st_adc_dma_set const * const setting, e_adc_dma_cmd_t cmd, st_adc_resources_t * const p_adc)
{
    e_adc_err_t result = ADC_OK;
    st_dma_transfer_data_cfg_t dma_config;
    
#if (1 == ADC_CFG_PARAM_CHECKING_ENABLE)
#if (0 != ADC_PRV_USED_DTC_DRV)
    if (&Driver_DTC == setting->dma_drv)
    {
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= setting->irq)
        {
            return ADC_ERROR_SYSTEM_SETTING;
        }
     }
#endif
#endif
#if (0 != ADC_PRV_USED_DTC_DRV)
    system_int_cb_t callback;
#endif
    
    /* DMA config setting */
    if (ADC_READ_BLOCK == cmd)
    {
        dma_config.mode = DMA_MODE_BLOCK | DMA_SRC_INCR | DMA_DEST_INCR | DMA_REPEAT_BLOCK_SRC;
    }
    else
    {
        dma_config.mode = DMA_MODE_NORMAL | DMA_SRC_FIXED | DMA_DEST_INCR;
    }
    
    if (ADC_READ_COMPARE == cmd)
    {
        /* In the case of compare specification, the transfer size is decided by argument. */
        switch (p_dma->reg_size)
        {
            case ADC_READ_BYTE:
            {
                dma_config.mode |= DMA_SIZE_BYTE;
            }
            break;
            case ADC_READ_WORD:
            {
                dma_config.mode |= DMA_SIZE_WORD;
            }
            break;
            case ADC_READ_LONG:
            {
                dma_config.mode |= DMA_SIZE_LONG;
            }
            break;
            default:
            {
                __NOP();
            }
            break;
        }
    }
    else
    {
        /* For other than compare, transfer with word size. */
        dma_config.mode |= DMA_SIZE_WORD;
    }
    dma_config.offset               = 0;
    dma_config.src_extended_repeat  = 0;
    dma_config.dest_extended_repeat = 0;
    dma_config.src_addr             = p_dma->src_addr;
    dma_config.dest_addr            = p_dma->dest_addr;
    dma_config.transfer_count       = p_dma->transfer_count;
    dma_config.block_size           = p_dma->block_size;
    
#if (0 != ADC_PRV_USED_DTC_DRV)
    /* In the case of DTC, in the ADI / GBADI / GCADI event, set a wrapper function in the callback. */
    switch (p_dma->dma_fact)
    {
        case ADC_READ_ADI:
        {
            callback = p_adc->dma_adi_callback;
            p_adc->reg->ADCSR_b.ADIE = 0;
            adc_clear_irq_req(p_adc->adi_irq);
        }
        break;
        
        case ADC_READ_GBADI:
        {
            callback = p_adc->dma_gbadi_callback;
            p_adc->reg->ADCSR_b.GBADIE = 0;
            adc_clear_irq_req(p_adc->gbadi_irq);
        }
        break;
        
        case ADC_READ_GCADI:
        {
            callback = p_adc->dma_gcadi_callback;
            p_adc->reg->ADGCTRGR_b.GCADIE = 0;
            adc_clear_irq_req(p_adc->gcadi_irq);
        }
        break;
        
        case ADC_READ_WCMPM:
        {
            callback = p_adc->dma_wcmpm_callback;
            (void)R_SYS_IrqEventLinkSet(setting->irq, 0, NULL);
            adc_clear_irq_req(p_adc->wcmpm_irq);
        }
        break;
        
        case ADC_READ_WCMPUM:
        {
            callback = p_adc->dma_wcmpum_callback;
            (void)R_SYS_IrqEventLinkSet(setting->irq, 0, NULL);
            adc_clear_irq_req(p_adc->wcmpum_irq);
        }
        break;
        
        default:
        {
            callback = NULL;
        }
        break;
    }
#endif
    
#if (0 != ADC_PRV_USED_DTC_DRV)
    dma_config.p_transfer_data      = setting->transfer_info;
#endif

    /* DMA Open not executed yet? */
    if (0 == (p_adc->adc_info->flags & ( 1 << (p_dma->dma_fact + ADC_FLAG_DMA_POS))))
    {
        result = (e_adc_err_t)setting->dma_drv->Open();
    }
    
    if(ADC_OK == result)
    {
        result = (e_adc_err_t)setting->dma_drv->Create(setting->dma_source, &dma_config);
        /* Set the DMA Open completed flag */
        p_adc->adc_info->flags |= ( 1 << (p_dma->dma_fact + ADC_FLAG_DMA_POS));
    }

    if (ADC_OK == result)
    {
#if (0 != ADC_PRV_USED_DTC_DRV)
        if ((&Driver_DTC) == setting->dma_drv)
        {
            /* In case of DTC control, set NVIC of cause. */
            if (0 == R_SYS_IrqEventLinkSet(setting->irq, setting->iesr, callback))
            {
                R_NVIC_SetPriority(setting->irq, setting->priority);
                if (R_NVIC_GetPriority (setting->irq) != setting->priority)
                {
                    result = ADC_ERROR_SYSTEM_SETTING;
                }
            }
            else
            {
                result = ADC_ERROR_SYSTEM_SETTING;
            }
            
            if ((ADC_OK == result) && (NULL != p_dma->cb_event))
            {
                R_NVIC_EnableIRQ(setting->irq);
            }
        }
#endif
#if (0 != ADC_PRV_USED_DMAC_DRV)
        if (((&Driver_DMAC0) == setting->dma_drv) ||
            ((&Driver_DMAC1) == setting->dma_drv) ||
            ((&Driver_DMAC2) == setting->dma_drv) ||
            ((&Driver_DMAC3) == setting->dma_drv))
        {
            /* DMAC Control */
            if (NULL != p_dma->cb_event)
            {
                result = (e_adc_err_t)setting->dma_drv->InterruptEnable (1, p_dma->cb_event);
            }
        }
#endif
    }
    return (result);

}/* End of function dma_config() */

#endif
/*************************************************************************//**
* @brief     Interrupt handler for normal/Group A/double trigger scan complete.
******************************************************************************/
/* Function Name : adc_s14adi0_isr */
static void adc_s14adi0_isr(st_adc_resources_t * const p_adc) // @suppress("Function definition ordering")
{
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->adc_info->cb_event)
    {
        p_adc->adc_info->cb_event(ADC_EVT_SCAN_COMPLETE);
    }
}/* End of function adc_s14adi0_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for Group B scan complete.
******************************************************************************/
/* Function Name : adc_gbadi_isr */
static void adc_gbadi_isr(st_adc_resources_t * const p_adc)
{
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->adc_info->cb_event)
    {
        p_adc->adc_info->cb_event(ADC_EVT_SCAN_COMPLETE_GROUPB);
    }
}/* End of function adc_gbadi_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for Group C scan complete.
******************************************************************************/
/* Function Name : adc_gcadi_isr */
static void adc_gcadi_isr(st_adc_resources_t * const p_adc)
{
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->adc_info->cb_event)
    {
        p_adc->adc_info->cb_event(ADC_EVT_SCAN_COMPLETE_GROUPC);
    }
}/* End of function adc_gcadi_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for WCMPM.
******************************************************************************/
/* Function Name : adc_wcmpm_isr */
static void adc_wcmpm_isr(st_adc_resources_t * const p_adc)
{
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->adc_info->cb_event)
    {
        p_adc->adc_info->cb_event(ADC_EVT_WINDOW_CMP_MATCH);
    }
}/* End of function adc_wcmpm_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for Group WCMPUM.
******************************************************************************/
/* Function Name : adc_wcmpum_isr */
static void adc_wcmpum_isr(st_adc_resources_t * const p_adc)
{
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->adc_info->cb_event)
    {
        p_adc->adc_info->cb_event(ADC_EVT_WINDOW_CMP_UNMATCH);
    }
}/* End of function adc_wcmpum_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for CMPAI.
******************************************************************************/
/* Function Name : adc_cmpai_isr */
static void adc_cmpai_isr(st_adc_resources_t * const p_adc)
{
    volatile uint16_t dummy;

    /* presence of callback function verified in Open() */
    if (NULL != p_adc->adc_info->cb_event)
    {
        p_adc->adc_info->cb_event(ADC_EVT_CONDITION_MET);
        adc_clear_irq_req(p_adc->cmpai_irq);
    }
    else
    {
        /* If the callback function is not registered, clear the interrupt factor. */
        dummy = p_adc->reg->ADCMPSR0;
        dummy = p_adc->reg->ADCMPSR1;
        p_adc->reg->ADCMPSR0 = 0x0000;
        p_adc->reg->ADCMPSR1 = 0x0000;
        adc_clear_irq_req(p_adc->cmpai_irq);
    }
}/* End of function adc_cmpai_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for CMPBI.
******************************************************************************/
/* Function Name : adc_cmpbi_isr */
static void adc_cmpbi_isr(st_adc_resources_t * const p_adc)
{
    volatile uint8_t dummy;

    /* presence of callback function verified in Open() */
    if (NULL != p_adc->adc_info->cb_event)
    {
        p_adc->adc_info->cb_event(ADC_EVT_CONDITION_METB);
        adc_clear_irq_req(p_adc->cmpbi_irq);
    }
    else
    {
        /* If the callback function is not registered, clear the interrupt factor. */
        dummy = p_adc->reg->ADCMPBSR;
        p_adc->reg->ADCMPBSR = 0x00;
        adc_clear_irq_req(p_adc->cmpbi_irq);
    }
}/* End of function adc_cmpbi_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for normal/Group A/double trigger scan complete.
******************************************************************************/
/* Function Name : adc_adi_isr */
static void adc_dma_adi_isr(st_adc_resources_t * const p_adc) // @suppress("Function definition ordering")
{
    /* Disable interrupts and clear pending to prevent interrupts at illegal timing. */
    p_adc->reg->ADCSR_b.ADIE = 0;
    adc_clear_irq_req(p_adc->adi_irq);

    /* presence of callback function verified in Open() */
    if (NULL != p_adc->dma_info->dma_adi_cb_event)
    {
        p_adc->dma_info->dma_adi_cb_event();
    }
}/* End of function adc_dma_adi_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for normal/Group A/double trigger scan complete.
******************************************************************************/
/* Function Name : adc_dma_gbadi_isr */
static void adc_dma_gbadi_isr(st_adc_resources_t * const p_adc) // @suppress("Function definition ordering")
{
    /* Disable interrupts and clear pending to prevent interrupts at illegal timing. */
    p_adc->reg->ADCSR_b.GBADIE    = 0;
    adc_clear_irq_req(p_adc->gbadi_irq);
    
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->dma_info->dma_gbadi_cb_event)
    {
        p_adc->dma_info->dma_gbadi_cb_event();
    }
}/* End of function adc_dma_gbadi_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for normal/Group A/double trigger scan complete.
******************************************************************************/
/* Function Name : adc_dma_gcadi_isr */
static void adc_dma_gcadi_isr(st_adc_resources_t * const p_adc) // @suppress("Function definition ordering")
{
    /* Disable interrupts and clear pending to prevent interrupts at illegal timing. */
    p_adc->reg->ADGCTRGR_b.GCADIE = 0;
    adc_clear_irq_req(p_adc->gcadi_irq);
 
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->dma_info->dma_gcadi_cb_event)
    {
        p_adc->dma_info->dma_gcadi_cb_event();
    }
}/* End of function adc_dma_gcadi_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for normal/Group A/double trigger scan complete.
******************************************************************************/
/* Function Name : adc_dma_wcmpm_isr */
static void adc_dma_wcmpm_isr(st_adc_resources_t * const p_adc) // @suppress("Function definition ordering")
{
    /* Disable interrupts and clear pending to prevent interrupts at illegal timing. */
    adc_disable_irq(p_adc->wcmpm_irq);
    
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->dma_info->dma_wcmpm_cb_event)
    {
        p_adc->dma_info->dma_wcmpm_cb_event();
    }
}/* End of function adc_dma_wcmpm_isr() */

/*************************************************************************//**
* @brief     Interrupt handler for normal/Group A/double trigger scan complete.
******************************************************************************/
/* Function Name : adc_dma_wcmpum_isr */
static void adc_dma_wcmpum_isr(st_adc_resources_t * const p_adc) // @suppress("Function definition ordering")
{
    /* Disable interrupts and clear pending to prevent interrupts at illegal timing. */
    adc_disable_irq(p_adc->wcmpum_irq);
 
    /* presence of callback function verified in Open() */
    if (NULL != p_adc->dma_info->dma_wcmpum_cb_event)
    {
        p_adc->dma_info->dma_wcmpum_cb_event();
    }
}/* End of function adc_dma_wcmpum_isr() */

/****************************************************************************************************************//**
* @brief The implementation of HAL-Driver S14AD Get Version API for the S14AD peripheral on RE Family.
*
* @retval version number
 *******************************************************************************************************************/
/* Function Name : S14AD0_GetVersion */
static int32_t S14AD0_GetVersion(void) // @suppress("Non-API function naming")
{
    return (R_ADC_GetVersion());
}/* End of function S14AD0_GetVersion() */

/****************************************************************************************************************//**
* @brief S14AD initialize method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance S14AD0_Open().
* @param[in] mode        Operational mode 
* @param[in] default_sampling_rate
*                        Set default value of sampling state. To change the sampling state individually, 
*                        change with the control function.
* @param[in] p_callback  Optional pointer to function called from interrupt when a scan completes
* @param[in] p_adc       Pointer to ADC resource
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : S14AD0_Open */
static e_adc_err_t S14AD0_Open(uint32_t mode, uint8_t default_sampling_rate, adc_cd_event_t const p_callback) // @suppress("Non-API function naming")
{
    return (R_ADC_Open(mode, default_sampling_rate, p_callback, &gs_adc0_resources));
}/* End of function S14AD0_Open() */

/****************************************************************************************************************//**
* @brief S14AD ScanSet method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance S14AD0_ScanSet().
* @param[in]     group    Configration target group
* @param[in]     pins     Set the A/D conversion target pin
* @param[in]     triger   Set to starting A/D conversion trigger
* @param[in,out] p_adc Pointer to ADC resource
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : S14AD0_ScanSet */
static e_adc_err_t S14AD0_ScanSet(e_adc_group_t group, st_adc_pins_t pins , e_adc_triger_t triger) // @suppress("Non-API function naming")
{
    return (R_ADC_ScanSet(group, pins, triger, &gs_adc0_resources));
}/* End of function S14AD0_ScanSet() */


/****************************************************************************************************************//**
* @brief S14AD S14AD0_Start method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance S14AD0_Start().
* @param[in,out]     p_adc     Pointer to ADC resource
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : S14AD0_Start */
static e_adc_err_t S14AD0_Start(void) // @suppress("Non-API function naming")
{
    return (R_ADC_Start(&gs_adc0_resources));
}/* End of function S14AD0_Start() */

/****************************************************************************************************************//**
* @brief S14AD S14AD0_Stop method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance S14AD0_Stop().
* @param[in,out]     p_adc     Pointer to ADC resource
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : S14AD0_Stop */
static e_adc_err_t S14AD0_Stop(void) // @suppress("Non-API function naming")
{
    return (R_ADC_Stop(&gs_adc0_resources));
}/* End of function S14AD0_Stop() */

/****************************************************************************************************************//**
* @brief S14AD0_Control method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance S14AD0_Control.
* @param[in]       cmd ADC Operation command
* @param[in,out]  p_args Pointer to Argument by command
* @param[in,out]  p_adc   Pointer to ADC resource
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : S14AD0_Control */
static e_adc_err_t S14AD0_Control(e_adc_cmd_t const cmd, void const * const p_args) // @suppress("Non-API function naming")
{
    return (R_ADC_Control(cmd, p_args, &gs_adc0_resources));
}/* End of function S14AD0_Control() */

/****************************************************************************************************************//**
* @brief S14AD0_Read method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance S14AD0_Read.
* @param[in]         sel_ch   Set reading channel
* @param[in,out]    p_data   Pointer to reading data storage
* @param[in,out]    p_adc    Pointer to ADC resource
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : S14AD0_Read */
static e_adc_err_t S14AD0_Read(e_adc_ssel_ch_t sel_ch, uint16_t * const p_data) // @suppress("Non-API function naming")
{
    return (R_ADC_Read(sel_ch, p_data, &gs_adc0_resources));
}/* End of function S14AD0_Read() */

/****************************************************************************************************************//**
* @brief S14AD0_Close method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance S14AD0_Close.
* @param[in,out]     p_adc     Set pointer to ADC resource
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : S14AD0_Close */
static e_adc_err_t S14AD0_Close(void) // @suppress("Non-API function naming")
{
    return (R_ADC_Close(&gs_adc0_resources));
}/* End of function S14AD0_Close() */

/****************************************************************************************************************//**
* @brief adc_adi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_adi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_adi_interrupt */
static void adc_adi_interrupt(void) // @suppress("ISR declaration")
{
    adc_s14adi0_isr(&gs_adc0_resources);
}/* End of function adc_adi_interrupt() */

/****************************************************************************************************************//**
* @brief adc_gbadi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_gbadi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_gbadi_interrupt */
static void adc_gbadi_interrupt(void) // @suppress("ISR declaration")
{
    adc_gbadi_isr(&gs_adc0_resources);
}/* End of function adc_gbadi_interrupt() */

/****************************************************************************************************************//**
* @brief adc_gcadi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_gcadi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_gcadi_interrupt */
static void adc_gcadi_interrupt(void) // @suppress("ISR declaration")
{
    adc_gcadi_isr(&gs_adc0_resources);
}/* End of function adc_gcadi_interrupt() */

/****************************************************************************************************************//**
* @brief adc_cmpai_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_cmpai_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_cmpai_interrupt */
static void adc_cmpai_interrupt(void) // @suppress("ISR declaration")
{
    adc_cmpai_isr(&gs_adc0_resources);
}/* End of function adc_cmpai_interrupt() */

/****************************************************************************************************************//**
* @brief adc_cmpbi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_cmpbi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_cmpbi_interrupt */
static void adc_cmpbi_interrupt(void) // @suppress("ISR declaration")
{
    adc_cmpbi_isr(&gs_adc0_resources);
}/* End of function adc_cmpbi_interrupt() */

/****************************************************************************************************************//**
* @brief adc_cmpbi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_wcmpm_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_wcmpm_interrupt */
static void adc_wcmpm_interrupt(void) // @suppress("ISR declaration")
{
    adc_wcmpm_isr(&gs_adc0_resources);
}/* End of function adc_wcmpm_interrupt() */

/****************************************************************************************************************//**
* @brief adc_cmpai_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_wcmpum_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_wcmpum_interrupt */
static void adc_wcmpum_interrupt(void) // @suppress("ISR declaration")
{
    adc_wcmpum_isr(&gs_adc0_resources);
}/* End of function adc_wcmpum_interrupt() */

/****************************************************************************************************************//**
* @brief adc_dma_adi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_adi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_dma_adi_interrupt */
static void adc_dma_adi_interrupt(void) // @suppress("ISR declaration")
{
    adc_dma_adi_isr(&gs_adc0_resources);
}/* End of function adc_dma_adi_interrupt() */

/****************************************************************************************************************//**
* @brief adc_dma_gbadi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_gbadi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_dma_gbadi_interrupt */
static void adc_dma_gbadi_interrupt(void) // @suppress("ISR declaration")
{
    adc_dma_gbadi_isr(&gs_adc0_resources);
}/* End of function adc_dma_gbadi_interrupt() */

/****************************************************************************************************************//**
* @brief adc_dma_gcadi_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_gcadi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_dma_gcadi_interrupt */
static void adc_dma_gcadi_interrupt(void) // @suppress("ISR declaration")
{
    adc_dma_gcadi_isr(&gs_adc0_resources);
}/* End of function adc_dma_gcadi_interrupt() */


/****************************************************************************************************************//**
* @brief adc_dma_wcmpm_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_gbadi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_dma_wcmpm_interrupt */
static void adc_dma_wcmpm_interrupt(void) // @suppress("ISR declaration")
{
    adc_dma_wcmpm_isr(&gs_adc0_resources);
}/* End of function adc_dma_wcmpm_interrupt() */

/****************************************************************************************************************//**
* @brief adc_dma_wcmpum_interrupt method for user applications to open the S14AD unit0. User applications should call
* this method through the Driver Instance adc_gcadi_interrupt.
* @retval See @ref e_adc_err_t
 *******************************************************************************************************************/
/* Function Name : adc_dma_wcmpum_interrupt */
static void adc_dma_wcmpum_interrupt(void) // @suppress("ISR declaration")
{
    adc_dma_wcmpum_isr(&gs_adc0_resources);
}/* End of function adc_dma_wcmpum_interrupt() */


DRIVER_S14AD Driver_S14AD  DATA_LOCATION_PRV_DRIVER_ADC =
{
    S14AD0_GetVersion,
    S14AD0_Open,
    S14AD0_ScanSet,
    S14AD0_Start,
    S14AD0_Stop,
    S14AD0_Control,
    S14AD0_Read,
    S14AD0_Close
};

/* End of file (r_adc.c) */

/******************************************************************************************************************//**
 * @} (end defgroup grp_hal_drv_impl_adc)
 *********************************************************************************************************************/
