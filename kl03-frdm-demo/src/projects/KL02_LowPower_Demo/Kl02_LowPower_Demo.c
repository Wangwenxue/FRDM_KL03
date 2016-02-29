#include "common.h"
#include "lptmr.h"

#define SLOW_BLINK      (10000000)
#define FAST_BLINK      (500000)
#define BLINK_DELAY     FAST_BLINK

#define RED		             (6)
#define RED_SHIFT		(1 << 6)
#define GREEN			     (7)
#define GREEN_SHIFT		(1 << 7)
#define BLUE			    (10)
#define BLUE_SHIFT	       (1 << 10)

#define RED_OFF			(GPIOB_PSOR = RED_SHIFT)
#define RED_ON			(GPIOB_PCOR = RED_SHIFT)
#define RED_TOGGLE		(GPIOB_PTOR = RED_SHIFT)

#define GREEN_OFF		(GPIOB_PSOR = GREEN_SHIFT)
#define GREEN_ON		(GPIOB_PCOR = GREEN_SHIFT)
#define GREEN_TOGGLE	        (GPIOB_PTOR = GREEN_SHIFT)

#define BLUE_OFF		(GPIOB_PSOR = BLUE_SHIFT)
#define BLUE_ON			(GPIOB_PCOR = BLUE_SHIFT)
#define BLUE_TOGGLE		(GPIOB_PTOR = BLUE_SHIFT)

/*  System mode macro  */
#define NormalWait   0x1
#define PSTOP2       0x2
#define PSTOP1       0x3
#define NormalStop   0x4
#define VLPR         0x5
#define VLPW         0x6
#define VLPS         0x7
#define VLLS3        0x8
#define VLLS1        0x9
#define VLLS0_1      0xA    // POR on
#define VLLS0_2      0xB    // POR off



/********************************************************************
                Function protetype
*********************************************************************/
void EnterLowPowerMode(unsigned char lowpowermode);
void delay(unsigned int delaytime);
void LowPowerMode_test(void);
void DisableSWDPin(void);
void InitRGB(void);


/********************************************************************
                initialize the rgb leds
*********************************************************************/
void InitRGB(void)
{
  /* Set the PTBx pin multiplexer to GPIO mode */
  PORTB_PCR6  = PORT_PCR_MUX(1);
  PORTB_PCR7  = PORT_PCR_MUX(1);
  PORTB_PCR10  = PORT_PCR_MUX(1);
  /* Set the initial output state to high */
  GPIOB_PSOR  = RED_SHIFT | GREEN_SHIFT | BLUE_SHIFT;
  /* Set the pins direction to output */
  GPIOB_PDDR  = RED_SHIFT | GREEN_SHIFT | BLUE_SHIFT;
}


/********************************************************************
                disable the SWD pin or big Idd
*********************************************************************/
void DisableSWDPin(void)
{
  PORTA_PCR0 = PORT_PCR_MUX(1);
  PORTA_PCR2 = PORT_PCR_MUX(1);
}



/********************************************************************
                Low power mode configuration

Note:
1. If entering VLPR mode, MCG has to be configured and enter BLPE mode or BLPI
mode with the Fast IRC clock selected(C2[IRCS]=1). 
   see Table 24-18. MCG modes of operation BLPI mode
2. when switch lowpower mode, a reset is recommended.
*********************************************************************/
void EnterLowPowerMode(unsigned char lowpowermode)
{
	  volatile unsigned int dummyread;
    switch(lowpowermode)
    {
    case NormalWait:
      printf("\n enter into wait mode--.\n");
      wait ();
      break;
    case NormalStop:
      printf("\n enter into stop mode--.\n");
      stop ();
      break;
    case PSTOP2:
      printf("\n enter into partial stop 2 mode--.\n");
      SMC_STOPCTRL = SMC_STOPCTRL_PSTOPO(2); // PSTOP2 - Partial Stop with system clock disabled and bus clock enabled    
      /*wait for write to complete to SMC before stopping core */  
      dummyread = SMC_STOPCTRL;
      dummyread++; 
      stop ();
      break;
    case PSTOP1:
      printf("\n enter into partial stop 1 mode--.\n");
      SMC_STOPCTRL = SMC_STOPCTRL_PSTOPO(1); // PSTOP1 - Partial Stop with system clock and bus clock disabled    
      /*wait for write to complete to SMC before stopping core */  
      dummyread = SMC_STOPCTRL;
      dummyread++;     
      stop ();
      break;
    case VLPR:
      printf("\n enter into VLPR mode--. No need to wake up, just reset.\n");
      /* configure MCGOUTCLK as 4MHz */
      // FBI mode
      MCG_C1 |= MCG_C1_CLKS(1);              // Internal reference clock is selected
      MCG_C2 |= MCG_C2_IRCS_MASK;            // Fast internal reference clock selected,4MHz
      MCG_SC  = 0x0;                         // 4M/1
      delay(10);
      // BLPI mode  
      MCG_C2 |= MCG_C2_LP_MASK;              // FLL is disabled in bypass modes (lower power), BLPI
      
      SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0) | // core clock 4M/1
                     SIM_CLKDIV1_OUTDIV4(4);  // Bus clock 4M/1/5
      
      
      SMC_PMPROT = SMC_PMPROT_AVLP_MASK;    // VLPR, VLPW and VLPS are allowed
      SMC_PMCTRL = SMC_PMCTRL_RUNM(2);      // 10:Very-Low-Power Run mode (VLPR),00:normal run     
      break;
    case VLPW:
      printf("\n enter into VLPW mode--.\n");
      
      /* enter VLPR first */
      // FBI mode
      MCG_C1 |= MCG_C1_CLKS(1);              // Internal reference clock is selected
      MCG_C2 |= MCG_C2_IRCS_MASK;            // Fast internal reference clock selected,4MHz
      MCG_SC  = 0x0;                         // 4M/1  
      delay(10);
      // BLPI mode  
      MCG_C2 |= MCG_C2_LP_MASK;              // FLL is disabled in bypass modes (lower power), BLPI
      
      SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0) | // core clock 4M/1
                     SIM_CLKDIV1_OUTDIV4(4);  // Bus clock 4M/1/5
      
      
      SMC_PMPROT = SMC_PMPROT_AVLP_MASK;    // VLPR, VLPW and VLPS are allowed
      SMC_PMCTRL = SMC_PMCTRL_RUNM(2);      // 10:Very-Low-Power Run mode (VLPR),00:normal run  

      wait ();
      break;
    case VLPS:
      printf("\n enter into VLPS mode--.\n");   
      SMC_PMPROT = SMC_PMPROT_AVLP_MASK;  
      SMC_PMCTRL = SMC_PMCTRL_STOPM(2);     // 10:Very-Low-Power Stop (VLPS)
      /*wait for write to complete to SMC before stopping core */  
      dummyread = SMC_PMCTRL;
      dummyread++;
      stop ();
      break;
    case VLLS3:
      printf("\n enter into VLLS3 mode--.\n");
      SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;    
      SMC_PMCTRL = SMC_PMCTRL_STOPM(4);      
      SMC_STOPCTRL = SMC_PMCTRL_STOPM(3);    // VLLS3
      /*wait for write to complete to SMC before stopping core */  
      dummyread = SMC_STOPCTRL;
      dummyread++;
      stop ();
      break;
    case VLLS1:
      printf("\n enter into VLLS1 mode--.\n");
      SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;    
      SMC_PMCTRL = SMC_PMCTRL_STOPM(4);      
      SMC_STOPCTRL = SMC_PMCTRL_STOPM(1);    // VLLS1
      /*wait for write to complete to SMC before stopping core */  
      dummyread = SMC_STOPCTRL;
      dummyread++;
      stop ();
      break;
    case VLLS0_1:
      printf("\n enter into VLLS0_1 mode--POR ON.\n");
      SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;    
      SMC_PMCTRL = SMC_PMCTRL_STOPM(4);      
      SMC_STOPCTRL = SMC_PMCTRL_STOPM(0);    // VLLS0
      /*wait for write to complete to SMC before stopping core */
      dummyread = SMC_STOPCTRL;
      dummyread++;
      stop ();
    case VLLS0_2:
      printf("\n enter into VLLS0_2 mode--POR OFF.\n");
      SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;    
      SMC_PMCTRL = SMC_PMCTRL_STOPM(4);      
      SMC_STOPCTRL = SMC_PMCTRL_STOPM(0) | SMC_STOPCTRL_PORPO_MASK;  // disable POR circuit 
      /*wait for write to complete to SMC before stopping core */  
      dummyread = SMC_STOPCTRL;
      dummyread++;		
      stop ();
      break;
    default:
      break;
    }
}


/********************************************************************
                         delay
*********************************************************************/
void delay(unsigned int delaytime)
{
  int i;
    while(delaytime--)
    {
      for(i=20000;i>0;i--);  // about 1 ms
    }
}



/********************************************************************
                  low power mode function
*********************************************************************/
void LowPowerMode_test(void)
{    
  unsigned char ReceivedMode,receivedchar;  
  printf("\n\n KL02 Low Power Mode Test. Default waken up by LPTMR 5s later.\n");
  do
  {
    printf("\n ********************************************************* \n");
    printf("\n KL02 Power Managerment Test, can test the Idd in all kinds of mode.");
    printf("\n 1.Wait    mode");
    printf("\n 2.PStop2  mode");
    printf("\n 3.PStop1  mode");
    printf("\n 4.Stop    mode");
    printf("\n 5.VLPR    mode");
    printf("\n 6.VLPW    mode");
    printf("\n 7.VLPS    mode");
    printf("\n 8.VLLS3   mode");      
    printf("\n 9.VLLS1 mode");
    printf("\n A(a).VLLS0-1 mode -- POR ON");  
    printf("\n B(b).VLLS0-2 mode -- POR OFF\n");     
    printf("\n Wake up source: LPTMR, Reset or NMI\n"); 
    printf("\n ******************************************************** \n");
    
    printf("\n POR before changing into another power mode.\n"); 
    printf("\n Enter the key number to choose one mode, mode a and b need external clock input from Extal pin. ");  
    receivedchar = in_char();
    if(receivedchar >= '1' && receivedchar <= '9')
      ReceivedMode = receivedchar - 0x30;
    if(receivedchar >= 'A' && receivedchar <= 'B') // A:0x41(65)
      ReceivedMode = receivedchar - 0x37;
    if(receivedchar >= 'a' && receivedchar <= 'b') // a:0x65(97)
      ReceivedMode = receivedchar - 0x57;    
    printf("%d",ReceivedMode); 
  }while(ReceivedMode > 12 || ReceivedMode < 1);
  
  //lptmr_init(32767 *2, LPTMR_USE_OSCERCLK);     // 32.768K clock, about 2s
  //lptmr_init(32767 *2, LPTMR_USE_ERCLK32);      // 32.768K clock, about 2s
  lptmr_init(5 * 1000, LPTMR_USE_LPOCLK);  // 1K clock, about 5
   
  switch(ReceivedMode)
  {
    case NormalWait:
      EnterLowPowerMode(NormalWait);
      break;
    case PSTOP2:
      EnterLowPowerMode(PSTOP2);
      break;
    case PSTOP1:
      EnterLowPowerMode(PSTOP1);
      break;
    case NormalStop:
      EnterLowPowerMode(NormalStop);
      break;
    case VLPR:
      EnterLowPowerMode(VLPR);
      break;
    case VLPW:
      EnterLowPowerMode(VLPW);
      break;
    case VLPS:
      EnterLowPowerMode(VLPS);
      break; 
    case VLLS3:
      EnterLowPowerMode(VLLS3);
      break;
    case VLLS1:
      EnterLowPowerMode(VLLS1);
      break;
    case VLLS0_1: 
      EnterLowPowerMode(VLLS0_1);
      break;
    case VLLS0_2:
      EnterLowPowerMode(VLLS0_2);
      break;
    default:
      printf("\n Wrong power mode, please double check again");
      break;
  }
  
   delay(40);
   printf("\n wake up from low power mode\n");
}


/********************************************************************/
int main (void)
{
  printf("\n\n\nRunning KL02 sample project.\n\n");
  
  InitRGB();
  clk_monitor_0(0);
  LowPowerMode_test();
  
  while(1)
  {   
    BLUE_TOGGLE;
    GREEN_TOGGLE;
    delay(100);
  }
  
  return 1;
}


/********************************************************************/
void LPTMR_ISR(void)
{
  LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;
  printf("\n****LPTMR ISR entered*****\r\n");
}