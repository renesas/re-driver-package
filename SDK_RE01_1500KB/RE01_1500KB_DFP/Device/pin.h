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
* File Name    : Pin.h
* Version      : 1.30
* Description  : Pin Setting
**********************************************************************************************************************/

/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 07.08.2018 0.50     First Release
*         : 20.12.2018 0.60     Update Comments
*                               Fix Pin setting for WS2 update.
*                               Adapted to GSCE coding rules.
*         : 13.03.2019 0.70     Delete EOFR setting
*         : 17.07.2019 1.00     IO register header name change(E017.h -> RE01_1500KB.h)
*                               Update Comments
*         : 30.09.2019 1.01     Fixed error in function name(GTP -> GPT)
*         : 11.06.2020 1.20     Removed the inclusion of "RE01_1500KB.h".     
*         : 05.11.2020 1.30     Remove unnecessary R_CCC_Pinset and R_CCC_Pinclr
*                               Add R_ICU_Pinset and R_ICU_Pinclr
*                               Split R_GPT_COM_Pinset/clr function
*                               into R_GPT_COMA_Pinset/clr and R_GPT_COMB_Pinset/clr
**********************************************************************************************************************/
/******************************************************************************************************************//**
 @addtogroup grp_pin_func
 @brief Perform Pin setting used for peripheral functions on RE Family.@n
        Some Pin setting functions allow the user to select the mode and pin to use.
 @section sec_driver_pin_setting_summary PinSetting function
          Make Pin settings to be used with the API.@n
          To change the terminal to be used, refer to the relevant API's Pin setting function.
          Several pin names have added _A, _B, and _C suffixes.@n 
          When assigning the RIIC and SPI functions, select the functional pins with the same suffix. 
          The other pins can be selected regardless of the suffix.@n 
          Assigning the same function to two or more pins simultaneously is prohibited.
 @code
 ...

void R_ICU_Pinset_CH0(void)
{
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    // IRQ0_A_DS : P411
    PFS->P411PFS_b.ASEL = 0U;
    PFS->P411PFS_b.PSEL = 0U;
    PFS->P411PFS_b.PDR  = 0U;
    PFS->P411PFS_b.PMR  = 0U;
    PFS->P411PFS_b.ISEL = 1U;

//     IRQ0_A_DS : P106           <- Unused terminals are commented out. When _A, _B, _C terminals are prepared,
                                     only enable the terminals to be used.
//    PFS->P106PFS_b.ASEL = 0U;
//    PFS->P106PFS_b.PSEL = 0U;
//    PFS->P106PFS_b.PDR  = 0U;
//    PFS->P106PFS_b.PMR  = 0U;
//    PFS->P106PFS_b.ISEL = 1U;

    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}


 ...
 @endcode


 @{
 *********************************************************************************************************************/
/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#ifndef PIN_H
#define PIN_H
#include "r_system_api.h"

/*****************************************************************************
  Macro definitions
 *****************************************************************************/
 
/*****************************************************************************
 Global Typedef definitions
 *****************************************************************************/
typedef void (*r_pinset_t) (void);      /*!< Pointer to r_pinset. */
typedef void (*r_pinclr_t) (void);      /*!< Pointer to r_pinclr. */

/*****************************************************************************
 Exported global functions (to be accessed by other files)
******************************************************************************/
void R_ICU_Pinset(void);
void R_ICU_Pinclr(void);
void R_ICU_Pinset_NMI(void);
void R_ICU_Pinclr_NMI(void);
void R_ICU_Pinset_CH0(void);
void R_ICU_Pinclr_CH0(void);
void R_ICU_Pinset_CH1(void);
void R_ICU_Pinclr_CH1(void);
void R_ICU_Pinset_CH2(void);
void R_ICU_Pinclr_CH2(void);
void R_ICU_Pinset_CH3(void);
void R_ICU_Pinclr_CH3(void);
void R_ICU_Pinset_CH4(void);
void R_ICU_Pinclr_CH4(void);
void R_ICU_Pinset_CH5(void);
void R_ICU_Pinclr_CH5(void);
void R_ICU_Pinset_CH6(void);
void R_ICU_Pinclr_CH6(void);
void R_ICU_Pinset_CH7(void);
void R_ICU_Pinclr_CH7(void);
void R_ICU_Pinset_CH8(void);
void R_ICU_Pinclr_CH8(void);
void R_ICU_Pinset_CH9(void);
void R_ICU_Pinclr_CH9(void);
void R_GPT_Pinset_CH0(void);
void R_GPT_Pinclr_CH0(void);
void R_GPT_Pinset_CH1(void);
void R_GPT_Pinclr_CH1(void);
void R_GPT_Pinset_CH2(void);
void R_GPT_Pinclr_CH2(void);
void R_GPT_Pinset_CH3(void);
void R_GPT_Pinclr_CH3(void);
void R_GPT_Pinset_CH4(void);
void R_GPT_Pinclr_CH4(void);
void R_GPT_Pinset_CH5(void);
void R_GPT_Pinclr_CH5(void);
void R_GPT_COMA_Pinset(void);
void R_GPT_COMA_Pinclr(void);
void R_GPT_COMB_Pinset(void);
void R_GPT_COMB_Pinclr(void);
void R_GPT_OPS_Pinset(void);
void R_GPT_OPS_Pinclr(void);
void R_AGT_Pinset_CH0(void);
void R_AGT_Pinclr_CH0(void);
void R_AGT_Pinset_CH1(void);
void R_AGT_Pinclr_CH1(void);
void R_SCI_Pinset_CH0(void);
void R_SCI_Pinclr_CH0(void);
void R_SCI_Pinset_CH1(void);
void R_SCI_Pinclr_CH1(void);
void R_SCI_Pinset_CH2(void);
void R_SCI_Pinclr_CH2(void);
void R_SCI_Pinset_CH3(void);
void R_SCI_Pinclr_CH3(void);
void R_SCI_Pinset_CH4(void);
void R_SCI_Pinclr_CH4(void);
void R_SCI_Pinset_CH5(void);
void R_SCI_Pinclr_CH5(void);
void R_SCI_Pinset_CH9(void);
void R_SCI_Pinclr_CH9(void);
void R_RSPI_Pinset_CH0(void);
void R_RSPI_Pinclr_CH0(void);
void R_RSPI_Pinset_CH1(void);
void R_RSPI_Pinclr_CH1(void);
void R_QSPI_Pinset(void); // @suppress("API header file naming")
void R_QSPI_Pinclr(void); // @suppress("API header file naming")
void R_RIIC_Pinset_CH0(void);
void R_RIIC_Pinclr_CH0(void);
void R_RIIC_Pinset_CH1(void);
void R_RIIC_Pinclr_CH1(void);
void R_CAC_Pinset(void); // @suppress("API header file naming")
void R_CAC_Pinclr(void);// @suppress("API header file naming")
void R_S14AD_Pinset(void);// @suppress("API header file naming")
void R_S14AD_Pinclr(void);// @suppress("API header file naming")
void R_R12DA_Pinset(void);// @suppress("API header file naming")
void R_R12DA_Pinclr(void);// @suppress("API header file naming")
void R_ACMP_Pinset(void);// @suppress("API header file naming")
void R_ACMP_Pinclr(void);// @suppress("API header file naming")
void R_MLCD_Pinset(void);// @suppress("API header file naming")
void R_MLCD_Pinclr(void);// @suppress("API header file naming")
void R_KINT_Pinset(void);// @suppress("API header file naming")
void R_KINT_Pinclr(void);// @suppress("API header file naming")
void R_USB_Pinset(void);// @suppress("API header file naming")
void R_USB_Pinclr(void);// @suppress("API header file naming")
void R_RTC_Pinset(void);// @suppress("API header file naming")
void R_RTC_Pinclr(void);// @suppress("API header file naming")
void R_LPG_Pinset(void);// @suppress("API header file naming")
void R_LPG_Pinclr(void);// @suppress("API header file naming")
void R_TMR_Pinset(void);// @suppress("API header file naming")
void R_TMR_Pinclr(void);// @suppress("API header file naming")
void R_CLKOUT_Pinset(void);// @suppress("API header file naming")
void R_CLKOUT_Pinclr(void);// @suppress("API header file naming")
void R_LED_Pinset(void);// @suppress("API header file naming")
void R_LED_Pinclr(void);// @suppress("API header file naming")

#endif /* PIN_H */

/* End of file (pin.h) */

/******************************************************************************************************************//**
 * @} (end addtogroup grp_device_pin_setting)
 *********************************************************************************************************************/
