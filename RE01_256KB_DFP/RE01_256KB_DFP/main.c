/*********************************************************************************************************************
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
#include "RE01_256KB.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"

/* Start user code for include. */

/* End user code.*/

/* Start user code for global.*/

/* End user code. */

/* Functions */
void NMI_Handler( void )  __attribute__ ((section(".ramfunc"))) ;     /* This is NMI Handler for User template*/
void LVD_for_EHC( void ) __attribute__ ((section(".ramfunc"))) ;   /* This is  User template code of EHC initialization in NMI Handler*/


/***********************************************************************************************************************
* Function Name: main
* Revision     : 1.20
* Description  : main function. Please add the code for your system.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
int main()
{
  uint32_t data;

  /**** It recommends use this part as template this device's start-up code. ****/
  /* If the API functions are allocated to RAM, this function must be executed first */
  /* It copies Driver codes, which change the allocation area from ROM to RAM, by using r_[module]_cfg.h */ 
  /* Attention: Every code Pin.c is always allocated to RAM. These functions are called from API.  */
  R_SYS_CodeCopy();
  /* Initialize System Function Driver. */
  /* This function needs to be called after R_SYS_CodeCopy function. */
  R_SYS_Initialize();
  /* Initialize LPM Function Driver. */
  /* This function needs to be called before call R_LPM_IOPowerSupplyModeSet function. */
  R_LPM_Initialize();
  /* Set Power Supply Open Control Register (VOCR register)                                      */
  /* If you doesn't clear bits which corresponding your using port domains, MCU cannot read      */
  /* correctly value of input signal.                                                            */
  /* The VOCR register is used for control such that, when power is not being supplied to        */
  /* the AVCC0, IOVCC0, IOVCC1.                                                                  */
  /* This feature is mainly used when the device is Energy Harvesting start-up.                  */
  /* Default value is "NOT propagated" signal into internal device.                              */
  /* Please change the value of argument with your target board                                  */
  R_LPM_IOPowerSupplyModeSet((uint8_t)LPM_IOPOWER_SUPPLY_NONE);
  /**** End of template code.   ****/

  /*******************************************************************/
  /**** Write user code for user init and system operations here. ****/
  /*******************************************************************/




  while(1)
  {
    data++;
  }
  return 0;
}

/**********************************************************************************************************************
* Function Name: BoardInit
* Description  : Configure board initial setting.
*                This function is called by SystemInit() function in system_RE01_256KB.c file.
*                This is reference to perform BoardInit process. Sample code of target is Evaluation Kit RE01 256KB
*                 on Renesas. Please modify this function to suit your board.
* Arguments    : none
* Return Value : none
**********************************************************************************************************************/
void BoardInit(void)
{
    /***** This function performs at beginning of start-up after released reset. *****/
    /***** Please set pins here if your board is needed pins setting at the device start-up. ****/
    /***** This function is suiting Evaluation Kit RE01 256KB. Please change the pin setting to suit your board. *****/

    /* Handling of Unused ports (IOCVCC domain) */

    /* PORT2 Setting */
    /* Evaluation Kit RE01 256KB has DCDCs whose output is controlled by P208 and P209. */
    /* Set P208 and P209 not to be used as DCDC_EN. (output low: Disable) */
    /* It need to set P208 and P209 to enable DCDC when using EHC start-up of this device. */
    /* Set P210 as LED0. (output high) */

    /* PODR - Port Output Data
       b15-b11  PODR15 - PODR11     - Output Low Level
       b10      PODR10              - Output High Level
       b9 -b0   PODR09 - PODR00     - Output Low Level */
    PORT2->PODR = 0x0400;

    /* PDR - Port Direction
       b15-b11  PDR15 - PDR11      - Input
       b10-b8   PDR10 - PDR08      - Output
       b7 -b0   PDR07 - PDR00      - Input */
    PORT2->PDR  = 0x0700;

    /* PORT4 Setting */
    /* Set P410 as LED1. (output high) */

    /* PODR - Port Output Data
       b15-b11  PODR15 - PODR11     - Output Low Level
       b10      PODR10              - Output High Level
       b9 -b0   PODR09 - PODR00     - Output Low Level */
    PORT4->PODR = 0x0400;

    /* PDR - Port Direction
       b15-b11  PDR15 - PDR11       - Input
       b10      PDR10               - Output
       b9 -b0   PDR09 - PDR00       - Input */
    PORT4->PDR  = 0x0400;

} /* End of function BoardInit */

/**********************************************************************************************************************
* Function Name: NMI_Handler
* Description  : NMI handler for User for template code when using EHC start-up.
*                This function is called when NMI interrupt is happen such as LVD, WDT and Oscillation stop detection.
*                This is must need to create when User uses Energy harvesting start-up(EHC).
*                (EHC is enable when SYSTEM_CFG_EHC_MODE of definition in r_core_cfg.h set "1".)
*                If use uses EHC, User must need to create process to initialize EHC circuit in this device
*                when detect voltage drop using LVD1.
*                This main.c is providing such a NMI Handler template code as default.
*
* Arguments    : none
* Return Value : none
**********************************************************************************************************************/
  void NMI_Handler( void ) 
{
  volatile uint16_t f_status;
  /* Read status flag for NMI interrupts */
  f_status = ICU->NMISR;

  if(0x0004 == (f_status & 0x0004))
  {
    /* When detect LVD1 */
    LVD_for_EHC();
    return; /* This return is executed when LVD1 detection was misdetection by any noise. */
  }



} /* End of function NMI_Handler() */

/**********************************************************************************************************************
* Function Name: LVD_for_EHC
* Description  : This function performs EHC initialization when it checks No misdetection of LVD.
*                This function checks this LVD is misdetection or not.
*                When this is not misdetection, it performs initialization of EHC.
*
* Arguments    : none
* Return Value : -1 : Error : This LVD detection is misdetection.
*                -  : When Properly , this is perform while loop after cut of power supply.
**********************************************************************************************************************/
void LVD_for_EHC()
{
#if SYSTEM_CFG_EHC_MODE == (1)
   volatile uint16_t i;

   /* Disable protect for LVD */
   R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_LVD);
   /* Clear LVD1 detection flag */
   SYSC->LVD1SR_b.DET = 0;            
   /* Clear LVD1 NMI interrpt flag */
   ICU->NMICLR_b.LVD1CLR = 1;            
   while(ICU->NMISR_b.LVD1ST != 0);
   /* Enable protect for LVD */
   R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_LVD);
   /**************************************************************************/
   /* Write user's code if you have any process to do before device power off.*/
   /**************************************************************************/
   /* Wait 100m second for stabilizetion of noise */
   R_SYS_SoftwareDelay(100, SYSTEM_DELAY_UNITS_MILLISECONDS);
   /* Check if LVD detected due to noise */ 
   for(i = 0; i < 3 ; i++)
   {
      if(0 != SYSC->LVD1SR_b.MON)
      {
        /* This LVD detection is misdetection. */
        /* VCC level is higher than LVD1det level. */
        /* So, This detection is error by any noise */
          return ;
       }
    }

    /*--------------------------------------------------------------------------
     * Set the protect function of the register
     *------------------------------------------------------------------------*/
    SYSC->PRCR = 0xA503U;
    
     /*--------------------------------------------------------------------------
     * Start the LOCO operation
     *------------------------------------------------------------------------*/
    SYSC->LOCOCR = 0x00U;     /* Start LOCO */

    /*--------------------------------------------------------------------------
     * Wait the LOCO clock stabilization
     *------------------------------------------------------------------------*/
    while(0x00 != SYSC->LOCOCR)
    {
        /* Retry the writing */
        SYSC->LOCOCR = 0x00U;
    }
    
    /*--------------------------------------------------------------------------
     * Set the system clock source
     * b2-b0 : [   CKSEL] System clock source select
     *                      - [010] LOCO selection
     *------------------------------------------------------------------------*/
    SYSC->SCKSCR = 0x02U;     /* Clock source : LOCO */
    while(0x02U != SYSC->SCKSCR)
    {
        /* Retry the writing */
        SYSC->SCKSCR = 0x02U;
    }
    
    /**** Enable Stop module clock for DTC/DMAC */
    SYSC->MSTPCRA_b.MSTPA22 = 1U;     /* Stop module clock for DTC/DMAC */
    
    /* Enable EHC Charging Capacitor Quick Wake-up function */
    /* This is must need for initializing EHC Circuit when using EHC mode. */
    EHC->EHCCR1_b.QUICKMODE = 0U;
    
    while(1)
    {
        ;    /* loop */
    }
#endif /* SYSTEM_CFG_EHC_MODE == (1) */

} /*End of function of LVD_for_EHC() */


/* End of File */
