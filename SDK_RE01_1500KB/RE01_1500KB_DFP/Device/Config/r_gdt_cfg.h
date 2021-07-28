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
* Copyright (C) 2018-2019 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_gdt_cfg.h
* Version      : 1.01
* Description  : 
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.11.2018 0.60    First Release
*         : 29.11.2018 0.60    Fixed for function naming error.
*                              - Fixed for API function naming error.
*                                e_GDT_Shrink->R_GDT_Shrink
*                              - Fixed for internal function naming error.
*                                e_GDT_judge_img_cfg       -> e_gdt_judge_img_cfg      
*                                e_GDT_judge_mod_cfg       -> e_gdt_judge_mod_cfg      
*                                v_GDT_basic_info_calc     -> v_gdt_basic_info_calc    
*                                v_GDT_blk_cut_shrink      -> v_gdt_blk_cut_shrink     
*                                v_GDT_cale_trans_info     -> v_gdt_cale_trans_info    
*                                v_GDT_seq_gen             -> v_gdt_seq_gen            
*                                v_GDT_dma_pre_set         -> v_gdt_dmac_pre_set        
*                                v_GDT_dmain_pre_set       -> v_gdt_dmacin_pre_set      
*                                v_GDT_dmaout_pre_set      -> v_gdt_dmacout_pre_set     
*                                v_GDT_shrink_cfg          -> v_gdt_shrink_cfg         
*                                v_GDT_interrupt_enable    -> v_gdt_imgdata_trans_irq_enable   
*                                v_GDT_interrupt_disable   -> v_gdt_interrupt_disable  
*                                v_GDT_dma_update_in_info  -> v_gdt_dmac_update_in_info 
*                                v_GDT_dma_update_out_info -> v_gdt_dmac_update_out_info
*                                v_GDT_cpu_update_in_info  -> v_gdt_cpu_update_in_info 
*                                v_GDT_cpu_update_out_info -> v_gdt_cpu_update_out_info
*                                v_GDT_cpu_byte_in         -> v_gdt_cpu_byte_in        
*                                v_GDT_cpu_byte_out        -> v_gdt_cpu_byte_out       
*                                v_isr_dmac_input          -> v_isr_dmac_input
*                                v_isr_dmac_output         -> v_isr_dmac_output
*         : 10.12.2018 0.60    Add macro definition due to adding function.
*         : 25.12.2018 0.60    Update comments
*                              Adapted to GSCE coding rules.
*         : 21.01.2019 0.61    First Release of endian module
*         : 20.02.2019 0.61    DMAC_CFG_PARAM_CHECKING_ENABLE -> GDT_CFG_PARAM_CHECKING_ENABLE
*         : 04.03.2019 0.61    First Release of monochrome and rotate module
*         : 10.03.2019 0.70    Adapted to GSCE coding rules. 
*         : 29.04.2019 0.72    API Release 
*                              - Fount 
*                              - Nochange 
*                              - Coloralign 
*                              - Colorsyn 
*                              - Color
*         : 22.07.2019 1.00    Update comments
*         : 11.09.2019 1.01    Delete unused definitions
*                              Fixed the problem that the following two internal functions are
*                              not placed in RAM even if they are set in RAM.
*                              - v_gdt_dmac_blk_upinf_in_array()
*                              - e_gdt_judge_cial_dest_mem_size()
*                               
***********************************************************************************************************************/

#ifndef R_GDT_CFG_H
#define R_GDT_CFG_H

/*******************************************************************************************************************//**
 * @addtogroup grp_device_hal_gdt
 * @{
 **********************************************************************************************************************/
 
/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif

#include "r_system_api.h"

/*******************************************************************************
 Macro definitions
*******************************************************************************/
/* Color mode setting (1:ON 0:OFF)  */
#define GDT_CFG_COLOR_ON      (1) 

/*!< GDT gdt configure parameter checking enable */
/* #define GDT_CFG_PARAM_CHECKING_ENABLE      (1) */

/*!< GDT input interrupt priority value     */ 
#define GDT_CFG_IN_INT_PRIORITY                 (2)                   /*!< (set to 0 to 3, 0 is highest priority) */            
 
/*!< GDT output interrupt priority value    */
#define GDT_CFG_OUT_INT_PRIORITY                (3)                   /*!< (set to 0 to 3, 0 is highest priority) */            
   
/*!< GDT Function */
#define GDT_CFG_R_GDT_GETVERSION                    (SYSTEM_SECTION_CODE)        /*!< R_GDT_GetVersion                             () section    */
#define GDT_CFG_R_GDT_OPEN                          (SYSTEM_SECTION_CODE)        /*!< R_GDT_Open                                   () section    */
#define GDT_CFG_R_GDT_CLOSE                         (SYSTEM_SECTION_CODE)        /*!< R_GDT_Close                                  () section    */
#define GDT_CFG_R_GDT_DMAC_CH_SEL                   (SYSTEM_SECTION_CODE)        /*!< R_GDT_Close                                  () section    */
#define GDT_CFG_E_GDT_SHRINK                        (SYSTEM_SECTION_CODE)        /*!< R_GDT_Shrink                                 () section    */
#define GDT_CFG_E_GDT_ENDIAN                        (SYSTEM_SECTION_CODE)        /*!< R_GDT_Endian                                 () section    */
#define GDT_CFG_E_GDT_MONOCHROME                    (SYSTEM_SECTION_CODE)        /*!< R_GDT_Monochrome                             () section    */
#define GDT_CFG_E_GDT_ROTATE                        (SYSTEM_SECTION_CODE)        /*!< R_GDT_Rotate                                 () section    */
#define GDT_CFG_E_GDT_SCROLL                        (SYSTEM_SECTION_CODE)        /*!< R_GDT_Scroll                                 () section    */
#define GDT_CFG_E_GDT_FOUNT                         (SYSTEM_SECTION_CODE)        /*!< R_GDT_Fount                                  () section    */
#define GDT_CFG_E_GDT_NOCHANGE                      (SYSTEM_SECTION_CODE)        /*!< R_GDT_Nochange                               () section    */
#define GDT_CFG_E_GDT_COLORALIGN                    (SYSTEM_SECTION_CODE)        /*!< R_GDT_Coloralign                             () section    */
#define GDT_CFG_E_GDT_COLORSYN                      (SYSTEM_SECTION_CODE)        /*!< R_GDT_Colorsyn                               () section    */
#define GDT_CFG_E_GDT_COLOR                         (SYSTEM_SECTION_CODE)        /*!< R_GDT_Color                                  () section    */
#define GDT_CFG_V_GDT_ON                            (SYSTEM_SECTION_CODE)        /*!< R_GDT_On                                     () section    */
#define GDT_CFG_V_GDT_OFF                           (SYSTEM_SECTION_CODE)        /*!< R_GDT_Off                                    () section    */
#define GDT_CFG_V_GDT_CLEAR_REG                     (SYSTEM_SECTION_CODE)        /*!< v_gdt_cale_trans_info                        () section    */
#define GDT_CFG_E_GDT_CHECK_IMG_NUM                 (SYSTEM_SECTION_CODE)        /*!< e_gdt_check_img_num                          () section    */
#define GDT_CFG_E_GDT_CHECK_DMAC_CFG                (SYSTEM_SECTION_CODE)        /*!< e_gdt_check_dmac_cfg                         () section    */
#define GDT_CFG_E_GDT_JUDGE_IMG_CFG                 (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_img_cfg                          () section    */
#define GDT_CFG_E_GDT_JUDGE_DEST_COMM_BLKEND_OB     (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_dest_comm_blkend_ob              () section    */
#define GDT_CFG_V_GDT_BASIC_INFO_CALC               (SYSTEM_SECTION_CODE)        /*!< e_gdt_basic_info_calc                        () section    */
#define GDT_CFG_E_GDT_DMAC_IN_CHANNEL_SEL           (SYSTEM_SECTION_CODE)        /*!< e_gdt_dmac_in_channel_sel                    () section    */
#define GDT_CFG_E_GDT_DMAC_OUT_CHANNEL_SEL          (SYSTEM_SECTION_CODE)        /*!< e_gdt_dmac_out_channel_sel                   () section    */
#define GDT_CFG_V_GDT_BLK_CUT_COMM_8X8              (SYSTEM_SECTION_CODE)        /*!< v_gdt_blk_cut_comm_8x8                       () section    */
#define GDT_CFG_V_GDT_BLK_CUT_COMM_16X16            (SYSTEM_SECTION_CODE)        /*!< v_gdt_blk_cut_comm_16x16                     () section    */
#define GDT_CFG_V_GDT_BLK_CUT_COMM_16X16_BURST      (SYSTEM_SECTION_CODE)        /*!< v_gdt_blk_cut_comm_16x16_burst               () section    */
#define GDT_CFG_V_GDT_BLK_CUT_COMM_16X16_BLOCK      (SYSTEM_SECTION_CODE)        /*!< v_gdt_blk_cut_comm_16x16_block               () section    */
#define GDT_CFG_V_GDT_DMACIN_PRE_SET                (SYSTEM_SECTION_CODE)        /*!< v_gdt_dmacin_pre_set                         () section    */
#define GDT_CFG_V_GDT_DMACIN_SET                    (SYSTEM_SECTION_CODE)        /*!< v_gdt_dmacin_set                             () section    */
#define GDT_CFG_V_GDT_DMACOUT_PRE_SET               (SYSTEM_SECTION_CODE)        /*!< v_gdt_dmacout_pre_set                        () section    */
#define GDT_CFG_V_GDT_SEQ_GEN_COMMON                (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen_common                         () section    */
#define GDT_CFG_V_GDT_SEQ_COMM_BIG_TRANS_BLK        (SYSTEM_SECTION_CODE)        /*!< gdt_cfg_v_gdt_seq_comm_big_trans_blk         () section    */
#define GDT_CFG_V_GDT_SEQ_COMM_REMN_TRANS_XBYTE     (SYSTEM_SECTION_CODE)        /*!< gdt_cfg_v_gdt_seq_comm_remain_trans_xbyte    () section    */
#define GDT_CFG_V_GDT_SEQ_COMM_REMN_TRANS_BYTE      (SYSTEM_SECTION_CODE)        /*!< gdt_cfg_v_gdt_seq_comm_remain_trans_byte     () section    */
#define GDT_CFG_V_GDT_IMGDATA_TRANS_IRQ_ENABLE      (SYSTEM_SECTION_CODE)        /*!< v_gdt_imgdata_trans_irq_enable               () section    */
#define GDT_CFG_V_GDT_IMGDATA_TRANS_IRQ_DISABLE     (SYSTEM_SECTION_CODE)        /*!< v_gdt_imgdata_trans_irq_disable              () section    */
#define GDT_CFG_V_ISR_CPU_INPUT                     (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_input                              () section    */
#define GDT_CFG_V_ISR_CPU_OUTPUT                    (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_output                             () section    */
#define GDT_CFG_V_ISR_CPU_INPUT_XBYTE               (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_output_xbyte                       () section    */
#define GDT_CFG_V_ISR_CPU_OUTPUT_XBYTE              (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_output_xbyte                       () section    */
#define GDT_CFG_V_ISR_DMAC_INPUT                    (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_input                             () section    */
#define GDT_CFG_V_ISR_DMAC_OUTPUT                   (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_output                            () section    */
#define GDT_CFG_V_ISR_DMAC_OUTPUT_ONCE              (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_output_once                       () section    */
#define GDT_CFG_V_GDT_CPU_UPDATE_IN_INFO            (SYSTEM_SECTION_CODE)        /*!< v_gdt_cpu_update_in_info                     () section    */
#define GDT_CFG_V_GDT_CPU_UPDATE_OUT_INFO           (SYSTEM_SECTION_CODE)        /*!< v_gdt_cpu_update_out_info                    () section    */
#define GDT_CFG_V_GDT_CPUBLK_BYTE_WR_GDT_NORMAL     (SYSTEM_SECTION_CODE)        /*!< v_gdt_cpublk_byte_wr_gdt_normal              () section    */
#define GDT_CFG_V_GDT_CPUBLK_BYTE_RD_GDT_NORMAL     (SYSTEM_SECTION_CODE)        /*!< v_gdt_cpublk_byte_rd_gdt_normal              () section    */
#define GDT_CFG_ST_GDT_CPUBLK_XBYTE_WR_GDT_NOR      (SYSTEM_SECTION_CODE)        /*!< gdt_cfg_st_gdt_cpublk_xbyte_wr_gdt_nor       () section    */
#define GDT_CFG_ST_GDT_CPUBLK_XBYTE_RD_GDT_NOR      (SYSTEM_SECTION_CODE)        /*!< gdt_cfg_st_gdt_cpublk_xbyte_rd_gdt_nor       () section    */
#define GDT_CFG_V_GDT_DMAC_BLK_UPINF_IN_ARRAY       (SYSTEM_SECTION_CODE)        /*!< v_gdt_dmac_blk_upinf_in_array                () section    */
#define GDT_CFG_V_GDT_DMAC_BLK_UPINF_OUT_ARRAY      (SYSTEM_SECTION_CODE)        /*!< v_gdt_dmac_blk_upinf_out_array               () section    */
#define GDT_CFG_V_GDT_CALE_SHRNKSIZE_INFO           (SYSTEM_SECTION_CODE)        /*!< v_gdt_cale_shrnksize_info                    () section    */
#define GDT_CFG_E_GDT_JUDGE_BLKEND_OBOUND           (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_blkend_obound                    () section    */
#define GDT_CFG_V_GDT_BLK_CUT_SHRINK                (SYSTEM_SECTION_CODE)        /*!< v_gdt_blk_cut_shrink                         () section    */
#define GDT_CFG_V_GDT_SHRINK_CFG                    (SYSTEM_SECTION_CODE)        /*!< v_gdt_shrink_cfg                             () section    */
#define GDT_CFG_V_GDT_SHRINK_NUM_CFG                (SYSTEM_SECTION_CODE)        /*!< v_gdt_shrink_num_cfg                         () section    */
#define GDT_CFG_V_GDT_CALC_TRANS_INFO               (SYSTEM_SECTION_CODE)        /*!< v_gdt_calc_trans_info                        () section    */
#define GDT_CFG_V_GDT_SEQ_GEN                       (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen                                () section    */
#define GDT_CFG_V_GDT_SEQ_SHRNK_BIG_TRANS           (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen                                () section    */
#define GDT_CFG_V_GDT_SEQ_SHRNK_REMAIN_TRANS        (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen                                () section    */
#define GDT_CFG_V_ISR_DMAC_SHK_OUTPUT               (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_chk_output                        () section    */
#define GDT_CFG_V_ISR_CPU_OUTPUT_LIMIT              (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_output_limit                       () section    */
#define GDT_CFG_V_GDT_DMAC_UPDATE_OUT_INFO          (SYSTEM_SECTION_CODE)        /*!< v_gdt_dmac_update_out_info                   () section    */
#define GDT_CFG_V_GDT_CPULINE_BYTE_RD_GDT_LIMIT     (SYSTEM_SECTION_CODE)        /*!< v_gdt_cpuline_byte_rd_gdt_limited            () section    */
#define GDT_CFG_V_GDT_CPU_UPDATE_OUT_LIMIT_INFO     (SYSTEM_SECTION_CODE)        /*!< v_gdt_cpu_update_out_limit_info              () section    */
#define GDT_CFG_V_GDT_ENDIAN_CFG                    (SYSTEM_SECTION_CODE)        /*!< gdt_cfg_v_gdt_endian_cfg                     () section    */
#define GDT_CFG_V_GDT_MONOCHROME_CFG                (SYSTEM_SECTION_CODE)        /*!< gdt_cfg_v_gdt_endian_cfg                     () section    */
#define GDT_CFG_E_GDT_JUDGE_DEST_RTT_BLKEND_OB      (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_dest_comm_blkend_ob              () section    */
#define GDT_CFG_V_GDT_ROTATE_CFG                    (SYSTEM_SECTION_CODE)        /*!< v_gdt_rotate_cfg                             () section    */
#define GDT_CFG_V_GDT_SEQ_GEN_ROTATE                (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen_rotate                         () section    */
#define GDT_CFG_V_GDT_SEQ_ROTATE_BIG_TRANS_BLK      (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_rotate_big_trans_blk               () section    */
#define GDT_CFG_ST_GDT_ROTATE_CAL_OUT_COODINATE     (SYSTEM_SECTION_CODE)        /*!< st_gdt_rotate_cal+out_coodinate              () section    */
#define GDT_CFG_V_ISR_DMAC_ROTATE_OUTPUT            (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_rotate_output                     () section    */
#define GDT_CFG_V_GDT_ROTATE_UPDATE_OUT_INFO        (SYSTEM_SECTION_CODE)        /*!< v_gdt_rotate_update_out_info                 () section    */
#define GDT_CFG_E_GDT_JUDGE_DEST_ISCR_BLKEND_OB     (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_dest_iscr_blkend_ob              () section    */
#define GDT_CFG_V_GDT_BLK_CUT_SCROLL_8X16           (SYSTEM_SECTION_CODE)        /*!< v_gdt_blk_cut_scroll_8x16                    () section    */
#define GDT_CFG_V_GDT_SCROLL_CFG                    (SYSTEM_SECTION_CODE)        /*!< v_gdt_scroll_cfg                             () section    */
#define GDT_CFG_V_GDT_SEQ_GEN_SCROLL                (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen_scroll                         () section    */
#define GDT_CFG_V_GDT_SEQ_SCROLL_BIG_TRANS          (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_scroll_big_trans                   () section    */
#define GDT_CFG_V_ISR_DMAC_SCROLL_INPUT             (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_scroll_input                      () section    */
#define GDT_CFG_V_ISR_DMAC_SCROLL_OUTPUT            (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_dmac_scroll_output                     () section    */
#define GDT_CFG_V_GDT_ISCR_DMAC_BLK_UPINF_IN        (SYSTEM_SECTION_CODE)        /*!< v_gdt_iscr_dmac_blk_upinf_in                 () section    */
#define GDT_CFG_V_GDT_ISCR_DMAC_BLK_UPINF_OUT       (SYSTEM_SECTION_CODE)        /*!< v_gdt_iscr_dmac_blk_upinf_out                () section    */
#define GDT_CFG_E_GDT_JUDGE_CFG_FOUNT               (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_cfg_fount                        () section    */ 
#define GDT_CFG_E_GDT_JUDGE_DEST_FOUNT              (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_dest_fount                       () section    */ 
#define GDT_CFG_V_GDT_FOUNT_CFG                     (SYSTEM_SECTION_CODE)        /*!< v_gdt_fount_cfg                              () section    */ 
#define GDT_CFG_V_GDT_SEQ_GEN_FOUNT                 (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen_fount                          () section    */ 
#define GDT_CFG_V_GDT_FOUNT_FINISH                  (SYSTEM_SECTION_CODE)        /*!< v_gdt_fount_finish                           () section    */ 
#define GDT_CFG_V_ISR_CPU_OUTPUT_FOUNT              (SYSTEM_SECTION_RAM_FUNC)    /*!< v_isr_cpu_output_fount                       () section    */ 
#define GDT_CFG_V_GDT_CPU_UPDATE_OUT_FOUNT_INFO     (SYSTEM_SECTION_CODE)        /*!< v_gdt_cpu_update_out_fount_info              () section    */ 
#define GDT_CFG_V_GDT_FOUNT_DMACIN_SET              (SYSTEM_SECTION_CODE)        /*!< v_gdt_fount_dmacin_set                       () section    */ 
#define GDT_CFG_E_GDT_JUDGE_CIAL_DEST_MEM_SIZE      (SYSTEM_SECTION_CODE)        /*!< e_gdt_judge_cial_dest_mem_size               () section    */ 
#define GDT_CFG_V_GDT_BLK_CUT_CLRALIGN_16X16        (SYSTEM_SECTION_CODE)        /*!< v_gdt_blk_cut_clralign_16x16                 () section    */ 
#define GDT_CFG_V_GDT_COLORALIGN_CFG                (SYSTEM_SECTION_CODE)        /*!< v_gdt_coloralign_cfg                         () section    */ 
#define GDT_CFG_V_GDT_SEQ_GEN_COLORALIGN            (SYSTEM_SECTION_CODE)        /*!< v_gdt_seq_gen_coloralign                     () section    */ 
#define GDT_CFG_V_GDT_COLORSYN_CFG                  (SYSTEM_SECTION_CODE)        /*!< v_gdt_colorsyn_cfg                           () section    */ 
#define GDT_CFG_V_GDT_COLOR_CFG                     (SYSTEM_SECTION_CODE)        /*!< v_gdt_color_cfg                              () section    */ 

#ifdef  __cplusplus
}
#endif

/******************************************************************************************************************//**
 * @} (end ingroup grp_device_hal_gdt
 *********************************************************************************************************************/
#endif /* R_GDT_CFG_H */
