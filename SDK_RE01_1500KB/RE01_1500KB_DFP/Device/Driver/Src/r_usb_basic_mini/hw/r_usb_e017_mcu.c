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
  ******************************************************************************/
/*******************************************************************************
 * File Name    : r_usb_e017_mcu.c
 * Description  : R7F0E01 MCU processing
 *****************************************************************************/
/******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.09.2014 1.00    First Release
*******************************************************************************/
/* Start user code for pragma. Do not edit comment generated here. */
/* End user code. Do not edit comment generated here. */

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_basic_mini_api.h"
#include "r_usb_bitdefine.h"
#include "r_usb_typedef.h"
#include "r_usb_reg_access.h"            /* Definition of the USB register access macro */
#include "r_usb_extern.h"

#if ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE))
#include "r_usb_dmac.h"
#endif  /* ((USB_CFG_DTC == USB_CFG_ENABLE) || (USB_CFG_DMA == USB_CFG_ENABLE)) */

#include "r_system_api.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#if defined(USB_CFG_HHID_USE) || defined(USB_CFG_PHID_USE)
  #if USB_CFG_DTC == USB_CFG_ENABLE
    #error  In HID class, can not set USB_CFG_ENABLE to USB_CFG_DTC in r_usb_basic_config.h.
  #endif /* USB_CFG_DTC == USB_CFG_ENABLE */

  #if USB_CFG_DMA == USB_CFG_ENABLE
    #error  In HID class, can not set USB_CFG_ENABLE to USB_CFG_DMA in r_usb_basic_config.h.
  #endif /* USB_CFG_DMA == USB_CFG_ENABLE */

#endif /* defined(USB_CFG_HHID_USE) || defined(USB_CFG_PHID_USE) */


#if USB_CFG_BC == USB_CFG_DISABLE
  #if USB_CFG_DCP == USB_CFG_ENABLE
    #error  Can not set USB_CFG_DISABLE to USB_CFG_BC when setting USB_CFG_ENABLE to USB_CFG_DCP \
         in r_usb_basic_config.h.

  #endif /* USB_CFG_DCP == USB_CFG_ENABLE */
#endif /* USB_CFG_BC == USB_CFG_DISABLE */


#if USB_CFG_DMA == USB_CFG_ENABLE
  #if USB_CFG_DTC == USB_CFG_ENABLE
    #error  Can not set USB_CFG_ENABLE to the definitions (USB_CFG_DMA and USB_CFG_DTC) at the same time \
         in r_usb_basic_config.h.

  #endif /* USB_CFG_DTC == USB_CFG_ENABLE */
#endif /* USB_CFG_DMA == USB_CFG_ENABLE */


#if USB_CFG_MODE == USB_CFG_HOST
  #if defined(USB_CFG_PCDC_USE) || defined(USB_CFG_PHID_USE) || defined(USB_CFG_PMSC_USE) || defined(USB_CFG_PVNDR_USE)
    #error  Can not enable these definitions(USB_CFG_PCDC_USE/USB_CFG_PHID_USE/USB_CFG_PMSC_USE/USB_CFG_PVNDR_USE) \
        when setting USB_HOST to USB_CFG_MODE in r_usb_basic_config.h.

  #endif /* defined(USB_CFG_PCDC_USE || USB_CFG_PHID_USE || USB_CFG_PMSC_USE || USB_CFG_PVNDR_USE) */
#endif /* USB_CFG_MODE == USB_HOST */


#if USB_CFG_MODE == USB_CFG_PERI
  #if defined(USB_CFG_HCDC_USE) || defined(USB_CFG_HHID_USE) || defined(USB_CFG_HMSC_USE) || defined(USB_CFG_HVNDR_USE)
    #error  Can not enable these definitions(USB_CFG_HCDC_USE/USB_CFG_HHID_USE/USB_CFG_HMSC_USE/USB_CFG_HVNDR_USE) \
        when setting USB_PERI to USB_CFG_MODE in r_usb_basic_config.h.

  #endif /* defined(USB_CFG_HCDC_USE || USB_CFG_HHID_USE || USB_CFG_HMSC_USE || USB_CFG_HVNDR_USE) */
#endif /* USB_CFG_MODE == USB_PERI */

/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static void usb_cpu_usb_interrupt (void) FUNC_LOCATION_PRV_DRIVER_USB;
void usb_cpu_d0fifo_isr(void) FUNC_LOCATION_PRV_DRIVER_USB;
void usb_cpu_d1fifo_isr(void) FUNC_LOCATION_PRV_DRIVER_USB;


/******************************************************************************
 Function Name   : usb_module_start
 Description     : USB module start
 Arguments       : none
 Return value    : none
 ******************************************************************************/
usb_err_t usb_module_start (void)
{
    /* Enable power for USB0 */
    MSTP->MSTPCRB_b.MSTPB11 = 0u;

    SYSTEM->PRCR = 0xA503U;     /* Protect OFF */
    
    SYSTEM->VOCR_b.UVCTL = 0U;  /* USB */
    SYSTEM->VOCR_b.IV1CTL = 0U; /* P202 ~ P204 */
    
    SYSTEM->PRCR = 0xA500U;     /* Protect ON */
    
#if USB_CFG_BC == USB_CFG_ENABLE
    hw_usb_set_vddusbe();
#endif  /* USB_CFG_BC == USB_CFG_ENABLE */

    return USB_SUCCESS;
} /*End of function usb_module_start */

/******************************************************************************
 Function Name   : usb_module_stop
 Description     : USB module stop
 Arguments       : none
 Return value    : none
 ******************************************************************************/
usb_err_t usb_module_stop (void)
{
    USBFS->DVSTCTR0 = 0;
    USBFS->DCPCTR = USB_SQSET;
    USBFS->PIPE1CTR = 0;
    USBFS->PIPE2CTR = 0;
    USBFS->PIPE3CTR = 0;
    USBFS->PIPE4CTR = 0;
    USBFS->PIPE5CTR = 0;
    USBFS->PIPE6CTR = 0;
    USBFS->PIPE7CTR = 0;
    USBFS->PIPE8CTR = 0;
    USBFS->PIPE9CTR = 0;
    USBFS->BRDYENB = 0;
    USBFS->NRDYENB = 0;
    USBFS->BEMPENB = 0;
    USBFS->INTENB0 = 0;
    USBFS->INTENB1 = 0;
    USBFS->SYSCFG &= (~USB_DPRPU);
    USBFS->SYSCFG &= (~USB_DRPD);
    USBFS->SYSCFG &= (~USB_USBE);
    USBFS->SYSCFG &= (~USB_DCFM);
    USBFS->BRDYSTS = 0;
    USBFS->NRDYSTS = 0;
    USBFS->BEMPSTS = 0;

    /* Disable power for USB0 */
    MSTP->MSTPCRB_b.MSTPB11 = 1u;

    return USB_SUCCESS;
} /*End of function usb_module_stop */

/******************************************************************************
 Function Name   : usb_cpu_usbint_init
 Description     : USB interrupt Initialize
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_cpu_usbint_init (void)
{
    /* USBI */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBI);
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBI, USB_CFG_INTERRUPT_PRIORITY);
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBI);
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBI, USBI_FS, &usb_cpu_usb_interrupt);

    /* USBR */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBR);
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBR, USB_CFG_INTERRUPT_PRIORITY);

    /* D0FIFO(RX) */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_USBFS_D0FIFO);
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_USBFS_D0FIFO, D0FIFO_FS, &usb_cpu_d0fifo_isr);

    /* D1FIFO(TX) */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_USBFS_D1FIFO);
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_USBFS_D1FIFO, D1FIFO_FS, &usb_cpu_d1fifo_isr);
} /*End of function usb_cpu_usbint_init */

/******************************************************************************
 Function Name   : usb_cpu_delay_1us
 Description     : 1us Delay timer
 Arguments       : uint16_t  time        ; Delay time(*1us)
 Return value    : none
 ******************************************************************************/
void usb_cpu_delay_1us (uint16_t time)
{
    volatile register uint16_t  i;

    /* Wait 1us (Please change for your MCU) */
    for(i = 0; i < (5 * time); ++i) /* 12MHz */
    {
    };
} /*End of function usb_cpu_delay_1us */

/******************************************************************************
 Function Name   : usb_cpu_delay_xms
 Description     : xms Delay timer
 Arguments       : uint16_t  time        ; Delay time(*1ms)
 Return value    : none
 ******************************************************************************/
void usb_cpu_delay_xms (uint16_t time)
{
    /* Wait xms (Please change for your MCU) */
    volatile register uint32_t  i;

    /* Wait 1ms */
        /* When "ICLK=120MHz" is set, this code is waiting for 1ms.
          Please change this code with CPU Clock mode. */
    for(i = 0; i < (5500 * time); ++i) /* 12MHz */
    {
    };
} /*End of function usb_cpu_delay_xms */

#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
/******************************************************************************
 Function Name   : usb_cpu_int_enable
 Description     : USB Interrupt Enable
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_cpu_int_enable (void)
{
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBI);
} /*End of function usb_cpu_int_enable */

/******************************************************************************
 Function Name   : usb_cpu_int_disable
 Description     : USB Interrupt disable
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_cpu_int_disable (void)
{
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_USBFS_USBI);
} /*End of function usb_cpu_int_disable */

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */



/******************************************************************************
 Function Name   : usb_chattering
 Description     : Remove chattering processing
 Arguments       : uint16_t *p_syssts : SYSSTS register value
 Return value    : LNST bit value
 ******************************************************************************/
uint16_t usb_chattering (uint16_t *p_syssts)
{
    uint16_t lnst[4];

    while (1)
    {
        lnst[0] = (*p_syssts) & USB_LNST;
        usb_cpu_delay_xms((uint16_t) 1); /* 1ms wait */
        lnst[1] = (*p_syssts) & USB_LNST;
        usb_cpu_delay_xms((uint16_t) 1); /* 1ms wait */
        lnst[2] = (*p_syssts) & USB_LNST;
        if ((lnst[0] == lnst[1]) && (lnst[0] == lnst[2]))
        {
            break;
        }
    }
    return lnst[0];
} /*End of function usb_chattering */


/******************************************************************************
 Function Name   : usb_cpu_usb_interrupt
 Description     : USB interrupt Handler
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_cpu_usb_interrupt (void)
{
    /* Call USB interrupt routine */
    if (USB_HOST == g_usb_cstd_usb_mode)
    {
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
    usb_hstd_interrupt_handler();

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
    }
    else
    {
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
        usb_pstd_usb_handler(); /* Call interrupt routine */

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */
    }
} /*End of function usb_cpu_usb_interrupt */


/******************************************************************************
Function Name   : usb_cpu_d0fifo_isr
Description     : D0FIFO interrupt Handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_d0fifo_isr(void)
{
#if (USB_CFG_DTC == USB_CFG_ENABLE)
    usb_cstd_dma_send_complete(USB_D0USE);
#endif /* USB_CFG_DTC == USB_CFG_ENABLE */
} /*End of function usb_cpu_d0fifo_isr */


/******************************************************************************
Function Name   : usb_cpu_d1fifo_isr
Description     : D0FIFO interrupt Handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_d1fifo_isr(void)
{
#if (USB_CFG_DTC == USB_CFG_ENABLE)
    usb_cstd_dma_send_complete(USB_D1USE);
#endif /* USB_CFG_DTC == USB_CFG_ENABLE */
}/*End of function usb_cpu_d1fifo_isr */


/******************************************************************************
End Of File
******************************************************************************/

