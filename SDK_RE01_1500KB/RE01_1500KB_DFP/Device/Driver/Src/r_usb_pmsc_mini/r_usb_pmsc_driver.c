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
#include <string.h>

#include "r_usb_basic_mini_api.h"
#include "r_usb_patapi.h"
#include "r_usb_pmsc.h"

#include "Driver_USBD.h"

/*******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/
usb_pmsc_cbm_t  g_usb_pmsc_message;                 /* Mass Storage Device Class */
usb_msc_cbw_t   g_usb_pmsc_cbw;                     /* Mass Storage Device Class CBW */
uint32_t        g_usb_pmsc_dtl;                     /* Mass Storage Device Class Transfer Length(CBW) */
uint16_t        g_usb_pmscstatus;                   /* Transfer Status */
uint8_t         g_usb_pmsc_seq;                     /* Mass Storage Device Class sequence */
uint8_t         g_usb_pmsc_csw_send_wait[USB_MAX_EP_NO +1];
uint8_t         g_usb_pmsc_csw_send_type[USB_MAX_EP_NO +1];

extern ARM_DRIVER_USBD Driver_USBD0;

void     usb_pmsc_check_cbw (void) FUNC_LOCATION_PRV_DRIVER_USB;
void     usb_pmsc_data_transfer (uint16_t status, void *p_adr, uint32_t size) FUNC_LOCATION_PRV_DRIVER_USB;
void     usb_pmsc_csw_transfer(uint8_t csw_status) FUNC_LOCATION_PRV_DRIVER_USB;
void     usb_pmsc_receive_cbw (void) FUNC_LOCATION_PRV_DRIVER_USB;

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static uint8_t  usb_pmsc_check_case13 (uint32_t ul_size, uint8_t *p_uc_case) FUNC_LOCATION_PRV_DRIVER_USB;
static uint8_t  usb_pmsc_transfer_matrix (uint8_t uc_pmsc_case) FUNC_LOCATION_PRV_DRIVER_USB;
static void     usb_pmsc_setcsw (uint8_t ar_resp) FUNC_LOCATION_PRV_DRIVER_USB;
static void     usb_pmsc_do_stall(uint16_t csw_state, uint8_t ep_addr) FUNC_LOCATION_PRV_DRIVER_USB;

static usb_msc_csw_t    gs_usb_pmsc_csw;    /* Mass Storage Device Class CSW */


/******************************************************************************
  * Function Name: usb_pmsc_check_cbw
  * Description  : Check CBW
  * Arguments    : none
  * Return Value : none
 ******************************************************************************/
void usb_pmsc_check_cbw (void)
{
    uint8_t     result;
    uint8_t     uc_case = USB_MSC_CASE_ERR;
    uint8_t     uc_pmsc_case = USB_MSC_CASE_ERR;

    result = USB_PMSC_CHECK;

    /*  CBW packet valiable check    */
    if (USB_MSC_CBW_SIGNATURE != g_usb_pmsc_cbw.dcbw_signature)
    {
        result = USB_PMSC_SIG_ERR;
    }

    /*  Check reserved bit   */
    if (0 != g_usb_pmsc_cbw.bmcbw_flags.reserved7)
    {
        result = USB_PMSC_IN_DATA_STALL_CSW_NG;
    }

    /*  Check reserved bit   */
    if (0 != g_usb_pmsc_cbw.bcbwlun.reserved4)
    {
        result = USB_PMSC_IN_DATA_STALL_CSW_NG;
    }

    /*  Check reserved bit   */
    if (0 != g_usb_pmsc_cbw.bcbwcb_length.reserved3)
    {
        result = USB_PMSC_IN_DATA_STALL_CSW_NG;
    }

    /*  Check Logical Unit Number   */
    if (g_usb_pmsc_cbw.bcbwlun.bcbwlun > 0)
    {
        result = USB_PMSC_IN_DATA_STALL_CSW_NG;
    }

    /*  Check Command Block Length  */
    if (g_usb_pmsc_cbw.bcbwcb_length.bcbwcb_length > USB_MSC_CBWCB_LENGTH)
    {
        result = USB_PMSC_IN_DATA_STALL_CSW_NG;
    }

    if (result == USB_PMSC_CHECK)
    {
        /*  Set data length */
        g_usb_pmsc_dtl = (uint32_t) g_usb_pmsc_cbw.dcbw_dtl_hi << 24;
        /* b23-b16 set */
        g_usb_pmsc_dtl |= ((uint32_t) g_usb_pmsc_cbw.dcbw_dtl_mh << 16);
        /* b15-b8 set */
        g_usb_pmsc_dtl |= ((uint32_t) g_usb_pmsc_cbw.dcbw_dtl_ml << 8);
        /* b7-b0 set \ */
        g_usb_pmsc_dtl |= (uint32_t) g_usb_pmsc_cbw.dcbw_dtl_lo;

        /* Analyze CBWCB Command */
        pmsc_atapi_analyze_cbwcb(g_usb_pmsc_cbw.cbwcb);

        /* Check Analyze_CBWCB result */
        switch (g_usb_pmsc_message.ar_rst)
        {
            case USB_ATAPI_NO_DATA:     /* No data commnad */
                uc_case = USB_MSC_DNXX;
            break;

            case USB_ATAPI_SND_DATAS:   /* Send data command */
                uc_case = USB_MSC_DIXX;
            break;

            case USB_ATAPI_RCV_DATAS:   /* Receive data command */
                uc_case = USB_MSC_DOXX;
            break;

            case USB_ATAPI_NOT_SUPPORT: /* Not Support Command */
                if (0UL == g_usb_pmsc_dtl)
                {
                    result = USB_PMSC_NO_DATA_CSW_NG;   /* No STALL Pipe */
                }
                else
                {
                    if (1 == g_usb_pmsc_cbw.bmcbw_flags.cbw_dir)
                    {
                        result = USB_PMSC_IN_DATA_STALL_CSW_NG;   /* IN Pipe Stall */
                    }
                    else
                    {
                        result = USB_PMSC_OUT_DATA_STALL_CSW_NG;   /* OUT Pipe Stall */
                    }
                }

            break;

            default:
                result = USB_PMSC_IN_DATA_STALL_CSW_NG;
            break;
        }
    }
    if (USB_PMSC_CHECK == result)
    {
        /* Check 13 case */
        uc_pmsc_case = usb_pmsc_check_case13(g_usb_pmsc_message.ul_size, &uc_case);

        /* Decide response according to 13 case */
        result = usb_pmsc_transfer_matrix(uc_pmsc_case);
    }

    switch (result)
    {
        case USB_PMSC_DATASND:
        case USB_PMSC_DATARCV:
        case USB_PMSC_CSWSND:
            g_usb_pmsc_seq = result;
            pmsc_atapi_command_processing(g_usb_pmsc_cbw.cbwcb, g_usb_pmscstatus);
        break;

        case USB_PMSC_SIG_ERR:
            /* b31-b24 set */
            g_usb_pmsc_dtl = (uint32_t) g_usb_pmsc_cbw.dcbw_dtl_hi << 24;
            /* b23-b16 set */
            g_usb_pmsc_dtl |= ((uint32_t) g_usb_pmsc_cbw.dcbw_dtl_mh << 16);
            /* b15-b8 set */
            g_usb_pmsc_dtl |= ((uint32_t) g_usb_pmsc_cbw.dcbw_dtl_ml << 8);
            /* b7-b0 set */
            g_usb_pmsc_dtl |= (uint32_t) g_usb_pmsc_cbw.dcbw_dtl_lo;

            if (0UL == g_usb_pmsc_dtl)
            {
                usb_pmsc_csw_transfer(USB_MSC_CSW_NG);
            }
            else
            {
                if (1 == g_usb_pmsc_cbw.bmcbw_flags.cbw_dir)
                {
                    /* IN Stall & CSW(NG) transfer*/
                    usb_pmsc_do_stall (USB_MSC_CSW_NG, (USB_EP_IN | USB_EP1));
                }
                else
                {
                    /* OUT Stall & CSW(NG) transfer */
                    usb_pmsc_do_stall (USB_MSC_CSW_NG, (USB_EP_OUT | USB_EP2));
                }
            }
        break;

        case USB_PMSC_IN_DATA_STALL_CSW_NG:
            usb_pmsc_do_stall (USB_MSC_CSW_NG, (USB_EP_IN | USB_EP1));
        break;

        case USB_PMSC_OUT_DATA_STALL_CSW_NG:
            usb_pmsc_do_stall (USB_MSC_CSW_NG, (USB_EP_OUT | USB_EP2));
        break;

        case USB_PMSC_IN_DATA_STALL_CSW_ERR:
            g_usb_pmsc_dtl = 0x00ul;
            usb_pmsc_do_stall (USB_MSC_CSW_PHASE_ERR, (USB_EP_IN | USB_EP1));
        break;

        case USB_PMSC_OUT_DATA_STALL_CSW_ERR:
            g_usb_pmsc_dtl = 0x00UL;
            usb_pmsc_do_stall (USB_MSC_CSW_PHASE_ERR, (USB_EP_OUT | USB_EP2));
        break;

        case USB_PMSC_NO_DATA_CSW_NG:
            /* CSW(NG) transfer */
            usb_pmsc_csw_transfer(USB_MSC_CSW_NG);
        break;

        default :
        break;
    }
} /* End of function usb_pmsc_check_cbw() */

/******************************************************************************
  * Function Name: usb_pmsc_check_case13
  * Description  : 13 case check
  * Arguments    : uint32_t     ul_size      : size
                 : uint8_t      *p_uc_case     : case
  * Return Value : uint8_t                   : result
 ******************************************************************************/
static uint8_t usb_pmsc_check_case13 (uint32_t ul_size, uint8_t *p_uc_case)
{
    uint8_t     result;

    if (0UL == g_usb_pmsc_dtl)
    {
        (*p_uc_case) |= USB_MSC_XXHN;    /* Host No Data */
    }
    else if (0 != g_usb_pmsc_cbw.bmcbw_flags.cbw_dir)
    {
        (*p_uc_case) |= USB_MSC_XXHI;    /* Host Recieved(IN) Data */
    }
    else
    {
        (*p_uc_case) |= USB_MSC_XXHO;    /* Host Send(OUT) Data */
    }

    /* 13cases */
    switch (*p_uc_case)
    {
        case USB_MSC_DNHN:  /* Device No Data */
            result = USB_MSC_CASE01;
        break;

        case USB_MSC_DNHI:  /* Device No Data & Host Recieved(IN) Data */
            result = USB_MSC_CASE04;
        break;

        case USB_MSC_DNHO:  /* Device No Data & Host Send(OUT) Data */
            result = USB_MSC_CASE09;
        break;

        case USB_MSC_DIHN:  /* Device Send(IN) Data & Host No Data */
            result = USB_MSC_CASE02;
        break;

        case USB_MSC_DIHI:  /* Device Send(IN) Data & Host Recieved(IN) Data */
            if (g_usb_pmsc_dtl > ul_size)
            {
                result = USB_MSC_CASE05;
            }
            else
            {
                if (g_usb_pmsc_dtl == ul_size)
                {
                    result = USB_MSC_CASE06;
                }
                else
                {
                    result = USB_MSC_CASE07;
                }
            }
        break;

        case USB_MSC_DIHO:  /* Device Send(IN) Data & Host Send(OUT) Data */
            result = USB_MSC_CASE10;
        break;

        case USB_MSC_DOHN:  /* Device Recieved(OUT) Data & Host No Data */
            result = USB_MSC_CASE03;
        break;

        case USB_MSC_DOHI:  /* Device Recieved(OUT) Data & Host Recieved(IN) Data */
            result = USB_MSC_CASE08;
        break;

        case USB_MSC_DOHO:  /* Device Recieved(OUT) Data & Host Send(OUT) Data */
            if (g_usb_pmsc_dtl > ul_size)
            {
                result = USB_MSC_CASE11;
            }
            else
            {
                if (g_usb_pmsc_dtl == ul_size)
                {
                    result = USB_MSC_CASE12;
                }
                else
                {
                    result = USB_MSC_CASE13;
                }
            }
        break;

        default :
            result = USB_MSC_CASE_ERR;
        break;
    }

    return result;
} /* End of function usb_pmsc_check_case13() */

/******************************************************************************
  * Function Name: usb_pmsc_transfer_matrix
  * Description  : Decide response according to 13 case
  * Arguments    : uint8_t      uc_case      : case
  * Return Value : uint8_t                   : result
 ******************************************************************************/
static uint8_t usb_pmsc_transfer_matrix (uint8_t uc_case)
{
    uint8_t     result;

    switch (uc_case)
    {
        case USB_MSC_CASE01:    /* CSW transfer */
            result = USB_PMSC_CSWSND;   /* Transfer Command Status Wrapper */
        break;

        case USB_MSC_CASE05:
        case USB_MSC_CASE06:    /* Data Transfer(IN) */
            result = USB_PMSC_DATASND;  /* Data Transport */
        break;

        case USB_MSC_CASE12:    /* Data Transfer(OUT) */
            result = USB_PMSC_DATARCV;  /* Data Transport */
        break;

        case USB_MSC_CASE04:    /* CSW transfer */
            result = USB_PMSC_IN_DATA_STALL_CSW_NG;   /* Data Transfer(IN) */
        break;

        case USB_MSC_CASE09:    /* CSW transfer */
        case USB_MSC_CASE11:    /* Data Transfer(OUT) */
            result = USB_PMSC_OUT_DATA_STALL_CSW_NG;
        break;

        case USB_MSC_CASE02:
        case USB_MSC_CASE03:
        case USB_MSC_CASE07:
        case USB_MSC_CASE08:
            result = USB_PMSC_IN_DATA_STALL_CSW_ERR;
        break;

        case USB_MSC_CASE10:
        case USB_MSC_CASE13:
            result = USB_PMSC_OUT_DATA_STALL_CSW_ERR;
        break;

        default:
            g_usb_pmsc_dtl = 0x00UL;
            result = USB_PMSC_OUT_DATA_STALL_CSW_ERR;   /* Internal Device Error */
        break;
    }
    return result;
} /* End of function usb_pmsc_transfer_matrix() */

/******************************************************************************
  * Function Name: usb_pmsc_csw_transfer
  * Description  : Create CSW and Transfer
  * Arguments    : uint8_t      csw_status      : status(USB_MSC_CSW_OK/USB_MSC_CSW_NG/USB_MSC_CSW_PHASE_ERR)
  * Return Value : none
 ******************************************************************************/
void usb_pmsc_csw_transfer(uint8_t csw_status)
{
    usb_pmsc_setcsw(csw_status);
    /* Cast Call-back function */
    Driver_USBD0.EndpointTransfer ((USB_EP_IN | USB_EP1), (uint8_t *)&gs_usb_pmsc_csw, USB_MSC_CSW_LENGTH);

    g_usb_pmsc_seq = USB_PMSC_CSWSND;
} /* End of function usb_pmsc_csw_transfer() */

/******************************************************************************
  * Function Name: usb_pmsc_setcsw
  * Description  : Create CSW
  * Arguments    : uint8_t      ar_resp      : status
  * Return Value : none
 ******************************************************************************/
static void usb_pmsc_setcsw (uint8_t ar_resp)
{
    /* Set CSW parameter */
    gs_usb_pmsc_csw.dcsw_signature       = USB_MSC_CSW_SIGNATURE;
    gs_usb_pmsc_csw.dcsw_tag             = g_usb_pmsc_cbw.dcbw_tag;
    /* b7-b0 set */
    gs_usb_pmsc_csw.dcsw_data_residue_lo = (uint8_t)g_usb_pmsc_dtl;
    /* b15-b8 set */
    gs_usb_pmsc_csw.dcsw_data_residue_ml = (uint8_t)(g_usb_pmsc_dtl >> 8);
    /* b23-b16 set */
    gs_usb_pmsc_csw.dcsw_data_residue_mh = (uint8_t)(g_usb_pmsc_dtl >> 16);
    /* b31-b24 set */
    gs_usb_pmsc_csw.dcsw_data_residue_hi = (uint8_t)(g_usb_pmsc_dtl >> 24);
    gs_usb_pmsc_csw.bcsw_status = ar_resp;
} /* End of function usb_pmsc_setcsw() */

/******************************************************************************
  * Function Name: usb_pmsc_data_transfer
  * Description  : Next treatment after ATAPI Command Execute
  * Arguments    : usb_putr_t  *p_mess       : Pointer to usb_putr_t structure
  * Return Value : none
 ******************************************************************************/
void usb_pmsc_data_transfer (uint16_t status, void *p_tranadr, uint32_t tranlen)
{
    switch (status)
    {
        case USB_PMSC_CMD_COMPLETE:     /* Command Execute -> Pass! */
            /* CSW(OK) transfer */
            usb_pmsc_csw_transfer(USB_MSC_CSW_OK);
        break;

        case USB_PMSC_CMD_SHT_COMPLETE: /* Command Execute -> Pass! */
            /* CSW(OK) transfer */
            usb_pmsc_do_stall (USB_MSC_CSW_OK, (USB_EP_IN | USB_EP1));
        break;

        case USB_PMSC_CMD_FAILED:       /* Command Execute -> Fail */
            if (0UL != g_usb_pmsc_dtl)
            { /* Zero or short packet */
                usb_pmsc_do_stall (USB_MSC_CSW_NG, (USB_EP_IN | USB_EP1));
            }
            else
            {
                /* CSW(NG) transfer */
                usb_pmsc_csw_transfer(USB_MSC_CSW_NG);
            }
        break;

        case USB_PMSC_CMD_CONTINUE:     /* Set Send /Recieve Data parameters */
            switch (g_usb_pmsc_seq)
            {
                case USB_PMSC_DATARCV:  /* OUT Data */
                    /* Data receive start */
                    Driver_USBD0.EndpointTransfer ((USB_EP_OUT | USB_EP2), (uint8_t*)p_tranadr, tranlen);

                break;

                case USB_PMSC_DATASND: /* IN Data */
                    /* Data transfer start */
                    Driver_USBD0.EndpointTransfer ((USB_EP_IN | USB_EP1), (uint8_t*)p_tranadr, tranlen);

                break;

                default:
                break;
            }
        break;

        default:
        break;
    }
} /* End of function usb_pmsc_data_transfer() */


/******************************************************************************
  * Function Name: usb_pmsc_receive_cbw
  * Description  : Ready to receive CBW
  * Arguments    : none
  * Return Value : none
 ******************************************************************************/
void usb_pmsc_receive_cbw (void)
{
    g_usb_pmsc_dtl = 0x00UL;
    /* Cast Call-back function */
    Driver_USBD0.EndpointTransfer ((USB_EP_OUT | USB_EP2), (uint8_t *)&g_usb_pmsc_cbw, USB_MSC_CBWLENGTH);

    g_usb_pmsc_seq = USB_PMSC_CBWRCV;
} /* End of function usb_pmsc_receive_cbw() */


/******************************************************************************
  * Function Name: usb_pmsc_init
  * Description  : 
  * Arguments    : none
  * Return Value : none
 ******************************************************************************/
void usb_pmsc_init (void)
{
    g_usb_pmsc_seq = 0;
    g_usb_pmscstatus = 0;
    /* Cast for argument type */
    memset((void *)&g_usb_pmsc_cbw, 0, sizeof(usb_msc_cbw_t));
    /* Cast for argument type */
    memset((void *)&gs_usb_pmsc_csw, 0, sizeof(usb_msc_csw_t));
    g_usb_pmsc_dtl = 0;
    /* Cast for argument type */
    memset((void *)&g_usb_pmsc_message, 0, sizeof(usb_pmsc_cbm_t));

    memset((void *)&g_usb_pmsc_csw_send_wait, 0, sizeof(g_usb_pmsc_csw_send_wait));
    memset((void *)&g_usb_pmsc_csw_send_type, 0, sizeof(g_usb_pmsc_csw_send_type));

    pmsc_atapi_init ();     /* Initialize for ATAPI process using ram. */
} /* End of function usb_pmsc_init() */

/******************************************************************************
 Function Name   : usb_pmsc_do_stall
 Description     : Change USB Device to the status specified by argument
 Arguments       : uint16_t state            : New device status.
                 : uint16_t keyword          : Pipe number etc.
                 : usb_pcb_t complete         : Callback function.
 Return value    : none
 ******************************************************************************/
void usb_pmsc_do_stall(uint16_t csw_state, uint8_t ep_addr)
{
    Driver_USBD0.EndpointStall(ep_addr, true);       /* Set pipe USB_PID_STALL */

    g_usb_pmsc_csw_send_wait[(USB_EPNUMFIELD & ep_addr)] = USB_TRUE;
    g_usb_pmsc_csw_send_type[(USB_EPNUMFIELD & ep_addr)] = csw_state;
} /* End of function usb_pmsc_do_stall */

/******************************************************************************
 End Of File
 ******************************************************************************/
