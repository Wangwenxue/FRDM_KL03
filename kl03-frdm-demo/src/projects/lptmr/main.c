/*
 * File:		main.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "lptmr.h"

/********************************************************************/
#define MCGIRCLK		0
#define LPO					1
#define ERCLK32K		2
#define OSCERCLK		3
/********************************************************************/


/********************************************************************/
int main (void)
{
	
		SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK				/*打开PORTA PORTB时钟门*/
							| SIM_SCGC5_PORTB_MASK;
					
		PORTB_PCR11 |= PORT_PCR_MUX(1);						/*PTB11复用为功能11 GPIO功能*/
		GPIOB_PDDR |= (1<<11);										/*PTB11设置为输出*/
		GPIOB_PSOR |= (1<<11);										/*PTB11置1,关闭绿灯*/  
	
		lptmr_init(500,LPO);											/*lptmr使用LPO,定时500ms*/
		while(1)
		{

		} 
}
/********************************************************************/

void LPTMR0_IRQHandler(void)
{
	//write 1 to TCF to clear the LPT timer compare flag
	LPTMR_CSR |=  LPTMR_CSR_TCF_MASK;

	GPIOB_PTOR |= (1<<11);											/*翻转PTB11 绿灯闪烁*/
}
