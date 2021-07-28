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
* File Name    : r_usart_cfg.h
* Version      : 1.00
* Description  : CMSIS-Driver for USART
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 01.04.2020 1.00     Fixed not to include header file when DMA is not used.
**********************************************************************************************************************/

#ifndef R_USART_CFG_H
#define R_USART_CFG_H
/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_usart
 * @{
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
#define USART_MODULATION_FUNC   (0)                 ///< Bit Modulation function enable(1)/disable(0)

/******************************************************************************************************************//**
 * @name SCIn_TRANSMIT_CONTROL/SCIn_RECEIVE_CONTROL
 *       Select whether to use interrupts or DMAC (DTC) for transmission and reception control.@n@n
 *       SCI_USED_INTERRUPT: Interrupt is used for transmission / reception control.@n
 *       SCI_USED_DMACn: DMACn is used for transmission / reception control.@n
 *       SCI_USED_DTC: DTC is used for  transmission / reception control.@n
 *          [Example]@n
 *          Example of definition when interrupt is used for transmission/reception of SCI0 
 *          and DMAC is used for transmission/reception of SCI1
 *          @code
 *          ...
 *          #define SCI0_TRANSMIT_CONTROL    SCI_USED_INTERRUPT
 *          #define SCI0_RECEIVE_CONTROL     SCI_USED_INTERRUPT
 *          #define SCI1_TRANSMIT_CONTROL    SCI_USED_DMAC0
 *          #define SCI1_RECEIVE_CONTROL     SCI_USED_DMAC1
 *
 *          ...
 *          @endcode
 *          [Note]@n
 *          Do not use the same DMAC channel for multiple SCI transmission or reception control
 *********************************************************************************************************************/
/* @{ */
#define SCI_USED_INTERRUPT  (0)                        ///< Use Interrupt for transmission and reception control of SCI
#define SCI_USED_DMAC0      (1<<0)                     ///< Use DMAC0 for transmission and reception control of SCI
#define SCI_USED_DMAC1      (1<<1)                     ///< Use DMAC1 for transmission and reception control of SCI
#define SCI_USED_DMAC2      (1<<2)                     ///< Use DMAC2 for transmission and reception control of SCI
#define SCI_USED_DMAC3      (1<<3)                     ///< Use DMAC3 for transmission and reception control of SCI
#define SCI_USED_DTC        (1<<15)                    ///< Use DTC for transmission and reception control of SCI


#define SCI0_TRANSMIT_CONTROL   SCI_USED_INTERRUPT     ///< SCI0 transmit control
#define SCI0_RECEIVE_CONTROL    SCI_USED_INTERRUPT     ///< SCI0 receive control

#define SCI1_TRANSMIT_CONTROL   SCI_USED_INTERRUPT     ///< SCI1 transmit control
#define SCI1_RECEIVE_CONTROL    SCI_USED_INTERRUPT     ///< SCI1 receive control

#define SCI2_TRANSMIT_CONTROL   SCI_USED_INTERRUPT     ///< SCI2 transmit control
#define SCI2_RECEIVE_CONTROL    SCI_USED_INTERRUPT     ///< SCI2 receive control

#define SCI3_TRANSMIT_CONTROL   SCI_USED_INTERRUPT     ///< SCI3 transmit control
#define SCI3_RECEIVE_CONTROL    SCI_USED_INTERRUPT     ///< SCI3 receive control

#define SCI4_TRANSMIT_CONTROL   SCI_USED_INTERRUPT     ///< SCI4 transmit control
#define SCI4_RECEIVE_CONTROL    SCI_USED_INTERRUPT     ///< SCI4 receive control

#define SCI5_TRANSMIT_CONTROL   SCI_USED_INTERRUPT     ///< SCI5 transmit control
#define SCI5_RECEIVE_CONTROL    SCI_USED_INTERRUPT     ///< SCI5 receive control

#define SCI9_TRANSMIT_CONTROL   SCI_USED_INTERRUPT     ///< SCI9 transmit control
#define SCI9_RECEIVE_CONTROL    SCI_USED_INTERRUPT     ///< SCI9 receive control
/* @} */

#define SCI_CHECK_TDRE_TIMEOUT  (10)                ///< TDRE check timeout value at transmission
                                                    ///  (Only when using DMAC at transmission)
 
#define SCI0_TXI_PRIORITY       (3)                 ///< TXI0 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI0_RXI_PRIORITY       (3)                 ///< RXI0 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI0_ERI_PRIORITY       (3)                 ///< ERI0 priority value(set to 0 to 3, 0 is highest priority.)

#define SCI1_TXI_PRIORITY       (3)                 ///< TXI1 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI1_RXI_PRIORITY       (3)                 ///< RXI1 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI1_ERI_PRIORITY       (3)                 ///< ERI1 priority value(set to 0 to 3, 0 is highest priority.)

#define SCI2_TXI_PRIORITY       (3)                 ///< TXI2 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI2_RXI_PRIORITY       (3)                 ///< RXI2 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI2_ERI_PRIORITY       (3)                 ///< ERI2 priority value(set to 0 to 3, 0 is highest priority.)

#define SCI3_TXI_PRIORITY       (3)                 ///< TXI3 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI3_RXI_PRIORITY       (3)                 ///< RXI3 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI3_ERI_PRIORITY       (3)                 ///< ERI3 priority value(set to 0 to 3, 0 is highest priority.)

#define SCI4_TXI_PRIORITY       (3)                 ///< TXI4 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI4_RXI_PRIORITY       (3)                 ///< RXI4 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI4_ERI_PRIORITY       (3)                 ///< ERI4 priority value(set to 0 to 3, 0 is highest priority.)

#define SCI5_TXI_PRIORITY       (3)                 ///< TXI5 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI5_RXI_PRIORITY       (3)                 ///< RXI5 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI5_ERI_PRIORITY       (3)                 ///< ERI5 priority value(set to 0 to 3, 0 is highest priority.)

#define SCI9_TXI_PRIORITY       (3)                 ///< TXI9 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI9_RXI_PRIORITY       (3)                 ///< RXI9 priority value(set to 0 to 3, 0 is highest priority.)
#define SCI9_ERI_PRIORITY       (3)                 ///< ERI9 priority value(set to 0 to 3, 0 is highest priority.)


/* When using GetModemStatus function, please define USART_CTS_PORT and USART_CTS_PIN. */
//#define USART0_CTS_PORT       (PORT0->PIDR)       ///< Used CTS0 port 
#define USART0_CTS_PIN          0                   ///< Used CTS0 pin number
//#define USART1_CTS_PORT       (PORT0->PIDR)       ///< Used CTS1 port 
#define USART1_CTS_PIN          0                   ///< Used CTS1 pin number
//#define USART2_CTS_PORT       (PORT0->PIDR)       ///< Used CTS2 port 
#define USART2_CTS_PIN          0                   ///< Used CTS2 pin number
//#define USART3_CTS_PORT       (PORT0->PIDR)       ///< Used CTS3 port 
#define USART3_CTS_PIN          0                   ///< Used CTS3 pin number
//#define USART4_CTS_PORT       (PORT0->PIDR)       ///< Used CTS4 port 
#define USART4_CTS_PIN          0                   ///< Used CTS4 pin number
//#define USART5_CTS_PORT       (PORT0->PIDR)       ///< Used CTS5 port 
#define USART5_CTS_PIN          0                   ///< Used CTS5 pin number
//#define USART9_CTS_PORT       (PORT0->PIDR)       ///< Used CTS9 port 
#define USART9_CTS_PIN          0                   ///< Used CTS9 pin number

/* When using SetModemControl function, please define USART_RTS_PORT and USART_RTS_PIN. */
//#define USART0_RTS_PORT       (PORT0->PODR)       ///< Used RTS0 port 
#define USART0_RTS_PIN          0                   ///< Used RTS0 pin number
//#define USART1_RTS_PORT       (PORT0->PODR)       ///< Used RTS1 port 
#define USART1_RTS_PIN          0                   ///< Used RTS1 pin number
//#define USART2_RTS_PORT       (PORT0->PODR)       ///< Used RTS2 port 
#define USART2_RTS_PIN          0                   ///< Used RTS2 pin number
//#define USART3_RTS_PORT       (PORT0->PODR)       ///< Used RTS3 port 
#define USART3_RTS_PIN          0                   ///< Used RTS3 pin number
//#define USART4_RTS_PORT       (PORT0->PODR)       ///< Used RTS4 port 
#define USART4_RTS_PIN          0                   ///< Used RTS4 pin number
//#define USART5_RTS_PORT       (PORT0->PODR)       ///< Used RTS5 port 
#define USART5_RTS_PIN          0                   ///< Used RTS5 pin number
//#define USART9_RTS_PORT       (PORT0->PODR)       ///< Used RTS9 port 
#define USART9_RTS_PIN          0                   ///< Used RTS9 pin number


/******************************************************************************************************************//**
 * @name R_USART_API_LOCATION_CONFIG
 *       Definition of R_USART API location configuration
 *       Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
 *********************************************************************************************************************/
/* @{ */
#define USART_CFG_SECTION_ARM_USART_GET_VERSION       (SYSTEM_SECTION_CODE)        ///< ARM_USART_GetVersion() section
#define USART_CFG_SECTION_ARM_USART_GET_CAPABILITES   (SYSTEM_SECTION_CODE)        ///< ARM_USART_GetCapabilities() section
#define USART_CFG_SECTION_ARM_USART_INITIALIZE        (SYSTEM_SECTION_CODE)        ///< ARM_USART_Initialize() section
#define USART_CFG_SECTION_ARM_USART_UNINITIALIZE      (SYSTEM_SECTION_CODE)        ///< ARM_USART_Uninitialize() section
#define USART_CFG_SECTION_ARM_USART_POWER_CONTROL     (SYSTEM_SECTION_CODE)        ///< ARM_USART_PowerControl() section
#define USART_CFG_SECTION_ARM_USART_SEND              (SYSTEM_SECTION_CODE)        ///< ARM_USART_Send() section
#define USART_CFG_SECTION_ARM_USART_RECEIVE           (SYSTEM_SECTION_CODE)        ///< ARM_USART_Receive() section
#define USART_CFG_SECTION_ARM_USART_TRANSFER          (SYSTEM_SECTION_CODE)        ///< ARM_USART_Transfer() section
#define USART_CFG_SECTION_ARM_USART_GET_TX_COUNT      (SYSTEM_SECTION_CODE)        ///< ARM_USART_GetTxCount() section
#define USART_CFG_SECTION_ARM_USART_GET_RX_COUNT      (SYSTEM_SECTION_CODE)        ///< ARM_USART_GetRxCount() section
#define USART_CFG_SECTION_ARM_USART_CONTROL           (SYSTEM_SECTION_CODE)        ///< ARM_USART_Control() section
#define USART_CFG_SECTION_ARM_USART_GET_STATUS        (SYSTEM_SECTION_CODE)        ///< ARM_USART_GetStatus() section
#define USART_CFG_SECTION_ARM_USART_SET_MODEM_CONTROL (SYSTEM_SECTION_CODE)        ///< ARM_USART_SetModemControl() section
#define USART_CFG_SECTION_ARM_USART_GET_MODEM_STATUS  (SYSTEM_SECTION_CODE)        ///< ARM_USART_GetModemStatus() section
#define USART_CFG_SECTION_TXI_HANDLER                 (SYSTEM_SECTION_RAM_FUNC)    ///< txi_handler() section
#define USART_CFG_SECTION_RXI_HANDLER                 (SYSTEM_SECTION_RAM_FUNC)    ///< rxi_handler() section
#define USART_CFG_SECTION_ERI_HANDLER                 (SYSTEM_SECTION_RAM_FUNC)    ///< eri_handler() section
/* @} */

#define USART_PRV_USED_DMAC_DTC_DRV (SCI0_TRANSMIT_CONTROL | SCI0_RECEIVE_CONTROL | \
                                     SCI1_TRANSMIT_CONTROL | SCI1_RECEIVE_CONTROL | \
                                     SCI2_TRANSMIT_CONTROL | SCI2_RECEIVE_CONTROL | \
                                     SCI3_TRANSMIT_CONTROL | SCI3_RECEIVE_CONTROL | \
                                     SCI4_TRANSMIT_CONTROL | SCI4_RECEIVE_CONTROL | \
                                     SCI5_TRANSMIT_CONTROL | SCI5_RECEIVE_CONTROL | \
                                     SCI9_TRANSMIT_CONTROL | SCI9_RECEIVE_CONTROL )

#define USART_PRV_USED_TX_DMAC_DTC_DRV (SCI0_TRANSMIT_CONTROL | SCI1_TRANSMIT_CONTROL | \
                                        SCI2_TRANSMIT_CONTROL | SCI3_TRANSMIT_CONTROL | \
                                        SCI4_TRANSMIT_CONTROL | SCI5_TRANSMIT_CONTROL | \
                                        SCI9_TRANSMIT_CONTROL )

#define USART_PRV_USED_RX_DMAC_DTC_DRV (SCI0_RECEIVE_CONTROL  | SCI1_RECEIVE_CONTROL  | \
                                        SCI2_RECEIVE_CONTROL  | SCI3_RECEIVE_CONTROL  | \
                                        SCI4_RECEIVE_CONTROL  | SCI5_RECEIVE_CONTROL  | \
                                        SCI9_RECEIVE_CONTROL  )

#define USART_PRV_USED_DMAC_DRV      (USART_PRV_USED_DMAC_DTC_DRV & 0x00FF)
#define USART_PRV_USED_TX_DMAC_DRV   (USART_PRV_USED_TX_DMAC_DTC_DRV & 0x00FF)
#define USART_PRV_USED_RX_DMAC_DRV   (USART_PRV_USED_RX_DMAC_DTC_DRV & 0x00FF)

#define USART_PRV_USED_DTC_DRV       (USART_PRV_USED_DMAC_DTC_DRV & SCI_USED_DTC)
#define USART_PRV_USED_TX_DTC_DRV    (USART_PRV_USED_TX_DMAC_DTC_DRV & SCI_USED_DTC)
#define USART_PRV_USED_RX_DTC_DRV    (USART_PRV_USED_RX_DMAC_DTC_DRV & SCI_USED_DTC)

#ifdef  __cplusplus
}
#endif
/*******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_usart)
 **********************************************************************************************************************/

#endif /** R_USART_CFG_H */

/* End of file (r_usart_cfg.h) */
