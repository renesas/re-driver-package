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
 * Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.    
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name   : r_smip_cfg.h
 * Version      : 0.80
 * Description  : Serial MIP-LCD API.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 02.03.2020 0.80     Addition of LSB / MSB setting switching definition of SPI. 
*                               - Added Definition to select communication format of SPI in r_smip_cfg.h.
***********************************************************************************************************************/

#ifndef R_SMIP_CFG_H
#define R_SMIP_CFG_H
/******************************************************************************************************************//**
 @addtogroup grp_device_hal_smip
 @{
 *********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define SMIP_CFG_PARAM_CHECKING_ENABLE      (1)             /*!< 0:Disable Parameter Check 1:Enable Parameter Check */
#define SMIP_CFG_SCS_WAIT_EN                (1)             /*!< SCS wait time enable(0: no wait, 1: wait) */
#define SMIP_CFG_COMMAND_MAX_BYTE           (2)             /*!< Command bayte max size(byte) */
#define SMIP_CFG_ALL_WRITE_BUFF             (37)            /*!< All write command max size(byte)  */
#define SMIP_CFG_EXTMODE                    (1)             /*!< EXTMODE(0: Low, 1:High) */
#define SMIP_CFG_AGTO_EN                    (0)             /*!< VCOM output selection (using AGTOn pin) @n
                                                                 0: Do not use AGIO pin for VCOM pin output @n
                                                                    (VCOM control is performed by port control) @n
                                                                 1: Use AGROn terminal for VCOM output  @n
                                                                    (Please set output terminal with pin.c) */

#define SMIP_CFG_VCOM_PORT                  (PORT1)         /*!< Select VCOM port */
#define SMIP_CFG_VCOM_PIN                   (6)             /*!< Select VCOM pin */
#define SMIP_CFG_SCS_PORT                   (PORT1)         /*!< Select SCS port */
#define SMIP_CFG_SCS_PIN                    (3)             /*!< Select SCS pin */
#define SMIP_CFG_RST_DISP_PORT              (PORT1)         /*!< Select RST/DISP port */
#define SMIP_CFG_RST_DISP_PIN               (5)             /*!< Select RST/DISP pin */

#define SMIP_CFG_MAKE_LINE_INFO             (1)             /*!< Select sending data  @n
                                                                  0 : When sending, do not add header and footer to data  @n
                                                                  1 : When sending, add header and footer to data */
#define SMIP_CFG_VCOM_TIMER_CH              (0)             /*!< Select AGT channel to use as timer for VCOM ( 0 or 1 ) */
#define SMIP_CFG_VCOM_TIMER_INT_PRIORITY    (3)             /*!< AGTIn interrupt priority value 
                                                                 (set to 0 to 3, 0 is highest priority.) */
#define SMIP_CFG_VCOM_TIMER_CLOCK           (0)             /*!< Select AGT source clock   @n
                                                                  0 : SOSC  @n
                                                                  1 : LOCO */
#define SMIP_CFG_VCOM_TIMER_LPM             (1)             /*!< Selece AGT lowpower mode(AGTMR2.LPM)  @n
                                                                  0 : Normal mode(AGTMR2.LPM=0)  @n
                                                                  1 : Low power mode(AGTMR2.LPM=1) */
#define SMIP_CFG_TIMEOUT                    (2000)          /*!< Communication timeout time(ms) */
#define SMIP_CFG_SPI_BIT_ORDER              (0)             /*!< Select SPI communication bit order setting @n
                                                                  0 : MSB  @n
                                                                  1 : LSB  */

/* JDI LCD Display setting */
#define SMIP_CFG_JDI_COLOR_MODE             (0)             /*!< Color mode for JDI(0: 3bit mode, 1: 4bit mode) */
#define SMIP_CFG_JDI_MODE_SIZE              (6)             /*!< [M] Mode bit size for JDI */
#define SMIP_CFG_JDI_MODE_DMY_SIZE          (0)             /*!< [DMY] DMY bit size for JDI */
#define SMIP_CFG_JDI_ADDR_SIZE              (10)            /*!< [AG] Address bit size for JDI */
#define SMIP_CFG_JDI_DUMMY_SIZE             (16)            /*!< DUMMY data bit size for JDI */
#define SMIP_CFG_JDI_DISP_WIDTH             (176)           /*!< LCD width dot size for JDI */
#define SMIP_CFG_JDI_DISP_LINE              (176)           /*!< LCD Line number for JDI */
#define SMIP_CFG_JDI_VCOM_CLK               (120*10)        /*!< VCOM clock(x0.1Hz) for JDI */
#define SMIP_CFG_JDI_T_MEMORY_INI           (1000)          /*!< Pixel memory initialization time(T2, T5) for JDI */
#define SMIP_CFG_JDI_T_COM_INI              (30)            /*!< COM and latch initialization time(T3, T4, T6) for JDI */
#define SMIP_CFG_JDI_TS_SCS                 (6)             /*!< CSC setup time for JDI */
#define SMIP_CFG_JDI_TH_SCS                 (2)             /*!< CSC hold time for JDI */
#define SMIP_CFG_JDI_TW_SCSL                (6)             /*!< CSC low width for JDI */
#define SMIP_CFG_JDI_FIRST_ADDR             (1)             /*!< First line address for JDI */

/* KYOCERA LCD Display setting */
#define SMIP_CFG_KYOCERA_COLOR_MODE         (0)             /*!< Color mode for Kyocera(0: 3bit mode, 1: 4bit mode) */
#define SMIP_CFG_KYOCERA_MODE_SIZE          (0)             /*!< [M] Mode bit size for Kyocera */
#define SMIP_CFG_KYOCERA_MODE_DMY_SIZE      (0)             /*!< [DMY] DMY bit size for Kyocera */
#define SMIP_CFG_KYOCERA_ADDR_SIZE          (8)             /*!< [AG] Address bit size for Kyocera */
#define SMIP_CFG_KYOCERA_DUMMY_SIZE         (32)            /*!< DUMMY data bit size for Kyocera */
#define SMIP_CFG_KYOCERA_DISP_WIDTH         (256)           /*!< LCD width dot size for Kyocera */
#define SMIP_CFG_KYOCERA_DISP_LINE          (256)           /*!< LCD Line number for Kyocera */
#define SMIP_CFG_KYOCERA_VCOM_CLK           (0.5*10)        /*!< VCOM clock(x0.1Hz) for Kyocera */
#define SMIP_CFG_KYOCERA_T_MEMORY_INI       (1000)          /*!< Pixel memory initialization time(t2) for Kyocera */
#define SMIP_CFG_KYOCERA_T_COM_INI          (1000)          /*!< COM and latch initialization time(t3, t4) for Kyocera */
#define SMIP_CFG_KYOCERA_TS_SCS             (4)             /*!< CSC setup time for Kyocera */
#define SMIP_CFG_KYOCERA_TH_SCS             (4)             /*!< CSC hold time for Kyocera */
#define SMIP_CFG_KYOCERA_TW_SCSL            (10)            /*!< CSC low width for Kyocera */
#define SMIP_CFG_KYOCERA_TS_VCOM            (4)             /*!< VCOM setup time for Kyocera */
#define SMIP_CFG_KYOCERA_TH_VCOM            (1)             /*!< VCOM setup time for Kyocera */
#define SMIP_CFG_KYOCERA_FIRST_ADDR         (0)             /*!< First line address for Kyocera */

/* SHARP LCD Display setting */
#define SMIP_CFG_SHARP_COLOR_MODE           (0)             /*!< Color mode for SHARP(0: 3bit mode, 1: 4bit mode) */
#define SMIP_CFG_SHARP_MODE_SIZE            (3)             /*!< [M] Mode bit size for SHARP */
#define SMIP_CFG_SHARP_MODE_DMY_SIZE        (5)             /*!< [DMY] DMY bit size for SHARP */
#define SMIP_CFG_SHARP_ADDR_SIZE            (8)             /*!< [AG] Address bit size for SHARP */
#define SMIP_CFG_SHARP_DUMMY_SIZE           (16)            /*!< DUMMY data bit size for SHARP */
#define SMIP_CFG_SHARP_DISP_WIDTH           (128)           /*!< LCD width dot size for SHARP */
#define SMIP_CFG_SHARP_DISP_LINE            (128)           /*!< LCD Line number for SHARP */
#define SMIP_CFG_SHARP_VCOM_CLK             (60*10)         /*!< VCOM clock(x0.1Hz) for SHARP */
#define SMIP_CFG_SHARP_T_MEMORY_INI         (0)             /*!< Pixel memory initialization time(T2, T5) for SHARP */
#define SMIP_CFG_SHARP_T_COM_INI            (30)            /*!< COM and latch initialization time(T3, T4, T6) for SHARP */
#define SMIP_CFG_SHARP_TS_SCS               (6)             /*!< CSC setup time for SHARP */
#define SMIP_CFG_SHARP_TH_SCS               (2)             /*!< CSC hold time for SHARP */
#define SMIP_CFG_SHARP_TW_SCSL              (6)             /*!< CSC low width for SHARP */
#define SMIP_CFG_SHARP_FIRST_ADDR           (1)             /*!< First line address for SHARP */


#if SMIP_CFG_VCOM_TIMER_CH >= 2
     #error "ERROR - SMIP_VCOM_TIMER_USED_AGT value is invalid (r_smip_cfg.h)."
#endif



/******************************************************************************************************************//**
 * @name R_SMIP_API_LOCATION_CONFIG
 *       Definition of R_SMIP API location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define SMIP_CFG_SECTION_R_SMIP_GETVERSION          (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_GetVersion() section */
#define SMIP_CFG_SECTION_R_SMIP_OPEN                (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_Open() section */
#define SMIP_CFG_SECTION_R_SMIP_CLOSE               (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_Close() section */
#define SMIP_CFG_SECTION_R_SMIP_POWER_ON            (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_PowerOn() section */
#define SMIP_CFG_SECTION_R_SMIP_POWER_OFF           (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_PowerOff() section */
#define SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_AllZero() section */
#define SMIP_CFG_SECTION_R_SMIP_ALL_ONE             (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_AllOne() section */
#define SMIP_CFG_SECTION_R_SMIP_SEND                (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_Send() section */
#define SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_SendColor() section */
#define SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_SendCommand() section */
#define SMIP_CFG_SECTION_R_SMIP_SUSPEND             (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_Suspend() section */
#define SMIP_CFG_SECTION_R_SMIP_RESUME              (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_Resume() section */
#define SMIP_CFG_SECTION_R_SMIP_RECONFIG_SPI_SPPED  (SYSTEM_SECTION_CODE)       /*!<  R_SMIP_ResetSpiSpeed() section */
#define SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        (SYSTEM_SECTION_RAM_FUNC)   /*!<  r_smip_cb_spi_event() section 
                                                                                      (SPI callback function)  */
#define SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT (SYSTEM_SECTION_RAM_FUNC)   /*!<  r_smip_cb_vcom_timer_event() section
                                                                                      (AGTI callback function) */
/* @} */

#ifdef  __cplusplus
}
#endif

#endif /* R_SMIP_CFG_H */
/*******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_smip)
 **********************************************************************************************************************/
/* End of file (r_smip_cfg.h) */

