/*
 * File:		fredom_test.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "freedom_gpio.h"
#include "hal_i2c.h"
#include "hal_dev_mma8451.h"
#include "angle_cal.h"
#include "task_mgr.h"
#include "global.h"
#include "mcg.h"
#include "lptmr.h"


// for TSS
#include "TSS_API.h"
#include "app_init.h"
#include "events.h"
#include "isr.h"
#include "main.h"
#include "TSS_SystemSetup.h"

// slide to unlock
unsigned char LOCK;

void accel_demo(void);
void accel_read(void);
void TPM_init(void);
void LPTMR_init(void);


/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/
signed short accel_x, accel_y, accel_z;
signed short resultx, resulty, resultz;

#define RED_MASK   0x01
#define GREEN_MASK 0x02
#define BLUE_MASK  0x04

void accel_init(void)
{
    unsigned char tmp;
//    printf("****Accelerometor init ****\n");
   //Configure MMA8451 sensor //MMA8451Q
    hal_dev_mma8451_init();      //Initialize I2C modules
    tmp = hal_dev_mma8451_read_reg(0x2a);
    hal_dev_mma8451_write_reg(0x2a,tmp|0x01);
}

void accel_read(void)
{
  if((hal_dev_mma8451_read_reg(0x00)&0xf) != 0)
  {
    accel_x   = hal_dev_mma8451_read_reg(0x01)<<8;
    accel_x  |= hal_dev_mma8451_read_reg(0x02);
    accel_x >>= 2;

    accel_y   = hal_dev_mma8451_read_reg(0x03)<<8;
    accel_y  |= hal_dev_mma8451_read_reg(0x04);
    accel_y >>= 2;

    accel_z   = hal_dev_mma8451_read_reg(0x05)<<8;
    accel_z  |= hal_dev_mma8451_read_reg(0x06);
    accel_z >>= 2;

    resultx   = hal_dev_mma8451_read_reg(0x01)<<8;
    resultx  |= hal_dev_mma8451_read_reg(0x02);
    resultx >>= 8;

    resulty   = hal_dev_mma8451_read_reg(0x03)<<8;
    resulty  |= hal_dev_mma8451_read_reg(0x04);
    resulty >>= 8;

    resultz   = hal_dev_mma8451_read_reg(0x05)<<8;
    resultz  |= hal_dev_mma8451_read_reg(0x06);
    resultz >>= 8;
                       
    angle_calculation(); //-900  to  900            
    detect_fall_detection();
            
    accel_demo();
  }         
}

#define T_DELAY_LED  10

void accel_demo(void)
{       
  if (ti_accel_sampling==0)
  {
    accel_read();              
    ti_accel_sampling = 10;           
  } 
  
   SET_LED_RED((abs(xz_angle) > 450) ? 0 : (450 - abs(xz_angle)));
   SET_LED_GREEN((abs(yz_angle) > 450) ? 0 : (450 - abs(yz_angle)));
   SET_LED_BLUE((abs(xy_angle) > 450) ? 0 : (450 - abs(xy_angle)));
}
   
void TPM_init(void)
{
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTA_MASK;
  SIM_SCGC6 |=( SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM1_MASK);     
  SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); // FLL as clock
      
  PORTB_PCR6   = (0|PORT_PCR_MUX(2)); /* TPM1_CH1 enable on PTB6 */
  PORTB_PCR7   = (0|PORT_PCR_MUX(2)); /* TPM1_CH0 enable on PTB7 */
  PORTB_PCR10  = (0|PORT_PCR_MUX(2)); /* TPM0_CH1 enable on PTB10 */
   
  // TPM1 is for PWM generation, as global timer
  TPM0_CONF = TPM_CONF_GTBEEN_MASK;   // use TPM1 global time base

  TPM1_MOD  = TPM_MODULE;  /* 0x0063 / 25MHz = 4uS PWM period */
  TPM1_C0SC = TPM_Cn_MODE;   /* No Interrupts; High True pulses on Edge Aligned PWM */
  TPM1_C1SC = TPM_Cn_MODE;   /* No Interrupts; High True pulses on Edge Aligned PWM */
  TPM0_C1SC = TPM_Cn_MODE;   /* No Interrupts; High True pulses on Edge Aligned PWM */
  TPM1_C0V  = TPM_INIT_VAL;  /* 90% pulse width */
  TPM1_C1V  = TPM_INIT_VAL;  /* 90% pulse width */
  TPM0_C1V  = TPM_INIT_VAL;  /* 90% pulse width */

  TPM1_SC   = TPM_SC_CMOD(1) | TPM_SC_PS(0);     /* Edge Aligned PWM running from BUSCLK / 1 */
}

void vfn_test_accel(void)
{
  if (ti_accel_sampling==0)
  {
    accel_read();              
    ti_accel_sampling = 10;            
  }  
}

void vfn_rgb_test(void)
{
  static char seq_count=0;
   
  if (!ti_task)
  {  
   seq_count++;
   ti_task = 500;
   switch (seq_count)  
   {
    case 1: 
       SET_LED_GREEN(200);
       SET_LED_RED(0);
       SET_LED_BLUE(0);        
     break;
    case 2: 
       SET_LED_GREEN(0);
       SET_LED_RED(200);
       SET_LED_BLUE(0);
     break;
   case 3: 
       SET_LED_GREEN(0);
       SET_LED_RED(0);
       SET_LED_BLUE(200);
     break;
    case 4: 
       SET_LED_GREEN(100);
       SET_LED_RED(100);
       SET_LED_BLUE(100);      
     break;
   default:
     seq_count=0;
     next_task(accel_demo);
	   break;
   };
  }
}


const int Led_sequence[] =
{
  0x0,
  RED_MASK,
  GREEN_MASK,
  BLUE_MASK,
  RED_MASK  | GREEN_MASK,
  RED_MASK  | BLUE_MASK,
  BLUE_MASK | GREEN_MASK,
  GREEN_MASK,
  BLUE_MASK,
  RED_MASK,
  0x0,
};
  
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
  unsigned char temp;
  
  // read the register
  temp = TSS_GetASliderConfig(cASlider1.ControlId, ASlider_DynamicStatus_Register);
 
  if(temp & 0x80)  // movement flag
  {
    if(temp & 0x40) // direction
    {
      LOCK = 1;
      printf("\nLEDs are locked. Slide to unlock.\n\n");     
    }
    else
    {
      LOCK = 0;
      printf("\nLEDs are unlocked. Slide to lock.");
      printf("\nTilt the board to check the changing of LEDS' colour and brightness.\n");
    }
  }
  else
  {
    
  }

  (void) u8ControlId;
  
  return;
}

/********************************************************************/
int main (void)
{
  printf("\n\n***********************************************************\n");
  printf("\nRunning in KL02 demo project.\n");
  printf(" --Unlock the LEDs: slide from 2 to 1 on E1 pad.\n");
  printf(" --Lock the LEDs: slide from 1 to 2 on E1 pad.\n");
  printf("\n***********************************************************\n");
  printf("\nKL02 Freedom Demo. Slide to unlock first.\n\n");
  
  LOCK = 1;  // lock leds first
  
  // For Touch sensor
  /* Init HW */
  InitPorts();
  /* Default TSS init */
  TSS_Init_ASlider();
	
  gpio_init();
  LED1_ON;
  LED2_ON;
  LED3_ON;
  LED1_OFF;
  LED2_OFF;
  LED3_OFF;  
  
  TPM_init();
  lptmr_init(48, LPTMR_USE_ERCLK32);
  
  /* Enable Interrupts globally */
  EnableInterrupts;

  next_task(vfn_rgb_test);
  printf("\nRunning the freedom_test project.\n");
  accel_init();
        
  while(1)
  {
    if (TSS_Task() == TSS_STATUS_OK)
    {
       if(LOCK)
       {
          SET_LED_RED(TPM_MODULE/64);
          SET_LED_GREEN(TPM_MODULE/16);  
          SET_LED_BLUE(TPM_MODULE/16);
       }
       else
        ptr_next_task();  // do the actual function    
    }      
    /* this is special for Triton, factory test package, can remove if debug */
    /*
    if(CounterForFAT == 1000)
    {
       printf("FRDM-KL02Z Demo Application\n");
    }
    */
  } 

}
/********************************************************************/
void LPTMR_ISR(void)
{  
  static char count_mseg=125;
  LPTMR0_CSR |= 0x80; // clear flag
  
  TSS_SET_SAMPLE_INTERRUPTED(); // tell the TSS in int_ISR now
   
  if (count_mseg)count_mseg--;
  if (ti_accel_sampling) 
    ti_accel_sampling--;
  else
  {
    count_mseg=125;
    base_time125m++; 
  }
  if (ti_task) ti_task--;
  if (ti_delay) ti_delay--;
  if (ti_print) ti_print--;
  /* this is special for Triton, factory test package, can remove if debug */
/*  if(CounterForFAT++ == 1001)
  {
    CounterForFAT = 0;
  }
*/
}