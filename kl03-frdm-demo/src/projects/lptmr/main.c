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
	
		SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK				/*��PORTA PORTBʱ����*/
							| SIM_SCGC5_PORTB_MASK;
					
		PORTB_PCR11 |= PORT_PCR_MUX(1);						/*PTB11����Ϊ����11 GPIO����*/
		GPIOB_PDDR |= (1<<11);										/*PTB11����Ϊ���*/
		GPIOB_PSOR |= (1<<11);										/*PTB11��1,�ر��̵�*/  
	
		lptmr_init(500,LPO);											/*lptmrʹ��LPO,��ʱ500ms*/
		while(1)
		{

		} 
}
/********************************************************************/

void LPTMR0_IRQHandler(void)
{
	//write 1 to TCF to clear the LPT timer compare flag
	LPTMR_CSR |=  LPTMR_CSR_TCF_MASK;

	GPIOB_PTOR |= (1<<11);											/*��תPTB11 �̵���˸*/
}
