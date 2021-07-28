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
 * File Name    : r_usb_reg_access.h
 * Description  : USB IP Register control code
 ******************************************************************************/
/*******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 30.11.2018 1.10    Supporting Smart Configurator
 ******************************************************************************/


/*******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#ifndef HW_USB_REG_ACCESS_H
#define HW_USB_REG_ACCESS_H

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#define USB_BUFSIZE_BIT   (10u)
#define USB_SUSPEND_MODE  (1u)
#define USB_NORMAL_MODE   (0)

#define USB0_CFIFO8     (USBFS->CFIFOL)
#define USB0_D0FIFO8    (USBFS->D0FIFOL)
#define USB0_D1FIFO8    (USBFS->D1FIFOL)
#define USB0_CFIFO16    (USBFS->CFIFO)
#define USB0_D0FIFO16   (USBFS->D0FIFO)
#define USB0_D1FIFO16   (USBFS->D1FIFO)

/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/*******************************************************************************
 Exported global variables
 ******************************************************************************/

/*******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
/****************/
/*  INITIARIZE  */
/****************/
void         hw_usb_hmodule_init(void) FUNC_LOCATION_PRV_DRIVER_USB;
void         hw_usb_pmodule_init( void ) FUNC_LOCATION_PRV_DRIVER_USB;


/************/
/*  SYSCFG  */
/************/
uint16_t hw_usb_read_syscfg (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_syscfg (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_dcfm (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_dcfm (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_drpd (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_usbe (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_usbe (void) FUNC_LOCATION_PRV_DRIVER_USB;
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
void hw_usb_pset_dprpu (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_pclear_dprpu (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */


/************/
/*  SYSSTS0 */
/************/
uint16_t hw_usb_read_syssts (void) FUNC_LOCATION_PRV_DRIVER_USB;

/**************/
/*  DVSTCTR0  */
/**************/
uint16_t hw_usb_read_dvstctr (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_dvstctr (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_rmw_dvstctr (uint16_t data, uint16_t width) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_dvstctr (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_vbout (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_vbout (void) FUNC_LOCATION_PRV_DRIVER_USB;
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
void hw_usb_hset_rwupe (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_rwupe (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_resume (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_resume (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_uact (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_uact (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
void hw_usb_pset_wkup (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/***************************/
/*  CFIFO, D0FIFO, D1FIFO  */
/***************************/
uint16_t hw_usb_read_fifo16 (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_fifo16 (uint16_t pipemode, uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_fifo8 (uint16_t pipemode, uint8_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************************************/
/*  CFIFOSEL, D0FIFOSEL, D1FIFOSEL  */
/************************************/
uint16_t hw_usb_read_fifosel (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_rmw_fifosel (uint16_t pipemode, uint16_t data, uint16_t width) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_dclrm (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_dclrm (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_dreqe (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_dreqe (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_mbw (uint16_t pipemode, uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_curpipe (uint16_t pipemode, uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/**********************************/
/* CFIFOCTR, D0FIFOCTR, D1FIFOCTR */
/**********************************/
uint16_t hw_usb_read_fifoctr (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_bval (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_bclr (uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;

/*************/
/*  INTENB0  */
/*************/
void hw_usb_write_intenb (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_intenb (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_enb_vbse (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_enb_sofe (void) FUNC_LOCATION_PRV_DRIVER_USB;
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
void hw_usb_pset_enb_rsme (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_pclear_enb_rsme (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/*************/
/*  BRDYENB  */
/*************/
void hw_usb_write_brdyenb (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_brdyenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_brdyenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/*************/
/*  NRDYENB  */
/*************/
void hw_usb_write_nrdyenb (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_nrdyenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_nrdyenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/*************/
/*  BEMPENB  */
/*************/
void hw_usb_write_bempenb (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_bempenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_bempenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/*************/
/*  SOFCFG   */
/*************/
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
void hw_usb_hset_trnensel (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_trnensel (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/*************/
/*  INTSTS0  */
/*************/
void hw_usb_write_intsts (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_sts_sofr (void) FUNC_LOCATION_PRV_DRIVER_USB;
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
uint16_t hw_usb_read_intsts (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_pclear_sts_resm (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_pclear_sts_valid (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/************/
/* BRDYSTS  */
/************/
uint16_t hw_usb_read_brdysts (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_brdysts (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_sts_brdy (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* NRDYSTS  */
/************/
void hw_usb_write_nrdy_sts (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_status_nrdy (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* BEMPSTS  */
/************/
void hw_usb_write_bempsts (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_status_bemp (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* FRMNUM   */
/************/
uint16_t hw_usb_read_frmnum (void) FUNC_LOCATION_PRV_DRIVER_USB;

#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
/************/
/* USBREQ   */
/************/
void hw_usb_hwrite_usbreq (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* USBVAL   */
/************/
void hw_usb_hset_usbval (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* USBINDX  */
/************/
void hw_usb_hset_usbindx (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* USBLENG  */
/************/
void hw_usb_hset_usbleng (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
/************/
/* USBREQ   */
/************/
uint16_t hw_usb_read_usbreq (void) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* USBVAL   */
/************/
uint16_t hw_usb_read_usbval (void) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* USBINDX  */
/************/
uint16_t hw_usb_read_usbindx (void) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* USBLENG  */
/************/
uint16_t hw_usb_read_usbleng (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/************/
/* DCPCFG   */
/************/
uint16_t hw_usb_read_dcpcfg (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_dcpcfg (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* DCPMAXP  */
/************/
uint16_t hw_usb_read_dcpmaxp (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_dcpmxps (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* DCPCTR   */
/************/
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
void hw_usb_hwrite_dcpctr (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_sureq (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
uint16_t hw_usb_read_dcpctr (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_pset_ccpl (void) FUNC_LOCATION_PRV_DRIVER_USB;

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_REPI */

/************/
/* PIPESEL  */
/************/
void hw_usb_write_pipesel (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* PIPECFG  */
/************/
uint16_t hw_usb_read_pipecfg (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_pipecfg (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* PIPEMAXP */
/************/
uint16_t hw_usb_read_pipemaxp (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_pipemaxp (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* PIPEPERI */
/************/
void hw_usb_write_pipeperi (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/********************/
/* DCPCTR, PIPEnCTR */
/********************/
uint16_t hw_usb_read_pipectr (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_write_pipectr (uint16_t pipeno, uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_aclrm (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_aclrm (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_sqclr (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_sqset (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_pid (uint16_t pipeno, uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_pid (uint16_t pipeno, uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* PIPEnTRE */
/************/
void hw_usb_set_trenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_trenb (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_trclr (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* PIPEnTRN */
/************/
void hw_usb_write_pipetrn (uint16_t pipeno, uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* BCCTRL   */
/************/
void hw_usb_set_bcctrl (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_bcctrl (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t hw_usb_read_bcctrl (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_vdmsrce (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_vdmsrce (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_idpsinke (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_idpsinke (void) FUNC_LOCATION_PRV_DRIVER_USB;
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
void hw_usb_hset_dcpmode (void) FUNC_LOCATION_PRV_DRIVER_USB;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

void hw_usb_set_vdcen(void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_vddusbe(void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_batchge(void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_batchge(void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_set_cnen(void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_clear_cnen(void) FUNC_LOCATION_PRV_DRIVER_USB;

/**********************************/
/*  DMA0CFG, DMA1CFG  for 597ASSP */
/**********************************/

#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
/*************/
/*  INTENB1  */
/*************/
void hw_usb_hwrite_intenb (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_enb_ovrcre (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_enb_ovrcre (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_enb_bchge (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_enb_bchge (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_enb_dtche (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_enb_dtche (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_enb_attche (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_enb_attche (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_enb_signe (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_enb_sacke (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hset_enb_pddetinte (void) FUNC_LOCATION_PRV_DRIVER_USB;

/*************/
/*  INTSTS1  */
/*************/
void hw_usb_hwrite_intsts (uint16_t data) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_sts_ovrcr (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_sts_bchg (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_sts_dtch (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_sts_attch (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_sts_sign (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_sts_sack (void) FUNC_LOCATION_PRV_DRIVER_USB;
void hw_usb_hclear_sts_pddetint (void) FUNC_LOCATION_PRV_DRIVER_USB;

/************/
/* DEVADDn  */
/************/

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#endif /* HW_USB_REG_ACCESS_H */
/******************************************************************************
 End of file
 ******************************************************************************/
