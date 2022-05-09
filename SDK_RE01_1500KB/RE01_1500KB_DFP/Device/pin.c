/**********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MECHANTABILITY, 
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
* File Name    : Pin.c
* Version      : 1.50
* Description  : Pin Setting
**********************************************************************************************************************/
/*********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 07.08.2018 0.50     First Release
*         : 20.12.2018 0.60     Update Comments
*                               Fix Pin setting for WS2 update.
*                               Adapted to GSCE coding rules.
*         : 21.02.2019 0.61     Update Comments
*         : 13.03.2019 0.70     Delete EOFR setting
*                               Change pin assignment
*                                LPGOUT   P404 -> P600
*                                CLKOUT32 P406 -> P610
*                               Update Comments
*         : 05.06.2019 0.72     Update Comments
*         : 17.07.2019 1.00     Update Comments
*         : 30.09.2019 1.01     Fixed error in function name(GTP -> GPT)
*                               Update Comments
*                               Comment out the default terminal setting
*         : 06.01.2020 1.10     PMR bit initialization added
*                               Revised pin release order of SCI PinClr function
*         : 20.08.2020 1.20     Update Comments
*         : 05.11.2020 1.30     Remove unnecessary R_CCC_Pinset and R_CCC_Pinclr
*                               Add R_ICU_Pinset and R_ICU_Pinclr
*                               Split R_GPT_COM_Pinset/clr function
*                               into R_GPT_COMA_Pinset/clr and R_GPT_COMB_Pinset/clr
*         : 25.11.2021 1.50     Fixed the bug that R12DA was not output.
**********************************************************************************************************************/


/******************************************************************************************************************//**
 * @addtogroup grp_pin_func
 * @{
 *********************************************************************************************************************/
/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#include "Pin.h"

/*****************************************************************************
  Macro definitions
 *****************************************************************************/
/**  @brief Mask to clear the PFS register. */
#define R_PIN_PRV_CLR_MASK                (0x00003CF5)   /*!< Clear bit : PSEL, PMR, ASEL, ISEL */

/******************************************************************************************************************//**
 * @name R_PIN_PRV_PSEL
 * @brief Peripheral function setting definition for PSEL bit of Pmn (m = 0 to 8, n = 00 to 15).
 *********************************************************************************************************************/
/* @{ */
#define R_PIN_PRV_GPT_PSEL                (0x03U)    /*!< GPT            : 00011b */
#define R_PIN_PRV_GPT_COM_PSEL            (0x02U)    /*!< GPT_COM        : 00010b */
#define R_PIN_PRV_GPT_OPS_PSEL            (0x02U)    /*!< GPT_OPS        : 00010b */
#define R_PIN_PRV_AGT_PSEL                (0x01U)    /*!< AGT            : 00001b */
#define R_PIN_PRV_SCI_PSEL_04             (0x04U)    /*!< SCI0/2/4/9     : 00100b */
#define R_PIN_PRV_SCI_PSEL_05             (0x05U)    /*!< SCI1/3/5       : 00101b */
#define R_PIN_PRV_RSPI_PSEL               (0x06U)    /*!< RSPI           : 00110b */
#define R_PIN_PRV_QSPI_PSEL               (0x11U)    /*!< QSPI           : 10001b */
#define R_PIN_PRV_RIIC_PSEL               (0x07U)    /*!< RIIC           : 00111b */
#define R_PIN_PRV_CAC_PSEL                (0x0AU)    /*!< CAC(Digital)   : 01010b */
#define R_PIN_PRV_S14AD_PSEL              (0x0AU)    /*!< S14AD(Digital) : 01010b */
#define R_PIN_PRV_MLCD_PSEL               (0x0EU)    /*!< MLDC           : 01110b */
#define R_PIN_PRV_KINT_PSEL               (0x10U)    /*!< KINT           : 10000b */
#define R_PIN_PRV_USB_PSEL                (0x13U)    /*!< USB            : 10011b */
#define R_PIN_PRV_RTC_PSEL                (0x09U)    /*!< RTC            : 01001b */
#define R_PIN_PRV_LPG_PSEL                (0x12U)    /*!< LPG            : 10010b */
#define R_PIN_PRV_TMR_PSEL                (0x08U)    /*!< TMR            : 01000b */
#define R_PIN_PRV_ACMP_PSEL               (0x19U)    /*!< ACMP(CMPOUT)   : 11001b */
#define R_PIN_PRV_CLKOUT_PSEL             (0x19U)    /*!< CLKOUT         : 11001b */
/* @} */

/**************************************************************************//**
* @brief This function sets Pin of ICU.
*******************************************************************************/
/* Function Name : R_ICU_Pinset */
void R_ICU_Pinset(void)  // @suppress("API function naming") @suppress("Function declaration")
{
    R_ICU_Pinset_NMI();
    R_ICU_Pinset_CH0();
    R_ICU_Pinset_CH1();
    R_ICU_Pinset_CH2();
    R_ICU_Pinset_CH3();
    R_ICU_Pinset_CH4();
    R_ICU_Pinset_CH5();
    R_ICU_Pinset_CH6();
    R_ICU_Pinset_CH7();
    R_ICU_Pinset_CH8();
    R_ICU_Pinset_CH9();

}/* End of Function R_ICU_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of ICU.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr */
void R_ICU_Pinclr(void)  // @suppress("API function naming")
{
    R_ICU_Pinclr_NMI();
    R_ICU_Pinclr_CH0();
    R_ICU_Pinclr_CH1();
    R_ICU_Pinclr_CH2();
    R_ICU_Pinclr_CH3();
    R_ICU_Pinclr_CH4();
    R_ICU_Pinclr_CH5();
    R_ICU_Pinclr_CH6();
    R_ICU_Pinclr_CH7();
    R_ICU_Pinclr_CH8();
    R_ICU_Pinclr_CH9();

}/* End of function R_ICU_Pinclr() */

/**************************************************************************//**
* @brief This function sets Pin of NMI.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_NMI */
void R_ICU_Pinset_NMI(void)  // @suppress("API function naming") @suppress("Function declaration")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* NMI : P200 */
//    PFS->P200PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P200PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P200PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P200PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P200PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of Function R_ICU_Pinset_NMI() */

/**************************************************************************//**
* @brief This function clears the pin setting of NMI.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_NMI */
void R_ICU_Pinclr_NMI(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* NMI : P200 */
//    PFS->P200PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_NMI() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ0.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH0 */
void R_ICU_Pinset_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ0_A_DS : P411 */
//    PFS->P411PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P411PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P411PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P411PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P411PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ0_B : P106 */
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P106PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ0_C : P506 */
//    PFS->P506PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P506PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P506PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P506PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P506PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH0() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ0.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH0 */
void R_ICU_Pinclr_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ0_A_DS : P411 */
//    PFS->P411PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ0_B : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ0_C : P506 */
//    PFS->P506PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH0() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ1.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH1 */
void R_ICU_Pinset_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ1_A_DS : P207 */
//    PFS->P207PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P207PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P207PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P207PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P207PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ1_B : P501 */
//    PFS->P501PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P501PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P501PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P501PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P501PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ1_C : P505 */
//    PFS->P505PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P505PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P505PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P505PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P505PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH1() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ1.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH1 */
void R_ICU_Pinclr_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ1_A_DS : P207 */
//    PFS->P207PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ1_B : P501 */
//    PFS->P501PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ1_C : P505 */
//    PFS->P505PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH1() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ2.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH2 */
void R_ICU_Pinset_CH2(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ2_A_DS : P410 */
//    PFS->P410PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P410PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P410PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P410PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P410PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ2_B : P808 */
//    PFS->P808PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P808PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P808PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P808PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P808PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH2() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ2.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH2 */
void R_ICU_Pinclr_CH2(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ2_A_DS : P410 */
//    PFS->P410PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ2_B : P808 */
//    PFS->P808PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH2() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ3.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH3 */
void R_ICU_Pinset_CH3(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ3_A_DS : P409 */
//    PFS->P409PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P409PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P409PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P409PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P409PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ3_B : P807 */
//    PFS->P807PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P807PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P807PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P807PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P807PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH3() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ3.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH3 */
void R_ICU_Pinclr_CH3(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ3_A_DS : P409 */
//    PFS->P409PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ3_B : P807 */
//    PFS->P807PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH3() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ4.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH4 */
void R_ICU_Pinset_CH4(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ4 : P202 */
//    PFS->P202PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P202PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P202PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P202PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P202PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ4 : P508 */
//    PFS->P508PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P508PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P508PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P508PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P508PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH4() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ4.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH4 */
void R_ICU_Pinclr_CH4(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ4 : P202 */
//    PFS->P202PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ4 : P508 */
//    PFS->P508PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH4() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ5.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH5 */
void R_ICU_Pinset_CH5(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ5 : P113 */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P113PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ5 : P604 */
//    PFS->P604PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P604PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P604PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P604PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P604PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH5() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ5.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH5 */
void R_ICU_Pinclr_CH5(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ5 : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ5 : P604 */
//    PFS->P604PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH5() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ6.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH6 */
void R_ICU_Pinset_CH6(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ6 : P112 */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P112PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ6 : P014 */
//    PFS->P014PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P014PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P014PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P014PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P014PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH6() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ6.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH6 */
void R_ICU_Pinclr_CH6(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ6 : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ6 : P014 */
//    PFS->P014PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH6() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ7.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH7 */
void R_ICU_Pinset_CH7(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ7 : P107 */
//    PFS->P107PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P107PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P107PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P107PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P107PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ7 : P015 */
//    PFS->P015PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P015PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P015PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH7() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ7.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH7 */
void R_ICU_Pinclr_CH7(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ7 : P107 */
//    PFS->P107PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ7 : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH7() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ8.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH8 */
void R_ICU_Pinset_CH8(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ8 : P105 */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P105PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ8 : P205 */
//    PFS->P205PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P205PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P205PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P205PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P205PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH8() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ8.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH8 */
void R_ICU_Pinclr_CH8(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ8 : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ8 : P205 */
//    PFS->P205PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH8() */

/**************************************************************************//**
* @brief This function sets Pin of IRQ9.
*******************************************************************************/
/* Function Name : R_ICU_Pinset_CH9 */
void R_ICU_Pinset_CH9(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ9 : P104 */
//    PFS->P104PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P104PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P104PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P104PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P104PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* IRQ9 : P204 */
//    PFS->P204PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P204PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P204PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P204PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P204PFS_b.ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinset_CH9() */

/**************************************************************************//**
* @brief This function clears the pin setting of IRQ9.
*******************************************************************************/
/* Function Name : R_ICU_Pinclr_CH9 */
void R_ICU_Pinclr_CH9(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* IRQ9 : P104 */
//    PFS->P104PFS &= R_PIN_PRV_CLR_MASK;

    /* IRQ9 : P204 */
//    PFS->P204PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ICU_Pinclr_CH9() */

/**************************************************************************//**
* @brief This function sets Pin of GPT_COM.
*******************************************************************************/
/* Function Name : R_GPT_COMA_Pinset */
void R_GPT_COMA_Pinset(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTETRGA : P413 */
//    PFS->P413PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P413PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P413PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P413PFS_b.PSEL = R_PIN_PRV_GPT_COM_PSEL;
//    PFS->P413PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTETRGA : P809 */
//    PFS->P809PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P809PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P809PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P809PFS_b.PSEL = R_PIN_PRV_GPT_COM_PSEL;
//    PFS->P809PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTETRGA : P608 */
//    PFS->P608PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P608PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P608PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P608PFS_b.PSEL = R_PIN_PRV_GPT_COM_PSEL;
//    PFS->P608PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_COMA_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT_COM.
*******************************************************************************/
/* Function Name : R_GPT_COMA_Pinclr */
void R_GPT_COMA_Pinclr(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTETRGA : P413 */
//    PFS->P413PFS &= R_PIN_PRV_CLR_MASK;

    /* GTETRGA : P809 */
//    PFS->P809PFS &= R_PIN_PRV_CLR_MASK;

    /* GTETRGA : P608 */
//    PFS->P608PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_COMA_Pinclr() */

/**************************************************************************//**
* @brief This function sets Pin of GPT_COM.
*******************************************************************************/
/* Function Name : R_GPT_COMB_Pinset */
void R_GPT_COMB_Pinset(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTETRGB : P412 */
//    PFS->P412PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P412PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P412PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P412PFS_b.PSEL = R_PIN_PRV_GPT_COM_PSEL;
//    PFS->P412PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTETRGB : P808 */
//    PFS->P808PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P808PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P808PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P808PFS_b.PSEL = R_PIN_PRV_GPT_COM_PSEL;
//    PFS->P808PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTETRGB : P607 */
//    PFS->P607PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P607PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P607PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P607PFS_b.PSEL = R_PIN_PRV_GPT_COM_PSEL;
//    PFS->P607PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_COMB_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT_COM.
*******************************************************************************/
/* Function Name : R_GPT_COMB_Pinclr */
void R_GPT_COMB_Pinclr(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTETRGB : P412 */
//    PFS->P412PFS &= R_PIN_PRV_CLR_MASK;

    /* GTETRGB : P808 */
//    PFS->P808PFS &= R_PIN_PRV_CLR_MASK;

    /* GTETRGB : P607 */
//    PFS->P607PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_COMB_Pinclr() */

/**************************************************************************//**
* @brief This function sets Pin of GPT320.
*******************************************************************************/
/* Function Name : R_GPT_Pinset_CH0 */
void R_GPT_Pinset_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC0A : P413 */
//    PFS->P413PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P413PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P413PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P413PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P413PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC0A : P101 */
//    PFS->P101PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P101PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P101PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P101PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P101PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC0A : P812 */
//    PFS->P812PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P812PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P812PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P812PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P812PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC0B : P412 */
//    PFS->P412PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P412PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P412PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P412PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P412PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC0B : P100 */
//    PFS->P100PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P100PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P100PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P100PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P100PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC0B : P811 */
//    PFS->P811PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P811PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P811PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P811PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P811PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinset_CH0() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT320.
*******************************************************************************/
/* Function Name : R_GPT_Pinclr_CH0 */
void R_GPT_Pinclr_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC0A : P413 */
//    PFS->P413PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0A : P101 */
//    PFS->P101PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0A : P812 */
//    PFS->P812PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0B : P412 */
//    PFS->P412PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0B : P100 */
//    PFS->P100PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0B : P811 */
//    PFS->P811PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinclr_CH0() */

/**************************************************************************//**
* @brief This function sets Pin of GPT321.
*******************************************************************************/
/* Function Name : R_GPT_Pinset_CH1 */
void R_GPT_Pinset_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC1A : P107 */
//    PFS->P107PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P107PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P107PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P107PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P107PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC1A : P807 */
//    PFS->P807PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P807PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P807PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P807PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P807PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC1A : P510 */
//    PFS->P510PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P510PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P510PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P510PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P510PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC1B : P106 */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P106PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P106PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC1B : P806 */
//    PFS->P806PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P806PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P806PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P806PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P806PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC1B : P511 */
//    PFS->P511PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P511PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P511PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P511PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P511PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinset_CH1() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT321.
*******************************************************************************/
/* Function Name : R_GPT_Pinclr_CH1 */
void R_GPT_Pinclr_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC1A : P107 */
//    PFS->P107PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC1A : P807 */
//    PFS->P807PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC1A : P510 */
//    PFS->P510PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC1B : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC1B : P806 */
//    PFS->P806PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC1B : P511 */
//    PFS->P511PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinclr_CH1() */

/**************************************************************************//**
* @brief This function sets Pin of GPT162.
*******************************************************************************/
/* Function Name : R_GPT_Pinset_CH2 */
void R_GPT_Pinset_CH2(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    
    /* GTIOC2A : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC2A : P810 */
//    PFS->P810PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P810PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P810PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P810PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P810PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC2A : P507 */
//    PFS->P507PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P507PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P507PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P507PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P507PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */


    /* GTIOC2B : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* GTIOC2B : P809 */
//    PFS->P809PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P809PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P809PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P809PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P809PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* GTIOC2B : P508 */
//    PFS->P508PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P508PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P508PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P508PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P508PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinset_CH2() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT162.
*******************************************************************************/
/* Function Name : R_GPT_Pinclr_CH2 */
void R_GPT_Pinclr_CH2(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC2A : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC2A : P810 */
//    PFS->P810PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC2A : P507 */
//    PFS->P507PFS &= R_PIN_PRV_CLR_MASK;


    /* GTIOC2B : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC2B : P809 */
//    PFS->P809PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC2B : P508 */
//    PFS->P508PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinclr_CH2() */

/**************************************************************************//**
* @brief This function sets Pin of GPT163.
*******************************************************************************/
/* Function Name : R_GPT_Pinset_CH3 */
void R_GPT_Pinset_CH3(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC3A : P113 */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P113PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P113PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC3A : P409 */
//    PFS->P409PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P409PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P409PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P409PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P409PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC3A : P013 */
//    PFS->P013PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P013PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P013PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P013PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P013PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC3B : P112 */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P112PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P112PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC3B : P410 */
//    PFS->P410PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P410PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P410PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P410PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P410PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC3B : P014 */
//    PFS->P014PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P014PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P014PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P014PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P014PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinset_CH3() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT163.
*******************************************************************************/
/* Function Name : R_GPT_Pinclr_CH3 */
void R_GPT_Pinclr_CH3(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC3A : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC3A : P409 */
//    PFS->P409PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC3A : P013 */
//    PFS->P013PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC3B : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC3A : P410 */
//    PFS->P410PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC3A : P014 */
//    PFS->P014PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinclr_CH3() */

/**************************************************************************//**
* @brief This function sets Pin of GPT164.
*******************************************************************************/
/* Function Name : R_GPT_Pinset_CH4 */
void R_GPT_Pinset_CH4(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC4A : P105 */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P105PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P105PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC4A : P315 */
//    PFS->P315PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P315PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P315PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P315PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P315PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC4A : P015 */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P015PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P015PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P015PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC4B : P104 */
//    PFS->P104PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P104PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P104PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P104PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P104PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC4B : P314 */
//    PFS->P314PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P314PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P314PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P314PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P314PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC4B : P500 */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P500PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinset_CH4() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT164.
*******************************************************************************/
/* Function Name : R_GPT_Pinclr_CH4 */
void R_GPT_Pinclr_CH4(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC4A : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC4A : P315 */
//    PFS->P315PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC4A : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC4B : P104 */
//    PFS->P104PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC4B : P314 */
//    PFS->P314PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC4B : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;


    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinclr_CH4() */

/**************************************************************************//**
* @brief This function sets Pin of GPT165.
*******************************************************************************/
/* Function Name : R_GPT_Pinset_CH5 */
void R_GPT_Pinset_CH5(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC5A : P103 */
//    PFS->P103PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P103PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P103PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P103PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P103PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC5A : P603 */
//    PFS->P603PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P603PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P603PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P603PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P603PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC5A : P815 */
//    PFS->P815PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P815PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P815PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P815PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P815PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC5B : P102 */
//    PFS->P102PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P102PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P102PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P102PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P102PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC5B : P604 */
//    PFS->P604PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P604PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P604PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P604PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P604PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIOC5B : P814 */
//    PFS->P814PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P814PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P814PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P814PFS_b.PSEL = R_PIN_PRV_GPT_PSEL;
//    PFS->P814PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinset_CH5() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT165.
*******************************************************************************/
/* Function Name : R_GPT_Pinclr_CH5 */
void R_GPT_Pinclr_CH5(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIOC5A : P103 */
//    PFS->P103PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC5A : P603 */
//    PFS->P603PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC5A : P815 */
//    PFS->P815PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0A : P102 */
//    PFS->P102PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0A : P604 */
//    PFS->P604PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIOC0A : P814 */
//    PFS->P814PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_Pinclr_CH5() */

/**************************************************************************//**
* @brief This function sets Pin of GPT_OPS.
*******************************************************************************/
/* Function Name : R_GPT_OPS_Pinset */
void R_GPT_OPS_Pinset(void)  // @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIU : P204 */
//    PFS->P204PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P204PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P204PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P204PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P204PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIU : P302 */
//    PFS->P302PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P302PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P302PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P302PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P302PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIU : P804 */
//    PFS->P804PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P804PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P804PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P804PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P804PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */


    /* GTIV : P203 */
//    PFS->P203PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P203PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P203PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P203PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P203PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIV : P301 */
//    PFS->P301PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P301PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P301PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P301PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P301PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIV : P803 */
//    PFS->P803PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P803PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P803PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P803PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P803PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIW : P202 */
//    PFS->P202PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P202PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P202PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P202PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P202PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIW : P300 */
//    PFS->P300PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P300PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P300PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P300PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P300PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTIW : P802 */
//    PFS->P802PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P802PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P802PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P802PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P802PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOULO : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOULO : P601 */
//    PFS->P601PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P601PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P601PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P601PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P601PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOULO : P800 */
//    PFS->P800PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P800PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P800PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P800PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P800PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOUUP : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOUUP : P602 */
//    PFS->P602PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P602PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P602PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P602PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P602PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOUUP : P801 */
//    PFS->P801PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P801PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P801PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P801PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P801PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOVLO : P108 */
//    PFS->P108PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P108PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P108PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P108PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P108PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOVLO : P510 */
//    PFS->P510PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P510PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P510PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P510PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P510PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOVLO : P407 */
//    PFS->P407PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P407PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P407PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P407PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P407PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOVUP : P109 */
//    PFS->P109PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P109PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P109PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P109PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P109PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOVUP : P511 */      
//    PFS->P511PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P511PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P511PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P511PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P511PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOVUP : P408 */
//    PFS->P408PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P408PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P408PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P408PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P408PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOWLO : P112 */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P112PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P112PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOWLO : P015 */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P015PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P015PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P015PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOWLO : P405 */
//    PFS->P405PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P405PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P405PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P405PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P405PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOWUP : P113 */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P113PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P113PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOWUP : P500 */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P500PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* GTOWUP : P406 */
//    PFS->P406PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P406PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P406PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P406PFS_b.PSEL = R_PIN_PRV_GPT_OPS_PSEL;
//    PFS->P406PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_OPS_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of GPT_OPS.
*******************************************************************************/
/* Function Name : R_GPT_OPS_Pinclr */
void R_GPT_OPS_Pinclr(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* GTIU : P204 */
//    PFS->P204PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIU : P302 */
//    PFS->P302PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIU : P804 */
//    PFS->P804PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIV : P203 */
//    PFS->P203PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIV : P301 */
//    PFS->P301PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIV : 803 */
//    PFS->P803PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIW : P202 */
//    PFS->P202PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIW : P300 */
//    PFS->P300PFS &= R_PIN_PRV_CLR_MASK;

    /* GTIW : P802 */
//    PFS->P802PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOULO : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOULO : P601 */
//    PFS->P601PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOULO : P800 */
//    PFS->P800PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOUUP : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOUUP : P602 */
//    PFS->P602PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOUUP : P801 */
//    PFS->P801PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOVLO : P108 */
//    PFS->P108PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOVLO : P510 */
//    PFS->P510PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOVLO : P407 */
//    PFS->P407PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOVUP : P109 */
//    PFS->P109PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOVUP : P511 */      
//    PFS->P511PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOVUP : P408 */
//    PFS->P408PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOWLO : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOWLO : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOWLO : P405 */
//    PFS->P405PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOWUP : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOWUP : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;

    /* GTOWUP : P406 */
//    PFS->P406PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_GPT_OPS_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of AGT0.
*******************************************************************************/
/* Function Name : R_AGT_Pinset_CH0 */
void R_AGT_Pinset_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* AGTEE0 : P809 */
//    PFS->P809PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P809PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P809PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P809PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P809PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTEE0 : P112 */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P112PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P112PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTIO0 : P810 */
//    PFS->P810PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P810PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P810PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P810PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P810PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTIO0 : P108 */
//    PFS->P108PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P108PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P108PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P108PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P108PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTO0 : P808 */
//    PFS->P808PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P808PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P808PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P808PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P808PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTO0 : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOA0 : P807 */
//    PFS->P807PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P807PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P807PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P807PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P807PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOA0 : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOB : P806 */
//    PFS->P806PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P806PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P806PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P806PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P806PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOB : P109 */
//    PFS->P109PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P109PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P109PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P109PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P109PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_AGT_Pinset_CH0() */

/**************************************************************************//**
* @brief This function clears the pin setting of AGT0.
*******************************************************************************/
/* Function Name : R_AGT_Pinclr_CH0 */
void R_AGT_Pinclr_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* AGTEE0 : P809 */
//    PFS->P809PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTEE0 : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTIO0 : P810 */
//    PFS->P810PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTIO0 : P108 */
//    PFS->P108PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTO0 : P808 */
//    PFS->P808PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTO0 : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOA0 : P807 */
//    PFS->P807PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOA0 : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOB : P806 */
//    PFS->P806PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOB : P109 */
//    PFS->P109PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_AGT_Pinclr_CH0() */


/**************************************************************************//**
* @brief This function sets Pin of AGT1.
*******************************************************************************/
/* Function Name : R_AGT_Pinset_CH1 */
void R_AGT_Pinset_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* AGTEE1 : P113 */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P113PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P113PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTEE1 : P014 */
//    PFS->P014PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P014PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P014PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P014PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P014PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTEE1 : P305 */
//    PFS->P305PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P305PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P305PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P305PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P305PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTIO1 : P104 */
//    PFS->P104PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P104PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P104PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P104PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P104PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTIO1 : P015 */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P015PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P015PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P015PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTIO1 : P309 */
//    PFS->P309PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P309PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P309PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P309PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P309PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTO1 : P105 */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P105PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P105PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTO1 : P013 */
//    PFS->P013PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P013PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P013PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P013PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P013PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTO1 : P308 */
//    PFS->P308PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P308PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P308PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P308PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P308PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOA : P106 */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P106PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P106PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOA : P501 */
//    PFS->P501PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P501PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P501PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P501PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P501PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOA : P307 */
//    PFS->P307PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P307PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P307PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P307PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P307PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOB1 : P107 */
//    PFS->P107PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P107PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P107PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P107PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P107PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOB1 : P500 */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P500PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* AGTOB1 : P306 */
//    PFS->P306PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P306PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P306PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P306PFS_b.PSEL = R_PIN_PRV_AGT_PSEL;
//    PFS->P306PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_AGT_Pinset_CH1() */

/**************************************************************************//**
* @brief This function clears the pin setting of AGT1.
*******************************************************************************/
/* Function Name : R_AGT_Pinclr_CH1 */
void R_AGT_Pinclr_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* AGTEE1 : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTEE1 : P014 */
//    PFS->P014PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTEE1 : P305 */
//    PFS->P305PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTIO1 : P104 */
//    PFS->P104PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTIO1 : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTIO1 : P309 */
//    PFS->P309PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTO1 : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTO1 : P013 */
//    PFS->P013PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTO1 : P308 */
//    PFS->P308PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOA : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOA : P501 */
//    PFS->P501PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOA : P307 */
//    PFS->P307PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOB1 : P107 */
//    PFS->P107PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOB1 : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;

    /* AGTOB1 : P306 */
//    PFS->P306PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_AGT_Pinclr_CH1() */


/**************************************************************************//**
* @brief This function sets Pin of SCI0.
*******************************************************************************/
/* Function Name : R_SCI_Pinset_CH0 */
void R_SCI_Pinset_CH0(void)  // @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CTS0 : P107 */
//    PFS->P107PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P107PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P107PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P107PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P107PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS0 : P500 */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P500PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS0 : P704 */
//    PFS->P704PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P704PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P704PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P704PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P704PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD0 : P106 */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P106PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P106PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P106PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P106PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD0 : P013 */
//    PFS->P013PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P013PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P013PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P013PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P013PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P013PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P013PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD0 : P703 */
//    PFS->P703PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P703PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P703PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P703PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P703PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P703PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P703PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD0 : P105 */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P105PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P105PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P105PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P105PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD0 : P014 */
//    PFS->P014PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P014PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P014PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P014PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P014PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P014PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P014PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD0 : P702 */
//    PFS->P702PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P702PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P702PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P702PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P702PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P702PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P702PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK0 : P104 */
//    PFS->P104PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P104PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P104PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P104PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P104PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK0 : P015 */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P015PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P015PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P015PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK0 : P700 */
//    PFS->P700PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P700PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P700PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P700PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P700PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_SCI_Pinset_CH0() */


/**************************************************************************//**
* @brief This function clears the pin setting of SCI0.
*******************************************************************************/
/* Function Name : R_SCI_Pinclr_CH0 */
void R_SCI_Pinclr_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* TXD0 : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD0 : P013 */
//    PFS->P013PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD0 : P703 */
//    PFS->P703PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK0 : P104 */
//    PFS->P104PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK0 : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK0 : P700 */
//    PFS->P700PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD0 : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD0 : P014 */
//    PFS->P014PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD0 : P702 */
//    PFS->P702PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS0 : P107 */
//    PFS->P107PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS0 : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS0 : P704 */
//    PFS->P704PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_SCI_Pinclr_CH0() */


/**************************************************************************//**
* @brief This function sets Pin of SCI1.
*******************************************************************************/
/* Function Name : R_SCI_Pinset_CH1 */
void R_SCI_Pinset_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CTS1 : P103 */
//    PFS->P103PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P103PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P103PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P103PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P103PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS1 : P311 */
//    PFS->P311PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P311PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P311PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P311PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P311PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD1 : P102 */
//    PFS->P102PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P102PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P102PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P102PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P102PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P102PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P102PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD1 : P313 */
//    PFS->P313PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P313PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P313PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P313PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P313PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P313PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P313PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD1 : P101 */
//    PFS->P101PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P101PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P101PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P101PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P101PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P101PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P101PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD1 : P312 */
//    PFS->P312PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P312PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P312PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P312PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P312PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P312PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P312PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK1 : P100 */
//    PFS->P100PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P100PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P100PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P100PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P100PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK1 : P310 */
//    PFS->P310PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P310PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P310PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P310PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P310PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_SCI_Pinset_CH1() */


/**************************************************************************//**
* @brief This function clears the pin setting of SCI1.
*******************************************************************************/
/* Function Name : R_SCI_Pinclr_CH1 */
void R_SCI_Pinclr_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* TXD1 : P102 */
//    PFS->P102PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD1 : P313 */
//    PFS->P313PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK1 : P100 */
//    PFS->P100PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK1 : P310 */
//    PFS->P310PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD1 : P101 */
//    PFS->P101PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD1 : P312 */
//    PFS->P312PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS1 : P103 */
//    PFS->P103PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS1 : P311 */
//    PFS->P311PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_SCI_Pinclr_CH1() */

/**************************************************************************//**
* @brief This function sets Pin of SCI2.
*******************************************************************************/
/* Function Name : R_SCI_Pinset_CH2 */
void R_SCI_Pinset_CH2(void)  // @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CTS2 : P103 */
//    PFS->P103PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P103PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P103PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P103PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P103PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS2 : P504 */
//    PFS->P504PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P504PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P504PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P504PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P504PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS2 : P607 */
//    PFS->P607PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P607PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P607PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P607PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P607PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* TXD2 : P102 */
//    PFS->P102PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P102PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P102PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P102PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P102PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P102PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P102PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD2 : P501 */
//    PFS->P501PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P501PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P501PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P501PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P501PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P501PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P501PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD2 : P609 */
//    PFS->P609PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P609PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P609PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P609PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P609PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P609PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P609PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* RXD2 : P101 */
//    PFS->P101PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P101PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P101PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P101PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P101PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P101PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P101PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD2 : P502 */
//    PFS->P502PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P502PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P502PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P502PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P502PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P502PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P502PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD2 : P608 */
//    PFS->P608PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P608PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P608PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P608PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P608PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P608PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P608PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* SCK2 : P100 */
//    PFS->P100PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P100PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P100PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P100PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P100PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK2 : P503 */
//    PFS->P503PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P503PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P503PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P503PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P503PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK2 : P606 */
//    PFS->P606PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P606PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P606PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P606PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P606PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_SCI_Pinset_CH2() */

/**************************************************************************//**
* @brief This function clears the pin setting of SCI2.
*******************************************************************************/
/* Function Name : R_SCI_Pinclr_CH2 */
void R_SCI_Pinclr_CH2(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    
    /* TXD2 : P102 */
//    PFS->P102PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD2 : P501 */
//    PFS->P501PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD2 : P609 */
//    PFS->P609PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK2 : P100 */
//    PFS->P100PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK2 : P503 */
//    PFS->P503PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK2 : P606 */
//    PFS->P606PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD2 : P101 */
//    PFS->P101PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD2 : P502 */
//    PFS->P502PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD2 : P608 */
//    PFS->P608PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS2 : P103 */
//    PFS->P103PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS2 : P504 */
//    PFS->P504PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS2 : P607 */
//    PFS->P607PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_SCI_Pinclr_CH2() */

/**************************************************************************//**
* @brief This function sets Pin of SCI3.
*******************************************************************************/
/* Function Name : R_SCI_Pinset_CH3 */
void R_SCI_Pinset_CH3(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CTS3 : P207 */
//    PFS->P207PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P207PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P207PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P207PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P207PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS3 : P807 */
//    PFS->P807PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P807PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P807PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P807PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P807PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD3 : P413 */
//    PFS->P413PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P413PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P413PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P413PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P413PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P413PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P413PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD3 : P809 */
//    PFS->P809PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P809PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P809PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P809PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P809PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P809PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P809PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD3 : P412 */
//    PFS->P412PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P412PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P412PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P412PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P412PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P412PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P412PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD3 : P808 */
//    PFS->P808PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P808PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P808PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P808PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P808PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P808PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P808PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK3 : P411 */
//    PFS->P411PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P411PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P411PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P411PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P411PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK3 : P810 */
//    PFS->P810PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P810PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P810PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P810PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P810PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_SCI_Pinset_CH3() */

/**************************************************************************//**
* @brief This function clears the pin setting of SCI3.
*******************************************************************************/
/* Function Name : R_SCI_Pinclr_CH3 */
void R_SCI_Pinclr_CH3(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* TXD3 : P413 */
//    PFS->P413PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD3 : P809 */
//    PFS->P809PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK3 : P411 */
//    PFS->P411PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK3 : P810 */
//    PFS->P810PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD3 : P412 */
//    PFS->P412PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD3 : P808 */
//    PFS->P808PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS3 : P207 */
//    PFS->P207PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS3 : P807 */
//    PFS->P807PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_SCI_Pinclr_CH3() */

/**************************************************************************//**
* @brief This function sets Pin of SCI4.
*******************************************************************************/
/* Function Name : R_SCI_Pinset_CH4 */
void R_SCI_Pinset_CH4(void)  // @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CTS4 : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS4 : P205 */
//    PFS->P205PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P205PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P205PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P205PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P205PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS4 : P815 */
//    PFS->P815PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P815PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P815PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P815PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P815PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD4 : P113 */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P113PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P113PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P113PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P113PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD4 : P203 */
//    PFS->P203PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P203PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P203PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P203PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P203PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P203PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P203PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD4 : P812 */
//    PFS->P812PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P812PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P812PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P812PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P812PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P812PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P812PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD4 : P112 */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P112PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P112PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P112PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P112PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD4 : P202 */
//    PFS->P202PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P202PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P202PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P202PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P202PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P202PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P202PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD4 : P813 */
//    PFS->P813PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P813PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P813PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P813PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P813PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P813PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P813PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK4 : P108 */
//    PFS->P108PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P108PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P108PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P108PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P108PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK4 : P204 */
//    PFS->P204PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P204PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P204PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P204PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P204PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK4 : P814 */
//    PFS->P814PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P814PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P814PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P814PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P814PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */


    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_SCI_Pinset_CH4() */

/**************************************************************************//**
* @brief This function clears the pin setting of SCI4.
*******************************************************************************/
/* Function Name : R_SCI_Pinclr_CH4 */
void R_SCI_Pinclr_CH4(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* TXD4 : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD4 : P203 */
//    PFS->P203PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD4 : P812 */
//    PFS->P812PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK4 : P108 */
//    PFS->P108PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK4 : P204 */
//    PFS->P204PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK4 : P814 */
//    PFS->P814PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD4 : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD4 : P202 */
//    PFS->P202PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD4 : P813 */
//    PFS->P813PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS4 : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS4 : P205 */
//    PFS->P205PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS4 : P815 */
//    PFS->P815PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_SCI_Pinclr_CH4() */

/**************************************************************************//**
* @brief This function sets Pin of SCI5.
*******************************************************************************/
/* Function Name : R_SCI_Pinset_CH5 */
void R_SCI_Pinset_CH5(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CTS5 : P109 */
//    PFS->P109PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P109PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P109PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P109PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P109PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS5 : P302 */
//    PFS->P302PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P302PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P302PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P302PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P302PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD5 : P108 */
//    PFS->P108PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P108PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P108PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P108PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P108PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P108PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P108PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD5 : P315 */
//    PFS->P315PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P315PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P315PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P315PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P315PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P315PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P315PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD5 : P304 */
//    PFS->P304PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P304PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P304PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P304PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P304PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P304PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P304PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD5 : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P111PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P111PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD5 : P314 */
//    PFS->P314PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P314PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P314PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P314PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P314PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P314PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P314PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD5 : P303 */
//    PFS->P303PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P303PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P303PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P303PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P303PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P303PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P303PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK5 : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK5 : P301 */
//    PFS->P301PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P301PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P301PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P301PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_05;
//    PFS->P301PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_SCI_Pinset_CH5() */

/**************************************************************************//**
* @brief This function clears the pin setting of SCI5.
*******************************************************************************/
/* Function Name : R_SCI_Pinclr_CH5 */
void R_SCI_Pinclr_CH5(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    /* TXD5 : P108 */
//    PFS->P108PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD5 : P315 */
//    PFS->P315PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD5 : P304 */
//    PFS->P304PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK5 : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK5 : P301 */
//    PFS->P301PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD5 : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD5 : P314 */
//    PFS->P314PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD5 : P303 */
//    PFS->P303PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS5 : P109 */
//    PFS->P109PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS5 : P302 */
//    PFS->P302PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_SCI_Pinclr_CH5() */


/**************************************************************************//**
* @brief This function sets Pin of SCI9.
*******************************************************************************/
/* Function Name : R_SCI_Pinset_CH9 */
void R_SCI_Pinset_CH9(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CTS9 : P109 */
//    PFS->P109PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P109PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P109PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P109PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P109PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CTS9 : P601 */
//    PFS->P601PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P601PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P601PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P601PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P601PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD9 : P411 */
//    PFS->P411PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P411PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P411PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P411PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P411PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P411PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P411PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD9 : P604 */
//    PFS->P604PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P604PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P604PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P604PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P604PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P604PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P604PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TXD9 : P605 */
//    PFS->P605PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P605PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P605PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P605PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P605PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P605PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P605PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD9 : P207 */
//    PFS->P207PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P207PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P207PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P207PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P207PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P207PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P207PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RXD9 : P603 */
//    PFS->P603PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P603PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P603PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* When using SCI in I2C mode, set the pin to NMOS Open drain. */
////    PFS->P603PFS_b.NCODR = 1U;  /* 0: CMOS output, 1: NMOS open-drain output. */
////    PFS->P603PFS_b.PCODR = 0U;  /* 0: CMOS output, 1: PMOS open-drain output. */
//    PFS->P603PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P603PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK9 : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK9 : P602 */
//    PFS->P602PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P602PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P602PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P602PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P602PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SCK9 : P600 */
//    PFS->P600PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P600PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P600PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P600PFS_b.PSEL = R_PIN_PRV_SCI_PSEL_04;
//    PFS->P600PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_SCI_Pinset_CH9() */

/**************************************************************************//**
* @brief This function clears the pin setting of SCI9.
*******************************************************************************/
/* Function Name : R_SCI_Pinclr_CH9 */
void R_SCI_Pinclr_CH9(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* TXD9 : P411 */
//    PFS->P411PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD9 : P604 */
//    PFS->P604PFS &= R_PIN_PRV_CLR_MASK;

    /* TXD9 : P605 */
//    PFS->P605PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK9 : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK9 : P602 */
//    PFS->P602PFS &= R_PIN_PRV_CLR_MASK;

    /* SCK9 : P600 */
//    PFS->P600PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD9 : P207 */
//    PFS->P207PFS &= R_PIN_PRV_CLR_MASK;

    /* RXD9 : P603 */
//    PFS->P603PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS9 : P109 */
//    PFS->P109PFS &= R_PIN_PRV_CLR_MASK;

    /* CTS9 : P601 */
//    PFS->P601PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_SCI_Pinclr_CH9() */


/**************************************************************************//**
* @brief This function sets Pin of RSPI0.
* @note  In the case of SPI function, the same signal names are present with @n
* the suffixes "_A", "_B" "_C" and "_D" attached. These indicate groups @n
* in terms of timing adjustment, and signals from different @n
* groups cannot be used at the same time. The exceptions are the RSPCKA_C and MOSIA_C signals @n
* for the SPI and the SSLB0_D signal, which can be used at the same time as signals from group B. @n
*******************************************************************************/
/* Function Name : R_RSPI_Pinset_CH0 */
void R_RSPI_Pinset_CH0(void)  // @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* MISOA_A : P105 */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P105PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P105PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MISOA_B : P500 */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P500PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MOSIA_A : P104 */
//    PFS->P104PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P104PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P104PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P104PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P104PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MOSIA_B : P010 */
//    PFS->P010PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P010PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P010PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P010PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P010PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MOSIA_C : P501 */
//    PFS->P501PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P501PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P501PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P501PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P501PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RSPCKA_A : P107 */
//    PFS->P107PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P107PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P107PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P107PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P107PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RSPCKA_B : P011 */
//    PFS->P011PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P011PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P011PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P011PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P011PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RSPCKA_C : P502 */
//    PFS->P502PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P502PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P502PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P502PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P502PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA0_A : P103 */
//    PFS->P103PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P103PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P103PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P103PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P103PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA0_B : P012 */
//    PFS->P012PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P012PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P012PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P012PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P012PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA1_A : P102 */
//    PFS->P102PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P102PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P102PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P102PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P102PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA1_B : P013 */
//    PFS->P013PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P013PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P013PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P013PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P013PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA2_A : P101 */
//    PFS->P101PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P101PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P101PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P101PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P101PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA2_B : P014 */
//    PFS->P014PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P014PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P014PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P014PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P014PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA3_A : P100 */
//    PFS->P100PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P100PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P100PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P100PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P100PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLA3_B : P015 */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P015PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P015PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P015PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RSPI_Pinset_CH0() */

/**************************************************************************//**
* @brief This function clears the pin setting of RSPI0.
*******************************************************************************/
/* Function Name : R_RSPI_Pinclr_CH0 */
void R_RSPI_Pinclr_CH0(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* MISOA_A : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* MISOA_B : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;

    /* MOSIA_A : P104 */
//    PFS->P104PFS &= R_PIN_PRV_CLR_MASK;

    /* MOSIA_B : P010 */
//    PFS->P010PFS &= R_PIN_PRV_CLR_MASK;

    /* MOSIA_C : P501 */
//    PFS->P501PFS &= R_PIN_PRV_CLR_MASK;

    /* RSPCKA_A : P107 */
//    PFS->P107PFS &= R_PIN_PRV_CLR_MASK;

    /* RSPCKA_B : P011 */
//    PFS->P011PFS &= R_PIN_PRV_CLR_MASK;

    /* RSPCKA_C : P502 */
//    PFS->P502PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA0_A : P103 */
//    PFS->P103PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA0_B : P012 */
//    PFS->P012PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA1_A : P102 */
//    PFS->P102PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA1_B : P013 */
//    PFS->P013PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA2_A : P101 */
//    PFS->P101PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA2_B : P014 */
//    PFS->P014PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA3_A : P100 */
//    PFS->P100PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLA3_B : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;


    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RSPI_Pinclr_CH0() */



/**************************************************************************//**
* @brief This function sets Pin of RSPI1.
* @note  In the case of SPI function, the same signal names are present with @n
* the suffixes "_A", "_B" "_C" and "_D" attached. These indicate groups @n
* in terms of timing adjustment, and signals from different @n
* groups cannot be used at the same time. The exceptions are the RSPCKA_C and MOSIA_C signals @n
* for the SPI and the SSLB0_D signal, which can be used at the same time as signals from group B. @n
*******************************************************************************/
/* Function Name : R_RSPI_Pinset_CH1 */
void R_RSPI_Pinset_CH1(void)  // @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* MISOB_A : P109 */
//    PFS->P109PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P109PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P109PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P109PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P109PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MISOB_B : P608 */
//    PFS->P608PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P608PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P608PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P608PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P608PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MISOB_C : P804 */
//    PFS->P804PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P804PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P804PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P804PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P804PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MOSIB_A : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MOSIB_B : P609 */
//    PFS->P609PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P609PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P609PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P609PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P609PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MOSIB_C : P805 */
//    PFS->P805PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P805PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P805PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P805PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P805PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RSPCKB_A : P108 */
//    PFS->P108PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P108PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P108PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P108PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P108PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RSPCKB_B : P607 */
//    PFS->P607PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P607PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P607PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P607PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P607PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RSPCKB_C : P801 */
//    PFS->P801PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P801PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P801PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P801PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P801PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB0_A : P106 */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P106PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P106PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB0_B : P610 */
//    PFS->P610PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P610PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P610PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P610PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P610PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB0_C : P800 */
//    PFS->P800PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P800PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P800PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P800PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P800PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB0_D : P603 */
//    PFS->P603PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P603PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P603PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P603PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P603PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB1_A : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB1_B : P605 */
//    PFS->P605PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P605PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P605PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P605PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P605PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB1_C : P802 */
//    PFS->P802PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P802PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P802PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P802PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P802PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB2_A : P113 */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P113PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P113PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB2_B : P606 */
//    PFS->P606PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P606PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P606PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P606PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P606PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* SSLB2_C : P803 */
//    PFS->P803PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P803PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P803PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P803PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P803PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB3_A : P112 */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P112PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P112PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB3_B : P604 */
//    PFS->P604PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P604PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P604PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P604PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P604PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SSLB3_C : P807 */
//    PFS->P807PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P807PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P807PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P807PFS_b.PSEL = R_PIN_PRV_RSPI_PSEL;
//    PFS->P807PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RSPI_Pinset_CH1() */

/**************************************************************************//**
* @brief This function clears the pin setting of RSPI1.
*******************************************************************************/
/* Function Name : R_RSPI_Pinclr_CH1 */
void R_RSPI_Pinclr_CH1(void)  // @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* MISOB_A : P109 */
//    PFS->P109PFS &= R_PIN_PRV_CLR_MASK;

    /* MISOB_B : P608 */
//    PFS->P608PFS &= R_PIN_PRV_CLR_MASK;

    /* MISOB_C : P804 */
//    PFS->P804PFS &= R_PIN_PRV_CLR_MASK;

    /* MOSIB_A : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* MOSIB_B : P609 */
//    PFS->P609PFS &= R_PIN_PRV_CLR_MASK;

    /* MOSIB_C : P805 */
//    PFS->P805PFS &= R_PIN_PRV_CLR_MASK;

    /* RSPCKB_A : P108 */
//    PFS->P108PFS &= R_PIN_PRV_CLR_MASK;

    /* RSPCKB_B : P607 */
//    PFS->P607PFS &= R_PIN_PRV_CLR_MASK;

    /* RSPCKB_C : P801 */
//    PFS->P801PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB0_A : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB0_B : P610 */
//    PFS->P610PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB0_C : P800 */
//    PFS->P800PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB0_D : P603 */
//    PFS->P603PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB1_A : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB1_B : P605 */
//    PFS->P605PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB1_C : P802 */
//    PFS->P802PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB2_A : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB2_B : P606 */
//    PFS->P606PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB2_C : P803 */
//    PFS->P803PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB3_A : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB3_B : P604 */
//    PFS->P604PFS &= R_PIN_PRV_CLR_MASK;

    /* SSLB3_C : P807 */
//    PFS->P807PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RSPI_Pinclr_CH1() */

/**************************************************************************//**
* @brief This function sets Pin of QSPI.
*******************************************************************************/
/* Function Name : R_QSPI_Pinset */
void R_QSPI_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* QSPCLK : P602 */
//    PFS->P602PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P602PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P602PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P602PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P602PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QSPCLK : P109 */
//    PFS->P109PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P109PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P109PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P109PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P109PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QSPCLK : P801 */
//    PFS->P801PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P801PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P801PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P801PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P801PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QSSL : P601 */
//    PFS->P601PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P601PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P601PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P601PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P601PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QSSL : P108 */
//    PFS->P108PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P108PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P108PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P108PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P108PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QSSL : P800 */
//    PFS->P800PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P800PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P800PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P800PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P800PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO0 : P500 */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P500PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO0 : P113 */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P113PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P113PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO0 : P805 */
//    PFS->P805PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P805PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P805PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P805PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P805PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO01 : P015 */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P015PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P015PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P015PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO01 : P112 */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P112PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P112PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO01 : P804 */
//    PFS->P804PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P804PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P804PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P804PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P804PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO2 : P014 */
//    PFS->P014PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P014PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P014PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P014PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P014PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO2 : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO2 : P803 */
//    PFS->P803PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P803PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P803PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P803PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P803PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO3 : P013 */
//    PFS->P013PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P013PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P013PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P013PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P013PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO3 : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* QIO3 : P802 */
//    PFS->P802PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P802PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P802PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P802PFS_b.PSEL = R_PIN_PRV_QSPI_PSEL;
//    PFS->P802PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_QSPI_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of QSPI.
*******************************************************************************/
/* Function Name : R_QSPI_Pinclr */
void R_QSPI_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* QSPCLK : P602 */
//    PFS->P602PFS &= R_PIN_PRV_CLR_MASK;

    /* QSPCLK : P109 */
//    PFS->P109PFS &= R_PIN_PRV_CLR_MASK;

    /* QSPCLK : P801 */
//    PFS->P801PFS &= R_PIN_PRV_CLR_MASK;

    /* QSSL : P601 */
//    PFS->P601PFS &= R_PIN_PRV_CLR_MASK;

    /* QSSL : P108 */
//    PFS->P108PFS &= R_PIN_PRV_CLR_MASK;

    /* QSSL : P800 */
//    PFS->P800PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO0 : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO0 : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO0 : P805 */
//    PFS->P805PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO01 : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO01 : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO01 : P804 */
//    PFS->P804PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO2 : P014 */
//    PFS->P014PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO2 : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO2 : P803 */
//    PFS->P803PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO3 : P013 */
//    PFS->P013PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO3 : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* QIO3 : P802 */
//    PFS->P802PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_QSPI_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of RIIC0.
*******************************************************************************/
/* Function Name : R_RIIC_Pinset_CH0 */
void R_RIIC_Pinset_CH0(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* SCL0 : P810 */    
//    PFS->P810PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P810PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P810PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P810PFS_b.DSCR = 0x2U;  /* When using RIIC : DSCR = 10b */
//    PFS->P810PFS_b.PSEL = R_PIN_PRV_RIIC_PSEL;
//    PFS->P810PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SDA0 : P809 */     
//    PFS->P809PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P809PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P809PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P809PFS_b.DSCR = 0x2U;  /* When using RIIC : DSCR = 10b */
//    PFS->P809PFS_b.PSEL = R_PIN_PRV_RIIC_PSEL;
//    PFS->P809PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_RIIC_Pinset_CH0() */

/**************************************************************************//**
* @brief This function clears the pin setting of RIIC0.
*******************************************************************************/
/* Function Name : R_RIIC_Pinclr_CH0 */
void R_RIIC_Pinclr_CH0(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* SCL0 : P810 */    
//    PFS->P810PFS &= R_PIN_PRV_CLR_MASK;

    /* SDA0 : P809 */     
//    PFS->P809PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RIIC_Pinclr_CH0() */


/**************************************************************************//**
* @brief This function sets Pin of RIIC1.
*******************************************************************************/
/* Function Name : R_RIIC_Pinset_CH1 */
void R_RIIC_Pinset_CH1(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* SCL1 : P701 */     
//    PFS->P701PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P701PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P701PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P701PFS_b.DSCR = 0x2U;  /* When using RIIC : DSCR = 10b */
//    PFS->P701PFS_b.PSEL = R_PIN_PRV_RIIC_PSEL;
//    PFS->P701PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* SDA1 : P700 */     
//    PFS->P700PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P700PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P700PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P700PFS_b.DSCR = 0x2U;  /* When using RIIC : DSCR = 10b */
//    PFS->P700PFS_b.PSEL = R_PIN_PRV_RIIC_PSEL;
//    PFS->P700PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_RIIC_Pinset_CH1() */

/**************************************************************************//**
* @brief This function clears the pin setting of RIIC1.
*******************************************************************************/
/* Function Name : R_RIIC_Pinclr_CH1 */
void R_RIIC_Pinclr_CH1(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* SCL1 : P701 */     
//    PFS->P701PFS &= R_PIN_PRV_CLR_MASK;

    /* SCL1 : P700 */     
//    PFS->P700PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RIIC_Pinclr_CH1() */

/**************************************************************************//**
* @brief This function sets Pin of CAC.
*******************************************************************************/
/* Function Name : R_CAC_Pinset */
void R_CAC_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CACREF : P202 */ 
//    PFS->P202PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P202PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P202PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P202PFS_b.PSEL = R_PIN_PRV_CAC_PSEL;
//    PFS->P202PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CACREF : P810 */ 
//    PFS->P810PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P810PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P810PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P810PFS_b.PSEL = R_PIN_PRV_CAC_PSEL;
//    PFS->P810PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_CAC_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of CAC.
*******************************************************************************/
/* Function Name : R_CAC_Pinclr */
void R_CAC_Pinclr(void) // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CACREF : P202 */ 
//    PFS->P202PFS &= R_PIN_PRV_CLR_MASK;

    /* CACREF : P810 */ 
//    PFS->P810PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_CAC_Pinclr() */

/**************************************************************************//**
* @brief This function sets Pin of S14AD.
*******************************************************************************/
/* Function Name : R_S14AD_Pinset */
void R_S14AD_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* AN000 : P000 */
//    PFS->P000PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P000PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P000PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P000PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P000PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN001 : P001 */
//    PFS->P001PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P001PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P001PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P001PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P001PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN002 : P002 */
//    PFS->P002PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P002PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P002PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P002PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P002PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN003 : P003 */
//    PFS->P003PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P003PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P003PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P003PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P003PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN004 : P004 */
//    PFS->P004PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P004PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P004PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P004PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P004PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN005 : P005 */
//    PFS->P005PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P005PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P005PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P005PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P005PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN006 : P006 */
//    PFS->P006PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P006PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P006PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P006PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P006PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN016 : P010 */
//    PFS->P010PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P010PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P010PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P010PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P010PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN017 : P011 */
//    PFS->P011PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P011PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P011PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P011PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P011PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN020 : P014 */
//    PFS->P014PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P014PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P014PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P014PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P014PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN021 : P015 */
//    PFS->P015PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P015PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P015PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P015PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P015PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN022 : P500 */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P500PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN023 : P501 */
//    PFS->P501PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P501PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P501PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P501PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P501PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN024 : P502 */
//    PFS->P502PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P502PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P502PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P502PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P502PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN025 : P503 */
//    PFS->P503PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P503PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P503PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P503PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P503PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN026 : P504 */
//    PFS->P504PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P504PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P504PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P504PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P504PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN027 : P505 */
//    PFS->P505PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P505PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P505PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P505PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P505PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* AN028 : P506 */
//    PFS->P506PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P506PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P506PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P506PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P506PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */


    /* ADTRG0 : P204 */
//    PFS->P204PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P204PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P204PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P204PFS_b.PSEL = R_PIN_PRV_S14AD_PSEL;
//    PFS->P204PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* ADTRG0 : P500 */
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P500PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P500PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P500PFS_b.PSEL = R_PIN_PRV_S14AD_PSEL;
//    PFS->P500PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */


    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_S14AD_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of S14AD.
*******************************************************************************/
/* Function Name : R_S14AD_Pinclr */
void R_S14AD_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* AN000 : P000 */
//    PFS->P000PFS &= R_PIN_PRV_CLR_MASK;

    /* AN001 : P001 */
//    PFS->P001PFS &= R_PIN_PRV_CLR_MASK;

    /* AN002 : P002 */
//    PFS->P002PFS &= R_PIN_PRV_CLR_MASK;

    /* AN003 : P003 */
//    PFS->P003PFS &= R_PIN_PRV_CLR_MASK;

    /* AN004 : P004 */
//    PFS->P004PFS &= R_PIN_PRV_CLR_MASK;

    /* AN005 : P005 */
//    PFS->P005PFS &= R_PIN_PRV_CLR_MASK;

    /* AN006 : P006 */
//    PFS->P006PFS &= R_PIN_PRV_CLR_MASK;

    /* AN016 : P010 */
//    PFS->P010PFS &= R_PIN_PRV_CLR_MASK;

    /* AN017 : P011 */
//    PFS->P011PFS &= R_PIN_PRV_CLR_MASK;

    /* AN020 : P014 */
//    PFS->P014PFS &= R_PIN_PRV_CLR_MASK;

    /* AN021 : P015 */
//    PFS->P015PFS &= R_PIN_PRV_CLR_MASK;

    /* AN022 : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;

    /* AN023 : P501 */
//    PFS->P501PFS &= R_PIN_PRV_CLR_MASK;

    /* AN024 : P502 */
//    PFS->P502PFS &= R_PIN_PRV_CLR_MASK;

    /* AN025 : P503 */
//    PFS->P503PFS &= R_PIN_PRV_CLR_MASK;

    /* AN026 : P504 */
//    PFS->P504PFS &= R_PIN_PRV_CLR_MASK;

    /* AN027 : P505 */
//    PFS->P505PFS &= R_PIN_PRV_CLR_MASK;

    /* AN028 : P506 */
//    PFS->P506PFS &= R_PIN_PRV_CLR_MASK;

    /* ADTRG0 : P204 */
//    PFS->P204PFS &= R_PIN_PRV_CLR_MASK;

    /* ADTRG0 : P500 */
//    PFS->P500PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_S14AD_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of R12DA.
*******************************************************************************/
/* Function Name : R_R12DA_Pinset */
void R_R12DA_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* DA0 : P007 */
//    PFS->P007PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P007PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P007PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P007PFS_b.PDR  = 0U;  /* 0: Input port,  1: Output port */
//    PFS->P007PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */


    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_R12DA_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of R12DA.
*******************************************************************************/
/* Function Name : R_R12DA_Pinclr */
void R_R12DA_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* DA0 : P007 */
//    PFS->P007PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_R12DA_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of ACMP.
*******************************************************************************/
/* Function Name : R_ACMP_Pinset */
void R_ACMP_Pinset(void) // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CMPIN : P008 */
//    PFS->P008PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P008PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P008PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P008PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* CMPREF : P009 */
//    PFS->P009PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P009PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P009PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P009PFS_b.ASEL = 1U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */

    /* VCOUT : P106 */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P106PFS_b.PSEL = R_PIN_PRV_ACMP_PSEL;
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* VCOUT : P807 */
//    PFS->P807PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P807PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P807PFS_b.PSEL = R_PIN_PRV_ACMP_PSEL;
//    PFS->P807PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P807PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_ACMP_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of ACMP.
*******************************************************************************/
/* Function Name : R_ACMP_Pinclr */
void R_ACMP_Pinclr(void) // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CMPIN : P008 */
//    PFS->P008PFS &= R_PIN_PRV_CLR_MASK;

    /* CMPREF : P009 */
//    PFS->P009PFS &= R_PIN_PRV_CLR_MASK;

    /* VCOUT : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* VCOUT : P807 */
//    PFS->P807PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_ACMP_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of MLCD.
*******************************************************************************/
/* Function Name : R_MLCD_Pinset */
void R_MLCD_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* MLCD_DEN : P110 */
//    PFS->P110PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P110PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P110PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P110PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P110PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_ENBG : P108 */
//    PFS->P108PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P108PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P108PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P108PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P108PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_ENBS : P109 */
//    PFS->P109PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P109PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P109PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P109PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P109PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SCLK : P111 */
//    PFS->P111PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P111PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P111PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P111PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P111PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI0 : P107 */
//    PFS->P107PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P107PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P107PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P107PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P107PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI1 : P106 */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P106PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P106PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI2 : P105 */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P105PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P105PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI3 : P104 */
//    PFS->P104PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P104PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P104PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P104PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P104PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI4 : P103 */
//    PFS->P103PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P103PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P103PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P103PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P103PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI5 : P102 */
//    PFS->P102PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P102PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P102PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P102PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P102PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI6 : P101 */
//    PFS->P101PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P101PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P101PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P101PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P101PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_SI7 : P100 */
//    PFS->P100PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P100PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P100PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P100PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P100PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_VCOM : P113 */
//    PFS->P113PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P113PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P113PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P113PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P113PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* MLCD_XRST : P112 */
//    PFS->P112PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P112PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P112PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P112PFS_b.PSEL = R_PIN_PRV_MLCD_PSEL;
//    PFS->P112PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_MLCD_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of MLCD.
*******************************************************************************/
/* Function Name : R_MLCD_Pinclr */
void R_MLCD_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* MLCD_DEN : P110 */
//    PFS->P110PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_ENBG : P108 */
//    PFS->P108PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_ENBS : P109 */
//    PFS->P109PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SCLK : P111 */
//    PFS->P111PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI0 : P107 */
//    PFS->P107PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI1 : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI2 : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI3 : P104 */
//    PFS->P104PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI4 : P103 */
//    PFS->P103PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI5 : P102 */
//    PFS->P102PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI6 : P101 */
//    PFS->P101PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_SI7 : P100 */
//    PFS->P100PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_VCOM : P113 */
//    PFS->P113PFS &= R_PIN_PRV_CLR_MASK;

    /* MLCD_XRST : P112 */
//    PFS->P112PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_MLCD_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of KINT.
*******************************************************************************/
/* Function Name : R_KINT_Pinset */
void R_KINT_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming") @suppress("Function length")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* KRM00 : P100 */
//    PFS->P100PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P100PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P100PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P100PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P100PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM00 : P507 */
//    PFS->P507PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P507PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P507PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P507PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P507PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* KRM01 : P101 */
//    PFS->P101PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P101PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P101PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P101PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P101PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM01 : P509 */
//    PFS->P509PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P509PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P509PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P509PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P509PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* KRM02 : P102 */
//    PFS->P102PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P102PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P102PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P102PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P102PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* KRM02 : P510 */
//    PFS->P510PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P510PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P510PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P510PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P510PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM03 : P103 */
//    PFS->P103PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P103PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P103PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P103PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P103PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM03 : P511 */
//    PFS->P511PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P511PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P511PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P511PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P511PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* KRM04 : P104 */
//    PFS->P104PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P104PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P104PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P104PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P104PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* KRM04 : P303 */
//    PFS->P303PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P303PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P303PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P303PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P303PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM05 : P105 */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P105PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P105PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM05 : P304 */
//    PFS->P304PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P304PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P304PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P304PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P304PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM06 : P106 */
//    PFS->P106PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P106PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P106PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P106PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P106PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM06 : P407 */
//    PFS->P407PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P407PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P407PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P407PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P407PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM07 : P107 */
//    PFS->P107PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P107PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P107PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P107PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P107PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* KRM07 : P408 */
//    PFS->P408PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P408PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P408PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P408PFS_b.PSEL = R_PIN_PRV_KINT_PSEL;
//    PFS->P408PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_KINT_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of KINT.
*******************************************************************************/
/* Function Name : R_KINT_Pinclr */
void R_KINT_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* KRM00 : P100 */
//    PFS->P100PFS &= R_PIN_PRV_CLR_MASK;
    
    /* KRM00 : P507 */
//    PFS->P507PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM01 : P101 */
//    PFS->P101PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM01 : P509 */
//    PFS->P509PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM02 : P102 */
//    PFS->P102PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM02 : P510 */
//    PFS->P510PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM03 : P103 */
//    PFS->P103PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM03 : P511 */
//    PFS->P511PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM04 : P104 */
//    PFS->P104PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM04 : P303 */
//    PFS->P303PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM05 : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM05 : P304 */
//    PFS->P304PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM06 : P106 */
//    PFS->P106PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM06 : P407 */
//    PFS->P407PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM07 : P107 */
//    PFS->P107PFS &= R_PIN_PRV_CLR_MASK;

    /* KRM07 : P408 */
//    PFS->P408PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_KINT_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of USB .
*******************************************************************************/
/* Function Name : R_USB_Pinset */
void R_USB_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* USB_EXICEN : P105 */
//    PFS->P105PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P105PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P105PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P105PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P105PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_ID : P207 */
//    PFS->P207PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P207PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P207PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P207PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P207PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_ID : P507 */
//    PFS->P507PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P507PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P507PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P507PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P507PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_OVRCURA : P203 */
//    PFS->P203PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P203PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P203PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P203PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P203PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_OVRCURA : P503 */
//    PFS->P503PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P503PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P503PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P503PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P503PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_OVRCURB : P202 */
//    PFS->P202PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P202PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P202PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P202PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P202PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_OVRCURB : P509 */
//    PFS->P509PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P509PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P509PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P509PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P509PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_VBUS : P204 */
//    PFS->P204PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P204PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P204PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P204PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P204PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_VBUSEN : P701 */
//    PFS->P701PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P701PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P701PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P701PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P701PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* USB_VBUSEN : P504 */
//    PFS->P504PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P504PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P504PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P504PFS_b.PSEL = R_PIN_PRV_USB_PSEL;
//    PFS->P504PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}/* End of function R_USB_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of USB.
*******************************************************************************/
/* Function Name : R_USB_Pinclr */
void R_USB_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* USB_EXICEN : P105 */
//    PFS->P105PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_ID : P207 */
//    PFS->P207PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_ID : P507 */
//    PFS->P507PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_OVRCURA : P203 */
//    PFS->P203PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_OVRCURA : P503 */
//    PFS->P503PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_OVRCURB : P202 */
//    PFS->P202PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_OVRCURB : P509 */
//    PFS->P509PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_VBUS : P204 */
//    PFS->P204PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_VBUSEN : P701 */
//    PFS->P701PFS &= R_PIN_PRV_CLR_MASK;

    /* USB_VBUSEN : P504 */
//    PFS->P504PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_USB_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of RTC.
*******************************************************************************/
/* Function Name : R_RTC_Pinset */
void R_RTC_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* RTCIC0 : P604 */
//    PFS->P604PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P604PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P604PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P604PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P604PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RTCIC0 : P204 */
//    PFS->P204PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P204PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P204PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P204PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P204PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RTCIC1 : P603 */
//    PFS->P603PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P603PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P603PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P603PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P603PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RTCIC1 : 203 */
//    PFS->P203PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P203PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P203PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P203PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P203PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RTCIC2 : P602 */
//    PFS->P602PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P602PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P602PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P602PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P602PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RTCIC2 : P701 */
//    PFS->P701PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P701PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P701PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P701PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P701PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RTCOUT : P301 */
//    PFS->P301PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P301PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P301PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P301PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P301PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* RTCOUT : P202 */
//    PFS->P202PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P202PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P202PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P202PFS_b.PSEL = R_PIN_PRV_RTC_PSEL;
//    PFS->P202PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RTC_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of RTC.
*******************************************************************************/
/* Function Name : R_RTC_Pinclr */
void R_RTC_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* RTCIC0 : P604 */
//    PFS->P604PFS &= R_PIN_PRV_CLR_MASK;

    /* RTCIC0 : P204 */
//    PFS->P204PFS &= R_PIN_PRV_CLR_MASK;

     /*RTCIC1 : P603 */
//    PFS->P603PFS &= R_PIN_PRV_CLR_MASK;

    /* RTCIC1 : 203 */
//    PFS->P203PFS &= R_PIN_PRV_CLR_MASK;

    /* RTCIC2 : P602 */
//    PFS->P602PFS &= R_PIN_PRV_CLR_MASK;

    /* RTCIC2 : P701 */
//    PFS->P701PFS &= R_PIN_PRV_CLR_MASK;

    /* RTCOUT : P301 */
//    PFS->P301PFS &= R_PIN_PRV_CLR_MASK;

    /* RTCOUT : P202 */
//    PFS->P202PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_RTC_Pinclr() */

/**************************************************************************//**
* @brief This function sets Pin of LPG.
*******************************************************************************/
/* Function Name : R_LPG_Pinset */
void R_LPG_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* LPGOUT : P600 */
//    PFS->P600PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P600PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P600PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P600PFS_b.PSEL = R_PIN_PRV_LPG_PSEL;
//    PFS->P600PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_LPG_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of LPG.
*******************************************************************************/
/* Function Name : R_LPG_Pinclr */
void R_LPG_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

   /* LPGOUT : P600 */
   PFS->P600PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_LPG_Pinclr() */


/**************************************************************************//**
* @brief This function sets Pin of TMR.
*******************************************************************************/
/* Function Name : R_TMR_Pinset */
void R_TMR_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* TMCI0 : P411 */
//    PFS->P411PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P411PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P411PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P411PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P411PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TMCI0 : P302 */
//    PFS->P302PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P302PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P302PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P302PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P302PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* TMCI1 : P704 */
//    PFS->P704PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P704PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P704PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P704PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P704PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TMO0 : P200 */
//    PFS->P200PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P200PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P200PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P200PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P200PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TMO0 : P300 */
//    PFS->P300PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P300PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P300PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P300PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P300PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* TMO1 : P700 */
//    PFS->P700PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P700PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P700PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P700PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P700PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TMRI0 : P201 */
//    PFS->P201PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P201PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P201PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P201PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P201PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    
    /* TMRI0 : P301 */
//    PFS->P301PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P301PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P301PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P301PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P301PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* TMRI1 : P701 */
//    PFS->P701PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P701PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P701PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P701PFS_b.PSEL = R_PIN_PRV_TMR_PSEL;
//    PFS->P701PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_TMR_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of TMR.
*******************************************************************************/
/* Function Name : R_TMR_Pinclr */
void R_TMR_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* TMCI0 : P411 */
//    PFS->P411PFS &= R_PIN_PRV_CLR_MASK;

    /* TMCI0 : P302 */
//    PFS->P302PFS &= R_PIN_PRV_CLR_MASK;

    /* TMCI1 : P704 */
//    PFS->P704PFS &= R_PIN_PRV_CLR_MASK;

    /* TMO0 : P200 */
//    PFS->P200PFS &= R_PIN_PRV_CLR_MASK;

    /* TMO0 : P300 */
//    PFS->P300PFS &= R_PIN_PRV_CLR_MASK;

    /* TMO1 : P700 */
//    PFS->P700PFS &= R_PIN_PRV_CLR_MASK;

    /* TMRI0 : P201 */
//    PFS->P201PFS &= R_PIN_PRV_CLR_MASK;

    /* TMRI0 : P301 */
//    PFS->P301PFS &= R_PIN_PRV_CLR_MASK;

    /* TMRI1 : P701 */
//    PFS->P701PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_TMR_Pinclr() */

/**************************************************************************//**
* @brief This function sets Pin of CLKOUT.
*******************************************************************************/
/* Function Name : R_CLKOUT_Pinset */
void R_CLKOUT_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CLKOUT : P011 */
//    PFS->P011PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P011PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P011PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P011PFS_b.PSEL = R_PIN_PRV_CLKOUT_PSEL;
//    PFS->P011PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CLKOUT32 : P411 */
//    PFS->P411PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P411PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P411PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P411PFS_b.PSEL = R_PIN_PRV_CLKOUT_PSEL;
//    PFS->P411PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CLKOUT32 : P409 */
//    PFS->P409PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P409PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P409PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P409PFS_b.PSEL = R_PIN_PRV_CLKOUT_PSEL;
//    PFS->P409PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* CLKOUT32 : P610 */
//    PFS->P610PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P610PFS_b.ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
//    PFS->P610PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P610PFS_b.PSEL = R_PIN_PRV_CLKOUT_PSEL;
//    PFS->P610PFS_b.PMR  = 1U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_CLKOUT_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of CLKOUT.
*******************************************************************************/
/* Function Name : R_CLKOUT_Pinclr */
void R_CLKOUT_Pinclr(void) // @suppress("Function declaration") @suppress("Source file naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* CLKOUT : P011 */
//    PFS->P011PFS &= R_PIN_PRV_CLR_MASK;

    /* CLKOUT32 : P411 */
//    PFS->P411PFS &= R_PIN_PRV_CLR_MASK;

    /* CLKOUT32 : P409 */
//    PFS->P409PFS &= R_PIN_PRV_CLR_MASK;

    /* CLKOUT32 : P610 */
//    PFS->P610PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_CLKOUT_Pinclr() */

/**************************************************************************//**
* @brief This function sets Pin of LED.
*******************************************************************************/
/* Function Name : R_LED_Pinset */
void R_LED_Pinset(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* LED1 : P514 */
//    PFS->P514PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P514PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P514PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P514PFS_b.ASEL = 1U;

    /* LED2 : P513 */
//    PFS->P513PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P513PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P513PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P513PFS_b.ASEL = 1U;

    /* LED3 : P512 */
//    PFS->P512PFS_b.ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
//    PFS->P512PFS_b.PSEL = 0U;  /* 0: Do not assign Peripheral */
//    PFS->P512PFS_b.PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
//    PFS->P512PFS_b.ASEL = 1U;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_LED_Pinset() */

/**************************************************************************//**
* @brief This function clears the pin setting of LED.
*******************************************************************************/
/* Function Name : R_LED_Pinclr */
void R_LED_Pinclr(void)  // @suppress("Source file naming") @suppress("API function naming")
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /* LED1 : P514 */
//    PFS->P514PFS &= R_PIN_PRV_CLR_MASK;

    /* LED2 : P513 */
//    PFS->P513PFS &= R_PIN_PRV_CLR_MASK;

    /* LED3 : P512 */
//    PFS->P512PFS &= R_PIN_PRV_CLR_MASK;

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}/* End of function R_LED_Pinclr() */


/* End of file (pin.c) */

/********************************************************************/
/* CCC has some input/output pin.                                   */
/* But these do not need setting of I/O register for PFS register.  */
/* So, there is no function for those.                              */
/********************************************************************/

/******************************************************************************************************************//**
 * @} (end addtogroup grp_device_pin_setting)
 *********************************************************************************************************************/
