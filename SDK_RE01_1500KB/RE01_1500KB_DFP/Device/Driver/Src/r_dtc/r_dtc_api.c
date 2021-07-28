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
* File Name    : r_dtc_api.c
* Version      : 1.40
* Description  : HAL Driver for DTC.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description      
*         : 01.11.2018 0.51    First Release     
*         : 22.11.2018 0.60    Fixed for driver linkage.
*                              - Changed by R_DTC_Open function so that DMA_OK will return even if it is already 
*                                executed in the Open state.
*                              - Fixed R_DTC_InterruptEnable function. If the argument "int_fact" is 0, interrupts 
*                                are not permitted and event link and priority are set.
*                               Compatible with GCC compiler
*                               - Include in intrinsics.h Remove
*                               Adapted to GSCE coding rules.
*                               - Modified to include st_dtc_mode_info_t,st_dtc_resource_t definition 
*                                 in r_dtc_api.c
*                               Changed file name to r_dtc_api.c
*                              When open flag is True, Fix to return DMA_OK with Open function.
*         : 06.02.2019 0.61    - DMAC / DTC active information (g_dma_active_flag) is defined
*                                in r_dma_common_api.h.
*                              - Added processing to check the DMA active state before
*                                executing the R_LPM_ModuleStop function.
*                              "DMA_CMD_REFRESH_DATA" is added to Control command
*         : 18.03.2019 0.70     Upgrade version to 0.70
*                               The processing to clear the DTCST bit was deleted when the
*                               DMA_CMD_REFRESH_DATA command was executed.
*         : 14.05.2019 0.72     Upgrade version to 0.72
*                               Add DMA_CMD_REFRESH_EXTRA command.
*                               Fixed RAM section layout definition.
*         : 02.07.2019 1.00     Add R_DTC_GetTransferByte function
*                               Upgrade version to 1.00
*         : 04.02.2020 1.10     Fixed the problem that the R_DTC_GetTransferByte function could not allocate RAM
*         : 27.08.2020 1.20     Fixed the problem that the dtc_cmd_refresh function could not allocate RAM
*         : 05.01.2021 1.40     Add "R_DTC_GetAvailabilitySrc" and "R_DTC_Release"
**********************************************************************************************************************/

/******************************************************************************************************************//**
 * @addtogroup grp_device_hal_dtc
 * @{
 *********************************************************************************************************************/
/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
/* Public interface header file for this package. */
#include <stdint.h>
#include <stdbool.h>
#include <r_dtc_api.h>

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define DTC_PRV_CHAIN_ENABLE                (0x00800000)    /*!< DTC chain transfer enable bit */
#define DTC_PRV_MIN_COUNT_VAL               (1)             /*!< DTC transfer min count */
#define DTC_PRV_MAX_16BITS_COUNT_VAL        (65536)         /*!< DTC 16bit transfer max count */
#define DTC_PRV_MAX_8BITS_COUNT_VAL         (256)           /*!< DTC 8bit transfer max count */
#define DTC_PRV_IELSR_DTCE                  (0x01000000)    /*!< IELSR DTCE bit mask */
#define DTC_PRV_COMPLETE_INT_IESR_VAL       (0x00000003)    /*!< DTC complete interrupt set value */
#define DTC_PRV_MASK_MRA_MD                 (0xC0000000)    /*!< DTC Mode check mask */
#define DTC_PRV_MRA_MD_NORMAL               (0x00000000)    /*!< DTC Mode normal value */
#define DTC_PRV_MRA_MD_REPEAT               (0x40000000)    /*!< DTC Mode repeat value */
#define DTC_PRV_MRA_MD_BLOCK                (0x80000000)    /*!< DTC Mode block value */
#define DTC_PRV_MASK_MRA_SZ                 (0x30000000)    /*!< DTC Size check mask */
#define DTC_PRV_MRA_SZ_BYTE                 (0x00000000)    /*!< DTC byte size value */
#define DTC_PRV_MRA_SZ_WORD                 (0x10000000)    /*!< DTC word size value */
#define DTC_PRV_MRA_SZ_LONG                 (0x20000000)    /*!< DTC long size value */

#define DTC_PRV_VECTOR_ALIGN   __attribute__ ((aligned (1024)))   /*!< DTC Vector align */ /* @suppress("Macro expansion") */

#if (DTC_CFG_R_DTC_OPEN              == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CLOSE             == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CREATE            == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CONTROL           == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_INTERRUPT_ENABLE  == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_INTERRUPT_DISABLE == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_GET_TRANSFER_BYTE == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_DTC_COMP_INTERRUPT      == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DTC_RESOURCES       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DTC_RESOURCES
#endif

#if (DTC_CFG_R_DTC_OPEN                 == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CLOSE                == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CREATE               == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CONTROL              == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_INTERRUPT_ENABLE     == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_INTERRUPT_DISABLE    == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_GET_TRANSFER_BYTE    == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_GET_VERSION          == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_GET_STATE            == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CLEAR_STATE          == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_RELEASE              == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_GET_AVAILABILITY_SRC == SYSTEM_SECTION_RAM_FUNC)
#define DATA_LOCATION_PRV_DRIVER_DTC       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_PRV_DRIVER_DTC
#endif

#if (DTC_CFG_R_DTC_CREATE       == SYSTEM_SECTION_RAM_FUNC) || \
    (DTC_CFG_R_DTC_CONTROL      == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_DTC_CREATE_PARAM       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_DTC_CREATE_PARAM
#endif

#if (DTC_CFG_DTC_COMP_INTERRUPT == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_DTC_COMP_INTERRUPT     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_DTC_COMP_INTERRUPT
#endif

#if (DTC_CFG_R_DTC_GET_VERSION == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_GET_VERSION       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_GET_VERSION
#endif

#if (DTC_CFG_R_DTC_OPEN == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_OPEN       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_OPEN
#endif

#if (DTC_CFG_R_DTC_CLOSE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_CLOSE      __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_CLOSE
#endif

#if (DTC_CFG_R_DTC_CREATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_CREATE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_CREATE
#endif

#if (DTC_CFG_R_DTC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_CONTROL       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_CONTROL
#endif

#if (DTC_CFG_R_DTC_INTERRUPT_ENABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_INTERRUPT_ENABLE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_INTERRUPT_ENABLE
#endif

#if (DTC_CFG_R_DTC_INTERRUPT_DISABLE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_INTERRUPT_DISABLE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_INTERRUPT_DISABLE
#endif

#if (DTC_CFG_R_DTC_CONTROL == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_DTC_CMD_REFRESH     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_DTC_CMD_REFRESH
#endif

#if (DTC_CFG_R_DTC_GET_STATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_GET_STATE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_GET_STATE
#endif

#if (DTC_CFG_R_DTC_CLEAR_STATE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_CLEAR_STATE     __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_CLEAR_STATE
#endif

#if (DTC_CFG_R_DTC_GET_TRANSFER_BYTE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_GET_TRANSFER_BYTE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_GET_TRANSFER_BYTE
#endif

#if (DTC_CFG_R_DTC_GET_AVAILABILITY_SRC == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_GET_AVAILABILITY_SRC       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_GET_AVAILABILITY_SRC
#endif

#if (DTC_CFG_R_DTC_RELEASE == SYSTEM_SECTION_RAM_FUNC)
#define FUNC_LOCATION_PRV_R_DTC_RELEASE       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_PRV_R_DTC_RELEASE
#endif

/******************************************************************************
 Local Typedef definitions
 *****************************************************************************/
/** DTC mode infomation */
typedef struct 
{           
    dma_cb_event_t        cb_event;                 /*!< Event callback                   */
    uint8_t               read_skip;                /*!< Read skip setting value          */
    uint8_t               flags;                    /*!< DMA driver flags                 */
}st_dtc_mode_info_t;

/** DTC recource data */
typedef struct 
{            
    volatile DTC_Type      *reg;                    /*!< Base pointer to DTC registers    */
    e_system_mcu_lock_t     lock_id;                /*!< Module lock id                   */
    e_lpm_mstp_t            mstp_id;                /*!< MSTP id                          */
    st_dtc_mode_info_t      *info;                  /*!< dtc mode infomation              */
} const st_dtc_resources_t;

/******************************************************************************
 Exported global variables (to be accessed by other files)
 *****************************************************************************/

 
/******************************************************************************
 Private global variables and functions
 *****************************************************************************/
/* DTC vector table */
static st_dma_transfer_data_t* gsp_dtc_vector_table[SYSTEM_IRQ_EVENT_NUMBER_NOT_USED] DTC_PRV_VECTOR_ALIGN;

/* DTC transfer count save buffer */
static uint32_t gsp_dtc_vector_table_set_transfer_count[SYSTEM_IRQ_EVENT_NUMBER_NOT_USED] DTC_PRV_VECTOR_ALIGN;

/* DTC mode infomation */
static st_dtc_mode_info_t gs_dtc_info = {0};

/* save DTC available source */
static uint64_t gs_dtc_available_src = 0;    

/* DTC Resources */
static st_dtc_resources_t gs_dtc_resources DATA_LOCATION_PRV_DTC_RESOURCES =
{
    DTC,                                        /* DTC regster base address  @suppress("Cast comment") */
    SYSTEM_LOCK_DTC,                            /* DTC resource lock id */
    LPM_MSTP_DTC_DMAC,                          /* DTC mstp id */
    &gs_dtc_info                                /* DTC infomation */
};

static uint32_t  R_DTC_GetVersion(void) FUNC_LOCATION_PRV_R_GET_VERSION;
static e_dma_err_t R_DTC_Open(void) FUNC_LOCATION_PRV_R_DTC_OPEN;
static e_dma_err_t R_DTC_Close(void) FUNC_LOCATION_PRV_R_DTC_CLOSE;
static e_dma_err_t R_DTC_Create(int16_t const dtc_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
                                FUNC_LOCATION_PRV_R_DTC_CREATE;
static e_dma_err_t R_DTC_Control(e_dma_cmd_t cmd, void const * const p_arg) FUNC_LOCATION_PRV_R_DTC_CONTROL;
static e_dma_err_t R_DTC_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
                                FUNC_LOCATION_PRV_R_DTC_INTERRUPT_ENABLE;
static e_dma_err_t R_DTC_InterruptDisable(void) FUNC_LOCATION_PRV_R_DTC_INTERRUPT_DISABLE;
static e_dma_err_t R_DTC_GetState(st_dma_state_t * const state) FUNC_LOCATION_PRV_R_DTC_GET_STATE;
static e_dma_err_t R_DTC_ClearState(uint32_t clr_state) FUNC_LOCATION_PRV_R_DTC_CLEAR_STATE;
static e_dma_err_t R_DTC_GetTransferByte(int16_t const dt_source, uint32_t *transfer_byte) 
                                                        FUNC_LOCATION_PRV_R_GET_TRANSFER_BYTE;
static e_dma_err_t R_DTC_GetAvailabilitySrc(uint64_t * const src_info)FUNC_LOCATION_PRV_R_DTC_GET_AVAILABILITY_SRC;
static e_dma_err_t R_DTC_Release(int16_t const dtc_source)FUNC_LOCATION_PRV_R_DTC_RELEASE;
static e_dma_err_t dtc_create_param(st_dma_transfer_data_t *p_transfer_info, 
                                    st_dma_transfer_data_cfg_t const * p_data_cfg, uint8_t chain_set_en)
                                    FUNC_LOCATION_PRV_DTC_CREATE_PARAM;
static void dtc_comp_interrupt(void) FUNC_LOCATION_PRV_DTC_COMP_INTERRUPT;
static e_dma_err_t dtc_cmd_refresh(st_dma_refresh_data_t const * const p_data) FUNC_LOCATION_PRV_DTC_CMD_REFRESH;


/*****************************************************************************************************************//**
 * @brief       Get API verion
 * @retval      uint32_t  Version.
**********************************************************************************************************************/
/* Function Name : R_DTC_GetVersion */
static uint32_t  R_DTC_GetVersion(void) // @suppress("Source line ordering")
{
    uint32_t const version = (DTC_VERSION_MAJOR << 16) | DTC_VERSION_MINOR;

    return (version);
}/* End of function R_DTC_GetVersion() */


/*****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DTC Open API for the DTC peripheral on RE Family.
 * @retval      DMA_OK  Open successful.
 * @retval      DMA_ERROR  Open error.
**********************************************************************************************************************/
/* Function Name : R_DTC_Open */
static e_dma_err_t R_DTC_Open(void)
{
    
    if (0U != (DMA_FLAG_OPENED & gs_dtc_resources.info->flags))
    {
        return (DMA_OK);
    }

    /* Lock DTC resource */
    if (0 != R_SYS_ResourceLock(gs_dtc_resources.lock_id))
    {
        return (DMA_ERROR_LOCKED);
    }
    
    /* Release module stop */
    if (0 != R_LPM_ModuleStart(gs_dtc_resources.mstp_id))
    {
        /* Release DTC resource */
        R_SYS_ResourceUnlock(gs_dtc_resources.lock_id);
        return (DMA_ERROR);
    }

    /** Resource initialize */
    gs_dtc_resources.info->flags = DMA_FLAG_OPENED;            /* Driver Opened                  */
    gs_dtc_resources.info->read_skip = 1;                      /* Read skip default valuse set   */
    gs_dtc_resources.info->cb_event = NULL;                    /* Callback function is NULL      */
    gs_dtc_available_src = 0;                                  /* Initialization of source information */

    /* DTCVBR - DTC Vector Base Register
       b31 : b0 - DTC Vector Base Address  */
    DTC->DTCVBR = (uint32_t)&gsp_dtc_vector_table[0];
    
    /* DTC vector clear */
    for (uint16_t i = 0; i < (uint16_t)SYSTEM_IRQ_EVENT_NUMBER_NOT_USED; i++)
    {
        gsp_dtc_vector_table[i] = NULL; // @suppress("Cast comment")
        gsp_dtc_vector_table_set_transfer_count[i] = 0;
    }
    g_dma_active_flag |= DMA_ACTIVE_DTC;
    
    return (DMA_OK);
}/* End of function R_DTC_Open() */

/****************************************************************************************************************//**
 * @brief       The implementation of HAL-Driver DTC Close API for the DTC peripheral on RE Family.
 * @retval      ARM_DRIVER_OK    Uninitialization was successful.
**********************************************************************************************************************/
/* Function Name : R_DTC_Close */
static e_dma_err_t R_DTC_Close(void)
{
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0); // @suppress("Cast comment")
    IRQn_Type irq;
    
    if (0 == (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_OK);
    }

    /* DTCST   - DTC Module Start Register
       b7 : b1 - Resaeved
       b0      - DTCST - DTC Module Start - DTC module stop */
    DTC->DTCST = 0x00;
    
    /* DTCCR   - DTC Control Register
       b4      - RRS - DTC Transfer Information Read Skip Enable - Transfer information read is not skipped */
    DTC->DTCCR_b.RRS = 0;
    
    /* Disable all DTC transfer factors */
    for (irq = SYSTEM_IRQ_EVENT_NUMBER0; irq < SYSTEM_IRQ_EVENT_NUMBER_NOT_USED; irq ++)
    {
        (*(p_ielsr + (uint32_t)irq)) &= (~DTC_PRV_IELSR_DTCE); // @suppress("Cast comment")
    }

    /* Disable DTC complete interrupt */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE);
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")
    
    /* Release dtc resource */
    R_SYS_ResourceUnlock(gs_dtc_resources.lock_id);
    g_dma_active_flag &= ~DMA_ACTIVE_DTC;
    
    if (0 == (g_dma_active_flag & DMA_ACTIVE_MASK))
    {
        /* Setting module stop */
        R_LPM_ModuleStop(gs_dtc_resources.mstp_id);
    }

    /* DTC infomation flag clear */
    gs_dtc_resources.info->flags = 0x00;
    
    /* Initialization of source information */
    gs_dtc_available_src = 0; 

    return (DMA_OK);

}/* End of function R_DTC_Close() */

/****************************************************************************************************************//**
 * @brief       Create DTC transfer infomation.
 * @param[in]   dtc_source      DTC transfer trigger source.
 * @param[in,out]   p_data_cfg      Pointer to DTC transfer setting infomation.
 * @retval      DMA_OK          DTC transfer infomation create successful.
 * @retval      DMA_ERROR       DTC transfer infomation create error.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DTC transfer trigger failed.
 * @note        If setting of DTC information fails, there is a possibility that the DTC transfer 
 *              information may be corrupted, so it transitions to the DTC transfer disabled state
**********************************************************************************************************************/
/* Function Name : R_DTC_Create */
static e_dma_err_t R_DTC_Create(int16_t const dtc_source, st_dma_transfer_data_cfg_t * const p_data_cfg)
{
    e_dma_err_t result = DMA_OK;
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0); // @suppress("Cast comment")

#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    /* DTC open check */
    if (DMA_FLAG_OPENED != (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
    
    /* DTC transfer trigger check */
    if ((SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= dtc_source) || (0 > dtc_source))
    {
        return (DMA_ERROR_PARAMETER);
    }
#endif
    /* Check DTC parameter check and create dtc transfer infomation. */
    result = dtc_create_param((st_dma_transfer_data_t *)(p_data_cfg->p_transfer_data), p_data_cfg, true);
    
    /* DTC infomation create ok? */
    if (DMA_OK == result)
    {
        /* Disable DTC transfer factor */
        (*(p_ielsr + (uint32_t)dtc_source)) &= (~DTC_PRV_IELSR_DTCE);
        
        /* DTCCR   - DTC Control Register
           b4      - RRS - DTC Transfer Information Read Skip Enable - Transfer information read is not skipped */
        DTC->DTCCR_b.RRS = 0;

        /* Write start address of Transfer data to Vector table. */
        gsp_dtc_vector_table[dtc_source] = (st_dma_transfer_data_t *)(p_data_cfg->p_transfer_data);
        
        if (DMA_MODE_REPEAT != ((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK))
        {
            gsp_dtc_vector_table_set_transfer_count[dtc_source] = p_data_cfg->transfer_count & 0x0000FFFF;
            if (0 == gsp_dtc_vector_table_set_transfer_count[dtc_source])
            {
                gsp_dtc_vector_table_set_transfer_count[dtc_source] = 65536;
            }
        }
        else
        {
            gsp_dtc_vector_table_set_transfer_count[dtc_source] = p_data_cfg->transfer_count & 0x000000FF;
            if (0 == gsp_dtc_vector_table_set_transfer_count[dtc_source])
            {
                gsp_dtc_vector_table_set_transfer_count[dtc_source] = 256;
            }
        }
        
        /* Restore RRS bit. */
        DTC->DTCCR_b.RRS = gs_dtc_resources.info->read_skip;

        /* Set the DTCE bit. */
        (*(p_ielsr + (uint32_t)dtc_source)) |= DTC_PRV_IELSR_DTCE;
        
        /* 1 or more create dtc table */
        gs_dtc_resources.info->flags |= DMA_FLAG_CREATED;

        R_SYS_EnterCriticalSection();
        /* Set available information */
        gs_dtc_available_src |= (0x1UL << dtc_source);
        R_SYS_ExitCriticalSection();

    }
    else
    {
        /* Because there is a possibility that the DTC information is broken, let the DTC stop */

        /* Disable all DTC transfer factor */
        (*(p_ielsr + (uint32_t)dtc_source)) &= (~DTC_PRV_IELSR_DTCE);
        gs_dtc_resources.info->flags = DMA_FLAG_OPENED;
    }
    return (result);
    
}/* End of function R_DTC_Create() */

/****************************************************************************************************************//**
 * @brief       Check DTC parameter check and create dtc transfer infomation.
 * @param[in,out]   p_transfer_data  Pointer to area storing DTC transfer information.
 * @param[in]   p_data_cfg      Pointer to DTC transfer setting infomation.
 * @param[in]   chain_cnt       Chain transfer data(Set 0 if chain transfer is not used).
 * @retval      DMA_OK          DTC transfer infomation create successful.
 * @retval      DMA_ERROR       DTC transfer infomation create error.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
**********************************************************************************************************************/
/* Function Name : dtc_create_param */
static e_dma_err_t dtc_create_param(st_dma_transfer_data_t *p_transfer_info,  // @suppress("Function length")
                                    st_dma_transfer_data_cfg_t const * p_data_cfg, uint8_t chain_set_en)
{
    bool chain_end = false;                     /* Transfer infomation set end flag */
    
    /* Loop until setting of chain transfer complete */
    while (false == chain_end)
    {
    
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
        /* If there is a value in the reserved bit area, an error is returned */
        if (0 != ((p_data_cfg->mode) & DMA_MODE_RASERVED))
        {
            return (DMA_ERROR_PARAMETER);
        }
        
        /* MODE check */
        switch ((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK)
        {
            case DMA_MODE_NORMAL:
            {
                /* In the case of normal transfer, the setting range of transfer count is 1 to 65536. */
                if ((p_data_cfg->transfer_count > DTC_PRV_MAX_16BITS_COUNT_VAL) ||
                    (p_data_cfg->transfer_count < DTC_PRV_MIN_COUNT_VAL))
                {
                    return (DMA_ERROR_PARAMETER);
                }
            }
            break;
            
            case DMA_MODE_REPEAT:
            {
                /* In the case of repeat transfer, the setting range of transfer count is 1 to 256. */
                if ((p_data_cfg->transfer_count > DTC_PRV_MAX_8BITS_COUNT_VAL) ||
                    (p_data_cfg->transfer_count < DTC_PRV_MIN_COUNT_VAL))
                {
                    return (DMA_ERROR_PARAMETER);
                }
            }
            break;
            case DMA_MODE_BLOCK:
            {
                /* In the case of block transfer, the setting range of block size is 1 to 256. */
                if ((p_data_cfg->block_size > DTC_PRV_MAX_8BITS_COUNT_VAL) ||
                    (p_data_cfg->block_size < DTC_PRV_MIN_COUNT_VAL))
                {
                    return (DMA_ERROR_PARAMETER);
                }

                /* In the case of block transfer, the setting range of transfer count is 1 to 65536. */
                if ((p_data_cfg->transfer_count < DTC_PRV_MIN_COUNT_VAL) ||
                    (p_data_cfg->transfer_count > DTC_PRV_MAX_16BITS_COUNT_VAL))
                {
                    return (DMA_ERROR_PARAMETER);
                }
            }
            break;
            
            default:
            {
                /* Invalid mode */
                return (DMA_ERROR_MODE);
            }
            break;
        }
        
        /* Check data size setting. */
        switch ((p_data_cfg->mode) & DMA_SIZE_MASK)
        {
            case DMA_SIZE_BYTE:
            { /* @suppress("Empty compound statement")  @suppress("Block comment") */
                /* No action to be performed */
            }
            break;

            case DMA_SIZE_WORD:
            {
                /* For word size, bit 0 of the transfer source / transfer destination address must be 0 */
                if ((0 != (p_data_cfg->src_addr & 0x00000001)) ||
                    (0 != (p_data_cfg->dest_addr & 0x00000001)))
                {
                    return (DMA_ERROR_PARAMETER);
                }
            }
            break;

            case DMA_SIZE_LONG:
            {
                /* For long size, bit 1 to 0 of the transfer source / transfer destination address must be 00 */
                if ((0 != (p_data_cfg->src_addr & 0x00000003)) ||
                    (0 != (p_data_cfg->dest_addr & 0x00000003)))
                {
                    return (DMA_ERROR_PARAMETER);
                }
            }
            break;
            
            default:
            {
                /* Invalid data size */
                return (DMA_ERROR_MODE);
            }
            break;
        }
        
        /* Check source addressing */
        switch ((p_data_cfg->mode) & DMA_SRC_ADDRESSING_MASK)
        {
            case DMA_SRC_FIXED:
            case DMA_SRC_INCR:
            case DMA_SRC_DECR:
            { /* @suppress("Empty compound statement") @suppress("Block comment") */
                /* No action to be performed */
            }
            break;
            
            default:
            {
                /* Invalid source addressing */
                return (DMA_ERROR_MODE);
            }
            break;
        }
    
        /* Check destination addressing */
        switch ((p_data_cfg->mode) & DMA_DEST_ADDRESSING_MASK)
        {
            case DMA_DEST_FIXED:
            case DMA_DEST_INCR:
            case DMA_DEST_DECR:
            { /* @suppress("Empty compound statement") @suppress("Block comment") */
                /* No action to be performed */
            }
            break;
            
            default:
            {
                /* Invalid destination addressing */
                return (DMA_ERROR_MODE);
            }
            break;
        }

        /* Check repeat block area */
        switch ((p_data_cfg->mode) & DMA_REPEAT_BLOCK_MASK)
        {
            case DMA_REPEAT_BLOCK_DEST:
            case DMA_REPEAT_BLOCK_SRC:
            { /* @suppress("Empty compound statement") @suppress("Block comment") */
                /* No action to be performed */
            }
            break;
            
            default:
            {
                /* Invalid repeat block selected */
                return (DMA_ERROR_MODE);
            }
            break;
        }

        /* Check chain transfer mode */
        switch ((p_data_cfg->mode) & DMA_CHAIN_MODE_MASK)
        {
            case DMA_CHAIN_DISABLE:
            case DMA_CHAIN_CONTINUOUSLY:
            case DMA_CHAIN_NORMAL:
            { /* @suppress("Empty compound statement") @suppress("Block comment") */
                /* No action to be performed */
            }
            break;
            
            default:
            {
                /* Invalid chain transfer mode */
                return (DMA_ERROR_MODE);
            }
            break;
        }
        
        /* "Interrupt selected" is not checked                      */
        /*  because there is no error pattern                        */
#endif /* (1 == DTC_CFG_PARAM_CHECKING_ENABLE) */

        p_transfer_info->mra_mrb = ((uint32_t)(p_data_cfg->mode) << 16); // @suppress("Cast comment")
        p_transfer_info->sar     = p_data_cfg->src_addr;
        p_transfer_info->dar     = p_data_cfg->dest_addr;
        switch (((p_data_cfg->mode) & DMA_TRANSFER_MODE_MASK))
        {
            case DMA_MODE_NORMAL:
            {
                p_transfer_info->cra_crb = ((p_data_cfg->transfer_count & 0x0000FFFF) << 16);
            }
            break;
    
            case DMA_MODE_BLOCK:
            {
                /* CRAL(Transfer Count) set */
                p_transfer_info->cra_crb = ((p_data_cfg->block_size & 0x000000FF) << 16);

                /* CRAH(Reload Count) set */
                p_transfer_info->cra_crb |= ((p_data_cfg->block_size & 0x000000FF) << 24);

                /* CRB(Block transfer count) set */
                p_transfer_info->cra_crb |= (p_data_cfg->transfer_count & 0x0000FFFF);
            }
            break;
            case DMA_MODE_REPEAT:
            {
                /* CRAL(Transfer Count) set */
                p_transfer_info->cra_crb = ((p_data_cfg->transfer_count & 0x000000FF) << 16);

                /* CRAH(Reload Count) set */
                p_transfer_info->cra_crb |= ((p_data_cfg->transfer_count & 0x000000FF) << 24);
            }
            break;
            default:
            { /* @suppress("Empty compound statement") @suppress("Block comment") */
                /* No action to be performed */
            }
            break;
        }
        
        /* Check chain end? */
        if ((false == chain_set_en) ||
            (DMA_CHAIN_DISABLE == ((p_data_cfg->mode) & DMA_CHAIN_MODE_MASK)))
        {
            chain_end = true;
        }
        
        /* Read address increment */
        p_transfer_info ++;
        p_data_cfg ++;
    }
    return (DMA_OK);
}/* End of function dtc_create_param() */

/****************************************************************************************************************//**
 * @brief       In this function, it is possible to start / stop DTC, enable / disable DTC activation factor, 
 *              enable / disable read skip function, cancel chain transfer, and so on.
 * @param[in]   cmd     DTC Operation command
 * @param[in]   p_arg   Pointer to Argument by command
 * @retval      DMA_OK          DTC control command accepted.
 * @retval      DMA_ERROR       DTC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DTC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DTC_Control */
static e_dma_err_t R_DTC_Control(e_dma_cmd_t cmd, void const * const p_arg) // @suppress("Function length")
{
    e_dma_err_t result = DMA_OK;
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0); // @suppress("Cast comment")
    uint32_t irq;
    st_dma_transfer_data_t *p_dtc_info;
    
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    
    if (DMA_FLAG_OPENED != (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }
#endif
    switch (cmd)
    {
        /* DTC start */
        case DMA_CMD_START:
        {
            /* DTCST   - DTC Module Start Register
               b0      - DTCST - DTC Module Start - DTC module start */
            DTC->DTCST_b.DTCST = 1;
        }
        break;
        
        /* DTC stop */
        case DMA_CMD_STOP:
        {
            /* DTCST   - DTC Module Start Register
               b0      - DTCST - DTC Module Start - DTC module stop */
            DTC->DTCST_b.DTCST = 0;
        }
        break;
        
        /* DTC transfer activation factor enable */
        case DMA_CMD_ACT_SRC_ENABLE:
        {
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }

            /* Check whether the DTC transfer activation factor is set in the system */
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= (*((IRQn_Type*)p_arg)))
            {
                result = DMA_ERROR;
                break;
            }
#endif
            irq = (uint32_t)(*((IRQn_Type*)p_arg)); // @suppress("Cast comment")

#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Check DTC vector table */
            if (NULL == gsp_dtc_vector_table[irq])
            {
                result = DMA_ERROR;
                break;
            }
#endif
            /* Set IELSRn.DTCE bit. */
            (*(p_ielsr + irq)) |= DTC_PRV_IELSR_DTCE;
        }
        break;

        /* DTC transfer activation factor disable */
        case DMA_CMD_ACT_SRC_DISABLE:
        {
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }

            /* Check whether the DTC transfer activation factor is set in the system */
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= (*((IRQn_Type*)p_arg)))
            {
                result = DMA_ERROR;
                break;
            }
#endif
            irq = (uint32_t)(*((IRQn_Type*)p_arg)); // @suppress("Cast comment")

#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Check DTC vector table */
            if (NULL == gsp_dtc_vector_table[irq])
            {
                result = DMA_ERROR;
                break;
            }
#endif
            /* Clear IELSRn.DTCE bit. */
            (*(p_ielsr + irq)) &= (~DTC_PRV_IELSR_DTCE);
        }
        break;
        
        /* Read skip enable setting */
        case DMA_CMD_DATA_READ_SKIP_ENABLE:
        {
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }
#endif
            /* Read skip enable/disable check */
            if (true == (*(uint8_t*)p_arg))
            {
                /* DTCCR   - DTC Control Register
                   b4      - RRS - DTC Transfer Information Read Skip Enable - Transfer information read is 
                                                                               skipped when vector numbers match. */
                DTC->DTCCR_b.RRS = 1;
                gs_dtc_resources.info->read_skip = 1;
            }
            else
            {
                /* DTCCR   - DTC Control Register
                   b4      - RRS - DTC Transfer Information Read Skip Enable - Transfer information read is not skipped */
                DTC->DTCCR_b.RRS = 0;
                gs_dtc_resources.info->read_skip = 0;
            }
        }
        break;
        
        /* Chain transfer abort command */
        case DMA_CMD_CHAIN_TRANSFER_ABORT:
        {
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }

            /* Check whether the DTC transfer activation factor is set in the system */
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= ((st_dma_chain_abort_t*)p_arg)->act_src)
            {
                result = DMA_ERROR;
                break;
            }
#endif
            irq = (uint32_t)(((st_dma_chain_abort_t*)p_arg)->act_src); // @suppress("Cast comment")

#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* If there is no DTC information in the target request signal, an error is returned */
            if (NULL == gsp_dtc_vector_table[irq])
            {
                result = DMA_ERROR;
                break;
            }
#endif
            p_dtc_info = gsp_dtc_vector_table[irq];

            /* Chain transfer setting clear */
            for (uint16_t i = 0; i < ((st_dma_chain_abort_t*)p_arg)->chain_transfer_nr; i++)
            {
                p_dtc_info->mra_mrb &= (~DTC_PRV_CHAIN_ENABLE);
                p_dtc_info++;
            }
        }
        break;
        
        case DMA_CMD_CHANGING_DATA_FORCIBLY_SET:
        {
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }

            /* Check whether the DTC transfer activation factor is set in the system */
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= ((st_dma_chg_data_t*)p_arg)->act_src)
            {
                result = DMA_ERROR;
                break;
            }
#endif
            irq = (uint32_t)(((st_dma_chg_data_t*)p_arg)->act_src); // @suppress("Cast comment")
            p_dtc_info = gsp_dtc_vector_table[irq];

#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* If there is no DTC information in the target request signal, an error is returned */
            if (NULL == p_dtc_info)
            {
                result = DMA_ERROR;
                break;
            }
#endif
            /* DTCCR   - DTC Control Register
               b4      - RRS - DTC Transfer Information Read Skip Enable - Transfer information read is not skipped */
            DTC->DTCCR_b.RRS = 0;

            /* Taget dtc infomation rewrite */
            result = dtc_create_param(&p_dtc_info[((st_dma_chg_data_t*)p_arg)->chain_transfer_nr],
                                        &(((st_dma_chg_data_t*)p_arg)->cfg_data), false); // @suppress("Cast comment")

            /* Restore RRS bit. */
            DTC->DTCCR_b.RRS = gs_dtc_resources.info->read_skip;
        }
        break;
        
        case DMA_CMD_REFRESH_DATA:
        {
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
            /* Parameter null check */
            if (NULL == p_arg)
            {
                result = DMA_ERROR;
                break;
            }
            /* Check whether the DTC transfer activation factor is set in the system */
            if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= ((st_dma_refresh_data_t*)p_arg)->act_src)
            {
                result = DMA_ERROR;
                break;
            }
#endif
            result = dtc_cmd_refresh((st_dma_refresh_data_t*)p_arg);
        }
        break;
        case DMA_CMD_SOFTWARE_TRIGGER:
        case DMA_CMD_AUTO_CLEAR_SOFT_REQ:
        case DMA_CMD_REFRESH_EXTRA:
        default:
        {
            result = DMA_ERROR;
        }
        break;
        
    }
    return (result);
}/* End of function R_DTC_Control() */

/****************************************************************************************************************//**
 * @brief       Enable the DTC transfer complete interrupt
 * @param[in]   int_fact    Interrupt factor
 * @param[in]   p_callback  Callback event for DTC complete.
 * @retval      DMA_OK          DTC complete settting successful.
 * @retval      DMA_ERROR       DTC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DTC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DTC_InterruptEnable */
static e_dma_err_t R_DTC_InterruptEnable(uint16_t const int_fact, dma_cb_event_t const p_callback)
{
    e_dma_err_t result = DMA_OK;
    
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    /* DTC open check */
    if (DMA_FLAG_OPENED != (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        result = DMA_ERROR;
    }
    
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE) // @suppress("Cast comment")
    {
        result = DMA_ERROR_SYSTEM_SETTING;
    }
    
    if (0 != (int_fact & (~DMA_INT_COMPLETE)))
    {
        result = DMA_ERROR;
    }
    
    if (DMA_OK != result)
    {
        return (result);
    }
    
#endif
    
    /* Event link setting of DTC transfer completion interrupt */
    if ((-1) == R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE, DTC_PRV_COMPLETE_INT_IESR_VAL,
                                    dtc_comp_interrupt))
    {
        result = DMA_ERROR_SYSTEM_SETTING;
    }

    /* DTC complete interrupt priority setting */
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE, DTC_COMPLETE_PRIORITY); // @suppress("Cast comment")
    if (DTC_COMPLETE_PRIORITY != R_NVIC_GetPriority(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE)) // @suppress("Cast comment")
    {
        result = DMA_ERROR_SYSTEM_SETTING;
    }
    
    if ((DMA_OK == result) && (DMA_INT_COMPLETE == int_fact))
    {
        /* callback event setting */
        gs_dtc_resources.info->cb_event = p_callback;
        
        /* Enable DTC complete interrupt */
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")
    }

    return (result);
}/* End of function R_DTC_InterruptEnable() */

/****************************************************************************************************************//**
 * @brief       Disable the DTC transfer complete interrupt
 * @retval      DMA_OK          DTC complete settting successful.
 * @retval      DMA_ERROR       DTC complete setting error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DTC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DTC_InterruptDisable */
static e_dma_err_t R_DTC_InterruptDisable(void)
{
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    e_dma_err_t result = DMA_OK;

    /* DTC open check */
    if (DMA_FLAG_OPENED != (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        result = DMA_ERROR;
    }
    
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE) // @suppress("Cast comment")
    {
        result = DMA_ERROR_SYSTEM_SETTING;
    }
    
    if (DMA_OK != result)
    {
        return (result);
    }

#endif
    
    /* Disable DTC complete interrupt */
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_DTC_COMPLETE); // @suppress("Cast comment")

    return (DMA_OK);
}/* End of function R_DTC_InterruptDisable() */

/****************************************************************************************************************//**
 * @brief       Get DTC status
 * @param[in,out]   state       Pointer to DTC status buffer
 * @retval      DMA_OK          Getting DTC state successfully.
 * @note   The DTC returns all statuses with 0
**********************************************************************************************************************/
/* Function Name : R_DTC_GetState */
static e_dma_err_t R_DTC_GetState(st_dma_state_t * const state)
{

    /* The DTC returns all statuses with 0 */
    state->in_progress = 0;
    state->esif_stat = 0;
    state->dtif_stat = 0;
    state->soft_req_stat =0;

    return (DMA_OK);
    
}/* End of function R_DTC_GetState() */

/****************************************************************************************************************//**
 * @brief       Clear DTC status
 * @param[in]   clr_state       Target clearing status
 * @retval      DMA_OK          Clear DTC state successfully.
 * @retval      DMA_ERROR       Clear DTC state error.
 * @note      Since there is no status to be cleared by the DTC, only DMA_OK is returned
**********************************************************************************************************************/
/* Function Name : R_DTC_ClearState */
static e_dma_err_t R_DTC_ClearState(uint32_t clr_state)
{
    return (DMA_OK);
}/* End of function R_DTC_ClearState() */

/*****************************************************************************************************************//**
 * @brief       Get transfer byte
 * @param[in]   dma_source   DTC transfer activation factor
 * @param[in,out]   transfer_byte       Pointer to transfer byte buffer
 * @retval      DMA_OK          Getting DTC transfer byte successfully.
 * @retval      DMA_ERROR       Getting DTC transfer byte error.
**********************************************************************************************************************/
/* Function Name : R_DTC_GetTransferByte */
static e_dma_err_t  R_DTC_GetTransferByte(int16_t const dtc_source, uint32_t *transfer_byte) // @suppress("Source line ordering")
{
    uint32_t retval = 0;
    uint32_t vector_transfer_count = 0;
    st_dma_transfer_data_t *p_dtc_info;
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0); // @suppress("Cast comment")
    uint8_t mra_sz = 0; /* Number of transfer bits at one time */

#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    /* DTC open check */
    if (DMA_FLAG_OPENED != (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        return (DMA_ERROR);
    }

    /* DTC transfer trigger check */
    if ((SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= dtc_source) || (0 > dtc_source))
    {
        return (DMA_ERROR_PARAMETER);
    }

    /* Check DTC vector table */
    if (NULL == gsp_dtc_vector_table[dtc_source])
    {
        return (DMA_ERROR);
    }
#endif
    p_dtc_info = gsp_dtc_vector_table[dtc_source];

    vector_transfer_count = gsp_dtc_vector_table_set_transfer_count[dtc_source];

    /* Check data size setting. */
    switch ((p_dtc_info->mra_mrb) & DTC_PRV_MASK_MRA_SZ)
    {
        case DTC_PRV_MRA_SZ_BYTE:
        {
            mra_sz = 1; /* 1byte */
        }
        break;

        case DTC_PRV_MRA_SZ_WORD:
        {
            mra_sz = 2; /* 2byte */
        }
        break;

        case DTC_PRV_MRA_SZ_LONG:
        {
            mra_sz = 4; /* 4byte */
        }
        break;
        
        default:
        {
            /* Invalid data size */
            return (DMA_ERROR_MODE);
        }
        break;
    }

    switch (((p_dtc_info->mra_mrb) & DTC_PRV_MASK_MRA_MD))
    {
        case DTC_PRV_MRA_MD_NORMAL:
        {
            uint32_t nor_transfer_times = ((p_dtc_info->cra_crb) >> 16) & 0x0000FFFF;
            if ((0 == nor_transfer_times)&&
                (0 != ((*(p_ielsr + dtc_source)) & DTC_PRV_IELSR_DTCE)))
            {
                nor_transfer_times = 0x10000;
            }

            retval =  mra_sz * (vector_transfer_count - nor_transfer_times);
        }
        break;

        case DTC_PRV_MRA_MD_BLOCK:
        {
            uint16_t block_size = ((p_dtc_info->cra_crb) >> 24) & 0x000000FF;
            uint16_t block_size_count = ((p_dtc_info->cra_crb) >> 16) & 0x000000FF;
            uint32_t block_trans_times = (p_dtc_info->cra_crb) & 0x0000FFFF;
            
            if (0 == block_size)
            {
                block_size = 256;
            }

            if (0 == block_size_count)
            {
                block_size_count = 256;
            }
            
            if ((0 == block_trans_times )&&
                (0 != ((*(p_ielsr + dtc_source)) & DTC_PRV_IELSR_DTCE)))
            {
                block_trans_times = 65536;
            }
            
            retval = mra_sz * ( ((vector_transfer_count - block_trans_times) * block_size) + (block_size - block_size_count) );
        }
        break;

        case DTC_PRV_MRA_MD_REPEAT:
        {
            uint16_t transfer_times = ((p_dtc_info->cra_crb) >> 24) & 0x000000FF;
            uint16_t transfer_count = ((p_dtc_info->cra_crb) >> 16) & 0x000000FF;
            
            if (0 == transfer_times)
            {
                transfer_times = 256;
            }
            if (0 == transfer_count)
            {
                transfer_count = 256;
            }
            retval =  mra_sz * (transfer_times - transfer_count);
        }
        break;

        default:
        {
            /* Invalid mode */
            return (DMA_ERROR_MODE);
        }
        break;
    }

    *transfer_byte = retval;
    
    return (DMA_OK);
}/* End of function R_DTC_GetTransferByte() */

/****************************************************************************************************************//**
 * @brief       Get DTC available source
 * @param[out]  src_info     Pointer to DTC available source data buffer
 * @retval      DMA_OK          Getting DTC available source data successfully.
 * @note    The available DTC factor information (act_src) is 64 bits,
 *          where bit0 corresponds to NVIC number 0 and bit31 corresponds to NVIC number 31.
**********************************************************************************************************************/
/* Function Name : R_DTC_GetAvailabilitySrc */
static e_dma_err_t R_DTC_GetAvailabilitySrc(uint64_t * const src_info)
{
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    /* DTC open check */
    if (DMA_FLAG_OPENED != (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        return DMA_ERROR;
    }
#endif
    /* The DTC returns available Src */
    *src_info = gs_dtc_available_src;

    return (DMA_OK);
}/* End of function R_DTC_GetAvailabilitySrc() */

/****************************************************************************************************************//**
 * @brief       Release DTC transfer source infomation.
 * @param[in]   dtc_source      DTC transfer trigger source.
 * @retval      DMA_OK          DTC transfer source infomation Release successful.
 * @retval      DMA_ERROR       DTC transfer infomation create error.
 * @retval      DMA_ERROR_SYSTEM_SETTING    System setting of DTC transfer trigger failed.
**********************************************************************************************************************/
/* Function Name : R_DTC_Release */
static e_dma_err_t R_DTC_Release(int16_t const dtc_source)
{
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0); // @suppress("Cast comment")
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    e_dma_err_t result = DMA_OK;
    /* DTC open check */
    if (DMA_FLAG_OPENED != (gs_dtc_resources.info->flags & DMA_FLAG_OPENED))
    {
        result = DMA_ERROR;
    }
    if (SYSTEM_IRQ_EVENT_NUMBER_NOT_USED <= dtc_source) // @suppress("Cast comment")
    {
        result = DMA_ERROR_SYSTEM_SETTING;
    }
    if (DMA_OK != result)
    {
        return (result);
    }
#endif

    /* Disable all DTC transfer factors */
    (*(p_ielsr + (uint32_t)dtc_source)) &= (~DTC_PRV_IELSR_DTCE); // @suppress("Cast comment")
    R_SYS_EnterCriticalSection();
    /* Clear the specified bit */
    gs_dtc_available_src &= ~(0x1UL << dtc_source);
    R_SYS_ExitCriticalSection();
    /* Clear DTC vector table */
    gsp_dtc_vector_table[dtc_source] = NULL;

    if (0 == gs_dtc_available_src)
    {
        /* 1 or more create dtc table */
        gs_dtc_resources.info->flags &= (~DMA_FLAG_CREATED);
    }
    return DMA_OK;
}

/****************************************************************************************************************//**
* @brief      dtc complate interrupt function
**********************************************************************************************************************/
/* Function Name : dtc_comp_interrupt */
static void dtc_comp_interrupt(void) // @suppress("ISR declaration")
{
    if (NULL != gs_dtc_resources.info->cb_event) // @suppress("Cast comment")
    {
        gs_dtc_resources.info->cb_event();
    }
}/* End of function dtc_comp_interrupt() */

/****************************************************************************************************************//**
 * @brief       Refresh the DMA transfer information 
 * @param[in]   p_data   Pointer to refresh data
 * @retval      DMA_OK          DMAC control command accepted.
 * @retval      DMA_ERROR       DMAC control command not accepted.
 * @retval      DMA_ERROR_MODE  The specified mode is invalid.
 * @retval      DMA_ERROR_PARAMETER Parameter setting is invalid.
**********************************************************************************************************************/
/* Function Name : dtc_cmd_refresh */
static e_dma_err_t dtc_cmd_refresh(st_dma_refresh_data_t const * const p_data)
{
    st_dma_transfer_data_t *p_dtc_info;
    uint32_t * p_ielsr = (uint32_t *)(&ICU->IELSR0); // @suppress("Cast comment")
    uint32_t irq;
    /* Address calculation */
    irq = (uint32_t)(p_data->act_src); // @suppress("Cast comment")
    p_dtc_info = gsp_dtc_vector_table[irq];
    p_dtc_info += p_data->chain_transfer_nr;
#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    /* If there is no DTC information in the target request signal, an error is returned */
    if (NULL == p_dtc_info)
    {
        return DMA_ERROR;
    }
#endif
    /* MODE check */
    uint16_t mode = (p_dtc_info->mra_mrb >> 16 );

#if (1 == DTC_CFG_PARAM_CHECKING_ENABLE)
    switch (mode & DMA_TRANSFER_MODE_MASK)
    {
        case DMA_MODE_NORMAL:
        {
            /* In the case of normal transfer, the setting range of transfer count is 1 to 65536. */
            if ((p_data->transfer_count > DTC_PRV_MAX_16BITS_COUNT_VAL) ||
                (p_data->transfer_count < DTC_PRV_MIN_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        
        case DMA_MODE_REPEAT:
        {
            /* In the case of repeat transfer, the setting range of transfer count is 1 to 256. */
            if ((p_data->transfer_count > DTC_PRV_MAX_8BITS_COUNT_VAL) ||
                (p_data->transfer_count < DTC_PRV_MIN_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        case DMA_MODE_BLOCK:
        {
            /* In the case of block transfer, the setting range of block size is 1 to 256. */
            if ((p_data->block_size > DTC_PRV_MAX_8BITS_COUNT_VAL) ||
                (p_data->block_size < DTC_PRV_MIN_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }

            /* In the case of block transfer, the setting range of transfer count is 1 to 65536. */
            if ((p_data->transfer_count < DTC_PRV_MIN_COUNT_VAL) ||
                (p_data->transfer_count > DTC_PRV_MAX_16BITS_COUNT_VAL))
            {
                return (DMA_ERROR_PARAMETER);
            }
        }
        break;
        
        default:
        {
            /* Invalid mode */
            return (DMA_ERROR_MODE);
        }
            break;
    }
#endif
    /* Clear IELSRn.DTCE bit. */
    (*(p_ielsr + irq)) &= (~DTC_PRV_IELSR_DTCE);
    
    /* DTCCR   - DTC Control Register
       b4      - RRS - DTC Transfer Information Read Skip Enable - Transfer information read is not skipped */
    DTC->DTCCR_b.RRS = 0;
    /* Rewriting transfer information */
    if ((uint32_t)NULL != p_data->src_addr)
    {
        p_dtc_info->sar = p_data->src_addr;
    }
    if ((uint32_t)NULL != p_data->dest_addr)
    {
        p_dtc_info->dar = p_data->dest_addr;
    }
    switch (mode & DMA_TRANSFER_MODE_MASK)
    {
        case DMA_MODE_NORMAL:
        {
            p_dtc_info->cra_crb = ((p_data->transfer_count & 0x0000FFFF) << 16);
            if (0 == p_data->chain_transfer_nr)
            {
                gsp_dtc_vector_table_set_transfer_count[irq] = p_data->transfer_count & 0x0000FFFF;
            }
        }
        break;

        case DMA_MODE_BLOCK:
        {
            /* CRAL(Transfer Count) set */
            p_dtc_info->cra_crb = ((p_data->block_size & 0x000000FF) << 16);

            /* CRAH(Reload Count) set */
            p_dtc_info->cra_crb |= ((p_data->block_size & 0x000000FF) << 24);

            /* CRB(Block transfer count) set */
            p_dtc_info->cra_crb |= (p_data->transfer_count & 0x0000FFFF);

            if (0 == p_data->chain_transfer_nr)
            {
                gsp_dtc_vector_table_set_transfer_count[irq] = p_data->transfer_count & 0x0000FFFF;
            }
        }
        break;
        case DMA_MODE_REPEAT:
        {
            /* CRAL(Transfer Count) set */
            p_dtc_info->cra_crb = ((p_data->transfer_count & 0x000000FF) << 16);

            /* CRAH(Reload Count) set */
            p_dtc_info->cra_crb |= ((p_data->transfer_count & 0x000000FF) << 24);

            if (0 == p_data->chain_transfer_nr)
            {
                gsp_dtc_vector_table_set_transfer_count[irq] = p_data->transfer_count & 0x000000FF;
            }
        }
        break;
        default:
        { /* @suppress("Empty compound statement") @suppress("Block comment") */
            /* No action to be performed */
        }
        break;
    }
    
    /* Restore RRS bit. */
    DTC->DTCCR_b.RRS = gs_dtc_resources.info->read_skip;
    /* When auto start is enabled, start transfer */
    if (true == p_data->auto_start)
    {
        (*(p_ielsr + irq)) |= DTC_PRV_IELSR_DTCE;
    }
    return DMA_OK;
}/* End of function dtc_cmd_refresh() */

/* End DTC Interface */

/* The Driver Instance of ACCESS STRUCT for the HAL-Driver DTC channel. */

DRIVER_DMA Driver_DTC DATA_LOCATION_PRV_DRIVER_DTC =
{
    R_DTC_GetVersion,
    R_DTC_Open,
    R_DTC_Close,
    R_DTC_Create,
    R_DTC_Control,
    R_DTC_InterruptEnable,
    R_DTC_InterruptDisable,
    R_DTC_GetState,
    R_DTC_ClearState,
    R_DTC_GetTransferByte,
    R_DTC_GetAvailabilitySrc,
    R_DTC_Release
};

/******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_dtc)
 *********************************************************************************************************************/
