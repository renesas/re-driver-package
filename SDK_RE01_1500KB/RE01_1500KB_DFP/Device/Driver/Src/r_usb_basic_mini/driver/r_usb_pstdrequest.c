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
 * Copyright (C) 2015(2018) Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * File Name    : r_usb_pstdrequest.c
 * Description  : USB Peripheral standard request code
 ******************************************************************************/
/*******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 30.11.2018 1.10    Supporting Smart Configurator
*******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usb_basic_mini_api.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#include "Driver_USBD.h"


#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)

/******************************************************************************
 Macro definitions
******************************************************************************/

/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_stand_req4
 Description     : The control read status stage of a standard request from host.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req4 (void)
{
    switch ((g_usb_pstd_req_type & USB_BREQUEST))
    {
        case USB_GET_STATUS :

            usb_cstd_set_buf((uint16_t) USB_PIPE0);            /* GetStatus4 */
        break;
        case USB_GET_DESCRIPTOR :

            usb_cstd_set_buf((uint16_t) USB_PIPE0);            /* GetDescriptor4 */
        break;
        case USB_GET_CONFIGURATION :

            usb_cstd_set_buf((uint16_t) USB_PIPE0);            /* GetConfiguration4 */
        break;
        case USB_GET_INTERFACE :

            usb_cstd_set_buf((uint16_t) USB_PIPE0);            /* GetInterface4 */
        break;
        case USB_SYNCH_FRAME :

            usb_cstd_set_buf((uint16_t) USB_PIPE0);            /* SynchFrame4 */
        break;
        default :

            usb_pstd_set_stall_pipe0();            /* Set pipe USB_PID_STALL */
        break;
    }

    (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_IN | USB_EP0), ARM_USBD_EVENT_IN);

    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END);    /* Control transfer stop(end) */

} /* End of function usb_pstd_stand_req4 */


/******************************************************************************
 Function Name   : usb_pstd_stand_req5
 Description     : The control write status stage of a standard request from host.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_stand_req5 (void)
{
    if ((g_usb_pstd_req_type & USB_BREQUEST) == USB_SET_DESCRIPTOR)
    {
        /* Set pipe PID_BUF */
        usb_cstd_set_buf((uint16_t) USB_PIPE0);
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_set_stall_pipe0();
    }

    (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_OUT | USB_EP0), ARM_USBD_EVENT_OUT);

    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END);    /* Control transfer stop(end) */

} /* End of function usb_pstd_stand_req5 */

/******************************************************************************
 Function Name   : usb_pstd_class_request
 Description     : Class request processing for Device class
 Arguments       : usb_setup_t *p_req    ; Class request information
                 : uint16_t ctsq        ; Control Stage
 Return value    : none
 ******************************************************************************/
void usb_pstd_class_request(usb_setup_t *p_req, uint16_t ctsq)
{
    /* Branch by the Control Transfer Stage */
    switch (ctsq)
    {
        case USB_CS_IDST :
            g_usb_pstd_pipe0_request = USB_OFF;
            usb_pstd_class_request_ioss(p_req);  /* class request (idle or setup stage) */
        break;

        case USB_CS_RDDS :
            usb_pstd_class_request_rwds(p_req);  /* class request (control read data stage) */
        break;

        case USB_CS_WRDS :
            usb_pstd_class_request_rwds(p_req);  /* class request (control write data stage) */
        break;

        case USB_CS_WRND :
            usb_pstd_class_request_wnss(p_req);  /* class request (control write nodata status stage) */
        break;

        case USB_CS_RDSS :
            usb_pstd_class_request_rss(p_req);   /* class request (control read status stage) */
        break;

        case USB_CS_WRSS :
            usb_pstd_class_request_wss(p_req);   /* class request (control write status stage) */
        break;

        case USB_CS_SQER :
            usb_pstd_ctrl_end((uint16_t) USB_DATA_ERR);     /* End control transfer. */
        break;

        default :
            usb_pstd_ctrl_end((uint16_t) USB_DATA_ERR);     /* End control transfer. */
        break;
    }
} /* End of function usb_pstd_class_request */

/******************************************************************************
 Function Name   : usb_pstd_class_request_ioss
 Description     : Class Request (idle or setup stage)
 Arguments       : usb_setup_t *req : Pointer to usb_setup_t structure
 Return value    : none
 ******************************************************************************/
void usb_pstd_class_request_ioss (usb_setup_t *p_req)
{
    /* Non */
} /* End of function usb_pstd_class_request_ioss */

/******************************************************************************
 Function Name   : usb_pstd_class_request_rwds
 Description     : Class request processing (control read/write data stage)
 Arguments       : usb_setup_t *req : Pointer to usb_setup_t structure
 Return value    : none
 ******************************************************************************/
void usb_pstd_class_request_rwds (usb_setup_t * p_req)
{
    uint8_t     ep_addr = USB_EP0;

    if (USB_EP_IN == (USB_EP_IN & p_req->type))
    {
        ep_addr |= USB_EP_IN;
    }

    /* Is a request receive target Interface? */
    if (0 == p_req->index)
    {
        usb_cstd_set_buf((uint16_t) USB_PIPE0); /* Set BUF */
        (gp_usb_pstd_cmsis_cb_endpoint_event)(ep_addr, ARM_USBD_EVENT_SETUP);
    }
    else
    {
        /* Set Stall */
        usb_pstd_set_stall_pipe0(); /* Req Error */
    }
} /* End of function usb_pstd_class_request_rwds */

/******************************************************************************
 Function Name   : usb_pstd_other_request
 Description     : Processing to notify the reception of the USB request
 Arguments       : usb_setup_t *p_req : Pointer to usb_setup_t structure
 Return value    : none
 ******************************************************************************/
void usb_pstd_other_request (usb_setup_t *p_req)
{
    uint8_t     ep_addr = USB_EP0;

    if (USB_EP_IN == (USB_EP_IN & p_req->type))
    {
        ep_addr |= USB_EP_IN;
    }

    usb_cstd_set_buf((uint16_t) USB_PIPE0); /* Set BUF */
    (gp_usb_pstd_cmsis_cb_endpoint_event)(ep_addr, ARM_USBD_EVENT_SETUP);

} /* End of function usb_pstd_other_request */

/******************************************************************************
 Function Name   : usb_pstd_class_request_wnss
 Description     : class request (control write nodata status stage)
 Arguments       : usb_setup_t *req : Pointer to usb_setup_t structure
 Return value    : none
 ******************************************************************************/
void usb_pstd_class_request_wnss (usb_setup_t *p_req)
{
    /* Is a request receive target Interface? */
    if (0 == p_req->index)
    {
        usb_cstd_set_buf((uint16_t) USB_PIPE0); /* Set BUF */
        (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_OUT | USB_EP0), ARM_USBD_EVENT_SETUP);
    }
    else
    {
        usb_pstd_set_stall_pipe0(); /* Req Error */
    }

    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END); /* End control transfer. */
} /* End of function usb_pstd_class_request_wnss */

/******************************************************************************
 Function Name   : usb_pstd_class_request_rss
 Description     : class request (control read status stage)
 Arguments       : usb_setup_t *req : Pointer to usb_setup_t structure
 Return value    : none
 ******************************************************************************/
void usb_pstd_class_request_rss (usb_setup_t *p_req)
{
    usb_cstd_set_buf((uint16_t) USB_PIPE0); /* Set BUF */
    (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_IN | USB_EP0), ARM_USBD_EVENT_IN);

    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END); /* End control transfer. */

} /* End of function usb_pstd_class_request_rss */

/******************************************************************************
 Function Name   : usb_pstd_class_request_wss
 Description     : class request (control write status stage)
 Arguments       : usb_setup_t *req : Pointer to usb_setup_t structure
 Return value    : none
 ******************************************************************************/
void usb_pstd_class_request_wss (usb_setup_t *p_req)
{
    usb_cstd_set_buf((uint16_t) USB_PIPE0); /* Set BUF */
    (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_OUT | USB_EP0), ARM_USBD_EVENT_OUT);

    usb_pstd_ctrl_end((uint16_t) USB_CTRL_END);
} /* End of function usb_pstd_class_request_wss */

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 End  Of File
 ******************************************************************************/
