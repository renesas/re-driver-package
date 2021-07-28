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
* Copyright (C) 2019-2021 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_pmip_api.c
* Version      : 1.20
* Description  : HAL Driver for PMIP
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description      
*         : 20.09.2019 0.40     Based on RE01 1500KB
*         : 30.01.2020 0.80     Modified the wait time of R_PMIP_PowerOff function from 1ms to 2ms.
*                               VCOM wait time changed from software wait to hardware wait.
*                               Configurable value of full screen rewrite data transmitted during power-on sequence.
*                               Fixed RAM to place gs_pmip_resources variable to .ramdata
*         : 23.04.2020 1.00     Fixed not to include header file when DMA is not used.
*         : 27.08.2020 1.10     Corrected comments in r_pmip_cfg.h according to "User's Manual:Hardware(R01UH0894)"
*         : 12.01.2021 1.20     Add R_PMIP_Reconfig().
**********************************************************************************************************************/
/******************************************************************************************************************//**
 * @addtogroup grp_device_hal_pmip
 * @{
 *********************************************************************************************************************/
#include "pin.h"
#include <r_pmip_api.h>
/******************************************************************************
 Macro definitions
*****************************************************************************/
#define PMIP_PRV_VCLK_PERIOD        (488)           /*!< VCLK period 488us */
#define PMIP_PRV_FMASK_VAL          ((PMIP_CFG_FMASK_US/PMIP_PRV_VCLK_PERIOD)+2)    /*!< VCOM hardware mask time(FMASK) */
#define PMIP_PRV_BMASK_VAL          ((PMIP_CFG_BMASK_US/PMIP_PRV_VCLK_PERIOD)+2)    /*!< VCOM hardware mask time(BMASK) */
#define PMIP_PRV_LIMIT_PCLKA        (8000)          /*!< PCLKA lower limit when using HW mask of VCOM.(8kHz) */
#if (0 == PMIP_CFG_DISP_INI_DATA)
  #define PMIP_PRV_INI_DATA         (0x00)          /*!< Full screen rewrite data sent during power-on sequence is All 0 */
#else
  #define PMIP_PRV_INI_DATA         (0xFF)          /*!< Full screen rewrite data sent during power-on sequence is All 1 */
#endif

/*****************************************************************************
Global variable
******************************************************************************/
#if (PMIP_CFG_R_PMIP_GETVERSION == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_OPEN       == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_CLOSE      == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_POWERON    == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_POWEROFF   == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_CONTROL    == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_ALLZERO    == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_ALLONE     == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_SEND       == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_SENDTRIM   == SYSTEM_SECTION_RAM_FUNC) || \
    (PMIP_CFG_R_PMIP_RECONFIG   == SYSTEM_SECTION_RAM_FUNC)

#define DATA_LOCATION_PRV_PMIP_RESOURCES     __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_PMIP_RESOURCES
#endif

#if (PMIP_CFG_V_MLCD_CLEAR_REG == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_MLCD_CLEAR_REG     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_MLCD_CLEAR_REG
#endif

#if (PMIP_CFG_E_MLCD_PRIORITY_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_MLCD_PRIORITY_SET     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_E_MLCD_PRIORITY_SET
#endif

#if (PMIP_CFG_V_VCOM_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_VCOM_SET     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_VCOM_SET
#endif

#if (PMIP_CFG_E_MLCD_DMAC_CH_SEL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_MLCD_DMAC_CH_SEL     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_E_MLCD_DMAC_CH_SEL
#endif

#if (PMIP_CFG_E_MLCD_ALLSET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_MLCD_ALLSET     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_E_MLCD_ALLSET
#endif

#if (PMIP_CFG_E_MLCD_DMAC_NORMAL_TRANS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_MLCD_DMAC_NORMAL_TRANS     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_E_MLCD_DMAC_NORMAL_TRANS
#endif

#if (PMIP_CFG_E_MLCD_CPU_NORMAL_TRANS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_MLCD_CPU_NORMAL_TRANS     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_E_MLCD_CPU_NORMAL_TRANS
#endif

#if (PMIP_CFG_V_MLCD_NORMAL_DMAC_PRE_SET == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_MLCD_NORMAL_DMAC_PRE_SET     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_MLCD_NORMAL_DMAC_PRE_SET
#endif

#if (PMIP_CFG_V_MLCD_DMAC_DATA_TRANS == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_MLCD_DMAC_DATA_TRANS     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_MLCD_DMAC_DATA_TRANS
#endif

#if (PMIP_CFG_V_CPU_TRANS_NORMAL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_CPU_TRANS_NORMAL     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_CPU_TRANS_NORMAL
#endif

#if (PMIP_CFG_E_SEND_CHECK == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_E_SEND_CHECK     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_E_SEND_CHECK
#endif

#if (PMIP_CFG_V_ISR_DMAC_NORMAL_TEMI == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_NORMAL_TEMI     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_ISR_DMAC_NORMAL_TEMI
#endif

#if (PMIP_CFG_V_ISR_DMAC_TEI == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_DMAC_TEI     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_ISR_DMAC_TEI
#endif

#if (PMIP_CFG_V_ISR_CPU_NORMAL_TEMI == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_NORMAL_TEMI     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_ISR_CPU_NORMAL_TEMI
#endif

#if (PMIP_CFG_V_ISR_CPU_TEI == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_V_ISR_CPU_TEI     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_V_ISR_CPU_TEI
#endif

/******************************************************************************************************************//**
 * common internal function
 *********************************************************************************************************************/
static void v_mlcd_clear_reg(void)FUNC_LOCATION_PRV_V_MLCD_CLEAR_REG;
static e_pmip_err_t e_mlcd_priority_set(void)FUNC_LOCATION_PRV_E_MLCD_PRIORITY_SET;
static void v_vcom_set(void)FUNC_LOCATION_PRV_V_VCOM_SET;
static e_pmip_err_t e_mlcd_cpu_normal_trans(uint32_t in_addr, uint32_t out_addr, uint8_t size_h, uint16_t size_v, uint32_t line_memsize)FUNC_LOCATION_PRV_E_MLCD_CPU_NORMAL_TRANS;
static e_pmip_err_t e_mlcd_allset(uint8_t set_data)FUNC_LOCATION_PRV_E_MLCD_ALLSET;
static void v_cpu_trans_normal(uint32_t src_addr,uint32_t dest_addr, uint32_t total_data_num)FUNC_LOCATION_PRV_V_CPU_TRANS_NORMAL;
static e_pmip_err_t e_send_check(uint8_t st_h, uint8_t st_v, uint8_t size_h, uint16_t size_v, 
                                 uint32_t line_memsize)FUNC_LOCATION_PRV_E_SEND_CHECK;
static void v_isr_cpu_normal_temi(void)FUNC_LOCATION_PRV_V_ISR_CPU_NORMAL_TEMI;
static void v_isr_cpu_tei(void)FUNC_LOCATION_PRV_V_ISR_CPU_TEI;

#if (PMIP_CFG_SUPPORT_DMAC == 1)
static e_pmip_err_t e_mlcd_dmac_ch_sel(uint8_t ch_sel)FUNC_LOCATION_PRV_E_MLCD_DMAC_CH_SEL;
static e_pmip_err_t e_mlcd_dmac_normal_trans(uint32_t in_addr, uint32_t out_addr, uint8_t size_h, uint16_t size_v, uint32_t line_memsize)FUNC_LOCATION_PRV_E_MLCD_DMAC_NORMAL_TRANS;

static e_pmip_err_t e_mlcd_normal_dmac_pre_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                                       uint32_t dmamd_dm, uint32_t dmtmd_dts, 
                                       uint32_t data_length, uint32_t transfer_count,
                                       uint32_t offset,
                                       uint32_t inaddr, uint32_t outaddr)FUNC_LOCATION_PRV_V_MLCD_NORMAL_DMAC_PRE_SET;

static e_pmip_err_t e_mlcd_dmac_data_trans(uint32_t data_length, uint32_t transfer_count, 
                                   uint32_t offset,
                                   uint32_t inaddr, uint32_t outaddr)FUNC_LOCATION_PRV_V_MLCD_DMAC_DATA_TRANS;

static void v_isr_dmac_normal_temi(void)FUNC_LOCATION_PRV_V_ISR_DMAC_NORMAL_TEMI;
static void v_isr_dmac_tei(void)FUNC_LOCATION_PRV_V_ISR_DMAC_TEI;
#endif

/** MLCD mode infomation */
static st_mlcd_mode_info_t gs_mlcd_info = {0};

/** PMIP resources */
static st_pmip_resources_t gs_pmip_resources DATA_LOCATION_PRV_PMIP_RESOURCES= 
{
    /* PMIP resources */
   (MLCD_Type*)MLCD,   /* PMIP  register base address */
   (ICU_Type*) ICU,    /* ICU   register base address */
   (MSTP_Type*)MSTP,   /* MSTP  register base address */
   LPM_MSTP_MLCD,
   PMIP_CFG_INTERRUPT_LEVEL,
   &gs_mlcd_info,      /* PMIP Driver flags initial      */
};

/** save transfer mode */
static uint8_t gs_trans_mode;
static uint32_t gs_size_h;
static uint16_t gs_size_v;
static uint32_t gs_in_addr;
static uint32_t gs_out_addr;
static uint32_t gs_line_memsize;

static uint16_t gs_temi_cnt;

#if (PMIP_CFG_SUPPORT_DMAC == 1)
static DRIVER_DMA *gsp_mlcd_dmacdev;
extern DRIVER_DMA Driver_DMAC0; 
extern DRIVER_DMA Driver_DMAC1; 
extern DRIVER_DMA Driver_DMAC2; 
extern DRIVER_DMA Driver_DMAC3; 
#endif
/********************************************************************//**
* @brief Get API Version
* @retval version Driver version number
***********************************************************************/
/* Function Name : R_PMIP_GetVersion */
uint32_t R_PMIP_GetVersion(void)
{
    uint32_t const version = (PMIP_VERSION_MAJOR << 16) | PMIP_VERSION_MINOR;
    return version;
} /* End of function R_PMIP_GetVersion */

/********************************************************************//**
* @brief PMIP Open
* @param[in] cb                     callback function
* @param[in] tr_mode                transfer mode
* @retval PMIP_OK                   successful operation                            
* @retval PMIP_ERROR                MLCD Open failed
* @retval PMIP_ERROR_SYSTEM_SETTING MLCD system config failed
***********************************************************************/
/* Function Name : R_PMIP_Open */
e_pmip_err_t R_PMIP_Open(pmip_cb_event_t cb, st_transmode_t * tr_mode)
{
#if (PMIP_CFG_SUPPORT_DMAC == 1)
    uint8_t  ch_sel;    
#endif
    uint16_t sckcr;    
    uint32_t iclk_hz;    
    uint32_t enbeg_set_val;
    uint32_t enbw_set_val; 
    uint32_t fmask_set_val;
    uint32_t bmask_set_val;
    uint32_t error;
    e_pmip_err_t result;
    result = PMIP_OK;
 
    if(MLCD_FLAG_OPENED == gs_pmip_resources.info->flags)  /* mlcd opend flags check */
    {
        return PMIP_ERROR;
    }
    error = R_LPM_ModuleStart(gs_pmip_resources.mstp_id);  /* Release module stop, mlcd clock on */
    if(0 != error)
    {
        return PMIP_ERROR;
    }
    result = e_mlcd_priority_set();  /* setting interrupt priority  */
    if(PMIP_OK != result)
    {
        return result;
    }
    gs_pmip_resources.info->flags    = MLCD_FLAG_OPENED; /* resource initialize */
    gs_pmip_resources.info->cb_event = cb;               /* callback setting */

    /* calculation output clk, set sckcr reg */
    R_SYS_SystemClockFreqGet(&iclk_hz);                 /* get the system clock frequency in Hz*/

    if(iclk_hz < PMIP_PRV_LIMIT_PCLKA)                  /* Check PCLKA lower limit when using HW mask of VCOM.(8kHz) */
    {
        return  PMIP_ERROR;
    }
    
    sckcr = (iclk_hz * PMIP_CFG_SCLKH) / 1000000;       /* calculation sckcr value */        
    if((255 < sckcr) || (0 == sckcr) )                  /* check scksr range 1~255 */
    {
        return  PMIP_ERROR_INPUT_CLK_OFFRANGE;
    }        
    gs_pmip_resources.reg_mlcd->MLCDCR_b.SCKCR = sckcr; /* set the transfer clock */

    /* disable VCOM output, and set VCOM High width */
    v_vcom_set();

    /* set ENB period high/low width */
    if(0 == PMIP_CFG_ENB_TBL)
    {        
        if((KYOCERA_ENB <= PMIP_CFG_SCLKH) || (4 > ((KYOCERA_ENB * iclk_hz) / 1000000) )) 
        {
            return PMIP_ERROR_ENBEG_CFG;
        }        
        enbeg_set_val = ((KYOCERA_ENB - PMIP_CFG_SCLKH) * iclk_hz) / 1000000;
        enbw_set_val  = (KYOCERA_ENBH * iclk_hz) / 1000000;
    }
    else
    {        
        if(OTHER_ENB <= PMIP_CFG_SCLKH)
        {
            return PMIP_ERROR_ENBEG_CFG;
        }        
        enbeg_set_val = ((OTHER_ENB - PMIP_CFG_SCLKH) * iclk_hz) / 1000000;
        enbw_set_val  = (OTHER_ENBH * iclk_hz) / 1000000;
    }
    
    /* set FMASK period high/low width */
    fmask_set_val = (((enbw_set_val + (enbeg_set_val * 2) + 2 ) * 2048 * 10) / iclk_hz); /* x10 Cul */
    fmask_set_val += 9;                                 /* Round up */
    fmask_set_val /= 10;
    fmask_set_val += 2;                                 /* Lower Limit */
    
    if (PMIP_PRV_FMASK_VAL > fmask_set_val)
    {
        fmask_set_val = PMIP_PRV_FMASK_VAL;
    }
    bmask_set_val = PMIP_PRV_BMASK_VAL;

    if ((2 > enbeg_set_val) || (enbeg_set_val > 255))    /* check enbeg value range */
    {
        return PMIP_ERROR_ENBEG_CFG;
    }
    if ((2 > enbw_set_val) || (enbw_set_val > 1023)) 
    {
        return PMIP_ERROR_ENBEG_CFG;
    }

    if ((255 < fmask_set_val) || (10 > fmask_set_val))   /* Check fmask value range(10 to 255) */
    {
        return PMIP_ERROR_VCOM_MASK_CFG;
    }

    if ((255 < bmask_set_val) || (4 > bmask_set_val))   /* Check bmask value range(4 to 255) */
    {
        return PMIP_ERROR_VCOM_MASK_CFG;
    }
    
    /* VCOM Hardware Mask Range */
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.BMASK = bmask_set_val;
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.FMASK = fmask_set_val;
    
    gs_pmip_resources.reg_mlcd->MLCDENBCR_b.ENBEG = enbeg_set_val; 
    gs_pmip_resources.reg_mlcd->MLCDENBCR_b.ENBW  = enbw_set_val; 

    /* set trans mode */
    gs_trans_mode = tr_mode->mode;

#if (PMIP_CFG_SUPPORT_DMAC == 1)
    ch_sel        = tr_mode->sel;
#endif

    switch(gs_trans_mode)
    {
    
#if (PMIP_CFG_SUPPORT_DMAC == 1)
        case DMAC_TR: /* DMAC trans mode */
        {        
            result = e_mlcd_dmac_ch_sel(ch_sel); /* DMAC resource is occupied check */
            if(PMIP_OK != result)
            {
                return result;
            }
        }
        break;
#endif
        case CPU_TR: /* CPU trans mode */
        {
            break;
        }

#if PMIP_CFG_SUPPORT_DTC == 1
        case DTC_TR: /* DTC trans mode */
        {  
            return (PMIP_ERROR_DTC_NOT_SUPPORT);
        }
        break;
#endif
        default:    /* No exist trans mode */
        {  
            return (PMIP_ERROR_TRANS_MODE);
        }
    }
    
    R_MLCD_Pinset(); /* call pinseting function */
    
    return result;
} /* End of function R_PMIP_Open */

/********************************************************************//**
* @brief PMIP Close
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed 
***********************************************************************/
/* Function Name : R_PMIP_Close */
e_pmip_err_t R_PMIP_Close(void)
{
    e_pmip_err_t result;
    result = PMIP_OK;
 
#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif

    /* Clear port setting */
    R_MLCD_Pinclr();
    
    /* clear MLCD register */
    v_mlcd_clear_reg();

    /* Setting module stop, mlcd clock off */
    if (0 != R_LPM_ModuleStop(gs_pmip_resources.mstp_id))
    {
        return PMIP_ERROR;
    }

    /* clear mlcd opened flags */
    gs_pmip_resources.info->flags = 0x0;

    return result;
} /* End of function R_PMIP_Close */

/********************************************************************//**
* @brief  PMIP Power On
* @retval PMIP_OK                  successful operation
* @retval PMIP_ERROR               MLCD Open failed
* @note   After executing this function, the HW mask is enabled.
***********************************************************************/
/* Function Name : R_PMIP_PowerOn */
e_pmip_err_t R_PMIP_PowerOn(void)
{
    e_pmip_err_t result;
    result = PMIP_OK;

#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif

    if(0 != (MLCD_FLAG_RECONFIG & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR_RECONFIG;
    }

    /* PMIP Power On sequence  */
    /* white display */
    result = e_mlcd_allset(PMIP_PRV_INI_DATA);
    if(PMIP_OK != result)
    {
        return result;
    }

    /* MLCD panel display enable */
    gs_pmip_resources.reg_mlcd->MLCDCR_b.XRST = 1;

    /* wait 1ms */
    R_SYS_SoftwareDelay(1,SYSTEM_DELAY_UNITS_MILLISECONDS);

    /* enable VCOM ouput */
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.VCOME = 0x1;

    /* enable HW VCOM Mask */
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.HWMSKEN = 0x1;

    return result;
} /* End of function R_PMIP_PowerOn */

/********************************************************************//**
* @brief PMIP Power Off
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed
* @note   After executing this function, the HW mask is disable.
***********************************************************************/
/* Function Name : R_PMIP_PowerOff */
e_pmip_err_t R_PMIP_PowerOff(void)
{
    e_pmip_err_t result;
    uint32_t timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;

    result = PMIP_OK;
 
#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif

    if(0 != (MLCD_FLAG_RECONFIG & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR_RECONFIG;
    }

    /* Check HW mask status. */
    while(0 != gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.HWMSKF)
    {
#if (1 == SYSTEM_CFG_WDT_REFRESH_ENABLE)
        r_system_wdt_refresh();
#endif
        if (0 == timeout)
        {
            result = PMIP_ERROR_TIMEOUT;
        }
        timeout--;
    }
    
    /* disable HW VCOM Mask */
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.HWMSKEN = 0x0;

    /* disable VCOM ouput */
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.VCOME = 0x0;

    /* wait 2ms(Includes 1ms until VCOM goes low) */
    R_SYS_SoftwareDelay(2,SYSTEM_DELAY_UNITS_MILLISECONDS);

    /* MLCD panel display disable */
    gs_pmip_resources.reg_mlcd->MLCDCR_b.XRST = 0;

    /* wait 1ms */
    R_SYS_SoftwareDelay(1,SYSTEM_DELAY_UNITS_MILLISECONDS);

    return result;
} /* End of function R_PMIP_PowerOff */

/********************************************************************//**
* @brief PMIP Control
* @param[in] cmd                   transfer command
* @param[in] set                   MLCDCR register setting BITSW HADDRDEC VADDRDEC
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed
* @retval PMIP_ERROR_CMD           MLCD command setting error
***********************************************************************/
/* Function Name : R_PMIP_Control */
e_pmip_err_t R_PMIP_Control(e_trans_cmd_t cmd, void * set)
{
    uint32_t set_temp;
    e_pmip_err_t result;
    result = PMIP_OK;
#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif

    if(0 != (MLCD_FLAG_RECONFIG & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR_RECONFIG;
    }

    /*set_temp value is 0 or 1*/
    set_temp = (uint32_t) set;

    /* check the configuration of the set value in the range */
    if(1 < set_temp) /* range: 0 1*/
    {
        return PMIP_ERROR_CONTROL_CMD;
    }

    switch(cmd)
    {
    case CMD0:  /* bit arrangement for data transmission */          
        {
            gs_pmip_resources.reg_mlcd->MLCDCR_b.BITSW    = set_temp;
        }        
        break;
    case CMD1:  /* automatic update method of horizontal address */
        {
            gs_pmip_resources.reg_mlcd->MLCDCR_b.HADDRDEC = set_temp;
        }        
        break;
    case CMD2:  /* automatic update method of vertical address */
        {
            gs_pmip_resources.reg_mlcd->MLCDCR_b.VADDRDEC = set_temp;
        }        
        break;
        default:
        {
            return PMIP_ERROR_CONTROL_CMD;
        }
    }        

    return result;
} /* End of function R_PMIP_Control */

/********************************************************************//**
* @brief PMIP all zero send
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed  
***********************************************************************/
/* Function Name : R_PMIP_AllZero */
e_pmip_err_t R_PMIP_AllZero(void)
{
    e_pmip_err_t result;
    result = PMIP_OK;

#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif

    if(0 != (MLCD_FLAG_RECONFIG & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR_RECONFIG;
    }
    
    /* All Zero send*/
    result = e_mlcd_allset(0x0);
    if(PMIP_OK != result)
    {
        return result;
    }

    /* MLCD Send Complete */
    if (NULL != gs_pmip_resources.info->cb_event) 
    {
        gs_pmip_resources.info->cb_event(PMIP_EVENT_SEND_COMPLETE);    
    }
    return result;
} /* End of function R_PMIP_AllZero */

/********************************************************************//**
* @brief PMIP all one send
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed  
***********************************************************************/
/* Function Name : R_PMIP_AllOne */
e_pmip_err_t R_PMIP_AllOne(void)
{
    e_pmip_err_t result;
    result = PMIP_OK;

#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED &  gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif

    if(0 != (MLCD_FLAG_RECONFIG & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR_RECONFIG;
    }

    /* All One send*/
    result = e_mlcd_allset(0xff);
    if(PMIP_OK != result)
    {
        return result;
    }

    /* MLCD Send Complete */
    if (NULL != gs_pmip_resources.info->cb_event) 
    {
        gs_pmip_resources.info->cb_event(PMIP_EVENT_SEND_COMPLETE);    
    }
    return result;
} /* End of function R_PMIP_AllOne */

/********************************************************************//**
* @brief PMIP data transmission
* @param[in] st_h                  start pixel of horizontal 
* @param[in] st_v                  start pixel of vertical
* @param[in] trim_size_h           data size of horizontal
* @param[in] size_v                data size of vertical
* @param[in] data                  image data
* @param[in] img_size_h            a line of image data which store to the memory (size x bytes)
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed  
* @retval PMIP_ERROR_SEND_CFG      MLCD Send config failed
***********************************************************************/
/* Function Name : R_PMIP_SendTrim */
e_pmip_err_t R_PMIP_SendTrim(uint8_t st_h, uint8_t st_v, uint16_t img_size_h, uint8_t trim_size_h, uint16_t size_v, 
                             uint8_t const data[])
{
    e_pmip_err_t result;
    result = PMIP_OK;
    uint32_t data_buf_addr;    
    uint32_t mlcd_img_addr;
    uint8_t  st_h_byte;

#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif

    if(0 != (MLCD_FLAG_RECONFIG & gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR_RECONFIG;
    }
    
    result = e_send_check(st_h, st_v, trim_size_h, size_v, img_size_h);
    if(PMIP_OK != result)
    {
        return result;
    }

    /* set interrupt enable */
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TEIE     = 0x1;
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TEMIE    = 0x1;

    /* set transfer coordinates */
    st_h_byte = st_h >> 3;
    gs_pmip_resources.reg_mlcd->MLCDADDR_b.HADDR  = st_h_byte;
    gs_pmip_resources.reg_mlcd->MLCDADDR_b.VADDR  = st_v;

    /* set transfer image data size */
    gs_pmip_resources.reg_mlcd->MLCDWRCR_b.BYTE   = trim_size_h; /* width  byte */
    gs_pmip_resources.reg_mlcd->MLCDWRCR_b.LINE   = size_v; /* height bit  */

    data_buf_addr  = PMIP_MLCD_BASE + MLCD_DATA_BUF_OFFSET; 

    /* store start address of transmission data */
    mlcd_img_addr   = (uint32_t)&data[0];

    if(CPU_TR == gs_trans_mode)
    {
        result = e_mlcd_cpu_normal_trans(mlcd_img_addr, data_buf_addr, trim_size_h, size_v, img_size_h);
        if(PMIP_OK != result)
        {
            return result;
        }
    }  /* CPU mode end */ 
#if (PMIP_CFG_SUPPORT_DMAC == 1)
    else if (DMAC_TR == gs_trans_mode)        
    {    
        result = e_mlcd_dmac_normal_trans(mlcd_img_addr, data_buf_addr, trim_size_h, size_v, img_size_h);
        if(PMIP_OK != result)
        {
            return result;
        }
    }  /* DMAC mode end */
#endif
#if (PMIP_CFG_SUPPORT_DTC == 1)
    else if (DTC_TR == gs_trans_mode)  
    {
        return PMIP_ERROR_DTC_NOT_SUPPORT;
    }  /* DTC mode end */ 
#endif
    else
    {
            return (PMIP_ERROR_TRANS_MODE);
    }
    return result;
} /* End of function R_PMIP_SendTrim */

/********************************************************************//**
* @brief PMIP data transmission
* @param[in] st_h                  start pixel of horizontal 
* @param[in] st_v                  start pixel of vertical
* @param[in] size_h                data size of horizontal
* @param[in] size_v                data size of vertical
* @param[in] data                  image data
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed  
* @retval PMIP_ERROR_SEND_CFG      MLCD Send config failed
***********************************************************************/
/* Function Name : R_PMIP_Send */
e_pmip_err_t R_PMIP_Send(uint8_t st_h, uint8_t st_v, uint8_t size_h, uint16_t size_v, 
                         uint8_t const data[])
{
    e_pmip_err_t result;
    result = PMIP_OK;
    result = R_PMIP_SendTrim(st_h, st_v, size_h, size_h, size_v, data);
    return result;
} /* End of function R_PMIP_Send */

/********************************************************************//**
* @brief PMIP Reconfig function
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed  
* @retval PMIP_ERROR_RECONFIG      Already reconfigured failed
* @retval PMIP_BUSY_ERROR          MLCD communication busy error
***********************************************************************/
/* Function Name : R_PMIP_Reconfig */
e_pmip_err_t R_PMIP_Reconfig(void)
{
    uint16_t sckcr;    
    uint32_t iclk_hz;    
    uint32_t enbeg_set_val;
    uint32_t enbw_set_val; 
    uint32_t fmask_set_val;
    uint32_t bmask_set_val;
    e_pmip_err_t result = PMIP_OK;

#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != (MLCD_FLAG_OPENED &  gs_pmip_resources.info->flags))
    {
        return PMIP_ERROR;
    }        
#endif    

    if(0 != (MLCD_FLAG_RECONFIG & gs_pmip_resources.info->flags))  /* mlcd Reconfig flags check */
    {
        return PMIP_ERROR_RECONFIG;
    }

    gs_pmip_resources.info->flags |= MLCD_FLAG_RECONFIG;
    /* Error if MLCD transfer in progress */
    if((0 != gs_pmip_resources.reg_mlcd->MLCDCR_b.TE) || (MLCD_TEND != gs_pmip_resources.reg_mlcd->MLCDSR_b.TEND))
    {
        gs_pmip_resources.info->flags &= (~MLCD_FLAG_RECONFIG);
        return PMIP_ERROR_BUSY;
    }

    /* calculation output clk, set sckcr reg */
    R_SYS_SystemClockFreqGet(&iclk_hz);                 /* get the system clock frequency in Hz*/

    if(iclk_hz < PMIP_PRV_LIMIT_PCLKA)                  /* Check PCLKA lower limit when using HW mask of VCOM.(8kHz) */
    {
        return  PMIP_ERROR;
    }
    
    sckcr = (iclk_hz * PMIP_CFG_SCLKH) / 1000000;       /* calculation sckcr value */        
    if((255 < sckcr) || (0 == sckcr) )                  /* check scksr range 1~255 */
    {
        result =  PMIP_ERROR_INPUT_CLK_OFFRANGE;
    }        

    /* set ENB period high/low width */
    if(PMIP_OK == result)
    {
        if(0 == PMIP_CFG_ENB_TBL)
        {
            if((KYOCERA_ENB <= PMIP_CFG_SCLKH) || (4 > ((KYOCERA_ENB * iclk_hz) / 1000000) ))
            {
                result = PMIP_ERROR_ENBEG_CFG;
            }
            enbeg_set_val = ((KYOCERA_ENB - PMIP_CFG_SCLKH) * iclk_hz) / 1000000;
            enbw_set_val  = (KYOCERA_ENBH * iclk_hz) / 1000000;
        }
        else
        {
            if(OTHER_ENB <= PMIP_CFG_SCLKH)
            {
                result = PMIP_ERROR_ENBEG_CFG;
            }
            enbeg_set_val = ((OTHER_ENB - PMIP_CFG_SCLKH) * iclk_hz) / 1000000;
            enbw_set_val  = (OTHER_ENBH * iclk_hz) / 1000000;
        }
        
        if((2 > enbeg_set_val) || (enbeg_set_val > 255))    /* check enbeg value range */
        {
            result = PMIP_ERROR_ENBEG_CFG;
        }
        if((2 > enbw_set_val) || (enbw_set_val > 1023))
        {
            result = PMIP_ERROR_ENBEG_CFG;
        }
        if (PMIP_OK == result)
        {
            /* set FMASK period high/low width */
	        fmask_set_val = (((enbw_set_val + (enbeg_set_val * 2) + 2 ) * 2048 * 10) / iclk_hz); /* x10 Cul */
	        fmask_set_val += 9;                                 /* Round up */
	        fmask_set_val /= 10;
	        fmask_set_val += 2;                                 /* Lower Limit */
	        
	        if (PMIP_PRV_FMASK_VAL > fmask_set_val)
	        {
	            fmask_set_val = PMIP_PRV_FMASK_VAL;
	        }
	        bmask_set_val = PMIP_PRV_BMASK_VAL;
	        
	        if ((255 < fmask_set_val) || (10 > fmask_set_val))   /* Check fmask value range(10 to 255) */
	        {
	            return PMIP_ERROR_VCOM_MASK_CFG;
	        }
	    
	        if ((255 < bmask_set_val) || (4 > bmask_set_val))   /* Check bmask value range(4 to 255) */
	        {
	            return PMIP_ERROR_VCOM_MASK_CFG;
	        }
	    }
    }
    
    if(PMIP_OK == result)
    {        
        gs_pmip_resources.reg_mlcd->MLCDCR_b.SCKCR = sckcr; /* set the transfer clock */
        gs_pmip_resources.reg_mlcd->MLCDENBCR_b.ENBEG = enbeg_set_val; 
        gs_pmip_resources.reg_mlcd->MLCDENBCR_b.ENBW  = enbw_set_val; 
        /* VCOM Hardware Mask Range */
        gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.BMASK = bmask_set_val;
        gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.FMASK = fmask_set_val;
    }
    else
    {
        /* If an error occurs on the way, do nothing */
    }
        gs_pmip_resources.info->flags &= (~MLCD_FLAG_RECONFIG);
return result;
}/* End of function R_PMIP_Reconfig */

/********************************************************************//**
* @brief PMIP All set value 
* @param[ini] set_data             setting data for full screen        
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR               MLCD Open failed  
***********************************************************************/
/* Function Name : e_mlcd_allset */
static e_pmip_err_t e_mlcd_allset(uint8_t set_data)
{
    uint8_t  disp_width_byte;
    uint32_t data_buf_addr;
    e_pmip_err_t result;
    uint32_t timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;

    result = PMIP_OK;

#if (PMIP_CFG_PARAM_CK_EN) 
    if(MLCD_FLAG_OPENED != gs_pmip_resources.info->flags)
    {
        return PMIP_ERROR;
    }        
#endif

    /* set interrupt enable */
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TEIE     = 0x1;

    /* set transfer coordinates */
    gs_pmip_resources.reg_mlcd->MLCDADDR_b.HADDR  = 0x00;
    gs_pmip_resources.reg_mlcd->MLCDADDR_b.VADDR  = 0x00;

    /* set transfer image data size */
    disp_width_byte = (PMIP_CFG_DISP_WIDTH >> 3); /* DISP_WIDTH  bit->byte */
    gs_pmip_resources.reg_mlcd->MLCDWRCR_b.BYTE   = disp_width_byte; /* width  byte */
    gs_pmip_resources.reg_mlcd->MLCDWRCR_b.LINE   = PMIP_CFG_DISP_HEIGHT;       /* height bit  */

    /* set transfer image data */
    data_buf_addr = PMIP_MLCD_BASE + MLCD_DATA_BUF_OFFSET; 
    for(uint8_t i =0; i<disp_width_byte; i++)
    {
           /* all zero or all one */
           *((uint8_t *)(data_buf_addr + i))= set_data;
    }        

    /* transfer start */
    gs_pmip_resources.reg_mlcd->MLCDSEND_b.EN = 0x1;
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TE   = 0x1;
    
    /* wait transmission end interrupt occurred */ 
    while(MLCD_TEND != gs_pmip_resources.reg_mlcd->MLCDSR_b.TEND)
    {
#if (1 == SYSTEM_CFG_WDT_REFRESH_ENABLE)
        r_system_wdt_refresh();
#endif
        if (0 == timeout)
        {
            result = PMIP_ERROR_TIMEOUT;
        }
        timeout--;
    }

    gs_pmip_resources.reg_mlcd->MLCDSEND_b.EN = 0x0;
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TE   = 0x0;

    /* disable MLCD interrupt */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);       /* End of transmission */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI);      /* Data register empty */

    /* clear the interrupt status */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);    /* End of transmission */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI);   /* Data register empty */

    /* clear pending the interrupt status */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);  /* End of transmission */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI); /* Data register empty */

    return result;
} /* End of function e_mlcd_allset */

/********************************************************************//**
* @brief clear mlcd registers
***********************************************************************/
/* Function Name : v_mlcd_clear_reg */
static void v_mlcd_clear_reg(void)
{
    uint32_t timeout = SYSTEM_CFG_API_TIMEOUT_COUNT;

    /* Check HW mask status. */
    while(0 != gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.HWMSKF)
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
    
    /* disable HW VCOM Mask */
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.HWMSKEN = 0x0;

    /* clear mlcd registers */    
    gs_pmip_resources.reg_mlcd->MLCDCR      = 0x00010000;
    gs_pmip_resources.reg_mlcd->MLCDADDR    = 0x00000000;
    gs_pmip_resources.reg_mlcd->MLCDWRCR    = 0x00010001;
    gs_pmip_resources.reg_mlcd->MLCDSEND    = 0x00000000;
    gs_pmip_resources.reg_mlcd->MLCDBKCR    = 0x00010000;
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL = 0x040A0000;
    gs_pmip_resources.reg_mlcd->MLCDENBCR   = 0x0004000A;

} /* End of Function v_mlcd_clear_reg */

/********************************************************************//**
* @brief DMAC channel sel fore MLCD module
* @retval PMIP_OK                      successful operation                            
* @retval PMIP_ERROR_SYSTEM_SETTING    MLCD system config failed                            
***********************************************************************/
/* Function Name : e_mlcd_priority_set */
static e_pmip_err_t e_mlcd_priority_set(void)
{
    e_pmip_err_t result;
    result = PMIP_OK;

    /* MLCD TEMI interrupt priority setting */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI,gs_pmip_resources.pmip_cfg_interrupt_level); /* setting temi interrupt priority */
    if(R_NVIC_GetPriority(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI) != gs_pmip_resources.pmip_cfg_interrupt_level)
    {
        return PMIP_ERROR_SYSTEM_SETTING;
    }

    /* MLCD TEI interrupt priority setting */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI,gs_pmip_resources.pmip_cfg_interrupt_level);  /* setting tei input interrupt priority */
    if(R_NVIC_GetPriority(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI) != gs_pmip_resources.pmip_cfg_interrupt_level)
    {
        return PMIP_ERROR_SYSTEM_SETTING;
    }
    return result;
} /* End of function e_mlcd_priority_set */

/********************************************************************//**
* @brief PMIP VCOM setting
***********************************************************************/
/* Function Name : v_vcom_set */
static void v_vcom_set(void)
{
    uint8_t  pmip_cfg_vcom_clk;    

    /* disable VCOM output, and set VCOM High width */
    switch(PMIP_CFG_VCOM_CLK)
    {
        case 0:
        {        
            pmip_cfg_vcom_clk = 1;
        }
        break;
        case 1:
        {
            pmip_cfg_vcom_clk = 0;
        }
        break;
        case 2: 
        {  
            pmip_cfg_vcom_clk = 2;
        }
        break;
        case 3: 
        {  
            pmip_cfg_vcom_clk = 3;
        }
        break;
        default: 
        {  
            pmip_cfg_vcom_clk = 1;
        }
    }
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.VCOME = 0x0;
    gs_pmip_resources.reg_mlcd->MLCDVCOMCTL_b.VCOMW = pmip_cfg_vcom_clk; 
} /* End of function v_vcom_set */

#if (PMIP_CFG_SUPPORT_DMAC == 1)
/********************************************************************//**
* @brief DMAC channel sel fore MLCD module
* @param[in] ch_sel              Dmac channel select 
* @retval PMIP_OK                successful operation                            
* @retval PMIP_ERROR_DMAC_CFG    DMCL config failed                            
***********************************************************************/
/* Function Name : e_mlcd_dmac_ch_sel */
static e_pmip_err_t e_mlcd_dmac_ch_sel(uint8_t ch_sel)
{
    e_pmip_err_t result;
    result = PMIP_OK;

    switch(ch_sel)
    {
    case 0:  /* dmac channel 0 */          
        {
            gsp_mlcd_dmacdev = &Driver_DMAC0;
        }        
        break;
    case 1:  /* dmac channel 1 */          
        {
            gsp_mlcd_dmacdev = &Driver_DMAC1;
        }        
        break;
    case 2:  /* dmac channel 2 */          
        {
            gsp_mlcd_dmacdev = &Driver_DMAC2;
        }        
        break;
    case 3:  /* dmac channel 3 */          
        {
            gsp_mlcd_dmacdev = &Driver_DMAC3;
        }        
        break;
        default:
        {
            return PMIP_ERROR_DMAC_CFG;
        }
    }

    /* DMAC resource is occupied check */
    if (gsp_mlcd_dmacdev->Open() != DMA_OK)
    {
        return PMIP_ERROR_DMAC_CFG;  
    }

    if (gsp_mlcd_dmacdev->Close() != DMA_OK)
    {
        return PMIP_ERROR_DMAC_CFG;  
    }

    return result;
} /* End of function e_mlcd_dmac_ch_sel */

/********************************************************************//**
* @brief PMIP normal dmac transmission
* @param[in] in_addr                input address
* @param[in] out_addr               output address
* @param[in] size_h                 data size of horizontal
* @param[in] size_v                 data size of vertical
* @param[in] line_memsize           memory size 
* @retval PMIP_OK                   successful operation                            
* @retval PMIP_ERROR_DMAC_TRANS     MLCD DMAC transfer failed
* @retval PMIP_ERROR_SYSTEM_SETTING MLCD system config failed
***********************************************************************/
/* Function Name : e_mlcd_dmac_normal_trans */
static e_pmip_err_t e_mlcd_dmac_normal_trans(uint32_t in_addr, uint32_t out_addr, uint8_t size_h, uint16_t size_v, uint32_t line_memsize)
{
    e_pmip_err_t result;
    result = PMIP_OK;

    /* DMAC setting */    
    result = e_mlcd_normal_dmac_pre_set(DMA_MODE_BLOCK, DMA_SIZE_BYTE, DMA_SRC_ADDR_OFFSET, DMA_DEST_INCR, DMA_REPEAT_BLOCK_SRC,
                                       size_h, 1, 1, in_addr, out_addr);
    if(PMIP_OK != result)
    {
        return result;
    }        

    /* MLCD TEI interrupt eventlink setting */
    if((-1) == (R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI, MLCD_TEI_IELS_VALUE, v_isr_dmac_tei)))           /* event link set */
    {
        return PMIP_ERROR_SYSTEM_SETTING;
    }

    /* MLCD TEMI interrupt eventlink setting */
    if((-1) == (R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI, MLCD_TEMI_IELS_VALUE, v_isr_dmac_normal_temi))) /* event link set */
    {
        return PMIP_ERROR_SYSTEM_SETTING;
    }

    /* MLCD Interrupt enable */    
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);  /* enable interrupt */
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI); /* enable interrupt */

    gs_size_h       = size_h;
    gs_size_v       = size_v;  
    gs_in_addr      = in_addr;
    gs_out_addr     = out_addr;
    gs_line_memsize = line_memsize;

    gs_temi_cnt = 0; /* TEMI counter clear*/   
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TE  = 0x1; /* transfer start */

    return result;
} /* End of function e_mlcd_dmac_normal_trans */
#endif
/********************************************************************//**
* @brief PMIP normal cpu transmission
* @param[in] in_addr                input address
* @param[in] out_addr               output address
* @param[in] size_h                 data size of horizontal
* @param[in] size_v                 data size of vertical
* @param[in] line_memsize           memory size 
* @retval PMIP_OK                   successful operation                            
* @retval PMIP_ERROR_SYSTEM_SETTING MLCD system config failed
***********************************************************************/
/* Function Name : e_mlcd_cpu_normal_trans */
static e_pmip_err_t e_mlcd_cpu_normal_trans(uint32_t in_addr, uint32_t out_addr, uint8_t size_h, uint16_t size_v, uint32_t line_memsize)
{
    e_pmip_err_t result;
    result = PMIP_OK;

    /* MLCD TEI interrupt eventlink setting */
    if((-1) == (R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI, MLCD_TEI_IELS_VALUE, v_isr_cpu_tei)))           /* event link set */
    {
        return PMIP_ERROR_SYSTEM_SETTING;
    }                

    /* MLCD TEMI interrupt eventlink setting */
    if((-1) == (R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI, MLCD_TEMI_IELS_VALUE, v_isr_cpu_normal_temi))) /* event link set */
    {
        return PMIP_ERROR_SYSTEM_SETTING;
    }

    /* MLCD Interrupt enable */    
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);  /* enable interrupt */
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI); /* enable interrupt */

    gs_size_h       = size_h;
    gs_size_v       = size_v;  
    gs_in_addr      = in_addr;
    gs_out_addr     = out_addr;
    gs_line_memsize = line_memsize;

    gs_temi_cnt = 0;  /* count clear of TEMI */       
    /* transfer start */
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TE  = 0x1; 
    return result;
} /* End of function e_mlcd_cpu_normal_trans */

#if (PMIP_CFG_SUPPORT_DMAC == 1)
/********************************************************************//**
* @brief configuration information for DMAC.
* @param[in] dmtmd_md       dmac transfer mode register configuration
* @param[in] dmtmd_sz       dmac transfer mode register configuration
* @param[in] dmamd_sm       dmac transfer mode register configuration
* @param[in] dmamd_dm       dmac transfer mode register configuration
* @param[in] dmtmd_dts      dmac transfer mode register configuration
* @param[in] data_length    dmac transfer register configuration
* @param[in] transfer_count dmac transfer register configuration
* @param[in] offset         dmac transfer register configuration
* @param[in] inaddr         dmac transfer register configuration
* @param[in] outaddr        dmac transfer register configuration
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR_DMAC_TRANS    MLCD DMAC transfer failed
***********************************************************************/
/* Function Name : e_mlcd_normal_dmac_pre_set */
static e_pmip_err_t e_mlcd_normal_dmac_pre_set(uint32_t dmtmd_md, uint32_t dmtmd_sz, uint32_t dmamd_sm, 
                                               uint32_t dmamd_dm, uint32_t dmtmd_dts, 
                                               uint32_t data_length, uint32_t transfer_count, 
                                               uint32_t offset,
                                               uint32_t inaddr, uint32_t outaddr)
{ 
    e_pmip_err_t result;
    result = PMIP_OK;
    st_dma_transfer_data_cfg_t config;

    /* set DMAC register */
    config.mode = dmtmd_md | dmtmd_sz | dmamd_sm | dmamd_dm | dmtmd_dts;

    config.src_addr       = inaddr;
    config.dest_addr      = outaddr;  
    config.transfer_count = transfer_count;
    config.offset         = offset;
    config.block_size     = data_length;
    config.src_extended_repeat  = 0;
    config.dest_extended_repeat = 0;

    /* open DMAC function */
    if (gsp_mlcd_dmacdev->Open() != DMA_OK)
    {
        return  PMIP_ERROR_DMAC_TRANS;
    }

    /* software trigger DMAC */
    if (gsp_mlcd_dmacdev->Create(DMAC_TRANSFER_REQUEST_SOFTWARE, &config) != DMA_OK) 
    {
        return  PMIP_ERROR_DMAC_TRANS;
    }
    return result;
} /* End of Function e_mlcd_normal_dmac_pre_set */

/********************************************************************//**
* @brief configuration information for DMAC.
* @param[in] data_length    dmac transfer register configuration
* @param[in] transfer_count dmac transfer register configuration
* @param[in] offset         dmac transfer register configuration
* @param[in] inaddr         dmac transfer register configuration
* @param[in] outaddr        dmac transfer register configuration
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR_DMAC_TRANS    MLCD DMAC transfer failed
***********************************************************************/
/* Function Name : e_mlcd_dmac_data_trans */
static e_pmip_err_t e_mlcd_dmac_data_trans(uint32_t data_length, uint32_t transfer_count, 
                                           uint32_t offset,
                                           uint32_t inaddr, uint32_t outaddr)
{
    e_pmip_err_t result;
    result = PMIP_OK;
    st_dma_refresh_data_t config;
 
    /* update address and DMAC enable */ 
    config.offset         = offset;
    config.src_addr       = inaddr;
    config.dest_addr      = outaddr;  /* input address of MLCD data reg */
    config.block_size     = data_length;
    config.transfer_count = transfer_count;
    config.auto_start     = true;
    config.keep_dma_req   = true;

    /* DMAC trans enable */
    if(gsp_mlcd_dmacdev->Control(DMA_CMD_SOFTWARE_TRIGGER,NULL) != DMA_OK)
    {
        return  PMIP_ERROR_DMAC_TRANS;
    }

    if (gsp_mlcd_dmacdev->Control(DMA_CMD_REFRESH_EXTRA,&config) != DMA_OK)
    {
        return  PMIP_ERROR_DMAC_TRANS;
    }

    return result;
} /* End of Function e_mlcd_dmac_data_trans */
#endif

/********************************************************************//**
* @brief Function of inputing data from source addr to MLCD data register normal trans
* @param[in] src_addr       source address of input data
* @param[in] dest_addr      destination address of input data
* @param[in] total_data_num number of transmitted data
***********************************************************************/
/* Function Name : v_cpu_trans_normal */
static void v_cpu_trans_normal(uint32_t src_addr,uint32_t dest_addr, uint32_t total_data_num)
{
    uint32_t i;
    
    for(i=0;i<total_data_num;i++) /* total data */
    {
        /* CPU trans valid data */      
        *((uint8_t *)dest_addr) = *((uint8_t *)src_addr);
        dest_addr ++;
        src_addr ++;
    }
} /* End of Function v_cpu_trans_normal */

/********************************************************************//**
* @brief PMIP Send config check
* @param[in] st_h                  start pixel of horizontal 
* @param[in] st_v                  start pixel of vertical
* @param[in] size_h                data size of horizontal
* @param[in] size_v                data size of vertical
* @param[in] line_memsize          memory size 
* @retval PMIP_OK                  successful operation                            
* @retval PMIP_ERROR_SEND_CFG      MLCD Send config failed
***********************************************************************/
/* Function Name : e_send_check */
static e_pmip_err_t e_send_check(uint8_t st_h, uint8_t st_v, uint8_t size_h, uint16_t size_v, 
                                 uint32_t line_memsize)
{
    e_pmip_err_t result;
    result = PMIP_OK;

    /* check st_h config */
    if(st_h > (PMIP_CFG_DISP_WIDTH - 8))
    {
        return PMIP_ERROR_SEND_CFG;
    }        
    if( (0 != (st_h & 0x07)) )
    {
        return PMIP_ERROR_SEND_CFG;
    }        

    /* check st_v config */
    if(st_v > (PMIP_CFG_DISP_HEIGHT - 1))
    {
        return PMIP_ERROR_SEND_CFG;
    }        

    /* check size_h config */
    if((1 > size_h) || (size_h > (PMIP_CFG_DISP_WIDTH >> 3))) 
    {
        return PMIP_ERROR_SEND_CFG;
    }        

    /* check send config offbound */
    if(((st_h >> 3) + size_h) > (PMIP_CFG_DISP_WIDTH >> 3))
    {
        return PMIP_ERROR_SEND_CFG;
    }

    /* check size_v config*/
    if((1 > size_v) || (size_v > PMIP_CFG_DISP_HEIGHT))
    {
        return PMIP_ERROR_SEND_CFG;
    }        

    /* check send config offbound */
    if((st_v + size_v) > PMIP_CFG_DISP_HEIGHT)
    {
        return PMIP_ERROR_SEND_CFG;
    }

    /* check line_memsize config*/
    if(size_h > line_memsize)  
    {
        return PMIP_ERROR_SEND_CFG;
    }        
    return result;
} /* End of function e_send_check */

#if (PMIP_CFG_SUPPORT_DMAC == 1)
/********************************************************************//**
* @brief MLCD TEMI Interrupt handler for dmac trans end
***********************************************************************/
/* Function Name : v_isr_dmac_normal_temi */
static void v_isr_dmac_normal_temi(void)
{
    e_pmip_err_t result;
    result = PMIP_OK;  

    /* MLCD_TEMI interrupts are occured (lines of image + 1) times. */
    /* If count value of MLCD_TEMI is bigger than lines of image, data of image doesn't send. */
    if (gs_temi_cnt < gs_size_v)
    {
        gs_temi_cnt++;  /* count-up of TEMI counter*/   
    
        /* dmac refresh data */
        result = e_mlcd_dmac_data_trans(gs_size_h, 1, 1, gs_in_addr, gs_out_addr);
        if(PMIP_OK != result)
        {
            /* MLCD Send Failed */
            if (NULL != gs_pmip_resources.info->cb_event) 
            {
                gs_pmip_resources.info->cb_event(PMIP_EVENT_ERROR_DMAC_TRANS);    
            }
        }        
        
        /* calculation image address */
        gs_in_addr = gs_in_addr + gs_line_memsize;
    }

} /* End of Function v_isr_dmac_normal_temi */        

/********************************************************************//**
* @brief MLCD TEI Interrupt handler for dmac normal trans
***********************************************************************/
/* Function Name : v_isr_dmac_tei */
static void v_isr_dmac_tei(void)
{
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TE     = 0x0;
    gs_pmip_resources.reg_mlcd->MLCDBKCR_b.BKEN = 0x0; /* block mode disable */

    /* Close DMAC function */
    if (gsp_mlcd_dmacdev->Close() != DMA_OK)
    {
        /* MLCD Send Failed */
        if (NULL != gs_pmip_resources.info->cb_event) 
        {
            gs_pmip_resources.info->cb_event(PMIP_EVENT_ERROR_DMAC_TRANS);    
        }
    }

    /* disable MLCD interrupt */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);       /* End of transmission */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI);      /* Data register empty */

    /* clear the interrupt status */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);    /* End of transmission */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI);   /* Data register empty */

    /* clear pending the interrupt status */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);  /* End of transmission */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI); /* Data register empty */

    /* MLCD Send Complete */
    if (NULL != gs_pmip_resources.info->cb_event) 
    {
        gs_pmip_resources.info->cb_event(PMIP_EVENT_SEND_COMPLETE);
    }
} /* End of Function v_isr_dmac_tei */        
#endif
/********************************************************************//**
* @brief MLCD TEMI Interrupt handler for cpu normal trans
***********************************************************************/
/* Function Name : v_isr_cpu_normal_temi */
static void v_isr_cpu_normal_temi(void)
{
    /* MLCD_TEMI interrupts are occured (lines of image + 1) times. */
    /* If count value of MLCD_TEMI is bigger than lines of image, data of image doesn't send. */
    if (gs_temi_cnt < gs_size_v)
    {
        gs_temi_cnt++;  /* count-up of TEMI counter*/ 
      
        /* cpu trans data */
        v_cpu_trans_normal(gs_in_addr,gs_out_addr, gs_size_h);

        /* calculation image address */
        gs_in_addr = gs_in_addr + gs_line_memsize;
    }
    
} /* End of Function v_isr_cpu_normal_temi */        

/********************************************************************//**
* @brief MLCD TEMI Interrupt handler for cpu trans end
***********************************************************************/
/* Function Name : v_isr_cpu_tei */
static void v_isr_cpu_tei(void)
{

    /* transfer end */
    gs_pmip_resources.reg_mlcd->MLCDCR_b.TE     = 0x0;
    gs_pmip_resources.reg_mlcd->MLCDBKCR_b.BKEN = 0x0; /* block mode disable */

    /* disable MLCD interrupt */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);       /* End of transmission */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI);      /* Data register empty */

    /* clear the interrupt status */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);    /* End of transmission */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI);   /* Data register empty */

    /* clear pending the interrupt status */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEI);  /* End of transmission */
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_MLCD_TEMI); /* Data register empty */

    /* MLCD Send Complete */
    if (NULL != gs_pmip_resources.info->cb_event) 
    {
        gs_pmip_resources.info->cb_event(PMIP_EVENT_SEND_COMPLETE);
    }
} /* End of Function v_isr_cpu_tei */        

/******************************************************************************************************************//**
 * @}(end addtogroup Device HAL PMIP)
 *********************************************************************************************************************/

/* End of file (r_pmip_api.c) */
