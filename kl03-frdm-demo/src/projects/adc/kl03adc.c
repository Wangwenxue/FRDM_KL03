/*
 * File:	  	adc.c
 * Purpose:		Demo UART Function
=======
 * File:		adc.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "adc.h"

void Delayms(unsigned int cnt)
{
  unsigned int i;
  for(;cnt>0;--cnt)
    for(i=500;i>0;--i)
    {
			#ifdef KEIL
       __nop();  
       __nop(); 			
			#else
			 asm("nop");
       asm("nop");
			#endif
    }
}

/********************************************************************/
int main (void)
{
	char ch;
  unsigned short Vout = 0;   

  printf("\nRunning adc example project.\n");
	
	InitADC();

	while(1)
	{
		    TriggerADC(29);                                                      /*   Channel 29£∫VDD    */  
       // TriggerADC(30);                                                    /*   Channel 30£∫Vss    */    
       // TriggerADC(27);                                                    /*   Channel 27£∫Bandgap   */   		
		    Vout = ReadADCPoll();                                                /*   ªÒ»°adc÷µ          */
        printf("Vout=%d\n",Vout);
			  Delayms(100);

	} 
}
/********************************************************************/