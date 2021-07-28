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
 * File Name    : r_usb_pdriver.c
 * Description  : USB Peripheral driver code.
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

#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
#include "r_usb_dmac.h"
#endif  /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)

#include "Driver_USBD.h"



/*******************************************************************************
 Macro definitions
 ******************************************************************************/

/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
******************************************************************************/

static void usb_pstd_interrupt_process (uint16_t type, uint16_t status) FUNC_LOCATION_PRV_DRIVER_USB;


/******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/
uint16_t g_usb_pstd_is_stall_pipe[USB_MAX_PIPE_NO + 1u];   /* Stall Pipe info */
usb_pcb_t g_usb_pstd_stall_cb;                          /* Stall Callback function */
uint16_t g_usb_pstd_remote_wakeup_state;                /* Result for Remote wake up */

uint16_t g_usb_pstd_req_type;                           /* Request type */
uint16_t g_usb_pstd_req_value;                          /* Value */
uint16_t g_usb_pstd_req_index;                          /* Index */
uint16_t g_usb_pstd_req_length;                         /* Length */

uint16_t g_usb_pstd_pipe0_request;

/* Driver registration */
usb_pcdreg_t g_usb_pstd_driver;
usb_setup_t g_usb_pstd_req_reg;                         /* Device Request - Request structure */


/******************************************************************************
 Function Name   : usb_pstd_epadr_to_pipe
 Description     : Get the associated pipe no. of the specified endpoint.
 Arguments       : uint16_t dir_ep : Direction + endpoint number.
 Return value    : uint16_t        : OK    : Pipe number.
                 :                 : ERROR : Error.
 ******************************************************************************/
uint16_t usb_pstd_epadr_to_pipe(uint16_t dir_ep)
{
    uint8_t     pipe_no;
    uint8_t     read_ep_addr;
    uint16_t    reg_value;

    if (USB_EP0 ==(USB_EPNUMFIELD & dir_ep))
    {
        return USB_PIPE0;
    }

    /* Loop USB Pipe resource */
    for (pipe_no = USB_PIPE1; pipe_no < (USB_PIPE9 +1); pipe_no++)
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */
        read_ep_addr = (USB_EPNUMFIELD & reg_value) | ((USB_DIRFIELD & reg_value) << 3);

        /* Cheak Endpoint Address */
        if (read_ep_addr == dir_ep)
        {
            return (pipe_no);
        }
    }
    return (USB_ERROR);

} /* End of function usb_pstd_epadr_to_pipe */


/******************************************************************************
 Function Name   : usb_pstd_interrupt_process
 Description     : Analyze the USB Peripheral interrupt event and execute the
                 : appropriate process.
 Arguments       : uint16_t type      : Interrupt type.
                 : uint16_t status    : BRDYSTS register & BRDYENB register.
 Return value    : none
 ******************************************************************************/
static void usb_pstd_interrupt_process (uint16_t type, uint16_t status)
{
    uint16_t stginfo;
    uint16_t pipe;
    uint16_t ep;

    /* check interrupt status */
    switch (type)
    {
        /* BRDY, BEMP, NRDY */
        case USB_INT_BRDY :
            usb_pstd_brdy_pipe(status);
        break;
        case USB_INT_BEMP :
            usb_pstd_bemp_pipe(status);
        break;
        case USB_INT_NRDY :
            usb_pstd_nrdy_pipe(status);
        break;

            /* Resume */
        case USB_INT_RESM :
            USB_PRINTF0("RESUME int peri\n");

            /* Callback */
            if (USB_NULL != g_usb_pstd_driver.devresume)
            {
                (*g_usb_pstd_driver.devresume)(USB_NULL, USB_NO_ARG, USB_NULL);
            }
            usb_pstd_resume_process();
        break;

            /* VBUS */
        case USB_INT_VBINT :
            if (USB_ATTACH == usb_pstd_chk_vbsts())
            {
                USB_PRINTF0("VBUS int attach\n");
                usb_pstd_attach_process();    /* USB attach */
            }
            else
            {
                USB_PRINTF0("VBUS int detach\n");
                usb_pstd_detach_process();    /* USB detach */
            }
        break;

            /* SOF */
        case USB_INT_SOFR :

            /* User program */
        break;

            /* DVST */
        case USB_INT_DVST :
            switch ((uint16_t) (status & USB_DVSQ))
            {
                /* Power state  */
                case USB_DS_POWR :
                break;

                    /* Default state  */
                case USB_DS_DFLT :
                    USB_PRINTF0("USB-reset int peri\n");
                    usb_pstd_bus_reset();
                break;

                    /* Address state  */
                case USB_DS_ADDS :
                break;

                    /* Configured state  */
                case USB_DS_CNFG :
                    USB_PRINTF0("Device configuration int peri\n");
                break;

                    /* Power suspend state */
                case USB_DS_SPD_POWR :

                    /* Continue */
                    /* Default suspend state */
                case USB_DS_SPD_DFLT :

                    /* Continue */
                    /* Address suspend state */
                case USB_DS_SPD_ADDR :

                    /* Continue */
                    /* Configured Suspend state */
                case USB_DS_SPD_CNFG :
                    USB_PRINTF0("SUSPEND int peri\n");
                    usb_pstd_suspend_process();
                break;

                    /* Error */
                default :
                break;
            }
        break;

            /* CTRT */
        case USB_INT_CTRT :
            stginfo = (uint16_t) (status & USB_CTSQ);
            if (USB_CS_IDST == stginfo)
            {
                /* Non process */
            }
            else
            {
                if (((USB_CS_RDDS == stginfo) || (USB_CS_WRDS == stginfo)) || (USB_CS_WRND == stginfo))
                {
                    /* Save request register */
                    usb_pstd_save_request();
                }
            }

            if ((g_usb_pstd_req_type & USB_BMREQUESTTYPETYPE) == USB_STANDARD)
            {
                /* Switch on the control transfer stage (CTSQ). */
                switch (stginfo)
                {
                    /* Idle or setup stage */
                    case USB_CS_IDST :
                        g_usb_pstd_pipe0_request = USB_OFF;
                    break;

                    /* Control read data stage */
                    case USB_CS_RDDS :
//                        (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_OUT | USB_EP0), ARM_USBD_EVENT_SETUP);
                        (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_IN | USB_EP0), ARM_USBD_EVENT_SETUP);
                    break;

                    /* Control write data stage */
                    case USB_CS_WRDS :
                        usb_pstd_set_stall_pipe0();
                    break;

                    /* Status stage of a control write where there is no data stage. */
                    case USB_CS_WRND :
                        if (USB_CLEAR_FEATURE == (USB_BREQUEST & g_usb_pstd_req_type))
                        {
                            if (0 == g_usb_pstd_req_length)
                            {
                                /* check request type */
                                if (USB_ENDPOINT == (USB_BMREQUESTTYPERECIP & g_usb_pstd_req_type))
                                {
                                    /* Endpoint number */
                                    ep = (uint16_t) (USB_EPNUMFIELD & g_usb_pstd_req_index);
                                    if (USB_ENDPOINT_HALT == g_usb_pstd_req_value)
                                    {
                                        /* EP1 to max */
                                        if ((USB_EP0 < ep) && (ep <= USB_MAX_EP_NO))
                                        {
                                            pipe = usb_pstd_epadr_to_pipe(g_usb_pstd_req_index);
                                            if (USB_PID_BUF == usb_cstd_get_pid(pipe))
                                            {
                                                usb_cstd_set_nak(pipe);
                                            }
                                            /* SQCLR=1 */
                                            hw_usb_set_sqclr(pipe);
                                        }
                                    }
                                }
                            }
                        }
                        (gp_usb_pstd_cmsis_cb_endpoint_event)((USB_EP_OUT | USB_EP0), ARM_USBD_EVENT_SETUP);
                    break;

                    /* Control read status stage */
                    case USB_CS_RDSS :
                        usb_pstd_stand_req4();
                    break;

                    /* Control write status stage */
                    case USB_CS_WRSS :
                        usb_pstd_stand_req5();
                    break;

                    /* Control sequence error */
                    case USB_CS_SQER :
                        usb_pstd_ctrl_end((uint16_t) USB_DATA_ERR);
                    break;

                    /* Illegal */
                    default :
                        usb_pstd_ctrl_end((uint16_t) USB_DATA_ERR);
                    break;
                }
            }
            else
            {
                /* Vender Specific */
                g_usb_pstd_req_reg.type = g_usb_pstd_req_type;
                g_usb_pstd_req_reg.value = g_usb_pstd_req_value;
                g_usb_pstd_req_reg.index = g_usb_pstd_req_index;
                g_usb_pstd_req_reg.length = g_usb_pstd_req_length;

                /* Callback */
                if (USB_NULL != g_usb_pstd_driver.ctrltrans)
                {
                    (*g_usb_pstd_driver.ctrltrans)((usb_setup_t *) &g_usb_pstd_req_reg, stginfo);
                }
            }
        break;

            /* Error */
        case USB_INT_UNKNOWN :
            USB_PRINTF0("pINT_UNKNOWN\n");
        break;
        default :
        break;
    }
} /* End of function usb_pstd_interrupt_process */



/******************************************************************************
 Function Name   : usb_pstd_pcd_task
 Description     : The Peripheral Control Driver(PCD) task.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_pcd_task (void)
{
    if (g_usb_pstd_usb_int.wp != g_usb_pstd_usb_int.rp)
    {
        /* Pop Interrupt info */
        usb_pstd_interrupt_process(g_usb_pstd_usb_int.buf[g_usb_pstd_usb_int.rp].type,
                g_usb_pstd_usb_int.buf[g_usb_pstd_usb_int.rp].status);

        /* Read CountUp */
        g_usb_pstd_usb_int.rp = ((g_usb_pstd_usb_int.rp + 1) % USB_INT_BUFSIZE);
    }
#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
    usb_cstd_dma_driver();           /* USB DMA driver */
#endif  /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */
} /* End of function usb_pstd_pcd_task */

/******************************************************************************
 Function Name   : usb_pstd_set_submitutr
 Description     : USB Peripheral Submit utr.
 Arguments       : usb_putr_t *p_utrmsg    : Pointer to usb_putr_t structure
 Return value    : usb_er_t
 ******************************************************************************/
usb_er_t usb_pstd_set_submitutr (usb_putr_t * p_utrmsg)
{
    uint16_t pipenum;

    pipenum = p_utrmsg->keyword;
    gp_usb_pstd_pipe[pipenum] = p_utrmsg;

    /* Check state (Configured) */
    if (USB_TRUE == usb_pstd_chk_configured())
    {
        /* Data transfer */
        if (USB_DIR_P_OUT == usb_cstd_get_pipe_dir(pipenum))
        {
            usb_pstd_receive_start(pipenum);    /* Out transfer */
        }
        else
        {
            /* In transfer */
            usb_pstd_send_start(pipenum);
        }
    }
    else
    {
        /* Transfer stop */
        usb_pstd_forced_termination(pipenum, (uint16_t) USB_DATA_ERR);
    }
    return USB_OK;
} /* End of function usb_pstd_set_submitutr */


/******************************************************************************
 Function Name   : usb_pstd_fifo_to_buf
 Description     : Request readout from USB FIFO to buffer and process depending
                 : on status; read complete or reading.
 Arguments       : uint16_t pipe     : Pipe no.
                 : uint16_t useport  : FIFO select(USB_CUSE,USB_DMA0,....)
 Return value    : none
 ******************************************************************************/
void usb_pstd_fifo_to_buf(uint16_t pipe, uint16_t useport)
{
    uint16_t end_flag;

    end_flag = USB_ERROR;

    if (USB_MAX_PIPE_NO < pipe)
    {
        return; /* Error */
    }

    end_flag = usb_pstd_read_data(pipe, useport);

    /* Check FIFO access sequence */
    switch (end_flag)
    {
        case USB_READING:

        /* Continue of data read */
        break;

        case USB_READEND:

        /* End of data read */
        usb_pstd_data_end(pipe, (uint16_t)USB_DATA_OK);
        break;

        case USB_READSHRT:

        /* End of data read */
        usb_pstd_data_end(pipe, (uint16_t)USB_DATA_SHT);
        break;

        case USB_READOVER:

        /* Buffer over */
        USB_PRINTF1("### Receive data over PIPE%d\n", pipe);
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_OVR);
        break;

        case USB_FIFOERROR:

        /* FIFO access error */
        USB_PRINTF0("### FIFO access error \n");
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;

        default:
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
} /* End of function usb_pstd_fifo_to_buf */

/******************************************************************************
 Function Name   : usb_pstd_buf_to_fifo
 Description     : Set USB registers as required to write from data buffer to USB
 : FIFO, to have USB FIFO to write data to bus.
 Arguments       : uint16_t pipe     : Pipe no.
                 : uint16_t useport  : Port no.
 Return value    : none
 ******************************************************************************/
void usb_pstd_buf_to_fifo(uint16_t pipe, uint16_t useport)
{
    uint16_t end_flag;

    if (USB_MAX_PIPE_NO < pipe)
    {
        return; /* Error */
    }

    /* Disable Ready Interrupt */
    hw_usb_clear_brdyenb(pipe);

    end_flag = usb_pstd_write_data(pipe, useport);

    /* Check FIFO access sequence */
    switch (end_flag)
    {
        case USB_WRITING:
            /* Continue of data write */
            /* Enable Ready Interrupt */
            hw_usb_set_brdyenb(pipe);

            /* Enable Not Ready Interrupt */
            usb_cstd_nrdy_enable(pipe);
        break;

        case USB_WRITEEND:
            /* End of data write */
            /* continue */
        case USB_WRITESHRT:
            /* End of data write */
            /* Enable Empty Interrupt */
            hw_usb_set_bempenb(pipe);

            /* Enable Not Ready Interrupt */
            usb_cstd_nrdy_enable(pipe);
        break;

        case USB_FIFOERROR:
            /* FIFO access error */
            USB_PRINTF0("### FIFO access error \n");
            usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;

        default:
            usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
} /* End of function usb_pstd_buf_to_fifo */

/******************************************************************************
 Function Name   : usb_pstd_transfer_start
 Description     : Transfer the data of each pipe
                 : Request PCD to transfer data, and the PCD transfers the data
                 : based on the transfer information stored in ptr
 Arguments       : usb_putr_t *ptr : Pointer to usb_putr_t structure.
 Return value    : usb_er_t       : Error info.
 ******************************************************************************/
usb_er_t usb_pstd_transfer_start(usb_putr_t * p_utr)
{
    usb_er_t err;
    uint16_t pipenum;

    pipenum = p_utr->keyword;
    if (USB_NULL != gp_usb_pstd_pipe[pipenum])
    {
        /* Get PIPE TYPE */
        if (usb_cstd_get_pipe_type(pipenum) != USB_TYPFIELD_ISO)
        {
            USB_PRINTF1("### usb_pstd_transfer_start overlaps %d\n", pipenum);
            return USB_QOVR;
        }
    }

    /* Check state (Configured) */
    if (USB_TRUE != usb_pstd_chk_configured())
    {
        USB_PRINTF0("### usb_pstd_transfer_start not configured\n");
        return USB_ERROR;
    }

    if (USB_PIPE0 == pipenum)
    {
        USB_PRINTF0("### usb_pstd_transfer_start PIPE0 is not support\n");
        return USB_ERROR;
    }

    err = usb_pstd_set_submitutr(p_utr);
    return err;
} /* End of function usb_pstd_transfer_start */

/******************************************************************************
 Function Name   : usb_pstd_transfer_end
 Description     : Force termination of data transfer of specified pipe. Request
                 : PCD to force termination data transfer, 
                 : and the PCD forced-terminates data transfer.
 Arguments       : uint16_t pipe     : Pipe number.
 Return value    : usb_er_t          : Error info.
 ******************************************************************************/
usb_er_t usb_pstd_transfer_end(uint16_t pipe)
{
    if (USB_MAX_PIPE_NO < pipe)
    {
        return USB_ERROR; /* Error */
    }

    if ((USB_NULL == gp_usb_pstd_pipe[pipe]) && (USB_PIPE0 != pipe))
    {
        USB_PRINTF0("### usb_pstd_transfer_end overlaps\n");
        return USB_ERROR;
    }
    else
    {
        usb_pstd_forced_termination(pipe, (uint16_t)USB_DATA_STOP);
    }

    return USB_OK;
} /* End of function usb_pstd_transfer_end */

/******************************************************************************
 Function Name   : usb_pstd_driver_registration
 Description     : Register pipe information table, descriptor information table,
                 : callback function, etc. This info is specified by the data in
                 : the structure usb_pcdreg_t.
 Arguments       : usb_pcdreg_t *registinfo     : Class driver structure.
 Return value    : none
 ******************************************************************************/
void usb_pstd_driver_registration(usb_pcdreg_t * p_registinfo)
{
    usb_pcdreg_t *p_driver;

    p_driver = &g_usb_pstd_driver;
    p_driver->devdefault   = p_registinfo->devdefault;   /* Device default */
    p_driver->devdetach    = p_registinfo->devdetach;    /* Device detach */
    p_driver->devsuspend   = p_registinfo->devsuspend;   /* Device suspend */
    p_driver->devresume    = p_registinfo->devresume;    /* Device resume */
    p_driver->ctrltrans    = p_registinfo->ctrltrans;    /* Control transfer */
} /* End of function usb_pstd_driver_registration */

/******************************************************************************
 Function Name   : usb_pstd_driver_release
 Description     : Clear the information registered in the structure usb_pcdreg_t.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_driver_release(void)
{
    usb_pcdreg_t *p_driver;

    p_driver = &g_usb_pstd_driver;
    p_driver->devdefault   = &usb_pstd_dummy_function;  /* Device default */
    p_driver->devdetach    = &usb_pstd_dummy_function;  /* Device detach */
    p_driver->devsuspend   = &usb_pstd_dummy_function;  /* Device suspend */
    p_driver->devresume    = &usb_pstd_dummy_function;  /* Device resume */
    p_driver->ctrltrans    = &usb_pstd_dummy_trn;       /* Control transfer */

}/* End of function usb_pstd_driver_release() */

/******************************************************************************
 Function Name   : usb_pstd_dummy_function
 Description     : dummy function
 Arguments       : uint16_t     data1   : Not used
                 : uint16_t     data2   : Not used
 Return value    : none
 ******************************************************************************/
void usb_pstd_dummy_function(usb_putr_t *p_utr,uint16_t data1, uint16_t data2)
{
    /* Dummy function */
} /* End of function usb_pstd_dummy_function */

/******************************************************************************
 Function Name   : usb_pstd_dummy_trn
 Description     : Class request processing for dummy
 Arguments       : usb_setup_t *preq  ; Class request information
 : uint16_t ctsq        ; Control Stage
 Return value    : none
 ******************************************************************************/
void usb_pstd_dummy_trn(usb_setup_t * p_req, uint16_t ctsq)
{
    /* Dummy function */
} /* End of function usb_pstd_dummy_trn */

/******************************************************************************
 Function Name   : usb_pstd_registration
 Description     : Registration of peripheral Devices Class Driver
 Arguments       : usb_putr_t *p_cfg : Pointer to config structure.
 Return value    : none
 ******************************************************************************/
void usb_pstd_registration(void)
{
    usb_pcdreg_t pdriver;

    /* pdriver registration */
    pdriver.devdefault  = &usb_pstd_devdefault;                 /* Device default */
    pdriver.devdetach   = &usb_pstd_detach;                     /* Device detach */
    pdriver.devsuspend  = (usb_pcb_t)&usb_pstd_suspended;       /* Device suspend */
    pdriver.devresume   = &usb_pstd_resume;                     /* Device resume */
    pdriver.ctrltrans   = (usb_cb_trn_t)&usb_pstd_class_request;/* Control Transfer */

    usb_pstd_driver_registration((usb_pcdreg_t*) &pdriver);
} /* End of function usb_pstd_registration() */

/******************************************************************************
 Function Name   : usb_pstd_devdefault
 Description     : Descriptor change
 Arguments       : usb_putr_t *p_utr       : Pointer to usb_putr_t structure.
                 : uint16_t mode        ; Speed mode
                 : uint16_t data2       ; BC Port Detect Result
 Return value    : none
 ******************************************************************************/
void usb_pstd_devdefault (usb_putr_t *p_utr, uint16_t mode, uint16_t data2)
{
    /* USB Reset occurred */
    usb_pstd_detach(USB_NULL, USB_NULL, USB_NULL);
    (gp_usb_pstd_cmsis_cb_device_event)((uint32_t)ARM_USBD_EVENT_RESET);
} /* End of function usb_pstd_devdefault() */


/******************************************************************************
 Function Name   : usb_pstd_detach
 Description     : Peripheral Devices Class close function
 Arguments       : usb_putr_t    *p_utr        : Not used
                 : uint16_t     data1       : Not used
                 : uint16_t     data2       : Not used
 Return value    : none
 ******************************************************************************/
void usb_pstd_detach (usb_putr_t *p_utr, uint16_t data1, uint16_t data2)
{
    (gp_usb_pstd_cmsis_cb_device_event)((uint32_t)ARM_USBD_EVENT_VBUS_OFF);
} /* End of function usb_pstd_detach() */

/******************************************************************************
 Function Name   : usb_pstd_suspended
 Description     : Peripheral Devices Class suspended function
 Arguments       : usb_putr_t    *p_utr        : Not used
                 : uint16_t     data1       : Not used
                 : uint16_t     data2       : Not used
 Return value    : none
 ******************************************************************************/
void usb_pstd_suspended(usb_putr_t *p_utr, uint16_t data1, uint16_t data2)
{
    (gp_usb_pstd_cmsis_cb_device_event)((uint32_t)ARM_USBD_EVENT_SUSPEND);
} /* End of function usb_pstd_suspended() */

/******************************************************************************
 Function Name   : usb_pstd_resume
 Description     : Peripheral Devices Class resume function
 Arguments       : usb_putr_t    *p_utr        : Not used
                 : uint16_t     data1       : Not used
                 : uint16_t     data2       : Not used
 Return value    : none
 ******************************************************************************/
void usb_pstd_resume(usb_putr_t *p_utr, uint16_t data1, uint16_t data2)
{
    (gp_usb_pstd_cmsis_cb_device_event)((uint32_t)ARM_USBD_EVENT_RESUME);
} /* End of function usb_pstd_resume() */


#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/******************************************************************************
 End  Of File
 ******************************************************************************/
