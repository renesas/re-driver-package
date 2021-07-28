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
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_smip_api.c
* Version      : 1.20
* Description  : Serial MIP-LCD API.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description      
*         : 23.07.2019 1.00    First Release     
*         : 11.10.2019 1.01    Fixed WDT refresh function
*                              Delete the sequence for transmission by VCOM synchronization method
*                              Added Suspend and Resume functions
*                              Added ReconfigSCLK function
*         : 19.02.2020 1.10    AGT module stop processing in r_smip_tm_lpm_on function was deleted.
*                              Addition of LSB / MSB setting switching definition of SPI. 
*                              - Added Definition to select communication format of SPI in r_smip_cfg.h.
*         : 10.04.2020 1.20    Fixed the problem that AGT register value could not be rewritten when Kyocera LCD is used.
*                              Fixed to reset in bit order specified in config when executing Resume
**********************************************************************************************************************/

/******************************************************************************************************************//**
 * @addtogroup grp_device_hal_smip
 * @{
 *********************************************************************************************************************/
/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
/* Public interface header file for this package. */
#include <stdint.h>
#include <stdbool.h>
#include "r_smip_api.h"


/******************************************************************************
 Macro definitions
 *****************************************************************************/

/** Version Number of API. */
#define SMIP_PRV_VERSION_MAJOR          (1)         /*!< Major Version */
#define SMIP_PRV_VERSION_MINOR          (20)        /*!< Minor Version */

#define SMIP_PRV_MAX_SPI_CH             (2)         /*!< SPI maximum number of channels */
#define SMIP_PRV_MAX_AGT_CH             (2)         /*!< AGT maximum number of channels */
#define SMIP_PRV_AGT_CSOURCE            (32768UL)   /* AGT count source(LOCO=32.768kHz) */

#define SMIP_PRV_FLAG_OPENED            (1 << 0)    /*!< Initialization flag */
#define SMIP_PRV_FLAG_POWERED           (1 << 1)    /*!< Started flag */
#define SMIP_PRV_FLAG_SUSPEND           (1 << 2)    /*!< Suspend flag */
#define SMIP_PRV_FLAG_RECONFIG          (1 << 3)    /*!< Reconfig flag */

#define SMIP_PRV_POWER_ON_OFF_JDI       (0x00)      /*!< JDI power on and power off sequence */
#define SMIP_PRV_POWER_ON_OFF_KYOCERA   (0x02)      /*!< Kyocera power on and power off sequence */

#define SMIP_PRV_MASK_COLOR_MODE        (0x01)      /* Mask value for color mode(3bit / 4bit) */
#define SMIP_PRV_MASK_ON_OFF_SEQ        (0x02)      /* Mask value for power ON / OFF sequence judgment */
#define SMIP_PRV_MASK_ADDR_ENDIAN       (0x04)      /*!< Address endian mask */
#define SMIP_PRV_MASK_MULTI_DUMMY_EN    (0x08)      /*!< Multi dummy transmission enable mask */
#define SMIP_PRV_MASK_SEND_SYNC         (0x10)      /*!< Synchronous transmission mask */

#define SMIP_PRV_AGT_LPM_WAIT           (72)        /* Wait time until LPM setting (count source 2 clock) */

#if (0 == SMIP_CFG_SPI_BIT_ORDER)

    /* SPI bit order is MSB */
    #define SMIP_PRV_SPI_BIT_ORDER      ARM_SPI_MSB_LSB     
    #define SMIP_PRV_CMD_ACL_JDI        (0x28)
    #define SMIP_PRV_CMD_ACL_SHARP      (0x20)
#else

    /* SPI bit order is LSB */
    #define SMIP_PRV_SPI_BIT_ORDER      ARM_SPI_LSB_MSB
    #define SMIP_PRV_CMD_ACL_JDI        (0x14)
    #define SMIP_PRV_CMD_ACL_SHARP      (0x04)
#endif

/** @brief JDI-LCD information */
#define SMIP_PRV_JDI_LCD_INFO    {                              /* @suppress("Macro expansion") */ \
                (0x04 | (SMIP_CFG_JDI_COLOR_MODE & SMIP_PRV_MASK_COLOR_MODE)),  /* Type */  \
                SMIP_CFG_JDI_MODE_SIZE,                         /* [M] Mode bit size */     \
                SMIP_CFG_JDI_MODE_DMY_SIZE,                     /* [DMY] DMY bit size */    \
                SMIP_CFG_JDI_ADDR_SIZE,                         /* [AG] Address bit size */ \
                SMIP_CFG_JDI_DUMMY_SIZE,                        /* DUMMY data bit size */   \
                SMIP_CFG_JDI_FIRST_ADDR,                        /* First address */         \
                SMIP_CFG_JDI_DISP_WIDTH,                        /* LCD width dot size */    \
                SMIP_CFG_JDI_DISP_LINE,                         /* LCD Line number */       \
                SMIP_CFG_JDI_VCOM_CLK,                          /* VCOM clock(Hz) */        \
                SMIP_CFG_JDI_T_MEMORY_INI,                      /* Pixel memory initialization time(T2, T5) */      \
                SMIP_CFG_JDI_T_COM_INI,                         /* COM and latch initialization time(T3, T4, T6) */ \
                SMIP_CFG_JDI_TS_SCS,                            /* CSC setup time for JDI */ \
                SMIP_CFG_JDI_TH_SCS,                            /* CSC hold time for JDI */  \
                SMIP_CFG_JDI_TW_SCSL,                           /* CSC low width for JDI */  \
                (0),                                            /* tsVCOM(Not used) */      \
                (0),                                            /* thVCOM(Not used) */      \
                gs_tbl_cmd_jdi }                                /* Command table */

/** @brief Kyocera-LCD information */
#define SMIP_PRV_KYOCERA_LCD_INFO  {                            /* @suppress("Macro expansion") */   \
                (0x1A | (SMIP_CFG_KYOCERA_COLOR_MODE & SMIP_PRV_MASK_COLOR_MODE)),    /* Type */     \
                SMIP_CFG_KYOCERA_MODE_SIZE,                     /* [M] Mode bit size */     \
                SMIP_CFG_KYOCERA_MODE_DMY_SIZE,                 /* [DMY] DMY bit size */    \
                SMIP_CFG_KYOCERA_ADDR_SIZE,                     /* [AG] Address bit size */ \
                SMIP_CFG_KYOCERA_DUMMY_SIZE,                    /* DUMMY data bit size */   \
                SMIP_CFG_KYOCERA_FIRST_ADDR,                    /* First address */         \
                SMIP_CFG_KYOCERA_DISP_WIDTH,                    /* LCD width dot size */    \
                SMIP_CFG_KYOCERA_DISP_LINE,                     /* LCD Line number */       \
                SMIP_CFG_KYOCERA_VCOM_CLK,                      /* VCOM clock(Hz) */        \
                SMIP_CFG_KYOCERA_T_MEMORY_INI,                  /* Pixel memory initialization time(T2, T5) */      \
                SMIP_CFG_KYOCERA_T_COM_INI,                     /* COM and latch initialization time(T3, T4, T6) */ \
                SMIP_CFG_KYOCERA_TS_SCS,                        /* CSC setup time for Kyocera */ \
                SMIP_CFG_KYOCERA_TH_SCS,                        /* CSC hold time for Kyocera */  \
                SMIP_CFG_KYOCERA_TW_SCSL,                       /* CSC low width for Kyocera */  \
                SMIP_CFG_KYOCERA_TS_VCOM,                       /* tsVCOM */                \
                SMIP_CFG_KYOCERA_TH_VCOM,                       /* thVCOM */                \
                NULL }                                          /* Command table */

/** @brief SHARP-LCD information */
#define SMIP_PRV_SHARP_LCD_INFO        {                        /* @suppress("Macro expansion") */  \
                (0x00 | (SMIP_CFG_SHARP_COLOR_MODE & SMIP_PRV_MASK_COLOR_MODE)), /* Type */         \
                SMIP_CFG_SHARP_MODE_SIZE,                       /* [M] Mode bit size */     \
                SMIP_CFG_SHARP_MODE_DMY_SIZE,                   /* [DMY] DMY bit size */    \
                SMIP_CFG_SHARP_ADDR_SIZE,                       /* [AG] Address bit size */ \
                SMIP_CFG_SHARP_DUMMY_SIZE,                      /* DUMMY data bit size */   \
                SMIP_CFG_SHARP_FIRST_ADDR,                      /* First address */         \
                SMIP_CFG_SHARP_DISP_WIDTH,                      /* LCD width dot size */    \
                SMIP_CFG_SHARP_DISP_LINE,                       /* LCD Line number */       \
                SMIP_CFG_SHARP_VCOM_CLK,                        /* VCOM clock(Hz) */        \
                SMIP_CFG_SHARP_T_MEMORY_INI,                    /* Pixel memory initialization time(T2, T5) */      \
                SMIP_CFG_SHARP_T_COM_INI,                       /* COM and latch initialization time(T3, T4, T6) */ \
                SMIP_CFG_SHARP_TS_SCS,                          /* CSC setup time for SHARP */ \
                SMIP_CFG_SHARP_TH_SCS,                          /* CSC hold time for SHARP */  \
                SMIP_CFG_SHARP_TW_SCSL,                         /* CSC low width for SHARP */ \
                0,                                              /* tsVCOM(Not used) */      \
                0,                                              /* thVCOM(Not used) */      \
                gs_tbl_cmd_sharp }                              /* Command table */

/****************************************************************************************************************//**
 * @brief       Inline function to toggle VCOM output
 *******************************************************************************************************************/
/* Function Name : r_smip_vcom_toggle */
__STATIC_FORCEINLINE void r_smip_vcom_toggle(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
#if ((1 == SMIP_CFG_EXTMODE) && (0 == SMIP_CFG_AGTO_EN))
    SMIP_CFG_VCOM_PORT->PODR = (SMIP_CFG_VCOM_PORT->PODR ^ (1 << SMIP_CFG_VCOM_PIN)); // @suppress("Cast comment")
#endif

}/* End of function r_smip_vcom_toggle */

/****************************************************************************************************************//**
 * @brief       Inline function to set VCOM output to negative level
 *******************************************************************************************************************/
/* Function Name : r_smip_vcom_negative */
__STATIC_FORCEINLINE void r_smip_vcom_negative(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
#if ((1 == SMIP_CFG_EXTMODE) && (0 == SMIP_CFG_AGTO_EN))
    SMIP_CFG_VCOM_PORT->PODR = (SMIP_CFG_VCOM_PORT->PODR & (~(1 << SMIP_CFG_VCOM_PIN))); // @suppress("Cast comment")
#endif

}/* End of function r_smip_vcom_negative */

/****************************************************************************************************************//**
 * @brief       Inline function to set SCS output to active level
 *******************************************************************************************************************/
/* Function Name : r_smip_scs_active */
__STATIC_FORCEINLINE void r_smip_scs_active(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
    SMIP_CFG_SCS_PORT->PODR = (SMIP_CFG_SCS_PORT->PODR | (1 << SMIP_CFG_SCS_PIN)); // @suppress("Cast comment")

}/* End of function r_smip_scs_active */

/****************************************************************************************************************//**
 * @brief       Inline function to set SCS output to negative level
 *******************************************************************************************************************/
/* Function Name : r_smip_scs_negative */
__STATIC_FORCEINLINE void r_smip_scs_negative(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
    SMIP_CFG_SCS_PORT->PODR = (SMIP_CFG_SCS_PORT->PODR & (~(1 << SMIP_CFG_SCS_PIN))); // @suppress("Cast comment")

}/* End of function r_smip_scs_negative */

/****************************************************************************************************************//**
 * @brief       Inline function to set Disp output to active level
 *******************************************************************************************************************/
/* Function Name : r_smip_disp_active */
__STATIC_FORCEINLINE void r_smip_disp_active(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
    SMIP_CFG_RST_DISP_PORT->PODR = (SMIP_CFG_RST_DISP_PORT->PODR | (1 << SMIP_CFG_RST_DISP_PIN)); // @suppress("Cast comment")

}/* End of function r_smip_disp_active */


/****************************************************************************************************************//**
 * @brief       Inline function to set Disp output to negative level
 *******************************************************************************************************************/
/* Function Name : r_smip_disp_negative */
__STATIC_FORCEINLINE void r_smip_disp_negative(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
    SMIP_CFG_RST_DISP_PORT->PODR = (SMIP_CFG_RST_DISP_PORT->PODR & (~(1 << SMIP_CFG_RST_DISP_PIN))); // @suppress("Cast comment")

}/* End of function r_smip_disp_negative */

/****************************************************************************************************************//**
 * @brief       Inline function to set RST output to active level
 *******************************************************************************************************************/
/* Function Name : r_smip_rst_active */
__STATIC_FORCEINLINE void r_smip_rst_active(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
    SMIP_CFG_RST_DISP_PORT->PODR = (SMIP_CFG_RST_DISP_PORT->PODR & (~(1 << SMIP_CFG_RST_DISP_PIN))); // @suppress("Cast comment")

}/* End of function r_smip_rst_active */

/****************************************************************************************************************//**
 * @brief       Inline function to set RST output to negative level
 *******************************************************************************************************************/
/* Function Name : r_smip_rst_negative */
__STATIC_FORCEINLINE void r_smip_rst_negative(void) /* @suppress("Function description comment")  @suppress("Source line ordering") */
{
    SMIP_CFG_RST_DISP_PORT->PODR = (SMIP_CFG_RST_DISP_PORT->PODR | (1 << SMIP_CFG_RST_DISP_PIN)); // @suppress("Cast comment")

}/* End of function r_smip_rst_negative */


/******************************************************************************
 Local Typedef definitions
 *****************************************************************************/
/** SMIP command number */
typedef enum // @suppress("Source line ordering")
{
    SMIP_PRV_CMD_NO_UPDATE = 0,                      /*!< Operation succeeded */
    SMIP_PRV_CMD_ALL_CLEAR,                          /*!< Unspecified error */
    SMIP_PRV_CMD_UPDATE_MONO,                        /*!< Driver is busy */
    SMIP_PRV_CMD_UPDATE_COLOR_3BIT,                  /*!< Parameter error */
    SMIP_PRV_CMD_UPDATE_COLOR_4BIT,                  /*!< Parameter error */
    SMIP_PRV_CMD_MAX
} e_smip_cmd_t;

/** SMIP Power on sequence */
typedef enum
{
    SMIP_PRV_PWON_SUCCESS = 0,                      /*!< Power on sequence success */
    SMIP_PRV_PWON_BUSY,                             /*!< Power on sequence running */
    SMIP_PRV_PWON_ERROR                             /*!< Power on sequence error */
} e_smip_pwon_seq_t;


/** SMIP  */
typedef enum
{
/* JDI */
    SMIP_PRV_STATE_FREE_RDY = 0,                    
#if (1 == SMIP_CFG_MAKE_LINE_INFO)
    SMIP_PRV_STATE_FREE_SND_HEADER,                 
    SMIP_PRV_STATE_FREE_SND_BODY,                   
    SMIP_PRV_STATE_FREE_SND_FOOTER,                 
#else
    SMIP_PRV_STATE_FREE_BUSY,                       
#endif
    SMIP_PRV_STATE_FREE_AW_BUSY,                    
    SMIP_PRV_STATE_FREE_CMD_BUSY,                   

/* kyosera */
    SMIP_PRV_STATE_ADJUST_RDY,                        
    SMIP_PRV_STATE_ADJUST_TS_VCOM,                    
#if (1 == SMIP_CFG_MAKE_LINE_INFO)
    SMIP_PRV_STATE_ADJUST_SND_HEADER,                 
    SMIP_PRV_STATE_ADJUST_SND_BODY,                   
    SMIP_PRV_STATE_ADJUST_SND_FOOTER,                 
#else
    SMIP_PRV_STATE_ADJUST_BUSY,                       
#endif
    SMIP_PRV_STATE_ADJUST_TH_VCOM,                    
    SMIP_PRV_STATE_ADJUST_AW_TS_VCOM,                 
    SMIP_PRV_STATE_ADJUST_AW_BUSY,
    SMIP_PRV_STATE_ADJUST_CMD_TS_VCOM,                 
    SMIP_PRV_STATE_ADJUST_CMD_BUSY,                   

    SMIP_PRV_STATE_MAX

} e_smip_state_t;


typedef struct {        
    uint8_t                 flag;
    const st_smip_cfg_t     *p_lcd_info;
    smip_cb_event_t         cb_event;
    
    uint8_t                 header_size;
    uint8_t                 body_mono_size;
    uint8_t                 body_color_size;
    uint8_t                 footer_size;
    
} st_smip_info_t;


typedef struct {        
    volatile AGT0_Type      *reg;               /*!< Base pointer to AGT registers */
    e_system_mcu_lock_t     lock_id;            /*!< Module lock id */
    e_lpm_mstp_t            mstp_id;            /*!< MSTP id */
    IRQn_Type               agti_irq;           /*!< AGTI IRQ Number */
    uint32_t                agti_iesr_val;      /*!< AGTI IESR value */
    r_pinset_t              agt_pin_set;        /*!< AGTOn pin setting */
    r_pinset_t              agt_pin_clr;        /*!< AGTOn pin clear */
} st_smip_vcom_timer_t;

typedef struct {
    const uint8_t           *data;              /*!<  */
#if (1 == SMIP_CFG_MAKE_LINE_INFO)
    uint16_t                line_cur;           /*!<  */
    uint16_t                line_end;           /*!<  */
    uint16_t                body_size;
#else
    uint32_t                send_size;          /*!<  */
#endif
    uint8_t                 hedder_cmd;
} st_smip_send_info_t;




/******************************************************************************
 Exported global variables (to be accessed by other files)
 *****************************************************************************/
extern ARM_DRIVER_SPI Driver_SPI0; // @suppress("Source line ordering")
extern ARM_DRIVER_SPI Driver_SPI1;

/******************************************************************************
 Private global variables and functions
 *****************************************************************************/
#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_TBL_COMMAND       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_TBL_COMMAND
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_OPEN                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CLOSE               == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define DATA_LOCATION_PRV_TBL_VCOM_TIMER       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_TBL_VCOM_TIMER
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_SEND_COMMAND_SUB       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SEND_COMMAND_SUB
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_ALL_WRITE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_ALL_WRITE
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_MAKE_HEDDER     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_MAKE_HEDDER
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_SEND                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_SEND_SEQ       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SEND_SEQ
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_OPEN                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CLOSE               == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_TIMER_CONTROL       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_TIMER_CONTROL
#endif


#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_POWER_ON_SEQ       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_POWER_ON_SEQ
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_MAKE_COOMAND       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_MAKE_COOMAND
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COLOR          == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_SEND_COMMAND        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))    
#define FUNC_LOCATION_R_SMIP_SCS_HOLD       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SCS_HOLD
#endif

#if (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_CALLBACK_CHECK       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_CALLBACK_CHECK
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_SEND_ERROR_SEQ       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SEND_ERROR_SEQ
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_OPEN                == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_POWER_OFF           == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_STATE_INIT       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_STATE_INIT
#endif

#if (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_CB_SPI_EVENT       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_CB_SPI_EVENT
#endif

#if (SMIP_CFG_SECTION_R_SMIP_CB_VCOM_TIMER_EVENT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_CB_VCOM_TIMER_EVENT       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_CB_VCOM_TIMER_EVENT
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_START_LINE_SEND       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_START_LINE_SEND
#endif

#if ((SMIP_CFG_SECTION_R_SMIP_POWER_ON            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ZERO            == SYSTEM_SECTION_RAM_FUNC) || \
     (SMIP_CFG_SECTION_R_SMIP_ALL_ONE             == SYSTEM_SECTION_RAM_FUNC))
#define FUNC_LOCATION_R_SMIP_SET_AW_DATA       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_SET_AW_DATA
#endif

static ARM_DRIVER_SPI       *gsp_spi_drv = &Driver_SPI0;
static uint16_t             gs_vcom_count;
static uint16_t             gs_ts_vcom;
static uint16_t             gs_th_vcom;

static st_smip_info_t       gs_smip_info;
static uint8_t              gs_wr_buff[SMIP_CFG_ALL_WRITE_BUFF];
static e_smip_state_t       gs_smip_state;
static st_smip_send_info_t  gs_smip_send_info;
static uint16_t             gs_aw_cur_addr;
volatile static e_smip_pwon_seq_t    gs_power_on_seq = SMIP_PRV_PWON_SUCCESS;

/* Command table                                        */
/* NUp     : No-Update command                          */
/* ACL     : All Clear command                          */
/* Ud-Mono : Data-Update 1bit command(Black and white)  */
/* Ud-Color: Data-Update 3bit/4bit command(Color)       */

                                                         /*  NUp,  ACL,                   Ud-Mono,Ud-Color(3),Ud-Color(4)*/
static const uint8_t gs_tbl_cmd_jdi[SMIP_PRV_CMD_MAX]  DATA_LOCATION_PRV_TBL_COMMAND
                                                         = { 0x00, SMIP_PRV_CMD_ACL_JDI,  0x88,   0x80,       0x90  };
static const uint8_t gs_tbl_cmd_sharp[SMIP_PRV_CMD_MAX] DATA_LOCATION_PRV_TBL_COMMAND
                                                         = { 0x00, SMIP_PRV_CMD_ACL_SHARP,0x80,   0x80,       0x80  } ;

const st_smip_cfg_t g_smip_tbl_lcd_info[3] DATA_LOCATION_PRV_TBL_LCD_INFO 
                                                         = { SMIP_PRV_KYOCERA_LCD_INFO, /* @suppress("Cast comment") */
                                                             SMIP_PRV_JDI_LCD_INFO,
                                                             SMIP_PRV_SHARP_LCD_INFO} ;

static const st_smip_vcom_timer_t gs_vcom_timer[SMIP_PRV_MAX_AGT_CH] DATA_LOCATION_PRV_TBL_VCOM_TIMER = 
                {{(AGT0_Type*)AGT0, SYSTEM_LOCK_AGT0, LPM_MSTP_AGT0, /* @suppress("Cast comment") */
                  SYSTEM_CFG_EVENT_NUMBER_AGT0_AGTI, 0x13, R_AGT_Pinset_CH0, R_AGT_Pinclr_CH0},  /* AGT0 */
                 {(AGT0_Type*)AGT1, SYSTEM_LOCK_AGT1, LPM_MSTP_AGT1,  /* @suppress("Cast comment") */
                  SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI, 0x06 ,R_AGT_Pinset_CH1, R_AGT_Pinclr_CH1}}; /* AGT1 */
static void r_smip_cb_spi_event(uint32_t event) FUNC_LOCATION_R_SMIP_CB_SPI_EVENT;
static void r_smip_cb_vcom_timer_event(void) FUNC_LOCATION_R_SMIP_CB_VCOM_TIMER_EVENT;

static e_smip_err_t r_smip_send_command_sub(uint8_t cmd) FUNC_LOCATION_R_SMIP_SEND_COMMAND_SUB;
static e_smip_err_t r_smip_all_write(uint8_t wr_data) FUNC_LOCATION_R_SMIP_ALL_WRITE;
static void r_smip_make_hedder(uint16_t address, uint8_t cmd_id, uint8_t *buff) FUNC_LOCATION_R_SMIP_MAKE_HEDDER;
static e_smip_err_t r_smip_send_seq(uint16_t st, uint16_t line_num, uint16_t body_size, const uint8_t *data, uint8_t cmd_id)
                                                                    FUNC_LOCATION_R_SMIP_SEND_SEQ;
static void r_smip_set_send_info(uint16_t body_size, uint16_t st, uint16_t line_num, const uint8_t *data, uint8_t cmd_id)
                                                                    FUNC_LOCATION_R_SMIP_SEND_SEQ;

static void r_smip_vcom_timer_stop(void) FUNC_LOCATION_R_SMIP_TIMER_CONTROL;
static void r_smip_vcom_timer_start(uint16_t time) FUNC_LOCATION_R_SMIP_TIMER_CONTROL;
static void r_smip_tm_lpm_on(void) FUNC_LOCATION_R_SMIP_TIMER_CONTROL;
static void r_smip_tm_lpm_off(void) FUNC_LOCATION_R_SMIP_TIMER_CONTROL;
static e_smip_err_t r_smip_wait_power_on_seq(void) FUNC_LOCATION_R_SMIP_POWER_ON_SEQ;
static void r_smip_make_cmd(uint8_t cmd, uint8_t *buff) FUNC_LOCATION_R_SMIP_MAKE_COOMAND;
static void r_smip_scs_hold(void) FUNC_LOCATION_R_SMIP_SCS_HOLD;
static void gs_smip_callback_check(void) FUNC_LOCATION_R_SMIP_CALLBACK_CHECK;
static void r_smip_send_error_seq(void) FUNC_LOCATION_R_SMIP_SEND_ERROR_SEQ;
static void r_smip_state_init(void) FUNC_LOCATION_R_SMIP_STATE_INIT;
static e_smip_err_t r_smip_start_line_send(void) FUNC_LOCATION_R_SMIP_START_LINE_SEND;
static void r_smip_set_aw_data(uint8_t wr_data, uint16_t all_wr_size) FUNC_LOCATION_R_SMIP_SET_AW_DATA;

#if (1 == SMIP_CFG_MAKE_LINE_INFO)
#if (SMIP_CFG_SECTION_R_SMIP_CB_SPI_EVENT        == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_R_SMIP_HEADER_SEND       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_R_SMIP_HEADER_SEND
#endif

static void r_smip_header_send(e_smip_state_t next_state) FUNC_LOCATION_R_SMIP_HEADER_SEND;
#endif


/*****************************************************************************************************************//**
 * @brief       Get API verion
 * @retval      uint32_t  Version.
**********************************************************************************************************************/
/* Function Name : R_SMIP_GetVersion */
uint32_t R_SMIP_GetVersion(void)
{
    uint32_t const version = (SMIP_PRV_VERSION_MAJOR << 16) | SMIP_PRV_VERSION_MINOR;

    return (version);
}/* End of function R_SMIP_GetVersion() */


/*****************************************************************************************************************//**
 * @brief       The implementation of Middleware SMIP Open API for the Serial MIP peripheral on RE Family.
 * @param[in]   ch       SPI channel to use
 * @param[in]   sclk     Transmit clock frequency (Hz)
 * @param[in]   *p_info  Information of serial MIP-LCD
 * @param[in]   cb_event Callback function
 * @retval      SMIP_OK     Open successful.
 * @retval      SMIP_ERROR  Open error.
 * @retval      SMIP_ERROR_PARAMETER       Parameter setting is invalid.
 * @retval      SMIP_ERROR_SYSTEM_SETTING  System setting is invalid.
**********************************************************************************************************************/
/* Function Name : R_SMIP_Open */
e_smip_err_t R_SMIP_Open(uint8_t ch, uint32_t sclk, const st_smip_cfg_t *p_info, smip_cb_event_t cb_event) /* @suppress("Function length") */
{
    uint32_t agt_value;
    uint16_t temp;
    
    if (0U != (SMIP_PRV_FLAG_OPENED & gs_smip_info.flag))
    {
        /* SMIP driver is already initialized */
        return (SMIP_ERROR);
    }

#if (1 == SMIP_CFG_PARAM_CHECKING_ENABLE)

    if (ch >= SMIP_PRV_MAX_SPI_CH)
    {
        return (SMIP_ERROR_PARAMETER);
    }

    if (NULL == p_info) /* @suppress("Cast comment") */
    {
        return (SMIP_ERROR_PARAMETER);
    }
    
#if (0 == SMIP_CFG_EXTMODE)
    if (0 != ((p_info->type) & SMIP_PRV_MASK_SEND_SYNC))
    {
#endif
        if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq) /* @suppress("Cast comment") */
        {
            return (SMIP_ERROR_SYSTEM_SETTING);
        }

#if (0 == SMIP_CFG_VCOM_TIMER_CLOCK)
        if(1U == SYSTEM->SOSCCR_b.SOSTP)
        {
            /* Returns an error if the subclock is not oscillating */
            return (SMIP_ERROR);
        }
#else
        if(1U == SYSTEM->LOCOCR_b.LCSTP)     /* @suppress("Cast comment") */
        {
            /* Returns an error if the LOCO is not oscillating */
            return (SMIP_ERROR);
        }
#endif

#if (0 == SMIP_CFG_EXTMODE)
    }
#endif
    
    if (0x3FFF < (p_info->disp_width))
    {
        /* An error occurs because an overflow occurs in the following operation */
        return (SMIP_ERROR);
    }
    
#endif /* (1 == SMIP_CFG_PARAM_CHECKING_ENABLE) */

    /* Calculation hedder size */
    temp = p_info->mode_size;
    temp += p_info->mode_dmy_size;
    temp += p_info->address_size;
    temp += 7;                                          /* Addition to round up */
    gs_smip_info.header_size = (uint8_t)(temp >> 3);    /* Convert from number of bits to number of bytes */

#if (1 == SMIP_CFG_PARAM_CHECKING_ENABLE)
    if (SMIP_CFG_COMMAND_MAX_BYTE < gs_smip_info.header_size)
    {
        return (SMIP_ERROR);
    }
#endif

    /* Calculate timer value of VCOM cycle */
    agt_value = (SMIP_PRV_AGT_CSOURCE * 10) / (p_info->vcom_clk * 2);

#if (1 == SMIP_CFG_PARAM_CHECKING_ENABLE)
    /* Over 16bit size */
    if (0xFFFF < agt_value)
    {
        return (SMIP_ERROR);
    }
#endif
    gs_vcom_count = (uint16_t)agt_value; /* @suppress("Cast comment") */

    /* Calculate timer value of thVCOM */
    agt_value = SMIP_PRV_AGT_CSOURCE;
    agt_value *= (p_info->th_vcom);
    agt_value /= 1000;                      /* (ms) */
#if (1 == SMIP_CFG_PARAM_CHECKING_ENABLE)
    /* Over 16bit size */
    if (0xFFFF < agt_value)
    {
        return (SMIP_ERROR);
    }
#endif
    gs_th_vcom = (uint16_t)agt_value; /* @suppress("Cast comment") */
    
    /* Calculate timer value of tsVCOM */
    agt_value = SMIP_PRV_AGT_CSOURCE;
    agt_value *= (p_info->ts_vcom);
    agt_value /= 1000;                      /* (ms) */
#if (1 == SMIP_CFG_PARAM_CHECKING_ENABLE)
    /* Over 16bit size */
    if (0xFFFF < agt_value)
    {
        return (SMIP_ERROR);
    }
#endif
    gs_ts_vcom = (uint16_t)agt_value; /* @suppress("Cast comment") */

    
    /* Calculation body size(write or black) */
    temp = p_info->disp_width;
    temp += 7;
    gs_smip_info.body_mono_size = (uint8_t)(temp >> 3); /* @suppress("Cast comment") */
    
    /* Calculation body size(color) */
    temp = p_info->disp_width;
    if (0 == (p_info->type & SMIP_PRV_MASK_COLOR_MODE))
    {
        /* 3bit color */
        temp = temp * 3;
    }
    else
    {
        /* 4bit color */
        temp = temp * 4;
    }
    temp += 7;
    gs_smip_info.body_color_size = (uint8_t)(temp >> 3); /* @suppress("Cast comment") */
    
    /* Calculation footer size */
    temp = p_info->end_dummy_size;
    temp += 7;
    gs_smip_info.footer_size = (uint8_t)(temp >> 3); /* @suppress("Cast comment") */

#if (0 == SMIP_CFG_EXTMODE)
    if (0 != ((p_info->type) & SMIP_PRV_MASK_SEND_SYNC))
    {
#endif
        /* Register agti interrupt handler */
        if ((-1) == R_SYS_IrqEventLinkSet(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq, 
                                          gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_iesr_val, r_smip_cb_vcom_timer_event))
        {
            return (SMIP_ERROR_SYSTEM_SETTING);
        }
    
        /* SPTI priority setting */
        R_NVIC_SetPriority(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq, SMIP_CFG_VCOM_TIMER_INT_PRIORITY);
        if (R_NVIC_GetPriority (gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq) != SMIP_CFG_VCOM_TIMER_INT_PRIORITY)
        {
            return (SMIP_ERROR_SYSTEM_SETTING);
        }
#if (0 == SMIP_CFG_EXTMODE)
    }
#endif

    /* Get SPI driver */
    if (0 == ch)
    {
        gsp_spi_drv = &Driver_SPI0;
    }
    else
    {
        gsp_spi_drv = &Driver_SPI1;
    }

    
    /* SPI driver initialize */
    if (ARM_DRIVER_OK != gsp_spi_drv->Initialize(r_smip_cb_spi_event))
    {
        return (SMIP_ERROR);
    }
    
    /* SPI driver power on */
    if (ARM_DRIVER_OK != gsp_spi_drv->PowerControl(ARM_POWER_FULL))
    {
        (void)gsp_spi_drv->Uninitialize(); /* @suppress("Cast comment") */
        return (SMIP_ERROR);
    }
    
    /* SPI */
    if (ARM_DRIVER_OK != gsp_spi_drv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | SMIP_PRV_SPI_BIT_ORDER |
                                   ARM_SPI_SS_MASTER_UNUSED | ARM_SPI_DATA_BITS(8) , (uint32_t)sclk)) /* @suppress("Cast comment") */
    {
        (void)gsp_spi_drv->Uninitialize(); /* @suppress("Cast comment") */
        return (SMIP_ERROR);
    }

    /* Set smip resource  */
    gs_smip_info.cb_event = cb_event;
    gs_smip_info.p_lcd_info = p_info;

    
#if ((1 == SMIP_CFG_EXTMODE) && (0 == SMIP_CFG_AGTO_EN))
    r_smip_vcom_negative();
    SMIP_CFG_VCOM_PORT->PDR = (SMIP_CFG_VCOM_PORT->PDR | (1 << SMIP_CFG_VCOM_PIN)); /* @suppress("Cast comment") */
#endif

    r_smip_scs_negative();
    SMIP_CFG_SCS_PORT->PDR = (SMIP_CFG_SCS_PORT->PDR | (1 << SMIP_CFG_SCS_PIN)); /* @suppress("Cast comment") */

    if (SMIP_PRV_POWER_ON_OFF_JDI == ((p_info->type) & SMIP_PRV_MASK_ON_OFF_SEQ))
    {
        r_smip_disp_negative();
    }
    else
    {
        r_smip_rst_negative();
    }
    SMIP_CFG_RST_DISP_PORT->PDR = (SMIP_CFG_RST_DISP_PORT->PDR | (1 << SMIP_CFG_RST_DISP_PIN)); /* @suppress("Cast comment") */


#if (0 == SMIP_CFG_EXTMODE)
    if (0 != ((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_SEND_SYNC))
    {
#endif
        /* VCOM frequency setting(AGTn) */
        
        /* AGTn Resource Lock */
        if (0 != R_SYS_ResourceLock(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].lock_id))
        {
            (void)gsp_spi_drv->Uninitialize(); /* @suppress("Cast comment") */
            return (SMIP_ERROR);
        }
        
        /* Release module stop */
        if (0 != R_LPM_ModuleStart(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].mstp_id))
        {
            R_SYS_ResourceUnlock(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].lock_id);
            (void)gsp_spi_drv->Uninitialize(); /* @suppress("Cast comment") */
            return (SMIP_ERROR);
        }
    
        /* AGT - AGT Counter Register */
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGT = gs_vcom_count;
    
        /* AGTCR - AGT Control Register
        b7   - TCMBF - Compare Match B Flag - No match
        b6   - TCMAF - Compare Match A Flag - No match
        b5   - TUNDF - Underflow Flag - No underflow
        b4   - TEDGF - Active Edge Judgment Flag - No active edge received
        b3   - Resaeved
        b2   - TSTOP - AGT Count Forced Stop 
        b1   - TCSTF - AGT Count Status Flag 
        b0   - TSTART - AGT Count Start - Stop counting  */
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR = 0x00;
    
#if (0 == SMIP_CFG_VCOM_TIMER_CLOCK)
        /* AGTMR1 - AGT Mode Register 1
        b7      - Reserved
        b6 : b4 - TCK    - Count Source - Divided clock AGTSCLK specified by CKS[2:0] bits in the AGTMR2 register.
        b3      - TEDGPL - Edge Polarity - Single-edge
        b2 : b0 - TMOD   - Operating Mode -Timer mode  */
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTMR1 = 0x60;
#else
    
        /* AGTMR1 - AGT Mode Register 1
        b7      - Reserved
        b6 : b4 - TCK    - Count Source - Divided clock AGTLCLK specified by CKS[2:0] bits in the AGTMR2 register
        b3      - TEDGPL - Edge Polarity - Single-edge
        b2 : b0 - TMOD   - Operating Mode -Timer mode  */
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTMR1 = 0x40;
#endif
    
    
        /* AGTMR2 - AGT Mode Register 2
        b7      - LPM - Low Power Consumption Mode Setting - Normal mode
        b6 : b3 - Reserved
        b2 : b0 - CKS - AGTSCLK/AGTLCLK Count Source Clock Frequency Division Ratio - 1/1  */
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTMR2 = 0x00;

        
        /* AGTIOC - AGT I/O Control Register
        b7 : b6 - TIOGT - Count Control - Always count events
        b5 : b4 - TIPF - Input Filter - No filter
        b3      - Reserved
        b2      - TOE - AGTOn Output Enable - Disable AGTOn output
        b1      - Reserved
        b0      - TEDGSEL - I/O Polarity Switch - Start output at low (initial level: low): Normal output */
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC = 0x01;
        
#if (1 == SMIP_CFG_AGTO_EN)
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agt_pin_set();
#endif
    
        R_SYS_IrqStatusClear(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
        R_NVIC_ClearPendingIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
        R_NVIC_EnableIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
    
#if (0 == SMIP_CFG_EXTMODE)
    }
#endif
    
    /** Resource initialize */
    gs_smip_info.flag = SMIP_PRV_FLAG_OPENED;                      /* Driver Opened                  */
    r_smip_state_init();
#if (0 == SMIP_CFG_EXTMODE)
    if (0 != ((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_SEND_SYNC))
    {
#endif
        r_smip_tm_lpm_on();
#if (0 == SMIP_CFG_EXTMODE)
    }
#endif

    return (SMIP_OK);
}/* End of function R_SMIP_Open() */

/****************************************************************************************************************//**
 * @brief       The implementation of Middleware SMIP Close API for the Serial MIP peripheral on RE Family.
 * @retval      SMIP_OK Close successful
**********************************************************************************************************************/
/* Function Name : R_SMIP_Close */
e_smip_err_t R_SMIP_Close(void)
{
    
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_OPENED))
    {
        return (SMIP_OK);
    }

#if (0 == SMIP_CFG_EXTMODE)
    if (0 != ((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_SEND_SYNC))
    {
#endif
        r_smip_tm_lpm_off();
        
        /* AGT Timer stop */
        r_smip_vcom_timer_stop();
        
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTMR1 = 0x00;
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC = 0x00;
        
#if (1 == SMIP_CFG_AGTO_EN)
        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agt_pin_clr();
        
#endif
        
        R_LPM_ModuleStop(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].mstp_id);
        R_SYS_ResourceUnlock(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].lock_id);
        
        R_SYS_IrqStatusClear(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
        R_NVIC_ClearPendingIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
#if (0 == SMIP_CFG_EXTMODE)
    }
#endif

    (void)gsp_spi_drv->Uninitialize(); /* @suppress("Cast comment") */

    /* Initialize port direction */
#if ((1 == SMIP_CFG_EXTMODE) && (0 == SMIP_CFG_AGTO_EN))
        SMIP_CFG_VCOM_PORT->PDR  = (SMIP_CFG_VCOM_PORT->PDR & (~(1 << SMIP_CFG_VCOM_PIN))); /* @suppress("Cast comment") */
        SMIP_CFG_VCOM_PORT->PODR = (SMIP_CFG_VCOM_PORT->PODR & (~(1 << SMIP_CFG_VCOM_PIN))); /* @suppress("Cast comment") */
#endif

    SMIP_CFG_SCS_PORT->PDR  = (SMIP_CFG_SCS_PORT->PDR & (~(1 << SMIP_CFG_SCS_PIN))); /* @suppress("Cast comment") */
    SMIP_CFG_SCS_PORT->PODR = (SMIP_CFG_SCS_PORT->PODR & (~(1 << SMIP_CFG_SCS_PIN))); /* @suppress("Cast comment") */
    SMIP_CFG_RST_DISP_PORT->PDR  = (SMIP_CFG_RST_DISP_PORT->PDR & (~(1 << SMIP_CFG_RST_DISP_PIN))); /* @suppress("Cast comment") */
    SMIP_CFG_RST_DISP_PORT->PODR = (SMIP_CFG_RST_DISP_PORT->PODR & (~(1 << SMIP_CFG_RST_DISP_PIN))); /* @suppress("Cast comment") */
    
    
    gs_smip_info.flag = 0;

    return (SMIP_OK);

}/* End of function R_SMIP_Close() */

/****************************************************************************************************************//**
 * @brief       Process MIP power on sequence.
 * @retval      SMIP_OK Power on sequence processing success
 * @retval      SMIP_ERROR  Power on sequence processing failure
 * @retval      SMIP_ERROR_BUSY Driver is busy
**********************************************************************************************************************/
/* Function Name : R_SMIP_PowerOn */
e_smip_err_t R_SMIP_PowerOn(void) // @suppress("Function length")
{
    e_smip_err_t result = SMIP_OK;
    
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_OPENED))
    {
        return (SMIP_ERROR);
    }

    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED))
    {
        return (SMIP_ERROR);
    }
    
    if ((SMIP_PRV_STATE_FREE_RDY != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_RDY != gs_smip_state))
    {
        return (SMIP_ERROR_BUSY);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_RECONFIG))
    {
        return (SMIP_ERROR);
    }
    
    switch((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_ON_OFF_SEQ)
    {
        case SMIP_PRV_POWER_ON_OFF_JDI:
        {
            if (NULL != gs_smip_info.p_lcd_info->cmd_tbl) /* @suppress("Cast comment") */
            {
                gs_power_on_seq = SMIP_PRV_PWON_BUSY;

                /* T2: All Clear Command and T2 time wait. */
                result = r_smip_send_command_sub(gs_smip_info.p_lcd_info->cmd_tbl[SMIP_PRV_CMD_ALL_CLEAR]);
                if (SMIP_OK == result)
                {
                    result = r_smip_wait_power_on_seq();
                    if (SMIP_OK != result)
                    {
                        break;
                    }
                    R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_memory_init, SYSTEM_DELAY_UNITS_MICROSECONDS);
                    
                    /* T3: DISP pin set active and T3 time wait. */
                    r_smip_disp_active();
                    R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_com_init, SYSTEM_DELAY_UNITS_MICROSECONDS);
                    
                    /* T4: EXTCOMIN Normal Operation and T4 time wait. */
#if (0 == SMIP_CFG_EXTMODE)
                    if (0 != ((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_SEND_SYNC))
                    {
#endif
                        r_smip_tm_lpm_off();
#if (1 == SMIP_CFG_AGTO_EN)
                        /* AGTIOC - AGT I/O Control Register
                        b2      - TOE - AGTOn Output Enable - Enable AGTOn output */
                        gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 1;
#else
                        r_smip_vcom_toggle();
#endif
                        r_smip_vcom_timer_start(gs_vcom_count);
                        r_smip_tm_lpm_on();
#if (0 == SMIP_CFG_EXTMODE)
                    }
#endif


                    R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_com_init, SYSTEM_DELAY_UNITS_MICROSECONDS);

                    gs_smip_info.flag |= SMIP_PRV_FLAG_POWERED;
                }
                else
                {
                    gs_power_on_seq = SMIP_PRV_PWON_ERROR;
                    result = SMIP_ERROR;
                }
            }
            else
            {
                result = SMIP_ERROR;
            }
        }
        break;
        
        case SMIP_PRV_POWER_ON_OFF_KYOCERA:
        {
            gs_power_on_seq = SMIP_PRV_PWON_BUSY;
            result = r_smip_all_write(0x00);
            if (SMIP_OK == result)
            {
                result = r_smip_wait_power_on_seq();
                if (SMIP_OK != result)
                {
                    break;
                }
                
                r_smip_rst_active();

                /* T2: All Clear Command and T2 time wait. */
                R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_com_init, SYSTEM_DELAY_UNITS_MICROSECONDS);
#if (0 == SMIP_CFG_EXTMODE)
                if (0 != ((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_SEND_SYNC))
                {
#endif
                    r_smip_tm_lpm_off();
#if (1 == SMIP_CFG_AGTO_EN)
                    /* AGTIOC - AGT I/O Control Register
                    b2      - TOE - AGTOn Output Enable - Enable AGTOn output */
                    gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 1;
#endif
                    r_smip_vcom_timer_start(gs_vcom_count);
                    r_smip_tm_lpm_on();
#if (0 == SMIP_CFG_EXTMODE)
                }
#endif
                gs_smip_info.flag |= SMIP_PRV_FLAG_POWERED;
            }
            else
            {
                gs_power_on_seq = SMIP_PRV_PWON_ERROR;
                result = SMIP_ERROR;
                break;
            }
        }
        break;
        
        default:
        {
            result = SMIP_ERROR;
        }
        break;
       
    }
    return (result);
}/* End of function R_SMIP_PowerOn() */

/****************************************************************************************************************//**
 * @brief       Process MIP power off sequence.
 * @retval      SMIP_OK         Power off sequence processing success
 * @retval      SMIP_ERROR      Power off sequence processing failure
 * @retval      SMIP_ERROR_BUSY Driver is busy
**********************************************************************************************************************/
/* Function Name : R_SMIP_PowerOff */
e_smip_err_t R_SMIP_PowerOff(void)
{
    e_smip_err_t result = SMIP_OK;
    
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_OPENED))
    {
        return (SMIP_ERROR);
    }

    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED))
    {
        return (SMIP_ERROR);
    }
    
    if ((SMIP_PRV_STATE_FREE_RDY != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_RDY != gs_smip_state))
    {
        return (SMIP_ERROR_BUSY);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_RECONFIG))
    {
        return (SMIP_ERROR);
    }
    
    switch((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_ON_OFF_SEQ)
    {
        case SMIP_PRV_POWER_ON_OFF_JDI:
        {
            if (NULL != gs_smip_info.p_lcd_info->cmd_tbl) /* @suppress("Cast comment") */
            {
                gs_power_on_seq = SMIP_PRV_PWON_BUSY;
                r_smip_tm_lpm_off();
                r_smip_vcom_timer_stop();
#if (1 == SMIP_CFG_AGTO_EN)
                /* AGTIOC - AGT I/O Control Register
                b2      - TOE - AGTOn Output Enable - Disable AGTOn output */
                gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 0;
#endif
                r_smip_tm_lpm_on();
                r_smip_disp_negative();
                r_smip_vcom_negative();
                
                /* T5: All Clear Command and T5 time wait. */
                result = r_smip_send_command_sub(gs_smip_info.p_lcd_info->cmd_tbl[SMIP_PRV_CMD_ALL_CLEAR]);
                if (SMIP_OK == result)
                {
                    result = r_smip_wait_power_on_seq();
                }
                else
                {
                    gs_power_on_seq = SMIP_PRV_PWON_ERROR;
                }
                
                R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_memory_init, SYSTEM_DELAY_UNITS_MICROSECONDS);

                /* T6:  T6 time wait. */
                R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_com_init, SYSTEM_DELAY_UNITS_MICROSECONDS);
                gs_smip_info.flag &= (~SMIP_PRV_FLAG_POWERED);

            }
            else
            {
                result = SMIP_ERROR;
            }
        }
        break;
        
        case SMIP_PRV_POWER_ON_OFF_KYOCERA:
        {
            /* T3: Stop VCOM and T3 time wait. */
            r_smip_tm_lpm_off();
            r_smip_vcom_timer_stop();
#if (1 == SMIP_CFG_AGTO_EN)
            /* AGTIOC - AGT I/O Control Register
            b2      - TOE - AGTOn Output Enable - Disable AGTOn output */
            gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 0;
#endif
            r_smip_tm_lpm_on();
            r_smip_vcom_negative();
            R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_com_init, SYSTEM_DELAY_UNITS_MICROSECONDS);
            
            
            /* T4: RST pin set negative and T4 time wait. */
            r_smip_rst_negative();
            R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->t_memory_init, SYSTEM_DELAY_UNITS_MICROSECONDS);
            gs_smip_info.flag &= (~SMIP_PRV_FLAG_POWERED);
        }
        break;
        
        default:
        {
            result = SMIP_ERROR;
        }
        break;
       
    }
    return (result);
}/* End of function R_SMIP_PowerOff() */

/****************************************************************************************************************//**
 * @brief       Transmission of "0" data to full screen.
 * @retval      SMIP_OK    Successful transmission of "0" data to full screen
 * @retval      SMIP_ERROR Failed to send "0" data to full screen
**********************************************************************************************************************/
/* Function Name : R_SMIP_AllZero */
e_smip_err_t R_SMIP_AllZero(void)
{
    e_smip_err_t result;

    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_RECONFIG))
    {
        return (SMIP_ERROR);
    }

    result = r_smip_all_write(0x00);
    return result;
}/* End of function R_SMIP_AllZero() */

/****************************************************************************************************************//**
 * @brief       Transmission of "1" data to full screen.
 * @retval      SMIP_OK    Successful transmission of "1" data to full screen
 * @retval      SMIP_ERROR Failed to send "1" data to full screen
**********************************************************************************************************************/
/* Function Name : R_SMIP_AllOne */
e_smip_err_t R_SMIP_AllOne(void)
{
    e_smip_err_t result;

    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_RECONFIG))
    {
        return (SMIP_ERROR);
    }

    result = r_smip_all_write(0xFF);
    return result;
}/* End of function R_SMIP_AllOne */

/****************************************************************************************************************//**
 * @brief       Write same value to full screen display.
 *              Write the value of argument to full screen.
 * @param[in]   wr_data Value to write
 * @retval      SMIP_OK    Success to write the same value to full screens
 * @retval      SMIP_ERROR Failure to write the same value to full screens
**********************************************************************************************************************/
/* Function Name : r_smip_all_write */
static e_smip_err_t r_smip_all_write(uint8_t wr_data)
{
    uint16_t all_wr_size;
    e_smip_err_t result = SMIP_OK;
    uint32_t timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;
    uint8_t check_ir;

    /* ALL white write size check(black or wtite) */
    all_wr_size = gs_smip_info.header_size;
    all_wr_size += gs_smip_info.body_mono_size;
    all_wr_size += gs_smip_info.footer_size;

#if (1 == SMIP_CFG_PARAM_CHECKING_ENABLE)
    if (SMIP_CFG_ALL_WRITE_BUFF < all_wr_size)
    {
        return (SMIP_ERROR);
    }
#endif
    switch (gs_smip_state)
    {
        case SMIP_PRV_STATE_FREE_RDY:
        {
            r_smip_set_aw_data(wr_data, all_wr_size);
            gs_smip_state = SMIP_PRV_STATE_FREE_AW_BUSY;
            result = r_smip_start_line_send();
        }    
        break;

        case SMIP_PRV_STATE_ADJUST_RDY:
        {
            r_smip_set_aw_data(wr_data, all_wr_size);
            if (SMIP_PRV_PWON_BUSY == gs_power_on_seq)
            {
                gs_smip_state = SMIP_PRV_STATE_ADJUST_AW_BUSY;
                result = r_smip_start_line_send();
            }
            else
            {
                r_smip_tm_lpm_off();
                r_smip_vcom_timer_stop();
                gs_smip_state = SMIP_PRV_STATE_ADJUST_AW_TS_VCOM;
#if (1 == SMIP_CFG_AGTO_EN)
                /* AGTIOC - AGT I/O Control Register
                b2      - TOE - AGTOn Output Enable - Disable AGTOn output */
                gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 0;
#endif
                
                r_smip_vcom_timer_start(gs_ts_vcom);
                r_smip_tm_lpm_on();
            }
        }
        break;
        
        case SMIP_PRV_STATE_ADJUST_TH_VCOM:
        {
            r_smip_tm_lpm_off();
            gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TSTART = 0;
            while(0 != gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TCSTF)
            {
#if (1 == SYSTEM_CFG_WDT_REFRESH_ENABLE)
                r_system_wdt_refresh();
#endif
                if (0 == timeout)
                {
                    break;
                }
                timeout--;
            }

            r_smip_set_aw_data(wr_data, all_wr_size);
            R_SYS_IrqStatusGet(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq, &check_ir);
            
            /* AGTn AGTI interrupt request clear */
            R_SYS_IrqStatusClear(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
            R_NVIC_ClearPendingIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);

            if ((SMIP_PRV_STATE_ADJUST_TH_VCOM == gs_smip_state) && (0 == check_ir))
            {
                gs_smip_state = SMIP_PRV_STATE_ADJUST_AW_BUSY;
                result = r_smip_start_line_send();
            }
            else
            {
                gs_smip_state = SMIP_PRV_STATE_ADJUST_AW_TS_VCOM;
                r_smip_vcom_timer_stop();
#if (1 == SMIP_CFG_AGTO_EN)
                /* AGTIOC - AGT I/O Control Register
                b2      - TOE - AGTOn Output Enable - Disable AGTOn output */
                gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 0;
#endif
                
                r_smip_vcom_timer_start(gs_ts_vcom);
            }
            r_smip_tm_lpm_on();

        }
        break;

#if (1 == SMIP_CFG_MAKE_LINE_INFO)
        case SMIP_PRV_STATE_FREE_SND_HEADER:
        case SMIP_PRV_STATE_FREE_SND_BODY:
        case SMIP_PRV_STATE_FREE_SND_FOOTER:
        case SMIP_PRV_STATE_ADJUST_SND_HEADER:
        case SMIP_PRV_STATE_ADJUST_SND_BODY:
        case SMIP_PRV_STATE_ADJUST_SND_FOOTER:
#else
        case SMIP_PRV_STATE_FREE_BUSY:
        case SMIP_PRV_STATE_ADJUST_BUSY:
#endif
        case SMIP_PRV_STATE_FREE_AW_BUSY:
        case SMIP_PRV_STATE_FREE_CMD_BUSY:
        case SMIP_PRV_STATE_ADJUST_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_AW_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_AW_BUSY:
        case SMIP_PRV_STATE_ADJUST_CMD_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_CMD_BUSY:
        {
            result = SMIP_ERROR_BUSY;
        }
        break;

        default:
        {
            result = SMIP_ERROR;
        }
        break;
    }
    return (result);

}/* End of function r_smip_all_write */

/****************************************************************************************************************//**
 * @brief       Send data in monochrome display.
 *              Starts the transmission sequence of monochrome data, and displays the data of argument on the LCD.
 * @param[in]   st       Transmission start address (vertical direction)
 * @param[in]   line_num Number of lines of transmission data (number of vertical bits)
 * @param[in]   *data    Transmission data address
 * @retval      SMIP_OK    Succeeded in monochrome data transmission
 * @retval      SMIP_ERROR Failed to send monochrome data
**********************************************************************************************************************/
/* Function Name : R_SMIP_Send */
e_smip_err_t R_SMIP_Send(uint16_t st, uint16_t line_num, const uint8_t *data)
{
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_RECONFIG))
    {
        return (SMIP_ERROR);
    }

    return r_smip_send_seq(st, line_num, gs_smip_info.body_mono_size, data, SMIP_PRV_CMD_UPDATE_MONO);
}/* End of function R_SMIP_Send */

/****************************************************************************************************************//**
 * @brief       Send data in color display
 *              Starts the transmission sequence of color data, and displays the data of argument on the LCD.
 * @param[in]   st       Transmission start address (vertical direction)
 * @param[in]   line_num Number of lines of transmission data (number of vertical bits)
 * @param[in]   *data    Transmission data address
 * @retval      SMIP_OK    Succeeded in monochrome data transmission
 * @retval      SMIP_ERROR Failed to send monochrome data
**********************************************************************************************************************/
/* Function Name : R_SMIP_SendColor */
e_smip_err_t R_SMIP_SendColor(uint16_t st, uint16_t line_num, const uint8_t *data)
{
    uint8_t cmd_id;
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED))
    {
        return (SMIP_ERROR);
    }

    if(0 == (gs_smip_info.p_lcd_info->type & SMIP_PRV_MASK_COLOR_MODE))
    {
        cmd_id = SMIP_PRV_CMD_UPDATE_COLOR_3BIT;
    }
    else
    {
        cmd_id = SMIP_PRV_CMD_UPDATE_COLOR_4BIT;
    } 
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_RECONFIG))
    {
        return (SMIP_ERROR);
    }    
    
    return r_smip_send_seq(st, line_num, gs_smip_info.body_color_size, data, cmd_id);
}/* End of function R_SMIP_SendColor */

/****************************************************************************************************************//**
 * @brief       Execute SMIP data transmission sequence.
 * @param[in]   st        Transmission start address (vertical direction)
 * @param[in]   line_num  Number of lines of transmission data (number of vertical bits)
 * @param[in]   body_size Body size of transmission data
 * @param[in]   *data     Transmission data address
 * @param[in]   cmd_id    Command ID
 * @retval      SMIP_OK SMIP data transmission sequence processing is successful
 * @retval      SMIP_ERROR SMIP data transmission sequence processing fails
 * @retval      SMIP_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      SMIP_ERROR_BUSY Driver is busy
**********************************************************************************************************************/
/* Function Name : r_smip_send_seq */
static e_smip_err_t r_smip_send_seq(uint16_t st, uint16_t line_num, uint16_t body_size, const uint8_t *data, uint8_t cmd_id) // @suppress("Function length")
{
    e_smip_err_t result = SMIP_OK;
    uint8_t check_ir;
    uint32_t result_spi = ARM_DRIVER_OK;
    uint32_t timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;
    
#if (1 == SMIP_CFG_PARAM_CHECKING_ENABLE)
    if ((st+line_num) > gs_smip_info.p_lcd_info->disp_line)
    {
        return (SMIP_ERROR_PARAMETER);
    }
#endif
    
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_OPENED))
    {
        return (SMIP_ERROR);
    }

    switch (gs_smip_state)
    {
        case SMIP_PRV_STATE_FREE_RDY:
        {
            /* SCS output "H" level */
            r_smip_scs_active();
            
            /* Wait for SCS setup time(tsSCS) */
            R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->ts_scs, SYSTEM_DELAY_UNITS_MICROSECONDS);
            r_smip_set_send_info(body_size, st, line_num, data, cmd_id);
#if (1 == SMIP_CFG_MAKE_LINE_INFO)
            r_smip_make_hedder(gs_smip_send_info.line_cur, gs_smip_send_info.hedder_cmd, gs_wr_buff);
            gs_smip_state = SMIP_PRV_STATE_FREE_SND_HEADER;
            result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size);
            if (ARM_DRIVER_OK != result_spi)
            {
                gs_smip_state = SMIP_PRV_STATE_FREE_RDY;
                /* SCS output "L" level */
                r_smip_scs_hold();
                result = SMIP_ERROR;
            }

#else
            gs_smip_state = SMIP_PRV_STATE_FREE_BUSY;
            result_spi = gsp_spi_drv->Send(gs_smip_send_info.data, gs_smip_send_info.send_size);
            
            if (ARM_DRIVER_OK != result_spi)
            {
                gs_smip_state = SMIP_PRV_STATE_FREE_RDY;

                /* SCS output "L" level */
                r_smip_scs_hold();
                result = SMIP_ERROR;
            }
#endif
        }
        break;
        
        case SMIP_PRV_STATE_ADJUST_RDY:
        {
            r_smip_tm_lpm_off();

            /* AGT Timer stop */
            r_smip_vcom_timer_stop();
            
            r_smip_set_send_info(body_size, st, line_num, data, cmd_id);
            gs_smip_state = SMIP_PRV_STATE_ADJUST_TS_VCOM;

#if (1 == SMIP_CFG_AGTO_EN)
            /* AGTIOC - AGT I/O Control Register
            b2      - TOE - AGTOn Output Enable - Disable AGTOn output */
            gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 0;
#endif
            /* tsVCOM time setting and timer start */
            r_smip_vcom_timer_start(gs_ts_vcom);
            r_smip_tm_lpm_on();
        }    
        break;

#if (1 == SMIP_CFG_MAKE_LINE_INFO)
        case SMIP_PRV_STATE_FREE_SND_HEADER:
        case SMIP_PRV_STATE_FREE_SND_BODY:
        case SMIP_PRV_STATE_FREE_SND_FOOTER:
        case SMIP_PRV_STATE_ADJUST_SND_HEADER:
        case SMIP_PRV_STATE_ADJUST_SND_BODY:
        case SMIP_PRV_STATE_ADJUST_SND_FOOTER:
#else
        case SMIP_PRV_STATE_FREE_BUSY:
        case SMIP_PRV_STATE_ADJUST_BUSY:
#endif
        case SMIP_PRV_STATE_FREE_AW_BUSY:
        case SMIP_PRV_STATE_FREE_CMD_BUSY:
        case SMIP_PRV_STATE_ADJUST_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_AW_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_AW_BUSY:
        case SMIP_PRV_STATE_ADJUST_CMD_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_CMD_BUSY:
        {
            result = SMIP_ERROR_BUSY;
        }
        break;
        
        case SMIP_PRV_STATE_ADJUST_TH_VCOM:
        {

            r_smip_tm_lpm_off();
            gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TSTART = 0;
            while(0 != gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TCSTF)
            {
#if (1 == SYSTEM_CFG_WDT_REFRESH_ENABLE)
                r_system_wdt_refresh();
#endif
                if (0 == timeout)
                {
                    break;
                }
                timeout--;
            }

            r_smip_set_send_info(body_size, st, line_num, data, cmd_id);
            R_SYS_IrqStatusGet(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq, &check_ir);
            
            /* AGTn AGTI interrupt request clear */
            R_SYS_IrqStatusClear(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
            R_NVIC_ClearPendingIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);

            if ((SMIP_PRV_STATE_ADJUST_TH_VCOM == gs_smip_state) && (0 == check_ir))
            {
                /* SCS output "H" level */
                r_smip_scs_active();
                /* Wait for SCS setup time(tsSCS) */
                R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->ts_scs, SYSTEM_DELAY_UNITS_MICROSECONDS);

#if (1 == SMIP_CFG_MAKE_LINE_INFO)
                gs_smip_state = SMIP_PRV_STATE_ADJUST_SND_HEADER;
                r_smip_make_hedder(gs_smip_send_info.line_cur, gs_smip_send_info.hedder_cmd, gs_wr_buff);
                result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size);
#else
                gs_smip_state = SMIP_PRV_STATE_ADJUST_BUSY;
                result_spi = gsp_spi_drv->Send(gs_smip_send_info.data, gs_smip_send_info.send_size);
#endif
                if (ARM_DRIVER_OK != result_spi)
                {
                    gs_smip_state = SMIP_PRV_STATE_ADJUST_TH_VCOM;
                    r_smip_scs_hold();
                    result = SMIP_ERROR;
                    gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TSTART = 1;
                }
            }
            else
            {
                gs_smip_state = SMIP_PRV_STATE_ADJUST_TS_VCOM;
#if (1 == SMIP_CFG_AGTO_EN)
                /* AGTIOC - AGT I/O Control Register
                b2      - TOE - AGTOn Output Enable - Disable AGTOn output */
                gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 0;
#endif
                r_smip_vcom_timer_start(gs_ts_vcom);
            }

            r_smip_tm_lpm_on();

        }
        break;
        
        default:
        {
            result = SMIP_ERROR;
        }
        break;
    }
    return result;
}/* End of function r_smip_send_seq */


/****************************************************************************************************************//**
 * @brief       Execute command sending process
 * @param[in]   cmd Command
 * @retval      SMIP_OK     Command transmission success
 * @retval      SMIP_ERROR  Command transmission failure
 * @retval      SMIP_ERROR_BUSY Driver is busy
**********************************************************************************************************************/
/* Function Name : R_SMIP_SendCommand */
e_smip_err_t R_SMIP_SendCommand(uint8_t cmd)
{
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_RECONFIG))
    {
        return (SMIP_ERROR);
    }

    return r_smip_send_command_sub(cmd);
}/* End of function R_SMIP_SendCommand */


/****************************************************************************************************************//**
 * @brief       Command send processing subfunction
 *              Execute common processing of transmission sequence, and transmit display data by SPI transmission.
 * @param[in]   cmd Command
 * @retval      SMIP_OK     Command transmission success
 * @retval      SMIP_ERROR  Command transmission failure
 * @retval      SMIP_ERROR_BUSY Driver is busy
**********************************************************************************************************************/
/* Function Name : r_smip_send_command_sub */
static e_smip_err_t r_smip_send_command_sub(uint8_t cmd) // @suppress("Function length")
{
    int32_t result_spi;
    e_smip_err_t result = SMIP_OK;
    
    if (0 == gs_smip_info.p_lcd_info->mode_size)
    {
        return SMIP_ERROR;
    }
    
    switch (gs_smip_state)
    {
        case SMIP_PRV_STATE_FREE_RDY:
        {
            r_smip_make_cmd(cmd, gs_wr_buff);
            r_smip_scs_active();

            /* Wait for SCS setup time(tsSCS) */
            R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->ts_scs, SYSTEM_DELAY_UNITS_MICROSECONDS);
            gs_smip_state = SMIP_PRV_STATE_FREE_CMD_BUSY;
            result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size);
            if (ARM_DRIVER_OK != result_spi)
            {
                r_smip_scs_hold();
                gs_smip_state = SMIP_PRV_STATE_FREE_RDY;
                result = SMIP_ERROR;
            }
        }
        break;
        
        case SMIP_PRV_STATE_ADJUST_RDY:
        case SMIP_PRV_STATE_ADJUST_TH_VCOM:
        {
            r_smip_make_cmd(cmd, gs_wr_buff);
            if (SMIP_PRV_PWON_BUSY == gs_power_on_seq)
            {
                /* SCS output "H" level */
                r_smip_scs_active();

                /* Wait for SCS setup time(tsSCS) */
                R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->ts_scs, SYSTEM_DELAY_UNITS_MICROSECONDS);
                gs_smip_state = SMIP_PRV_STATE_ADJUST_CMD_BUSY;
                result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size);

                if (ARM_DRIVER_OK != result_spi)
                {
                    r_smip_scs_hold();
                    gs_smip_state = SMIP_PRV_STATE_ADJUST_RDY;
                    result = SMIP_ERROR;
                    
                }
            }
            else
            {
                r_smip_tm_lpm_off();
                r_smip_vcom_timer_stop();
                
                gs_smip_state = SMIP_PRV_STATE_ADJUST_CMD_TS_VCOM;
                r_smip_vcom_timer_start(gs_ts_vcom);
                r_smip_tm_lpm_on();
            }
        }
        break;


#if (1 == SMIP_CFG_MAKE_LINE_INFO)
        case SMIP_PRV_STATE_FREE_SND_HEADER:
        case SMIP_PRV_STATE_FREE_SND_BODY:
        case SMIP_PRV_STATE_FREE_SND_FOOTER:
        case SMIP_PRV_STATE_ADJUST_SND_HEADER:
        case SMIP_PRV_STATE_ADJUST_SND_BODY:
        case SMIP_PRV_STATE_ADJUST_SND_FOOTER:
#else
        case SMIP_PRV_STATE_FREE_BUSY:
        case SMIP_PRV_STATE_ADJUST_BUSY:
#endif
        case SMIP_PRV_STATE_FREE_AW_BUSY:
        case SMIP_PRV_STATE_FREE_CMD_BUSY:
        case SMIP_PRV_STATE_ADJUST_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_AW_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_AW_BUSY:
        case SMIP_PRV_STATE_ADJUST_CMD_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_CMD_BUSY:
        {
            result = SMIP_ERROR_BUSY;
        }
        break;

        default:
        {
            result = SMIP_ERROR;
        }
        break;
    }
    
    return result;
}/* End of function r_smip_send_command_sub */

/****************************************************************************************************************//**
 * @brief       Generate header of transmission data.
 * @param[in]   address Address
 * @param[in]   cmd_id Command ID
 * @param[in]   *buff Write data buffer pointer
**********************************************************************************************************************/
/* Function Name : r_smip_make_hedder */
static void r_smip_make_hedder(uint16_t address, uint8_t cmd_id, uint8_t *buff)
{
    uint32_t temp = 0;
    uint16_t ag_cul_mask;
    uint16_t i;
    
    address += gs_smip_info.p_lcd_info->first_addr; 
    if (0 != (gs_smip_info.p_lcd_info->type & SMIP_PRV_MASK_ADDR_ENDIAN))
    {
        temp = (uint32_t)address;                  /* @suppress("Cast comment") */
    }
    else
    {
        temp = 0;
        ag_cul_mask = 0x0001;
        for (i = 0; i < gs_smip_info.p_lcd_info->address_size; i++)
        {
            temp = temp << 1;
            if (0 != (address & ag_cul_mask))
            {
                temp = temp | 0x00000001;
            }
            ag_cul_mask = ag_cul_mask << 1;
        }
    }
    
    for (i = gs_smip_info.header_size; i > 0; i--)
    {
        buff[i-1] = (uint8_t)(temp & 0x000000FF); /* @suppress("Cast comment") */
        temp = temp >> 8;
    }
    if (NULL != gs_smip_info.p_lcd_info->cmd_tbl) /* @suppress("Cast comment") */
    {
        buff[0] |= (gs_smip_info.p_lcd_info->cmd_tbl[cmd_id]);
    }
    
#if (1 == SMIP_CFG_SPI_BIT_ORDER)
    /* Bit conversion when bit order of SPI communication is LSB  */
    for (i = 0; i < gs_smip_info.header_size;  i++)
    {
        buff[i] = (((buff[i] & 0x55) << 1) | ((buff[i] & 0xAA) >> 1));
        buff[i] = (((buff[i] & 0x33) << 2) | ((buff[i] & 0xCC) >> 2));
        buff[i] = (((buff[i] << 4) & 0xF0) | ((buff[i] >> 4) & 0x0F));
    }

#endif

}/* End of function r_smip_make_hedder */


/****************************************************************************************************************//**
 * @brief       Set transmission information
 * @param[in]   body_size Data body size
 * @param[in]   st        Transmission start address (vertical direction)
 * @param[in]   line_num Number of lines of transmission data (number of vertical bits)
 * @param[in]   cmd_id    Command ID
 * @param[in]   *buff     Write data buffer pointer
 *******************************************************************************************************************/
/* Function Name: r_smip_set_send_info */
static void r_smip_set_send_info(uint16_t body_size, uint16_t st, uint16_t line_num, const uint8_t *data, uint8_t cmd_id) /* @suppress("ISR declaration") */
{
#if (1 == SMIP_CFG_MAKE_LINE_INFO)

    gs_smip_send_info.line_cur = st;
    gs_smip_send_info.line_end = st+line_num-1;
    gs_smip_send_info.data = data + (body_size * st);
    gs_smip_send_info.body_size = body_size;
#else
    uint32_t offset;

    offset = gs_smip_info.header_size + body_size;

    if (0 != (gs_smip_info.p_lcd_info->type & SMIP_PRV_MASK_MULTI_DUMMY_EN))
    {
        offset += gs_smip_info.footer_size;
        gs_smip_send_info.data = data + (offset * st);
        gs_smip_send_info.send_size = offset;
        gs_smip_send_info.send_size *= line_num;
    }
    else
    {
        gs_smip_send_info.data = data + (offset * st);
        gs_smip_send_info.send_size = offset;
        gs_smip_send_info.send_size *= line_num;
        gs_smip_send_info.send_size += gs_smip_info.footer_size;
    }
#endif
    gs_smip_send_info.hedder_cmd = cmd_id;
    
}/* End of function r_smip_set_send_info */


/****************************************************************************************************************//**
 * @brief       Stop ACOM timer for VCOM
 *******************************************************************************************************************/
/* Function Name : r_smip_vcom_timer_stop */
static void r_smip_vcom_timer_stop(void)
{
    uint32_t timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;
    
    /* AGT Timer stop */
    gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TSTART = 0;
    while(0 != gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TCSTF)
    {
#if (1 == SYSTEM_CFG_WDT_REFRESH_ENABLE)
        r_system_wdt_refresh();
#endif
        if (0 == timeout)
        {
            break;
        }
        timeout--;
    }

    /* AGTn AGTI interrupt request clear */
    R_SYS_IrqStatusClear(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
    R_NVIC_ClearPendingIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
    
}/* End of function r_smip_vcom_timer_stop */


/****************************************************************************************************************//**
 * @brief       Start AGT timer for VCOM
 * @param[in]   time Timer setting value.
 *******************************************************************************************************************/
/* Function Name : r_smip_vcom_timer_start */
static void r_smip_vcom_timer_start(uint16_t time)
{
    uint32_t timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;

    gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGT = time;
    gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TSTART = 1;
    while(1 != gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTCR_b.TCSTF)
    {
#if (1 == SYSTEM_CFG_WDT_REFRESH_ENABLE)
        r_system_wdt_refresh();
#endif
        if (0 == timeout)
        {
            break;
        }
        timeout--;
    }
}/* End of function r_smip_vcom_timer_start */

/****************************************************************************************************************//**
 * @brief       Suspend the SPI pin setting
 *******************************************************************************************************************/
/* Function Name : R_SMIP_Suspend */
e_smip_err_t R_SMIP_Suspend(void)
{
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_OPENED))
    {
        return (SMIP_ERROR);
    }
    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        /* Already in Suspend state */
        return (SMIP_OK);
    }
    
    /* Can Suspend be executed? */
    if ((SMIP_PRV_STATE_FREE_RDY       != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_RDY     != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_TH_VCOM != gs_smip_state))
    {
        return (SMIP_ERROR_BUSY);
    }

    /* Temporarily set to Suspend state. */
    gs_smip_info.flag |= SMIP_PRV_FLAG_SUSPEND;
    
    /* Check again if it is in Suspend executable state.
       Suspend is not executed if the status has been changed by an interrupt. */
    if ((SMIP_PRV_STATE_FREE_RDY       != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_RDY     != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_TH_VCOM != gs_smip_state))
    {
        /* Clear the Suspend flag */
        gs_smip_info.flag &= ~SMIP_PRV_FLAG_SUSPEND;
        return (SMIP_ERROR_BUSY);
    }
    
    /* SPI driver power off */
    (void)gsp_spi_drv->PowerControl(ARM_POWER_OFF);
    
    return (SMIP_OK);
}/* End of function R_SMIP_Suspend */
/****************************************************************************************************************//**
 * @brief       Reset the SPI pin setting
 * @param[in]   sclk Transmit clock frequency (Hz)
 *******************************************************************************************************************/
/* Function Name : R_SMIP_Resume */
e_smip_err_t R_SMIP_Resume(uint32_t sclk)
{
    e_smip_err_t result = SMIP_OK;
    
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_OPENED))
    {
        return (SMIP_ERROR);
    }
    
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }
    
    /* SPI driver power on */
    (void)gsp_spi_drv->PowerControl(ARM_POWER_FULL);
    
    /* SPI */
    if (ARM_DRIVER_OK != gsp_spi_drv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | SMIP_PRV_SPI_BIT_ORDER |
                                   ARM_SPI_SS_MASTER_UNUSED | ARM_SPI_DATA_BITS(8) , (uint32_t)sclk)) /* @suppress("Cast comment") */
    {
        (void)gsp_spi_drv->PowerControl(ARM_POWER_OFF);
        result = SMIP_ERROR_PARAMETER;
    }
    else
    {
        gs_smip_info.flag &= ~SMIP_PRV_FLAG_SUSPEND;
    }
    
    return (result);

}/* End of function R_SMIP_Resume */
/****************************************************************************************************************//**
 * @brief       Changing the SPI communication speed setting
 * @param[in]   sclk Transmit clock frequency (Hz)
 *******************************************************************************************************************/
/* Function Name : R_SMIP_ReconfigSpiSpeed */
e_smip_err_t R_SMIP_ReconfigSpiSpeed(uint32_t sclk)
{
    if (0 == (gs_smip_info.flag & SMIP_PRV_FLAG_OPENED))
    {
        return (SMIP_ERROR);
    }

    if (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_SUSPEND))
    {
        return (SMIP_ERROR);
    }

    /* Can Suspend be executed? */
    if ((SMIP_PRV_STATE_FREE_RDY != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_RDY != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_TH_VCOM != gs_smip_state))
    {
        return (SMIP_ERROR_BUSY);
    }   
    
    /* Set the refresh flag.
       SCLK change is performed before the transmission process. */
    gs_smip_info.flag        |= SMIP_PRV_FLAG_RECONFIG;
    
    /* Check again if it is in Suspend executable state.
       Suspend is not executed if the status has been changed by an interrupt. */
    if ((SMIP_PRV_STATE_FREE_RDY != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_RDY != gs_smip_state) &&
        (SMIP_PRV_STATE_ADJUST_TH_VCOM != gs_smip_state))
    {
        /* Clear the Suspend flag */
        gs_smip_info.flag &= ~SMIP_PRV_FLAG_RECONFIG;
        return (SMIP_ERROR_BUSY);
    }
    
    if (ARM_DRIVER_OK != gsp_spi_drv->Control(ARM_SPI_SET_BUS_SPEED, (uint32_t)sclk)) /* @suppress("Cast comment") */
    {
        gs_smip_info.flag &= ~SMIP_PRV_FLAG_RECONFIG;
        return (SMIP_ERROR_PARAMETER);
    }
    
    gs_smip_info.flag &= ~SMIP_PRV_FLAG_RECONFIG;
    return (SMIP_OK);
    
}/* End of function R_SMIP_ReconfigSpiSpeed */

/****************************************************************************************************************//**
 * @brief Callback for SPI Driver
 *        Called when SPI transmission is complete.
 *        Starts header, footer, and line transmission processing according to the state. @n
 *        In the case of final line output, call the callback function of SMIP.
 * @param[in]   event    SPI callback event
 *******************************************************************************************************************/
/* Function Name: r_smip_cb_spi_event */
static void r_smip_cb_spi_event(uint32_t event) /* @suppress("ISR declaration") @suppress("Function length") */
{
    uint32_t result_spi = ARM_DRIVER_OK;
#if (1 == SMIP_CFG_MAKE_LINE_INFO)
    uint8_t i;
#endif

    switch (gs_smip_state)
    {
        
#if (1 == SMIP_CFG_MAKE_LINE_INFO)
        case SMIP_PRV_STATE_FREE_SND_HEADER:
        case SMIP_PRV_STATE_ADJUST_SND_HEADER:
        {
            gs_smip_state++;
            result_spi = gsp_spi_drv->Send(gs_smip_send_info.data, gs_smip_send_info.body_size);
            gs_smip_send_info.data += gs_smip_send_info.body_size;
            if (ARM_DRIVER_OK != result_spi)
            {
                r_smip_send_error_seq();
            }
        }    
        break;

        case SMIP_PRV_STATE_FREE_SND_BODY:
        case SMIP_PRV_STATE_ADJUST_SND_BODY:
        {
            if ((0 == (gs_smip_info.p_lcd_info->type & SMIP_PRV_MASK_MULTI_DUMMY_EN)) &&
                (gs_smip_send_info.line_cur < gs_smip_send_info.line_end))
            {
                if (SMIP_PRV_STATE_FREE_SND_BODY == gs_smip_state)
                {
                    r_smip_header_send(SMIP_PRV_STATE_FREE_SND_HEADER);
                }
                else
                {
                    r_smip_header_send(SMIP_PRV_STATE_ADJUST_SND_HEADER);
                }
                
            }
            else
            {
                for (i = 0; i < gs_smip_info.footer_size; i++)
                {
                    gs_wr_buff[i] = 0;
                }
                gs_smip_state++;
                result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.footer_size);
                if (ARM_DRIVER_OK != result_spi)
                {
                    r_smip_send_error_seq();
                }
            }
        }
        break;
        
        case SMIP_PRV_STATE_FREE_SND_FOOTER:
        {
            if (gs_smip_send_info.line_cur < gs_smip_send_info.line_end)
            {
                r_smip_header_send(SMIP_PRV_STATE_FREE_SND_HEADER);
            }
            else
            {
                r_smip_scs_hold();
                gs_smip_state = SMIP_PRV_STATE_FREE_RDY;
                /* Call callback function  */
                if (NULL != gs_smip_info.cb_event)
                {
                    gs_smip_info.cb_event(SMIP_EVENT_SEND_COMPLETE);
                }
            }
        }
        break;

        case SMIP_PRV_STATE_ADJUST_SND_FOOTER:
        {
            if (gs_smip_send_info.line_cur < gs_smip_send_info.line_end)
            {
                r_smip_header_send(SMIP_PRV_STATE_ADJUST_SND_HEADER);
            }
            else
            {

                /* Wait for SCS hold time(thSCS) */
                r_smip_scs_hold();
                gs_smip_state = SMIP_PRV_STATE_ADJUST_TH_VCOM;
                gs_smip_callback_check();
                if (SMIP_PRV_STATE_ADJUST_TH_VCOM == gs_smip_state)
                {
                    r_smip_tm_lpm_off();
                    /* AGT Timer stop */
                    r_smip_vcom_timer_stop();
                    r_smip_vcom_timer_start(gs_th_vcom);
                    r_smip_tm_lpm_on();
                }
            }
        }
        break;
        
#else /* 0 ==SMIP_CFG_MAKE_LINE_INFO */
        case SMIP_PRV_STATE_FREE_BUSY:
        {
            r_smip_scs_hold();
            gs_smip_state = SMIP_PRV_STATE_FREE_RDY;
            gs_smip_callback_check();
        }
        break;

        case SMIP_PRV_STATE_ADJUST_BUSY:
        {
            r_smip_scs_hold();
            gs_smip_state = SMIP_PRV_STATE_ADJUST_TH_VCOM;
            gs_smip_callback_check();
            if (SMIP_PRV_STATE_ADJUST_TH_VCOM == gs_smip_state)
            {
                r_smip_tm_lpm_off();

                /* AGT Timer stop */
                r_smip_vcom_timer_stop();
                r_smip_vcom_timer_start(gs_th_vcom);
                r_smip_tm_lpm_on();
            }
        }
        break;
        
#endif /* (1 == SMIP_CFG_MAKE_LINE_INFO) */

        case SMIP_PRV_STATE_FREE_CMD_BUSY:
        {
            r_smip_scs_hold();
            gs_smip_state = SMIP_PRV_STATE_FREE_RDY;
            gs_smip_callback_check();
        }
        break;

        case SMIP_PRV_STATE_ADJUST_CMD_BUSY:
        {
            r_smip_scs_hold();
            if (SMIP_PRV_PWON_BUSY == gs_power_on_seq)
            {
                gs_smip_state = SMIP_PRV_STATE_ADJUST_RDY;
            }
            else
            {
                gs_smip_state = SMIP_PRV_STATE_ADJUST_TH_VCOM;
            }
            gs_smip_callback_check();
            
            if (SMIP_PRV_STATE_ADJUST_TH_VCOM == gs_smip_state)
            {
                r_smip_tm_lpm_off();

                /* AGT Timer stop */
                r_smip_vcom_timer_stop();
                r_smip_vcom_timer_start(gs_th_vcom);
                r_smip_tm_lpm_on();
            }
        }
        break;

        case SMIP_PRV_STATE_ADJUST_AW_BUSY:
        case SMIP_PRV_STATE_FREE_AW_BUSY:
        {
            /* Make hedder data */
            gs_aw_cur_addr++;
            
            if (gs_aw_cur_addr >= gs_smip_info.p_lcd_info->disp_line)
            {
                if(SMIP_PRV_STATE_ADJUST_AW_BUSY == gs_smip_state)
                {
                    r_smip_scs_hold();
                    
                    if (SMIP_PRV_PWON_BUSY == gs_power_on_seq)
                    {
                        gs_smip_state = SMIP_PRV_STATE_ADJUST_RDY;
                    }
                    else
                    {
                        gs_smip_state = SMIP_PRV_STATE_ADJUST_TH_VCOM;
                    }
                    gs_smip_callback_check();
                    if (SMIP_PRV_STATE_ADJUST_TH_VCOM == gs_smip_state)
                    {
                        r_smip_tm_lpm_off();
                        
                        /* AGT Timer stop */
                        r_smip_vcom_timer_stop();
                        r_smip_vcom_timer_start(gs_th_vcom);
                        r_smip_tm_lpm_on();
                    }
                }
                else
                {
                    /* Wait for SCS hold time(thSCS) */
                    r_smip_scs_hold();
                    gs_smip_state = SMIP_PRV_STATE_FREE_RDY;
                    gs_smip_callback_check();
                }
            }
            else
            {
                r_smip_make_hedder(gs_aw_cur_addr, gs_smip_send_info.hedder_cmd, gs_wr_buff);
                if ((gs_aw_cur_addr >= (gs_smip_info.p_lcd_info->disp_line-1)) ||
                    (0 != (gs_smip_info.p_lcd_info->type & SMIP_PRV_MASK_MULTI_DUMMY_EN)))
                {
                    result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size + 
                                              gs_smip_info.body_mono_size + gs_smip_info.footer_size);
                }
                else
                {
                    result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size + gs_smip_info.body_mono_size);
                }
            }
            if (ARM_DRIVER_OK != result_spi)
            {
                r_smip_send_error_seq();
            }
        }
        break;    
        
        default:
        {
            /* Wait for SCS hold time(thSCS) */
            r_smip_scs_hold();
            r_smip_state_init();
        }
        break;
    }


}/* End of function r_smip_cb_spi_event() */

/****************************************************************************************************************//**
 * @brief Callback for AGTn AGTI
 *        Call by AGTI interrupt. Control VCOM and SCS output according to the state and mode.
 *******************************************************************************************************************/
/* Function Name: r_smip_cb_vcom_timer_event */
static void r_smip_cb_vcom_timer_event(void) // @suppress("Function length")
{
    uint32_t result_spi = ARM_DRIVER_OK;

    switch (gs_smip_state)
    {
        case SMIP_PRV_STATE_ADJUST_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_AW_TS_VCOM:
        case SMIP_PRV_STATE_ADJUST_CMD_TS_VCOM:
        {
            r_smip_tm_lpm_off();

            /* Timer stop */
            r_smip_vcom_timer_stop();
            r_smip_tm_lpm_on();

            /* SCS output "H" level */
            r_smip_scs_active();

            /* Wait for SCS setup time(tsSCS) */
            R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->ts_scs, SYSTEM_DELAY_UNITS_MICROSECONDS);

            
            if (SMIP_PRV_STATE_ADJUST_TS_VCOM == gs_smip_state)
            {
#if (1 == SMIP_CFG_MAKE_LINE_INFO)
                r_smip_make_hedder(gs_smip_send_info.line_cur, gs_smip_send_info.hedder_cmd, gs_wr_buff);
                gs_smip_state = SMIP_PRV_STATE_ADJUST_SND_HEADER;
                result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size);

#else
                gs_smip_state = SMIP_PRV_STATE_ADJUST_BUSY;
                result_spi = gsp_spi_drv->Send(gs_smip_send_info.data, gs_smip_send_info.send_size);
#endif
            }
            else if (SMIP_PRV_STATE_ADJUST_AW_TS_VCOM == gs_smip_state)
            {
                /* All Zero or All One */
                gs_smip_state = SMIP_PRV_STATE_ADJUST_AW_BUSY;
                if (0 == (gs_smip_info.p_lcd_info->type & SMIP_PRV_MASK_MULTI_DUMMY_EN))
                {
                    result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size + gs_smip_info.body_mono_size);
                }
                else
                {
                    result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size + 
                                              gs_smip_info.body_mono_size + gs_smip_info.footer_size);
                }
            }
            else
            {
                gs_smip_state = SMIP_PRV_STATE_ADJUST_CMD_BUSY;
                result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size);
                
            }
            if (ARM_DRIVER_OK != result_spi)
            {
                r_smip_send_error_seq();
            }
        }
        break;

        case SMIP_PRV_STATE_ADJUST_TH_VCOM:
        {
            r_smip_tm_lpm_off();
            r_smip_vcom_timer_stop();
            r_smip_vcom_toggle();
            gs_smip_state = SMIP_PRV_STATE_ADJUST_RDY;
#if (1 == SMIP_CFG_AGTO_EN)
            /* AGTIOC - AGT I/O Control Register
            b2      - TOE - AGTOn Output Enable - Enable AGTOn output */
            gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTIOC_b.TOE = 1;
#endif
            r_smip_vcom_timer_start(gs_vcom_count);
            r_smip_tm_lpm_on();
        }
        break;
        
        default:
        {
            r_smip_vcom_toggle();
        }
        break;
    }
}/* End of function r_smip_cb_vcom_timer_event */

/****************************************************************************************************************//**
 * @brief       Turn on timer LPM
 *******************************************************************************************************************/
/* Function Name : r_smip_tm_lpm_on */
static void r_smip_tm_lpm_on(void)
{
    
#if (1 == SMIP_CFG_VCOM_TIMER_LPM)
    /* AGTMR2 - AGT Mode Register 2
    b7      - LPM - Low Power Consumption Mode Setting - Low power consumption mode */
    gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTMR2_b.LPM = 1;
    R_SYS_SoftwareDelay(SMIP_PRV_AGT_LPM_WAIT, SYSTEM_DELAY_UNITS_MICROSECONDS);
#endif

    /* AGTI interrupt enable */
    R_NVIC_EnableIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);

}/* End of function r_smip_tm_lpm_on */

/****************************************************************************************************************//**
 * @brief       Turn timer LPM off
 *******************************************************************************************************************/
/* Function Name : r_smip_tm_lpm_off */
static void r_smip_tm_lpm_off(void)
{

    /* AGTI interrupt disable */
    R_NVIC_DisableIRQ(gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].agti_irq);
#if (1 == SMIP_CFG_VCOM_TIMER_LPM)
    /* AGTMR2 - AGT Mode Register 2
    b7      - LPM - Low Power Consumption Mode Setting - Normal mode.  */
    gs_vcom_timer[SMIP_CFG_VCOM_TIMER_CH].reg->AGTMR2_b.LPM = 0;
    R_SYS_SoftwareDelay(SMIP_PRV_AGT_LPM_WAIT, SYSTEM_DELAY_UNITS_MICROSECONDS);
#endif
    
}/* End of function r_smip_tm_lpm_off */


/****************************************************************************************************************//**
 * @brief       Wait for power on sequence
 * @retval      SMIP_OK    Power on sequence processing success
 * @retval      SMIP_ERROR_POWER_SEQUENCE Power on sequence processing failure
 * @retval      SMIP_ERROR_TIMEOUT A timeout occurred in the power on busy count.
 *******************************************************************************************************************/
/* Function Name : r_smip_wait_power_on_seq */
static e_smip_err_t r_smip_wait_power_on_seq(void)
{
    e_smip_err_t result = SMIP_OK;
    uint32_t timeout;

    timeout = SMIP_CFG_TIMEOUT * 100;
    while(SMIP_PRV_PWON_BUSY == gs_power_on_seq)
    {
        if (0 == timeout)
        {
            break;
        }
        R_SYS_SoftwareDelay(10, SYSTEM_DELAY_UNITS_MICROSECONDS);
#if (1 == SYSTEM_CFG_WDT_REFRESH_ENABLE)
        r_system_wdt_refresh();
#endif
        timeout --;
    }
    if (SMIP_PRV_PWON_SUCCESS == gs_power_on_seq)
    {
        result = SMIP_OK;   
    }
    else if (SMIP_PRV_PWON_ERROR == gs_power_on_seq)
    {
        result = SMIP_ERROR_POWER_SEQUENCE;
    }
    else
    {
        r_smip_state_init();
        gs_power_on_seq = SMIP_PRV_PWON_ERROR;
        result = SMIP_ERROR_TIMEOUT;
    }
    return result;
}/* End of function r_smip_wait_power_on_seq */

/****************************************************************************************************************//**
 * @brief       Generate SMIP command
 * @param[in]   cmd      Command to generate
 * @param[in]   *buff    Pointer to the storage destination buffer for generated commands
 *******************************************************************************************************************/
/* Function Name : r_smip_make_cmd */
static void r_smip_make_cmd(uint8_t cmd, uint8_t *buff)
{
    uint8_t i;
    buff[0] = cmd;
    for (i = 1; i < gs_smip_info.header_size; i++)
    {
        buff[i] = 0x00;
    }
}/* End of function r_smip_make_cmd */


/****************************************************************************************************************//**
 * @brief       Wait for SCS signal hold time
 *******************************************************************************************************************/
/* Function Name : r_smip_scs_hold */
static void r_smip_scs_hold(void)
{
    /* Wait for SCS hold time(thSCS) */
    R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->th_scs, SYSTEM_DELAY_UNITS_MICROSECONDS);
    r_smip_scs_negative();

#if (1 == SMIP_CFG_SCS_WAIT_EN)
    /* Wait for SCS low witdh */
    R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->tw_scsl, SYSTEM_DELAY_UNITS_MICROSECONDS);
#endif              

}/* End of function r_smip_scs_hold */

/****************************************************************************************************************//**
 * @brief       Check for SMIP callback events
 *******************************************************************************************************************/
/* Function Name : gs_smip_callback_check */
static void gs_smip_callback_check(void)
{
    if (SMIP_PRV_PWON_BUSY == gs_power_on_seq)
    {
        gs_power_on_seq = SMIP_PRV_PWON_SUCCESS;
    }
    else
    {
        /* Call callback function  */
        if (NULL != gs_smip_info.cb_event)
        {
            gs_smip_info.cb_event(SMIP_EVENT_SEND_COMPLETE);
        }
    }
}/* End of function gs_smip_callback_check */



/****************************************************************************************************************//**
 * @brief       SMIP error sequence processing.@n
 *              Depending on the state, call the callback function by state initialization or error event
 *******************************************************************************************************************/
/* Function Name : r_smip_send_error_seq */
static void r_smip_send_error_seq(void)
{
    r_smip_scs_hold();
    
    if (SMIP_PRV_PWON_BUSY == gs_power_on_seq)
    {
        gs_power_on_seq = SMIP_PRV_PWON_ERROR;
        r_smip_state_init();
    }
    else
    {
        r_smip_state_init();
        if ((SMIP_PRV_STATE_ADJUST_RDY == gs_smip_state) && 
            (0 != (gs_smip_info.flag & SMIP_PRV_FLAG_POWERED)))
        {
            gs_smip_state = SMIP_PRV_STATE_ADJUST_TH_VCOM;
        }
        
        
        /* Call callback function  */
        if (NULL != gs_smip_info.cb_event)
        {
            gs_smip_info.cb_event(SMIP_EVENT_SEND_ERROR);
        }
        
        if (SMIP_PRV_STATE_ADJUST_TH_VCOM == gs_smip_state) 
        {
            r_smip_tm_lpm_off();
            r_smip_vcom_timer_stop();
            r_smip_vcom_timer_start(gs_th_vcom);
            r_smip_tm_lpm_on();
        }
    }
    
}/* End of function r_smip_send_error_seq */


#if (1 == SMIP_CFG_MAKE_LINE_INFO)

/****************************************************************************************************************//**
 * @brief       Send header and transition to the state specified by the argument.
 * @param[in]   next_state     Transition destination state
 *******************************************************************************************************************/
/* Function Name : r_smip_header_send */
static void r_smip_header_send(e_smip_state_t next_state)
{
    uint32_t result_spi = ARM_DRIVER_OK;

    gs_smip_send_info.line_cur++;
    r_smip_make_hedder(gs_smip_send_info.line_cur, gs_smip_send_info.hedder_cmd, gs_wr_buff);
    gs_smip_state = next_state;
    result_spi = gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size);
    if (ARM_DRIVER_OK != result_spi)
    {
        r_smip_send_error_seq();
    }
}/* End of function r_smip_header_send */
#endif

/****************************************************************************************************************//**
 * @brief       Initialize State information to READY state
 *******************************************************************************************************************/
/* Function Name : r_smip_state_init */
static void r_smip_state_init(void)
{
    if(0 != ((gs_smip_info.p_lcd_info->type) & SMIP_PRV_MASK_SEND_SYNC))
    {
        gs_smip_state = SMIP_PRV_STATE_ADJUST_RDY;
    }
    else
    {
        gs_smip_state = SMIP_PRV_STATE_FREE_RDY;
    }
    
}/* End of function r_smip_state_init */


/****************************************************************************************************************//**
 * @brief       Output start line
 * @retval      SMIP_OK     Successful startline transmission
 * @retval      SMIP_ERROR  Start line transmission failure
 *******************************************************************************************************************/
/* Function Name : r_smip_start_line_send */
static e_smip_err_t r_smip_start_line_send(void)
{
    e_smip_err_t result = SMIP_OK;

    /* SCS output "H" level */
    r_smip_scs_active();
    
    /* Wait for SCS setup time(tsSCS) */
    R_SYS_SoftwareDelay(gs_smip_info.p_lcd_info->ts_scs, SYSTEM_DELAY_UNITS_MICROSECONDS);
    
    if (0 == (gs_smip_info.p_lcd_info->type & SMIP_PRV_MASK_MULTI_DUMMY_EN))
    {
        if (ARM_DRIVER_OK != gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size + gs_smip_info.body_mono_size))
        {
            result = SMIP_ERROR;
        }
    }
    else
    {
        if (ARM_DRIVER_OK != gsp_spi_drv->Send(gs_wr_buff, gs_smip_info.header_size + 
                                          gs_smip_info.body_mono_size + gs_smip_info.footer_size))
        {
            result = SMIP_ERROR;
        }
    }
    if (SMIP_OK != result)
    {
        r_smip_scs_hold();
    }
    
    return result;

}/* End of function r_smip_start_line_send */

/****************************************************************************************************************//**
 * @brief       Set the data when sending all data the same value
 * @param[in]   wr_data Value to write
 * @param[in]   all_wr_size Total write data size
 *******************************************************************************************************************/
/* Function Name : r_smip_set_aw_data */
static void r_smip_set_aw_data(uint8_t wr_data, uint16_t all_wr_size)
{
    uint16_t i;
    
    /* All write buffer initialize    */
    /*  Header is 0 clear             */
    /*  Data area is 0 clear or 1 set */
    /*  Dmuuy area is 0 clear         */
    r_smip_make_hedder(0, SMIP_PRV_CMD_UPDATE_MONO, gs_wr_buff);

    for (i = gs_smip_info.header_size; i < (gs_smip_info.header_size + gs_smip_info.body_mono_size); i++)
    {
        gs_wr_buff[i] = wr_data;
    }

    for ( ; i < all_wr_size; i++)
    {
        gs_wr_buff[i] = 0x00;
    }
    gs_aw_cur_addr = 0;
    
}/* End of function r_smip_set_aw_data */


/******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_smip)
 *********************************************************************************************************************/
