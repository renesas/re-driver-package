
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
* Copyright (C) 2018-2020 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_gdt_api.c
* Version      : 1.20
* Description  : HAL Driver for GDT
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description      
*         : 26.11.2018 0.60    First Release     
*         : 29.11.2018 0.60    Fixed for function naming error.
*                              - Fixed for API function naming error.
*                                e_GDT_Shrink->R_GDT_Shrink
*                              - Fixed for internal function naming error.
*                                e_GDT_judge_img_cfg       -> e_gdt_judge_img_cfg      
*                                e_GDT_judge_mod_cfg       -> e_gdt_check_gdtclk_on      
*                                v_GDT_basic_info_calc     -> v_gdt_basic_info_calc    
*                                v_GDT_blk_cut_shrink      -> v_gdt_blk_cut_shrink     
*                                v_GDT_cale_trans_info     -> v_gdt_calc_trans_info    
*                                v_GDT_seq_gen             -> v_gdt_seq_gen            
*                                v_GDT_dma_pre_set         -> v_gdt_dmac_pre_set        
*                                v_GDT_dmain_pre_set       -> v_gdt_dmacin_pre_set      
*                                v_GDT_dmaout_pre_set      -> v_gdt_dmacout_pre_set     
*                                v_GDT_shrink_cfg          -> v_gdt_shrink_cfg         
*                                v_GDT_interrupt_enable    -> v_gdt_imgdata_trans_irq_enable   
*                                v_GDT_interrupt_disable   -> v_gdt_imgdata_trans_irq_disable  
*                                v_GDT_dma_update_in_info  -> v_gdt_dmac_update_in_info 
*                                v_GDT_dma_update_out_info -> v_gdt_dmac_update_out_info
*                                v_GDT_cpu_update_in_info  -> v_gdt_cpu_update_in_info 
*                                v_GDT_cpu_update_out_info -> v_gdt_cpu_update_out_info
*                                v_GDT_cpu_byte_in         -> v_gdt_cpu_byte_in        
*                                v_GDT_cpu_byte_out        -> v_gdt_cpu_byte_out       
*                                v_isr_dmac_input          -> v_isr_dmac_input
*                                v_isr_dmac_output         -> v_isr_dmac_output
*         : 10.12.2018 0.60    Fixed the bug where shrink_num=1 or shrink_size1 could not shrink.
*                              add a processing method for only one input and output case.                               
*                               - modify function
*                                 v_gdt_seq_gen
*                              - add function    
*                                v_gdt_dmacin_set    
*                                v_gdt_dmacout_set
*                                v_isr_dmac_output_once
*                              modify the function for that the shrink_num=1 can not complete the shrink. 
*                               - modify function
*                                v_gdt_dmac_update_in_info
*                                v_gdt_dmac_update_out_info
*         : 10.12.2018 0.60    Add a judgement thar the block horizontal pixel is not a multiple of eitht.
*         : 10.12.2018 0.60    Fixed the bug that processing method is incorrect when the endpoint is out of boundary. 
*                              - add function    
*                                v_gdt_cale_shrnksize_info
*                                e_gdt_judge_blk_end_offbound
*         : 13.12.2018 0.60    Fixed a bug that could not traverse conditional judgments.
*                              - function name : e_gdt_judge_blk_end_offbound and e_gdt_judge_img_cfg
*                                else if -> if
*         : 13.12.2018 0.60    Fixed a bug that The start address of the line was not updated 
*                              when calculating the 2nd address.
*                              - add code to v_gdt_seq_gen function
*                                gs_in_img_addr_line_org = gs_in_img_addr;
*         : 13.12.2018 0.60    Fixed a bug
*                              When the horizontal and vertical coordinates of a block are judged to be a 
*                              multiple of 8, the decision logic is erroneous.
*                              - function name : e_gdt_judge_img_cfg
*         : 16.12.2018 0.60    Fixed 2 bug
*                              - DMAC mode of v_gdt_seq_gen() add transmision case to complete the process
*                              - DMAC mode trans that only trans once disregard second times trans
*         : 17.12.2018 0.60    Fixed 3 bug
*                              - fix calculation of trans address when start source pixel is not (0,0) of only remain
*                                erea trans
*                              - fix that redundancy cpu output intterrupt during trans of remain erea 
*                              - fix that calculation of line offset address that use actual image horizontal size
*                                but memory size of image storage
*         : 21.12.2018 0.60    Fixed the bug. Block size judgment locig error. ( &&->|| )
*                              change file name r_gdt.c ->r_gdt_api.c    
*         : 24.12.2018 0.60    Structure name change
*                              - dma_transfer_data_cfg_t -> st_dma_transfer_data_cfg_t
*         : 25.12.2018 0.60    Update comments
*                              Adapted to GSCE coding rules.
*         : 26.12.2018 0.60    Add a return value for the v_gdt_basic_info_calc function.
*                              (v_gdt_basic_info_calc -> e_gdt_basic_info_calc  void ->e_gdt_err_t)
*                              In order to correspond to the length limit, the split the contents of 
*                              functions e_gdt_basic_info_calc and e_gdt_basic_info_calc
*                              - e_gdt_basic_info_calc
*                                   e_gdt_dmac_in_channel_sel
*                                   e_gdt_dmac_out_channel_sel
*                              - v_gdt_seq_gen
*                                   v_gdt_seq_shrnk_big_trans
*                                   v_gdt_seq_shrnk_remain_trans
*         : 21.01.2019 0.61    First Release of endian module
*         : 24.01.2019 0.61    Fiexd bug 
*                              - function name: v_gdt_seq_comm_big_trans_blk 
*                                only output once in 1line, and the next time the output is ot the next line
*                                the DMAC will perfor one  more date transfer
*         : 28.01.2019 0.61    function e_gdt_judge_img_cfg add interface member uint32_t gdtdsz
*                              global variable gs_gdt_unit_size added for gdt process unit size 
*         : 04.03.2019 0.61    First Release of monochrome and rotate module
*         : 10.03.2019 0.70    Adapted to GSCE coding rules. 
*         : 29.03.2019 0.70    fiexd bug. the eventlink set function remove to gdt_seq_shrnk_remain_trans in shrink mode
*         : 09.04.2019 0.70    add R_GDT_DmacChSel function for DMAC channle select 
*         : 29.04.2019 0.72    API Release 
*                              - Fount 
*                              - Nochange 
*                              - Coloralign 
*                              - Colorsyn 
*                              - Color
*         : 22.07.2019 1.00   Update comments
*         : 10.09.2019 1.01   Added timeout processing.
*                             Fixed the problem that the following two internal functions are
*                             not placed in RAM even if they are set in RAM.
*                             - v_gdt_dmac_blk_upinf_in_array()
*                             - e_gdt_judge_cial_dest_mem_size()
*         : 14.02.2020 1.10   Tentatively Fixed defect of v_gdt_cpuline_byte_rd_gdt_limit function.
*         : 26.08.2020 1.20   Replace asm("nop"); with __NOP();
**********************************************************************************************************************/
/******************************************************************************************************************//**
 * @addtogroup grp_device_hal_gdt
 * @{
 *********************************************************************************************************************/

#include <r_gdt_api.h>
/*****************************************************************************
Global variable
******************************************************************************/

#if (DMAC_CFG_R_DMAC_OPEN            == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CLOSE           == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CREATE          == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CONTROL         == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_GET_STATE       == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_R_DMAC_CLEAR_STATE     == SYSTEM_SECTION_RAM_FUNC) || \
    (DMAC_CFG_DMAC_INTERRUPT_HANDLER == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DMAC_RESOURCES       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DMAC_RESOURCES
#endif


#if (GDT_CFG_V_GDT_ON == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_ON      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_PRV_V_GDT_ON
#endif

#if (GDT_CFG_V_GDT_OFF == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_OFF      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_PRV_V_GDT_OFF
#endif

#if (GDT_CFG_V_GDT_CLEAR_REG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CLEAR_REG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_CLEAR_REG
#endif

/******************************************************************************************************************//**
 * common internal function
 *********************************************************************************************************************/
 
#if (GDT_CFG_E_GDT_CHECK_IMG_NUM == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_CHECK_IMG_NUM      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_PRV_E_GDT_CHECK_IMG_NUM
#endif

#if (GDT_CFG_E_GDT_CHECK_DMAC_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_CHECK_DMAC_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_PRV_E_GDT_CHECK_DMAC_CFG
#endif

#if (GDT_CFG_E_GDT_JUDGE_IMG_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_IMG_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_IMG_CFG
#endif

#if (GDT_CFG_E_GDT_JUDGE_DEST_COMM_BLKEND_OB == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_COMM_BLKEND_OB      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_COMM_BLKEND_OB
#endif   

#if (GDT_CFG_V_GDT_BASIC_INFO_CALC == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BASIC_INFO_CALC      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_BASIC_INFO_CALC
#endif

#if (GDT_CFG_E_GDT_DMAC_IN_CHANNEL_SEL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_DMAC_IN_CHANNEL_SEL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_PRV_E_GDT_DMAC_IN_CHANNEL_SEL
#endif

#if (GDT_CFG_E_GDT_DMAC_OUT_CHANNEL_SEL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_DMAC_OUT_CHANNEL_SEL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_PRV_E_GDT_DMAC_OUT_CHANNEL_SEL
#endif

#if (GDT_CFG_V_GDT_BLK_CUT_COMM_8X8 == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_8X8      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_8X8
#endif

#if (GDT_CFG_V_GDT_BLK_CUT_COMM_16X16 == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16
#endif

#if (GDT_CFG_V_GDT_BLK_CUT_COMM_16X16_BURST == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16_BURST      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16_BURST
#endif

#if (GDT_CFG_V_GDT_BLK_CUT_COMM_16X16_BLOCK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16_BLOCK      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16_BLOCK
#endif

#if (GDT_CFG_V_GDT_DMACIN_PRE_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_DMACIN_PRE_SET      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                          
#define FUNC_LOCATION_PRV_V_GDT_DMACIN_PRE_SET
#endif

#if (GDT_CFG_V_GDT_DMACIN_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_DMACIN_SET      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                          
#define FUNC_LOCATION_PRV_V_GDT_DMACIN_SET
#endif

#if (GDT_CFG_V_GDT_DMACOUT_PRE_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_DMACOUT_PRE_SET      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                          
#define FUNC_LOCATION_PRV_V_GDT_DMACOUT_PRE_SET
#endif

#if (GDT_CFG_V_GDT_SEQ_GEN_COMMON == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_COMMON      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_COMMON
#endif

#if (GDT_CFG_V_GDT_SEQ_COMM_BIG_TRANS_BLK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_BIG_TRANS_BLK      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_BIG_TRANS_BLK
#endif

#if (GDT_CFG_V_GDT_SEQ_COMM_REMN_TRANS_XBYTE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_REMN_TRANS_XBYTE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_REMN_TRANS_XBYTE
#endif

#if (GDT_CFG_V_GDT_SEQ_COMM_REMN_TRANS_BYTE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_REMN_TRANS_BYTE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_REMN_TRANS_BYTE
#endif

#if (GDT_CFG_V_GDT_IMGDATA_TRANS_IRQ_ENABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_IMGDATA_TRANS_IRQ_ENABLE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                         
#define FUNC_LOCATION_PRV_V_GDT_IMGDATA_TRANS_IRQ_ENABLE
#endif

#if (GDT_CFG_V_GDT_IMGDATA_TRANS_IRQ_DISABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_IMGDATA_TRANS_IRQ_DISABLE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                         
#define FUNC_LOCATION_PRV_V_GDT_IMGDATA_TRANS_IRQ_DISABLE
#endif

#if (GDT_CFG_V_ISR_CPU_INPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_INPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_CPU_INPUT
#endif

#if (GDT_CFG_V_ISR_CPU_OUTPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT
#endif

#if (GDT_CFG_V_ISR_CPU_INPUT_XBYTE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_INPUT_XBYTE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_CPU_INPUT_XBYTE
#endif

#if (GDT_CFG_V_ISR_CPU_OUTPUT_XBYTE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_XBYTE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_XBYTE
#endif

#if (GDT_CFG_V_ISR_DMAC_INPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_INPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_DMAC_INPUT
#endif

#if (GDT_CFG_V_ISR_DMAC_OUTPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_OUTPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_DMAC_OUTPUT
#endif

#if (GDT_CFG_V_ISR_DMAC_OUTPUT_ONCE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_OUTPUT_ONCE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_DMAC_OUTPUT_ONCE
#endif

#if (GDT_CFG_V_GDT_CPU_UPDATE_IN_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_IN_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_IN_INFO
#endif

#if (GDT_CFG_V_GDT_CPU_UPDATE_OUT_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_INFO
#endif

#if (GDT_CFG_V_GDT_CPUBLK_BYTE_WR_GDT_NORMAL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CPUBLK_BYTE_WR_GDT_NORMAL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_V_GDT_CPUBLK_BYTE_WR_GDT_NORMAL
#endif

#if (GDT_CFG_V_GDT_CPUBLK_BYTE_RD_GDT_NORMAL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CPUBLK_BYTE_RD_GDT_NORMAL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_V_GDT_CPUBLK_BYTE_RD_GDT_NORMAL
#endif

#if (GDT_CFG_ST_GDT_CPUBLK_XBYTE_WR_GDT_NOR == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ST_GDT_CPUBLK_XBYTE_WR_GDT_NOR      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_ST_GDT_CPUBLK_XBYTE_WR_GDT_NOR
#endif

#if (GDT_CFG_ST_GDT_CPUBLK_XBYTE_RD_GDT_NOR == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ST_GDT_CPUBLK_XBYTE_RD_GDT_NOR      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_ST_GDT_CPUBLK_XBYTE_RD_GDT_NOR
#endif

#if (GDT_CFG_V_GDT_DMAC_BLK_UPINF_IN_ARRAY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_DMAC_BLK_UPINF_IN_ARRAY      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                                         
#define FUNC_LOCATION_PRV_V_GDT_DMAC_BLK_UPINF_IN_ARRAY
#endif

#if (GDT_CFG_V_GDT_DMAC_BLK_UPINF_OUT_ARRAY == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_DMAC_BLK_UPINF_OUT_ARRAY      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                                         
#define FUNC_LOCATION_PRV_V_GDT_DMAC_BLK_UPINF_OUT_ARRAY
#endif

/******************************************************************************************************************//**
 * Shrink internal function
 *********************************************************************************************************************/

#if (GDT_CFG_V_GDT_CALE_SHRNKSIZE_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CALE_SHRNKSIZE_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_CALE_SHRNKSIZE_INFO
#endif

#if (GDT_CFG_E_GDT_JUDGE_BLKEND_OBOUND == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_BLKEND_OBOUND      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_BLKEND_OBOUND
#endif

#if (GDT_CFG_V_GDT_BLK_CUT_SHRINK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_SHRINK      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_SHRINK
#endif

#if (GDT_CFG_V_GDT_SHRINK_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SHRINK_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_GDT_SHRINK_CFG
#endif

#if (GDT_CFG_V_GDT_SHRINK_NUM_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SHRINK_NUM_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_GDT_SHRINK_NUM_CFG
#endif

#if (GDT_CFG_V_GDT_CALC_TRANS_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CALC_TRANS_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_V_GDT_CALC_TRANS_INFO
#endif

#if (GDT_CFG_V_GDT_SEQ_GEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                          
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN
#endif

#if (GDT_CFG_V_GDT_SEQ_SHRNK_BIG_TRANS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_SHRNK_BIG_TRANS      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                          
#define FUNC_LOCATION_PRV_V_GDT_SEQ_SHRNK_BIG_TRANS
#endif

#if (GDT_CFG_V_GDT_SEQ_SHRNK_REMAIN_TRANS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_SHRNK_REMAIN_TRANS      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                          
#define FUNC_LOCATION_PRV_V_GDT_SEQ_SHRNK_REMAIN_TRANS
#endif

#if (GDT_CFG_V_ISR_DMAC_SHK_OUTPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_SHK_OUTPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_DMAC_SHK_OUTPUT
#endif

#if (GDT_CFG_V_ISR_CPU_OUTPUT_LIMIT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_LIMIT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                         
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_LIMIT
#endif

#if (GDT_CFG_V_GDT_DMAC_UPDATE_OUT_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_DMAC_UPDATE_OUT_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_DMAC_UPDATE_OUT_INFO
#endif

#if (GDT_CFG_V_GDT_CPULINE_BYTE_RD_GDT_LIMIT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CPULINE_BYTE_RD_GDT_LIMIT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                               
#define FUNC_LOCATION_PRV_V_GDT_CPULINE_BYTE_RD_GDT_LIMIT
#endif

#if (GDT_CFG_V_GDT_CPU_UPDATE_OUT_LIMIT_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_LIMIT_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_LIMIT_INFO
#endif

/******************************************************************************************************************//**
 * Endian internal function
 *********************************************************************************************************************/

#if (GDT_CFG_V_GDT_ENDIAN_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_ENDIAN_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_ENDIAN_CFG
#endif

/******************************************************************************************************************//**
 * Monochrome internal function
 *********************************************************************************************************************/

#if (GDT_CFG_V_GDT_MONOCHROME_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_MONOCHROME_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_MONOCHROME_CFG
#endif

/******************************************************************************************************************//**
 * Rotate internal function
 *********************************************************************************************************************/

#if (GDT_CFG_E_GDT_JUDGE_DEST_RTT_BLKEND_OB == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_RTT_BLKEND_OB      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                           
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_RTT_BLKEND_OB
#endif   

#if (GDT_CFG_V_GDT_ROTATE_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_ROTATE_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_ROTATE_CFG
#endif

#if (GDT_CFG_V_GDT_SEQ_GEN_ROTATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_ROTATE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_ROTATE
#endif

#if (GDT_CFG_V_GDT_SEQ_ROTATE_BIG_TRANS_BLK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_ROTATE_BIG_TRANS_BLK      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_SEQ_ROTATE_BIG_TRANS_BLK
#endif

#if (GDT_CFG_ST_GDT_ROTATE_CAL_OUT_COODINATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_ST_GDT_ROTATE_CAL_OUT_COODINATE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_ST_GDT_ROTATE_CAL_OUT_COODINATE
#endif

#if (GDT_CFG_V_ISR_DMAC_ROTATE_OUTPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_ROTATE_OUTPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_ISR_DMAC_ROTATE_OUTPUT
#endif

#if (GDT_CFG_V_GDT_ROTATE_UPDATE_OUT_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_ROTATE_UPDATE_OUT_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_ROTATE_UPDATE_OUT_INFO
#endif

/******************************************************************************************************************//**
 * Scroll internal function
 *********************************************************************************************************************/
#if (GDT_CFG_E_GDT_JUDGE_DEST_ISCR_BLKEND_OB == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_ISCR_BLKEND_OB      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_ISCR_BLKEND_OB
#endif

#if (GDT_CFG_V_GDT_BLK_CUT_SCROLL_8X16 == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_SCROLL_8X16      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_SCROLL_8X16
#endif

#if (GDT_CFG_V_GDT_SCROLL_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SCROLL_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_SCROLL_CFG
#endif

#if (GDT_CFG_V_GDT_SEQ_GEN_SCROLL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_SCROLL      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_SCROLL
#endif

#if (GDT_CFG_V_GDT_SEQ_SCROLL_BIG_TRANS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_SCROLL_BIG_TRANS      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_SEQ_SCROLL_BIG_TRANS
#endif

#if (GDT_CFG_V_ISR_DMAC_SCROLL_INPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_SCROLL_INPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_ISR_DMAC_SCROLL_INPUT
#endif

#if (GDT_CFG_V_ISR_DMAC_SCROLL_OUTPUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_SCROLL_OUTPUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_ISR_DMAC_SCROLL_OUTPUT
#endif

#if (GDT_CFG_V_GDT_ISCR_DMAC_BLK_UPINF_IN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_ISCR_DMAC_BLK_UPINF_IN      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_ISCR_DMAC_BLK_UPINF_IN
#endif

#if (GDT_CFG_V_GDT_ISCR_DMAC_BLK_UPINF_OUT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_ISCR_DMAC_BLK_UPINF_OUT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_ISCR_DMAC_BLK_UPINF_OUT
#endif

#if (GDT_CFG_E_GDT_JUDGE_CFG_FOUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_CFG_FOUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_CFG_FOUNT
#endif

#if (GDT_CFG_E_GDT_JUDGE_DEST_FOUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_FOUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_FOUNT
#endif

#if (GDT_CFG_V_GDT_FOUNT_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_FOUNT_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_FOUNT_CFG
#endif

#if (GDT_CFG_V_GDT_SEQ_GEN_FOUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_FOUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_FOUNT
#endif

#if (GDT_CFG_V_GDT_FOUNT_FINISH == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_FOUNT_FINISH      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_FOUNT_FINISH
#endif

#if (GDT_CFG_V_ISR_CPU_OUTPUT_FOUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_FOUNT      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_FOUNT
#endif

#if (GDT_CFG_V_GDT_CPU_UPDATE_OUT_FOUNT_INFO == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_FOUNT_INFO      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_FOUNT_INFO
#endif

#if (GDT_CFG_V_GDT_FOUNT_DMACIN_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_FOUNT_DMACIN_SET      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_FOUNT_DMACIN_SET
#endif

#if (GDT_CFG_E_GDT_JUDGE_CIAL_DEST_MEM_SIZE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_CIAL_DEST_MEM_SIZE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_E_GDT_JUDGE_CIAL_DEST_MEM_SIZE
#endif

#if (GDT_CFG_V_GDT_BLK_CUT_CLRALIGN_16X16 == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_CLRALIGN_16X16      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_BLK_CUT_CLRALIGN_16X16
#endif

#if (GDT_CFG_V_GDT_COLORALIGN_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_COLORALIGN_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_COLORALIGN_CFG
#endif

#if (GDT_CFG_V_GDT_SEQ_GEN_COLORALIGN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_COLORALIGN      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_COLORALIGN
#endif

#if (GDT_CFG_V_GDT_COLORSYN_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_COLORSYN_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_COLORSYN_CFG
#endif

#if (GDT_CFG_V_GDT_COLOR_CFG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_GDT_COLOR_CFG      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else                       
#define FUNC_LOCATION_PRV_V_GDT_COLOR_CFG
#endif

/******************************************************************************************************************//**
 * common internal function
 *********************************************************************************************************************/
static void v_gdt_on(void) FUNC_LOCATION_PRV_V_GDT_ON;
static void v_gdt_off(void) FUNC_LOCATION_PRV_V_GDT_OFF;
static void v_gdt_clear_reg(void) FUNC_LOCATION_PRV_V_GDT_CLEAR_REG;
static e_gdt_err_t e_gdt_check_img_num(uint8_t gdt_function, uint8_t special_reg, uint8_t src_img_num, uint8_t dest_img_num) FUNC_LOCATION_PRV_E_GDT_CHECK_IMG_NUM;
static e_gdt_err_t e_gdt_check_dmac_cfg(st_trans_mod_cfg_t* trans_mod_cfg) FUNC_LOCATION_PRV_E_GDT_CHECK_DMAC_CFG; 
static e_gdt_err_t e_gdt_judge_img_cfg(st_img_in_info_t     * st_img_src_t,
                                       st_img_out_info_t    * st_img_dest_t,
                                       st_blk_conv_info_t   * st_blk_conv_info_t,
                                       uint32_t gdtdsz, uint8_t gdt_function) 
                                       FUNC_LOCATION_PRV_E_GDT_JUDGE_IMG_CFG;         

static e_gdt_err_t e_gdt_judge_img_src_cfg(st_img_in_info_t     * st_img_src_t,
                                           st_blk_conv_info_t   * st_blk_conv_info_t,
                                           uint32_t gdtdsz, uint8_t gdt_function) 
                                           FUNC_LOCATION_PRV_E_GDT_JUDGE_IMG_CFG;         

static e_gdt_err_t e_gdt_judge_img_dest_cfg(st_img_out_info_t    * st_img_dest_t,
                                            st_blk_conv_info_t   * st_blk_conv_info_t,
                                            uint32_t gdtdsz, uint8_t gdt_function) 
                                            FUNC_LOCATION_PRV_E_GDT_JUDGE_IMG_CFG;         

static e_gdt_err_t e_gdt_judge_dest_comm_blkend_ob(st_img_in_info_t   * st_img_src_t, 
                                                   st_img_out_info_t  * st_img_dest_t,
                                                   st_blk_conv_info_t * st_blk_conv_info_t)
                                                   FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_COMM_BLKEND_OB;

static void v_gdt_basic_info_calc(st_img_in_info_t  * img_src ,
                                  st_img_out_info_t * img_dest) FUNC_LOCATION_PRV_V_GDT_BASIC_INFO_CALC;

static e_gdt_err_t e_gdt_dmac_in_channel_sel(st_trans_mod_cfg_t * trans_mod_cfg)
                                             FUNC_LOCATION_PRV_E_GDT_DMAC_IN_CHANNEL_SEL;

static e_gdt_err_t e_gdt_dmac_out_channel_sel(st_trans_mod_cfg_t * trans_mod_cfg)
                                              FUNC_LOCATION_PRV_E_GDT_DMAC_OUT_CHANNEL_SEL; 

static void v_gdt_blk_cut_comm_8x8(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest) FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_8X8;
static void v_gdt_blk_cut_comm_16x16(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest) FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16; 
static void v_gdt_blk_cut_comm_16x16_burst(uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM], uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM], 
                                           uint32_t block_size_h_byte, uint32_t block_size_v_line)
                                           FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16_BURST;

static void v_gdt_blk_cut_comm_16x16_block(uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM], uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM], 
                                           uint32_t block_size_h_byte, uint32_t block_size_v_line)
                                           FUNC_LOCATION_PRV_V_GDT_BLK_CUT_COMM_16X16_BLOCK;


static void v_gdt_dmacin_pre_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                                 uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length, 
                                 uint32_t offset, uint32_t inaddr, isr_function const isr_func, 
                                 gdt_cb_event_t const p_callback)
                                 FUNC_LOCATION_PRV_V_GDT_DMACIN_PRE_SET; 

static void v_gdt_dmacin_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                             uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length, 
                             uint32_t offset, uint32_t inaddr, gdt_cb_event_t const p_callback)
                             FUNC_LOCATION_PRV_V_GDT_DMACIN_SET; 

static void v_gdt_dmacout_pre_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm,
                                  uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length,
                                  uint32_t offset, uint32_t outaddr, isr_function const isr_func,
                                  gdt_cb_event_t const p_callback)
                                  FUNC_LOCATION_PRV_V_GDT_DMACOUT_PRE_SET; 

static void v_gdt_seq_gen_common( gdt_cb_event_t const p_callback )FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_COMMON;
static void v_gdt_seq_comm_big_trans_blk(uint32_t src_data_length, uint32_t dest_data_length,
                                         gdt_cb_event_t const p_callback ) FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_BIG_TRANS_BLK;

static void v_gdt_seq_big_trans_src(uint32_t data_length,
                                    gdt_cb_event_t const p_callback ) FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_BIG_TRANS_BLK; 

static void v_gdt_seq_big_trans_dest(uint32_t data_length,
                                     gdt_cb_event_t const p_callback ) FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_BIG_TRANS_BLK; 

static void v_gdt_seq_comm_remn_trans_xbyte(void) FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_REMN_TRANS_XBYTE;
static void v_gdt_seq_comm_remn_trans_byte(void) FUNC_LOCATION_PRV_V_GDT_SEQ_COMM_REMN_TRANS_BYTE;
static void v_gdt_imgdata_trans_irq_enable(void) FUNC_LOCATION_PRV_V_GDT_IMGDATA_TRANS_IRQ_ENABLE;
static void v_gdt_imgdata_trans_irq_disable(void) FUNC_LOCATION_PRV_V_GDT_IMGDATA_TRANS_IRQ_DISABLE;
static void v_isr_cpu_input (void) FUNC_LOCATION_PRV_V_ISR_CPU_INPUT;
static void v_isr_cpu_output (void) FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT;
static void v_isr_cpu_input_xbyte (void) FUNC_LOCATION_PRV_V_ISR_CPU_INPUT_XBYTE;       
static void v_isr_cpu_output_xbyte (void) FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_XBYTE;       
static void v_isr_dmac_input (void) FUNC_LOCATION_PRV_V_ISR_DMAC_INPUT;
static void v_isr_dmac_output (void) FUNC_LOCATION_PRV_V_ISR_DMAC_OUTPUT;
static void v_isr_dmac_output_once (void) FUNC_LOCATION_PRV_V_ISR_DMAC_OUTPUT_ONCE;
static void v_gdt_cpu_update_in_info(void) FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_IN_INFO;
static void v_gdt_cpu_update_out_info(void) FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_INFO;
static void v_gdt_cpublk_byte_wr_gdt_normal (uint32_t src_addr,    uint32_t dest_addr,
                                             uint32_t offset_src_addr, uint32_t data_n) 
                                             FUNC_LOCATION_PRV_V_GDT_CPUBLK_BYTE_WR_GDT_NORMAL;

static void v_gdt_cpublk_byte_rd_gdt_normal (uint32_t src_addr,    uint32_t dest_addr,
                                             uint32_t offset_dest_addr, uint32_t data_n) 
                                             FUNC_LOCATION_PRV_V_GDT_CPUBLK_BYTE_RD_GDT_NORMAL;

static st_cpublk_xbyte_trans_info_t st_gdt_cpublk_xbyte_wr_gdt_nor (uint32_t src_addr,    uint32_t dest_addr, 
                                                                  uint32_t offset_src_addr, uint32_t data_n,
                                                                  uint32_t offset_line_src_addr,
                                                                  uint32_t range_h_byte, uint32_t range_start_num )
                                                                  FUNC_LOCATION_PRV_ST_GDT_CPUBLK_XBYTE_WR_GDT_NOR;

static st_cpublk_xbyte_trans_info_t st_gdt_cpublk_xbyte_rd_gdt_nor (uint32_t src_addr,    uint32_t dest_addr, 
                                                                  uint32_t offset_dest_addr, uint32_t data_n,
                                                                  uint32_t offset_line_dest_addr,
                                                                  uint32_t range_h_byte, uint32_t range_start_num )
                                                                  FUNC_LOCATION_PRV_ST_GDT_CPUBLK_XBYTE_RD_GDT_NOR;

static void v_gdt_dmac_blk_upinf_in_array(void)FUNC_LOCATION_PRV_V_GDT_DMAC_BLK_UPINF_IN_ARRAY; 
static void v_gdt_dmac_blk_upinf_out_array(void)FUNC_LOCATION_PRV_V_GDT_DMAC_BLK_UPINF_OUT_ARRAY;

/******************************************************************************************************************//**
 * Shrink internal function
 *********************************************************************************************************************/
static void v_gdt_cale_shrnksize_info(st_gdt_shrink_para_cfg_t  * gdt_shrink_para_cfg)
                                      FUNC_LOCATION_PRV_V_GDT_CALE_SHRNKSIZE_INFO; 

static e_gdt_err_t e_gdt_judge_dest_blkend_obound(st_img_in_info_t * st_img_src_t,
                                                  st_img_out_info_t * st_img_dest_t,
                                                  st_blk_conv_info_t * st_blk_conv_info_t ) 
                                                  FUNC_LOCATION_PRV_E_GDT_JUDGE_BLKEND_OBOUND;         
                                                
static void v_gdt_blk_cut_shrink(st_blk_conv_info_t * blk) FUNC_LOCATION_PRV_V_GDT_BLK_CUT_SHRINK;
static void v_gdt_shrink_cfg(st_gdt_shrink_para_cfg_t * gdt_shrink_para_cfg) FUNC_LOCATION_PRV_V_GDT_SHRINK_CFG;
static void v_gdt_shrink_num_cfg(uint8_t shrinknum) FUNC_LOCATION_PRV_V_GDT_SHRINK_NUM_CFG;

static void v_gdt_calc_trans_info(void) FUNC_LOCATION_PRV_V_GDT_CALC_TRANS_INFO;
static void v_gdt_seq_gen_shrink(gdt_cb_event_t const p_callback) FUNC_LOCATION_PRV_V_GDT_SEQ_GEN;
static void v_gdt_seq_shrnk_big_trans(gdt_cb_event_t const p_callback) FUNC_LOCATION_PRV_V_GDT_SEQ_SHRNK_BIG_TRANS;
static void v_gdt_seq_shrnk_remain_trans(gdt_cb_event_t const p_callback) FUNC_LOCATION_PRV_V_GDT_SEQ_SHRNK_REMAIN_TRANS;
static void v_isr_dmac_shk_output (void) FUNC_LOCATION_PRV_V_ISR_DMAC_SHK_OUTPUT;
static void v_isr_cpu_output_limit (void) FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_LIMIT;      
static void v_gdt_shk_dmac_update_out_info(void) FUNC_LOCATION_PRV_V_GDT_DMAC_UPDATE_OUT_INFO;
static void v_gdt_cpuline_byte_rd_gdt_limit (uint32_t src_addr,    uint32_t dest_addr,
                                             uint32_t valid_data_n,uint32_t total_data_n)
                                             FUNC_LOCATION_PRV_V_GDT_CPULINE_BYTE_RD_GDT_LIMIT;

static void v_gdt_cpu_update_out_limit_info(void) FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_LIMIT_INFO;

/******************************************************************************************************************//**
 * Endian internal function
 *********************************************************************************************************************/
static void v_gdt_endian_cfg(st_gdt_endian_para_cfg_t* gdt_endian_para_cfg)FUNC_LOCATION_PRV_V_GDT_ENDIAN_CFG;

/******************************************************************************************************************//**
 * Monochrome internal function
 *********************************************************************************************************************/
static void v_gdt_monochrome_cfg(st_gdt_monochrome_para_cfg_t* gdt_monochrome_para_cfg)FUNC_LOCATION_PRV_V_GDT_MONOCHROME_CFG;

/******************************************************************************************************************//**
 * Rotate internal function
 *********************************************************************************************************************/
static e_gdt_err_t e_gdt_judge_dest_rtt_blkend_ob(uint8_t rttfc,
                                                     st_img_in_info_t* st_img_src_t, 
                                                     st_img_out_info_t* st_img_dest_t,
                                                     st_blk_conv_info_t* st_blk_conv_info_t) FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_RTT_BLKEND_OB;

static void v_gdt_rotate_cfg(st_gdt_rotate_para_cfg_t* gdt_rotate_para_cfg) FUNC_LOCATION_PRV_V_GDT_ROTATE_CFG;
static void v_gdt_seq_gen_rotate( gdt_cb_event_t const p_callback ) FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_ROTATE;
static void v_gdt_seq_rotate_big_trans_blk(uint32_t data_length, gdt_cb_event_t const p_callback ) FUNC_LOCATION_PRV_V_GDT_SEQ_ROTATE_BIG_TRANS_BLK;
static void v_gdt_seq_rotate_big_trans_src(uint32_t data_length, gdt_cb_event_t const p_callback) FUNC_LOCATION_PRV_V_GDT_SEQ_ROTATE_BIG_TRANS_BLK;
static void v_gdt_seq_rotate_big_trans_dest(uint32_t data_length, gdt_cb_event_t const p_callback) FUNC_LOCATION_PRV_V_GDT_SEQ_ROTATE_BIG_TRANS_BLK;
static st_unit_output_coodinate_t st_gdt_rotate_cal_out_coodinate(uint8_t  gdt_rttfc,
                                                                  uint32_t input_coodinate_h, 
                                                                  uint32_t input_coodinate_v, 
                                                                  uint32_t block_size_h, 
                                                                  uint32_t block_size_v) FUNC_LOCATION_PRV_ST_GDT_ROTATE_CAL_OUT_COODINATE;

static void v_isr_dmac_rotate_output (void) FUNC_LOCATION_PRV_V_ISR_DMAC_ROTATE_OUTPUT;
static void v_gdt_rotate_update_out_info(void) FUNC_LOCATION_PRV_V_GDT_ROTATE_UPDATE_OUT_INFO;

/******************************************************************************************************************//**
 * Scroll internal function
 *********************************************************************************************************************/
static e_gdt_err_t e_gdt_judge_dest_iscr_blkend_ob(st_img_in_info_t* st_img_src_t,st_img_out_info_t* st_img_dest_t,st_blk_conv_info_t* st_blk_conv_info_t) FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_ISCR_BLKEND_OB;
static void v_gdt_blk_cut_scroll_8x16(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest) FUNC_LOCATION_PRV_V_GDT_BLK_CUT_SCROLL_8X16;
static void v_gdt_scroll_cfg(st_gdt_scroll_para_cfg_t* gdt_scroll_para_cfg) FUNC_LOCATION_PRV_V_GDT_SCROLL_CFG;
static void v_gdt_seq_gen_scroll( gdt_cb_event_t const p_callback) FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_SCROLL;
static void v_gdt_seq_scroll_big_trans(uint32_t data_length,gdt_cb_event_t const p_callback) FUNC_LOCATION_PRV_V_GDT_SEQ_SCROLL_BIG_TRANS;
static void v_isr_dmac_scroll_input (void) FUNC_LOCATION_PRV_V_ISR_DMAC_SCROLL_INPUT;
static void v_isr_dmac_scroll_output (void) FUNC_LOCATION_PRV_V_ISR_DMAC_SCROLL_OUTPUT;

static void v_gdt_iscr_dmac_blk_upinf_in(void) FUNC_LOCATION_PRV_V_GDT_ISCR_DMAC_BLK_UPINF_IN;
static void v_gdt_iscr_dmac_blk_upinf_out(void) FUNC_LOCATION_PRV_V_GDT_ISCR_DMAC_BLK_UPINF_OUT;

/******************************************************************************************************************//**
 * Fount internal function
 *********************************************************************************************************************/
static e_gdt_err_t e_gdt_judge_cfg_fount(st_gdt_fount_para_cfg_t* gdt_fount_para_cfg)FUNC_LOCATION_PRV_E_GDT_JUDGE_CFG_FOUNT; 
static e_gdt_err_t e_gdt_judge_dest_fount(st_img_in_info_t* st_img_src_t, 
                                          st_img_out_info_t* st_img_dest_t,
                                          st_blk_conv_info_t* st_blk_conv_info_t)FUNC_LOCATION_PRV_E_GDT_JUDGE_DEST_FOUNT;
static void v_gdt_fount_cfg(st_gdt_fount_para_cfg_t* gdt_fount_para_cfg, uint8_t fdir)FUNC_LOCATION_PRV_V_GDT_FOUNT_CFG;
static void v_gdt_seq_gen_fount(
                                st_img_in_info_t             * img_src,      
                                st_img_out_info_t            * img_dest,     
                                st_blk_conv_info_t           * blk_conv_info,
                                uint8_t dmac_in_block_num,   
                                uint8_t dmac_in_data_length, 
                                uint8_t dmac_out_block_num,  
                                uint8_t dmac_out_data_length,
                                gdt_cb_event_t const p_callback )FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_FOUNT;
static void v_gdt_fount_finish( gdt_cb_event_t const p_callback )FUNC_LOCATION_PRV_V_GDT_FOUNT_FINISH;
static void v_isr_cpu_output_fount (void)FUNC_LOCATION_PRV_V_ISR_CPU_OUTPUT_FOUNT;
static void v_gdt_cpu_update_out_fount_info(void)FUNC_LOCATION_PRV_V_GDT_CPU_UPDATE_OUT_FOUNT_INFO;

static void v_gdt_fount_dmacin_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                             uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length, uint32_t transfer_count,
                             uint32_t inaddr, uint32_t outaddr,gdt_cb_event_t const p_callback)FUNC_LOCATION_PRV_V_GDT_FOUNT_DMACIN_SET;

/******************************************************************************************************************//**
 * coloralign internal function
 *********************************************************************************************************************/
static e_gdt_err_t e_gdt_judge_cial_dest_mem_size(st_img_out_info_t* st_img_dest_t, uint8_t cialgsl)FUNC_LOCATION_PRV_E_GDT_JUDGE_CIAL_DEST_MEM_SIZE;
static void v_gdt_blk_cut_clralign_16x16(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest) FUNC_LOCATION_PRV_V_GDT_BLK_CUT_CLRALIGN_16X16; 
static void v_gdt_coloralign_cfg(st_gdt_coloralign_para_cfg_t* gdt_coloralign_para_cfg)FUNC_LOCATION_PRV_V_GDT_COLORALIGN_CFG;
static void v_gdt_seq_gen_coloralign( uint8_t cialg_bit_mod, gdt_cb_event_t const p_callback )FUNC_LOCATION_PRV_V_GDT_SEQ_GEN_COLORALIGN;

/******************************************************************************************************************//**
 * colorsyn internal function
 *********************************************************************************************************************/
static void v_gdt_colorsyn_cfg(st_gdt_colorsyn_para_cfg_t* gdt_colorsyn_para_cfg)FUNC_LOCATION_PRV_V_GDT_COLORSYN_CFG;

/******************************************************************************************************************//**
 * color internal function
 *********************************************************************************************************************/
static void v_gdt_color_cfg(st_gdt_color_para_cfg_t* gdt_color_para_cfg)FUNC_LOCATION_PRV_V_GDT_COLOR_CFG;

/** GDT resources */
static st_gdt_resources_t gs_gdt_resources DATA_LOCATION_PRV_DMAC_RESOURCES = 
{
    /* GDT resources */
   (GDT_Type*)GDT,     /* GDT   register base address */
   (DMAC0_Type*)DMAC0, /* DMAC0 register base address */
   (DMAC1_Type*)DMAC1, /* DMAC1 register base address */
   (DMAC2_Type*)DMAC2, /* DMAC2 register base address */
   (DMAC3_Type*)DMAC3, /* DMAC3 register base address */
   (ICU_Type*) ICU,    /* ICU   register base address */
   (MSTP_Type*)MSTP,   /* MSTP  register base address */
   LPM_MSTP_GDT,
   GDT_CFG_IN_INT_PRIORITY,
   GDT_CFG_OUT_INT_PRIORITY,
   DMAC0_INT_PRIORITY, /* DMAC0 interrupt priority value */
   DMAC1_INT_PRIORITY, /* DMAC1 interrupt priority value */
   DMAC2_INT_PRIORITY, /* DMAC2 interrupt priority value */
   DMAC3_INT_PRIORITY, /* DMAC3 interrupt priority value */
};

/** the variable is prepared for big in  range of image */
static st_range_in_addr_info_t  gs_big_in_range_addr_info_t;

/** the variable is prepared for big out range of image */
static st_range_out_addr_info_t gs_big_out_range_addr_info_t;

/** the variable is prepared for remain in range of image */
static st_range_in_addr_info_t  gs_remain_in_range_addr_info_t;

/** the variable is prepared for remain out range of image */
static st_range_out_addr_info_t gs_remain_out_range_addr_info_t;

/** image in number counter */
static uint32_t gs_in_img_cnt;

/** image out number counter */
static uint32_t gs_out_img_cnt;

/** in data horizon data unit counter */
static uint16_t gs_in_dat_unit_cnt_h;

/** in data vertical data unit counter */
static uint32_t gs_in_dat_unit_cnt_v;

/** in image horizon originate address */
static uint32_t gs_in_img_addr_line_org_array[IMG_IN_FRAME_NUM];

/** in image address */
static uint32_t gs_in_img_addr;

/** in images address */
static uint32_t gs_in_img_addr_array[IMG_IN_FRAME_NUM];

/** cpu/dmac in horizon offset */ 
static uint16_t gs_in_offset_h;

/** cpu/dmac in vertical offset */ 
static uint16_t gs_in_offset_v[IMG_IN_FRAME_NUM];

/** in data line data unit counter */
static uint16_t gs_in_dat_unit_line_y;

/** cpu/dmac in data horizon amount */ 
static uint16_t gs_in_dat_unit_num_h;

/** cpu/dmac in data vertical amount */ 
static uint16_t gs_in_dat_unit_num_v;

/** GDT IBUF addr */
static uint32_t gs_in_buf_addr;

/** out data horizon data unit counter */
static uint8_t  gs_out_dat_unit_cnt_y;

/** out data horizon data unit counter */
static uint16_t gs_out_dat_unit_cnt_h;

/** out data vertical data unit counter */
static uint32_t gs_out_dat_unit_cnt_v;

/** out image horizon originate address */
static uint32_t gs_out_img_addr_line_org_array[IMG_OUT_FRAME_NUM];

/** out image address */
static uint32_t gs_out_img_addr;

/** out images address */
static uint32_t gs_out_img_addr_array[IMG_OUT_FRAME_NUM];

/** cpu/dmac out horizon offset */
static uint16_t gs_out_offset_h;

/** cpu/dmac out vertical offset */ 
static uint16_t gs_out_offset_v[IMG_OUT_FRAME_NUM];

/** out data line data unit counter */
static uint16_t gs_out_dat_unit_line_y;

/** cpu/dmac out data horizon amount */ 
static uint16_t gs_out_dat_unit_num_h;

/** cpu/dmac out data vertical amount */ 
static uint16_t gs_out_dat_unit_num_v;

/** GDT OBUF addr */
static uint32_t gs_out_buf_addr;  

/** the row address of out image */
static uint32_t gs_out_img_addr_unit_line_org;

/** memory h size global variable */  
static uint16_t gs_src_mem_size_h[IMG_IN_FRAME_NUM];

/** memory h size global variable */
static uint16_t gs_dest_mem_size_h[IMG_OUT_FRAME_NUM];

/** gdt special reg global variable */
static uint8_t  gs_special_reg;

/** mode for transferring remain area gdt data */
static uint16_t gs_remain_trans_mod;

/** gdt process unit size */
static uint16_t gs_gdt_unit_size;

/** cpu/dmac in offset */ 
static uint16_t gs_in_offset;

/** cpu/dmac out offset */ 
static uint16_t gs_out_offset;

static uint32_t volatile *gsp_dmac_reg_in_sar;
static uint32_t volatile *gsp_dmac_reg_in_dar;
static uint8_t  volatile *gsp_dmac_reg_in_cnt;
static uint16_t volatile *gsp_dmac_reg_in_crb;
static uint32_t volatile *gsp_dmac_reg_in_ofr;

static uint32_t volatile *gsp_dmac_reg_out_sar;
static uint32_t volatile *gsp_dmac_reg_out_dar;
static uint8_t  volatile *gsp_dmac_reg_out_cnt;
static uint16_t volatile *gsp_dmac_reg_out_crb;
static uint32_t volatile *gsp_dmac_reg_out_ofr;

static DRIVER_DMA *gsp_dmacdev_in ;
static DRIVER_DMA *gsp_dmacdev_out;

/**  call back configuration seq_gen */
static gdt_cb_event_t            gs_int_callback;

/** block start pix horizon low 2 bits */ 
uint32_t blk_start_pix_h_src_low2bit; 

/** block start pix vertical low 2 bits */ 
uint32_t blk_start_pix_v_src_low2bit;

extern DRIVER_DMA Driver_DMAC0; 
extern DRIVER_DMA Driver_DMAC1; 
extern DRIVER_DMA Driver_DMAC2; 
extern DRIVER_DMA Driver_DMAC3; 

/** gdt srouce block information */
st_img_in_prosess_t  gdt_img_src_t;

/** gdt destination block information */
st_img_out_prosess_t gdt_img_dest_t;

/** gdt big range information */
st_range_info_t      gdt_big_range_info_t;

/** gdt remain range information */
st_range_info_t      gdt_remain_range_info_t;

/******************************************************************************************************************//**
 * external function
 *********************************************************************************************************************/

/********************************************************************//**
* @brief Get API Version
* @retval version Driver version number
***********************************************************************/
/* Function Name : R_GDT_GetVersion */
uint32_t R_GDT_GetVersion(void)
{
    uint32_t const version = (GDT_VERSION_MAJOR << 16) | GDT_VERSION_MINOR;
    return version;
} /* End of function R_GDT_GetVersion */

/********************************************************************//**
* @brief Ready to start GDT clock
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR               unspecified error                               
***********************************************************************/
/* Function Name : R_GDT_Open */
e_gdt_err_t R_GDT_Open(void)
{
    e_gdt_err_t result;
    result = GDT_OK;

    /* open GDT lpm mask bit */
    if (0 != R_LPM_ModuleStart((e_lpm_mstp_t)LPM_MSTP_GDT))
    {
        result = GDT_ERROR;
    }

    /* Release module stop check */
    if (0 != (gs_gdt_resources.reg_mstp->MSTPCRC_b.MSTPC26))
    {
        return GDT_ERROR;
    }
    return result;
} /* End of function R_GDT_Open */

/********************************************************************//**
* @brief Ready to close GDT clock
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR               unspecified error                               
***********************************************************************/
/* Function Name : R_GDT_Close */
e_gdt_err_t R_GDT_Close(void)
{
    e_gdt_err_t result;
    result = GDT_OK;
 
    /* close GDT lpm mask bit */
    if (0 != R_LPM_ModuleStop((e_lpm_mstp_t)LPM_MSTP_GDT))
    {
        result = GDT_ERROR;
    }
    return result;
} /* End of function R_GDT_Close */


/********************************************************************//**
* @brief DMAC channel select 
* @param[in] trans_mod_cfg        Dmac channel select by Gdt used
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_MOD_CFG       GDT mode configuration error                    
* @retval GDT_ERROR_DMAC_CH_CFG   DMAC channel select error                       
***********************************************************************/
/* Function Name : R_GDT_DmacChSel */
e_gdt_err_t R_GDT_DmacChSel(st_trans_mod_cfg_t  * trans_mod_cfg)
{
    e_gdt_err_t result;

    result = e_gdt_check_dmac_cfg(trans_mod_cfg);
    if(GDT_OK != result)
    {
        return result;
    }

    /* input DMAC channel select*/
    result = e_gdt_dmac_in_channel_sel(trans_mod_cfg);
    if(GDT_OK != result)
    {
        return result;
    }

    /* output DMAC channel select*/
    result = e_gdt_dmac_out_channel_sel(trans_mod_cfg);
    if(GDT_OK != result)
    {
        return result;
    }

    /* DMAC resource is occupied check */
    if (gsp_dmacdev_in->Open() != DMA_OK)
    {
        return GDT_ERROR_DMAC_CH_CFG;  
    }

    if (gsp_dmacdev_in->Close() != DMA_OK)
    {
        gs_int_callback();
    }

    if (gsp_dmacdev_out->Open() != DMA_OK)
    {
        return GDT_ERROR_DMAC_CH_CFG;  
    }

    if (gsp_dmacdev_out->Close() != DMA_OK)
    {
        gs_int_callback();
    }

    return result;
} /* End of function R_GDT_DmacChSel */


/********************************************************************//**
* @brief The function shrink image. This function should be called when shrinking the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_shrink_para_cfg     GDT shrink parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     Successful operation                            
* @retval GDT_ERROR                  Unspecified error                               
* @retval GDT_ERROR_IMG_CFG          Image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    Block over bound error                          
* @retval GDT_ERROR_BLK_CFG          Source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Shrink */
e_gdt_err_t R_GDT_Shrink(st_img_in_info_t         * img_src, 
                         st_img_out_info_t        * img_dest,
                         st_blk_conv_info_t       * blk_conv_info,
                         st_gdt_shrink_para_cfg_t * gdt_shrink_para_cfg,
                         gdt_cb_event_t const     p_callback )
{
    uint32_t gdtdsz; 
    uint8_t src_img_num;
    uint8_t dest_img_num;

    e_gdt_err_t ret = GDT_OK;
    
    gs_int_callback = p_callback;
    
    gdtdsz = 1;             /* gdt data size sel 0:16*16 1:8*8*/
    gs_gdt_unit_size = 0x8; /* GDT process unit size 8*8bit */ 
    
    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 

    /* GDT check image number NULL mean no used */
    ret = e_gdt_check_img_num(GDT_SHRNK, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_SHRNK);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* calculation shrinksieze */
    v_gdt_cale_shrnksize_info(gdt_shrink_para_cfg); 

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_blkend_obound(img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src, img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* seting interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    v_gdt_blk_cut_shrink(blk_conv_info);

    v_gdt_calc_trans_info();

    v_gdt_shrink_cfg(gdt_shrink_para_cfg);

    v_gdt_seq_gen_shrink(gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Shrink */

/********************************************************************//**
* @brief The function endian image. This function should be called when endian the image.
* @param[in] img_src              GDT source image information           
* @param[in] img_dest             GDT destination image information          
* @param[in] blk_conv_info        GDT block information
* @param[in] gdt_endian_para_cfg  GDT endian parameter configuration
* @param[in,out] p_callback       API call back function
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR               unspecified error                               
* @retval GDT_ERROR_IMG_CFG       image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
* @retval GDT_ERROR_BLK_CFG       source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG       GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM       GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Endian */
e_gdt_err_t R_GDT_Endian(st_img_in_info_t         * img_src, 
                         st_img_out_info_t        * img_dest,
                         st_blk_conv_info_t       * blk_conv_info,
                         st_gdt_endian_para_cfg_t * gdt_endian_para_cfg,
                         gdt_cb_event_t const     p_callback )
{
    uint32_t gdtdsz; 
    uint8_t src_img_num;
    uint8_t dest_img_num;
    e_gdt_err_t ret = GDT_OK;
  
    gs_int_callback = p_callback;
    
    gdtdsz = 0;              /* gdt data size sel 0:16*16 1:8*8 */
    gs_gdt_unit_size = 0x10; /* GDT process unit size 16*16bit */ 

    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 

    /* GDT check image number '0' mean no used */
    ret = e_gdt_check_img_num(GDT_ENDIAN, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_ENDIAN);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_comm_blkend_ob(img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src ,img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* setting GDT output interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    /* cut block unit(16*16) */
    v_gdt_blk_cut_comm_16x16(blk_conv_info, img_src, img_dest); 

    v_gdt_endian_cfg(gdt_endian_para_cfg);
    v_gdt_seq_gen_common(gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Endian */

/********************************************************************//**
* @brief The function monochrome image. This function should be called when monochrome the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_monochrome_para_cfg GDT monochrome parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     successful operation                            
* @retval GDT_ERROR                  unspecified error                               
* @retval GDT_ERROR_IMG_CFG          image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    block over bound error                          
* @retval GDT_ERROR_BLK_CFG          source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Monochrome */
e_gdt_err_t R_GDT_Monochrome(st_img_in_info_t             * img_src, 
                             st_img_out_info_t            * img_dest,
                             st_blk_conv_info_t           * blk_conv_info,
                             st_gdt_monochrome_para_cfg_t * gdt_monochrome_para_cfg,
                             gdt_cb_event_t const         p_callback )
{
    uint32_t gdtdsz; 
    uint8_t src_img_num;
    uint8_t dest_img_num;
    e_gdt_err_t ret = GDT_OK;
  
    gs_int_callback = p_callback;
    
    gdtdsz = gdt_monochrome_para_cfg->gdtdsz; /* gdt data size sel */

    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 


    /* GDT check image number */
    ret = e_gdt_check_img_num(GDT_MONOCHROME, gdt_monochrome_para_cfg->mbrden, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* GDT process unit size sel */
    if( 1 == gdtdsz ) /* 8*8bit */
    {        
        gs_gdt_unit_size = 0x8; 
    }
    else              /* 16*16bit no used */ 
    {        
        gs_gdt_unit_size = 0x10; 
        if(0x10 == gs_gdt_unit_size) 
        {
            ret = GDT_ERROR_MOD_CFG;    
            return ret;        
        }
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_MONOCHROME);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_comm_blkend_ob(img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src ,img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* setting GDT output interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    if( 8 == gs_gdt_unit_size )
    {
        v_gdt_blk_cut_comm_8x8(blk_conv_info, img_src, img_dest); 
    }
    else /* 16x16 not used */
    {
        v_gdt_blk_cut_comm_16x16(blk_conv_info, img_src, img_dest); 
    }
    
    
    
    

    /* GDT register cfg  */
    v_gdt_monochrome_cfg(gdt_monochrome_para_cfg);

    v_gdt_seq_gen_common(gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Monochrome */

/********************************************************************//**
* @brief The function rotate image. This function should be called when rotate the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_rotate_para_cfg     GDT rotate parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     successful operation                            
* @retval GDT_ERROR                  unspecified error                               
* @retval GDT_ERROR_IMG_CFG          image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    block over bound error                          
* @retval GDT_ERROR_BLK_CFG          source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Rotate */
e_gdt_err_t R_GDT_Rotate(st_img_in_info_t             * img_src, 
                         st_img_out_info_t            * img_dest,
                         st_blk_conv_info_t           * blk_conv_info,
                         st_gdt_rotate_para_cfg_t     * gdt_rotate_para_cfg,
                         gdt_cb_event_t const         p_callback )
{
    uint32_t gdtdsz; 
    uint8_t  src_img_num;
    uint8_t  dest_img_num;
    uint32_t line_start_addr_src;
    uint32_t line_start_addr_dest;
    uint32_t addr_offset_src;  
    uint32_t addr_offset_dest;  
    uint32_t block_start_pix_addr_src_array;
    uint32_t block_start_pix_addr_dest_array;
    e_gdt_err_t ret = GDT_OK;
    gs_int_callback = p_callback;
    
    gs_special_reg = gdt_rotate_para_cfg->rttfc;  /* use gs_special_reg to store rttfc */
    gdtdsz         = gdt_rotate_para_cfg->gdtdsz; /* gdt data size sel */
    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 

    /* GDT check image number '0' mean no used */
    ret = e_gdt_check_img_num(GDT_ROTATE, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* GDT process unit size sel */
    if( 1 == gdtdsz ) /* 8*8bit */
    {        
        gs_gdt_unit_size = 0x8; 
    }
    else              /* 16*16bit */ 
    {        
        gs_gdt_unit_size = 0x10; 
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_ROTATE);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_rtt_blkend_ob(gdt_rotate_para_cfg->rttfc, img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src ,img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* setting GDT output interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    if( 8 == gs_gdt_unit_size )
    {
        v_gdt_blk_cut_comm_8x8(blk_conv_info, img_src, img_dest); 
    }
    else 
    {
        /* in the 16bit mode of rotate function input addr of src and output addr of dest must be even */
        line_start_addr_src  = blk_conv_info->start_pix_v_src[0]  * gs_src_mem_size_h[0];
        addr_offset_src      = line_start_addr_src  + (blk_conv_info->start_pix_h_src[0] >> 3);
        line_start_addr_dest = blk_conv_info->start_pix_v_dest[0] * gs_dest_mem_size_h[0];
        addr_offset_dest     = line_start_addr_dest + (blk_conv_info->start_pix_h_dest[0] >> 3);

        block_start_pix_addr_src_array  = img_src->start_addr[0]  + addr_offset_src;
        block_start_pix_addr_dest_array = img_dest->start_addr[0] + addr_offset_dest;

        if((0 != (block_start_pix_addr_src_array  & 0x00000001)) ||
           (0 != (block_start_pix_addr_dest_array & 0x00000001)))
        {
            ret = GDT_ERROR_BLK_CFG;    
            return ret;        
        }

        v_gdt_blk_cut_comm_16x16(blk_conv_info, img_src, img_dest); 
    }
    
    
    
    

    /* GDT register cfg  */
    v_gdt_rotate_cfg(gdt_rotate_para_cfg);

    v_gdt_seq_gen_rotate(gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Rotate */

/********************************************************************//**
* @brief The function scroll image. This function should be called when scroll the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_scroll_para_cfg     GDT scroll parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     successful operation                            
* @retval GDT_ERROR                  unspecified error                               
* @retval GDT_ERROR_IMG_CFG          image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    block over bound error                          
* @retval GDT_ERROR_BLK_CFG          source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Scroll */
e_gdt_err_t R_GDT_Scroll(st_img_in_info_t             * img_src, 
                         st_img_out_info_t            * img_dest,
                         st_blk_conv_info_t           * blk_conv_info,
                         st_gdt_scroll_para_cfg_t     * gdt_scroll_para_cfg,
                         gdt_cb_event_t const         p_callback )
{
    uint32_t gdtdsz; 
    uint8_t src_img_num;
    uint8_t dest_img_num;

    e_gdt_err_t ret = GDT_OK;
    gs_int_callback = p_callback;
    
    gdtdsz = 0;              /* gdt data size sel scroll fix to 0 */
    gs_gdt_unit_size = 0x10; /* GDT process unit size 16*16bit */

    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 

    if(0 == gdt_scroll_para_cfg->iscren) /* check scroll config */
    {
        ret = GDT_ERROR_MOD_CFG;
        return ret;
    }

    /* GDT check image number */
    ret = e_gdt_check_img_num(GDT_SCROLL, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_SCROLL);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_iscr_blkend_ob(img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src ,img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* setting GDT output interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    v_gdt_blk_cut_scroll_8x16(blk_conv_info, img_src, img_dest); /* unit size (8*16)*2 bit */

    /* GDT register cfg  */
    v_gdt_scroll_cfg(gdt_scroll_para_cfg);

    v_gdt_seq_gen_scroll(gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Scroll */

/********************************************************************//**
* @brief The function fount image. This function should be called when scroll the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_fount_para_cfg      GDT fount parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     successful operation                            
* @retval GDT_ERROR                  unspecified error                               
* @retval GDT_ERROR_IMG_CFG          image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    block over bound error                          
* @retval GDT_ERROR_BLK_CFG          source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Fount */
e_gdt_err_t R_GDT_Fount(st_img_in_info_t             * img_src, 
                        st_img_out_info_t            * img_dest,
                        st_blk_conv_info_t           * blk_conv_info,
                        st_gdt_fount_para_cfg_t      * gdt_fount_para_cfg,
                        gdt_cb_event_t const         p_callback )
{
    uint8_t src_img_num;
    uint8_t dest_img_num;
    uint8_t fdir;
    uint8_t sac;
    uint8_t fdltdsz;
    uint8_t fdlngsz;
    uint16_t fount_data_size;

    uint8_t dmac_in_block_num;    /* dmac input block number  */                           
    uint8_t dmac_in_data_length;  /* dmac input data length   */                           
    uint8_t dmac_out_block_num;   /* dmac output block number */                           
    uint8_t dmac_out_data_length; /* dmac output data length  */                           
    uint32_t valid_pix;
    uint32_t dest_block_size_h;

    e_gdt_err_t ret = GDT_OK;
    gs_int_callback = p_callback;
    src_img_num     = img_src->img_num; 
    dest_img_num    = img_dest->img_num; 
    
    sac     = gdt_fount_para_cfg->sac; 
    fdltdsz = gdt_fount_para_cfg->fdltdsz; 
    fdlngsz = gdt_fount_para_cfg->fdlngsz; 

    /* calculation block h/v size */
    valid_pix = sac + fdltdsz;
    dest_block_size_h = ((valid_pix >> 3) << 3);
    if(0 != (valid_pix & 0x07))                    /* (sac + fdltdsz) % 8 */
    {
        dest_block_size_h = dest_block_size_h + 8; /* unit is bit */
    }
    blk_conv_info->src_size_h = dest_block_size_h; 
    blk_conv_info->src_size_v = fdlngsz; 

    /* GDT check image number */
    ret = e_gdt_check_img_num(GDT_FOUNT, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the fount configure parameter */
    ret = e_gdt_judge_cfg_fount(gdt_fount_para_cfg); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_fount(img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    /* calculation DMAC input times*/
    fount_data_size = fdltdsz * fdlngsz;
    fdir = fount_data_size >> 6;      /* fount_data_size/64bit */
    if(0 != (fount_data_size & 0x3f)) /* fount_data_size % 64bit */
    {
        fdir = fdir + 1;
    }
    dmac_in_block_num    = fdir;    /* dmac input loop number  */                           
    dmac_in_data_length  = 8;       /* dmac input data length   */                           

    /* calculation dmac out data length */
    dmac_out_data_length = dest_block_size_h >> 3;
    dmac_out_block_num   = fdlngsz; /* dmac output block number */                           

    /* setting GDT fount interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    /* eventlink initial */
    /* enable GDT output interrupt trigger CPU  */
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, GDT_OUT_IELS_VALUE, v_isr_cpu_output_fount);  

    /* GDT register cfg  */
    v_gdt_fount_cfg(gdt_fount_para_cfg,fdir);

    v_gdt_seq_gen_fount(img_src, img_dest, blk_conv_info,                    
                        dmac_in_block_num, dmac_in_data_length, 
                        dmac_out_block_num, dmac_out_data_length, 
                        p_callback );

    return ret;
} /* End of Function R_GDT_Fount */

/********************************************************************//**
* @brief The function reverse image. This function should be called when reverse the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_nochange_para_cfg   GDT nochange parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     successful operation                            
* @retval GDT_ERROR                  unspecified error                               
* @retval GDT_ERROR_IMG_CFG          image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    block over bound error                          
* @retval GDT_ERROR_BLK_CFG          source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Nochange */
e_gdt_err_t R_GDT_Nochange(st_img_in_info_t           * img_src, 
                           st_img_out_info_t          * img_dest,
                           st_blk_conv_info_t         * blk_conv_info,
                           st_gdt_nochange_para_cfg_t * gdt_nochange_para_cfg,
                           gdt_cb_event_t const       p_callback )
{
    e_gdt_err_t ret;
    st_gdt_endian_para_cfg_t gdt_mod_para_cfg;

    gdt_mod_para_cfg.endian  = 0;                              /* GDT_CFG_ENDIAN_EN */
    gdt_mod_para_cfg.iflp_en = gdt_nochange_para_cfg->iflp_en; /* GDT_CFG_IFLP_EN   */
	
    ret = R_GDT_Endian(img_src,img_dest,blk_conv_info,&gdt_mod_para_cfg,p_callback);

    return ret;
} /* End of Function R_GDT_Nochange */

/********************************************************************//**
* @brief The function coloralign image. This function should be called when coloralign the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_coloralign_para_cfg GDT coloralign parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     successful operation                            
* @retval GDT_ERROR                  unspecified error                               
* @retval GDT_ERROR_IMG_CFG          image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    block over bound error                          
* @retval GDT_ERROR_BLK_CFG          source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Coloralign */
e_gdt_err_t R_GDT_Coloralign(st_img_in_info_t             * img_src, 
                             st_img_out_info_t            * img_dest,
                             st_blk_conv_info_t           * blk_conv_info,
                             st_gdt_coloralign_para_cfg_t * gdt_coloralign_para_cfg,
                             gdt_cb_event_t const         p_callback )
{
 
    uint32_t gdtdsz; 
    uint8_t i; 
    uint8_t src_img_num;
    uint8_t dest_img_num;
    st_blk_conv_info_t    rebuild_blk_conv_info;
    e_gdt_err_t ret = GDT_OK;
    gs_int_callback = p_callback;

    /* check color mode is on */
    if(1 != GDT_CFG_COLOR_ON)
    {
        ret = GDT_ERROR;
    }
    if(GDT_OK != ret)
    {
        return ret;
    }

    gdtdsz           = 0;       /* gdt data size sel 0:16*16 1:8*8 */
    gs_gdt_unit_size = 0x10;    /* GDT process unit size 16*16bit */ 
    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 

    if(0 == gdt_coloralign_para_cfg->cialgsl) /* 3bit */
    {
        gs_special_reg = 3;
    }
    else /* 4bit */
    {
        gs_special_reg = 4;
    }

    rebuild_blk_conv_info.src_size_h = blk_conv_info->src_size_h * gs_special_reg;
    rebuild_blk_conv_info.src_size_v = blk_conv_info->src_size_v;

    for(i=0;i<src_img_num;i++)
    {        
        rebuild_blk_conv_info.start_pix_h_src[i]  = blk_conv_info->start_pix_h_src[i];          
        rebuild_blk_conv_info.start_pix_v_src[i]  = blk_conv_info->start_pix_v_src[i];          
    }
    for(i=0;i<dest_img_num;i++)
    {        
        rebuild_blk_conv_info.start_pix_h_dest[i] = blk_conv_info->start_pix_h_dest[i];        
        rebuild_blk_conv_info.start_pix_v_dest[i] = blk_conv_info->start_pix_v_dest[i];        
    }

    /* GDT check image number '0' mean no used */
    ret = e_gdt_check_img_num(GDT_COLORALIGN, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* IMG memory size setting judge*/
    ret = e_gdt_judge_cial_dest_mem_size(img_dest,gs_special_reg);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_COLORALIGN);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_comm_blkend_ob(img_src,img_dest,&rebuild_blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src ,img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* setting GDT output interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    /* cut block unit(16*16) */
    v_gdt_blk_cut_clralign_16x16(blk_conv_info, img_src, img_dest); 

    v_gdt_coloralign_cfg(gdt_coloralign_para_cfg);
    v_gdt_seq_gen_coloralign(gs_special_reg, gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Coloralign */

/********************************************************************//**
* @brief The function Color image. This function should be called when monochrome the image.
* @param[in] img_src                 GDT source image information           
* @param[in] img_dest                GDT destination image information          
* @param[in] blk_conv_info           GDT block information
* @param[in] gdt_colorsyn_para_cfg   GDT colorsyn parameter configuration
* @param[in,out] p_callback          API call back function
* @retval GDT_OK                     successful operation                            
* @retval GDT_ERROR                  unspecified error                               
* @retval GDT_ERROR_IMG_CFG          image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND    block over bound error                          
* @retval GDT_ERROR_BLK_CFG          source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG          GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM          GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Colorsyn */
e_gdt_err_t R_GDT_Colorsyn(st_img_in_info_t             * img_src, 
                           st_img_out_info_t            * img_dest,
                           st_blk_conv_info_t           * blk_conv_info,
                           st_gdt_colorsyn_para_cfg_t   * gdt_colorsyn_para_cfg,
                           gdt_cb_event_t const         p_callback )
{
    uint32_t gdtdsz; 
    uint8_t src_img_num;
    uint8_t dest_img_num;
    e_gdt_err_t ret = GDT_OK;
    gs_int_callback = p_callback;
    
    /* check color mode is on */
    if(1 != GDT_CFG_COLOR_ON)
    {
        ret = GDT_ERROR;
    }
    if(GDT_OK != ret)
    {
        return ret;
    }

    gdtdsz = gdt_colorsyn_para_cfg->gdtdsz; /* gdt data size sel */

    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 

    /* GDT check image number */
    ret = e_gdt_check_img_num(GDT_COLORSYN, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* GDT process unit size sel */
    if( 1 == gdtdsz ) /* 8*8bit */
    {        
        gs_gdt_unit_size = 0x8; 
    }
    else /* 16*16bit */ 
    {        
        gs_gdt_unit_size = 0x10; 
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_COLORSYN);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_comm_blkend_ob(img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src ,img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* setting GDT output interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    if( 8 == gs_gdt_unit_size )
    {
        v_gdt_blk_cut_comm_8x8(blk_conv_info, img_src, img_dest); 
    }
    else if( 16 == gs_gdt_unit_size )
    {
        v_gdt_blk_cut_comm_16x16(blk_conv_info, img_src, img_dest); 
    }
    else
    {
        __NOP();
    }

    /* GDT register cfg  */
    v_gdt_colorsyn_cfg(gdt_colorsyn_para_cfg);

    v_gdt_seq_gen_common(gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Colorsyn */

/********************************************************************//**
* @brief The function Color synthesis image. This function should be called when monochrome the image.
* @param[in] img_src               GDT source image information           
* @param[in] img_dest              GDT destination image information          
* @param[in] blk_conv_info         GDT block information
* @param[in] gdt_color_para_cfg    GDT color parameter configuration
* @param[in,out] p_callback        API call back function
* @retval GDT_OK                   successful operation                            
* @retval GDT_ERROR                unspecified error                               
* @retval GDT_ERROR_IMG_CFG        image configuration error                       
* @retval GDT_ERROR_BLK_OFF_BOUND  block over bound error                          
* @retval GDT_ERROR_BLK_CFG        source or destination block configuration error 
* @retval GDT_ERROR_MOD_CFG        GDT mode configuration error                    
* @retval GDT_ERROR_IMG_NUM        GDT image number configuration error            
***********************************************************************/
/* Function Name : R_GDT_Color */
e_gdt_err_t R_GDT_Color(st_img_in_info_t            * img_src, 
                           st_img_out_info_t        * img_dest,
                           st_blk_conv_info_t       * blk_conv_info,
                           st_gdt_color_para_cfg_t  * gdt_color_para_cfg,
                           gdt_cb_event_t const     p_callback )
{
    uint32_t gdtdsz; 
    uint8_t src_img_num;
    uint8_t dest_img_num;
    e_gdt_err_t ret = GDT_OK;
    gs_int_callback = p_callback;
    
    /* check color mode is on */
    if(1 != GDT_CFG_COLOR_ON)
    {
        ret = GDT_ERROR;
    }
    if(GDT_OK != ret)
    {
        return ret;
    }

    gdtdsz = 0;              /* gdt data size sel 0:16*16 1:8*8 */
    gs_gdt_unit_size = 0x10; /* GDT process unit size 16*16bit */ 

    src_img_num  = img_src->img_num; 
    dest_img_num = img_dest->img_num; 

    /* GDT check image number */
    ret = e_gdt_check_img_num(GDT_MONO_TO_COLOR, 0, src_img_num, dest_img_num);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* IMG setting judge*/
    ret = e_gdt_judge_img_cfg(img_src,img_dest,blk_conv_info,gdtdsz,GDT_MONO_TO_COLOR);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* judge the end of the block off boundary*/
    ret = e_gdt_judge_dest_comm_blkend_ob(img_src,img_dest,blk_conv_info); 
    if(GDT_OK != ret)
    {
        return ret;
    }

    v_gdt_clear_reg(); /* clear gdt register */

    v_gdt_basic_info_calc(img_src ,img_dest);

    /* setting GDT input interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII,gs_gdt_resources.gdt_in_int_priority);

    /* setting GDT output interrupt priority  */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI,gs_gdt_resources.gdt_out_int_priority);

    v_gdt_blk_cut_comm_16x16(blk_conv_info, img_src, img_dest); 

    /* GDT register cfg  */
    v_gdt_color_cfg(gdt_color_para_cfg);

    v_gdt_seq_gen_common(gs_int_callback); 

    return ret;
} /* End of Function R_GDT_Color */

/******************************************************************************************************************//**
 * common internal function
 *********************************************************************************************************************/

/* Function Name : v_gdt_on */
/********************************************************************//**
* @brief Ready to start GDT
***********************************************************************/
static void v_gdt_on(void)
{
    /* enable GDT start */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTSTART = VAL_ENABLE;
} /* End of function v_gdt_on */

/********************************************************************//**
* @brief closed GDT function
***********************************************************************/
/* Function Name : v_gdt_off */
static void v_gdt_off(void)    
{
    /* enable GDT Stop */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTSTART = VAL_DISABLE;
} /* End of function v_gdt_off */

/* Function Name : v_gdt_clear_reg */
/********************************************************************//**
* @brief clear gdt registers
***********************************************************************/
static void v_gdt_clear_reg(void)
{
    /* GDTCR */    
    gs_gdt_resources.reg_gdt->GDTCR   = 0x00000000;

    /* GDTSCR */    
    gs_gdt_resources.reg_gdt->GDTSCR  = 0x07000000;

    /* GDTFDCS */    
    gs_gdt_resources.reg_gdt->GDTFDCS = 0x00000100;

    /* GDTPIER */    
    gs_gdt_resources.reg_gdt->GDTPIER = 0x00000000;

} /* End of Function v_gdt_clear_reg */

/********************************************************************//**
* @brief image number judege for GDT function
* @param[in] gdt_function         GDT mode selection
* @param[in] special_reg          Registers that affect the number of inputs in special mode 
* @param[in] src_img_num          Source image number
* @param[in] dest_img_num         Destination image number
* @retval GDT_OK                  Successful operation                            
* @retval GDT_ERROR_IMG_NUM       GDT image number configuration error 
***********************************************************************/
/* Function Name : e_gdt_check_img_num */
static e_gdt_err_t e_gdt_check_img_num(uint8_t gdt_function, uint8_t special_reg, uint8_t src_img_num, uint8_t dest_img_num)
{
    e_gdt_err_t ret = GDT_OK;

    switch(gdt_function)
    {
        case GDT_MONOCHROME:
            {
                if( 0 == special_reg )  /* border disable */
                {
                    if((2 == src_img_num) && (1 == dest_img_num))  /* src img judge */
                    {
                        ret = GDT_OK;
                    }
                    else
                    {
                        ret =  GDT_ERROR_IMG_NUM;
                    }
                }
                if( 1 == special_reg )  /* border enable */
                {
                    if((3 == src_img_num) && (1 == dest_img_num))  /* src img judge */
                    {
                        ret =  GDT_OK;
                    }
                    else
                    {
                        ret =  GDT_ERROR_IMG_NUM;
                    }
                }
                return ret;
            }
        break;
        case GDT_MONO_TO_COLOR:
            {
                if((1 == src_img_num) && (3 == dest_img_num))  /* src img judge */
                {
                    ret =  GDT_OK;
                }
                else
                {
                    ret =  GDT_ERROR_IMG_NUM;
                }
                return ret;
            }
        break;
        case GDT_COLORSYN:
            {
                if((6 == src_img_num) && (3 == dest_img_num))  /* src img judge */
                {
                    ret =  GDT_OK;
                }
                else
                {
                    ret =  GDT_ERROR_IMG_NUM;
                }
                return ret;
            }
        break;
        case GDT_COLORALIGN:
            {
                if((3 == src_img_num) && (1 == dest_img_num))  /* src img judge */
                {
                    ret =  GDT_OK;
                }
                else
                {
                    ret =  GDT_ERROR_IMG_NUM;
                }
                return ret;
            }
        break;
        default :
            {
                if((1 == src_img_num) && (1 == dest_img_num))  /* src img judge */
                {
                    ret =  GDT_OK;
                }
                else
                {
                    ret =  GDT_ERROR_IMG_NUM;
                }
                return ret;

            }
    }
} /* End of function e_gdt_check_img_num */

/********************************************************************//**
* @brief Determining a dmac channel configuration is correct
* @param[in] trans_mod_cfg        GDT transfer mode configuration
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_DMAC_CH_CFG   DMAC channel select error                       
***********************************************************************/
/* Function Name : e_gdt_check_dmac_cfg */
static e_gdt_err_t e_gdt_check_dmac_cfg(st_trans_mod_cfg_t* trans_mod_cfg)
{
    uint8_t  ch_in;
    uint8_t  ch_out;
    uint32_t dmac_in_priority;
    uint32_t dmac_out_priority;
    ch_in  = trans_mod_cfg->ch_in;
    ch_out = trans_mod_cfg->ch_out;
    
    switch(ch_in)
    {
        case 0 :
            {    
            dmac_in_priority = gs_gdt_resources.dmac0_int_priority;
            }
        break;
        case 1:
            {   
            dmac_in_priority = gs_gdt_resources.dmac1_int_priority;
            }
        break;
        case 2:
            {    
            dmac_in_priority = gs_gdt_resources.dmac2_int_priority;
            }
        break;
        case 3:
            {    
            dmac_in_priority = gs_gdt_resources.dmac3_int_priority;
            }
        break;
        default : 
            {
            dmac_in_priority = gs_gdt_resources.dmac0_int_priority;
            }
        break;
    }

    switch(ch_out)
    {
        case 0 :
            {    
            dmac_out_priority = gs_gdt_resources.dmac0_int_priority;
            }    
        break;   
        case 1:  
            {    
            dmac_out_priority = gs_gdt_resources.dmac1_int_priority;
            }    
        break;   
        case 2:  
            {    
            dmac_out_priority = gs_gdt_resources.dmac2_int_priority;
            }    
        break;   
        case 3:  
            {    
            dmac_out_priority = gs_gdt_resources.dmac3_int_priority;
            }    
        break;   
        default:
            {    
            dmac_out_priority = gs_gdt_resources.dmac0_int_priority;
            }
        break;
    }

    /* dmac input channel check */
    if((ch_in > 3))
    {
        return GDT_ERROR_DMAC_CH_CFG;  
    }

    /* dmac output channel check */
    if((ch_out > 3))
    {
        return GDT_ERROR_DMAC_CH_CFG;  
    }

    /* DMAC channel repeat setting check */
    if(ch_in ==  ch_out)
    {
        return GDT_ERROR_DMAC_CH_CFG;  
    }

    if((dmac_in_priority > dmac_out_priority)) /* small value is highest priority */
    {
        return GDT_ERROR_DMAC_CH_CFG;  
    }

    if((dmac_in_priority == dmac_out_priority) && (ch_in > ch_out)) /* dmac channel number check */
    {
        return GDT_ERROR_DMAC_CH_CFG;  
    }

    return GDT_OK;
} /* End of Function e_gdt_check_dmac_cfg */

/* Function Name : e_gdt_judge_img_cfg */
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_src_t         GDT source image information           
* @param[in] st_img_dest_t        GDT destination image information          
* @param[in] st_blk_conv_info_t   GDT block information
* @param[in] gdtdsz               GDT transfer unit size selection
* @param[in] gdt_function         GDT mode selection
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_IMG_CFG       image configuration error                       
* @retval GDT_ERROR_BLK_CFG       source or destination block configuration error 
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
***********************************************************************/
static e_gdt_err_t e_gdt_judge_img_cfg(st_img_in_info_t* st_img_src_t,
                                       st_img_out_info_t* st_img_dest_t,
                                       st_blk_conv_info_t* st_blk_conv_info_t,
                                       uint32_t gdtdsz, uint8_t gdt_function)
{
    e_gdt_err_t ret = GDT_OK;

    /* source image setting judge*/
    ret = e_gdt_judge_img_src_cfg(st_img_src_t,st_blk_conv_info_t,gdtdsz,gdt_function);
    if(GDT_OK != ret)
    {
         return ret;
    }

    /* destnation image setting judge*/
    ret = e_gdt_judge_img_dest_cfg(st_img_dest_t,st_blk_conv_info_t,gdtdsz,gdt_function);
    if(GDT_OK != ret)
    {
         return ret;
    }

    return ret;
} /* End of Function e_gdt_judge_img_cfg */

/* Function Name : e_gdt_judge_img_src_cfg */
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_src_t         GDT source image information           
* @param[in] st_blk_conv_info_t   GDT block information
* @param[in] gdtdsz               GDT transfer unit size selection
* @param[in] gdt_function         GDT mode selection
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_IMG_CFG       image configuration error                       
* @retval GDT_ERROR_BLK_CFG       source or destination block configuration error 
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
***********************************************************************/
static e_gdt_err_t e_gdt_judge_img_src_cfg(st_img_in_info_t* st_img_src_t,
                                           st_blk_conv_info_t* st_blk_conv_info_t,
                                           uint32_t gdtdsz, uint8_t gdt_function)
{
    uint8_t i;
    for(i=0;i<st_img_src_t->img_num;i++)
    {    
        /* memory size check */
        if(!((32  == st_img_src_t->mem_size_h[i]) ||
             (64  == st_img_src_t->mem_size_h[i]) ||
             (128 == st_img_src_t->mem_size_h[i]) ||
             (256 == st_img_src_t->mem_size_h[i])
            ))
        {
            return GDT_ERROR_IMG_CFG;
        }
            
        /* Is the input/output image size larger than the maximum memory size */
        if(st_img_src_t->size_h[i] > st_img_src_t->mem_size_h[i])
        {
            return GDT_ERROR_IMG_CFG;             /* image configuration error */
        }
    
        /* Judge the input/output image size not 8 times value */
        if((0 != (st_img_src_t->size_h[i] & 0x00000007)) || ( 8 > st_img_src_t->size_h[i] ))
        {
            return GDT_ERROR_IMG_CFG;             /* image configuration error */
        }
        if((0 != (st_img_src_t->size_v[i] & 0x00000007)) || ( 8 > st_img_src_t->size_v[i] ))
        {
            return GDT_ERROR_IMG_CFG;
        }
    
        /* block infomation check */
        if(0 != (st_blk_conv_info_t->start_pix_h_src[i] & 0x00000007))  /* The block start point of horizontal is not 8 times value */ 
        {
            return GDT_ERROR_BLK_CFG;
        }
        if((st_blk_conv_info_t->start_pix_h_src[i]) > (st_img_src_t->size_h[i])) /* The block start point is off boundary */ 
        {
            return GDT_ERROR_BLK_OFF_BOUND;                   /* image configuration error */
        }
        if((st_blk_conv_info_t->start_pix_v_src[i]) > (st_img_src_t->size_v[i]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
        if((st_blk_conv_info_t->start_pix_h_src[i] + st_blk_conv_info_t->src_size_h) > (st_img_src_t->size_h[i])) /* The block end point is off boundary */ 
        {
            return GDT_ERROR_BLK_OFF_BOUND;                   /* block over bound error */
        }
        if((st_blk_conv_info_t->start_pix_v_src[i] + st_blk_conv_info_t->src_size_v) > (st_img_src_t->size_v[i]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
    }

    /* The block size is not 8 times value */
    if((0 != (st_blk_conv_info_t->src_size_h & 0x00000007)) || (8 > st_blk_conv_info_t->src_size_h ))
    {
        return GDT_ERROR_BLK_CFG;               /* source or destination block size error */
    }
    if((0 != (st_blk_conv_info_t->src_size_v & 0x00000007)) || (8 > st_blk_conv_info_t->src_size_v ))
    {
        return GDT_ERROR_BLK_CFG;
    }

    if(0 == gdtdsz) /*gdt data size 16*16bit */
    {
        /* The block size is not 16 times value */
        if((0 != (st_blk_conv_info_t->src_size_h & 0x0000000f)) || (16 > st_blk_conv_info_t->src_size_h ))
        {
            return GDT_ERROR_BLK_CFG;           /* source or destination block size error */
        }
        if((0 != (st_blk_conv_info_t->src_size_v & 0x0000000f)) || (16 > st_blk_conv_info_t->src_size_v ))
        {
            return GDT_ERROR_BLK_CFG;
        }
    }

    return GDT_OK;
} /* End of Function e_gdt_judge_img_src_cfg */

/* Function Name : e_gdt_judge_img_dest_cfg */
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_dest_t        GDT destination image information          
* @param[in] st_blk_conv_info_t   GDT block information
* @param[in] gdtdsz               GDT transfer unit size selection
* @param[in] gdt_function         GDT mode selection
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_IMG_CFG       image configuration error                       
* @retval GDT_ERROR_BLK_CFG       source or destination block configuration error 
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
***********************************************************************/
static e_gdt_err_t e_gdt_judge_img_dest_cfg(st_img_out_info_t* st_img_dest_t,
                                            st_blk_conv_info_t* st_blk_conv_info_t,
                                            uint32_t gdtdsz, uint8_t gdt_function)
{
    uint8_t i;
    for(i=0;i<st_img_dest_t->img_num;i++)
    {    
        /* memory size check except function coloralign */
        if( GDT_COLORALIGN != gdt_function)
        {
            if(!((32  == st_img_dest_t->mem_size_h[i]) ||
                 (64  == st_img_dest_t->mem_size_h[i]) ||
                 (128 == st_img_dest_t->mem_size_h[i]) ||
                 (256 == st_img_dest_t->mem_size_h[i])  
                ))
            {
                return GDT_ERROR_IMG_CFG;
            }
        }

        /* Is the input/output image size larger than the maximum memory size */
        if(st_img_dest_t->size_h[i] > st_img_dest_t->mem_size_h[i])
        {
            return GDT_ERROR_IMG_CFG;
        }
    
        /* Judge the input/output image size not 8 times value */
        if((0 != (st_img_dest_t->size_h[i] & 0x00000007)) || ( 8 > st_img_dest_t->size_h[i] ))
        {
            return GDT_ERROR_IMG_CFG;
        }
        if((0 != (st_img_dest_t->size_v[i] & 0x00000007)) || ( 8 > st_img_dest_t->size_v[i] ))
        {
            return GDT_ERROR_IMG_CFG;
        }
    
        /* block infomation check */
        if(0 != (st_blk_conv_info_t->start_pix_h_dest[i] & 0x00000007)) /* The block start point of horizontal is not 8 times value */
        {
            return GDT_ERROR_BLK_CFG;
        }
        if((st_blk_conv_info_t->start_pix_h_dest[i]) > (st_img_dest_t->size_h[i])) /* The block start point is off boundary */ 
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
        if((st_blk_conv_info_t->start_pix_v_dest[i]) > (st_img_dest_t->size_v[i]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
    }
    return GDT_OK;
} /* End of Function e_gdt_judge_img_dest_cfg */

/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_src_t         GDT source image information           
* @param[in] st_img_dest_t        GDT destination image information          
* @param[in] st_blk_conv_info_t   GDT block information
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
***********************************************************************/
/* Function Name : e_gdt_judge_dest_comm_blkend_ob */
static e_gdt_err_t e_gdt_judge_dest_comm_blkend_ob(st_img_in_info_t* st_img_src_t, 
                                                   st_img_out_info_t* st_img_dest_t,
                                                   st_blk_conv_info_t* st_blk_conv_info_t)
{
    uint8_t i;    
    for(i=0;i<st_img_dest_t->img_num;i++)
    {    
        if((st_blk_conv_info_t->start_pix_h_dest[i] + st_blk_conv_info_t->src_size_h) > (st_img_dest_t->size_h[i]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
        if((st_blk_conv_info_t->start_pix_v_dest[i] + st_blk_conv_info_t->src_size_v) > (st_img_dest_t->size_v[i]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
    }
    return GDT_OK;
} /* End of Function e_gdt_judge_dest_comm_blkend_ob */

/* Function Name : v_gdt_basic_info_calc */
/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range and remain range information.
* @param[in] img_src   GDT source image information           
* @param[in] img_dest  GDT destination image information          
***********************************************************************/
static void v_gdt_basic_info_calc(st_img_in_info_t  * img_src ,
                                  st_img_out_info_t * img_dest) 
{
    uint8_t i;

    /* Assign parameters to global variables */
    gdt_img_src_t.img_num     = img_src->img_num;

    for(i=0;i<gdt_img_src_t.img_num;i++)
    {        
        gdt_img_src_t.start_addr[i]  = img_src->start_addr[i]; 
        gdt_img_src_t.size_h[i]      = img_src->size_h[i];
        gdt_img_src_t.size_v[i]      = img_src->size_v[i];
        gdt_img_src_t.size_h_byte[i] = (img_src->size_h[i] >> 3);
        gs_src_mem_size_h[i]         = img_src->mem_size_h[i] >> 3;
    }

    /* Assign parameters to global variables */
    gdt_img_dest_t.img_num     = img_dest->img_num;
    for(i=0;i<gdt_img_dest_t.img_num;i++)
    {        
        gdt_img_dest_t.start_addr[i]  = img_dest->start_addr[i]; 
        gdt_img_dest_t.size_h[i]      = img_dest->size_h[i];
        gdt_img_dest_t.size_v[i]      = img_dest->size_v[i];
        gdt_img_dest_t.size_h_byte[i] = (img_dest->size_h[i] >> 3);
        gs_dest_mem_size_h[i]         = img_dest->mem_size_h[i] >> 3;
    }

} /* End of Function v_gdt_basic_info_calc */

/********************************************************************//**
* @brief This function is to select the DMAC channel to transmit input data.
* @param[in] trans_mod_cfg   GDT transfer mode configuration
* @retval GDT_ERROR_DMAC_CH_CFG   DMAC channel select error                       
***********************************************************************/
/* Function Name : e_gdt_dmac_in_channel_sel */
static e_gdt_err_t e_gdt_dmac_in_channel_sel(st_trans_mod_cfg_t * trans_mod_cfg)
{
    e_gdt_err_t ret = GDT_OK;

    /* input DMAC channel select*/
    switch(trans_mod_cfg->ch_in)
    {
        case DMA_CH0 :
            {    
            gsp_dmacdev_in  = &Driver_DMAC0;
            gsp_dmac_reg_in_sar = &gs_gdt_resources.reg_dmac0->DMSAR;
            gsp_dmac_reg_in_dar = &gs_gdt_resources.reg_dmac0->DMDAR;
            gsp_dmac_reg_in_cnt = &gs_gdt_resources.reg_dmac0->DMCNT;
            gsp_dmac_reg_in_crb = &gs_gdt_resources.reg_dmac0->DMCRB;
            gsp_dmac_reg_in_ofr = &gs_gdt_resources.reg_dmac0->DMOFR;
            }
        break;
        case DMA_CH1 :
            {    
            gsp_dmacdev_in  = &Driver_DMAC1;
            gsp_dmac_reg_in_sar = &gs_gdt_resources.reg_dmac1->DMSAR;
            gsp_dmac_reg_in_dar = &gs_gdt_resources.reg_dmac1->DMDAR;
            gsp_dmac_reg_in_cnt = &gs_gdt_resources.reg_dmac1->DMCNT;
            gsp_dmac_reg_in_crb = &gs_gdt_resources.reg_dmac1->DMCRB;
            gsp_dmac_reg_in_ofr = &gs_gdt_resources.reg_dmac1->DMOFR;
            }
        break;
        case DMA_CH2 :
            {    
            gsp_dmacdev_in  = &Driver_DMAC2;
            gsp_dmac_reg_in_sar = &gs_gdt_resources.reg_dmac2->DMSAR;
            gsp_dmac_reg_in_dar = &gs_gdt_resources.reg_dmac2->DMDAR;
            gsp_dmac_reg_in_cnt = &gs_gdt_resources.reg_dmac2->DMCNT;
            gsp_dmac_reg_in_crb = &gs_gdt_resources.reg_dmac2->DMCRB;
            gsp_dmac_reg_in_ofr = &gs_gdt_resources.reg_dmac2->DMOFR;
            }
        break;
        case DMA_CH3 :
            {    
            gsp_dmacdev_in  = &Driver_DMAC3;
            gsp_dmac_reg_in_sar = &gs_gdt_resources.reg_dmac3->DMSAR;
            gsp_dmac_reg_in_dar = &gs_gdt_resources.reg_dmac3->DMDAR;
            gsp_dmac_reg_in_cnt = &gs_gdt_resources.reg_dmac3->DMCNT;
            gsp_dmac_reg_in_crb = &gs_gdt_resources.reg_dmac3->DMCRB;
            gsp_dmac_reg_in_ofr = &gs_gdt_resources.reg_dmac3->DMOFR;
            }
        break;
        default:
            {    
                ret = GDT_ERROR_DMAC_CH_CFG;
            }    
        break;
    }
    return ret;
} /* End of Function e_gdt_dmac_in_channel_sel */ 

/********************************************************************//**
* @brief This function is to select the DMAC channel to transmit output data.
* @param[in] trans_mod_cfg   GDT transfer mode configuration
* @retval GDT_ERROR_DMAC_CH_CFG   DMAC channel select error                       
***********************************************************************/
/* Function Name : e_gdt_dmac_out_channel_sel */
static e_gdt_err_t e_gdt_dmac_out_channel_sel(st_trans_mod_cfg_t * trans_mod_cfg)
{
    e_gdt_err_t ret = GDT_OK;

    /* output DMAC channel select*/
    switch(trans_mod_cfg->ch_out)
    {
        case DMA_CH0 :
            {    
            gsp_dmacdev_out  = &Driver_DMAC0;
            gsp_dmac_reg_out_sar = &gs_gdt_resources.reg_dmac0->DMSAR;
            gsp_dmac_reg_out_dar = &gs_gdt_resources.reg_dmac0->DMDAR;
            gsp_dmac_reg_out_cnt = &gs_gdt_resources.reg_dmac0->DMCNT;
            gsp_dmac_reg_out_crb = &gs_gdt_resources.reg_dmac0->DMCRB;
            gsp_dmac_reg_out_ofr = &gs_gdt_resources.reg_dmac0->DMOFR;
            }
        break;
        case DMA_CH1 :
            {    
            gsp_dmacdev_out  = &Driver_DMAC1;
            gsp_dmac_reg_out_sar = &gs_gdt_resources.reg_dmac1->DMSAR;
            gsp_dmac_reg_out_dar = &gs_gdt_resources.reg_dmac1->DMDAR;
            gsp_dmac_reg_out_cnt = &gs_gdt_resources.reg_dmac1->DMCNT;
            gsp_dmac_reg_out_crb = &gs_gdt_resources.reg_dmac1->DMCRB;
            gsp_dmac_reg_out_ofr = &gs_gdt_resources.reg_dmac1->DMOFR;
            }
        break;
        case DMA_CH2 :
            {    
            gsp_dmacdev_out  = &Driver_DMAC2;
            gsp_dmac_reg_out_sar = &gs_gdt_resources.reg_dmac2->DMSAR;
            gsp_dmac_reg_out_dar = &gs_gdt_resources.reg_dmac2->DMDAR;
            gsp_dmac_reg_out_cnt = &gs_gdt_resources.reg_dmac2->DMCNT;
            gsp_dmac_reg_out_crb = &gs_gdt_resources.reg_dmac2->DMCRB;
            gsp_dmac_reg_out_ofr = &gs_gdt_resources.reg_dmac2->DMOFR;
            }
        break;
        case DMA_CH3 :
            {    
            gsp_dmacdev_out  = &Driver_DMAC3;
            gsp_dmac_reg_out_sar = &gs_gdt_resources.reg_dmac3->DMSAR;
            gsp_dmac_reg_out_dar = &gs_gdt_resources.reg_dmac3->DMDAR;
            gsp_dmac_reg_out_cnt = &gs_gdt_resources.reg_dmac3->DMCNT;
            gsp_dmac_reg_out_crb = &gs_gdt_resources.reg_dmac3->DMCRB;
            gsp_dmac_reg_out_ofr = &gs_gdt_resources.reg_dmac3->DMOFR;
            }
        break;
        default:
            {    
                ret = GDT_ERROR_DMAC_CH_CFG;
            }    
        break;
    }
    return ret;
} /* End of Function e_gdt_dmac_out_channel_sel */ 

/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range    
* @param[in] blk       GDT block information
* @param[in] img_src   GDT source image information           
* @param[in] img_dest  GDT destination image information                 
***********************************************************************/
/* Function Name : v_gdt_blk_cut_comm_8x8 */
static void v_gdt_blk_cut_comm_8x8(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest)
{
    uint8_t  i;    
    uint16_t fst_pix_h_src[IMG_IN_FRAME_NUM];    
    uint16_t fst_pix_v_src[IMG_IN_FRAME_NUM];
    uint16_t fst_pix_h_dest[IMG_OUT_FRAME_NUM];    
    uint16_t fst_pix_v_dest[IMG_OUT_FRAME_NUM];

    uint32_t line_start_addr_src[IMG_IN_FRAME_NUM];
    uint32_t line_start_addr_dest[IMG_OUT_FRAME_NUM];
    uint32_t addr_offset_src[IMG_IN_FRAME_NUM];  
    uint32_t addr_offset_dest[IMG_OUT_FRAME_NUM];  

    uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM];
    uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM];

    uint32_t block_size_h_byte;
    uint32_t block_size_v_line;

    block_size_h_byte = blk->src_size_h >> 3;
    block_size_v_line = blk->src_size_v;

    gs_big_in_range_addr_info_t.exist     = RANGE_NOT_EXIST; /* only have big area */ 
    gs_big_out_range_addr_info_t.exist    = RANGE_NOT_EXIST; /* only have big area */ 
    gs_remain_in_range_addr_info_t.exist  = RANGE_NOT_EXIST;
    gs_remain_out_range_addr_info_t.exist = RANGE_NOT_EXIST;
    gs_big_in_range_addr_info_t.dat_unit_x_byte  = 0;
    gs_big_in_range_addr_info_t.dat_unit_num_h   = 0;
    gs_big_out_range_addr_info_t.dat_unit_x_byte = 0;
    gs_big_out_range_addr_info_t.dat_unit_num_h  = 0;

    for(i=0;i<img_src->img_num;i++)
    {        
        fst_pix_h_src[i]        = blk->start_pix_h_src[i];
        fst_pix_v_src[i]        = blk->start_pix_v_src[i];
        line_start_addr_src[i]  = fst_pix_v_src[i]  * gs_src_mem_size_h[i];
        addr_offset_src[i]      = line_start_addr_src[i]  + (fst_pix_h_src[i] >> 3);
        block_start_pix_addr_src_array[i] = img_src->start_addr[i]  + addr_offset_src[i];
        gs_in_offset_v[i]       = gs_src_mem_size_h[i]  << 3; /* the byte number of a line x 8line */
    }

    for(i=0;i<img_dest->img_num;i++)
    {        
        fst_pix_h_dest[i]       = blk->start_pix_h_dest[i];
        fst_pix_v_dest[i]       = blk->start_pix_v_dest[i];
        line_start_addr_dest[i] = fst_pix_v_dest[i] * gs_dest_mem_size_h[i];
        addr_offset_dest[i]     = line_start_addr_dest[i] + (fst_pix_h_dest[i] >> 3);
        block_start_pix_addr_dest_array[i] = img_dest->start_addr[i] + addr_offset_dest[i];
        gs_out_offset_v[i]      = gs_dest_mem_size_h[i] << 3; /* the byte number of a line x 8line */
    }

    gs_big_in_range_addr_info_t.exist     = RANGE_EXIST;
    for(i=0;i<(gdt_img_src_t.img_num);i++)
    {        
        gs_big_in_range_addr_info_t.range_fst_addr[i]   = block_start_pix_addr_src_array[i] ;
    }
    gs_big_in_range_addr_info_t.dat_unit_x_byte  = 1;
    gs_big_in_range_addr_info_t.dat_unit_y_line  = 8;
    gs_big_in_range_addr_info_t.dat_unit_num_h   = block_size_h_byte;
    gs_big_in_range_addr_info_t.dat_unit_num_v   = block_size_v_line >> 3;

    gs_big_out_range_addr_info_t.exist           = RANGE_EXIST;
    for(i=0;i<(gdt_img_dest_t.img_num);i++)
    {        
        gs_big_out_range_addr_info_t.range_fst_addr[i]  = block_start_pix_addr_dest_array[i];
    }
    gs_big_out_range_addr_info_t.dat_unit_x_byte = 1;
    gs_big_out_range_addr_info_t.dat_unit_y_line = 8;
    gs_big_out_range_addr_info_t.dat_unit_num_h  = block_size_h_byte;
    gs_big_out_range_addr_info_t.dat_unit_num_v  = block_size_v_line >> 3;
    
} /* End of Function v_gdt_blk_cut_comm_8x8 */

/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range and remain range information
* @param[in] blk       GDT block information
* @param[in] img_src   GDT source image information           
* @param[in] img_dest  GDT destination image information           
***********************************************************************/
/* Function Name : v_gdt_blk_cut_comm_16x16 */
static void v_gdt_blk_cut_comm_16x16(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest) 
{
    uint8_t  i;    
    uint16_t fst_pix_h_src[IMG_IN_FRAME_NUM];    
    uint16_t fst_pix_v_src[IMG_IN_FRAME_NUM];
    uint32_t line_start_addr_src[IMG_IN_FRAME_NUM];
    uint32_t addr_offset_src[IMG_IN_FRAME_NUM];  
    uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM];
    uint16_t fst_pix_h_dest[IMG_OUT_FRAME_NUM];    
    uint16_t fst_pix_v_dest[IMG_OUT_FRAME_NUM];
    uint32_t line_start_addr_dest[IMG_OUT_FRAME_NUM];
    uint32_t addr_offset_dest[IMG_OUT_FRAME_NUM];  
    uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM];
    uint8_t  src_check_ng_flag ;
    uint8_t  dest_check_ng_flag;
    uint8_t  src_unequal_dest_flag;
    uint32_t block_size_h_byte;
    uint32_t block_size_v_line;
    src_check_ng_flag = 0x0;       /*check flag initial*/ 
    dest_check_ng_flag= 0x0;       /*check flag initial*/ 
    src_unequal_dest_flag = 0x0;   /*check flag initial*/ 
    block_size_h_byte = blk->src_size_h >> 3;
    block_size_v_line = blk->src_size_v;
    gs_big_in_range_addr_info_t.exist     = RANGE_NOT_EXIST;
    gs_big_out_range_addr_info_t.exist    = RANGE_NOT_EXIST;
    gs_remain_in_range_addr_info_t.exist  = RANGE_NOT_EXIST;
    gs_remain_out_range_addr_info_t.exist = RANGE_NOT_EXIST;
    gs_big_in_range_addr_info_t.dat_unit_x_byte  = 0;
    gs_big_in_range_addr_info_t.dat_unit_num_h   = 0;
    gs_big_out_range_addr_info_t.dat_unit_x_byte = 0;
    gs_big_out_range_addr_info_t.dat_unit_num_h  = 0;

    for(i=0;i<img_src->img_num;i++)
    {        
        gs_in_offset_v[i]  = 0;
    }
    for(i=0;i<img_dest->img_num;i++)
    {        
        gs_out_offset_v[i] = 0;
    }
    for(i=0;i<img_src->img_num;i++)
    {        
        fst_pix_h_src[i]  = blk->start_pix_h_src[i];
        fst_pix_v_src[i]  = blk->start_pix_v_src[i];
    }
    for(i=0;i<img_dest->img_num;i++)
    {        
        fst_pix_h_dest[i] = blk->start_pix_h_dest[i];
        fst_pix_v_dest[i] = blk->start_pix_v_dest[i];
    }
    for(i=0;i<img_src->img_num;i++)
    {        
        line_start_addr_src[i]  = fst_pix_v_src[i]  * gs_src_mem_size_h[i];
        addr_offset_src[i]      = line_start_addr_src[i]  + (fst_pix_h_src[i] >> 3);
    }
    for(i=0;i<img_dest->img_num;i++)
    {        
        line_start_addr_dest[i] = fst_pix_v_dest[i] * gs_dest_mem_size_h[i];
        addr_offset_dest[i]     = line_start_addr_dest[i] + (fst_pix_h_dest[i] >> 3);
    }
    for(i=0;i<(img_src->img_num);i++)
    {        
        block_start_pix_addr_src_array[i] = img_src->start_addr[i]  + addr_offset_src[i];
    }
    for(i=0;i<(img_dest->img_num);i++)
    {        
        block_start_pix_addr_dest_array[i] = img_dest->start_addr[i] + addr_offset_dest[i];
    }

    /* add block start addr check */
    for(i=0;i<(img_src->img_num-1);i++)
    {        
        if ((block_start_pix_addr_src_array[i] & 0x00000001) != (block_start_pix_addr_src_array[i+1] & 0x00000001)) 
        {
            src_check_ng_flag = 1;
        }                
    }
    for(i=0;i<(img_dest->img_num-1);i++)
    {        
        if((block_start_pix_addr_dest_array[i] & 0x00000001) != (block_start_pix_addr_dest_array[i+1] & 0x00000001))
        {
            dest_check_ng_flag = 1;
        }                
    }
    src_unequal_dest_flag= ((block_start_pix_addr_src_array[0] & 0x00000001) != (block_start_pix_addr_dest_array[0] & 0x00000001));

    /* src and dst addr are other cases*/ 
    if (src_check_ng_flag | dest_check_ng_flag | src_unequal_dest_flag) 
    {
        v_gdt_blk_cut_comm_16x16_burst( block_start_pix_addr_src_array, block_start_pix_addr_dest_array, 
                                        block_size_h_byte, block_size_v_line);    
    }

    /* src and dst addr are both even or both odd */ 
    else 
    {
        v_gdt_blk_cut_comm_16x16_block( block_start_pix_addr_src_array, block_start_pix_addr_dest_array, 
                                        block_size_h_byte, block_size_v_line);    
    }
} /*End of Function v_gdt_blk_cut_comm_16x16 */

/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range and remain range information
* @param[in] block_start_pix_addr_src_array    source block start address
* @param[in] block_start_pix_addr_dest_array   destination block start address
* @param[in] block_size_h_byte                 The number of bytes in horizontal 
* @param[in] block_size_v_line                 The number of bits in vertical  
***********************************************************************/
/* Function Name : v_gdt_blk_cut_comm_16x16_burst */
static void v_gdt_blk_cut_comm_16x16_burst(uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM], uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM], 
                                           uint32_t block_size_h_byte, uint32_t block_size_v_line)    
{
    uint8_t  i;
    uint32_t range_size_h_byte_big;
    uint32_t range_size_h_byte_remain;

    /* burst transfer not need the big range */
    range_size_h_byte_big    = 0;
    range_size_h_byte_remain = block_size_h_byte ;

    /* --------------- remain_range cut         ------------------ */
    if (range_size_h_byte_remain > 0 )
    {
        gs_remain_in_range_addr_info_t.exist            = RANGE_EXIST;
        for(i=0;i<(gdt_img_src_t.img_num);i++)
        {        
            gs_remain_in_range_addr_info_t.range_fst_addr[i]   = block_start_pix_addr_src_array[i] + range_size_h_byte_big;
            gs_in_offset_v[i]                                  = 0;
        }
        gs_remain_in_range_addr_info_t.dat_unit_x_byte  = range_size_h_byte_remain;
        gs_remain_in_range_addr_info_t.dat_unit_y_line  = 32; /* cpu trans byte number of 1 time */
        gs_remain_in_range_addr_info_t.dat_unit_num_h   = 1;
        gs_remain_in_range_addr_info_t.dat_unit_num_v   = block_size_v_line;

        gs_remain_out_range_addr_info_t.exist           = RANGE_EXIST;
        for(i=0;i<(gdt_img_dest_t.img_num);i++)
        {        
            gs_remain_out_range_addr_info_t.range_fst_addr[i]  = block_start_pix_addr_dest_array[i] + range_size_h_byte_big;
            gs_out_offset_v[i]                                 = 0;
        }
        gs_remain_out_range_addr_info_t.dat_unit_x_byte = range_size_h_byte_remain;
        gs_remain_out_range_addr_info_t.dat_unit_y_line = 32; /* cpu trans byte number of 1 time */
        gs_remain_out_range_addr_info_t.dat_unit_num_h  = 1;
        gs_remain_out_range_addr_info_t.dat_unit_num_v  = block_size_v_line;

        gs_remain_trans_mod = COMMON_REMAIN_CPU_XBYTE;
    }
} /*End of Function v_gdt_blk_cut_comm_16x16_burst */

/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range and remain range information
* @param[in] block_start_pix_addr_src_array    source block start address
* @param[in] block_start_pix_addr_dest_array   destination block start address
* @param[in] block_size_h_byte                 The number of bytes in horizontal 
* @param[in] block_size_v_line                 The number of bits in vertical  
***********************************************************************/
/* Function Name : v_gdt_blk_cut_comm_16x16_block */
static void v_gdt_blk_cut_comm_16x16_block(uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM], uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM], 
                                           uint32_t block_size_h_byte, uint32_t block_size_v_line)    
{
        uint8_t i;

        /* --------------- src and dst addr are both odd --------------*/
        /* --------------- remain_range cut              ------------- */
        if (block_start_pix_addr_src_array[0] & 0x00000001) 
        {
            /* --------------- remain_range cut         ------------------ */
            gs_remain_in_range_addr_info_t.exist            = RANGE_EXIST;
            for(i=0;i<(gdt_img_src_t.img_num);i++)
            {        
                gs_remain_in_range_addr_info_t.range_fst_addr[i]   = block_start_pix_addr_src_array[i];
            }
            gs_remain_in_range_addr_info_t.dat_unit_x_byte  = 1;
            gs_remain_in_range_addr_info_t.dat_unit_y_line  = 16;
            gs_remain_in_range_addr_info_t.dat_unit_num_h   = 1;
            gs_remain_in_range_addr_info_t.dat_unit_num_v   = block_size_v_line >> 4;

            gs_remain_out_range_addr_info_t.exist           = RANGE_EXIST;
            for(i=0;i<(gdt_img_dest_t.img_num);i++)
            {        
                gs_remain_out_range_addr_info_t.range_fst_addr[i]  = block_start_pix_addr_dest_array[i] ;
            }
            gs_remain_out_range_addr_info_t.dat_unit_x_byte = 1;
            gs_remain_out_range_addr_info_t.dat_unit_y_line = 16;
            gs_remain_out_range_addr_info_t.dat_unit_num_h  = 1;
            gs_remain_out_range_addr_info_t.dat_unit_num_v  = block_size_v_line >> 4;

            gs_remain_trans_mod = COMMON_REMAIN_CPU_BYTE;

            /* --------------- big_range cut         ------------------ */
            if (block_size_h_byte > 2) 
            {
                gs_big_in_range_addr_info_t.exist            = RANGE_EXIST;

                for(i=0;i<(gdt_img_src_t.img_num);i++)
                {        
                    gs_big_in_range_addr_info_t.range_fst_addr[i]   = block_start_pix_addr_src_array[i] + 1;
                }
                gs_big_in_range_addr_info_t.dat_unit_x_byte  = 2;
                gs_big_in_range_addr_info_t.dat_unit_y_line  = 16;
                gs_big_in_range_addr_info_t.dat_unit_num_h   = (block_size_h_byte >> 1) - 1;
                gs_big_in_range_addr_info_t.dat_unit_num_v   = block_size_v_line >> 4;

                gs_big_out_range_addr_info_t.exist           = RANGE_EXIST;
                
                for(i=0;i<(gdt_img_dest_t.img_num);i++)
                {        
                    gs_big_out_range_addr_info_t.range_fst_addr[i]  = block_start_pix_addr_dest_array[i] + 1;
                }
                gs_big_out_range_addr_info_t.dat_unit_x_byte = 2;
                gs_big_out_range_addr_info_t.dat_unit_y_line = 16;
                gs_big_out_range_addr_info_t.dat_unit_num_h  = (block_size_h_byte >> 1) - 1;
                gs_big_out_range_addr_info_t.dat_unit_num_v  = block_size_v_line >> 4;

            }
        }

        /* --------------- src and dst addr are both even --------------*/
        else
        {
            gs_big_in_range_addr_info_t.exist            = RANGE_EXIST;
            
            for(i=0;i<(gdt_img_src_t.img_num);i++)
            {        
                gs_big_in_range_addr_info_t.range_fst_addr[i]   = block_start_pix_addr_src_array[i] ;
            }
            gs_big_in_range_addr_info_t.dat_unit_x_byte  = 2;
            gs_big_in_range_addr_info_t.dat_unit_y_line  = 16;
            gs_big_in_range_addr_info_t.dat_unit_num_h   = block_size_h_byte >> 1;
            gs_big_in_range_addr_info_t.dat_unit_num_v   = block_size_v_line >> 4;

            gs_big_out_range_addr_info_t.exist           = RANGE_EXIST;

            for(i=0;i<(gdt_img_dest_t.img_num);i++)
            {        
                gs_big_out_range_addr_info_t.range_fst_addr[i]  = block_start_pix_addr_dest_array[i];
            }
            gs_big_out_range_addr_info_t.dat_unit_x_byte = 2;
            gs_big_out_range_addr_info_t.dat_unit_y_line = 16;
            gs_big_out_range_addr_info_t.dat_unit_num_h  = block_size_h_byte >> 1;
            gs_big_out_range_addr_info_t.dat_unit_num_v  = block_size_v_line >> 4;
            
        }

        /* input/output offset v assignment */
        for(i=0;i<(gdt_img_src_t.img_num);i++)
        {        
            gs_in_offset_v[i]  = gs_src_mem_size_h[i]  << 4;
        }
        for(i=0;i<(gdt_img_dest_t.img_num);i++)
        {        
            gs_out_offset_v[i] = gs_dest_mem_size_h[i] << 4;
        }

} /*End of Function v_gdt_blk_cut_comm_16x16_block */

/********************************************************************//**
* @brief configuration information for DMAC (source to gdt inbuffer).
* @param[in] dmtmd_md     dmac transfer mode register configuration
* @param[in] dmtmd_sz     dmac transfer mode register configuration
* @param[in] dmamd_sm     dmac transfer mode register configuration
* @param[in] dmamd_dm     dmac transfer mode register configuration
* @param[in] dmtmd_dts    dmac transfer mode register configuration
* @param[in] data_length  dmac transfer mode register configuration
* @param[in] offset       dmac transfer mode register configuration
* @param[in] inaddr       dmac transfer mode register configuration
* @param[in] isr_func     dmac transfer mode register configuration
* @param[in] p_callback   API call back function
***********************************************************************/
/* Function Name : v_gdt_dmacin_pre_set */
static void v_gdt_dmacin_pre_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                                 uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length, 
                                 uint32_t offset, uint32_t inaddr, isr_function const isr_func,
                                 gdt_cb_event_t const p_callback)
{ 
    st_dma_transfer_data_cfg_t config;

    /* set DMAC register */
    config.mode = dmtmd_md | dmtmd_sz | dmamd_sm | dmamd_dm | dmtmd_dts;

    config.src_addr             = inaddr;
    config.dest_addr            = GDTIBUF0;  /* input address of GDT IN_BUF */ 
    config.transfer_count       = 1;
    config.offset               = offset;
    config.block_size           = data_length;
    config.src_extended_repeat  = 0;
    config.dest_extended_repeat = 0;

    /* open DMAC function */
    if (gsp_dmacdev_in->Open() != DMA_OK)
    {
        p_callback();
    }

    /* enable GDT input interrupt trigger DMAC */
    if (gsp_dmacdev_in->Create(GDT_TO_DMA_IN_TRG, &config) != DMA_OK) 
    {
        p_callback();
    }

    /* enable DMAC input interrupt */    
    if (gsp_dmacdev_in->InterruptEnable(DMA_INT_COMPLETE, isr_func) != DMA_OK)
    {
        p_callback();
    }

    /* DMAC trans enable */
    if (gsp_dmacdev_in->Control(DMA_CMD_START, NULL) != DMA_OK)
    {
        p_callback();
    }
} /* End of Function v_gdt_dmacin_pre_set */

/********************************************************************//**
* @brief configuration information for DMAC (source to gdt inbuffer).
* @param[in] dmtmd_md     dmac transfer mode register configuration
* @param[in] dmtmd_sz     dmac transfer mode register configuration
* @param[in] dmamd_sm     dmac transfer mode register configuration
* @param[in] dmamd_dm     dmac transfer mode register configuration
* @param[in] dmtmd_dts    dmac transfer mode register configuration
* @param[in] data_length  dmac transfer mode register configuration
* @param[in] offset       dmac transfer mode register configuration
* @param[in] inaddr       dmac transfer mode register configuration
* @param[in] p_callback   dmac transfer mode register configuration
**************************API call back function*********************************************/
/* Function Name : v_gdt_dmacin_set */
static void v_gdt_dmacin_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                             uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length, 
                             uint32_t offset, uint32_t inaddr, gdt_cb_event_t const p_callback)
{ 
    st_dma_transfer_data_cfg_t config;

    /* set DMAC register */
    config.mode = dmtmd_md | dmtmd_sz | dmamd_sm | dmamd_dm | dmtmd_dts;

    config.src_addr             = inaddr;
    config.dest_addr            = GDTIBUF0;  /* input address of GDT IN_BUF */ 
    config.transfer_count       = 1;
    config.offset               = offset;
    config.block_size           = data_length;
    config.src_extended_repeat  = 0;
    config.dest_extended_repeat = 0;

    /* open DMAC function */
    if (gsp_dmacdev_in->Open() != DMA_OK)
    {
        /**/ 
        p_callback();
    }

    /* enable GDT input interrupt trigger DMAC */
    if (gsp_dmacdev_in->Create(GDT_TO_DMA_IN_TRG, &config) != DMA_OK) 
    {
        p_callback();
    }

    /* disable DMAC input interrupt */
    if (gsp_dmacdev_in->InterruptDisable() != DMA_OK)
    {
        p_callback();
    }

    /* DMAC trans enable */
    if (gsp_dmacdev_in->Control(DMA_CMD_START, NULL) != DMA_OK)
    {
        p_callback();
    }
} /* End of Function v_gdt_dmacin_set */

/********************************************************************//**
* @brief configuration information for DMAC (gdt outbuffer to destination).
* @param[in] dmtmd_md     dmac transfer mode register configuration
* @param[in] dmtmd_sz     dmac transfer mode register configuration
* @param[in] dmamd_sm     dmac transfer mode register configuration
* @param[in] dmamd_dm     dmac transfer mode register configuration
* @param[in] dmtmd_dts    dmac transfer mode register configuration
* @param[in] data_length  dmac transfer mode register configuration
* @param[in] offset       dmac transfer mode register configuration
* @param[in] outaddr      dmac transfer mode register configuration
* @param[in] isr_function dmac transfer mode register configuration
* @param[in] p_callback   API call back function
***********************************************************************/
/* Function Name : v_gdt_dmacout_pre_set */
static void v_gdt_dmacout_pre_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm,
                                  uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length,
                                  uint32_t offset, uint32_t outaddr, isr_function const isr_func,
                                  gdt_cb_event_t const p_callback)
{
    st_dma_transfer_data_cfg_t config;

    /* set DMAC register */
    config.mode = dmtmd_md | dmtmd_sz | dmamd_sm | dmamd_dm | dmtmd_dts;

    config.src_addr             = GDTOBUF0;
    config.dest_addr            = outaddr;
    config.transfer_count       = 1;
    config.offset               = offset;
    config.block_size           = data_length;    
    config.src_extended_repeat  = 0;
    config.dest_extended_repeat = 0;

    /* open DMAC function */
    if (gsp_dmacdev_out->Open() != DMA_OK)
    {
        p_callback();
    }

    /* enable GDT output interrupt trigger DMAC */    
    if (gsp_dmacdev_out->Create(GDT_TO_DMA_OUT_TRG, &config) != DMA_OK)
    {
        p_callback();
    }

    /* enable GDT output interrupt trigger DMAC */
    if (gsp_dmacdev_out->InterruptEnable(DMA_INT_COMPLETE, isr_func) != DMA_OK)
    {
        p_callback();
    }

    /* DMAC trans enalbe */
    if (gsp_dmacdev_out->Control(DMA_CMD_START, NULL) != DMA_OK)
    {
        p_callback();
    }
} /* End of Function v_gdt_dmacout_pre_set */

/********************************************************************//**
* @brief This function processes the conversion block in the set order.
         This  function is for common
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_gen_common */
static void v_gdt_seq_gen_common( gdt_cb_event_t const p_callback )
{
    /* function of common sequence */
    /* Remain area do not exist */
    if(RANGE_NOT_EXIST == gs_remain_in_range_addr_info_t.exist)
    {
        /* big area transmit */
        if(RANGE_EXIST == gs_big_in_range_addr_info_t.exist)
        {
            /* block transfer(2byte * 16line or 1 byte * 8line) */
            v_gdt_seq_comm_big_trans_blk(gs_gdt_unit_size,gs_gdt_unit_size,p_callback); /* src dest gs_gdt_unit_size = 16 */

            /* GDT set and GDT srart (function) */
            v_gdt_imgdata_trans_irq_enable();
            v_gdt_on();
        }

        /* transmit finished, execute callback */
        else
        {
            /* clear pending the interrupt status */
            R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

            /* clear pending the interrupt status */
            R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

            /* clear the interrupt status */
            R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

            /* clear the interrupt status */
            R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);    

            /* disable GDT input interrupt */
            R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

            /* disable GDT output interrupt */
            R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

            /* disable GDT input interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);

            /* disable GDT output interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

            /* when endian finish, then execute callback */
            p_callback();
        }
    }

    /* remain range transmit firstly */
    else 
    {
        /* CPU byte transfer */
        if(COMMON_REMAIN_CPU_BYTE == gs_remain_trans_mod)
        {
            /* enable GDT input interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, GDT_IN_IELS_VALUE,  v_isr_cpu_input);

            /* enable GDT output interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, GDT_OUT_IELS_VALUE, v_isr_cpu_output);

            v_gdt_seq_comm_remn_trans_byte();
        }

        /* CPU xbyte transfer */
        else if (COMMON_REMAIN_CPU_XBYTE == gs_remain_trans_mod)
        {
            /* enable GDT output interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, GDT_IN_IELS_VALUE,  v_isr_cpu_input_xbyte);

            /* enable GDT output interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, GDT_OUT_IELS_VALUE, v_isr_cpu_output_xbyte);

            v_gdt_seq_comm_remn_trans_xbyte();
        }
        else  /* do nothing */ 
        {
            __NOP();
        }

        /* GDT set and GDT srart (function) */
        v_gdt_imgdata_trans_irq_enable();
        v_gdt_on();
    }
} /* End of Function v_gdt_seq_gen_common */

/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the set order.
* @param[in] src_data_length    source data length
* @param[in] dest_data_length   destination data length
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_comm_big_trans_blk */
static void v_gdt_seq_comm_big_trans_blk(uint32_t src_data_length, uint32_t dest_data_length,
                                         gdt_cb_event_t const p_callback )
{

    /* global variable assign */
    if(1 == gdt_img_src_t.img_num) /* sigle image */
    {
        gs_in_dat_unit_cnt_h    = 1;
    }
    else                           /* multiple image */
    {        
        gs_in_dat_unit_cnt_h    = 0;
    }

    gs_in_dat_unit_cnt_v    = 0;
    for(gs_in_img_cnt=0;gs_in_img_cnt<(gdt_img_src_t.img_num);gs_in_img_cnt++)
    {        
        gs_in_img_addr_line_org_array[gs_in_img_cnt] = gs_big_in_range_addr_info_t.range_fst_addr[gs_in_img_cnt];
    }    

    gs_in_offset_h          = gs_big_in_range_addr_info_t.dat_unit_x_byte;
    gs_in_dat_unit_num_h    = gs_big_in_range_addr_info_t.dat_unit_num_h;
    gs_in_dat_unit_num_v    = gs_big_in_range_addr_info_t.dat_unit_num_v;
    gs_in_img_addr          = gs_big_in_range_addr_info_t.range_fst_addr[0]; /* block addr of first image */
    gs_in_buf_addr          = GDTIBUF0;
    gs_in_offset            = gs_src_mem_size_h[0];
    for(gs_in_img_cnt=0;gs_in_img_cnt<(gdt_img_src_t.img_num);gs_in_img_cnt++)
    {        
        gs_in_img_addr_array[gs_in_img_cnt] = gs_big_in_range_addr_info_t.range_fst_addr[gs_in_img_cnt];
    }
    gs_in_img_cnt = 0;
    
    /* global variable assign */
    if(1 == gdt_img_dest_t.img_num) /* sigle image */
    {
        gs_out_dat_unit_cnt_h    = 1;
    }
    else                           /* multiple image */
    {        
        gs_out_dat_unit_cnt_h    = 0;
    }
    gs_out_dat_unit_cnt_y    = 0;
    gs_out_dat_unit_cnt_v    = 0;
    for(gs_out_img_cnt=0;gs_out_img_cnt<(gdt_img_dest_t.img_num);gs_out_img_cnt++)
    {        
        gs_out_img_addr_line_org_array[gs_out_img_cnt] = gs_big_out_range_addr_info_t.range_fst_addr[gs_out_img_cnt];
    }  

    gs_out_offset_h          = gs_big_out_range_addr_info_t.dat_unit_x_byte;
    gs_out_dat_unit_num_h    = gs_big_out_range_addr_info_t.dat_unit_num_h;
    gs_out_dat_unit_num_v    = gs_big_out_range_addr_info_t.dat_unit_num_v;
    gs_out_img_addr          = gs_big_out_range_addr_info_t.range_fst_addr[0]; /* block addr of first image */ 
    gs_out_buf_addr          = GDTOBUF0;
    gs_out_offset            = gs_dest_mem_size_h[0];
    for(gs_out_img_cnt=0;gs_out_img_cnt<(gdt_img_dest_t.img_num);gs_out_img_cnt++)
    {        
        gs_out_img_addr_array[gs_out_img_cnt] = gs_big_out_range_addr_info_t.range_fst_addr[gs_out_img_cnt];
    }
    gs_out_img_cnt=0;

    /* big trans sequence for sre image */
    v_gdt_seq_big_trans_src(src_data_length,p_callback);

    /* big trans sequence for dest image */
    v_gdt_seq_big_trans_dest(dest_data_length,p_callback);

} /* End of Function v_gdt_seq_comm_big_trans_blk */        

/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the sequence transmit for source image.
* @param[in] data_length        data length
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_big_trans_src */
static void v_gdt_seq_big_trans_src(uint32_t data_length,
                                    gdt_cb_event_t const p_callback )
{
    uint16_t dmac_size;

    /* internal variable assign */
    switch(gs_gdt_unit_size)
    {
        case 8 :
            {    
            dmac_size = DMA_SIZE_BYTE;
            }
        break;
        case 16:
            {    
            dmac_size = DMA_SIZE_WORD;
            }
        break;
        default : 
            {
            dmac_size = DMA_SIZE_WORD;
            }
        break;
    }

    if(1 == gdt_img_src_t.img_num) /* sigle image */
    {        
        /* only input 1 unit ( 2 byte * 16 line or 1 byte * 8 line) */ 
        if ((1 == gs_in_dat_unit_num_h) && (1 == gs_in_dat_unit_num_v))
        {
            /* DMAC input 1st trans */ 
            v_gdt_dmacin_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                             data_length, gs_src_mem_size_h[0], gs_in_img_addr, p_callback);
        }
        /* other situation */
        else
        {
            /* DMAC input 1st trans */ 
            v_gdt_dmacin_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                                 data_length, gs_src_mem_size_h[0], gs_in_img_addr, v_isr_dmac_input, p_callback);
    
            /* calculation DMAC 2nd input IMG address */
            /* only input once in 1 line, next DMAC trans will to next line */
            if(1 == gs_in_dat_unit_num_h)
            {
                gs_in_dat_unit_cnt_v++;     /* After the end of a line, the number of columns is incremented */          
                gs_in_img_addr  = gs_in_img_addr_line_org_array[0]  + gs_in_offset_v[0];
                gs_in_img_addr_line_org_array[0]  = gs_in_img_addr;
                gs_in_buf_addr = GDTIBUF0;
            }

            /* only input once in 1 line, next DMAC trans will to next same line */
            else
            {
                gs_in_img_addr  = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h;
                gs_in_buf_addr  = GDTIBUF0;
                gs_in_img_addr_array[0] = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h;
            }
        }
    }
    else  /* multiple image */
    {
            v_gdt_dmacin_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                                 data_length, gs_in_offset, gs_in_img_addr, v_isr_dmac_input, p_callback);
    
            /* calculation DMAC 2nd output IMG address */
            gs_in_img_cnt++;
            gs_in_offset    = gs_src_mem_size_h[gs_in_img_cnt];
            gs_in_img_addr  = gs_in_img_addr_line_org_array[gs_in_img_cnt];
            gs_in_buf_addr  = GDTIBUF0 + DAT_BUF_OFFSET;
    }
    
} /* End of Function v_gdt_seq_big_trans_src */        

/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the sequence of transmit for destination image.
* @param[in] data_length        data length
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_big_trans_dest */
static void v_gdt_seq_big_trans_dest(uint32_t data_length,
                                     gdt_cb_event_t const p_callback )
{
    uint16_t dmac_size;

    /* internal variable assign */
    switch(gs_gdt_unit_size)
    {
        case 8 :
            {    
            dmac_size = DMA_SIZE_BYTE;
            }
        break;
        case 16:
            {    
            dmac_size = DMA_SIZE_WORD;
            }
        break;
        default : 
            {
            dmac_size = DMA_SIZE_WORD;
            }
        break;
    }

    if(1 == gdt_img_dest_t.img_num) /* sigle image */
    {        
        /* only output 1unit ( 2 byte * 16 line or 1 byte * 8 line ) */ 
        if ((1 == gs_out_dat_unit_num_h) && (1 == gs_out_dat_unit_num_v))
        {
            /* DMAC output 1st trans and call v_isr_dmac_output_once */ 
            v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_INCR, DMA_DEST_ADDR_OFFSET, DMA_REPEAT_BLOCK_DEST,
                                 data_length, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_output_once, p_callback);
        }

        /* other situation */
        else
        {
            /* DMAC output 1st trans */ 
            v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_INCR, DMA_DEST_ADDR_OFFSET, DMA_REPEAT_BLOCK_DEST,
                                  data_length, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_output, p_callback);
    
            /* only output once in 1 line, next DMAC trans will to next line */
            if(1 == gs_out_dat_unit_num_h)
            {
                gs_out_dat_unit_cnt_v++;     /* After the end of a line, the number of columns is incremented */           
                gs_out_img_addr = gs_out_img_addr_line_org_array[0] + gs_out_offset_v[0];
                gs_out_img_addr_line_org_array[0]  = gs_out_img_addr;
                gs_out_buf_addr = GDTOBUF0;
            }
    
            /* only output once in 1 line, next DMAC trans will to next same line */
            else
            {
                gs_out_img_addr = gs_out_img_addr_line_org_array[0] + gs_out_offset_h;
                gs_out_buf_addr = GDTOBUF0;
                gs_out_img_addr_array[0] = gs_out_img_addr_line_org_array[0]  + gs_out_offset_h; 
            }
        }
    }
    else /* multiple image */
    {
        /* DMAC output 1st trans */ 
        v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_INCR, DMA_DEST_ADDR_OFFSET, DMA_REPEAT_BLOCK_DEST,
                              data_length, gs_out_offset, gs_out_img_addr, v_isr_dmac_output, p_callback);
    
        /* calculation DMAC 2nd output IMG address */
        gs_out_img_cnt++;
        gs_out_offset   = gs_dest_mem_size_h[gs_out_img_cnt];
        gs_out_img_addr = gs_out_img_addr_line_org_array[gs_out_img_cnt] ;
        gs_out_buf_addr = GDTOBUF0 + DAT_BUF_OFFSET; 
    }
} /* End of Function v_gdt_seq_big_trans_dest */        

/********************************************************************//**
* @brief This function processes remain area of the conversion block 
*        in the set order.
***********************************************************************/
/* Function Name : v_gdt_seq_comm_remn_trans_xbyte */
static void v_gdt_seq_comm_remn_trans_xbyte(void)
{
    uint8_t i;
        
    /* global variable assign */
    gs_in_img_cnt  = 0; 
    gs_out_img_cnt = 0; 

    /* prepare the first src addr */
    gs_in_dat_unit_cnt_h    = 0;
    gs_in_dat_unit_cnt_v    = 0;
    for(i=0;i<(gdt_img_src_t.img_num);i++)
    {        
        gs_in_img_addr_line_org_array[i] = 0;
    }
    gs_in_offset_h          = gs_remain_in_range_addr_info_t.dat_unit_x_byte;
    gs_in_dat_unit_line_y   = gs_remain_in_range_addr_info_t.dat_unit_y_line; 
    gs_in_dat_unit_num_h    = gs_remain_in_range_addr_info_t.dat_unit_num_h;
    gs_in_dat_unit_num_v    = gs_remain_in_range_addr_info_t.dat_unit_num_v;
    gs_in_img_addr          = gs_remain_in_range_addr_info_t.range_fst_addr[0];
    for(i=0;i<(gdt_img_src_t.img_num);i++)
    {        
        gs_in_img_addr_array[i] = gs_remain_in_range_addr_info_t.range_fst_addr[i];
    }
    gs_in_buf_addr          = GDTIBUF0;
    
    /* prepare the first dest addr */
    gs_out_dat_unit_cnt_h    = 0;
    gs_out_dat_unit_cnt_y    = 0;
    gs_out_dat_unit_cnt_v    = 0;
    for(i=0;i<(gdt_img_dest_t.img_num);i++)
    {        
        gs_out_img_addr_line_org_array[i] = 0;
    }
    gs_out_offset_h          = gs_remain_out_range_addr_info_t.dat_unit_x_byte;
    gs_out_dat_unit_line_y   = gs_remain_out_range_addr_info_t.dat_unit_y_line; 
    gs_out_dat_unit_num_h    = gs_remain_out_range_addr_info_t.dat_unit_num_h;
    gs_out_dat_unit_num_v    = gs_remain_out_range_addr_info_t.dat_unit_num_v;
    gs_out_img_addr          = gs_remain_out_range_addr_info_t.range_fst_addr[0];
    for(i=0;i<(gdt_img_dest_t.img_num);i++)
    {        
        gs_out_img_addr_array[i]          = gs_remain_out_range_addr_info_t.range_fst_addr[i];
    }
    gs_out_img_addr_unit_line_org = gs_out_img_addr;
    gs_out_buf_addr          = GDTOBUF0;

    /* CPU input and output 1st trans information initial */ 
    gs_in_dat_unit_cnt_h = 0;  /* the start number in range*/
    gs_in_dat_unit_cnt_v = ((gs_remain_in_range_addr_info_t.range_fst_addr[gdt_img_src_t.img_num-1]
                           + gs_remain_in_range_addr_info_t.dat_unit_x_byte)
                           + (gs_remain_in_range_addr_info_t.dat_unit_num_v * gs_src_mem_size_h[gdt_img_src_t.img_num-1]))
                           - gs_src_mem_size_h[gdt_img_src_t.img_num-1];   /* the end addr of range */ 

    gs_out_dat_unit_cnt_h = 0;  /* the start number in range*/
    gs_out_dat_unit_cnt_v = ((gs_remain_out_range_addr_info_t.range_fst_addr[gdt_img_dest_t.img_num-1]
                           + gs_remain_out_range_addr_info_t.dat_unit_x_byte)
                           + (gs_remain_out_range_addr_info_t.dat_unit_num_v * gs_dest_mem_size_h[gdt_img_dest_t.img_num-1]))
                           - gs_dest_mem_size_h[gdt_img_dest_t.img_num-1]; /* the end addr of range */ 

    /* CPU input and output interrupt enable */
    /* enable cpu input interrupt */
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

    /* enable cpu output interrupt*/
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

} /* End of Function v_gdt_seq_comm_remn_trans_xbyte */        

/********************************************************************//**
* @brief This function processes remain area of the conversion block 
*        in the set order.
***********************************************************************/
/* Function Name : v_gdt_seq_comm_remn_trans_byte */
static void v_gdt_seq_comm_remn_trans_byte(void)
{
    uint8_t i;

    /* global variable assign */
    gs_in_img_cnt  = 0;
    gs_out_img_cnt = 0;

    /* prepare the first src addr */
    gs_in_dat_unit_cnt_h    = 0;
    gs_in_dat_unit_cnt_v    = 0;
    for(i=0;i<(gdt_img_src_t.img_num);i++)
    {        
        gs_in_img_addr_line_org_array[i] = gs_remain_in_range_addr_info_t.range_fst_addr[i];
    }

    gs_in_offset_h          = 1
                              +(gs_big_in_range_addr_info_t.dat_unit_num_h   
                              * gs_big_in_range_addr_info_t.dat_unit_x_byte);
    
    gs_in_dat_unit_line_y   = gs_remain_in_range_addr_info_t.dat_unit_y_line; 
    gs_in_dat_unit_num_h    = gs_remain_in_range_addr_info_t.dat_unit_num_h;
    gs_in_dat_unit_num_v    = gs_remain_in_range_addr_info_t.dat_unit_num_v;
    gs_in_img_addr          = gs_remain_in_range_addr_info_t.range_fst_addr[0];

    for(i=0;i<(gdt_img_src_t.img_num);i++)
    {        
        gs_in_img_addr_array[i]          = gs_remain_in_range_addr_info_t.range_fst_addr[i];
    }
    gs_in_buf_addr          = GDTIBUF0;
    
    /* prepare the first dest addr */
    gs_out_dat_unit_cnt_h    = 0;
    gs_out_dat_unit_cnt_y    = 0;
    gs_out_dat_unit_cnt_v    = 0;
    for(i=0;i<(gdt_img_dest_t.img_num);i++)
    {        
        gs_out_img_addr_line_org_array[i] = gs_remain_out_range_addr_info_t.range_fst_addr[i];
    }

    gs_out_offset_h          = 1
                               +(gs_big_out_range_addr_info_t.dat_unit_num_h   
                               * gs_big_out_range_addr_info_t.dat_unit_x_byte);

    gs_out_dat_unit_line_y   = gs_remain_out_range_addr_info_t.dat_unit_y_line; 
    gs_out_dat_unit_num_h    = gs_remain_out_range_addr_info_t.dat_unit_num_h;
    gs_out_dat_unit_num_v    = gs_remain_out_range_addr_info_t.dat_unit_num_v;
    gs_out_img_addr          = gs_remain_out_range_addr_info_t.range_fst_addr[0];

    for(i=0;i<(gdt_img_dest_t.img_num);i++)
    {        
        gs_out_img_addr_array[i]          = gs_remain_out_range_addr_info_t.range_fst_addr[i];
    }
    gs_out_img_addr_unit_line_org = gs_out_img_addr;
    gs_out_buf_addr          = GDTOBUF0;

    /* CPU input and output interrupt enable */
    /* enable cpu input interrupt */
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

    /* enable cpu input interrupt */
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

} /* End of Function v_gdt_seq_comm_remn_trans_byte */        

/********************************************************************//**
* @brief enable interrupt of GDT
***********************************************************************/
/* Function Name : v_gdt_imgdata_trans_irq_enable */
static void v_gdt_imgdata_trans_irq_enable(void) 
{
    /* GDTPIER enable */
    gs_gdt_resources.reg_gdt->GDTPIER_b.DATIIE  = VAL_ENABLE;
    gs_gdt_resources.reg_gdt->GDTPIER_b.DATOIE  = VAL_ENABLE;
    gs_gdt_resources.reg_gdt->GDTPIER_b.INTMODE = INTMOD_DMAC; 

} /* End of function v_gdt_imgdata_trans_irq_enable */

/********************************************************************//**
* @brief disable interrupt of GDT
***********************************************************************/
/* Function Name : v_gdt_imgdata_trans_irq_disable */
static void v_gdt_imgdata_trans_irq_disable(void)
{
    /* GDTPIER disable */
    gs_gdt_resources.reg_gdt->GDTPIER_b.DATIIE = VAL_DISABLE;
    gs_gdt_resources.reg_gdt->GDTPIER_b.DATOIE = VAL_DISABLE;
} /* End of Function v_gdt_imgdata_trans_irq_disable */

/********************************************************************//**
* @brief Interrupt handler for GDT input request
***********************************************************************/
/* Function Name : v_isr_cpu_input */
static void v_isr_cpu_input (void)
{
    uint32_t in_buf_addr; 

    /* CPU FUNCTION */
    /* first transfer data  1byte* 16line*/

    v_gdt_cpublk_byte_wr_gdt_normal(gs_in_img_addr,gs_in_buf_addr,gs_src_mem_size_h[gs_in_img_cnt],gs_in_dat_unit_line_y);

    /* calculation the 2nd img address */
    gs_in_img_addr = gs_in_img_addr + gs_in_offset_h;
    in_buf_addr    = gs_in_buf_addr + gs_in_dat_unit_line_y;

    /* second transfer data */
    v_gdt_cpublk_byte_wr_gdt_normal(gs_in_img_addr,in_buf_addr,gs_src_mem_size_h[gs_in_img_cnt],gs_in_dat_unit_line_y);

    /* update next times CPU trans address and CPU trans end disable */
    v_gdt_cpu_update_in_info();
} /* End of Function v_isr_cpu_input */

/********************************************************************//**
* @brief Interrupt handler for GDT output request
***********************************************************************/
/* Function Name : v_isr_cpu_output */
static void v_isr_cpu_output (void)                     
{
    uint32_t out_buf_addr; 

    /* CPU FUNCTION */
    /* first transfer data */
    v_gdt_cpublk_byte_rd_gdt_normal(gs_out_buf_addr,gs_out_img_addr,gs_dest_mem_size_h[gs_out_img_cnt],gs_out_dat_unit_line_y);

    /* calculation the 2nd img address */
    gs_out_img_addr = gs_out_img_addr+ gs_out_offset_h;
    out_buf_addr    = gs_out_buf_addr + gs_out_dat_unit_line_y;

    /* second transfer data */
     v_gdt_cpublk_byte_rd_gdt_normal(out_buf_addr,gs_out_img_addr,gs_dest_mem_size_h[gs_out_img_cnt],gs_out_dat_unit_line_y);

    /* update next times CPU trans address and CPU trans end disable */
    v_gdt_cpu_update_out_info();
} /* End of Function v_isr_cpu_output */

/********************************************************************//**
* @brief Interrupt handler for GDT input request
***********************************************************************/
/* Function Name : v_isr_cpu_input_xbyte */
static void v_isr_cpu_input_xbyte (void)
{
    st_cpublk_xbyte_trans_info_t trans_info_wr;
                                                                        
    /* judge the range trans over */
    if ( gs_in_img_addr_array[gdt_img_src_t.img_num-1] >= gs_in_dat_unit_cnt_v) /* gs_in_dat_unit_cnt_v stands for the end addr of range */
    {
        /* clear remain area flag */    
        gs_remain_in_range_addr_info_t.exist  = RANGE_NOT_EXIST;
        gs_remain_out_range_addr_info_t.exist = RANGE_NOT_EXIST;

        /* clear GDT input interrupt flag */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);    

        /* disable GDT input interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* disable GDT input interrupt trigger CPU  */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);

        v_gdt_imgdata_trans_irq_disable();
        v_gdt_off();                                                           /* disable GDT start */ 
        v_gdt_seq_gen_common(gs_int_callback); 
    }
    else 
    {
        gs_in_img_addr       = gs_in_img_addr_array[gs_in_img_cnt] ;           /* start addr */
        gs_in_dat_unit_cnt_h = gs_in_img_addr_line_org_array[gs_in_img_cnt] ;  /* start pix coodinate in line */

        /* CPU FUNCTION */
        trans_info_wr = st_gdt_cpublk_xbyte_wr_gdt_nor(
                        gs_in_img_addr,        /* source address of input data  */
                        gs_in_buf_addr,        /* destination address of input data (GDTIBUF) */
                        1,                     /* offset address of src_addr between first data and next */
                        gs_in_dat_unit_line_y, /* (byte)number of input data per interrupt */
                        gs_src_mem_size_h[gs_in_img_cnt],  /* offset address of src_addr between first line data and next line */
                        gs_remain_in_range_addr_info_t.dat_unit_x_byte, /* (byte)number of input data per interrupt */ 
                        gs_in_dat_unit_cnt_h   /* the position at which each line starts to be transmitted */ 
                        );

        gs_in_buf_addr                               = gs_in_buf_addr + DAT_BUF_OFFSET;  
        gs_in_img_addr_array         [gs_in_img_cnt] = trans_info_wr.trans_addr   ;
        gs_in_img_addr_line_org_array[gs_in_img_cnt] = trans_info_wr.range_h_byte ;    

        if(gs_in_img_cnt >= (gdt_img_src_t.img_num-1))
        {        
            gs_in_buf_addr = GDTIBUF0;
            gs_in_img_cnt  = 0;
        }
        else
        {
            gs_in_img_cnt++;
        }
    }

} /* End of Function v_isr_cpu_input_xbyte */

/********************************************************************//**
* @brief Interrupt handler for GDT output request
***********************************************************************/
/* Function Name : v_isr_cpu_output_xbyte */
static void v_isr_cpu_output_xbyte (void)
{
    st_cpublk_xbyte_trans_info_t trans_info_rd;

    /* judge the range trans over */
    if ( gs_out_img_addr_array[gdt_img_dest_t.img_num-1] >= gs_out_dat_unit_cnt_v) /* gs_out_dat_unit_cnt_v stands for the end addr of range */
    {
        /* clear GDT input interrupt flag */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);    

        /* disable GDT input interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* disable GDT output interrupt trigger CPU  */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);
    }

    else
    {        
        gs_out_img_addr       = gs_out_img_addr_array[gs_out_img_cnt] ;          /* start addr */
        gs_out_dat_unit_cnt_h = gs_out_img_addr_line_org_array[gs_out_img_cnt] ; /*  start pix coodinate in line. */

        /* CPU FUNCTION */
        trans_info_rd = st_gdt_cpublk_xbyte_rd_gdt_nor(
                        gs_out_buf_addr,        /* source address of input data  */                                    
                        gs_out_img_addr,        /* destination address of input data (GDTIBUF) */                      
                        1,                      /* offset address of dest_addr between first data and next */           
                        gs_out_dat_unit_line_y, /* (byte)number of input data per interrupt */                         
                        gs_dest_mem_size_h[gs_out_img_cnt],  /* offset address of dest_addr between first line data and next line */ 
                        gs_remain_out_range_addr_info_t.dat_unit_x_byte, /* (byte)number of input data per interrupt */  
                        gs_out_dat_unit_cnt_h   /* the position at which each line starts to be transmitted */  
                        );

        gs_out_buf_addr = gs_out_buf_addr + DAT_BUF_OFFSET;  
        gs_out_img_addr_array         [gs_out_img_cnt] = trans_info_rd.trans_addr   ;
        gs_out_img_addr_line_org_array[gs_out_img_cnt] = trans_info_rd.range_h_byte ;    
        
        if(gs_out_img_cnt >= (gdt_img_dest_t.img_num-1))
        {        
            gs_out_buf_addr = GDTOBUF0;
            gs_out_img_cnt  = 0;
        }
        else
        {
            gs_out_img_cnt++;
        }
    }
} /* End of Function v_isr_cpu_output_xbyte */

/********************************************************************//**
* @brief Interrupt handler for dmac input complete request
***********************************************************************/
/* Function Name : v_isr_dmac_input */
static void v_isr_dmac_input (void)
{
    /* update address and DMAC enable */ 
    *gsp_dmac_reg_in_ofr    = gs_in_offset;
    *gsp_dmac_reg_in_sar    = gs_in_img_addr; 
    *gsp_dmac_reg_in_dar    = gs_in_buf_addr;       
    *gsp_dmac_reg_in_crb    = 0x01;           
    *gsp_dmac_reg_in_cnt    = VAL_ENABLE;         
    
    /* DMAC FUNCTION */
    /* update next times input DMAC trans address and DMAC trans end disable */
    v_gdt_dmac_blk_upinf_in_array();
} /* End of Function v_isr_dmac_input */

/********************************************************************//**
* @brief Interrupt handler for dmac output complete request
***********************************************************************/
/* Function Name : v_isr_dmac_output */
static void v_isr_dmac_output (void)
{
    /* update address and DMAC enable */   
    *gsp_dmac_reg_out_ofr    = gs_out_offset;
    *gsp_dmac_reg_out_dar    = gs_out_img_addr; 
    *gsp_dmac_reg_out_sar    = gs_out_buf_addr; 
    *gsp_dmac_reg_out_crb    = 0x01;            
    *gsp_dmac_reg_out_cnt    = VAL_ENABLE;          

    /* DMAC FUNCTION */
    /* update next times output DMAC trans address and DMAC trans end disable */
    v_gdt_dmac_blk_upinf_out_array();  
} /* End of Function v_isr_dmac_output */

/********************************************************************//**
* @brief Interrupt handler for dmac once output complete request
***********************************************************************/
/* Function Name : v_isr_dmac_output_once */
static void v_isr_dmac_output_once (void)
{
    gs_out_dat_unit_cnt_v = 0;
    gs_big_in_range_addr_info_t.exist = RANGE_NOT_EXIST;
     
    if (gsp_dmacdev_out->InterruptDisable() != DMA_OK)
    {
        gs_int_callback();
    }

    if (gsp_dmacdev_in->Close() != DMA_OK)
    {
        gs_int_callback();
    }

    if (gsp_dmacdev_out->Close() != DMA_OK)
    {
        gs_int_callback();
    }
    v_gdt_imgdata_trans_irq_disable();
    v_gdt_off();                           /* disable GDT start */
    v_gdt_seq_gen_common(gs_int_callback); 
} /* End of Function v_isr_dmac_output_once */

/********************************************************************//**
* @brief Update the next CPU transfer message for CPU input
***********************************************************************/
/* Function Name : v_gdt_cpu_update_in_info */
static void v_gdt_cpu_update_in_info(void)
{

    if(gs_in_img_cnt >= (gdt_img_src_t.img_num-1))
    {        
        gs_in_dat_unit_cnt_h++;
        gs_in_buf_addr = GDTIBUF0;
        gs_in_img_cnt = 0;
    
        /* unit line transfer completed */
        if(gs_in_dat_unit_cnt_h == gs_in_dat_unit_num_h)
        {    
            gs_in_dat_unit_cnt_h = 0;             /* trans end gs_in_dat_unit_cnt_v clear */
            gs_in_dat_unit_cnt_v++;               /* After the end of a line, the number of columns is incremented */
    
            /* unit transfer completed */
            if(gs_in_dat_unit_cnt_v == gs_in_dat_unit_num_v)
            {
                gs_in_dat_unit_cnt_v = 0;         /* trans end gs_in_dat_unit_cnt_v clear */
    
                /* clear GDT input interrupt flag */
                R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);    
    
                /* disable GDT input interrupt */
                R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);
    
                /* disable GDT input interrupt trigger CPU  */
                R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);
            }
            /*  transfer to next line */
            else
            {
    
                for (gs_in_img_cnt=0;gs_in_img_cnt<gdt_img_src_t.img_num;gs_in_img_cnt++)
                {
                    gs_in_img_addr_array[gs_in_img_cnt] = gs_in_img_addr_line_org_array[gs_in_img_cnt] + gs_in_offset_v[gs_in_img_cnt];    /* Base address plus line offset */ 
                    gs_in_img_addr_line_org_array[gs_in_img_cnt] = gs_in_img_addr_array[gs_in_img_cnt];
                }
                gs_in_img_cnt = 0;
                gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt];
            }
        }
        else
        {
            for (gs_in_img_cnt=0;gs_in_img_cnt<gdt_img_src_t.img_num;gs_in_img_cnt++)
            {
                gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt] + gs_in_offset_h;   /* Base address plus row offset */
                gs_in_img_addr_array[gs_in_img_cnt] = gs_in_img_addr;
            }
            gs_in_img_cnt = 0;
            gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt];
        }
    }
    else
    {
        gs_in_img_cnt++;
        gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt];    /* Base address plus row offset */
        gs_in_buf_addr = gs_in_buf_addr + DAT_BUF_OFFSET;  
    }

} /* End of Function v_gdt_cpu_update_in_info */

/********************************************************************//**
* @brief Update the next CPU transfer message for CPU input
***********************************************************************/
/* Function Name : v_gdt_cpu_update_out_info */
static void v_gdt_cpu_update_out_info(void)
{
    if(gs_out_img_cnt >= (gdt_img_dest_t.img_num-1))
    {        
        gs_out_dat_unit_cnt_h++;
        gs_out_buf_addr = GDTOBUF0;
        gs_out_img_cnt = 0;
    
        /* unit line transfer completed */
        if(gs_out_dat_unit_cnt_h == gs_out_dat_unit_num_h)
        {    
            gs_out_dat_unit_cnt_h = 0;             /* trans end gs_out_dat_unit_cnt_v clear */
            gs_out_dat_unit_cnt_v++;               /* After the end of a line, the number of columns is incremented */
    
            /* unit transfer completed */
            if(gs_out_dat_unit_cnt_v == gs_out_dat_unit_num_v)
            {
                gs_out_dat_unit_cnt_v = 0;         /* trans end gs_out_dat_unit_cnt_v clear */
                v_gdt_imgdata_trans_irq_disable();
                gs_remain_in_range_addr_info_t.exist  = RANGE_NOT_EXIST;
                gs_remain_out_range_addr_info_t.exist = RANGE_NOT_EXIST;
    
                /* clear GDT output interrupt flag */
                R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);
    
                /* disable GDT input interrupt */
                R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);
    
                /* disable GDT output interrupt trigger CPU  */
                R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);
                v_gdt_off();                       /* disable GDT start */ 
                v_gdt_seq_gen_common(gs_int_callback); 
            }
            else
            {
                for(gs_out_img_cnt=0;gs_out_img_cnt<gdt_img_dest_t.img_num;gs_out_img_cnt++)
                {
                    gs_out_img_addr_array[gs_out_img_cnt] = gs_out_img_addr_line_org_array[gs_out_img_cnt] + gs_out_offset_v[gs_out_img_cnt];   /* Base address plus line offset */ 
                    gs_out_img_addr_line_org_array[gs_out_img_cnt] = gs_out_img_addr_array[gs_out_img_cnt];
                }
                gs_out_img_cnt = 0;
                gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt];
            }
        }
        else
        {
            for(gs_out_img_cnt=0;gs_out_img_cnt<gdt_img_dest_t.img_num;gs_out_img_cnt++)
            {
                gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt] + gs_out_offset_h;    /* Base addreess plus row offset */
                gs_out_img_addr_array[gs_out_img_cnt] = gs_out_img_addr;
            }
            gs_out_img_cnt = 0;
            gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt];
        }
    }
    else
    {
        gs_out_img_cnt++;
        gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt];
        gs_out_buf_addr = gs_out_buf_addr + DAT_BUF_OFFSET;
    }

} /* End of Function v_gdt_cpu_update_out_info */

/********************************************************************//**
* @brief Function of inputing data from source addr to GDT with cpu
    byte after byte                                           
* @param[in] src_addr        source address of input data
* @param[in] dest_addr       destination address of input data(GDTIBUF) 
* @param[in] offset_src_addr offset address of src_addr between first data and next
* @param[in] data_n          (byte)number of input data per interrupt
***********************************************************************/
/* Function Name : v_gdt_cpublk_byte_wr_gdt_normal */
static void v_gdt_cpublk_byte_wr_gdt_normal (uint32_t src_addr,    uint32_t dest_addr,
                                             uint32_t offset_src_addr, uint32_t data_n)
{
    uint32_t i;

    for(i=0;i<data_n;i++)
    {
        /* CPU trans valid data */      
        *((uint8_t *)dest_addr) = *((uint8_t *)src_addr);
        src_addr += offset_src_addr;
        dest_addr++;
    }
} /* End of Function v_gdt_cpublk_byte_wr_gdt_normal */

/********************************************************************//**
* @brief Function of outputing data from GDT to destination addr with
         cpu byte after byte.                                           
* @param[in] src_addr         source address of input data (GDTOBUF)  
* @param[in] dest_addr        destination address of input data
* @param[in] offset_dest_addr offset address of src_addr between first data and next
* @param[in] data_n           (byte)number of input data per interrupt
***********************************************************************/
/* Function Name : v_gdt_cpublk_byte_rd_gdt_normal */
static void v_gdt_cpublk_byte_rd_gdt_normal (uint32_t src_addr,    uint32_t dest_addr, 
                                             uint32_t offset_dest_addr, uint32_t data_n)
{
    uint32_t i;

    for(i=0;i<data_n;i++)
    {
        /* CPU trans valid data */      
        *((uint8_t *)dest_addr) = *((uint8_t *)src_addr);
        dest_addr += offset_dest_addr;
        src_addr++;

        /* src addr not changed (GDTOBUF0)*/
    }
} /* End of Function v_gdt_cpublk_byte_rd_gdt_normal */

/********************************************************************//**
* @brief Function of inputing data from source addr to GDT with
         cpu byte after byte.                                           
* @param[in] src_addr             source address of input data 
* @param[in] dest_addr            destination address of input data (GDTIBUF)
* @param[in] offset_src_addr      offset address of src_addr between first data and next
* @param[in] data_n               (byte)number of input data per interrupt
* @param[in] offset_line_src_addr offset address of src_addr between first line data and next line 
* @param[in] range_h_byte         (byte)number of input data per interrupt 
* @param[in] range_start_num      (byte)the position at which each line starts to be transmitted 
* @retval cpublk_xbyte_trans_info_wr   cpu block write transfer information
***********************************************************************/
/* Function Name : st_gdt_cpublk_xbyte_wr_gdt_nor */
static st_cpublk_xbyte_trans_info_t st_gdt_cpublk_xbyte_wr_gdt_nor (uint32_t src_addr,    uint32_t dest_addr, 
                                                                    uint32_t offset_src_addr, uint32_t data_n,
                                                                    uint32_t offset_line_src_addr,
                                                                    uint32_t range_h_byte, uint32_t range_start_num )
{
    st_cpublk_xbyte_trans_info_t cpublk_xbyte_trans_info_wr;

    uint32_t i;
    uint32_t range_h_loop;

    range_h_loop = range_start_num;

    for(i=0;i<data_n;i++)
    {
        /* CPU trans valid data */      
        *((uint8_t *)dest_addr) = *((uint8_t *)src_addr);

        /* next dest addr calculation */      
        dest_addr ++;

        /* next src addr calculation */      
        if ( range_h_loop < (range_h_byte-1))
        {
            src_addr = src_addr + offset_src_addr;
            range_h_loop++;
        }
        else
        {
            src_addr = ((src_addr + offset_line_src_addr) - range_h_byte) + 1;
            range_h_loop=0;
        }
    }
    cpublk_xbyte_trans_info_wr.range_h_byte = range_h_loop;
    cpublk_xbyte_trans_info_wr.trans_addr   = src_addr;    
    return  cpublk_xbyte_trans_info_wr;
} /* End of Function st_gdt_cpublk_xbyte_wr_gdt_nor */

/********************************************************************//**
* @brief Function of outputing data from GDT to destination addr with
         cpu byte after byte.                                           
* @param[in] src_addr              source address of input data (GDTOBUF)  
* @param[in] dest_addr             destination address of input data
* @param[in] offset_dest_addr      offset address of src_addr between first data and next
* @param[in] data_n                (byte)number of input data per interrupt
* @param[in] offset_line_dest_addr offset address of src_addr between first line data and next line 
* @param[in] range_h_byte          (byte)number of input data per interrupt 
* @param[in] range_start_num       (byte)the position at which each line starts to be transmitted 
* @retval cpublk_xbyte_trans_info_rd  cpu block read transfer information
***********************************************************************/
/* Function Name : st_gdt_cpublk_xbyte_rd_gdt_nor */
static st_cpublk_xbyte_trans_info_t st_gdt_cpublk_xbyte_rd_gdt_nor (uint32_t src_addr,    uint32_t dest_addr, 
                                                                    uint32_t offset_dest_addr, uint32_t data_n,
                                                                    uint32_t offset_line_dest_addr,
                                                                    uint32_t range_h_byte, uint32_t range_start_num )
{
    st_cpublk_xbyte_trans_info_t cpublk_xbyte_trans_info_rd;

    uint32_t i;
    uint32_t range_h_loop;

    range_h_loop = range_start_num;

    for(i=0;i<data_n;i++)
    {
        /* CPU trans valid data */      
        *((uint8_t *)dest_addr) = *((uint8_t *)src_addr);

        /* next src addr calculation */      
        src_addr++;

        /* next dest addr calculation */      
        if ( range_h_loop < (range_h_byte-1))
        {
            dest_addr = dest_addr + offset_dest_addr;
            range_h_loop++;
        }
        else
        {
            dest_addr = ((dest_addr + offset_line_dest_addr) - range_h_byte) + 1;
            range_h_loop=0;
        }
    }

    cpublk_xbyte_trans_info_rd.range_h_byte = range_h_loop;
    cpublk_xbyte_trans_info_rd.trans_addr   = dest_addr;    
    return  cpublk_xbyte_trans_info_rd;
} /* End of Function st_gdt_cpublk_xbyte_rd_gdt_nor */

/********************************************************************//**
* @brief Update the next DMAC transfer message for DMAC input
***********************************************************************/
/* Function Name : v_gdt_dmac_blk_upinf_in_array */
static void v_gdt_dmac_blk_upinf_in_array(void)
{
    if(gs_in_img_cnt >= (gdt_img_src_t.img_num-1))
    {        
        gs_in_dat_unit_cnt_h++;
        gs_in_buf_addr = GDTIBUF0;
        gs_in_img_cnt = 0;
    
        /* line trans end */
        if(gs_in_dat_unit_cnt_h >= gs_in_dat_unit_num_h)
        {    
            gs_in_dat_unit_cnt_h = 0;        /* trans end gs_in_dat_unit_cnt_v clear */
            gs_in_dat_unit_cnt_v++;          /* After the end of a line, the number of columns is incremented */
    
            /* range transfer completed */
            if(gs_in_dat_unit_cnt_v == gs_in_dat_unit_num_v)
            {
                gs_in_dat_unit_cnt_v = 0;    /* trans end gs_in_dat_unit_cnt_v clear */
            }
    
            /* range transfer transferring */
            else
            {
                for (gs_in_img_cnt=0;gs_in_img_cnt<gdt_img_src_t.img_num;gs_in_img_cnt++)
                {
                    gs_in_img_addr_array[gs_in_img_cnt] = gs_in_img_addr_line_org_array[gs_in_img_cnt] + gs_in_offset_v[gs_in_img_cnt];    /* Base address plus line offset */ 
                    gs_in_img_addr_line_org_array[gs_in_img_cnt] = gs_in_img_addr_array[gs_in_img_cnt];
                }
                gs_in_img_cnt = 0;
                gs_in_offset = gs_src_mem_size_h[gs_in_img_cnt];                
                gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt];
            }
        }
    
        /* line trans transferring */
        else
        {
            for (gs_in_img_cnt=0;gs_in_img_cnt<gdt_img_src_t.img_num;gs_in_img_cnt++)
            {
                gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt] + gs_in_offset_h;   /* Base address plus row offset */
                gs_in_img_addr_array[gs_in_img_cnt] = gs_in_img_addr;
            }
            gs_in_img_cnt = 0;
            gs_in_offset = gs_src_mem_size_h[gs_in_img_cnt];            
            gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt];
        }
    }
    else
    {
        gs_in_img_cnt++;
        gs_in_offset = gs_src_mem_size_h[gs_in_img_cnt];
        gs_in_img_addr = gs_in_img_addr_array[gs_in_img_cnt];    /* Base address plus row offset */
        gs_in_buf_addr = gs_in_buf_addr + DAT_BUF_OFFSET;  
    }

} /* End of Function v_gdt_dmac_blk_upinf_in_array */

/********************************************************************//**
* @brief Update the next DMAC block transfer message for DMAC output
***********************************************************************/
/* Function Name : v_gdt_dmac_blk_upinf_out_array */
static void v_gdt_dmac_blk_upinf_out_array(void)
{
    uint32_t timeout;
    if(gs_out_img_cnt >= (gdt_img_dest_t.img_num-1))
    {        
        st_dma_state_t state;    
        gs_out_dat_unit_cnt_h++;
        gs_out_buf_addr = GDTOBUF0;
        gs_out_img_cnt = 0;
    
        /* unit line transfer completed */
        if(gs_out_dat_unit_cnt_h >= gs_out_dat_unit_num_h)
        {   
            gs_out_dat_unit_cnt_h = 0;    /* trans end gs_ut_dat_unit_cnt_v clear */
            gs_out_dat_unit_cnt_v ++;     /* After the end of a line, the number of columns is incremented */
    
            /* range transfer completed */
            if(gs_out_dat_unit_cnt_v == gs_out_dat_unit_num_v) 
    
            /* gs_out_dat_unit_num_v=gdt_big_range_info_t.dat_unit_num_v * gs_shrnksize*/
            /* Functions other than shrinking do not require this formula */
            {
                /* wait last times DMAC trans complete */
                gsp_dmacdev_out->GetState(&state);

                /* wait DMAC trans complete. check dmac DMSTS.DTIF bit */
                timeout = SYSTEM_CFG_API_TIMEOUT_COUNT; 
                while(DMA_DTIF == (uint8_t)state.dtif_stat)
                {
                    gsp_dmacdev_out->GetState(&state);
                    __NOP();
                    if (0 == (timeout--))
                    {
                    
                      /* There is a timeout process because an infinite loop occurs 
                         depending on the optimization level. */
                      break;
                    }
                }        

                gs_out_dat_unit_cnt_v = 0;
                v_gdt_imgdata_trans_irq_disable();
                gs_big_in_range_addr_info_t.exist = RANGE_NOT_EXIST;

                /* Close DMAC function */
                if (gsp_dmacdev_in->InterruptDisable() != DMA_OK)
                {
                    gs_int_callback();
                }

                if (gsp_dmacdev_out->InterruptDisable() != DMA_OK)
                {
                    gs_int_callback();
                }

                if (gsp_dmacdev_in->Close() != DMA_OK)
                {
                    gs_int_callback();
                }

                if (gsp_dmacdev_out->Close() != DMA_OK)
                {
                    gs_int_callback();
                }

                v_gdt_off();    /* disable GDT start */
                v_gdt_seq_gen_common(gs_int_callback);
            }
    
            /* range transfer transferring */
            else
            {
                for(gs_out_img_cnt=0;gs_out_img_cnt<gdt_img_dest_t.img_num;gs_out_img_cnt++)
                {
                    gs_out_img_addr_array[gs_out_img_cnt] = gs_out_img_addr_line_org_array[gs_out_img_cnt] + gs_out_offset_v[gs_out_img_cnt];   /* Base address plus line offset */ 
                    gs_out_img_addr_line_org_array[gs_out_img_cnt] = gs_out_img_addr_array[gs_out_img_cnt];
                }
                gs_out_img_cnt = 0;
                gs_out_offset = gs_dest_mem_size_h[gs_out_img_cnt];                
                gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt];
            }
        }
    
        /* unit line transfer transferring */
        else
        {
            for(gs_out_img_cnt=0;gs_out_img_cnt<gdt_img_dest_t.img_num;gs_out_img_cnt++)
            {
                gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt] + gs_out_offset_h;    /* Base addreess plus row offset */
                gs_out_img_addr_array[gs_out_img_cnt] = gs_out_img_addr;
            }
            gs_out_img_cnt = 0;
            gs_out_offset = gs_dest_mem_size_h[gs_out_img_cnt];
            gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt];
        }
    }    
    else
    {
        gs_out_img_cnt++;
        gs_out_offset = gs_dest_mem_size_h[gs_out_img_cnt];
        gs_out_img_addr = gs_out_img_addr_array[gs_out_img_cnt];
        gs_out_buf_addr = gs_out_buf_addr + DAT_BUF_OFFSET;
    }        

} /* End of Function v_gdt_dmac_blk_upinf_out_array */

/******************************************************************************************************************//**
 * Shrink internal function
 *********************************************************************************************************************/

/* Function Name : v_gdt_cale_shrnksize_info */
/********************************************************************//**
* @brief This function applies to calculation shrink size information.
* @param[in] gdt_shrink_para_cfg     GDT shrink parameter configuration
***********************************************************************/
static void v_gdt_cale_shrnksize_info(st_gdt_shrink_para_cfg_t  * gdt_shrink_para_cfg)
{
    /* Assign parameters to global variables */
    switch(gdt_shrink_para_cfg->shrnk_size)    /* use gs_special_reg to store shrink_size */ 
    {
        case SHRNK_SIZE_1 :
            {    
            gs_special_reg = SHRNKED_SIZE_1; 
            }
        break;
        case SHRNK_SIZE_2 :
            {    
            gs_special_reg = SHRNKED_SIZE_2;
            }
        break;
        case SHRNK_SIZE_3 :
            {    
            gs_special_reg = SHRNKED_SIZE_3;
            }
        break;
        case SHRNK_SIZE_4 :
            {    
            gs_special_reg = SHRNKED_SIZE_4;
            }
        break;
        case SHRNK_SIZE_5 :
            {    
            gs_special_reg = SHRNKED_SIZE_5;
            }
        break;
        case SHRNK_SIZE_6 :
            {    
            gs_special_reg = SHRNKED_SIZE_6;
            }
        break;
        case SHRNK_SIZE_7 :
            {    
            gs_special_reg = SHRNKED_SIZE_7;
            }
        break;
        case SHRNK_SIZE_8 :
            {    
            gs_special_reg = SHRNKED_SIZE_4;
            }
        break;
        default : 
            {
            gs_special_reg = SHRNKED_SIZE_4;
            }
        break;
    }
} /* End of Function v_gdt_cale_shrnksize_info */

/* Function Name : e_gdt_judge_dest_blkend_obound */
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_src_t       GDT source image information           
* @param[in] st_img_dest_t      GDT destination image information  
* @param[in] st_blk_conv_info_t GDT block information.
* @retval GDT_OK                  successful operation                                                  
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                           
***********************************************************************/
static e_gdt_err_t e_gdt_judge_dest_blkend_obound(st_img_in_info_t* st_img_src_t, 
                                                  st_img_out_info_t* st_img_dest_t,
                                                  st_blk_conv_info_t* st_blk_conv_info_t)
{
    uint32_t blk_size_h_shrnk;  /* shrink block size of horizon */
    uint32_t blk_size_v_shrnk;  /* shrink block size of vertical */ 
    blk_size_h_shrnk = (st_blk_conv_info_t->src_size_h*gs_special_reg) >> 3;    /* use gs_special_reg to store shrink_size */ 
    blk_size_v_shrnk = (st_blk_conv_info_t->src_size_v*gs_special_reg) >> 3;

    if((st_blk_conv_info_t->start_pix_h_dest[0] + blk_size_h_shrnk) > (st_img_dest_t->size_h[0]))
    {
        return GDT_ERROR_BLK_OFF_BOUND;
    }
    if((st_blk_conv_info_t->start_pix_v_dest[0] + blk_size_v_shrnk) > (st_img_dest_t->size_v[0]))
    {
        return GDT_ERROR_BLK_OFF_BOUND;
    }
    return GDT_OK;
} /* End of Function e_gdt_judge_dest_blkend_obound */

/* Function Name : v_gdt_blk_cut_shrink */
/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range and remain range information
* @param[in] blk GDT block information
***********************************************************************/
static void v_gdt_blk_cut_shrink(st_blk_conv_info_t * blk)
{
    uint32_t block_h_byte_num;         /* horizontal byte number of conversion block(1byte)*/
    uint32_t big_range_h_num;          /* horizontal big range number(8byte)*/
    uint32_t remain_range_h_byte_num;  /* horizontal remain range byte number(1byte)*/

    block_h_byte_num = (blk->src_size_h  >> 3);
    big_range_h_num  = (block_h_byte_num >> 3);
    remain_range_h_byte_num = (block_h_byte_num & 0x00000007);

    /* big_range cut */
    /* big range horizontal number existence judgement */
    if (0 != big_range_h_num)
    {
        gdt_big_range_info_t.exist = RANGE_EXIST;
        gdt_big_range_info_t.range_fst_pix_in_h  = blk->start_pix_h_src[0];
        gdt_big_range_info_t.range_fst_pix_in_v  = blk->start_pix_v_src[0];
        gdt_big_range_info_t.range_fst_pix_out_h = blk->start_pix_h_dest[0];
        gdt_big_range_info_t.range_fst_pix_out_v = blk->start_pix_v_dest[0];
        gdt_big_range_info_t.dat_unit_x_byte     = 1;
        gdt_big_range_info_t.dat_unit_y_line     = 8;
        gdt_big_range_info_t.dat_unit_num_h      = (big_range_h_num << 3);
        gdt_big_range_info_t.dat_unit_num_v      = (blk->src_size_v >> 3);
    }

    /* remain_range cut */
    /* remain range horizontal number existence judgement */
    if (0 != remain_range_h_byte_num)
    {
        gdt_remain_range_info_t.exist = RANGE_EXIST;
        gdt_remain_range_info_t.range_fst_pix_in_h  = blk->start_pix_h_src[0]  + (big_range_h_num << 6);
        gdt_remain_range_info_t.range_fst_pix_in_v  = blk->start_pix_v_src[0];
        gdt_remain_range_info_t.range_fst_pix_out_h = blk->start_pix_h_dest[0] + (big_range_h_num << 6);
        gdt_remain_range_info_t.range_fst_pix_out_v = blk->start_pix_v_dest[0];
        gdt_remain_range_info_t.dat_unit_x_byte     = 1;
        gdt_remain_range_info_t.dat_unit_y_line     = 8;
        gdt_remain_range_info_t.dat_unit_num_h      = remain_range_h_byte_num;
        gdt_remain_range_info_t.dat_unit_num_v      = (blk->src_size_v >> 3);
    }
} /* End of Function v_gdt_blk_cut_shrink */

/* Function Name : v_gdt_shrink_cfg */
/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_shrink_para_cfg     GDT shrink parameter configuration
***********************************************************************/
static void v_gdt_shrink_cfg(st_gdt_shrink_para_cfg_t* gdt_shrink_para_cfg)
{
    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_shrink_para_cfg->iflp_en;

    /* Shrink function configuration register */
    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ    = 1; /* shrink gdtsz (8*8) GDTDSZ FIX '1'*/
    gs_gdt_resources.reg_gdt->GDTSCR_b.SHRNKEN  = VAL_ENABLE;
    gs_gdt_resources.reg_gdt->GDTSCR_b.SHRNKSZ  = gdt_shrink_para_cfg->shrnk_size;
    gs_gdt_resources.reg_gdt->GDTSCR_b.SBCS     = gdt_shrink_para_cfg->shrnk_bak_grnd_colr;
} /* End of Function v_gdt_shrink_cfg */

/* Function Name : v_gdt_shrink_num_cfg */
/********************************************************************//**
* @brief GDT function register configuration
* @param[in] shrinknum shrink register set parameter
***********************************************************************/
static void v_gdt_shrink_num_cfg(uint8_t shrinknum)
{
    uint32_t shrink_num_setting;
    shrink_num_setting = shrinknum - 1;

    gs_gdt_resources.reg_gdt->GDTSCR_b.SHRNKNUM = shrink_num_setting;
} /* End of Function v_gdt_shrink_num_cfg */

/* Function Name : v_gdt_calc_trans_info */
/********************************************************************//**
* @brief This function applies to calculation  block into GDT
    conversion range and update big range and remain range information.
***********************************************************************/
static void v_gdt_calc_trans_info(void)
{
    /* Defintion of intermediate variables */
    uint16_t fst_pix_h;    
    uint16_t fst_pix_v;
    uint32_t line_start_addr;
    uint32_t addr_offset;  

    uint16_t remainout_dat_unit_x_bit;  
    uint8_t  remainout_dat_unit_x_byte;
    
    /* big range process */
    if (RANGE_EXIST == gdt_big_range_info_t.exist)
    {
        /* big range input VAR */    
        /* 1 line byte number x lines number + pix start offset */
        fst_pix_h = gdt_big_range_info_t.range_fst_pix_in_h;
        fst_pix_v = gdt_big_range_info_t.range_fst_pix_in_v;
        line_start_addr = fst_pix_v * gs_src_mem_size_h[0];
        addr_offset = line_start_addr + (fst_pix_h >> 3);

        gs_big_in_range_addr_info_t.exist              = gdt_big_range_info_t.exist;
        gs_big_in_range_addr_info_t.range_fst_addr[0]  = gdt_img_src_t.start_addr[0] + addr_offset;
        gs_big_in_range_addr_info_t.dat_unit_x_byte = gdt_big_range_info_t.dat_unit_x_byte;
        gs_big_in_range_addr_info_t.dat_unit_y_line = gdt_big_range_info_t.dat_unit_y_line;
        gs_big_in_range_addr_info_t.dat_unit_num_h  = gdt_big_range_info_t.dat_unit_num_h;
        gs_big_in_range_addr_info_t.dat_unit_num_v  = gdt_big_range_info_t.dat_unit_num_v;

        gs_in_offset_v[0]  = (gs_src_mem_size_h[0] * gdt_big_range_info_t.dat_unit_y_line);

        /* big range output VAR */    
        fst_pix_h = gdt_big_range_info_t.range_fst_pix_out_h;
        fst_pix_v = gdt_big_range_info_t.range_fst_pix_out_v;
        line_start_addr = fst_pix_v * gs_dest_mem_size_h[0];
        addr_offset = line_start_addr + (fst_pix_h >> 3);    

        gs_big_out_range_addr_info_t.exist              = gdt_big_range_info_t.exist;
        gs_big_out_range_addr_info_t.range_fst_addr[0]  = gdt_img_dest_t.start_addr[0] + addr_offset;
        gs_big_out_range_addr_info_t.dat_unit_x_byte = gs_special_reg;    /* use gs_special_reg to store shrink_size */ 
        gs_big_out_range_addr_info_t.dat_unit_y_line = 1;
        gs_big_out_range_addr_info_t.dat_unit_num_h  = gdt_big_range_info_t.dat_unit_num_h >> 3;
        gs_big_out_range_addr_info_t.dat_unit_num_v  = gdt_big_range_info_t.dat_unit_num_v * gs_special_reg;
    }
    else
    {  
        gs_big_out_range_addr_info_t.dat_unit_num_h = 0;
        fst_pix_h = gdt_remain_range_info_t.range_fst_pix_out_h;
        fst_pix_v = gdt_remain_range_info_t.range_fst_pix_out_v;
        line_start_addr = fst_pix_v * gs_dest_mem_size_h[0];
        addr_offset = line_start_addr + (fst_pix_h >> 3);    
        gs_big_out_range_addr_info_t.range_fst_addr[0] = gdt_img_dest_t.start_addr[0] + addr_offset;
    }

    /* remain range proces */
    if (RANGE_EXIST == gdt_remain_range_info_t.exist)
    {
        /* remain range input VAR */    
        fst_pix_h = gdt_remain_range_info_t.range_fst_pix_in_h;
        fst_pix_v = gdt_remain_range_info_t.range_fst_pix_in_v;
        line_start_addr = fst_pix_v * gs_src_mem_size_h[0];
        addr_offset = line_start_addr + (fst_pix_h >> 3);

        gs_remain_in_range_addr_info_t.exist              = gdt_remain_range_info_t.exist;
        gs_remain_in_range_addr_info_t.range_fst_addr[0]  = gdt_img_src_t.start_addr[0] + addr_offset;
        gs_remain_in_range_addr_info_t.dat_unit_x_byte = gdt_remain_range_info_t.dat_unit_x_byte;
        gs_remain_in_range_addr_info_t.dat_unit_y_line = gdt_remain_range_info_t.dat_unit_y_line;
        gs_remain_in_range_addr_info_t.dat_unit_num_h  = gdt_remain_range_info_t.dat_unit_num_h;
        gs_remain_in_range_addr_info_t.dat_unit_num_v  = gdt_remain_range_info_t.dat_unit_num_v;

        gs_in_offset_v[0]  = (gs_src_mem_size_h[0] * gdt_remain_range_info_t.dat_unit_y_line);   

        /* remain range output VAR */    
        addr_offset = (gs_big_out_range_addr_info_t.dat_unit_num_h * gs_special_reg);
        remainout_dat_unit_x_bit = gdt_remain_range_info_t.dat_unit_num_h * gs_special_reg;
        
        /* Determine the valid num when after shrnk */
        if(0 != (remainout_dat_unit_x_bit & 0x00000007))
        {
            remainout_dat_unit_x_byte = (remainout_dat_unit_x_bit >>3) + 1;
        }
        else
        {
            remainout_dat_unit_x_byte = (remainout_dat_unit_x_bit >>3);
        }

        /* remain range output VAR */         
        gs_remain_out_range_addr_info_t.exist              = gdt_remain_range_info_t.exist;
        gs_remain_out_range_addr_info_t.range_fst_addr[0]  = gs_big_out_range_addr_info_t.range_fst_addr[0] + addr_offset;
        gs_remain_out_range_addr_info_t.dat_unit_x_byte = remainout_dat_unit_x_byte;
        gs_remain_out_range_addr_info_t.dat_unit_y_line = 1;
        gs_remain_out_range_addr_info_t.dat_unit_num_h  = 1;
        gs_remain_out_range_addr_info_t.dat_unit_num_v  = gdt_remain_range_info_t.dat_unit_num_v * gs_special_reg;   
    }      
    gs_out_offset_v[0] = (gs_dest_mem_size_h[0] * gs_special_reg);     
} /* End of Function v_gdt_calc_trans_info */

/* Function Name : v_gdt_seq_gen_shrink */
/********************************************************************//**
* @brief This function processes the conversion block in the set order.
         This  function is for Shrink 
* @param[in] p_callback    API call back function
***********************************************************************/
static void v_gdt_seq_gen_shrink(gdt_cb_event_t const p_callback )
{
    uint8_t remain_shrnk_num = 0;

    /* Remain area do not exist */
    if(RANGE_NOT_EXIST == gs_remain_in_range_addr_info_t.exist)
    {
        /* big area transmit */
        if(RANGE_EXIST == gs_big_in_range_addr_info_t.exist) /* big range transmit */
        {
            v_gdt_seq_shrnk_big_trans(p_callback); 

            /* GDT set and GDT srart (function) */
            v_gdt_shrink_num_cfg(SHRNK_NUM_8);
            v_gdt_imgdata_trans_irq_enable();
            v_gdt_on();
        }

        /* shrink transmit finished, execute callback */
        else
        {
            /* clear pending the interrupt status */
            R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

            /* clear pending the interrupt status */
            R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

            /* clear the interrupt status */
            R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

            /* clear the interrupt status */
            R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);    

            /* disable GDT input interrupt */
            R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

            /* disable GDT output interrupt */
            R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

            /* disable GDT input interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);

            /* disable GDT output interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

            /* when shrink finish, then execute callback */
            p_callback();
        }
    }

    /* remain range transmit firstly */
    else 
    {
        v_gdt_seq_shrnk_remain_trans(p_callback); 

        /* GDT set and GDT srart  */
        remain_shrnk_num = gs_remain_in_range_addr_info_t.dat_unit_num_h;
        v_gdt_shrink_num_cfg(remain_shrnk_num);
        v_gdt_imgdata_trans_irq_enable();
        v_gdt_on();            
    }

} /* End of Function v_gdt_seq_gen_shrink */

/* Function Name : v_gdt_seq_shrnk_big_trans */
/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the set order.
* @param[in] p_callback    API call back function
***********************************************************************/
static void v_gdt_seq_shrnk_big_trans(gdt_cb_event_t const p_callback )
{
    /* global variable assign */
    gs_in_img_cnt  = 0;
    gs_out_img_cnt = 0;
    gs_in_dat_unit_cnt_h    = 1;
    gs_in_dat_unit_cnt_v    = 0;
    gs_in_img_addr_line_org_array[0] = gs_big_in_range_addr_info_t.range_fst_addr[0];
    gs_in_offset_h          = gs_big_in_range_addr_info_t.dat_unit_x_byte;
    gs_in_dat_unit_num_h    = gs_big_in_range_addr_info_t.dat_unit_num_h;
    gs_in_dat_unit_num_v    = gs_big_in_range_addr_info_t.dat_unit_num_v;
    gs_in_img_addr          = gs_big_in_range_addr_info_t.range_fst_addr[0];
    gs_in_buf_addr          = GDTIBUF0;
    gs_in_offset            = gs_src_mem_size_h[0]; 
    gs_in_img_addr_array[0] = gs_big_in_range_addr_info_t.range_fst_addr[0];

    gs_out_dat_unit_cnt_h    = 0;
    gs_out_dat_unit_cnt_y    = 0;
    gs_out_dat_unit_cnt_v    = 0;
    gs_out_img_addr_line_org_array[0] = gs_big_out_range_addr_info_t.range_fst_addr[0];
    gs_out_offset_h          = gs_big_out_range_addr_info_t.dat_unit_x_byte;
    gs_out_dat_unit_num_h    = gs_big_out_range_addr_info_t.dat_unit_num_h;
    gs_out_dat_unit_num_v    = gs_big_out_range_addr_info_t.dat_unit_num_v;
    gs_out_img_addr          = gs_big_out_range_addr_info_t.range_fst_addr[0];
    gs_out_img_addr_unit_line_org = gs_out_img_addr;
    gs_out_buf_addr          = GDTOBUF0;
    gs_out_offset            = gs_dest_mem_size_h[0]; 
    gs_out_img_addr_array[0] = gs_big_out_range_addr_info_t.range_fst_addr[0];

    /* input block(8x8) x 8times, but output only 1byte */
    if((1 == gs_special_reg)    /* use gs_special_reg to store shrink_size */
        && (1 == gs_big_out_range_addr_info_t.dat_unit_num_v)
        && (1 ==gs_big_out_range_addr_info_t.dat_unit_num_h))
    {
        /* DMAC input 1st trans */ 
        v_gdt_dmacin_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                             gs_gdt_unit_size, gs_src_mem_size_h[0], gs_in_img_addr, v_isr_dmac_input, p_callback);

        /* cpu ignore DMAC trans end IRQ call v_isr_dmac_out_once_shk*/
        v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_INCR, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                             gs_special_reg, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_output_once, p_callback);
    }

    /* other situation */ 
    else
    {
        /* DMAC input and output 1st trans */ 
        v_gdt_dmacin_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                             gs_gdt_unit_size, gs_src_mem_size_h[0], gs_in_img_addr, v_isr_dmac_input, p_callback);
        v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_INCR, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                              gs_special_reg, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_shk_output, p_callback);

        /* calculation DMAC 2nd output IMG address */
        /* only output once in 1 line, next DMAC trans will to next line */
        if((1 == gs_special_reg)&&(1==gs_big_out_range_addr_info_t.dat_unit_num_h))
        {
            gs_out_img_addr = gs_out_img_addr_line_org_array[0] + gs_out_offset_v[0];   /* Base address plus line offset */ 
            gs_out_img_addr_line_org_array[0] = gs_out_img_addr;
            gs_out_img_addr_unit_line_org  = gs_out_img_addr;

            gs_out_dat_unit_cnt_h    = 0;
            gs_out_dat_unit_cnt_y    = 0;
            gs_out_dat_unit_cnt_v    = 1;
            gs_out_buf_addr          = GDTOBUF0;
        }

        /* 1 Unit trans end, next DMAC will to next unit in the same line */
        else if (1 == gs_special_reg)
        {
            gs_out_img_addr = gs_out_img_addr_unit_line_org + gs_out_offset_h;  /* Base address plus row offset */
            gs_out_img_addr_unit_line_org = gs_out_img_addr;
            gs_out_img_addr_array[0]      = gs_out_img_addr_unit_line_org  + gs_out_offset_h; 
            gs_out_dat_unit_cnt_y         = 0;
            gs_out_buf_addr               = GDTOBUF0;
            gs_out_dat_unit_cnt_h++;
        }

        /* normal situation */
        else
        {
            gs_out_img_addr = gs_out_img_addr + gs_dest_mem_size_h[0]; /* memory size */

            /* calculation GDT OBUF address,plus 128bit */
            gs_out_buf_addr = gs_out_buf_addr + 0x10;
            gs_out_dat_unit_cnt_y++;
        }
    }

    /* calculation DMAC 2nd input IMG address */
    gs_in_img_addr = gs_in_img_addr + gs_in_offset_h;          /* Base address plus line offset */
    gs_in_img_addr_array[0] = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h; 

} /* End of Function v_gdt_seq_shrnk_big_trans */        

/* Function Name : v_gdt_seq_shrnk_remain_trans */
/********************************************************************//**
* @brief This function processes remain area of the conversion block 
*        in the set order.
* @param[in] p_callback    API call back function
***********************************************************************/
static void v_gdt_seq_shrnk_remain_trans(gdt_cb_event_t const p_callback )
{
    /* global variable assign */
    gs_in_img_cnt  = 0;
    gs_out_img_cnt = 0;
    gs_in_dat_unit_cnt_h    = 0;
    gs_in_dat_unit_cnt_v    = 0;
    gs_in_img_addr_line_org_array[0] = gs_remain_in_range_addr_info_t.range_fst_addr[0];
    gs_in_offset_h          = gs_remain_in_range_addr_info_t.dat_unit_x_byte;
    gs_in_dat_unit_num_h    = gs_remain_in_range_addr_info_t.dat_unit_num_h;
    gs_in_dat_unit_num_v    = gs_remain_in_range_addr_info_t.dat_unit_num_v;
    gs_in_img_addr          = gs_remain_in_range_addr_info_t.range_fst_addr[0];
    gs_in_buf_addr          = GDTIBUF0;
    gs_in_offset            = gs_src_mem_size_h[0]; 
    gs_in_img_addr_array[0] = gs_big_in_range_addr_info_t.range_fst_addr[0];

    gs_out_dat_unit_cnt_h    = 0;
    gs_out_dat_unit_cnt_y    = 0;
    gs_out_dat_unit_cnt_v    = 0;
    gs_out_img_addr_line_org_array[0] = gs_remain_out_range_addr_info_t.range_fst_addr[0];
    gs_out_offset_h          = gs_remain_out_range_addr_info_t.dat_unit_x_byte;
    gs_out_dat_unit_num_h    = gs_remain_out_range_addr_info_t.dat_unit_num_h;
    gs_out_dat_unit_num_v    = gs_remain_out_range_addr_info_t.dat_unit_num_v;
    gs_out_img_addr          = gs_remain_out_range_addr_info_t.range_fst_addr[0];
    gs_out_img_addr_unit_line_org = gs_out_img_addr;
    gs_out_buf_addr          = GDTOBUF0;
    
    /* enable GDT output interrupt trigger CPU  */
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, GDT_OUT_IELS_VALUE, v_isr_cpu_output_limit);  
    
    /* only input 1 byte * 8 line */ 
    if((1 == gs_remain_in_range_addr_info_t.dat_unit_num_h)
       && (1 == gs_remain_in_range_addr_info_t.dat_unit_num_v))  
    {
        /* DMAC input data */
        /* disenable GDT interrupt trigger DMAC */
        v_gdt_dmacin_set(DMA_MODE_BLOCK,DMA_SIZE_BYTE, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC, 
                         gs_gdt_unit_size, gs_src_mem_size_h[0],gs_remain_in_range_addr_info_t.range_fst_addr[0],
                         p_callback);

        /* CPU output data */
        /* enable interrupt */
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        gs_in_dat_unit_cnt_v = 0;  /* trans end gs_in_dat_unit_cnt_v clear */
    }

    /* other situation */ 
    else  
    {
        /* DMAC input data */
        /* DMAC pre set function */
        /* enable GDT interrupt trigger DMAC */
        v_gdt_dmacin_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                             gs_gdt_unit_size, gs_src_mem_size_h[0], gs_remain_in_range_addr_info_t.range_fst_addr[0],
                             v_isr_dmac_input ,p_callback);

        /* CPU output data */
        /* enable interrupt */
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);
    
        /* calculation DMAC 2nd input IMG address */
        /* input 1 byte but more than 8 line */
        if (1 == gs_remain_in_range_addr_info_t.dat_unit_num_h)
        {
            gs_in_img_addr =  gs_in_img_addr_line_org_array[0] + gs_in_offset_v[0];  /* gs_out_offset_v /h_size_byte * y_line */ 
            gs_in_img_addr_line_org_array[0] = gs_in_img_addr;
            gs_in_dat_unit_cnt_h = 0;   /* trans end gs_out_dat_unit_cnt_h clear */
            gs_in_dat_unit_cnt_v++;     /* After the end of a line, the number of columns is incremented */
        }

        /* normal situation */
        else
        {
           gs_in_img_addr = gs_in_img_addr + gs_in_offset_h;            /* Base address plus line offset */
           gs_in_img_addr_array[0] = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h; 
           gs_in_dat_unit_cnt_h++;
        }            
    }

} /* End of Function v_gdt_seq_shrnk_remain_trans */

/********************************************************************//**
* @brief Interrupt handler for dmac output complete request
***********************************************************************/
/* Function Name : v_isr_dmac_shk_output */
static void v_isr_dmac_shk_output (void)
{
    /* update address and DMAC enable */   
    *gsp_dmac_reg_out_dar    = gs_out_img_addr; 
    *gsp_dmac_reg_out_sar    = gs_out_buf_addr; 
    *gsp_dmac_reg_out_crb    = 0x01;            
    *gsp_dmac_reg_out_cnt    = VAL_ENABLE;          

    /* DMAC FUNCTION */
    /* update next times output DMAC trans address and DMAC trans end disable */
    v_gdt_shk_dmac_update_out_info();
} /* End of Function v_isr_dmac_shk_output */

/********************************************************************//**
* @brief Interrupt handler for GDT output request
***********************************************************************/
/* Function Name : v_isr_cpu_output_limit */
static void v_isr_cpu_output_limit (void)
{
    /* CPU FUNCTION */  
    v_gdt_cpuline_byte_rd_gdt_limit(gs_out_buf_addr,gs_out_img_addr,
                                    gs_remain_out_range_addr_info_t.dat_unit_x_byte,
                                    gs_special_reg);    /* use gs_special_reg to store shrink_size */

    /* update next times CPU trans address and CPU trans end disable */
    v_gdt_cpu_update_out_limit_info();
} /* End of Function v_isr_cpu_output_limit */

/********************************************************************//**
* @brief Update the next DMAC transfer message for DMAC output
***********************************************************************/
/* Function Name : v_gdt_shk_dmac_update_out_info */
static void v_gdt_shk_dmac_update_out_info(void)
{
    st_dma_state_t state;    
    gs_out_dat_unit_cnt_y++;
    uint32_t timeout;

    /* unit trans end */
    if(gs_out_dat_unit_cnt_y >= gs_special_reg)      /* use gs_special_reg to store shrink_size */
    {
        gs_out_dat_unit_cnt_y = 0;
        gs_out_dat_unit_cnt_h++;
        gs_out_buf_addr = GDTOBUF0;

        /* unit line transfer completed */
        if(gs_out_dat_unit_cnt_h >= gs_out_dat_unit_num_h)
        {   
            gs_out_dat_unit_cnt_h = 0;               /* trans end gs_ut_dat_unit_cnt_v clear */
            gs_out_dat_unit_cnt_v += gs_special_reg; /* After the end of a line, the number of columns is incremented */

            /* range transfer completed */
            if(gs_out_dat_unit_cnt_v == gs_out_dat_unit_num_v)
            {
                /* wait last times DMAC trans complete */
                gsp_dmacdev_out->GetState(&state);

                /* wait DMAC trans complete. check dmac DMSTS.DTIF bit */
                timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;      
                while(DMA_DTIF == (uint8_t)state.dtif_stat)
                {
                    gsp_dmacdev_out->GetState(&state);
                    __NOP();
                    if (0 == (timeout--))
                    {
                    
                      /* There is a timeout process because an infinite loop occurs 
                        depending on the optimization level. */
                      break;
                    }
                }        

                gs_out_dat_unit_cnt_v = 0;
                gs_big_in_range_addr_info_t.exist = RANGE_NOT_EXIST;
                gdt_big_range_info_t.exist        = RANGE_NOT_EXIST;

                if (gsp_dmacdev_in->InterruptDisable() != DMA_OK)
                {
                    gs_int_callback();
                }

                if (gsp_dmacdev_out->InterruptDisable() != DMA_OK)
                {
                    gs_int_callback();
                }

                if (gsp_dmacdev_in->Close() != DMA_OK)
                {
                    gs_int_callback();
                }

                if (gsp_dmacdev_out->Close() != DMA_OK)
                {
                    gs_int_callback();
                }
                v_gdt_imgdata_trans_irq_disable();
                v_gdt_off();    /* disable GDT start */
                v_gdt_seq_gen_shrink(gs_int_callback);
            }

            /* range transfer transferring */
            else
            {
                gs_out_img_addr = gs_out_img_addr_line_org_array[0] + gs_out_offset_v[0];   /* Base address plus line offset */ 
                gs_out_img_addr_line_org_array[0] = gs_out_img_addr;
                gs_out_img_addr_unit_line_org     = gs_out_img_addr;
            }
        }

        /* unit line transfer transferring */
        else
        {
            gs_out_img_addr = gs_out_img_addr_unit_line_org + gs_out_offset_h;  /* Base address plus row offset */
            gs_out_img_addr_unit_line_org = gs_out_img_addr;
        }
    }

    /* unit trans transferring */
    else 
    { 
        gs_out_img_addr = gs_out_img_addr + gs_dest_mem_size_h[0]; /* memory size */

        /* calculation GDT OBUF address,plus 128bit */
        gs_out_buf_addr = gs_out_buf_addr + 0x10;
    }
} /* End of Function v_gdt_shk_dmac_update_out_info */

/********************************************************************//**
* @brief Function of outputing data from GDT to destination addr with
     cpu byte after byte,the invalid data must be read
* @param[in] src_addr       source address of output data(GDTOBUF)
* @param[in] dest_addr      destination address of output data
* @param[in] valid_data_num valid number of input data
* @param[in] total_data_num total number of input data
***********************************************************************/
/* Function Name : v_gdt_cpuline_byte_rd_gdt_limit */
static void v_gdt_cpuline_byte_rd_gdt_limit (uint32_t src_addr,uint32_t dest_addr, 
                                             uint32_t valid_data_num,uint32_t total_data_num)
{
    volatile uint32_t p_ptr_invalid;
    uint32_t i;
    
    for(i=0;i<total_data_num;i++) /* total data */
    {
        if(i<valid_data_num)      /* valid data */
        {
          /* CPU trans valid data */      
          *((uint8_t *)dest_addr) = *((uint8_t *)src_addr);
          dest_addr ++;
        }
        else
        {
          /* CPU trans invalid data */      
           p_ptr_invalid = *((uint8_t *)src_addr);
        }
        src_addr ++;
    }

} /* End of Function v_gdt_cpuline_byte_rd_gdt_limit */

/********************************************************************//**
* @brief Update the next CPU transfer message for CPU output
*        only for Remain range transmit
***********************************************************************/
/* Function Name : v_gdt_cpu_update_out_limit_info */
static void v_gdt_cpu_update_out_limit_info(void)
{
    gs_out_dat_unit_cnt_y++;

    /* y line trans end*/
    if(gs_out_dat_unit_cnt_y >= gs_special_reg)    /* use gs_special_reg to store shrink_size */
    {
        gs_out_dat_unit_cnt_y = 0;
        gs_out_dat_unit_cnt_h++;
        gs_out_buf_addr = GDTOBUF0;

        /* line transfer completed */
        if(gs_out_dat_unit_cnt_h >= gs_out_dat_unit_num_h)
        {   
            gs_out_dat_unit_cnt_h = 0;                /* trans end gs_ut_dat_unit_cnt_h clear */
            gs_out_dat_unit_cnt_v += gs_special_reg;  /* After the end of a line, the number of columns is incremented */

            /* range transfer completed */
            if(gs_out_dat_unit_cnt_v == gs_out_dat_unit_num_v)
            {
                gs_out_dat_unit_cnt_v = 0;
                gs_remain_in_range_addr_info_t.exist  = RANGE_NOT_EXIST;
                gs_remain_out_range_addr_info_t.exist = RANGE_NOT_EXIST;
                gdt_remain_range_info_t.exist         = RANGE_NOT_EXIST;

                /* disable GDT output interrupt */
                R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

                /* clear GDT output interrupt flag */
                R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

                /* close cpu output data IRQ */ 
                R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI); 

                /* disable GDT output interrupt trigger CPU  */
                R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

                if (gsp_dmacdev_in->InterruptDisable() != DMA_OK)
                {
                    gs_int_callback();
                }

                if (gsp_dmacdev_in->Close() != DMA_OK)
                {
                    gs_int_callback();
                }

                v_gdt_imgdata_trans_irq_disable();
                v_gdt_off();                                                    /* disable GDT start */ 
                v_gdt_seq_gen_shrink(gs_int_callback);
            }

            /* range transferring */
            else
            {
                gs_out_img_addr = gs_out_img_addr_line_org_array[0] + gs_out_offset_v[0];   /* Base address plus line offset */ 
                gs_out_img_addr_line_org_array[0] = gs_out_img_addr;
                gs_out_img_addr_unit_line_org     = gs_out_img_addr;
            }
        }

        /* line transeferring */
        else
        {
            gs_out_img_addr = gs_out_img_addr_unit_line_org + gs_out_offset_h;  /* Base address plus row offset */
            gs_out_img_addr_unit_line_org = gs_out_img_addr;
        }
    }

    /* unit transferring */
    else
    {
        /* calculation memory address */
        gs_out_img_addr = gs_out_img_addr + gs_dest_mem_size_h[0]; 

        /* calculation GDT OBUF address plus 128 bit */
        gs_out_buf_addr = gs_out_buf_addr + 0x10;
    }
} /* End of Function v_gdt_cpu_update_out_limit_info */

/******************************************************************************************************************//**
 * Endian internal function
 *********************************************************************************************************************/

/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_endian_para_cfg     GDT endian parameter configuration 
***********************************************************************/
/* Function Name : v_gdt_endian_cfg */
static void v_gdt_endian_cfg(st_gdt_endian_para_cfg_t* gdt_endian_para_cfg)
{
    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ = 0; /* endian gdtsz(16*16) GDTDSZ FIX '0' */

    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_endian_para_cfg->iflp_en;

    /* endian function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.ENDIAN = gdt_endian_para_cfg->endian;
} /* End of Function v_gdt_endian_cfg */

/******************************************************************************************************************//**
 * Monochrome internal function
 *********************************************************************************************************************/

/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_monochrome_para_cfg     GDT monochrome parameter configuration 
***********************************************************************/
/* Function Name : v_gdt_monochrome_cfg */
static void v_gdt_monochrome_cfg(st_gdt_monochrome_para_cfg_t* gdt_monochrome_para_cfg)
{
    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_monochrome_para_cfg->iflp_en;

    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ = gdt_monochrome_para_cfg->gdtdsz;

    /* monochrome priority color specification register */
    gs_gdt_resources.reg_gdt->GDTCR_b.MPCS   = gdt_monochrome_para_cfg->mpcs;

    /* monochrome border register */
    gs_gdt_resources.reg_gdt->GDTCR_b.MBRDEN = gdt_monochrome_para_cfg->mbrden;

    /* monochrome function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.MSYNEN = VAL_ENABLE;
} /* End of Function v_gdt_monochrome_cfg */

/******************************************************************************************************************//**
  Rotate internal function
 *********************************************************************************************************************/
 
/********************************************************************//**
* @brief Determining a processing block does not to beyond the boundary for rotate function
* @param[in] rttfc                rotation direction select register
* @param[in] st_img_src_t         GDT source image information           
* @param[in] st_img_dest_t        GDT destination image information  
* @param[in] st_blk_conv_info_t   GDT block information.
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
***********************************************************************/
/* Function Name : e_gdt_judge_dest_rtt_blkend_ob */
static e_gdt_err_t e_gdt_judge_dest_rtt_blkend_ob(uint8_t rttfc,
                                                     st_img_in_info_t* st_img_src_t, 
                                                     st_img_out_info_t* st_img_dest_t,
                                                     st_blk_conv_info_t* st_blk_conv_info_t)
{
    uint32_t valid_range_h;
    uint32_t valid_range_v;
    uint32_t blk_size_h_temp;
    uint32_t blk_size_v_temp;
    uint32_t blk_size_h;
    uint32_t blk_size_v;

    e_gdt_err_t ret = GDT_OK;

    blk_size_h_temp = st_blk_conv_info_t->src_size_h;
    blk_size_v_temp = st_blk_conv_info_t->src_size_v;

    if((RTTFC_RIGHT90 == gs_special_reg) || (RTTFC_LEFT90 == gs_special_reg)) /* right90 or left90 mode calculate the size of block area  */
    {
        /* Calculate the block size */
        if(blk_size_h_temp > blk_size_v_temp)
        {
            blk_size_h = blk_size_h_temp; /* block size of h */
            blk_size_v = blk_size_h_temp; /* block size of v */
        }
        else if(blk_size_h_temp < blk_size_v_temp)
        {
            blk_size_h = blk_size_v_temp; /* block size of h */
            blk_size_v = blk_size_v_temp; /* block size of v */
        }
        else
        {
            blk_size_h = blk_size_h_temp; /* block size of h */
            blk_size_v = blk_size_v_temp; /* block size of v */
        }
    }
    else /* up_down or left_right mode calculate the size of block  */
    {
        blk_size_h = blk_size_h_temp; /* block size of h */
        blk_size_v = blk_size_v_temp; /* block size of v */
    }

    if((0 == rttfc) || (1 == rttfc))
    {       
        /* rotate right 90 or left 90 */    
        valid_range_h = st_img_dest_t->size_h[0] - st_blk_conv_info_t->start_pix_h_dest[0]; /* calculate the valid range of H after rotate right 90 */
        valid_range_v = st_img_dest_t->size_v[0] - st_blk_conv_info_t->start_pix_v_dest[0]; /* calculate the valid range of V after rotate right 90 */
    
        if((blk_size_h) > (valid_range_h))
        {
            ret =  GDT_ERROR_BLK_OFF_BOUND;
        }
        if((blk_size_v) > (valid_range_v))
        {
            ret =  GDT_ERROR_BLK_OFF_BOUND;
        }
    }
    
    else
    {
        /* rotate up-down or left-right*/    
        if((st_blk_conv_info_t->start_pix_h_dest[0] + st_blk_conv_info_t->src_size_h) > (st_img_dest_t->size_h[0]))
        {
            ret =  GDT_ERROR_BLK_OFF_BOUND;
        }
        if((st_blk_conv_info_t->start_pix_v_dest[0] + st_blk_conv_info_t->src_size_v) > (st_img_dest_t->size_v[0]))
        {
            ret =  GDT_ERROR_BLK_OFF_BOUND;
        }
    }    
    return ret;
} /* End of Function e_gdt_judge_dest_rtt_blkend_ob */

/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_rotate_para_cfg     GDT rotate parameter configuration  
***********************************************************************/
/* Function Name : v_gdt_rotate_cfg */
static void v_gdt_rotate_cfg(st_gdt_rotate_para_cfg_t* gdt_rotate_para_cfg)
{
    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_rotate_para_cfg->iflp_en;

    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ = gdt_rotate_para_cfg->gdtdsz;

    /* rotate function select register */
    gs_gdt_resources.reg_gdt->GDTCR_b.RTTFC  = gdt_rotate_para_cfg->rttfc;

    /* rotate function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.RTTEN  = VAL_ENABLE;

} /* End of Function v_gdt_rotate_cfg */

/********************************************************************//**
* @brief This function processes the conversion block in the set order.
         This  function is for rotate
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_gen_rotate */
static void v_gdt_seq_gen_rotate( gdt_cb_event_t const p_callback )
{
    /* function of rotate sequence */
    /* big area transmit */
    if(RANGE_EXIST == gs_big_in_range_addr_info_t.exist)
    {
        /* block transfer(2byte * 16line or 1 byte * 8line) */
        v_gdt_seq_rotate_big_trans_blk(gs_gdt_unit_size,p_callback); /* gs_gdt_unit_size = 8*8 16*16 */ 

        /* GDT set and GDT srart (function) */
        v_gdt_imgdata_trans_irq_enable();
        v_gdt_on();
    }

    /* rotate transmit finished, execute callback */
    else
    {
        /* clear pending the interrupt status */
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* clear pending the interrupt status */
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* clear the interrupt status */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* clear the interrupt status */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);    

        /* disable GDT input interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* disable GDT output interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* disable GDT input interrupt trigger CPU */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);

        /* disable GDT output interrupt trigger CPU */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

        /* when endian finish, then execute callback */
        p_callback();
    }

} /* End of Function v_gdt_seq_gen_rotate */

/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the set order.
* @param[in] data_length        data length
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_rotate_big_trans_blk */
static void v_gdt_seq_rotate_big_trans_blk(uint32_t data_length, gdt_cb_event_t const p_callback)
{
    uint32_t input_coodinate_h;
    uint32_t input_coodinate_v;
    uint32_t output_coodinate_h;
    uint32_t output_coodinate_v;
    uint32_t unit_num_h;
    uint32_t unit_num_v;
    uint32_t unit_addr;
    st_unit_output_coodinate_t unit_output_coodinate;

    gs_in_dat_unit_cnt_h    = 1;
    gs_in_dat_unit_cnt_v    = 0;
    gs_in_img_addr_line_org_array[0] = gs_big_in_range_addr_info_t.range_fst_addr[0];
    gs_in_offset_h          = gs_big_in_range_addr_info_t.dat_unit_x_byte;
    gs_in_dat_unit_num_h    = gs_big_in_range_addr_info_t.dat_unit_num_h;
    gs_in_dat_unit_num_v    = gs_big_in_range_addr_info_t.dat_unit_num_v;
    gs_in_img_addr          = gs_big_in_range_addr_info_t.range_fst_addr[0]; /* block addr of first image */
    gs_in_buf_addr          = GDTIBUF0;
    gs_in_offset            = gs_src_mem_size_h[0]; 
    gs_out_dat_unit_cnt_h   = 0;
    gs_out_dat_unit_cnt_y   = 0;
    gs_out_dat_unit_cnt_v   = 0;
    gs_out_offset_h         = gs_big_out_range_addr_info_t.dat_unit_x_byte;
    gs_out_dat_unit_num_h   = gs_big_out_range_addr_info_t.dat_unit_num_h;
    gs_out_dat_unit_num_v   = gs_big_out_range_addr_info_t.dat_unit_num_v;
    gs_out_buf_addr         = GDTOBUF0;

    /**calculate 1st gs_out_img_addr */
    input_coodinate_h = gs_out_dat_unit_cnt_h ;
    input_coodinate_v = gs_out_dat_unit_cnt_v ;

    unit_num_h = gs_big_out_range_addr_info_t.dat_unit_num_h; /* block unit number of h */
    unit_num_v = gs_big_out_range_addr_info_t.dat_unit_num_v; /* block unit number of v */

    if(gs_special_reg < RTTFC_UP_DOWN) /* right90 or left90 mode calculate the size of rotation area  */
    {
        /* Calculate the size of the rotation area based on the block size */
        if(unit_num_h > unit_num_v)
        {
            gs_big_out_range_addr_info_t.dat_unit_num_h = unit_num_h; /* block unit number of h */
            gs_big_out_range_addr_info_t.dat_unit_num_v = unit_num_h; /* block unit number of v */
        }
        else if(unit_num_h < unit_num_v)
        {
            gs_big_out_range_addr_info_t.dat_unit_num_h = unit_num_v; /* block unit number of h */
            gs_big_out_range_addr_info_t.dat_unit_num_v = unit_num_v; /* block unit number of v */
        }
        else
        {
            gs_big_out_range_addr_info_t.dat_unit_num_h = unit_num_h; /* block unit number of h */
            gs_big_out_range_addr_info_t.dat_unit_num_v = unit_num_v; /* block unit number of v */
        }
    }
    else /* up_down or left_right mode calculate the size of rotation area  */
    {
        gs_big_out_range_addr_info_t.dat_unit_num_h = unit_num_h; /* block unit number of h */
        gs_big_out_range_addr_info_t.dat_unit_num_v = unit_num_v; /* block unit number of v */
    }

    unit_output_coodinate = st_gdt_rotate_cal_out_coodinate(gs_special_reg,                               /* gdt_rttfc bit */
                                                            input_coodinate_h,                            /* input_coodinate_h */ 
                                                            input_coodinate_v,                            /* input_coodinate_v */ 
                                                            gs_big_out_range_addr_info_t.dat_unit_num_h,  /* block_size_h */
                                                            gs_big_out_range_addr_info_t.dat_unit_num_v); /* block_size_v */
    output_coodinate_h = unit_output_coodinate.coodinate_h; 
    output_coodinate_v = unit_output_coodinate.coodinate_v; 

    /* calculation DMAC output IMG address */
    if( 8 == gs_gdt_unit_size )
    {
        output_coodinate_v = output_coodinate_v << 3;
    }
    else
    {
        output_coodinate_h = output_coodinate_h << 1;
        output_coodinate_v = output_coodinate_v << 4;
    }

    /* calculation unit output address */
    unit_addr = output_coodinate_h + ( output_coodinate_v * gs_dest_mem_size_h[0] );

    /* calculation DMAC output IMG address */
    gs_out_img_addr = gs_big_out_range_addr_info_t.range_fst_addr[0] + unit_addr;

    /* big trans sequence for sre image */
    v_gdt_seq_rotate_big_trans_src(data_length, p_callback);

    /* big trans sequence for dest image */
    v_gdt_seq_rotate_big_trans_dest(data_length, p_callback);

} /* End of Function v_gdt_seq_rotate_big_trans_blk */        

/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the set order.
* @param[in] data_length        data length
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_rotate_big_trans_src */
static void v_gdt_seq_rotate_big_trans_src(uint32_t data_length, gdt_cb_event_t const p_callback)
{
    uint16_t dmac_size;

    /* internal variable assign */
    switch(gs_gdt_unit_size)
    {
        case 8 :
            {    
            dmac_size = DMA_SIZE_BYTE;
            }
        break;
        case 16:
            {    
            dmac_size = DMA_SIZE_WORD;
            }
        break;
        default : 
            {
            dmac_size = DMA_SIZE_WORD;
            }
        break;
    }

    /* only input 1 unit (2 byte * 16 line ot 1 byte * 8 line) */ 
    if ((1 == gs_in_dat_unit_num_h) && (1 == gs_in_dat_unit_num_v))
    {
        /* DMAC input 1st trans */ 
        v_gdt_dmacin_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                         data_length, gs_src_mem_size_h[0], gs_in_img_addr, p_callback);
    }
    /* other situation */
    else
    {
        /* DMAC input 1st trans */ 
        v_gdt_dmacin_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                             data_length, gs_src_mem_size_h[0], gs_in_img_addr, v_isr_dmac_input, p_callback);
    
        /* calculation DMAC 2nd input IMG address */
        /* only input once in 1 line, next DMAC trans will to next line */
        if(1 == gs_in_dat_unit_num_h)
        {
            gs_in_dat_unit_cnt_v++;     /* After the end of a line, the number of columns is incremented */          
            gs_in_img_addr  = gs_in_img_addr_line_org_array[0]  + gs_in_offset_v[0];                                                                 
            gs_in_img_addr_line_org_array[0]  = gs_in_img_addr;
            gs_in_buf_addr = GDTIBUF0;

            gs_out_dat_unit_cnt_v ++;
        }
        /* only input once in 1 line, next DMAC trans will to next same line */
        else
        {
            gs_in_img_addr  = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h;
            gs_in_buf_addr  = GDTIBUF0;
            gs_in_img_addr_array[0] = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h; 
            
            gs_out_dat_unit_cnt_h ++;
        }
    }
} /* End of Function v_gdt_seq_rotate_big_trans_src */        


/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the set order.
* @param[in] data_length        data length
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_rotate_big_trans_dest */
static void v_gdt_seq_rotate_big_trans_dest(uint32_t data_length, gdt_cb_event_t const p_callback)
{
    uint16_t dmac_size;
    uint32_t unit_addr;
    uint32_t input_coodinate_h;
    uint32_t input_coodinate_v;
    uint32_t output_coodinate_h;
    uint32_t output_coodinate_v;
    st_unit_output_coodinate_t unit_output_coodinate;

    /* internal variable assign */
    switch(gs_gdt_unit_size)
    {
        case 8 :
            {    
            dmac_size = DMA_SIZE_BYTE;
            }
        break;
        case 16:
            {    
            dmac_size = DMA_SIZE_WORD;
            }
        break;
        default : 
            {
            dmac_size = DMA_SIZE_WORD;
            }
        break;
    }

    /* only output 1 unit ( 2 byte * 16 line or 1 byte * 8 line ) */ 
    if ((1 == gs_out_dat_unit_num_h) && (1 == gs_out_dat_unit_num_v))
    {
        /* DMAC output 1st trans call v_isr_dmac_output_once */ 
        v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_INCR, DMA_DEST_ADDR_OFFSET, DMA_REPEAT_BLOCK_DEST,
                              data_length, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_output_once, p_callback);
    }
    /* other situation */
    else
    {
        /* DMAC output 2st trans */ 
        v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, dmac_size, DMA_SRC_INCR, DMA_DEST_ADDR_OFFSET, DMA_REPEAT_BLOCK_DEST,
                              data_length, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_rotate_output, p_callback);

        input_coodinate_h = gs_out_dat_unit_cnt_h ;
        input_coodinate_v = gs_out_dat_unit_cnt_v ;

        unit_output_coodinate=st_gdt_rotate_cal_out_coodinate(gs_special_reg,                               /* gdt_rttfc bit */
                                                              input_coodinate_h,                            /* input_coodinate_h */ 
                                                              input_coodinate_v,                            /* input_coodinate_v */ 
                                                              gs_big_out_range_addr_info_t.dat_unit_num_h,  /* block_size_h */
                                                              gs_big_out_range_addr_info_t.dat_unit_num_v); /* block_size_v */

        output_coodinate_h = unit_output_coodinate.coodinate_h; 
        output_coodinate_v = unit_output_coodinate.coodinate_v; 
        if( 8 == gs_gdt_unit_size )
        {
            output_coodinate_v = output_coodinate_v << 3;
        }
        else
        {
            output_coodinate_h = output_coodinate_h << 1;
            output_coodinate_v = output_coodinate_v << 4;
        }

        /* calculation unit output address */
        unit_addr = output_coodinate_h + ( output_coodinate_v * gs_dest_mem_size_h[0] );

        /* calculation DMAC output IMG address */
        gs_out_img_addr = gs_big_out_range_addr_info_t.range_fst_addr[0] + unit_addr;  
    }
} /* End of Function v_gdt_seq_rotate_big_trans_dest */        

/********************************************************************//**
* @brief The function calculate the coodinate for rotate. This function should be called when rotate the function.
* @param[in] gdt_rttfc          rotation direction select register
* @param[in] input_coodinate_h  input horizon coordinate
* @param[in] input_coodinate_v  input vertical coordinate
* @param[in] block_size_h       (byte)block horizon size
* @param[in] block_size_v       (byte)block vertical size
* @retval unit_output_coodinate return value coordinate
***********************************************************************/
/* Function Name : st_gdt_rotate_cal_out_coodinate */
static st_unit_output_coodinate_t st_gdt_rotate_cal_out_coodinate(uint8_t  gdt_rttfc,
                                                                  uint32_t input_coodinate_h, 
                                                                  uint32_t input_coodinate_v, 
                                                                  uint32_t block_size_h, 
                                                                  uint32_t block_size_v)
{
    uint32_t output_coodinate_h; 
    uint32_t output_coodinate_v; 
    st_unit_output_coodinate_t   unit_output_coodinate;

    /* calculation the output coodinate */
    if( 0 == gdt_rttfc )      /* right 90 */
    {
        output_coodinate_h = (block_size_h - input_coodinate_v) - 1;
        output_coodinate_v = input_coodinate_h;
    }
    else if( 1 == gdt_rttfc ) /* left 90 */
    {
        output_coodinate_h = input_coodinate_v;
        output_coodinate_v = (block_size_h - input_coodinate_h) - 1;
    }
    else if( 2 == gdt_rttfc ) /* up down */
    {
        output_coodinate_h = input_coodinate_h;
        output_coodinate_v = (block_size_v - input_coodinate_v) - 1;
    
    }
    else if( 3 == gdt_rttfc ) /* left right */
    {
        output_coodinate_h = (block_size_h - input_coodinate_h) - 1;
        output_coodinate_v = input_coodinate_v;
    }
    else  /* do nothing */
    {
        __NOP();
    }

    unit_output_coodinate.coodinate_h = output_coodinate_h;
    unit_output_coodinate.coodinate_v = output_coodinate_v;    
    return  unit_output_coodinate;

} /* End of Function st_gdt_rotate_cal_out_coodinate */ 

/********************************************************************//**
* @brief Interrupt handler for dmac output complete request
***********************************************************************/
/* Function Name : v_isr_dmac_rotate_output */
static void v_isr_dmac_rotate_output (void)
{
    /* update address and DMAC enable */   
    *gsp_dmac_reg_out_dar    = gs_out_img_addr; 
    *gsp_dmac_reg_out_sar    = gs_out_buf_addr; 
    *gsp_dmac_reg_out_crb    = 0x01;            
    *gsp_dmac_reg_out_cnt    = VAL_ENABLE;          

    /* DMAC FUNCTION */
    /* update next times output DMAC trans address and DMAC trans end disable */
    v_gdt_rotate_update_out_info();
} /* End of Function v_isr_dmac_rotate_output */

/********************************************************************//**
* @brief Update the next DMAC block transfer message for DMAC output
***********************************************************************/
/* Function Name : v_gdt_rotate_update_out_info */
static void v_gdt_rotate_update_out_info(void)
{

    uint32_t input_coodinate_h;
    uint32_t input_coodinate_v;
    uint32_t output_coodinate_h;
    uint32_t output_coodinate_v;
    uint32_t unit_addr;
    uint32_t timeout;
    st_unit_output_coodinate_t unit_output_coodinate;
    st_dma_state_t state;    
    gs_out_dat_unit_cnt_h++;
    gs_out_buf_addr = GDTOBUF0;

    /* unit line transfer completed */
    if(gs_out_dat_unit_cnt_h >= gs_out_dat_unit_num_h)
    {   
        gs_out_dat_unit_cnt_h = 0;             /* trans end gs_ut_dat_unit_cnt_v clear */
        gs_out_dat_unit_cnt_v ++;              /* After the end of a line, the number of columns is incremented */

        /* range transfer completed */
        if(gs_out_dat_unit_cnt_v == gs_out_dat_unit_num_v) 

        /* Functions other than shrinking do not require this formula */
        {
            /* wait last times DMAC trans complete */
            gsp_dmacdev_out->GetState(&state);

            /* wait DMAC trans complete. check dmac DMSTS.DTIF bit */
            timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;      
            while(DMA_DTIF == (uint8_t)state.dtif_stat)
            {
                gsp_dmacdev_out->GetState(&state);
                __NOP();
                if (0 == (timeout--))
                {
                    
                  /* There is a timeout process because an infinite loop occurs 
                     depending on the optimization level. */
                  break;
                }
            }        

            gs_out_dat_unit_cnt_v = 0;
            v_gdt_imgdata_trans_irq_disable();
            gs_big_in_range_addr_info_t.exist = RANGE_NOT_EXIST;

            if (gsp_dmacdev_in->InterruptDisable() != DMA_OK)
            {
                gs_int_callback();
            }

            if (gsp_dmacdev_out->InterruptDisable() != DMA_OK)
            {
                gs_int_callback();
            }

            if (gsp_dmacdev_in->Close() != DMA_OK)
            {
                gs_int_callback();
            }

            if (gsp_dmacdev_out->Close() != DMA_OK)
            {
                gs_int_callback();
            }
            v_gdt_off();    /* disable GDT start */
            v_gdt_seq_gen_rotate(gs_int_callback);
        }

    }

    input_coodinate_h = gs_out_dat_unit_cnt_h ;
    input_coodinate_v = gs_out_dat_unit_cnt_v ;

    unit_output_coodinate=st_gdt_rotate_cal_out_coodinate(gs_special_reg,                               /* gdt_rttfc bit */
                                                          input_coodinate_h,                            /* input_coodinate_h */ 
                                                          input_coodinate_v,                            /* input_coodinate_v */ 
                                                          gs_big_out_range_addr_info_t.dat_unit_num_h,  /* block_size_h */
                                                          gs_big_out_range_addr_info_t.dat_unit_num_v); /* block_size_v */

    output_coodinate_h = unit_output_coodinate.coodinate_h; 
    output_coodinate_v = unit_output_coodinate.coodinate_v; 

    /* calculation DMAC output IMG address */
    if( 8 == gs_gdt_unit_size )
    {
        output_coodinate_v = output_coodinate_v << 3;
    }
    else
    {
        output_coodinate_h = output_coodinate_h << 1;
        output_coodinate_v = output_coodinate_v << 4;
    }

    /* calculation unit output address */
    unit_addr = output_coodinate_h + ( output_coodinate_v * gs_dest_mem_size_h[0] );

    /* calculation DMAC output IMG address */
    gs_out_img_addr = gs_big_out_range_addr_info_t.range_fst_addr[0] + unit_addr;

} /* End of Function v_gdt_rotate_update_out_info */

/******************************************************************************************************************//**
 * Scroll internal function
 *********************************************************************************************************************/
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_src_t       GDT source image information           
* @param[in] st_img_dest_t      GDT destination image information  
* @param[in] st_blk_conv_info_t GDT block information.
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
***********************************************************************/
/* Function Name : e_gdt_judge_dest_iscr_blkend_ob */
static e_gdt_err_t e_gdt_judge_dest_iscr_blkend_ob(st_img_in_info_t* st_img_src_t, 
                                                   st_img_out_info_t* st_img_dest_t,
                                                   st_blk_conv_info_t* st_blk_conv_info_t)
{
    uint8_t i;    
    for(i=0;i<st_img_dest_t->img_num;i++)
    {    
        if((st_blk_conv_info_t->start_pix_h_dest[i] + (st_blk_conv_info_t->src_size_h - 8)) > (st_img_dest_t->size_h[0]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
        if((st_blk_conv_info_t->start_pix_v_dest[i] + st_blk_conv_info_t->src_size_v) > (st_img_dest_t->size_v[0]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
    }
    return GDT_OK;
} /* End of Function e_gdt_judge_dest_iscr_blkend_ob */

/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range information
* @param[in] blk           GDT block information.
* @param[in] img_src       GDT source image information           
* @param[in] img_dest      GDT destination image information  
***********************************************************************/
/* Function Name : v_gdt_blk_cut_scroll_8x16 */
static void v_gdt_blk_cut_scroll_8x16(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest)
{
    uint16_t fst_pix_h_src[IMG_IN_FRAME_NUM];    
    uint16_t fst_pix_v_src[IMG_IN_FRAME_NUM];
    uint16_t fst_pix_h_dest[IMG_OUT_FRAME_NUM];    
    uint16_t fst_pix_v_dest[IMG_OUT_FRAME_NUM];

    uint32_t line_start_addr_src[IMG_IN_FRAME_NUM];
    uint32_t line_start_addr_dest[IMG_OUT_FRAME_NUM];
    uint32_t addr_offset_src[IMG_IN_FRAME_NUM];  
    uint32_t addr_offset_dest[IMG_OUT_FRAME_NUM];  

    uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM];
    uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM];

    uint32_t block_size_h_byte;
    uint32_t block_size_v_line;

    block_size_h_byte = blk->src_size_h >> 3;
    block_size_v_line = blk->src_size_v;

    gs_big_in_range_addr_info_t.exist     = RANGE_NOT_EXIST;
    gs_big_out_range_addr_info_t.exist    = RANGE_NOT_EXIST;
    gs_remain_in_range_addr_info_t.exist  = RANGE_NOT_EXIST;
    gs_remain_out_range_addr_info_t.exist = RANGE_NOT_EXIST;
    gs_big_in_range_addr_info_t.dat_unit_x_byte  = 0;
    gs_big_in_range_addr_info_t.dat_unit_num_h   = 0;
    gs_big_out_range_addr_info_t.dat_unit_x_byte = 0;
    gs_big_out_range_addr_info_t.dat_unit_num_h  = 0;
    gs_in_offset_v[0]  = 0;
    gs_out_offset_v[0] = 0;

    /* src block start pix addr calculate */
    fst_pix_h_src[0]  = blk->start_pix_h_src[0];
    fst_pix_v_src[0]  = blk->start_pix_v_src[0];

    line_start_addr_src[0]  = fst_pix_v_src[0]  * gs_src_mem_size_h[0];
    addr_offset_src[0]      = line_start_addr_src[0]  + (fst_pix_h_src[0] >> 3);

    block_start_pix_addr_src_array[0] = img_src->start_addr[0]  + addr_offset_src[0];

    /* dest block start pix addr calculate */
    fst_pix_h_dest[0] = blk->start_pix_h_dest[0];
    fst_pix_v_dest[0] = blk->start_pix_v_dest[0];

    line_start_addr_dest[0] = fst_pix_v_dest[0] * gs_dest_mem_size_h[0];
    addr_offset_dest[0]     = line_start_addr_dest[0] + (fst_pix_h_dest[0] >> 3);

    block_start_pix_addr_dest_array[0] = img_dest->start_addr[0] + addr_offset_dest[0];

    /* big range exist flag of input */
    gs_big_in_range_addr_info_t.exist              = RANGE_EXIST;
    gs_big_in_range_addr_info_t.range_fst_addr[0]  = block_start_pix_addr_src_array[0];
    gs_big_in_range_addr_info_t.dat_unit_x_byte    = 1;
    gs_big_in_range_addr_info_t.dat_unit_y_line    = 16;
    gs_big_in_range_addr_info_t.dat_unit_num_h     = (block_size_h_byte-1);  /* the last byte is not needed in the scroll cut */
    gs_big_in_range_addr_info_t.dat_unit_num_v     = block_size_v_line >> 4;

    /* big range exist flag of output */
    gs_big_out_range_addr_info_t.exist             = RANGE_EXIST;
    gs_big_out_range_addr_info_t.range_fst_addr[0] = block_start_pix_addr_dest_array[0];
    gs_big_out_range_addr_info_t.dat_unit_x_byte   = 1;
    gs_big_out_range_addr_info_t.dat_unit_y_line   = 16;
    gs_big_out_range_addr_info_t.dat_unit_num_h    = (block_size_h_byte-1);  /* the last byte is not needed in the scroll cut */
    gs_big_out_range_addr_info_t.dat_unit_num_v    = block_size_v_line >> 4;

    gs_in_offset_v[0]   = gs_src_mem_size_h[0]  << 4;
    gs_out_offset_v[0]  = gs_dest_mem_size_h[0] << 4;
} /* End of Function v_gdt_blk_cut_scroll_8x16 */


/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_scroll_para_cfg     GDT scroll parameter configuration 
***********************************************************************/
/* Function Name : v_gdt_scroll_cfg */
static void v_gdt_scroll_cfg(st_gdt_scroll_para_cfg_t* gdt_scroll_para_cfg)
{
    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ = 0; /* scroll gdtsz(16*16) GDTDSZ FIX '0' */

    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_scroll_para_cfg->iflp_en;

    /* scroll function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.ISCREN  =gdt_scroll_para_cfg->iscren;  

} /* End of Function v_gdt_scroll_cfg */

/********************************************************************//**
* @brief This function processes the conversion block in the set order.
         This  function is for scroll function
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_gen_scroll */
static void v_gdt_seq_gen_scroll( gdt_cb_event_t const p_callback )
{
    /* big area transmit */
    if(RANGE_EXIST == gs_big_in_range_addr_info_t.exist)
    {
        /* block transfer(2byte * 16line or 1 byte * 8line) */
        v_gdt_seq_scroll_big_trans(gs_gdt_unit_size,p_callback); /* gs_gdt_unit_size = 16 */

        /* GDT set and GDT srart (function) */
        v_gdt_imgdata_trans_irq_enable();
        v_gdt_on();
    }

    /* transmit finished, execute callback */
    else
    {
        /* clear pending the interrupt status */
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* clear pending the interrupt status */
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* clear the interrupt status */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* clear the interrupt status */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);    

        /* disable GDT input interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* disable GDT output interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* disable GDT input interrupt trigger CPU */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);

        /* disable GDT output interrupt trigger CPU */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

        /* when endian finish, then execute callback */
        p_callback();
    }

} /* End of Function v_gdt_seq_gen_scroll */

/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the set order.
* @param[in] data_length        data length
* @param[in] p_callback         API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_scroll_big_trans */
static void v_gdt_seq_scroll_big_trans(uint32_t data_length,
                                       gdt_cb_event_t const p_callback )
{


    /* global variable assign for gdt input */
    gs_in_dat_unit_cnt_h    = 1;
    gs_in_dat_unit_cnt_v    = 0;
    gs_special_reg   = 0; /* gs_special_reg represents flag for next DMAC trans will to next lineunit in scroll functon */

    gs_in_offset_h          = gs_big_in_range_addr_info_t.dat_unit_x_byte;
    gs_in_dat_unit_num_h    = gs_big_in_range_addr_info_t.dat_unit_num_h;
    gs_in_dat_unit_num_v    = gs_big_in_range_addr_info_t.dat_unit_num_v;
    gs_in_img_addr          = gs_big_in_range_addr_info_t.range_fst_addr[0]; /* block addr of first image */
    gs_in_buf_addr          = GDTIBUF0;
    gs_in_img_addr_array[0] = gs_big_in_range_addr_info_t.range_fst_addr[0];
    gs_in_img_addr_line_org_array[0] = gs_big_in_range_addr_info_t.range_fst_addr[0];
    
    /* global variable assign for gdt output */
    gs_out_dat_unit_cnt_h    = 1;
    gs_out_dat_unit_cnt_v    = 0;

    gs_out_offset_h          = gs_big_out_range_addr_info_t.dat_unit_x_byte;
    gs_out_dat_unit_num_h    = gs_big_out_range_addr_info_t.dat_unit_num_h;
    gs_out_dat_unit_num_v    = gs_big_out_range_addr_info_t.dat_unit_num_v;
    gs_out_img_addr          = gs_big_out_range_addr_info_t.range_fst_addr[0]; /* block addr of first image */ 
    gs_out_buf_addr          = GDTOBUF0;
    gs_out_img_addr_array[0] = gs_big_out_range_addr_info_t.range_fst_addr[0];
    gs_out_img_addr_line_org_array[0] = gs_big_out_range_addr_info_t.range_fst_addr[0];

    /* DMAC input 1st trans */ 
    v_gdt_dmacin_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                         data_length, gs_src_mem_size_h[0], gs_in_img_addr, v_isr_dmac_scroll_input, p_callback);
    
    /* calculation DMAC 2nd input IMG address */
    gs_in_img_addr  = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h;
    gs_in_buf_addr  = GDTIBUF0;
    gs_in_img_addr_array[0] = gs_in_img_addr_line_org_array[0]  + gs_in_offset_h;
    gs_special_reg     = 1; /* gs_special_reg represents flag in scroll functon */

    /* only output 1unit ( 2 byte * 16 line or 1 byte * 8 line ) */ 
    if ((1 == gs_out_dat_unit_num_h) && (1 == gs_out_dat_unit_num_v))
    {
        /* DMAC output 1st trans call v_isr_output_once_scroll*/ 
        v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_INCR, DMA_DEST_ADDR_OFFSET, DMA_REPEAT_BLOCK_DEST,
                              data_length, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_output_once, p_callback);
    }
    /* other situation */
    else
    {
        /* DMAC output 1st trans */ 
        v_gdt_dmacout_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_INCR, DMA_DEST_ADDR_OFFSET, DMA_REPEAT_BLOCK_DEST,
                              data_length, gs_dest_mem_size_h[0], gs_out_img_addr, v_isr_dmac_scroll_output, p_callback);
    
        /* only output once in 1 line, next DMAC trans will to next line */
        if(1 == gs_out_dat_unit_num_h)
        {
            gs_out_dat_unit_cnt_v++;     /* After the end of a line, the number of columns is incremented */           
            gs_out_img_addr = gs_out_img_addr_line_org_array[0] + gs_out_offset_v[0];
            gs_out_img_addr_line_org_array[0]  = gs_out_img_addr;
            gs_out_buf_addr = GDTOBUF0;
        }
    
        /* only output once in 1 line, next DMAC trans will to next same line */
        else
        {
            gs_out_img_addr = gs_out_img_addr_line_org_array[0] + gs_out_offset_h;
            gs_out_buf_addr = GDTOBUF0;
            gs_out_img_addr_array[0] = gs_out_img_addr_line_org_array[0]  + gs_out_offset_h; 
        }
    }

} /* End of Function v_gdt_seq_scroll_big_trans */        

/********************************************************************//**
* @brief Interrupt handler for dmac input complete request
***********************************************************************/
/* Function Name : v_isr_dmac_scroll_input */
static void v_isr_dmac_scroll_input (void)
{
    /* update address and DMAC enable */ 
    *gsp_dmac_reg_in_sar    = gs_in_img_addr; 
    *gsp_dmac_reg_in_dar    = gs_in_buf_addr;       
    *gsp_dmac_reg_in_crb    = 0x01;           
    *gsp_dmac_reg_in_cnt    = VAL_ENABLE;         
    
    /* DMAC FUNCTION */
    /* update next times input DMAC trans address and DMAC trans end disable */
    v_gdt_iscr_dmac_blk_upinf_in();
} /* End of Function v_isr_dmac_scroll_input */

/********************************************************************//**
* @brief Interrupt handler for dmac output complete request
***********************************************************************/
/* Function Name : v_isr_dmac_scroll_output */ 
static void v_isr_dmac_scroll_output (void)
{
    /* update address and DMAC enable */   
    *gsp_dmac_reg_out_dar    = gs_out_img_addr; 
    *gsp_dmac_reg_out_sar    = gs_out_buf_addr; 
    *gsp_dmac_reg_out_crb    = 0x01;            
    *gsp_dmac_reg_out_cnt    = VAL_ENABLE;          

    /* DMAC FUNCTION */
    /* update next times output DMAC trans address and DMAC trans end disable */
    v_gdt_iscr_dmac_blk_upinf_out();  
} /* End of Function v_isr_dmac_scroll_output */

/********************************************************************//**
* @brief Update the next DMAC transfer message for DMAC input
***********************************************************************/
/* Function Name : v_gdt_iscr_dmac_blk_upinf_in */
static void v_gdt_iscr_dmac_blk_upinf_in(void)
{
    gs_in_buf_addr = GDTIBUF0;

    /* line trans end (8*16)*2 */
    if((gs_in_dat_unit_cnt_h >= gs_in_dat_unit_num_h) && (1 == gs_special_reg))
    {    
        gs_in_dat_unit_cnt_h = 1;   /* trans end gs_in_dat_unit_cnt_v clear */
        gs_in_dat_unit_cnt_v++;     /* After the end of a line, the number of columns is incremented */
    
        /* range transfer completed */
        if(gs_in_dat_unit_cnt_v == gs_in_dat_unit_num_v)
        {
            gs_in_dat_unit_cnt_v = 0;    /* trans end gs_in_dat_unit_cnt_v clear */
        }
    
        /* range transfer transferring */
        else
        {
            gs_in_img_addr_array[0]          = gs_in_img_addr_line_org_array[0] + gs_in_offset_v[0];    /* Base address plus line offset */ 
            gs_in_img_addr_line_org_array[0] = gs_in_img_addr_array[0];
            gs_in_img_addr                   = gs_in_img_addr_array[0];
            gs_special_reg                   = 0; /* flag 1 switches to 0 */
        }
    }
    
    /* line trans transferring */
    else
    {
        if(0 == gs_special_reg)
        {
            gs_in_img_addr          = gs_in_img_addr_array[0] + gs_in_offset_h;   /* Base address plus row offset */
            gs_in_img_addr_array[0] = gs_in_img_addr;
            gs_special_reg          = 1; /* flag 0 switches to 1 */
        }
        else
        {
            gs_special_reg = 0; /* flag 1 switches to 0 */
            gs_in_dat_unit_cnt_h++;
        }
    }

} /* End of Function v_gdt_iscr_dmac_blk_upinf_in */

/********************************************************************//**
* @brief Update the next DMAC transfer message for DMAC output
***********************************************************************/
/* Function Name : v_gdt_iscr_dmac_blk_upinf_out */  
static void v_gdt_iscr_dmac_blk_upinf_out(void)
{
    st_dma_state_t state;    
    gs_out_dat_unit_cnt_h++;
    gs_out_buf_addr = GDTOBUF0;
    uint32_t timeout;
    
    /* unit line transfer completed */
    if(gs_out_dat_unit_cnt_h >= gs_out_dat_unit_num_h)
    {   
        gs_out_dat_unit_cnt_h = 0;    /* trans end gs_ut_dat_unit_cnt_v clear */
        gs_out_dat_unit_cnt_v ++;     /* After the end of a line, the number of columns is incremented */
    
        /* range transfer completed */
        if(gs_out_dat_unit_cnt_v == gs_out_dat_unit_num_v) 
    
        /* gs_out_dat_unit_num_v=gdt_big_range_info_t.dat_unit_num_v * gs_shrnksize*/
        /* Functions other than shrinking do not require this formula */
        {
            /* wait last times DMAC trans complete */
            gsp_dmacdev_out->GetState(&state);

            /* wait DMAC trans complete. check dmac DMSTS.DTIF bit */
            timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;      
            while(DMA_DTIF == (uint8_t)state.dtif_stat)
            {
                gsp_dmacdev_out->GetState(&state);
                __NOP();
                if (0 == (timeout--))
                {
                    
                  /* There is a timeout process because an infinite loop occurs 
                     depending on the optimization level. */
                  break;
                }
            }        

            gs_out_dat_unit_cnt_v = 0;
            v_gdt_imgdata_trans_irq_disable();
            gs_big_in_range_addr_info_t.exist = RANGE_NOT_EXIST;

            if (gsp_dmacdev_in->InterruptDisable() != DMA_OK)
            {
                gs_int_callback();
            }
            if (gsp_dmacdev_out->InterruptDisable() != DMA_OK)
            {
                gs_int_callback();
            }
            if (gsp_dmacdev_in->Close() != DMA_OK)
            {
                gs_int_callback();
            }
            if (gsp_dmacdev_out->Close() != DMA_OK)
            {
                gs_int_callback();
            }
            v_gdt_off();    /* disable GDT start */
            v_gdt_seq_gen_scroll(gs_int_callback);
        }
    
        /* range transfer transferring */
        else
        {
            gs_out_img_addr_array[0] = gs_out_img_addr_line_org_array[0] + gs_out_offset_v[0];   /* Base address plus line offset */ 
            gs_out_img_addr_line_org_array[0] = gs_out_img_addr_array[0];
            gs_out_img_addr = gs_out_img_addr_array[0];
        }
    }
    
    /* unit line transfer transferring */
    else
    {
        gs_out_img_addr = gs_out_img_addr_array[0] + gs_out_offset_h;    /* Base addreess plus row offset */
        gs_out_img_addr_array[0] = gs_out_img_addr;
    }

} /* End of Function v_gdt_iscr_dmac_blk_upinf_out */

/******************************************************************************************************************//**
 * fount internal function
 *********************************************************************************************************************/
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] gdt_fount_para_cfg   font parameter configuration      
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_MOD_CFG       unspecified error                               
***********************************************************************/
/* Function Name : e_gdt_judge_cfg_fount */
static e_gdt_err_t e_gdt_judge_cfg_fount(st_gdt_fount_para_cfg_t* gdt_fount_para_cfg) 
{
    uint8_t fdltdsz;
    uint8_t fdlngsz;

    fdltdsz = gdt_fount_para_cfg->fdltdsz; 
    fdlngsz = gdt_fount_para_cfg->fdlngsz; 

    /* horizontal bit number 7-63 */
    if(fdltdsz > 63)
    {
        return GDT_ERROR_MOD_CFG;
    }
    if(fdltdsz < 7)
    {
        return GDT_ERROR_MOD_CFG;
    }

    /* vertical bit number 7-64 */
    if(fdlngsz > 64)
    {
        return GDT_ERROR_MOD_CFG;
    }
    if(fdlngsz < 7)
    {
        return GDT_ERROR_MOD_CFG;
    }

    return GDT_OK;
} /* End of Function e_gdt_judge_cfg_fount */
                                           
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_src_t         GDT source image information           
* @param[in] st_img_dest_t        GDT destination image information  
* @param[in] st_blk_conv_info_t   GDT block information.
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_IMG_CFG       image configuration error                       
* @retval GDT_ERROR_BLK_CFG       source or destination block configuration error 
* @retval GDT_ERROR_BLK_OFF_BOUND block over bound error                          
***********************************************************************/
/* Function Name : e_gdt_judge_dest_fount */
static e_gdt_err_t e_gdt_judge_dest_fount(st_img_in_info_t* st_img_src_t, 
                                          st_img_out_info_t* st_img_dest_t,
                                          st_blk_conv_info_t* st_blk_conv_info_t)
{
        if(st_img_dest_t->size_h[0] > st_img_dest_t->mem_size_h[0])
        {
            return GDT_ERROR_IMG_CFG;
        }
    
        /* Judge the input/output image size not 8 times value */
        if((0 != (st_img_dest_t->size_h[0] & 0x00000007)) || ( 8 > st_img_dest_t->size_h[0] ))
        {
            return GDT_ERROR_IMG_CFG;
        }
        if((0 != (st_img_dest_t->size_v[0] & 0x00000007)) || ( 8 > st_img_dest_t->size_v[0] ))
        {
            return GDT_ERROR_IMG_CFG;
        }
    
        /* block infomation check */
        if(0 != (st_blk_conv_info_t->start_pix_h_dest[0] & 0x00000007)) /* The block start point of horizontal is not 8 times value */
        {
            return GDT_ERROR_BLK_CFG;
        }
        if((st_blk_conv_info_t->start_pix_h_dest[0]) > (st_img_dest_t->size_h[0])) /* The block start point is off boundary */ 
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
        if((st_blk_conv_info_t->start_pix_v_dest[0]) > (st_img_dest_t->size_v[0]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }

        /* block offbound judge */
        if((st_blk_conv_info_t->start_pix_h_dest[0] + st_blk_conv_info_t->src_size_h) > (st_img_dest_t->size_h[0]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
        if((st_blk_conv_info_t->start_pix_v_dest[0] + st_blk_conv_info_t->src_size_v) > (st_img_dest_t->size_v[0]))
        {
            return GDT_ERROR_BLK_OFF_BOUND;
        }
    return GDT_OK;
} /* End of Function e_gdt_judge_dest_fount */

/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_fount_para_cfg     GDT fount parameter configuration 
* @param[in] fdir               fount function blank bit configuration	
***********************************************************************/
/* Function Name : v_gdt_fount_cfg */
static void v_gdt_fount_cfg(st_gdt_fount_para_cfg_t* gdt_fount_para_cfg, uint8_t fdir)
{
    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ   = 0; /* fount gdtsz(16*16) GDTDSZ FIX '0' */

    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN   = gdt_fount_para_cfg->iflp_en;

    /* fount function blank bit setting */
    gs_gdt_resources.reg_gdt->GDTFDCS_b.SAC    = gdt_fount_para_cfg->sac;  

    /* fount function blank bit setting */
    gs_gdt_resources.reg_gdt->GDTFDCS_b.FDHAD  = gdt_fount_para_cfg->fdhad;  

    /* fount function proces number of fount setting */
    gs_gdt_resources.reg_gdt->GDTFDCS_b.FDIR   = fdir;  

    /* fount horizontal number of bit setting */
    gs_gdt_resources.reg_gdt->GDTFDCS_b.FDLTDSZ= gdt_fount_para_cfg->fdltdsz;  

    /* fount vertical number of bit setting */
    gs_gdt_resources.reg_gdt->GDTFDCS_b.FDLNGSZ= gdt_fount_para_cfg->fdlngsz;  

    /* fount function enable register */
    gs_gdt_resources.reg_gdt->GDTFDCS_b.FDCEN  = 1;  

} /* End of Function v_gdt_fount_cfg */

/********************************************************************//**
* @brief This function processes large area of the conversion block 
*        in the set order.
* @param[in] img_src         GDT source image information           
* @param[in] img_dest        GDT destination image information  
* @param[in] blk_conv_info   GDT block information.
* @param[in] dmac_in_block_num    dmac input block number 
* @param[in] dmac_in_data_length  dmac input data length  
* @param[in] dmac_out_block_num   dmac output block number
* @param[in] dmac_out_data_length dmac output data length 
* @param[in] p_callback    API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_gen_fount */
static void v_gdt_seq_gen_fount(
                                st_img_in_info_t             * img_src,      
                                st_img_out_info_t            * img_dest,     
                                st_blk_conv_info_t           * blk_conv_info,
                                uint8_t dmac_in_block_num,    /* dmac input block number  */ 
                                uint8_t dmac_in_data_length,  /* dmac input data length   */                          
                                uint8_t dmac_out_block_num,   /* dmac output block number */ 
                                uint8_t dmac_out_data_length, /* dmac output data length  */                          
                                gdt_cb_event_t const p_callback )
{
    uint16_t fst_pix_h_dest;    
    uint16_t fst_pix_v_dest;
    uint32_t line_start_addr_dest;
    uint32_t addr_offset_dest;  
    uint32_t block_start_addr_dest;

    /* calculation start address of block */
    fst_pix_h_dest = blk_conv_info->start_pix_h_dest[0];
    fst_pix_v_dest = blk_conv_info->start_pix_v_dest[0];
    line_start_addr_dest = fst_pix_v_dest * (img_dest->mem_size_h[0] >> 3); 
    addr_offset_dest     = line_start_addr_dest + (fst_pix_h_dest >> 3);
    block_start_addr_dest= img_dest->start_addr[0] + addr_offset_dest;

    gs_in_img_addr          = img_src->start_addr[0];  
    gs_in_buf_addr          = GDTIBUF0;
    gs_in_dat_unit_cnt_v    = 0; 
    gs_in_dat_unit_num_v    = dmac_in_block_num;
    gs_in_dat_unit_num_h    = dmac_in_data_length;

    gs_out_img_addr         = block_start_addr_dest;
    gs_out_buf_addr         = GDTOBUF0;
    gs_out_dat_unit_line_y  = dmac_out_block_num;
    gs_out_dat_unit_cnt_y   = 0;
    gs_special_reg          = dmac_out_data_length;  /* use gs_special_reg to store number of foun data */ 
    gs_dest_mem_size_h[0]   = img_dest->mem_size_h[0] >> 3;

    /* CPU output data */
    /* enable interrupt */
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

    /* block_size     = gs_in_dat_unit_num_h */
    /* transfer_count = gs_in_dat_unit_num_v */
    v_gdt_fount_dmacin_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_INCR, DMA_DEST_INCR, DMA_REPEAT_BLOCK_DEST,
                           gs_in_dat_unit_num_h, gs_in_dat_unit_num_v, gs_in_img_addr, gs_in_buf_addr,p_callback);

    /* GDT set and GDT srart  */
    v_gdt_imgdata_trans_irq_enable();
    v_gdt_on();            

} /* End of Function v_gdt_seq_gen_fount */

/********************************************************************//**
* @brief This function processes the conversion block in the set order.
         This  function is for fount function
* @param[in] p_callback    API call back function
***********************************************************************/
/* Function Name : v_gdt_fount_finish */
static void v_gdt_fount_finish( gdt_cb_event_t const p_callback )
{
    /* clear pending the interrupt status */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

    /* clear pending the interrupt status */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

    /* clear the interrupt status */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

    /* clear the interrupt status */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);    

    /* disable GDT input interrupt */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

    /* disable GDT output interrupt */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

    /* disable GDT input interrupt trigger CPU */
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);

    /* disable GDT output interrupt trigger CPU */
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

    /* when endian finish, then execute callback */
    p_callback();

} /* End of Function v_gdt_fount_finish */

/********************************************************************//**
* @brief Interrupt handler for GDT fount output request
***********************************************************************/
/* Function Name : v_isr_cpu_output_fount */
static void v_isr_cpu_output_fount (void)
{
    /* CPU FUNCTION */  
    v_gdt_cpuline_byte_rd_gdt_limit(gs_out_buf_addr,gs_out_img_addr,
                                    gs_special_reg,  /* valid length */
                                    gs_special_reg); /* total length */

    /* update next times CPU trans address and CPU trans end disable */
    v_gdt_cpu_update_out_fount_info();
} /* End of Function v_isr_cpu_output_fount */

/********************************************************************//**
* @brief Update the next CPU transfer message for CPU output
*        only for Remain range transmit
***********************************************************************/
/* Function Name : v_gdt_cpu_update_out_fount_info */
static void v_gdt_cpu_update_out_fount_info(void)
{
    gs_out_dat_unit_cnt_y++;

    /* y line trans end*/
    if(gs_out_dat_unit_cnt_y >= gs_out_dat_unit_line_y)    /* vertical data size */
    {
        gs_out_dat_unit_cnt_y = 0;
        gs_out_buf_addr = GDTOBUF0;

        /* disable GDT output interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* clear GDT output interrupt flag */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* close cpu output data IRQ */ 
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI); 

        /* disable GDT output interrupt trigger CPU  */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

        /*  disable dmac module */
        if (gsp_dmacdev_in->InterruptDisable() != DMA_OK)
        {
            gs_int_callback();
        }
        if (gsp_dmacdev_in->Close() != DMA_OK)
        {
            gs_int_callback();
        }
        v_gdt_imgdata_trans_irq_disable();
        v_gdt_off();    /* disable GDT start */ 
        v_gdt_fount_finish(gs_int_callback);
    }

    /* unit transferring */
    else
    {
        /* calculation memory address */
        gs_out_img_addr = gs_out_img_addr + gs_dest_mem_size_h[0]; 
        gs_out_buf_addr = GDTOBUF0;
    }
} /* End of Function v_gdt_cpu_update_out_fount_info */

/********************************************************************//**
* @brief configuration information for DMAC (source to gdt inbuffer).
* @param[in] dmtmd_md       dmac transfer mode register configuration
* @param[in] dmtmd_sz       dmac transfer mode register configuration
* @param[in] dmamd_sm       dmac transfer mode register configuration
* @param[in] dmamd_dm       dmac transfer mode register configuration
* @param[in] dmtmd_dts      dmac transfer mode register configuration
* @param[in] data_length    dmac transfer mode register configuration
* @param[in] transfer_count dmac transfer mode register configuration
* @param[in] inaddr         dmac transfer mode register configuration
* @param[in] outaddr        dmac transfer mode register configuration
* @param[in] p_callback     API call back function
***********************************************************************/
/* Function Name : v_gdt_fount_dmacin_set */
static void v_gdt_fount_dmacin_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                                   uint32_t dmamd_dm, uint32_t dmtmd_dts, uint32_t data_length, uint32_t transfer_count,
                                   uint32_t inaddr, uint32_t outaddr, gdt_cb_event_t const p_callback)
{ 
    st_dma_transfer_data_cfg_t config;

    /* set DMAC register */
    config.mode = dmtmd_md | dmtmd_sz | dmamd_sm | dmamd_dm | dmtmd_dts;

    config.src_addr       = inaddr;
    config.dest_addr      = outaddr;  /* GDTIBUF */ 
    config.transfer_count = transfer_count;
    config.block_size     = data_length;
    config.src_extended_repeat  = 0;
    config.dest_extended_repeat = 0;

    /* open DMAC function */
    if (gsp_dmacdev_in->Open() != DMA_OK)
    {
        p_callback();
    }

    /* enable GDT input interrupt trigger DMAC */
    if (gsp_dmacdev_in->Create(GDT_TO_DMA_IN_TRG, &config) != DMA_OK) 
    {
        p_callback();
    }

    /* enable DMAC input interrupt */    
    if (gsp_dmacdev_in->InterruptEnable(DMA_INT_COMPLETE, NULL) != DMA_OK)
    {
        p_callback();
    }

    /* DMAC trans enable */
    if (gsp_dmacdev_in->Control(DMA_CMD_START, NULL) != DMA_OK)
    {
        p_callback();
    }
} /* End of Function v_gdt_fount_dmacin_set */

/******************************************************************************************************************//**
 * coloralign internal function
 *********************************************************************************************************************/
/********************************************************************//**
* @brief Determining a processing parameter configuration is correct
* @param[in] st_img_dest_t        GDT source image information                    
* @param[in] cialgsl              4bit or 3bit mode selection
* @retval GDT_OK                  successful operation                            
* @retval GDT_ERROR_IMG_CFG       image configuration error                       
***********************************************************************/
/* Function Name : e_gdt_judge_cial_dest_mem_size */
static e_gdt_err_t e_gdt_judge_cial_dest_mem_size(st_img_out_info_t* st_img_dest_t, uint8_t cialgsl)
{

    uint8_t  cialgsl_temp;
    uint32_t dest0_mem_size_h;

    cialgsl_temp     = cialgsl;
    dest0_mem_size_h = st_img_dest_t->mem_size_h[0];

    if(4 == cialgsl_temp) /* 4bit mode memory size check */
    {
        /* memory size check */
        if(!((128  == dest0_mem_size_h) ||
             (256  == dest0_mem_size_h) ||
             (512  == dest0_mem_size_h) ||
             (1024 == dest0_mem_size_h)
             ))
        {
            return GDT_ERROR_IMG_CFG;
        }
    }
    else if(3 == cialgsl_temp) /* 3bit mode memory size check */
    {
        /* memory size check */
        if(!((96  == dest0_mem_size_h) ||
             (192 == dest0_mem_size_h) ||
             (384 == dest0_mem_size_h) ||
             (768 == dest0_mem_size_h)
             ))
        {
            return GDT_ERROR_IMG_CFG;
        }
    
    }
    else
    {
        __NOP();
    }

    return GDT_OK;
} /* End of Function e_gdt_judge_cial_dest_mem_size */

/********************************************************************//**
* @brief This function applies to divide conversion block into GDT
    conversion range and update big range and remain range information
* @param[in] blk           GDT block information.
* @param[in] img_src       GDT source image information           
* @param[in] img_dest      GDT destination image information     
***********************************************************************/
/* Function Name : v_gdt_blk_cut_clralign_16x16 */
static void v_gdt_blk_cut_clralign_16x16(st_blk_conv_info_t * blk, st_img_in_info_t * img_src,  st_img_out_info_t * img_dest) 
{
    uint8_t  i;    
    uint16_t fst_pix_h_src[IMG_IN_FRAME_NUM];    
    uint16_t fst_pix_v_src[IMG_IN_FRAME_NUM];
    uint16_t fst_pix_h_dest[IMG_OUT_FRAME_NUM];    
    uint16_t fst_pix_v_dest[IMG_OUT_FRAME_NUM];

    uint32_t line_start_addr_src[IMG_IN_FRAME_NUM];
    uint32_t line_start_addr_dest[IMG_OUT_FRAME_NUM];
    uint32_t addr_offset_src[IMG_IN_FRAME_NUM];  
    uint32_t addr_offset_dest[IMG_OUT_FRAME_NUM];  

    uint32_t block_start_pix_addr_src_array[IMG_IN_FRAME_NUM];
    uint32_t block_start_pix_addr_dest_array[IMG_OUT_FRAME_NUM];

    uint32_t block_size_h_byte;
    uint32_t block_size_v_line;

    block_size_h_byte = blk->src_size_h >> 3;
    block_size_v_line = blk->src_size_v;

    gs_big_in_range_addr_info_t.exist     = RANGE_NOT_EXIST;
    gs_big_out_range_addr_info_t.exist    = RANGE_NOT_EXIST;
    gs_remain_in_range_addr_info_t.exist  = RANGE_NOT_EXIST;
    gs_remain_out_range_addr_info_t.exist = RANGE_NOT_EXIST;
    gs_big_in_range_addr_info_t.dat_unit_x_byte  = 0;
    gs_big_in_range_addr_info_t.dat_unit_num_h   = 0;
    gs_big_out_range_addr_info_t.dat_unit_x_byte = 0;
    gs_big_out_range_addr_info_t.dat_unit_num_h  = 0;

    for(i=0;i<img_src->img_num;i++)
    {        
        gs_in_offset_v[i]       = 0;
        fst_pix_h_src[i]        = blk->start_pix_h_src[i];
        fst_pix_v_src[i]        = blk->start_pix_v_src[i];
        line_start_addr_src[i]  = fst_pix_v_src[i]  * gs_src_mem_size_h[i];
        addr_offset_src[i]      = line_start_addr_src[i]  + (fst_pix_h_src[i] >> 3);
        block_start_pix_addr_src_array[i] = img_src->start_addr[i]  + addr_offset_src[i];
    }

    for(i=0;i<img_dest->img_num;i++)
    {        
        gs_out_offset_v[i]      = 0;
        fst_pix_h_dest[i]       = blk->start_pix_h_dest[i];
        fst_pix_v_dest[i]       = blk->start_pix_v_dest[i];
        line_start_addr_dest[i] = fst_pix_v_dest[i] * gs_dest_mem_size_h[i];
        addr_offset_dest[i]     = line_start_addr_dest[i] + (fst_pix_h_dest[i] >> 3);
        block_start_pix_addr_dest_array[i] = img_dest->start_addr[i] + addr_offset_dest[i];
    }

    v_gdt_blk_cut_comm_16x16_burst( block_start_pix_addr_src_array, block_start_pix_addr_dest_array, 
                                    block_size_h_byte, block_size_v_line);    

} /*End of Function v_gdt_blk_cut_clralign_16x16 */

/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_coloralign_para_cfg    coloralign parameter configuration
***********************************************************************/
/* Function Name : v_gdt_coloralign_cfg */
static void v_gdt_coloralign_cfg(st_gdt_coloralign_para_cfg_t* gdt_coloralign_para_cfg)
{
    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ = 0; /* coloralign gdtsz(16*16) GDTDSZ FIX '0' */

    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_coloralign_para_cfg->iflp_en;

    /* coloralign function align mod sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CIALGSL = gdt_coloralign_para_cfg->cialgsl;

    /* coloralign function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CIALGEN = 1;
} /* End of Function v_gdt_coloralign_cfg */

/********************************************************************//**
* @brief This function processes the conversion block in the set order.
         This  function is for common
* @param[in] cialg_bit_mod  4bit or 3bit mode selection
* @param[in] p_callback API call back function
***********************************************************************/
/* Function Name : v_gdt_seq_gen_coloralign */
static void v_gdt_seq_gen_coloralign( uint8_t cialg_bit_mod, gdt_cb_event_t const p_callback )
{
    /* dest block remani area unit num h enlarge */    
    gs_out_dat_unit_num_h                           = gs_remain_out_range_addr_info_t.dat_unit_num_h  * cialg_bit_mod;
    gs_remain_out_range_addr_info_t.dat_unit_x_byte = gs_remain_out_range_addr_info_t.dat_unit_x_byte * cialg_bit_mod;
    gs_remain_out_range_addr_info_t.dat_unit_y_line = gs_remain_out_range_addr_info_t.dat_unit_y_line * cialg_bit_mod;

    /* function of common sequence */
    /* Remain area do not exist */
    if(RANGE_NOT_EXIST == gs_remain_in_range_addr_info_t.exist)
    {

        /* transmit finished, execute callback */
        /* clear pending the interrupt status */
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* clear pending the interrupt status */
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* clear the interrupt status */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* clear the interrupt status */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);    

        /* disable GDT input interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII);

        /* disable GDT output interrupt */
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI);

        /* disable GDT input interrupt trigger CPU */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, VAL_DISABLE, NULL);

        /* disable GDT output interrupt trigger CPU */
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, VAL_DISABLE, NULL);

        /* when endian finish, then execute callback */
        p_callback();
    }

    /* remain range transmit firstly */
    else 
    {

        /* CPU xbyte transfer */
        if (COMMON_REMAIN_CPU_XBYTE == gs_remain_trans_mod)
        {
            /* enable GDT output interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATII, GDT_IN_IELS_VALUE,  v_isr_cpu_input_xbyte);

            /* enable GDT output interrupt trigger CPU */
            R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_GDT_DATOI, GDT_OUT_IELS_VALUE, v_isr_cpu_output_xbyte);

            v_gdt_seq_comm_remn_trans_xbyte();
        }
        else  /* do nothing */ 
        {
            __NOP();
        }

        /* GDT set and GDT srart function */
        v_gdt_imgdata_trans_irq_enable();
        v_gdt_on();
    }
} /* End of Function v_gdt_seq_gen_coloralign */

/******************************************************************************************************************//**
 * colorsyn internal function
 *********************************************************************************************************************/
/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_colorsyn_para_cfg coloralign parameter configuration
***********************************************************************/
/* Function Name : v_gdt_colorsyn_cfg */
static void v_gdt_colorsyn_cfg(st_gdt_colorsyn_para_cfg_t* gdt_colorsyn_para_cfg)
{
    /* GDT data size sel register */
    gs_gdt_resources.reg_gdt->GDTCR_b.GDTDSZ = gdt_colorsyn_para_cfg->gdtdsz; /* colorsyn gdtsz( 8*8 or 16*16) */

    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_colorsyn_para_cfg->iflp_en;

    /* color synthesis function through setting register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CPTS = gdt_colorsyn_para_cfg->cpts;

    /* color synthesis function special color setting register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CDCS = gdt_colorsyn_para_cfg->cdcs;

    /* color synthesis function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CSYNEN = 1;

} /* End of Function v_gdt_colorsyn_cfg */

/******************************************************************************************************************//**
 * color internal function
 *********************************************************************************************************************/
/********************************************************************//**
* @brief GDT function register configuration
* @param[in] gdt_color_para_cfg   color parameter configuration
***********************************************************************/
/* Function Name : v_gdt_color_cfg */
static void v_gdt_color_cfg(st_gdt_color_para_cfg_t* gdt_color_para_cfg)
{
    /* Reverse function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.IFLPEN = gdt_color_para_cfg->iflp_en;

    /* part 0 color setting register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CLRDS0 = gdt_color_para_cfg->clrds0;

    /* part 1 color setting register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CLRDS1 = gdt_color_para_cfg->clrds1;

    /* color function enable register */
    gs_gdt_resources.reg_gdt->GDTCR_b.CLREN = 1;

} /* End of Function v_gdt_color_cfg */

/******************************************************************************************************************//**
 * @} (end addtogroup Device HAL GDT )
 *********************************************************************************************************************/

/* End of file (r_gdt_api.c) */

