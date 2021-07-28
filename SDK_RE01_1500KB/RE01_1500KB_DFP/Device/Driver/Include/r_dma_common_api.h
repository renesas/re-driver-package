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
* Copyright (C) 2018-2021 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_dma_common_api.h
* Version      : 1.40
* Description  : HAL Driver for DMAC/DTC
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.11.2018 0.51     First Release
*         : 19.12.2018 0.60     Adapted to GSCE coding rules.
*                               - Change structer name
*                                 dma_transfer_data_cfg_t -> st_dma_transfer_data_cfg_t
*                                 dma_state_t -> st_dma_state_t
*                               Changed file name to r_dma_common_api.c
*         : 06.02.2019 0.61    DMAC / DTC active information (g_dma_active_flag) is defined
*                              in r_dma_common_api.h.
*         : 14.05.2019 0.72     Add DMA_CMD_REFRESH_EXTRA command
*         : 02.07.2019 1.00     Add R_DMAC_GetTransferByte function
*         : 05.01.2021 1.40     Add "DMA_ERROR_UNSUPPORTED" to the error definition.
*                               Add "GetAvailabilitySrc" and "Release" to API
***********************************************************************************************************************/
/******************************************************************************************************************//**
* @addtogroup grp_device_hal_dmac
* @{
 *********************************************************************************************************************/
 /***********************************************************
Includes <System Includes> , "Project Includes"
***********************************************************/
#ifndef R_DMA_COMMON_H
#define R_DMA_COMMON_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

/******************************************************************************
Macro definitions
*******************************************************************************/
/** DMA flag. */
#define DMA_FLAG_OPENED                 (1U << 0)   /*!< DMA Open flag */
#define DMA_FLAG_INITIALIZED            (1U << 1)   /*!< DMA Initialize flag */
#define DMA_FLAG_CREATED                (1U << 2)   /*!< DMA Created flag */

/****** DMA Control Codes *****/
/*----- DMA Control Codes: Mode -----*/
#define DMA_TRANSFER_MODE_MASK          (0xC000U)   /*!< DMA Transfer mode mask. */
#define DMA_MODE_NORMAL                 (0x0000U)   /*!< DMA Normal transfer mode. */
#define DMA_MODE_REPEAT                 (0x4000U)   /*!< DMA Repeat transfer mode. */
#define DMA_MODE_BLOCK                  (0x8000U)   /*!< DMA Block transfer mode. */

/*----- DMA Control Codes: Size -----*/
#define DMA_SIZE_MASK                   (0x3000U)   /*!< DMA Data size mask. */
#define DMA_SIZE_BYTE                   (0x0000U)   /*!< DMA Data size is byte. */
#define DMA_SIZE_WORD                   (0x1000U)   /*!< DMA Data size is word(2-bytes). */
#define DMA_SIZE_LONG                   (0x2000U)   /*!< DMA Data size is long(4-bytes). */

/*----- DMA Control Codes: Source address addressing mode -----*/
#define DMA_SRC_ADDRESSING_MASK         (0x0C00U)   /*!< DMA Source address mask. */
#define DMA_SRC_FIXED                   (0x0000U)   /*!< DMA Source address fixed. */
#define DMA_SRC_INCR                    (0x0800U)   /*!< DMA Source address increment. */
#define DMA_SRC_DECR                    (0x0C00U)   /*!< DMA Source address decrement. */
#define DMA_SRC_ADDR_OFFSET             (0x0400U)   /*!< DMA Source address offset addition(DMAC only). */

/*----- DMA Control Codes: Destination address addressing mode -----*/
#define DMA_DEST_ADDRESSING_MASK        (0x000CU)   /*!< DMA Destination address mask. */
#define DMA_DEST_FIXED                  (0x0000U)   /*!< DMA Destination address fixed. */
#define DMA_DEST_INCR                   (0x0008U)   /*!< DMA Destination address increment. */
#define DMA_DEST_DECR                   (0x000CU)   /*!< DMA Destination address decrement. */
#define DMA_DEST_ADDR_OFFSET            (0x0004U)   /*!< DMA Destination address offset addition(DMAC only). */

/*----- DMA Control Codes: Repeat block -----*/
#define DMA_REPEAT_BLOCK_MASK           (0x0011U)   /*!< DMA Repeat or block select mask. */
#define DMA_REPEAT_BLOCK_DEST           (0x0000U)   /*!< DMA Destinaion address is repeate or block area. */
#define DMA_REPEAT_BLOCK_SRC            (0x0010U)   /*!< DMA Source address is repeate or block area. */
#define DMA_REPEAT_BLOCK_NONE           (0x0001U)   /*!< DMA Repeat or block area nothing(DMAC only). */

/*----- DMA Control Codes: Interrupt selected(DTC only) -----*/
#define DMA_INT_SELECT_MASK             (0x0020U)   /*!< DMA Interrupt select mask. */
#define DMA_INT_AFTER_ALL_COMPLETE      (0x0000U)   /*!< DMA Interrupt is generated when specified data 
                                                         transfer is completed. */
#define DMA_INT_PER_SINGLE_TRANSFER     (0x0020U)   /*!< DMA Interrupt is generated when each transfer 
                                                         time is completed. */

/*----- DMA Control Codes: Chain transfer mode(DTC only) -----*/
#define DMA_CHAIN_MODE_MASK             (0x00C0U)   /*!< DMA Chain mode mask. */
#define DMA_CHAIN_DISABLE               (0x0000U)   /*!< DMA Chain mode disable. */
#define DMA_CHAIN_CONTINUOUSLY          (0x0080U)   /*!< DMA Chain transfer is performed continuously.  */
#define DMA_CHAIN_NORMAL                (0x00C0U)   /*!< DMA Chain transfer is performed only when the counter is 
                                                         changed to 0 or CRAH. */

/*----- DMA Control Codes: DMA interrupt factor -----*/
#define DMA_INT_MASK                    (0x001F)    /*!< DMA Interrupt factor mask. */
#define DMA_INT_COMPLETE                (1U<<0)     /*!< DMA Transfer complete interrupt. */
#define DMA_INT_SRC_OVERFLOW            (1U<<1)     /*!< DMA Source address overflow interrupt(DMAC only). */
#define DMA_INT_DEST_OVERFLOW           (1U<<2)     /*!< DMA Destination address overflow interrupt(DMAC only). */
#define DMA_INT_REPEAT_END              (1U<<3)     /*!< DMA Repeat size end interrupt(DMAC only). */
#define DMA_INT_ESCAPE_END              (1U<<4)     /*!< DMA Escape end interrupt(DMAC only). */

#define DMA_MODE_RASERVED               (0x0302)    /*!< DMA Reserved bit */

/*----- DMA Active flag -----*/
#define DMA_ACTIVE_DMAC0             (0x0001)        /*!< DMAC0 Active */
#define DMA_ACTIVE_DMAC1             (0x0002)        /*!< DMAC1 Active */
#define DMA_ACTIVE_DMAC2             (0x0004)        /*!< DMAC2 Active */
#define DMA_ACTIVE_DMAC3             (0x0008)        /*!< DMAC3 Active */
#define DMA_ACTIVE_DTC               (0x8000)        /*!< DMAC3 Active */
#define DMA_ACTIVE_MASK              (0x800F)        /*!< DMAC0 to DMAC3 and DTC Active mask */
#define DMA_ACTIVE_DMAC_MASK         (0x000F)        /*!< DMAC0 to DMAC3 Active mask */

/*****************************************************************************
Typedef definitions
******************************************************************************/
/** DMA API error codes */
typedef enum
{
    DMA_OK = 0,                                     /*!< Operation succeeded */
    DMA_ERROR,                                      /*!< Unspecified error */
    DMA_ERROR_BUSY,                                 /*!< Driver is busy */
    DMA_ERROR_PARAMETER,                            /*!< Parameter error */
    DMA_ERROR_MODE,                                 /*!< Mode error */
    DMA_ERROR_LOCKED,                               /*!< This Module is already locked */
    DMA_ERROR_SYSTEM_SETTING,                       /*!< error for System Driver (R_System) setting */
    DMA_ERROR_UNSUPPORTED,                          /*!< API not supported  */
} e_dma_err_t;

/** DMA Control command codes */
typedef enum
{
    DMA_CMD_START,                                  /*!< DMA Transfer start command. */
    DMA_CMD_STOP,                                   /*!< DMA Transfer stop command. */
    DMA_CMD_ACT_SRC_ENABLE,                         /*!< DMA Transfer action source enable(DTC only, default enable). */
    DMA_CMD_ACT_SRC_DISABLE,                        /*!< DMA Transfer action source disable(DTC only). */
    DMA_CMD_DATA_READ_SKIP_ENABLE,                  /*!< DMA Read skip enable(DTC only,  default enable). */
    DMA_CMD_CHAIN_TRANSFER_ABORT,                   /*!< DMA Transfer abort(DTC only). */
    DMA_CMD_CHANGING_DATA_FORCIBLY_SET,             /*!< DMA Infomation forcibly changing(DTC only). */
    DMA_CMD_SOFTWARE_TRIGGER,                       /*!< DMA Software trigger(DMAC only). */
    DMA_CMD_AUTO_CLEAR_SOFT_REQ,                    /*!< DMA Software request auto clear enable(DMAC only). */
    DMA_CMD_REFRESH_DATA,                           /*!< DMA Refresh the transfer information. */
    DMA_CMD_REFRESH_EXTRA                           /*!< DMA Refresh the transfer information with 
                                                         DMA IR holding function and offset update function.(DMAC only) */
} e_dma_cmd_t;

/** DMA Transfer infomation configrate data */
typedef struct {
    uint16_t            mode;                       /*!< DMA Transfer mode. */
    uint32_t            src_addr;                   /*!< DMA Transfer source address. */
    uint32_t            dest_addr;                  /*!< DMA Transfer destination address. */
    uint32_t            transfer_count;             /*!< DMA Transfer count. */
    uint32_t            block_size;                 /*!< DMA Block/repeat size. */
    int32_t             offset;                     /*!< DMA Offset size(DMAC only). */
    uint8_t             src_extended_repeat;        /*!< DMA Source extended repeat area(DMAC only). */
    uint8_t             dest_extended_repeat;       /*!< DMA Destination extended repeat area(DMAC only). */
    void                *p_transfer_data;           /*!< DMA Transfer infomation data(DTC only). */
} st_dma_transfer_data_cfg_t;

/** DMA transfer information setting data for refresh */
typedef struct {
    IRQn_Type           act_src;                     /*!< DMA activation factor (DTC only). */
    uint32_t            chain_transfer_nr;          /*!< DMA Chain Number. */
    uint32_t            src_addr;                   /*!< DMA Transfer source address. */
    uint32_t            dest_addr;                  /*!< DMA Transfer destination address. */
    uint32_t            transfer_count;             /*!< DMA Transfer count. */
    uint32_t            block_size;                 /*!< DMA Block/repeat size. */
    bool                auto_start;                 /*!< DMA auto start mode. */
    bool                keep_dma_req;               /*!< DMA keep ir bit (DMAC only) */
    int32_t             offset;                     /*!< DMA Offset size (DMAC only) */
} st_dma_refresh_data_t;

/** DMA Transfer state */
typedef struct {
    bool                in_progress;                /*!< DMA Active flag */
    bool                esif_stat;                  /*!< DMA Transfer escape interrupt flag(DMAC only) */
    bool                dtif_stat;                  /*!< DMA Transfer end interrupt flag(DMAC only) */
    bool                soft_req_stat;              /*!< DMA Software request flag(DMAC only) */
} st_dma_state_t;


typedef void (*dma_cb_event_t) (void);              /*!< Pointer to DMA interrupt callback function. */

typedef struct {
    uint32_t      (*GetVersion)   (void);         /*!< Pointer to \ref R_DMAC_GetVersion or \ref R_DTC_GetVersion  
                                                       : Get driver version. */
    e_dma_err_t   (*Open)         (void);         /*!< Pointer to \ref R_DMAC_Open or \ref R_DTC_Open  
                                                       : Open DMAC interface. */
    e_dma_err_t   (*Close)        (void);         /*!< Pointer to \ref R_DMAC_Close or \ref R_DMAC_Close
                                                       : Close DMAC interface. */
    e_dma_err_t   (*Create)       (int16_t const dma_source, st_dma_transfer_data_cfg_t * const data_cfg);
                                                  /*!< Pointer to \ref R_DMAC_Create or \ref R_DTC_Create  
                                                       : DMAC transfer infomation create. */
    e_dma_err_t   (*Control)      (e_dma_cmd_t cmd, void const * const arg);
                                                  /*!< Pointer to \ref R_DMAC_Control or \ref R_DTC_Control  
                                                       : Control DMAC interface. */
    e_dma_err_t   (*InterruptEnable)  (uint16_t const int_fact, dma_cb_event_t const p_callback);
                                                  /*!< Pointer to \ref R_DMAC_InterruptEnable or  
                                                       \ref R_DTC_InterruptEnable : Interrupt enable. */
    e_dma_err_t   (*InterruptDisable)  (void);    /*!< Pointer to \ref R_DMAC_InterruptDisable or
                                                       \ref R_DTC_InterruptDisable : Interrupt disable.  */
    e_dma_err_t   (*GetState)     (st_dma_state_t * const state);  /*!< Pointer to \ref R_DMAC_GetState or
                                                       \ref R_DTC_GetState : Get DMAC state.  */
    e_dma_err_t   (*ClearState)   (uint32_t clr_state);  /*!< Pointer to \ref R_DMAC_GetState 
                                                              or \ref R_DTC_GetState : Get DMAC state. */
    e_dma_err_t   (*GetTransferByte)   (int16_t const dma_source, uint32_t * transfer_byte);   
                                                  /*!< Pointer to \ref R_DMAC_GetTransferByte : Get transfer byte. */
    e_dma_err_t   (*GetAvailabilitySrc)     (uint64_t * const src_info);  /*!< Pointer to \ref R_DMAC_GetAvailabilitySrc or
                                                       \ref R_DTC_GetAvailabilitySrc : Get DTC source information.  */
    e_dma_err_t   (*Release)   (int16_t const dma_source);  /*!< Pointer to \ref R_DMAC_Release
                                                              or \ref R_DTC_Release : Release DTC source */

} const DRIVER_DMA; // @suppress("Data type naming")


/******************************************************************************
 Exported global variables (to be accessed by other files)
 *****************************************************************************/
extern uint16_t g_dma_active_flag;               /*!< DMAC Active flag */

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/

#ifdef  __cplusplus
}
#endif

#endif /* R_DMA_COMMON_H */
/******************************************************************************************************************//**
 * @} (end  addgroup grp_device_hal_dmac)
 *********************************************************************************************************************/
/* End of file (r_dma_common_api.h) */
