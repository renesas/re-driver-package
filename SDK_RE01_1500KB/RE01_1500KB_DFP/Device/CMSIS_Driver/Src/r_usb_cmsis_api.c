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
*******************************************************************************/

/******************************************************************************
 * File Name    : r_usb_cmsis_api.c
 * Description  : CMSIS USB Peripheral driver API
 ******************************************************************************/
/******************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 26.04.2019 0.50     Alpha Version
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "Driver_USBD.h"
#include <string.h>

#include "r_usb_basic_mini_api.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
#include "r_usb_dmac.h"
#endif  /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */


#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)

/******************************************************************************
Macro definitions
******************************************************************************/
#define ARM_USBD_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

ARM_USBD_SignalDeviceEvent_t    gp_usb_pstd_cmsis_cb_device_event;
ARM_USBD_SignalEndpointEvent_t  gp_usb_pstd_cmsis_cb_endpoint_event;

/* Driver Capabilities */
static const ARM_USBD_CAPABILITIES usbd_driver_capabilities DATA_LOCATION_PRV_DRIVER_USB =
{
    1, /* vbus_detection */
    1, /* event_vbus_on */
    1  /* event_vbus_off */
};

/* number of successfully transferred data bytes */
uint32_t g_usb_pstd_cmsis_pipe_transfer_result_size[USB_MAX_PIPE_NO + 1u];
/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Driver Version */
static const ARM_DRIVER_VERSION usbd_driver_version DATA_LOCATION_PRV_DRIVER_USB =
{ 
    ARM_USBD_API_VERSION,
    ARM_USBD_DRV_VERSION
};

static ARM_DRIVER_VERSION ARM_USBD_GetVersion(void) FUNC_LOCATION_PRV_DRIVER_USB;
static ARM_USBD_CAPABILITIES ARM_USBD_GetCapabilities(void) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_Initialize(ARM_USBD_SignalDeviceEvent_t cb_device_event,
                            ARM_USBD_SignalEndpointEvent_t cb_endpoint_event) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_Uninitialize(void) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_PowerControl(ARM_POWER_STATE state) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_DeviceConnect(void) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_DeviceDisconnect(void) FUNC_LOCATION_PRV_DRIVER_USB;
static ARM_USBD_STATE ARM_USBD_DeviceGetState(void) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_DeviceRemoteWakeup(void) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_DeviceSetAddress(uint8_t dev_addr) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_ReadSetupPacket(uint8_t *setup) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_EndpointConfigure(uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size)
                                          FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_EndpointUnconfigure(uint8_t ep_addr) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_EndpointStall(uint8_t ep_addr, bool stall) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_EndpointTransfer(uint8_t ep_addr, uint8_t *data, uint32_t num) FUNC_LOCATION_PRV_DRIVER_USB;
static uint32_t ARM_USBD_EndpointTransferGetResult(uint8_t ep_addr) FUNC_LOCATION_PRV_DRIVER_USB;
static int32_t ARM_USBD_EndpointTransferAbort(uint8_t ep_addr) FUNC_LOCATION_PRV_DRIVER_USB;
static uint16_t ARM_USBD_GetFrameNumber(void) FUNC_LOCATION_PRV_DRIVER_USB;
static void DUMMY_ARM_USBD_SignalDeviceEvent(uint32_t event) FUNC_LOCATION_PRV_DRIVER_USB;
static void DUMMY_ARM_USBD_SignalEndpointEvent(uint8_t ep_addr, uint32_t ep_event) FUNC_LOCATION_PRV_DRIVER_USB;
static void usb_pstd_cmsis_transfer_complete (usb_putr_t *p_mess, uint16_t data1, uint16_t data2)
                                              FUNC_LOCATION_PRV_DRIVER_USB;

/******************************************************************************
Function Name   : ARM_USBD_GetVersion
Description     : Get driver version. 
Arguments       : none
Return value    : ARM_DRIVER_VERSION
******************************************************************************/
ARM_DRIVER_VERSION ARM_USBD_GetVersion(void)
{
    return (usbd_driver_version);
} /* End of function ARM_USBD_GetVersion */

/******************************************************************************
Function Name   : ARM_USBD_GetCapabilities
Description     : Get driver capabilities. 
Arguments       : none
Return value    : ARM_USBD_CAPABILITIES
******************************************************************************/
ARM_USBD_CAPABILITIES ARM_USBD_GetCapabilities(void)
{
    return (usbd_driver_capabilities);
} /* End of function ARM_USBD_GetCapabilities */

/******************************************************************************
Function Name   : ARM_USBD_Initialize
Description     : Initialize USB Device Interface.
Arguments       : [in] cb_device_event      Pointer to ARM_USBD_SignalDeviceEvent
                : [in] cb_endpoint_event    Pointer to ARM_USBD_SignalEndpointEvent
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_Initialize(ARM_USBD_SignalDeviceEvent_t cb_device_event,
                            ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
{
    gp_usb_pstd_cmsis_cb_device_event   = cb_device_event;      /* Set callback function. Signal USB Device Event. */
    gp_usb_pstd_cmsis_cb_endpoint_event = cb_endpoint_event;    /* Set callback function. Signal USB Endpoint Event. */

    memset((void *)&g_usb_read_request_size, 0, ((USB_MAXPIPE_NUM + 1) * 4));
    memset((void *)&g_usb_pstd_data, 0, ((USB_MAXPIPE_NUM + 1) * sizeof(usb_putr_t)));
    g_usb_cstd_usb_mode = USB_PERI;


    usb_cpu_usbint_init();                      /* Initialized USB interrupt */

    usb_pstd_driver_init();                     /* USB driver initialization */
    hw_usb_pmodule_init();                      /* Setting USB relation register  */

    return (ARM_DRIVER_OK);
} /* End of function ARM_USBD_Initialize */

/******************************************************************************
Function Name   : ARM_USBD_Uninitialize
Description     : De-initialize USB Device Interface. 
Arguments       : none
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_Uninitialize(void)
{
    /* Set callback function. Signal USB Device Event. */
    gp_usb_pstd_cmsis_cb_device_event   = &DUMMY_ARM_USBD_SignalDeviceEvent;
    /* Set callback function. Signal USB Endpoint Event. */
    gp_usb_pstd_cmsis_cb_endpoint_event = &DUMMY_ARM_USBD_SignalEndpointEvent;

    usb_pstd_driver_release();   /* Clear the information registered in the structure usb_pcdreg_t. */
    usb_cstd_pipe_table_clear();
    return (ARM_DRIVER_OK);
} /* End of function ARM_USBD_Uninitialize */

/******************************************************************************
Function Name   : ARM_USBD_PowerControl
Description     : Control USB Device Interface Power. 
Arguments       : [in] state    Power state
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_PowerControl(ARM_POWER_STATE state)
{
    usb_err_t   result;
    int32_t     ret_val;

    switch (state)
    {
        case ARM_POWER_OFF:
            result = usb_module_stop();
            if (USB_SUCCESS == result)
            {
                ret_val = ARM_DRIVER_OK;
            }
            else if(USB_ERR_BUSY == result)
            {
                ret_val = ARM_DRIVER_ERROR_BUSY;
            }
            else
            {
                ret_val = ARM_DRIVER_ERROR;
            }
        break;

        case ARM_POWER_LOW:
            ret_val = ARM_DRIVER_ERROR_UNSUPPORTED;
        break;

        case ARM_POWER_FULL:
            result = usb_module_start();            /* Start USB module */
            if (USB_SUCCESS == result)
            {
                ret_val = ARM_DRIVER_OK;
            }
            else if(USB_ERR_BUSY == result)
            {
                ret_val = ARM_DRIVER_ERROR_BUSY;
            }
            else
            {
                ret_val = ARM_DRIVER_ERROR;
            }
        break;

        default:
            ret_val = ARM_DRIVER_ERROR_UNSUPPORTED;
        break;
    }

    return ret_val;
} /* End of function ARM_USBD_PowerControl */

/******************************************************************************
Function Name   : ARM_USBD_DeviceConnect
Description     : Connect USB Device. 
Arguments       : none
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_DeviceConnect(void)
{
    hw_usb_pset_dprpu();

    return (ARM_DRIVER_OK);
} /* End of function ARM_USBD_DeviceConnect */

/******************************************************************************
Function Name   : ARM_USBD_DeviceDisconnect
Description     : Disconnect USB Device. 
Arguments       : none
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_DeviceDisconnect(void)
{
    hw_usb_pclear_dprpu();

    return (ARM_DRIVER_OK);
} /* End of function ARM_USBD_DeviceDisconnect */

/******************************************************************************
Function Name   : ARM_USBD_DeviceGetState
Description     : Get current USB Device State. 
Arguments       : none
Return value    : Device State ARM_USBD_STATE
******************************************************************************/
ARM_USBD_STATE ARM_USBD_DeviceGetState(void)
{
    ARM_USBD_STATE  usb_state;
    uint16_t        status;

    status = hw_usb_read_intsts();

    if (USB_VBSTS == (USB_VBSTS & status))
    {
        usb_state.vbus   = true;                    /* USB Device VBUS flag VBUS on */
    }
    else
    {
        usb_state.vbus   = false;                   /* USB Device VBUS flag VBUS off */
    }

    switch (usb_cstd_port_speed())
    {
        case USB_HSCONNECT:
            usb_state.speed  = ARM_USB_SPEED_HIGH;  /* USB Device speed setting (ARM_USB_SPEED_HIGH) */
        break;
        case USB_FSCONNECT:
            usb_state.speed  = ARM_USB_SPEED_FULL;  /* USB Device speed setting (ARM_USB_SPEED_FULL) */
        break;
        case USB_LSCONNECT:
            usb_state.speed  = ARM_USB_SPEED_LOW;   /* USB Device speed setting (ARM_USB_SPEED_LOW) */
        break;
        default:
        break;
    }

    if (USB_DS_CNFG == (USB_DVSQ & status))
    {
        usb_state.active = true;                    /* USB Device active flag */
    }
    else
    {
        usb_state.active = false;                   /* USB Device active flag */
    }

    return (usb_state);
} /* End of function ARM_USBD_DeviceGetState */

/******************************************************************************
Function Name   : ARM_USBD_DeviceRemoteWakeup
Description     : Trigger USB Remote Wakeup. 
Arguments       : none
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_DeviceRemoteWakeup(void)
{
    int32_t ret_val;

    usb_pstd_remote_wakeup();
    if (USB_OK == g_usb_pstd_remote_wakeup_state)
    {
        ret_val = ARM_DRIVER_OK;
    }
    else if (USB_QOVR == g_usb_pstd_remote_wakeup_state)
    {
        ret_val = ARM_DRIVER_ERROR_BUSY;
    }
    else
    {
        ret_val = ARM_DRIVER_ERROR;
    }

    return ret_val;
} /* End of function ARM_USBD_DeviceRemoteWakeup */

/******************************************************************************
Function Name   : ARM_USBD_DeviceSetAddress
Description     : Set USB Device Address. 
Arguments       : [in] dev_addr     Device Address
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_DeviceSetAddress(uint8_t dev_addr)
{
    return (ARM_DRIVER_OK);
} /* End of function ARM_USBD_DeviceSetAddress */

/******************************************************************************
Function Name   : ARM_USBD_ReadSetupPacket
Description     : Read setup packet received over Control Endpoint. 
Arguments       : [out] setup   Pointer to buffer for setup packet
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_ReadSetupPacket(uint8_t *setup)
{
    uint16_t    *p_setup;

    p_setup = (uint16_t*)setup;
    *p_setup = g_usb_pstd_req_type;     /* bRequest and bmRequestType */
    p_setup++;
    *p_setup = g_usb_pstd_req_value;    /* wValue */
    p_setup++;
    *p_setup = g_usb_pstd_req_index;    /* wIndex */
    p_setup++;
    *p_setup = g_usb_pstd_req_length;   /* wLength */

    return (ARM_DRIVER_OK);
} /* End of function ARM_USBD_ReadSetupPacket */

/******************************************************************************
Function Name   : ARM_USBD_EndpointConfigure
Description     : Configure USB Endpoint.
Arguments       : [in] ep_addr  Endpoint Address
                :               ep_addr.0..3: Address
                :               ep_addr.7: Direction
                : [in] ep_type  Endpoint Type (ARM_USB_ENDPOINT_xxx)
                : [in] ep_max_packet_size   Endpoint Maximum Packet Size 
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_EndpointConfigure(uint8_t ep_addr,
                                   uint8_t ep_type,
                                   uint16_t ep_max_packet_size)
{
    uint8_t     start_pipe_no;
    uint8_t     end_pipe_no;
    uint8_t     pipe_no;
    uint16_t    reg_value;
    uint16_t    pipe_cfg_value;
#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
    uint16_t    useport;
#endif /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

    if (ARM_USB_ENDPOINT_CONTROL == ep_type)
    {
        if (USB_EP0 != (USB_EP_NUMMASK & ep_addr))
        {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        pipe_cfg_value = ((USB_EP_DIRMASK & ep_addr) >> 3);
        if (USB_EP_DIRMASK != (USB_EP_DIRMASK & ep_addr))
        {
            pipe_cfg_value |= USB_SHTNAKON;
        }

        /* Unconfigure USB Endpoint */
        hw_usb_write_dcpcfg(pipe_cfg_value);           /* Set Pipe Configuration Register */
        hw_usb_write_dcpmxps(ep_max_packet_size);      /* Set Endpoint Maximum Packet Size */
        return (ARM_DRIVER_OK);
    }
    else if (ARM_USB_ENDPOINT_BULK == ep_type)
    {
        start_pipe_no   = USB_PIPE1;
        end_pipe_no     = USB_PIPE5;
        pipe_cfg_value  = USB_TYPFIELD_BULK | USB_CFG_DBLB;
        if (USB_EP_DIRMASK != (USB_EP_DIRMASK & ep_addr))
        {
            pipe_cfg_value |= USB_SHTNAKON;
        }
    }
    else if (ARM_USB_ENDPOINT_INTERRUPT == ep_type)
    {
        start_pipe_no   = USB_PIPE6;
        end_pipe_no     = USB_PIPE9;
        pipe_cfg_value  = USB_TYPFIELD_INT;
    }
    else if (ARM_USB_ENDPOINT_ISOCHRONOUS == ep_type)
    {
        start_pipe_no   = USB_PIPE1;
        end_pipe_no     = USB_PIPE2;
        pipe_cfg_value  = USB_TYPFIELD_ISO | USB_CFG_DBLB;
        if (USB_EP_DIRMASK != (USB_EP_DIRMASK & ep_addr))
        {
            pipe_cfg_value |= USB_SHTNAKON;
        }
    }
    else
    {
        return (ARM_DRIVER_ERROR_PARAMETER);
    }

    /* Loop USB Pipe resource */
    for (pipe_no = start_pipe_no; pipe_no < (end_pipe_no +1); pipe_no++)
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */

        /* Cheak Endpoint Address */
        if (USB_NULL == reg_value)
        {
            pipe_cfg_value |= (USB_EP_NUMMASK & ep_addr) | ((USB_EP_DIRMASK & ep_addr) >> 3);

#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
            useport = usb_cstd_pipe_to_fport(pipe_no);
            if ((USB_D0USE == useport) || (USB_D1USE == useport))
            {
                pipe_cfg_value |= USB_BFREON;
            }
#endif /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

            /* Unconfigure USB Endpoint */
            hw_usb_write_pipecfg(pipe_cfg_value);           /* Set Pipe Configuration Register */
            hw_usb_write_pipemaxp(ep_max_packet_size);      /* Set Endpoint Maximum Packet Size */
            return (ARM_DRIVER_OK);
        }
    }

    return (ARM_DRIVER_ERROR_BUSY);
} /* End of function ARM_USBD_EndpointConfigure */

/******************************************************************************
Function Name   : ARM_USBD_EndpointUnconfigure
Description     : Unconfigure USB Endpoint.
Arguments       : [in] ep_addr  Endpoint Address
                :               ep_addr.0..3: Address
                :               ep_addr.7: Direction
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_EndpointUnconfigure(uint8_t ep_addr)
{
    uint8_t     pipe_no;
    uint8_t     read_ep_addr;
    uint16_t    reg_value;

    if (USB_EP0 ==(USB_EPNUMFIELD & ep_addr))
    {
        /* Unconfigure USB Endpoint */
        hw_usb_write_dcpcfg(USB_NULL);     /* Clear Pipe Configuration Register */
        hw_usb_write_dcpmxps(USB_NULL);    /* Clear Endpoint Maximum Packet Size */
        return (ARM_DRIVER_OK);
    }

    /* Loop USB Pipe resource */
    for (pipe_no = USB_PIPE1; pipe_no < (USB_PIPE9 +1); pipe_no++)
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */
        read_ep_addr = (USB_EPNUMFIELD & reg_value) | ((USB_DIRFIELD & reg_value) << 3);

        /* Cheak Endpoint Address */
        if (read_ep_addr == ep_addr)
        {
            /* Unconfigure USB Endpoint */
            usb_cstd_clr_pipe_config (pipe_no);
            return (ARM_DRIVER_OK);
        }
    }

    return (ARM_DRIVER_ERROR_PARAMETER);
} /* End of function ARM_USBD_EndpointUnconfigure */

/******************************************************************************
Function Name   : ARM_USBD_EndpointStall
Description     : Set/Clear Stall for USB Endpoint.
Arguments       : [in] ep_addr  Endpoint Address
                :               ep_addr.0..3: Address
                :               ep_addr.7: Direction
                : [in] stall    Operation(false: Clear / true: Set)
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_EndpointStall(uint8_t ep_addr, bool stall)
{
    uint8_t     pipe_no;
    uint8_t     read_ep_addr;
    uint16_t    reg_value;

    if (USB_EP0 ==(USB_EPNUMFIELD & ep_addr))
    {
        if (true == stall)
        {
            usb_pstd_set_stall_pipe0();
        }
        else
        {
            usb_cstd_clr_stall(USB_EP0);
        }
        usb_pstd_ctrl_end((uint16_t) USB_CTRL_END); /* Control transfer stop(end) */
        return (ARM_DRIVER_OK);
    }

    /* Loop USB Pipe resource */
    for (pipe_no = USB_PIPE1; pipe_no < (USB_PIPE9 +1); pipe_no++)
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */
        read_ep_addr = (USB_EPNUMFIELD & reg_value) | ((USB_DIRFIELD & reg_value) << 3);

        /* Cheak Endpoint Address */
        if (read_ep_addr == ep_addr)
        {
            if (true == stall)
            {
                usb_pstd_set_stall(pipe_no);
            }
            else
            {
                usb_cstd_clr_stall(pipe_no);
            }
            return (ARM_DRIVER_OK);
        }
    }
    return (ARM_DRIVER_ERROR_PARAMETER);
} /* End of function ARM_USBD_EndpointStall */

/******************************************************************************
Function Name   : ARM_USBD_EndpointTransfer
Description     : Read data from or Write data to USB Endpoint.
Arguments       : [in] ep_addr  Endpoint Address
                :               ep_addr.0..3: Address
                :               ep_addr.7: Direction
                : [out] data    Pointer to buffer for data to read or with data to write 
                : [in]  num     Number of data bytes to transfer 
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_EndpointTransfer(uint8_t ep_addr, uint8_t *data, uint32_t num)
{
    uint8_t     pipe_no;
    uint8_t     read_ep_addr;
    uint16_t    reg_value;
    usb_er_t    err;
    int32_t     ret_code;

    if (USB_EP0 ==(USB_EPNUMFIELD & ep_addr))
    {
        if (USB_EP_DIRMASK != (USB_EP_DIRMASK & ep_addr))
        {
            if (USB_ON == g_usb_pstd_pipe0_request)
            {
                return ARM_DRIVER_ERROR_BUSY;
            }
            g_usb_read_request_size[USB_PIPE0] = num;
            usb_pstd_ctrl_write(num, data);
        }
        else
        {
            if ((USB_NULL == data) && (USB_NULL == num))
            {
                usb_cstd_set_buf((uint16_t) USB_PIPE0);   /* Set BUF */
                usb_pstd_ctrl_end((uint16_t) USB_CTRL_END); /* Control transfer stop(end) */
            }
            else
            {
                if (USB_ON == g_usb_pstd_pipe0_request)
                {
                    return ARM_DRIVER_ERROR_BUSY;
                }
                usb_pstd_ctrl_read(num, data);
            }
        }
        return (ARM_DRIVER_OK);
    }

    /* Loop USB Pipe resource */
    for (pipe_no = USB_PIPE1; pipe_no < (USB_PIPE9 +1); pipe_no++)
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */
        read_ep_addr = (USB_EPNUMFIELD & reg_value) | ((USB_DIRFIELD & reg_value) << 3);

        /* Cheak Endpoint Address */
        if (read_ep_addr == ep_addr)
        {
            g_usb_pstd_data[pipe_no].keyword    = pipe_no;              /* Pipe No */
            g_usb_pstd_data[pipe_no].p_tranadr  = data;                 /* Data address */
            g_usb_pstd_data[pipe_no].tranlen    = num;                  /* Data Size */
            g_usb_pstd_data[pipe_no].complete   = (usb_pcb_t)&usb_pstd_cmsis_transfer_complete;  /* Callback function */

            g_usb_read_request_size[pipe_no] = num;
            g_usb_pstd_cmsis_pipe_transfer_result_size[pipe_no] = USB_NULL;
            err = usb_pstd_transfer_start(&g_usb_pstd_data[pipe_no]);      /* USB Transfer Start */

            if (USB_OK == err)
            {
                ret_code = ARM_DRIVER_OK;
            }
            else if (USB_QOVR == err)
            {
                ret_code = ARM_DRIVER_ERROR_BUSY;
            }
            else
            {
                ret_code = ARM_DRIVER_ERROR;
            }

            return ret_code;
        }
    }
    return (ARM_DRIVER_ERROR_PARAMETER);
} /* End of function ARM_USBD_EndpointTransfer */

/******************************************************************************
Function Name   : ARM_USBD_EndpointTransferGetResult
Description     : Get result of USB Endpoint transfer. 
Arguments       : [in] ep_addr  Endpoint Address
                :               ep_addr.0..3: Address
                :               ep_addr.7: Direction
Return value    : number of successfully transferred data bytes
******************************************************************************/
uint32_t ARM_USBD_EndpointTransferGetResult(uint8_t ep_addr)
{
    uint8_t     pipe_no;
    uint8_t     read_ep_addr;
    uint16_t    reg_value;

    if (USB_EP0 ==(USB_EPNUMFIELD & ep_addr))
    {
        return (g_usb_pstd_cmsis_pipe_transfer_result_size[USB_PIPE0] - g_usb_pstd_data_cnt[USB_PIPE0]);
    }

    /* Loop USB Pipe resource */
    for (pipe_no = USB_PIPE1; pipe_no < (USB_PIPE9 +1); pipe_no++)
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */
        read_ep_addr = (USB_EPNUMFIELD & reg_value) | ((USB_DIRFIELD & reg_value) << 3);

        /* Cheak Endpoint Address */
        if (read_ep_addr == ep_addr)
        {
            return (g_usb_pstd_cmsis_pipe_transfer_result_size[pipe_no]);
        }
    }
    return USB_NULL;    /* Error */
} /* End of function ARM_USBD_EndpointTransferGetResult */

/******************************************************************************
Function Name   : ARM_USBD_EndpointTransferAbort
Description     : Abort current USB Endpoint transfer. 
Arguments       : [in] ep_addr  Endpoint Address
                :               ep_addr.0..3: Address
                :               ep_addr.7: Direction
Return value    : Status Error Codes
******************************************************************************/
int32_t ARM_USBD_EndpointTransferAbort(uint8_t ep_addr)
{
    uint8_t     pipe_no;
    uint8_t     read_ep_addr;
    uint16_t    reg_value;
    usb_er_t    err;

    if (USB_EP0 ==(USB_EPNUMFIELD & ep_addr))
    {
        err = usb_pstd_transfer_end(USB_PIPE0);
        if (USB_OK == err)
        {
            return (ARM_DRIVER_OK);
        }
        else
        {
            return (ARM_DRIVER_ERROR_PARAMETER);
        }
    }

    /* Loop USB Pipe resource */
    for (pipe_no = USB_PIPE1; pipe_no < (USB_PIPE9 +1); pipe_no++)
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */
        read_ep_addr = (USB_EPNUMFIELD & reg_value) | ((USB_DIRFIELD & reg_value) << 3);

        /* Cheak Endpoint Address */
        if (read_ep_addr == ep_addr)
        {
            err = usb_pstd_transfer_end(pipe_no);
            if (USB_OK == err)
            {
                return (ARM_DRIVER_OK);
            }
            else
            {
                return (ARM_DRIVER_ERROR_PARAMETER);
            }
        }
    }
    return (ARM_DRIVER_ERROR_PARAMETER);
} /* End of function ARM_USBD_EndpointTransferAbort */

/******************************************************************************
Function Name   : ARM_USBD_GetFrameNumber
Description     : Signal USB Device Event. 
Arguments       : [in] event    USBD Device Events
Return value    : none
******************************************************************************/
uint16_t ARM_USBD_GetFrameNumber(void)
{
    uint16_t frame_number;

    frame_number = hw_usb_read_frmnum();    /* Latest frame number */
    frame_number &= 0x07ff;

    return (frame_number);
} /* End of function ARM_USBD_GetFrameNumber */

/******************************************************************************
Function Name   : DUMMY_ARM_USBD_SignalDeviceEvent
Description     : Signal USB Device Event.
Arguments       : [in] event    USBD Device Events
Return value    : none
******************************************************************************/
static void DUMMY_ARM_USBD_SignalDeviceEvent(uint32_t event)
{

    if (ARM_USBD_EVENT_VBUS_ON & event)
    {
        /* USB Device VBUS On */
    }

    if (ARM_USBD_EVENT_VBUS_OFF & event)
    {
        /* USB Device VBUS Off */
    }

    if (ARM_USBD_EVENT_RESET & event)
    {
        /* USB Reset occurred */
    }

    if (ARM_USBD_EVENT_HIGH_SPEED & event)
    {
        /* USB switch to High Speed occurred */
    }

    if (ARM_USBD_EVENT_SUSPEND & event)
    {
        /* USB Suspend occurred */
    }

    if (ARM_USBD_EVENT_RESUME & event)
    {
        /* USB Resume occurred */
    }




} /* End of function DUMMY_ARM_USBD_SignalDeviceEvent */

/******************************************************************************
Function Name   : DUMMY_ARM_USBD_SignalEndpointEvent
Description     : Signal USB Endpoint Event. 
Arguments       : [in] ep_addr  Endpoint Address
                :               ep_addr.0..3: Address
                :               ep_addr.7: Direction
                : [in] event    USBD Endpoint Events
Return value    : none
******************************************************************************/
static void DUMMY_ARM_USBD_SignalEndpointEvent(uint8_t ep_addr, uint32_t ep_event)
{
    if (ARM_USBD_EVENT_SETUP & ep_event)
    {
        /* SETUP Packet */
    }

    if (ARM_USBD_EVENT_OUT & ep_event)
    {
        /* OUT Packet(s) */
    }

    if (ARM_USBD_EVENT_IN & ep_event)
    {
        /* IN Packet(s) */
    }



} /* End of function DUMMY_ARM_USBD_SignalEndpointEvent */

ARM_DRIVER_USBD Driver_USBD0 DATA_LOCATION_PRV_DRIVER_USB =
{
    ARM_USBD_GetVersion,
    ARM_USBD_GetCapabilities,
    ARM_USBD_Initialize,
    ARM_USBD_Uninitialize,
    ARM_USBD_PowerControl,
    ARM_USBD_DeviceConnect,
    ARM_USBD_DeviceDisconnect,
    ARM_USBD_DeviceGetState,
    ARM_USBD_DeviceRemoteWakeup,
    ARM_USBD_DeviceSetAddress,
    ARM_USBD_ReadSetupPacket,
    ARM_USBD_EndpointConfigure,
    ARM_USBD_EndpointUnconfigure,
    ARM_USBD_EndpointStall,
    ARM_USBD_EndpointTransfer,
    ARM_USBD_EndpointTransferGetResult,
    ARM_USBD_EndpointTransferAbort,
    ARM_USBD_GetFrameNumber
};

/******************************************************************************
 Function Name   : usb_pstd_cmsis_transfer_complete
 Description     : CallBack Function
 Argument        : usb_putr_t *mess           : Transfer result message
 Return          : none
 ******************************************************************************/
static void usb_pstd_cmsis_transfer_complete (usb_putr_t *p_mess, uint16_t data1, uint16_t data2)
{
    uint8_t     ep_addr;
    uint16_t    reg_value;
    uint16_t    pipe_no;
    uint32_t    ep_event;

    pipe_no = p_mess->keyword;         /* Pipe number setting */

    if (USB_PIPE0 == pipe_no)
    {
        reg_value = hw_usb_read_dcpcfg();      /* Read Pipe Configuration Register */
    }
    else
    {
        hw_usb_write_pipesel(pipe_no);          /* Select USB Pipe */
        reg_value = hw_usb_read_pipecfg();      /* Read Pipe Configuration Register */
    }

    if (USB_DIRFIELD == (USB_DIRFIELD & reg_value))
    {
        ep_event = (uint32_t)ARM_USBD_EVENT_IN;
        g_usb_pstd_cmsis_pipe_transfer_result_size[pipe_no] = g_usb_read_request_size[pipe_no];
    }
    else
    {
        ep_event = (uint32_t)ARM_USBD_EVENT_OUT;
        g_usb_pstd_cmsis_pipe_transfer_result_size[pipe_no] = g_usb_read_request_size[pipe_no] - p_mess->tranlen;
    }

    ep_addr = (USB_EPNUMFIELD & reg_value) | ((USB_DIRFIELD & reg_value) << 3);

    (gp_usb_pstd_cmsis_cb_endpoint_event)(ep_addr, ep_event);
} /* End of function usb_pstd_cmsis_transfer_complete() */

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
