/*
 * File:		KL02_GPIO_TSS_Demo.c
 * Purpose:		Main process
 *
 */
#include "TSS_API.h"
#include "app_init.h"
#include "events.h"
#include "isr.h"
#include "main.h"
//#include "common.h"


void PWM_init(void);

/*
** ===================================================================
**     Event       :  TSS1_fCallBack4 (module Events)
**
**     Component   :  TSS1 [TSS_Library]
**     Description :
**         Callback definition for Control 1. This event is enabled
**         only if Control 3 is enabled.
**         The default CallBack Name is automatically generated with
**         automatic prefix update by current Component Name. User can
**         define own name, but then the automatic name update is not
**         functional.
**     Parameters  :
**         NAME            - DESCRIPTION
**         u8ControlId     - Valid unique Identifier of
**                           the Control which generated the CallBack
**                           function. This Id can be used for finding
**                           of Callback's source Control.
**     Returns     : Nothing
** ===================================================================
*/
void TSS1_fCallBack1(TSS_CONTROL_ID u8ControlId)
{
  /* Set LED brightness */ 
  SET_LED_BLUE(cASlider1.Position * 4);

  (void) u8ControlId;
  
  return;
}

/***************************************************************************//*!
*
* @brief  Function for init of PWM for LED control 
*
* @param  void
*
* @return void
*
* @remarks
*
****************************************************************************/
void PWM_init(void)
{ 
	/* already done in the TSS_fOnInit function */
   SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
   SIM_SCGC6 |=( SIM_SCGC6_TPM0_MASK); 
	
   SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);

   PORTB_PCR10  = (0|PORT_PCR_MUX(2)); /* TPM0_CH1 enable on PTB10 */

   TPM0_MOD  = TPM_MODULE;  /* 0x0063 / 25MHz = 4uS PWM period */
   TPM0_C1SC = TPM_Cn_MODE;   /* No Interrupts; High True pulses on Edge Aligned PWM */
   TPM0_C1V  = TPM_INIT_VAL;  /* 90% pulse width */

   TPM0_SC   = TPM_SC_CMOD(1) | TPM_SC_PS(0);     /* Edge Aligned PWM running from BUSCLK / 1 */
}



/***************************************************************************
* Main
* --------------------------------------------------------------------------
*
****************************************************************************/

int main (void)
{
    printf("\n\n*******************************************************************************\n");
    printf("\nRunning in KL02_GPIO_TSS_Demo.");
    printf("\nSlide on the electrode to change the LED's brightness.\n");
    printf("\nKL02 has no TSI, but TSSW takes the GPIO method to realize touch sensor.\n");
    printf("\n*******************************************************************************\n");
    
    /* Init HW */
    InitPorts();
    /* Default TSS init */
    TSS_Init_ASlider();
    /* Init PWM for LED control */
    PWM_init();
    /* Enable Interrupts globally */
    EnableInterrupts;	
    /* Main Loop */
    for(;;)
    {
        /* TSS Task */
        if (TSS_Task() == TSS_STATUS_OK)
        {
        }     
        /* Write your code here ... */
    }
}

/***************************************************************************/
