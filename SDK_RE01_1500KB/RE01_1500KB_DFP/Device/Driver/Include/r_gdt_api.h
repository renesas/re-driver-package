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
* File Name    : r_gdt_api.h
* Version      : 1.20
* Description  : HAL-Driver for GDT
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.11.2018 0.60    First Release
*         : 29.11.2018 0.60    Fixed for function naming error.
*                              - Fixed for API function naming error.
*                                e_GDT_Shrink->R_GDT_Shrink
*         : 10.12.2018 0.60    Add macro definition due to adding function.
*         : 10.12.2018 0.60    change file name r_gdt_api.h    
*         : 25.12.2018 0.60    Update comments
*                              Adapted to GSCE coding rules.
*         : 21.01.2019 0.61    First Release of endian module
*         : 28.01.2019 0.61    structure st_gdt_shrink_para_cfg_t and st_gdt_endian_para_cfg_t add member gdtdsz
*                              delete the define value GDT_UNIT_SIZE
*         : 04.03.2019 0.61    First Release of monochrome and rotate module
*         : 10.03.2019 0.70    Adapted to GSCE coding rules. 
*         : 29.04.2019 0.72    API Release 
*                              - Fount 
*                              - Nochange 
*                              - Coloralign 
*                              - Colorsyn 
*                              - Color
*         : 22.07.2019 1.00    Change the product name to RE Family
*                              - Change the include file names
*                              - Update comments
*         : 11.09.2019 1.01    Upgrade version to 1.01
*                              Delete unused definitions
*         : 13.02.2020 1.10    Upgrade version to 1.10
*         : 26.08.2020 1.20    Upgrade version to 1.20
*
***********************************************************************************************************************/
#ifndef R_GDT_API_H
#define R_GDT_API_H

#ifdef  __cplusplus
extern "C"
{
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "RE01_1500KB.h"
#include "r_lpm_api.h"
#include "r_dmac_api.h"
#include "r_system_api.h"
/******************************************************************************************************************//**
 @ingroup  grp_device_hal
 @defgroup grp_device_hal_gdt Device HAL GDT Implementation
 @brief The GDT (Graphic Data Transformation) driver implementation for the Device HAL driver interface on RE01 Group.
    
 @section 
 @code
 @endcode
 @section 

 @{
 *********************************************************************************************************************/

#include "r_gdt_cfg.h"

/******************************************************************************
Macro definitions
*******************************************************************************/
/** Major Version Number of API. */
#define GDT_VERSION_MAJOR    (1)
/** Minor Version Number of API. */
#define GDT_VERSION_MINOR    (20)

/** Disable value */
#define VAL_DISABLE              (0)
/** Enable value */
#define VAL_ENABLE               (1)
 /** DMAC initial mode */
#define INTMOD_DMAC              (0) 

/** GDT cpu transfer mode */
#define GDT_TRANS_CPU_MOD    (0)
/** GDT dmac transfer mode */
#define GDT_TRANS_DMAC_MOD   (1)

/** big or remain range exist */
#define RANGE_EXIST          (1)
/** big or remain range not exist */
#define RANGE_NOT_EXIST      (0)
/** DMAC channel 0 */
#define DMA_CH0              (0)
/** DMAC channel 1 */
#define DMA_CH1              (1)
/** DMAC channel 2 */
#define DMA_CH2              (2)
/** DMAC channel 3 */
#define DMA_CH3              (3)
/** GDT in buffer relative address */
#define GDTIBUF0_OFFSET      (0x00000100UL)  
/** GDT in buffer address */
#define GDTIBUF0             (GDT_BASE + GDTIBUF0_OFFSET)
/** GDT out buffer relative address */
#define GDTOBUF0_OFFSET      (0x00000200UL)  
/** GDT out buffer address */
#define GDTOBUF0             (GDT_BASE + GDTOBUF0_OFFSET)
/** enable GDT input interrupt trigger DMAC */
#define GDT_TO_DMA_IN_TRG    (0xA8U)
/** enable GDT input interrupt trigger DMAC */
#define GDT_TO_DMA_OUT_TRG   (0xA9U)
/** enable GDT input interrupt trigger fount DMAC */
#define GDT_TO_DMA_FDCENDI_TRG   (0xAAU)
/** enable GDT input interrupt trigger CPU */
#define GDT_IN_IELS_VALUE    (0x1EU)
/** enable GDT output interrupt trigger CPU */
#define GDT_OUT_IELS_VALUE   (0x1EU)

#define SHRNK_SIZE_1         (0)
#define SHRNK_SIZE_2         (1)
#define SHRNK_SIZE_3         (2)
#define SHRNK_SIZE_4         (3)
#define SHRNK_SIZE_5         (4)
#define SHRNK_SIZE_6         (5)
#define SHRNK_SIZE_7         (6)
#define SHRNK_SIZE_8         (7)

#define SHRNKED_SIZE_1       (1)
#define SHRNKED_SIZE_2       (2)
#define SHRNKED_SIZE_3       (3)
#define SHRNKED_SIZE_4       (4)
#define SHRNKED_SIZE_5       (5)
#define SHRNKED_SIZE_6       (6)
#define SHRNKED_SIZE_7       (7)

#define SHRNK_NUM_8          (8)
/** DMAC trans complete flag 0:not complete 1:complete*/
#define DMA_DTIF             (0x0)

/** mode for transferring gdt data */  
#define SHK_IN_1X8_OUT_LINE        (0)
/** common transferring mode 2byte * 16line */  
#define COMMON_BIG_BLK_2X16        (1)
/** common transferring mode 1byte * 8line */  
#define COMMON_BIG_BLK_1X8         (2)
/** cpu transferring mode byte */  
#define COMMON_REMAIN_CPU_BYTE     (3)
/** cpu transferring mode xbyte */  
#define COMMON_REMAIN_CPU_XBYTE    (4)

/** rotate mode turn right 90 */
#define RTTFC_RIGHT90      (0)
/** rotate mode turn left 90 */
#define RTTFC_LEFT90       (1)
/** rotate mode up and down */
#define RTTFC_UP_DOWN      (2)
/** rotate mode left and right */
#define RTTFC_LEFT_RIGHT   (3)

#if GDT_CFG_COLOR_ON
/** color mode image in munber */
#define IMG_IN_FRAME_NUM   (6)
/** color mode image out munber */
#define IMG_OUT_FRAME_NUM  (3)
#else
/** monochroe mode image in munber */
#define IMG_IN_FRAME_NUM   (3)
/** monochroe mode image in munber */
#define IMG_OUT_FRAME_NUM  (1)
#endif

/** color mode data buffer offset */
#define DAT_BUF_OFFSET     (0x20) 

/** shrink mode selet */
#define GDT_SHRNK         (0)
/** endian mode selet */
#define GDT_ENDIAN        (1)
/** monechrome mode selet */
#define GDT_MONOCHROME    (2)
/** rotate mode selet */
#define GDT_ROTATE        (3)
/** scroll mode selet */
#define GDT_SCROLL        (4)
/** font mode selet */
#define GDT_FOUNT         (5)
/** no change mode selet */
#define GDT_NULL          (6)
/** monochrome to color mode selet */
#define GDT_MONO_TO_COLOR (7)
/** colorsyn mode selet */
#define GDT_COLORSYN      (8)
/** coloralign mode selet */
#define GDT_COLORALIGN    (9)

/*****************************************************************************
Typedef definitions
******************************************************************************/

/** @brief GDT driver call back founction */
typedef void (*gdt_cb_event_t) (void);           // @suppress("Source line ordering")

/** @brief Function pointer of the interrupt service routine */
typedef void (*isr_function) (void);             // @suppress("Source line ordering")

/** @brief GDT API error codes */
typedef enum
{
    GDT_OK                    = 1,               /*!< successful operation                            */
    GDT_ERROR                 = 2,               /*!< unspecified error                               */
    GDT_ERROR_IMG_CFG         = 3,               /*!< image configuration error                       */
    GDT_ERROR_BLK_OFF_BOUND   = 4,               /*!< block over bound error                          */
    GDT_ERROR_BLK_CFG         = 5,               /*!< source or destination block configuration error */
    GDT_ERROR_MOD_CFG         = 6,               /*!< GDT mode configuration error                    */
    GDT_ERROR_IMG_NUM         = 7,               /*!< GDT image number configuration error            */
    GDT_ERROR_DMAC_CH_CFG     = 8                /*!< GDT dmac channel configuration error            */

} e_gdt_err_t;

/** @brief image information */
typedef struct 
{
    uint8_t             img_num;                        /*!< the number of source images      */
    uint32_t            start_addr[IMG_IN_FRAME_NUM];   /*!< source image address             */
    uint32_t            size_h[IMG_IN_FRAME_NUM];       /*!< source image horizon size        */
    uint32_t            size_v[IMG_IN_FRAME_NUM];       /*!< source image vertical size       */
    uint32_t            mem_size_h[IMG_IN_FRAME_NUM];   /*!< source image memory horizon size */
} st_img_in_info_t;

/** @brief image information */
typedef struct 
{
    uint8_t             img_num;                        /*!< the number of source images      */
    uint32_t            start_addr[IMG_OUT_FRAME_NUM];  /*!< source image address             */
    uint32_t            size_h[IMG_OUT_FRAME_NUM];      /*!< source image horizon size        */
    uint32_t            size_v[IMG_OUT_FRAME_NUM];      /*!< source image vertical size       */
    uint32_t            mem_size_h[IMG_OUT_FRAME_NUM];  /*!< source image memory horizon size */
} st_img_out_info_t;

/** @brief information of change block */
typedef struct 
{
    uint32_t            start_pix_h_src[IMG_IN_FRAME_NUM];           /*!< start pixel of source image, horizon direction       */
    uint32_t            start_pix_v_src[IMG_IN_FRAME_NUM];           /*!< start pixel of source image, vertical direction      */
    uint32_t            start_pix_h_dest[IMG_OUT_FRAME_NUM];         /*!< start pixel of destination image, horizon direction  */
    uint32_t            start_pix_v_dest[IMG_OUT_FRAME_NUM];         /*!< start pixel of destination image, vertical direction */
    uint32_t            src_size_h;                /*!< the size of modification area, horizon direction     */
    uint32_t            src_size_v;                /*!< the size of modification area, vertical direction    */
} st_blk_conv_info_t;

/** @brief GDT function shrink setting */
typedef struct
{
    uint8_t                iflp_en;                /*!< reverse function enable       */
    uint8_t                shrnk_size;             /*!< shrink size select            */
    uint8_t                shrnk_bak_grnd_colr;    /*!< set the color for remain area */
} st_gdt_shrink_para_cfg_t; 

/** @brief GDT function endian setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable               */
    uint8_t                endian;                /*!< endian function enable                */
} st_gdt_endian_para_cfg_t; 

/** @brief GDT function monochrome setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable                 */
    uint8_t                gdtdsz;                /*!< gdt data size sel 0:16*16bit(no used) 1:8*8bit   */
    uint8_t                mpcs;                  /*!< monochrome priority color specification */
    uint8_t                mbrden;                /*!< monochrome border enabel                */
} st_gdt_monochrome_para_cfg_t; 

/** @brief GDT function rotate setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable                 */
    uint8_t                gdtdsz;                /*!< gdt data size sel 0:16*16bit 1:8*8bit   */
    uint8_t                rttfc;                 /*!< rotate function select                */
} st_gdt_rotate_para_cfg_t; 

/** @brief GDT function scroll setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable                 */
    uint8_t                iscren;                /*!< scroll function enable                  */
} st_gdt_scroll_para_cfg_t; 

/** @brief GDT function fount setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable                 */
    uint8_t                sac;                   /*!< start address change bit                */
    uint8_t                fdhad;                 /*!< blank bit setting                       */
    uint8_t                fdltdsz;               /*!< fount horizontal number of bit setting  */
    uint8_t                fdlngsz;               /*!< fount vertical number of bit setting    */
} st_gdt_fount_para_cfg_t; 

/** @brief GDT function nochange setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable             */
} st_gdt_nochange_para_cfg_t; 

/** @brief GDT function coloralign setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable             */
    uint8_t                cialgsl;               /*!< coloralign function align mode sel  */
} st_gdt_coloralign_para_cfg_t; 

/** @brief GDT function color synthesis setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable               */
    uint8_t                gdtdsz;                /*!< gdt data size sel 0:16*16bit 1:8*8bit */
    uint8_t                cpts;                  /*!< color synthesis through setting       */
    uint8_t                cdcs;                  /*!< color synthesis special color setting */
} st_gdt_colorsyn_para_cfg_t; 

/** @brief GDT function color setting */
typedef struct
{
    uint8_t                iflp_en;               /*!< reverse function enable  */
    uint8_t                clrds0;                /*!< part 0 RGB color setting */
    uint8_t                clrds1;                /*!< part 1 RGB color setting */
} st_gdt_color_para_cfg_t; 

/** @brief select transfer mode */
typedef struct
{
    uint8_t                ch_in;                  /*!< input transfer channel select      */
    uint8_t                ch_out;                 /*!< output transfer channel select     */
} st_trans_mod_cfg_t;                                                                      

/** @brief cut type block information */
typedef struct
{
    uint32_t            exist;                     /*!< judge type exists            */
    uint32_t            fst_addr;                  /*!< first block address          */
    uint32_t            dat_unit;                  /*!< block size type              */
    uint32_t            dat_unit_num_h;            /*!< the number of block horizon  */
    uint32_t            dat_unit_num_v;            /*!< the number of block vertical */
} st_blk_info_t;

/** @brief cut block information */
typedef struct
{
    st_blk_info_t        big_blk_info;             /*!< judge type exists           */
    st_blk_info_t        small_blk_info;           /*!< first block address         */
    st_blk_info_t        cpu_blk_info_left;        /*!< block size type             */
    st_blk_info_t        cpu_blk_info_right;       /*!< the number of block horizon */
} st_blk_cut_info_t;                                                                

/** @brief detailed range information after cut*/
typedef struct
{
    uint8_t              exist;                    /*!< judge type exists            */
    uint32_t             range_fst_pix_in_h;       /*!< range coordinate information */
    uint32_t             range_fst_pix_in_v;       /*!< range coordinate information */
    uint32_t             range_fst_pix_out_h;      /*!< range coordinate information */
    uint32_t             range_fst_pix_out_v;      /*!< range coordinate information */
    uint32_t             dat_unit_x_byte;          /*!< gdt process unit information */
    uint32_t             dat_unit_y_line;          /*!< gdt process unit information */
    uint32_t             dat_unit_num_h;           /*!< img information              */
    uint32_t             dat_unit_num_v;           /*!< img information              */
}st_range_info_t;                                                                    

/** @brief detailed range addr information after cut*/
typedef struct
{
    uint8_t              exist;                               /*!< judge type exists            */
    uint32_t             range_fst_addr[IMG_IN_FRAME_NUM];    /*!< range coordinate information */
    uint16_t             dat_unit_x_byte;                     /*!< gdt process unit information */
    uint32_t             dat_unit_y_line;                     /*!< gdt process unit information */
    uint16_t             dat_unit_num_h;                      /*!< img information              */
    uint16_t             dat_unit_num_v;                      /*!< img information              */
}st_range_in_addr_info_t;                                                               
                                                                                     
/** @brief detailed range addr information after cut*/
typedef struct
{
    uint8_t              exist;                               /*!< judge type exists            */
    uint32_t             range_fst_addr[IMG_OUT_FRAME_NUM];   /*!< range coordinate information */
    uint16_t             dat_unit_x_byte;                     /*!< gdt process unit information */
    uint32_t             dat_unit_y_line;                     /*!< gdt process unit information */
    uint16_t             dat_unit_num_h;                      /*!< img information              */
    uint16_t             dat_unit_num_v;                      /*!< img information              */
}st_range_out_addr_info_t;                                                               

/** @brief range coordinate information */                                                  
typedef struct                                                                       
{
    uint8_t              img_num;                         /*!< the number of source images */
    uint32_t             start_addr[IMG_IN_FRAME_NUM];    /*!< source image address        */
    uint16_t             size_h[IMG_IN_FRAME_NUM];        /*!< source image vertical size  */
    uint32_t             size_v[IMG_IN_FRAME_NUM];        /*!< source image vertical size  */
    uint8_t              size_h_byte[IMG_IN_FRAME_NUM];   /*!< image horizon byte          */
}st_img_in_prosess_t;

/** @brief range coordinate information */                                                  
typedef struct                                                                       
{
    uint8_t              img_num;                          /*!< the number of source images */
    uint32_t             start_addr[IMG_OUT_FRAME_NUM];    /*!< source image address        */
    uint16_t             size_h[IMG_OUT_FRAME_NUM];        /*!< source image vertical size  */
    uint32_t             size_v[IMG_OUT_FRAME_NUM];        /*!< source image vertical size  */
    uint8_t              size_h_byte[IMG_OUT_FRAME_NUM];   /*!< image horizon byte          */
}st_img_out_prosess_t;

/** @brief range coordinate information for xbyte transmit */                                                  
typedef struct                                                                       
{
    uint32_t             range_h_byte;             /*!< source image address        */
    uint32_t             trans_addr;               /*!< source image vertical size  */
}st_cpublk_xbyte_trans_info_t;

/** @brief unit coordinate information */                                                  
typedef struct                                                                       
{
    uint32_t             coodinate_h;              /*!< the coodinate_h of ouput unit  */
    uint32_t             coodinate_v;              /*!< the coodinate_v of ouput unit  */
}st_unit_output_coodinate_t;

/** @brief GDT recource data */
typedef struct 
{           
    volatile GDT_Type       *reg_gdt;              /*!< Base pointer to GDT registers       */
    volatile DMAC0_Type     *reg_dmac0;            /*!< Base pointer to DMAC0 registers     */
    volatile DMAC1_Type     *reg_dmac1;            /*!< Base pointer to DMAC1 registers     */
    volatile DMAC2_Type     *reg_dmac2;            /*!< Base pointer to DMAC2 registers     */
    volatile DMAC3_Type     *reg_dmac3;            /*!< Base pointer to DMAC3 registers     */
    volatile ICU_Type       *reg_icu;              /*!< Base pointer to ICU registers       */
    volatile MSTP_Type      *reg_mstp;             /*!< Base pointer to MSTP registers      */
    e_lpm_mstp_t            mstp_id;               /*!< MSTP id                             */
    uint8_t                 gdt_in_int_priority;   /*!< GDT input interrupt priority value  */
    uint8_t                 gdt_out_int_priority;  /*!< GDT output interrupt priority value */
    uint32_t                dmac0_int_priority;    /*!< DMAC0 interrupt priority            */
    uint32_t                dmac1_int_priority;    /*!< DMAC1 interrupt priority            */
    uint32_t                dmac2_int_priority;    /*!< DMAC2 interrupt priority            */
    uint32_t                dmac3_int_priority;    /*!< DMAC3 interrupt priority            */
} const st_gdt_resources_t; 

/******************************************************************************
 Exported global function (to be accessed by other files) 
******************************************************************************/

#if (GDT_CFG_R_GDT_GETVERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_GDT_GETVERSION      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_GDT_GETVERSION
#endif

#if (GDT_CFG_R_GDT_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_GDT_OPEN      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else
#define FUNC_LOCATION_R_GDT_OPEN
#endif

#if (GDT_CFG_R_GDT_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_GDT_CLOSE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else
#define FUNC_LOCATION_R_GDT_CLOSE
#endif

#if (GDT_CFG_R_GDT_DMAC_CH_SEL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_GDT_DMACCHSEL     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else
#define FUNC_LOCATION_R_GDT_DMACCHSEL
#endif

#if (GDT_CFG_E_GDT_SHRINK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_SHRINK    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_SHRINK
#endif

#if (GDT_CFG_E_GDT_ENDIAN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_ENDIAN    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_ENDIAN
#endif

#if (GDT_CFG_E_GDT_MONOCHROME == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_MONOCHROME    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_MONOCHROME
#endif

#if (GDT_CFG_E_GDT_ROTATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_ROTATE    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_ROTATE
#endif

#if (GDT_CFG_E_GDT_SCROLL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_SCROLL    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_SCROLL
#endif

#if (GDT_CFG_E_GDT_FOUNT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_FOUNT    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_FOUNT
#endif

#if (GDT_CFG_E_GDT_NOCHANGE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_NOCHANGE    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_NOCHANGE
#endif

#if (GDT_CFG_E_GDT_COLORALIGN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_COLORALIGN    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_COLORALIGN
#endif

#if (GDT_CFG_E_GDT_COLORSYN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_COLORSYN    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_COLORSYN
#endif

#if (GDT_CFG_E_GDT_COLOR == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_E_GDT_COLOR    __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */ 
#else                           
#define FUNC_LOCATION_E_GDT_COLOR
#endif

uint32_t R_GDT_GetVersion(void) FUNC_LOCATION_R_GDT_GETVERSION;
e_gdt_err_t R_GDT_Open(void) FUNC_LOCATION_R_GDT_OPEN;
e_gdt_err_t R_GDT_Close(void) FUNC_LOCATION_R_GDT_CLOSE;
e_gdt_err_t R_GDT_DmacChSel(st_trans_mod_cfg_t  * trans_mod_cfg) FUNC_LOCATION_R_GDT_DMACCHSEL;
e_gdt_err_t R_GDT_Shrink(st_img_in_info_t         * img_src, 
                         st_img_out_info_t        * img_dest,
                         st_blk_conv_info_t       * blk_conv_info,
                         st_gdt_shrink_para_cfg_t * gdt_mod_para_cfg,
                         gdt_cb_event_t const     p_callback) FUNC_LOCATION_E_GDT_SHRINK;

e_gdt_err_t R_GDT_Endian(st_img_in_info_t         * img_src, 
                         st_img_out_info_t        * img_dest,
                         st_blk_conv_info_t       * blk_conv_info,
                         st_gdt_endian_para_cfg_t * gdt_mod_para_cfg,
                         gdt_cb_event_t const     p_callback) FUNC_LOCATION_E_GDT_ENDIAN;

e_gdt_err_t R_GDT_Monochrome(st_img_in_info_t             * img_src, 
                             st_img_out_info_t            * img_dest,
                             st_blk_conv_info_t           * blk_conv_info,
                             st_gdt_monochrome_para_cfg_t * gdt_mod_para_cfg,
                             gdt_cb_event_t const         p_callback) FUNC_LOCATION_E_GDT_MONOCHROME;

e_gdt_err_t R_GDT_Rotate(st_img_in_info_t         * img_src, 
                         st_img_out_info_t        * img_dest,
                         st_blk_conv_info_t       * blk_conv_info,
                         st_gdt_rotate_para_cfg_t * gdt_rotate_para_cfg,
                         gdt_cb_event_t const     p_callback) FUNC_LOCATION_E_GDT_ROTATE;

e_gdt_err_t R_GDT_Scroll(st_img_in_info_t         * img_src, 
                         st_img_out_info_t        * img_dest,
                         st_blk_conv_info_t       * blk_conv_info,
                         st_gdt_scroll_para_cfg_t * gdt_scroll_para_cfg,
                         gdt_cb_event_t const     p_callback) FUNC_LOCATION_E_GDT_SCROLL;

e_gdt_err_t R_GDT_Fount(st_img_in_info_t        * img_src, 
                        st_img_out_info_t       * img_dest,
                        st_blk_conv_info_t      * blk_conv_info,
                        st_gdt_fount_para_cfg_t * gdt_fount_para_cfg,
                        gdt_cb_event_t const    p_callback) FUNC_LOCATION_E_GDT_FOUNT;

e_gdt_err_t R_GDT_Nochange(st_img_in_info_t           * img_src, 
                           st_img_out_info_t          * img_dest,
                           st_blk_conv_info_t         * blk_conv_info,
                           st_gdt_nochange_para_cfg_t * gdt_nochange_para_cfg,
                           gdt_cb_event_t const       p_callback) FUNC_LOCATION_E_GDT_NOCHANGE;

e_gdt_err_t R_GDT_Coloralign(st_img_in_info_t             * img_src, 
                             st_img_out_info_t            * img_dest,
                             st_blk_conv_info_t           * blk_conv_info,
                             st_gdt_coloralign_para_cfg_t * gdt_coloralign_para_cfg,
                             gdt_cb_event_t const         p_callback) FUNC_LOCATION_E_GDT_COLORALIGN;

e_gdt_err_t R_GDT_Colorsyn(st_img_in_info_t           * img_src, 
                           st_img_out_info_t          * img_dest,
                           st_blk_conv_info_t         * blk_conv_info,
                           st_gdt_colorsyn_para_cfg_t * gdt_colorsyn_para_cfg,
                           gdt_cb_event_t const       p_callback) FUNC_LOCATION_E_GDT_COLORSYN;

e_gdt_err_t R_GDT_Color(st_img_in_info_t        * img_src, 
                        st_img_out_info_t       * img_dest,
                        st_blk_conv_info_t      * blk_conv_info,
                        st_gdt_color_para_cfg_t * gdt_color_para_cfg,
                        gdt_cb_event_t const    p_callback) FUNC_LOCATION_E_GDT_COLOR;

#ifdef  __cplusplus
}
#endif

/******************************************************************************************************************//**
 * @} (end defgroup grp_device_hal_gdt)
 *********************************************************************************************************************/

#endif /* R_GDT_API_H */
/* End of file (r_gdt_api.h) */
