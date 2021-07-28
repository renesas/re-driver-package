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
 * File Name    : r_usb_cdataio.c
 * Description  : USB Host and Peripheral low level data I/O code
 ******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.09.2014 1.00    First Release
*           : 01.06.2015 1.01    Added RX231.
*           : 29.12.2015 1.02    Minor Update.
*           : 30.11.2018 1.10    Supporting Smart Configurator
*******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_basic_mini_api.h"
#include "r_usb_bitdefine.h"
#include "r_usb_typedef.h"
#include "r_usb_reg_access.h"            /* Definition of the USB register access macro */
#include "r_usb_extern.h"

#if defined(USB_CFG_HCDC_USE)
#include "r_usb_hcdc.h"
#endif /* defined(USB_CFG_HCDC_USE) */

#if defined(USB_CFG_PCDC_USE)
#include "r_usb_pcdc_mini_api.h"
#endif /* defined(USB_CFG_PCDC_USE) */

#if defined(USB_CFG_PMSC_USE)
#include "r_usb_pmsc_mini_api.h"
#endif /* defined(USB_CFG_PMSC_USE) */

#if defined(USB_CFG_PHID_USE)
#include "r_usb_phid_mini_api.h"
#endif /* defined(USB_CFG_PHID_USE) */

#if defined(USB_CFG_HHID_USE)
#include "r_usb_hhid_mini_api.h"
#endif /* defined(USB_CFG_HHID_USE) */

#if defined(USB_CFG_HMSC_USE)
#include "r_usb_hmsc_mini_api.h"
#include "r_usb_hmsc.h"
#endif /* defined(USB_CFG_HMSC_USE) */

#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
#include "r_usb_dmac.h"
#endif  /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

/*******************************************************************************
 Macro definitions
 ******************************************************************************/
/* setup packet table size (uint16_t * 5) */
#define     USB_CDC_SETUP_TBL_BSIZE             (10u)


/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/*******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/* variables */
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
USB_STATIC usb_ctrl_trans_t     gs_usb_cstd_ctrl_request;
#endif /* ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST) */

uint32_t        g_usb_read_request_size[USB_MAXPIPE_NUM + 1];
usb_putr_t      g_usb_pstd_data[USB_MAXPIPE_NUM + 1];
uint16_t        g_usb_cstd_usb_mode;

/* functions */

/*******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/
/* variables */
/*Interrupt global variables */

#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
uint16_t    g_usb_hstd_pipe_request[USB_MAX_PIPE_NO + 1u];

/* Pipe number of USB Host transfer.(Read pipe/Write pipe) */
const uint8_t g_usb_hstd_pipe_no_tbl[] DATA_LOCATION_PRV_DRIVER_USB =
{
    /* READ pipe */         /* WRITE pipe */
    /* IN pipe */           /* OUT pipe */
  #if defined(USB_CFG_HCDC_USE)
    USB_CFG_HCDC_BULK_IN,   USB_CFG_HCDC_BULK_OUT,    /* HCDC(CDC Data class) */
  #else
    USB_NULL,               USB_NULL,
  #endif
  #if defined(USB_CFG_HCDC_USE)
    USB_CFG_HCDC_INT_IN,    USB_NULL,                  /* HCDCC(CDC Control class) */
  #else
    USB_NULL,               USB_NULL,
  #endif
  #if defined(USB_CFG_HHID_USE)
    USB_CFG_HHID_INT_IN,    USB_CFG_HHID_INT_OUT,      /* HHID */
  #else
    USB_NULL,               USB_NULL,
  #endif
};

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */


#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
/* USB data transfer */
uint32_t    g_usb_pstd_data_cnt[USB_MAX_PIPE_NO + 1u];  /* PIPEn Buffer counter */
uint8_t     *gp_usb_pstd_data[USB_MAX_PIPE_NO + 1u];   /* PIPEn Buffer pointer(8bit) */
usb_putr_t  *gp_usb_pstd_pipe[USB_MAX_PIPE_NO + 1u];   /* Message pipe */

#if defined(USB_CFG_PCDC_USE)

/* Abstract Control Model Notification - SerialState */
uint8_t g_usb_pcdc_serial_state_tbl[USB_CDC_SETUP_TBL_BSIZE] DATA_LOCATION_PRV_DRIVER_USB =
{
    0xA1,       /* bmRequestType */
    0x20,       /* bNotification:SERIAL_STATE */
    0x00, 0x00, /* wValue:Zero */
    0x00, 0x00, /* wIndex:Interface */
    0x02, 0x00, /* wLength:2 */
    0x00, 0x00, /* Data:UART State bitmap */
};

#endif /* defined(USB_CFG_PCDC_USE) */

#endif /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI) */

usb_pipe_table_t    g_usb_cstd_pipe_tbl[USB_MAXPIPE_NUM+1];


uint16_t     g_usb_cstd_bemp_skip[USB_MAX_PIPE_NO + 1u];

/******************************************************************************
 Function Name   : usb_cstd_debug_hook
 Description     : Debug hook
 Arguments       : uint16_t error_code          : error code
 Return value    : none
 ******************************************************************************/
void usb_cstd_debug_hook (uint16_t error_code)
{
    while (1)
    {
        /* Non */
    }
} /* End of function usb_cstd_debug_hook() */

/******************************************************************************
 Function Name   : usb_cstd_usb_task
 Description     : USB driver main loop processing.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_cstd_usb_task (void)
{
    if (USB_HOST == g_usb_cstd_usb_mode)
    {
#if (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST

    if (USB_FLGSET == usb_hstd_scheduler() )      /* Check message for task */
    {
        usb_hstd_hcd_task();                /* USB Host driver task */
        usb_hstd_mgr_task();                /* USB Manager task */
        usb_hstd_class_task();

#if defined(USB_CFG_HMSC_USE)
        usb_hmsc_strg_drive_task();         /* HSTRG Task */

#endif /* defined(USB_CFG_HMSC_USE) */

    }


#endif /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
        usb_pstd_pcd_task();
#endif /*( (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI )*/
    }
} /* End of function usb_cstd_usb_task() */


/******************************************************************************
 Function Name   : usb_cstd_pipe_to_fport
 Description     : Get port No. from the specified pipe No. by argument
 Arguments       : uint16_t pipe  : Pipe number.
 Return value    : uint16_t       : FIFO port selector.
 ******************************************************************************/
uint16_t usb_cstd_pipe_to_fport(uint16_t pipe)
{
    uint16_t fifo_mode = USB_CUSE;

    if (USB_MAX_PIPE_NO < pipe)
    {
        return USB_NULL; /* Error */
    }

#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))

    if (USB_PIPE1 == pipe)
    {
        fifo_mode = USB_D0USE;
    }
    if (USB_PIPE2 == pipe)
    {
        fifo_mode = USB_D1USE;
    }

#endif  /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

    return fifo_mode;
} /* End of function usb_cstd_pipe_to_fport */

/******************************************************************************
Function Name   : usb_cstd_pipe_table_clear
Description     : Clear pipe table.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_pipe_table_clear (void)
{
    uint8_t pipe_no;

    /* Search use pipe block */
    for (pipe_no = USB_MIN_PIPE_NO; pipe_no < (USB_MAX_PIPE_NO +1); pipe_no++)
    {   /* Check use block */
        if (USB_TRUE == g_usb_cstd_pipe_tbl[pipe_no].use_flag)
        {   /* Clear use block */
            g_usb_cstd_pipe_tbl[pipe_no].use_flag = USB_FALSE;
            g_usb_cstd_pipe_tbl[pipe_no].pipe_cfg = 0;
            g_usb_cstd_pipe_tbl[pipe_no].pipe_maxp = 0;
            g_usb_cstd_pipe_tbl[pipe_no].pipe_peri = 0;
        }
    }

} /* End of function usb_cstd_pipe_table_clear() */


/******************************************************************************
Function Name   : usb_cstd_pipe_reg_clear
Description     : Clear Pipe registers. (Use Pipe All)
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_pipe_reg_clear (void)
{
    uint8_t pipe_no;

    /* Search use pipe block */
    for (pipe_no = USB_MIN_PIPE_NO; pipe_no < (USB_MAX_PIPE_NO +1); pipe_no++)
    {   /* Check use block */
        if (USB_TRUE == g_usb_cstd_pipe_tbl[pipe_no].use_flag)
        {   /* Clear specified pipe configuration register. */
            usb_cstd_clr_pipe_config(pipe_no);
        }
    }
} /* End of function usb_cstd_pipe_reg_clear() */


/******************************************************************************
End of file
******************************************************************************/
