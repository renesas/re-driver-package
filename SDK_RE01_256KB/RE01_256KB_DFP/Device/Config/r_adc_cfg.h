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
* Copyright (C) 2018-2020 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_adc_cfg.h
* Version      : 1.00
* Description  : HAL Driver for ADC
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
***********************************************************************************************************************/
/******************************************************************************************************************//**
 @addtogroup grp_device_hal_adc
 @{
 *********************************************************************************************************************/

#ifndef R_ADC_CFG_H
#define R_ADC_CFG_H

#ifdef  __cplusplus
extern "C"
{
#endif

/******************************************************************************************************************//**
 * @name S14AD_XXX_CONTROL
 *       Select whether to use interrupts or DMAC (DTC) for reading control.@n@n
 *       S14AD_USED_INTERRUPT: Interrupt is used for reading control.@n
 *       S14AD_USED_DMACn    : DMACn is used for reading control.@n
 *       S14AD_USED_DTC      : DTC is used for reading control.@n
 *          [Example]@n
 *          Example of definition when interrupt is used for reading of ADI event
 *          and DMAC is used for reading of GBADI event
 *          @code
 *          ...
 *          #define S14AD_ADI_CONTROL       S14AD_USED_INTERRUPT
 *          #define S14AD_GBADI_CONTROL     S14AD_USED_DMAC0
 *          #define S14AD_GCADI_CONTROL     S14AD_USED_INTERRUPT
 *          #define S14AD_WCMPM_CONTROL     S14AD_USED_INTERRUPT
 *          #define S14AD_WCMPUM_CONTROL    S14AD_USED_INTERRUPT
 *
 *          ...
 *          @endcode
 *********************************************************************************************************************/
/* @{ */
#define S14AD_USED_INTERRUPT  (0)                        ///< Use Interrupt for transmission and reception control of S14AD
#define S14AD_USED_DMAC0      (1<<0)                     ///< Use DMAC0 for transmission and reception control of S14AD
#define S14AD_USED_DMAC1      (1<<1)                     ///< Use DMAC1 for transmission and reception control of S14AD
#define S14AD_USED_DMAC2      (1<<2)                     ///< Use DMAC2 for transmission and reception control of S14AD
#define S14AD_USED_DMAC3      (1<<3)                     ///< Use DMAC3 for transmission and reception control of S14AD
#define S14AD_USED_DTC        (1<<15)                    ///< Use DTC for transmission and reception control of S14AD

#define S14AD_ADI_CONTROL    S14AD_USED_INTERRUPT   ///< Read control of AD conversion value by group A scan end event
#define S14AD_GBADI_CONTROL  S14AD_USED_INTERRUPT   ///< Read control of AD conversion value by group B scan end event
#define S14AD_GCADI_CONTROL  S14AD_USED_INTERRUPT   ///< Read control of AD conversion value by group C scan end event
#define S14AD_WCMPM_CONTROL  S14AD_USED_INTERRUPT   ///< Read control of AD conversion value by WCMPM
#define S14AD_WCMPUM_CONTROL S14AD_USED_INTERRUPT   ///< Read control of AD conversion value by WCMPUM

/******************************************************************************************************************//**
 * @name R_ADC_INTERRUPT_PRIORITY
 *       Definition of ADC interrrupt priority
 *       Please set to 0 to 3.@n
 *********************************************************************************************************************/
/* @{ */
#define S14AD_ADI_PRIORITY     (3)                          ///< ADI priority value(set to 0 to 3, 0 is highest priority.)
#define S14AD_GBADI_PRIORITY   (3)                          ///< GBADI priority value(set to 0 to 3, 0 is highest priority.)
#define S14AD_GCADI_PRIORITY   (3)                          ///< GCADI priority value(set to 0 to 3, 0 is highest priority.)
#define S14AD_CMPAI_PRIORITY   (3)                          ///< CMPAI priority value(set to 0 to 3, 0 is highest priority.)
#define S14AD_CMPBI_PRIORITY   (3)                          ///< CMPBI priority value(set to 0 to 3, 0 is highest priority.)
#define S14AD_WCMPM_PRIORITY   (3)                          ///< WCMPM priority value(set to 0 to 3, 0 is highest priority.)
#define S14AD_WCMPUM_PRIORITY  (3)                         ///< WCMPUM priority value(set to 0 to 3, 0 is highest priority.)
/* @} */
/******************************************************************************************************************//**
 * @brief Parameter check enable@n
 *        0 = Disable@n
 *        1 = Enable (Default)@n
 *********************************************************************************************************************/
 
#define ADC_CFG_PARAM_CHECKING_ENABLE    (1)


/********************************************************//**
 * @name ADC_SNOOZE_USE
 * @brief Following options are used in Snooze mode that it preforms AD.@n
 * When it set as follows options to "1", this driver doesn't set
 * NVIC for ADC CMPAI or CMPBI interrupts.@n
 * When in snooze mode, set "1" when using A/D Compare Function without
 * using CMPAI or CMPBI interrupts.@n
 * Set "0" when using CMPAI or CMPBI interrupts regardless of 
 * the low power consumption mode.@n
 * When checking A/D conversion status by S/W polling, set "1".
 * Please also set "SYSTEM_IRQ_EVENT_NUMBER_NOT_USED" 
 * as corresponding definition values of A/D interrupts in r_system_cfg.h. @n
 * 
 *        0 = Not use ADC in Snooze mode (default) @n
 *        1 = Use ADC in Snooze mode@n
 ********************************************************/
 /* @{ */
#define ADC_CMPAI_SNOOZE_USE  (0) 
#define ADC_CMPBI_SNOOZE_USE  (0) 
/* @} */

/******************************************************************************************************************//**
 * @name R_ADC_API_LOCATION_CONFIG
 *       Definition of R_ADC API location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define S14AD_CFG_R_ADC_OPEN            (SYSTEM_SECTION_CODE)     ///<R_ADC_Open() section
#define S14AD_CFG_R_ADC_SCAN_SET        (SYSTEM_SECTION_CODE)     ///<R_ADC_ScanSet() section
#define S14AD_CFG_R_ADC_START           (SYSTEM_SECTION_CODE)     ///<R_ADC_Start() section
#define S14AD_CFG_R_ADC_STOP            (SYSTEM_SECTION_CODE)     ///<R_ADC_Stop() section
#define S14AD_CFG_R_ADC_CONTROL         (SYSTEM_SECTION_CODE)     ///<R_ADC_Control() section
#define S14AD_CFG_R_ADC_READ            (SYSTEM_SECTION_CODE)     ///<R_ADC_Read() section
#define S14AD_CFG_R_ADC_CLOSE           (SYSTEM_SECTION_CODE)     ///<R_ADC_Close() section
#define S14AD_CFG_R_ADC_GET_VERSION     (SYSTEM_SECTION_CODE)     ///<R_ADC_GetVersion() section
#define S14AD_CFG_ADC_S14ADI0_ISR       (SYSTEM_SECTION_RAM_FUNC) ///<adc_s14adi0_isr() section
#define S14AD_CFG_ADC_GBADI_ISR         (SYSTEM_SECTION_RAM_FUNC) ///<adc_gbadi_isr() section
#define S14AD_CFG_ADC_GCADI_ISR         (SYSTEM_SECTION_RAM_FUNC) ///<adc_gcadi_isr() section
#define S14AD_CFG_ADC_CMPAI_ISR         (SYSTEM_SECTION_RAM_FUNC) ///<adc_cmpai_isr() section
#define S14AD_CFG_ADC_CMPBI_ISR         (SYSTEM_SECTION_RAM_FUNC) ///<adc_cmpbi_isr() section
#define S14AD_CFG_ADC_WCMPM_ISR         (SYSTEM_SECTION_RAM_FUNC) ///<adc_wcmpm_isr() section
#define S14AD_CFG_ADC_WCMPUM_ISR        (SYSTEM_SECTION_RAM_FUNC) ///<adc_wcmpum_isr() section

/* @} */

/* Definition of using DMAC / DTC */
#define ADC_PRV_USED_DMAC_DRV (0x00FF & (S14AD_ADI_CONTROL | S14AD_GBADI_CONTROL | S14AD_GCADI_CONTROL | \
                                S14AD_WCMPM_CONTROL | S14AD_WCMPUM_CONTROL))
#define ADC_PRV_USED_DTC_DRV  (S14AD_USED_DTC & (S14AD_ADI_CONTROL | S14AD_GBADI_CONTROL | S14AD_GCADI_CONTROL | \
                                S14AD_WCMPM_CONTROL | S14AD_WCMPUM_CONTROL))
#define ADC_PRV_USED_DTC_DMAC_DRV (ADC_PRV_USED_DMAC_DRV | ADC_PRV_USED_DTC_DRV)

#define ADC_PRV_WCMP_USED_DMAC_DRV (0x00FF & (S14AD_WCMPM_CONTROL | S14AD_WCMPUM_CONTROL))
#define ADC_PRV_WCMP_USED_DTC_DRV  (S14AD_USED_DTC & (S14AD_WCMPM_CONTROL | S14AD_WCMPUM_CONTROL))
#define ADC_PRV_WCMP_USED_DTC_DMAC_DRV (ADC_PRV_WCMP_USED_DMAC_DRV | ADC_PRV_WCMP_USED_DTC_DRV)

#ifdef  __cplusplus
}
#endif
/*******************************************************************************************************************//**
 * @} (end ingroup grp_hal_drv_adc)
 **********************************************************************************************************************/

#endif /* R_ADC_CFG_H */
/* End of file (r_adc_cfg.h) */

