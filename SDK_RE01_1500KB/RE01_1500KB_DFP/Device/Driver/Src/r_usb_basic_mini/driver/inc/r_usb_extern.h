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
 * File Name    : r_usb_extern.h
 * Description  : USB common extern header
 ******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.09.2014 1.00    First Release
*           : 01.06.2015 1.01    Added RX231.
*           : 30.11.2018 1.10    Supporting Smart Configurator
*******************************************************************************/

/*******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
 
/*******************************************************************************
 Macro definitions
 ******************************************************************************/
#ifndef R_USB_EXTERN_H
#define R_USB_EXTERN_H


/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/*******************************************************************************
 Exported global variables
 ******************************************************************************/
/* r_usb_usbif_api.c */
extern uint16_t         g_usb_cstd_usb_mode;                         /* USB mode HOST/PERI */
extern uint32_t         g_usb_read_request_size[USB_MAXPIPE_NUM + 1];
extern uint16_t         g_usb_cstd_open_class;


/* r_usb_cdataio.c */
extern usb_pipe_table_t  g_usb_cstd_pipe_tbl[USB_MAXPIPE_NUM+1];
extern uint16_t     g_usb_cstd_bemp_skip[USB_MAX_PIPE_NO + 1u];

#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
/* r_usb_cdataio.c */
extern usb_putr_t       *gp_usb_pstd_pipe[USB_MAX_PIPE_NO + 1u];
extern uint32_t          g_usb_pstd_data_cnt[USB_MAX_PIPE_NO + 1u]; /* PIPEn Buffer counter */
extern uint8_t          *gp_usb_pstd_data[USB_MAX_PIPE_NO + 1u];    /* PIPEn Buffer pointer(8bit) */

/* r_usb_pdriver.c */
extern uint16_t     g_usb_pstd_is_stall_pipe[USB_MAX_PIPE_NO + 1u]; /* Stall Pipe info */
extern usb_pcb_t    g_usb_pstd_stall_cb;      /* Stall Callback function */
extern uint16_t     g_usb_pstd_remote_wakeup_state;                /* Result for Remote wake up */
extern uint16_t     g_usb_pstd_eptbl_index[2][USB_MAX_EP_NO + 1u]; /* Index of Endpoint Information table */
extern uint16_t     g_usb_pstd_req_type;     /* Request type */
extern uint16_t     g_usb_pstd_req_value;    /* Value */
extern uint16_t     g_usb_pstd_req_index;    /* Index */
extern uint16_t     g_usb_pstd_req_length;   /* Length */
extern usb_pcdreg_t g_usb_pstd_driver;       /* Driver registration */
extern usb_setup_t  g_usb_pstd_req_reg;      /* Request variable */
extern usb_int_t    g_usb_pstd_usb_int;
extern uint16_t     g_usb_pstd_pipe0_request;
extern uint16_t     g_usb_pstd_std_request;


/* r_usb_usbif_api.c */
extern usb_putr_t       g_usb_pstd_data[USB_MAXPIPE_NUM + 1];

/* Driver_USBD.c */
extern ARM_USBD_SignalDeviceEvent_t     gp_usb_pstd_cmsis_cb_device_event;
extern ARM_USBD_SignalEndpointEvent_t   gp_usb_pstd_cmsis_cb_endpoint_event;

#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */

#if (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST
/* r_usb_hstdfunction.c */
extern usb_pipe_no_t    g_usb_hstd_current_pipe;               /* Pipe number */
extern usb_hutr_t      *gp_usb_hstd_pipe[];                    /* Message pipe */
extern usb_leng_t       g_usb_hstd_data_cnt[];                 /* PIPEn Buffer counter */
extern uint8_t         *gp_usb_hstd_data_ptr[];                /* PIPEn Buffer pointer(8bit) */
extern usb_intinfo_t    g_usb_hstd_int_msg[];
extern uint8_t          g_usb_hstd_int_msg_cnt;
extern usb_addr_t       g_usb_hcd_devaddr;                     /* Device address */
extern uint8_t          g_usb_hcd_ctsq;                        /* Control transfer stage management */
extern usb_port_t       g_usb_hstd_hcd_port;
extern usb_hcdreg_t     g_usb_hcd_driver;                      /* Device driver (registration) */
extern usb_hcddevinf_t  g_usb_hcd_dev_info;                    /* device address, status, config, speed, */
extern uint8_t          g_usb_hcd_remote_wakeup;
extern usb_addr_t       g_usb_mgr_devaddr;                     /* Device address */
extern uint8_t          g_usb_mgr_seq;
extern uint8_t          g_usb_mgr_seq_mode;
extern usb_tskinfo_t   *gp_usb_hstd_hcd_message;               /* Hcd Task receive message */
extern uint16_t         g_usb_hstd_hcd_dcp_mxps[USB_MAXDEVADDR+1u];    /* DEVSEL & DCPMAXP (Multiple device) */
/* r_usb_hstdfunction.c scheduler */
extern usb_msg_t       *g_usb_hstd_msg_tbl_sch[][USB_TABLEMAX];
extern uint8_t          g_usb_hstd_read_sch[];
extern uint8_t          g_usb_hstd_write_sch[];
extern usb_tskinfo_t    g_usb_hstd_block[];
extern usb_waitinfo_t   g_usb_hstd_wait_msg_sch[];
extern usb_waitque_t    g_usb_hstd_wait_que_sch[];
extern usb_msg_t       *gp_usb_hstd_next_addr_sch;
extern uint8_t          g_usb_hstd_next_id_sch;                /* Priority Counter */
extern uint8_t          g_usb_next_read_ptr_sch;               /* Priority Table Reading pointer */

/* r_usb_hdriver.c */
extern usb_port_t       g_usb_mgr_port;
extern uint8_t          g_usb_hstd_devspd;                     /* Reset handshake result */
extern uint8_t          g_usb_hstd_cfg_descriptor[USB_CONFIGSIZE];
extern usb_hutr_t       g_usb_mgr_ctrl_msg;
extern usb_tskinfo_t   *gp_usb_mgr_msg;                        /* Mgr Task receive message */
extern uint16_t         g_usb_enum_seq;                        /* Enumeration Sequence */
extern usb_hcdrequest_t g_usb_hstd_request;                    /* Control Transfer Request field */

/* r_usb_usbif_api.c */
extern uint16_t         g_usb_hstd_is_susp_resm;
extern usb_hutr_t       g_usb_hstd_data[USB_MAXPIPE_NUM + 1];

extern uint16_t         g_usb_hstd_pipe_request[USB_MAX_PIPE_NO + 1u];


#endif /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

#if USB_CFG_BC == USB_CFG_ENABLE
#if (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI
/* r_usb_pbc.c */
extern uint16_t        g_usb_pstd_bc_detect;
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
/* r_usb_hbc.c */
extern usb_bc_status_t  g_usb_hstd_bc;
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */
#endif  /* USB_CFG_BC == USB_CFG_ENABLE */


#if USB_CFG_COMPLIANCE == USB_CFG_ENABLE
extern uint16_t         g_usb_hstd_response_counter;
#endif /* USB_CFG_COMPLIANCE == USB_CFG_ENABLE */

#if defined(USB_CFG_HMSC_USE)
extern uint8_t          g_usb_hmsc_sub_class;
#endif /* defined(USB_CFG_HMSC_USE) */

#if defined(USB_CFG_HHID_USE)
extern uint8_t         *gp_usb_hstd_interface_tbl;
#endif /* defined(USB_CFG_HMSC_USE) */

/*******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
/* r_usb_rx_mcu.c */
usb_err_t usb_module_start (void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_err_t usb_module_stop (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cpu_delay_xms (uint16_t time) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cpu_delay_1us (uint16_t time) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cpu_usbint_init (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_chattering (uint16_t *p_syssts) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cpu_int_enable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cpu_int_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_cdataio.c */
void      usb_cstd_debug_hook (uint16_t error_code) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_creg_abs.c */
uint16_t  usb_cstd_is_set_frdy (uint16_t pipe, uint16_t fifosel, uint16_t isel) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_chg_curpipe (uint16_t pipe, uint16_t fifosel, uint16_t isel) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_set_transaction (uint16_t pipe, uint16_t trncnt) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_clr_transaction (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_set_pipe_config (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_clr_pipe_config (uint16_t pipeno) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_cstd_get_maxpacket_size (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_clibusbip.c */
void      usb_cstd_nrdy_enable (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_cstd_port_speed (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_cstd_get_pipe_dir (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_cstd_get_pipe_type (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_do_aclrm (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_set_buf (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_clr_stall (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_usb_task (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_set_nak (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_pipe_table_clear (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_cstd_pipe_reg_clear (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_cstd_pipe_to_fport (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_cstd_get_pid (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;


#if defined(USB_CFG_HMSC_USE)
uint16_t         usb_hmsc_pipe_info(uint8_t *table, uint16_t length) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hmsc_process (usb_tskinfo_t *mess) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* defined(USB_CFG_HMSC_USE) */




#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)

/* r_usb_plibusbip.c */
void      usb_pstd_send_start (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_buf_to_fifo (uint16_t pipe, uint16_t  useport) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_pstd_write_data (uint16_t pipe, uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_receive_start (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_fifo_to_buf (uint16_t pipe, uint16_t  useport) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_pstd_read_data (uint16_t pipe, uint16_t pipemode) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_data_end (uint16_t pipe, uint16_t status) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_nrdy_pipe_process(uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_bemp_pipe_process(uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_brdy_pipe_process(uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;


/* r_usb_preg_abs.c */
uint8_t  *usb_pstd_write_fifo (uint16_t count, uint16_t pipemode, uint8_t  * p_buff) FUNC_LOCATION_PRV_DRIVER_USB;
uint8_t  *usb_pstd_read_fifo (uint16_t count, uint16_t pipemode, uint8_t  * p_buff) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_forced_termination (uint16_t pipe, uint16_t status) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_interrupt_clock (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_self_clock (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_stop_clock (void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_pinthandler_usbip0.c */
void      usb_pstd_usb_handler (void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_pdriver.c */
void      usb_pstd_pcd_task (void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t  usb_pstd_set_submitutr (usb_putr_t * p_utrmsg) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_clr_eptbl_index (void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t  usb_pstd_transfer_start (usb_putr_t *p_utr) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t  usb_pstd_transfer_end (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_driver_registration (usb_pcdreg_t *p_callback) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_driver_release (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_dummy_function (usb_putr_t *p_utr, uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_dummy_trn (usb_setup_t * p_req, uint16_t ctsq) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_pstd_epadr_to_pipe(uint16_t dir_ep) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_pcontrolrw.c */
uint16_t  usb_pstd_ctrl_read (uint32_t bsize, uint8_t  *p_table) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_ctrl_write (uint32_t bsize, uint8_t  *p_table) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_ctrl_end (uint16_t status) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_pintfifo.c */
void      usb_pstd_brdy_pipe (uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_nrdy_pipe (uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_bemp_pipe (uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_pstdfunction.c */
void      usb_pstd_driver_init(void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_preg_abs.c */
void      usb_pstd_interrupt_handler (uint16_t * p_type, uint16_t * p_status) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_save_request (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_pstd_chk_configured (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_bus_reset (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_remote_wakeup (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_attach_process (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_detach_process (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_suspend_process (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_resume_process (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t  usb_pstd_chk_vbsts (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_set_stall (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_set_stall_pipe0 (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_select_nak (uint16_t pipe) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_pstdrequest.c */
void      usb_pstd_stand_req4 (void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_stand_req5 (void) FUNC_LOCATION_PRV_DRIVER_USB;

/* peri_processing.c */
void      usb_pstd_registration(void) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_devdefault (usb_putr_t *p_utr, uint16_t mode, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_detach (usb_putr_t *p_utr, uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_suspended(usb_putr_t *p_utr, uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_resume(usb_putr_t *p_utr, uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_class_request(usb_setup_t *p_req, uint16_t ctsq) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_class_request_ioss (usb_setup_t *p_req) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_class_request_rwds (usb_setup_t * p_req) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_class_request_wds (usb_setup_t * p_req) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_other_request (usb_setup_t *p_req) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_class_request_wnss (usb_setup_t *p_req) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_class_request_rss (usb_setup_t *p_req) FUNC_LOCATION_PRV_DRIVER_USB;
void      usb_pstd_class_request_wss (usb_setup_t *p_req) FUNC_LOCATION_PRV_DRIVER_USB;


#if USB_CFG_BC == USB_CFG_ENABLE
/* r_usb_hbc.c */
   void      usb_pstd_bc_detect_process(void) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* USB_CFG_BC == USB_CFG_ENABLE */



#endif/* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI) */

#if (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST

/* r_usb_hreq_abs.c */
void             usb_hstd_check_interrupt_source(usb_strct_t *p_inttype, uint16_t *p_bitsts) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_forced_termination(uint16_t status) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_interrupt_handler(void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_hsignal.c */
void             usb_hstd_detach_control(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_attach_control(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_int_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_interrupt_process(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_vbus_control_on(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_vbus_control_off(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_suspend_process(void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_strct_t      usb_hstd_port_speed(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_set_hw_function(void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_hstdfunction.c */
uint8_t          usb_hstd_check_device_address(void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_addr_t       usb_hstd_pipe_to_addr(void) FUNC_LOCATION_PRV_DRIVER_USB;
uint8_t          usb_hstd_enumeration(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_update_devinfo(uint8_t state) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_update_devstate(uint8_t state) FUNC_LOCATION_PRV_DRIVER_USB;
usb_port_t       usb_hstd_devaddr_to_port(void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t         usb_hstd_get_descriptor(uint16_t CntValue) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t         usb_hstd_set_configuration(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hclass_request_complete (usb_hutr_t *p_mess) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_sw_reset(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_class_check(uint8_t **pp_table) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_class_enumeration(usb_tskinfo_t *p_mess) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_hw_start(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_status_notification(usb_strct_t msginfo, usb_strct_t keyword) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_setup_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_port_enable(void) FUNC_LOCATION_PRV_DRIVER_USB;

void             usb_hstd_class_task (void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_read_complete (usb_hutr_t *p_mess) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_write_complete (usb_hutr_t *p_mess) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_suspend_complete (uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_resume_complete (uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_dummy_function(usb_hutr_t *p_utr, uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_driver_reg_init(void) FUNC_LOCATION_PRV_DRIVER_USB;  /* Initialized g_usb_HcdDriver */
void             usb_hstd_clr_device_info(void) FUNC_LOCATION_PRV_DRIVER_USB;     /* Initialized g_usb_HcdDevInfo */
void             usb_hstd_pipe_to_hcddevaddr(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_connect_err_event_set(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_set_devaddx_register(uint8_t usbspd) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_buf_to_fifo(uint16_t useport) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_fifo_to_buf(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_control_read_start(void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t         usb_hstd_control_write_start(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_status_start(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_setup_start(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_control_end(usb_strct_t status) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_cstd_init_library(void) FUNC_LOCATION_PRV_DRIVER_USB;
uint8_t          usb_hstd_devaddr_to_speed(void) FUNC_LOCATION_PRV_DRIVER_USB;
uint8_t          usb_hstd_check_attach(void) FUNC_LOCATION_PRV_DRIVER_USB;


/* r_usb_creg_abs.c */
void             usb_hstd_bus_reset(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_detach_process(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_attach_process(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_resume_process(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_read_lnst(uint16_t *p_buf) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_bemp_pipe(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_nrdy_pipe(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_brdy_pipe(void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t         usb_hstd_set_feature(uint16_t epnum) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t         usb_hstd_clear_feature(uint16_t epnum) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t         usb_hstd_status_change(usb_strct_t msginfo, usb_strct_t keyword) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_status_result(usb_strct_t status) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_transferend_check(usb_strct_t status) FUNC_LOCATION_PRV_DRIVER_USB;
uint8_t          usb_hstd_pipe_to_epadr(void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_hlibusbip.c */
void             usb_hstd_send_start(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_receive_start(void) FUNC_LOCATION_PRV_DRIVER_USB;
uint8_t          usb_cstd_pipe_table_set (uint16_t usb_class, uint8_t *p_descriptor) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_data_end(uint16_t status) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_bchg_enable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_bchg_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_set_uact(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_ovrcr_clear_status(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_ovrcr_enable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_ovrcr_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_attch_clear_status(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_detch_clear_status(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_attch_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_detach_enable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_detach_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_bus_int_disable(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_host_registration (void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_class_driver_start (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint8_t          usb_hstd_get_pipe_no (uint16_t usb_class, uint8_t type, uint8_t dir) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t         usb_hstd_get_pipe_peri_value (uint8_t binterval) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_brdy_pipe_process(uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_nrdy_pipe_process(uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_bemp_pipe_process(uint16_t bitsts) FUNC_LOCATION_PRV_DRIVER_USB;


/* r_usb_hdriver.c */
void             usb_hstd_hcd_task(void) FUNC_LOCATION_PRV_DRIVER_USB;
void             usb_hstd_mgr_task(void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_usbif_api.c  */
void             usb_hstd_driver_init(void) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_hdriverapi.c */
void        usb_hstd_driver_registration(usb_hcdreg_t *p_registinfo) FUNC_LOCATION_PRV_DRIVER_USB;
void        usb_hstd_driver_release(uint8_t devclass) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_transfer_start(usb_hutr_t *p_utr_table) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_change_device_state(usb_cbinfo_t complete, usb_strct_t msginfo, usb_strct_t keyword) FUNC_LOCATION_PRV_DRIVER_USB;
void        usb_hstd_return_enu_mgr(uint16_t cls_result) FUNC_LOCATION_PRV_DRIVER_USB;

/* r_usb_hscheduler.c */
uint8_t     usb_hstd_scheduler(void) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_rec_msg( uint8_t id, usb_msg_t** pp_mess ) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_snd_msg( uint8_t id, usb_msg_t* p_mess ) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_isnd_msg( uint8_t id, usb_msg_t* p_mess ) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_wai_msg( uint8_t id, usb_msg_t* p_mess, uint16_t count ) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_pget_send( uint8_t id, usb_strct_t msginfo, usb_cbinfo_t complete, usb_strct_t keyword ) FUNC_LOCATION_PRV_DRIVER_USB;
usb_er_t    usb_hstd_rel_blk( uint8_t blk_num ) FUNC_LOCATION_PRV_DRIVER_USB;

#if USB_CFG_BC == USB_CFG_ENABLE
/* r_usb_hbc.c */

   void      usb_hstd_pddetint_process(void) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* USB_CFG_BC == USB_CFG_ENABLE */



#endif /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */




#if defined(USB_CFG_HVND_USE)
void        vnd_registration (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t    usb_hvnd_pipe_info(uint8_t *table, uint16_t length) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* defined(USB_CFG_HVND_USE) */

#if defined(USB_CFG_HHID_USE)
void        hid_registration (void) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t    usb_hhid_pipe_info(uint8_t *table, uint16_t length) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* defined(USB_CFG_HHID_USE) */

#if defined(USB_CFG_HCDC_USE)
void        cdc_registration(void) FUNC_LOCATION_PRV_DRIVER_USB;                        /* Sample driver registration */
void        set_control_line_state(usb_ctrl_t *p_ctrl) FUNC_LOCATION_PRV_DRIVER_USB;
void        get_line_coding(usb_ctrl_t *p_ctrl) FUNC_LOCATION_PRV_DRIVER_USB;
void        usb_hcdc_device_state(uint16_t data, uint16_t state) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t    usb_hcdc_pipe_info(uint8_t *table, uint16_t length) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* defined(USB_CFG_HCDC_USE) */

#if defined(USB_CFG_HMSC_USE)
void        usb_hmsc_strg_cmd_complete(usb_tskinfo_t   *p_mess) FUNC_LOCATION_PRV_DRIVER_USB;
uint16_t    R_USB_HmscStrgDriveSearch (usb_hutr_t *ptr, uint16_t addr, usb_hcb_t complete) FUNC_LOCATION_PRV_DRIVER_USB;
void        msc_registration( void ) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* defined(USB_CFG_HMSC_USE) */

#if defined(USB_CFG_PVND_USE)
void        usb_pstd_read_complete(usb_putr_t *p_mess, uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
void        usb_pstd_write_complete(usb_putr_t *p_mess, uint16_t data1, uint16_t data2) FUNC_LOCATION_PRV_DRIVER_USB;
#endif /* defined(USB_CFG_PVND_USE) */


#if USB_CFG_BC == USB_CFG_ENABLE
void       (*gp_usb_hstd_bc_func[USB_BC_STATE_MAX][USB_BC_EVENT_MAX])(void) FUNC_LOCATION_PRV_DRIVER_USB;
#endif  /* USB_CFG_BC == USB_CFG_ENABLE */


#if USB_CFG_COMPLIANCE == USB_CFG_ENABLE
void      usb_compliance_disp (void *) FUNC_LOCATION_PRV_DRIVER_USB;

#endif /* USB_CFG_COMPLIANCE == USB_CFG_ENABLE */


#endif  /* R_USB_EXTERN_H */
/******************************************************************************
 End  Of File
 ******************************************************************************/
