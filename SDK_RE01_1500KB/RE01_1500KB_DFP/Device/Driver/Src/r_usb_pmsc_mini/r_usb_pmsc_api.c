/*******************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only
 * intended for use with Renesas products. No other uses are authorized. This
 * software is owned by Renesas Electronics Corporation and is protected under
 * all applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 * LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 * TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 * ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 * ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software
 * and to discontinue the availability of this software. By using this software,
 * you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ******************************************************************************/
/******************************************************************************
 * File Name    : r_usb_pmsc_driver.c
 * Description  : USB Host and Peripheral Interrupt code
 ******************************************************************************/
/******************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 26.04.2019 0.50     Alpha Version
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usb_basic_mini_api.h"
#include "r_usb_typedef.h"

#include "r_usb_patapi.h"
#include "r_usb_pmsc.h"
#include "r_usb_pmsc_mini_api.h"
#include "Driver_USBD.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/*******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/
extern usb_msc_cbw_t    g_usb_pmsc_cbw;     /* Mass Storage Device Class CBW */
extern uint8_t          g_usb_pmsc_seq;     /* Mass Storage Device Class sequence */
extern uint16_t         g_usb_pmscstatus;   /* Transfer Status */
extern uint8_t          g_usb_pmsc_csw_send_wait[USB_MAX_EP_NO +1];
extern uint8_t          g_usb_pmsc_csw_send_type[USB_MAX_EP_NO +1];
extern ARM_DRIVER_USBD  Driver_USBD0;

extern void     usb_pmsc_check_cbw (void);
extern void     usb_pmsc_csw_transfer(uint8_t csw_status);
extern void     usb_pmsc_receive_cbw (void);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
  * Function Name: R_USB_PmscCbwReceive
  * Description  : Ready to receive CBW
  * Arguments    : none
  * Return Value : none
 ******************************************************************************/
void R_USB_PmscCbwReceive (void)
{
    usb_pmsc_receive_cbw();
} /* End of function R_USB_PmscCbwReceive() */


/******************************************************************************
  * Function Name: R_USB_PmscInitialize
  * Description  : 
  * Arguments    : none
  * Return Value : none
 ******************************************************************************/
void R_USB_PmscInitialize (void)
{
    usb_pmsc_init ();       /* Initialize for PMSC process using ram. */
    pmsc_atapi_init ();     /* Initialize for ATAPI process using ram. */
} /* End of function R_USB_PmscInitialize() */

/******************************************************************************
 Function Name   : R_USB_PmscCswPipeStall
 Description     : CSW Transfer
 Arguments       : uint8_t ep            : End point no.
 Return value    : none
 ******************************************************************************/
void R_USB_PmscCswPipeStall (uint8_t ep)
{
    if (USB_TRUE == g_usb_pmsc_csw_send_wait[ep])
    {
        g_usb_pmsc_csw_send_wait[ep] = USB_FALSE;
        usb_pmsc_csw_transfer(g_usb_pmsc_csw_send_type[ep]);
    }
} /* End of function R_USB_PmscCswPipeStall() */

/******************************************************************************
 Function Name   : R_USB_PmscBotProcess
 Description     : BOT protocol process
 Arguments       : uint32_t size    :transfer size
 Return value    : none
 ******************************************************************************/
void R_USB_PmscBotProcess (uint32_t size)
{

    switch (g_usb_pmsc_seq)
    {
        case USB_PMSC_DATASND:
            g_usb_pmscstatus = USB_DATA_NONE;
            pmsc_atapi_command_processing(g_usb_pmsc_cbw.cbwcb, g_usb_pmscstatus);
        break;

        case USB_PMSC_CSWSND :
            g_usb_pmscstatus = USB_DATA_NONE;
            usb_pmsc_receive_cbw();
        break;

        /* Normal  pmsc_seq Conditions */
        case USB_PMSC_CBWRCV:
            g_usb_pmscstatus = USB_DATA_OK;
            usb_pmsc_check_cbw();
        break;

        case USB_PMSC_DATARCV:
            g_usb_pmscstatus = USB_DATA_OK;
            pmsc_atapi_command_processing(g_usb_pmsc_cbw.cbwcb, g_usb_pmscstatus);
        break;

        /* Abnormal pmsc_seq Conditions */
        default:
        break;
    }
} /* End of function R_USB_PmscBotProcess */


/******************************************************************************
 End Of File
 ******************************************************************************/
