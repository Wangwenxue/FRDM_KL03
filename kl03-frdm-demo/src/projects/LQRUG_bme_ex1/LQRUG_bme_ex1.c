/*
 * File:		LQRUG_bme_ex1.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "BME.h"
#include "systick.h"

#ifdef CMSIS
#include "start.h"
#endif

/* Macros */   

/* Global Variables */

/* Function Declarations */
void logic_op_demo_with_normalc(void);
void logic_op_with_bme_macros(void);

void bfi_op_with_normalc(uint32_t volatile *addr, uint32_t wdata, uint8_t bitpos, uint8_t fieldwidth);
void bfi_op_with_bme_macros(void);

void lac1_op_with_normalc(uint32_t volatile *addr, uint8_t bitpos);
void lac1_op_with_bme_macros(void);

void las1_op_with_normalc(uint32_t volatile *addr, uint8_t bitpos);
void las1_op_with_bme_macros(void);

void ubfx_op_with_normalc(uint32_t volatile *addr, uint32_t *rdata, uint8_t bitpos, uint8_t fieldwidth);
void ubfx_op_with_bme_macros(void);


/********************************************************************/
int main (void)
{
  	uint32_t rdata;
	
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
	#ifndef KEIL
    start();
	#endif
#endif
	printf("\n*********************************************\n");
	printf("\n\rRunning the LQRUG_bme_ex1 project.\n\n\r");
	printf("\*********************************************\n\n");
	
	BME_OR_W(&SIM_SCGC6, SIM_SCGC6_ADC0_MASK);	//open ADC clock gate
	
	//Calculate systick counter read overhead
	systick_init();	
	cal_systick_read_overhead();
	systick_disable();
	
	//Logic operation with normal C
	systick_init();	
	logic_op_demo_with_normalc();
	systick_disable();
	
	//Logical operation with BME C macro
	systick_init();	
	logic_op_with_bme_macros();
	systick_disable();
	
	//BFI operation with normal C
	//Change CFG1[ADIV]=0b10, BIT=5, WIDTH=2, WDATA=0x40
	//Please notice, WDATA should be properly aligned based on bit position
	systick_init();	
	bfi_op_with_normalc(&ADC0_CFG1, 2<<ADC_CFG1_ADIV_SHIFT, ADC_CFG1_ADIV_SHIFT, 0x2);
	systick_disable();
	
	ADC0_CFG1 = 0x0; //clear register for next access
	
	//BFI operation with BME C macro
	systick_init();
	bfi_op_with_bme_macros();
	systick_disable();
	
	//LAC1 operation with normal C
	systick_init();
	//enable AIEN
	BME_OR_W(&ADC0_SC1A, ADC_SC1_AIEN_MASK);
	lac1_op_with_normalc(&ADC0_SC1A, ADC_SC1_AIEN_SHIFT);
	systick_disable();
	
	//LAC1 operation with BME C macro
	systick_init();	
	lac1_op_with_bme_macros();
	systick_disable();
	
	//LAS1 operation with normal C
	systick_init();	
	las1_op_with_normalc(&ADC0_SC1A, ADC_SC1_AIEN_SHIFT);
	systick_disable();
	
	//LAS1 operation with BME C macro
	systick_init();	
	las1_op_with_bme_macros();
	systick_disable();
	
	//UBFX operation with normal C
	systick_init();
	ubfx_op_with_normalc(&ADC0_CFG1, &rdata, 0x5, 0x2);
	systick_disable();
	
	//UBFX operation with BME C macro
	systick_init();
	ubfx_op_with_bme_macros();
	systick_disable();
	
	while(1);
	return 0;
}
/********************************************************************/


void logic_op_demo_with_normalc(void)
{
    uint32_t cnt_start_value;
    uint32_t cnt_end_value;
	
	uint32_t execution_cycle;	//actual execution cycle
  
    //configure PTA5 as GPIO 
    PORTA_PCR5 = PORT_PCR_MUX(1);
  
    //configure PTA5 as output pin
    GPIOA_PDDR |= 0x20;
  
    cnt_start_value = SYST_CVR;
  
    GPIOA_PDOR ^= 0x20;
  
    cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for logic operation with normal C code: 0x%x\n\n\r", execution_cycle);
#endif		
}

void logic_op_with_bme_macros(void)
{
	uint32_t cnt_start_value;
    uint32_t cnt_end_value;
	
	uint32_t execution_cycle;	//actual execution cycle
  
    //configure PTA5 as GPIO 
    PORTA_PCR5 = PORT_PCR_MUX(1);
  
    //configure PTA5 as output pin
    GPIOA_PDDR |= 0x20;
  
    cnt_start_value = SYST_CVR;

	BME_XOR_W(&GPIOA_PDOR, 0x20);
	
	cnt_end_value = SYST_CVR;

	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for logic operation with BME macros: 0x%x\n\n\n\r", execution_cycle);
#endif		
}

void bfi_op_with_normalc(uint32_t volatile *addr, uint32_t wdata, uint8_t bitpos, uint8_t fieldwidth)
{
	uint32_t reg_val;
    uint32_t mask;
	
	uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle
	
	cnt_start_value = SYST_CVR;
	
	reg_val = *addr;   
    mask = ((1 << (fieldwidth)) - 1) << bitpos;
    reg_val = (reg_val & ~mask)|((wdata) & mask);
    *addr = reg_val;
	
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for BFI operation with normal C: 0x%x\n\n\r", execution_cycle);
#endif		
	
}

void bfi_op_with_bme_macros(void)
{
	uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle
	
	cnt_start_value = SYST_CVR;
	
	BME_BFI_W(&ADC0_CFG1, 2<<ADC_CFG1_ADIV_SHIFT, ADC_CFG1_ADIV_SHIFT, 0x02);
	
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for BFI operation with BME macro: 0x%x\n\n\n\r", execution_cycle);
#endif	
	
}


void lac1_op_with_normalc(uint32_t volatile *addr, uint8_t bitpos)
{
  	uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle
	
	uint32_t mask;
	uint8_t load_bit;	//This bit will be loaded in CPU Rx register
	
	cnt_start_value = SYST_CVR;
	
	//LAC1 operation in C code 
	mask= 1<<bitpos;
	load_bit = (*addr & mask)>>bitpos;
	*addr &= ~mask;
	
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("load_bit is %d\n\r", load_bit);
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for LAC1 operation with normal C: 0x%x\n\n\r", execution_cycle);
#endif	
}

void lac1_op_with_bme_macros(void)
{
  	uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle
	
	uint8_t load_bit;
	  
	BME_OR_W(&ADC0_SC1A, ADC_SC1_AIEN_MASK);	
	
	cnt_start_value = SYST_CVR;
	
	//clear AIEN and load current AIEN value into CPU Rx register	
	load_bit = BME_LAC1_W(&ADC0_SC1A, ADC_SC1_AIEN_SHIFT);
  
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("load bit is %d\n\r", load_bit);
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for LAC1 operation with BME macro: 0x%x\n\n\n\r", execution_cycle);
#endif	
	
}

void las1_op_with_normalc(uint32_t volatile *addr, uint8_t bitpos)
{
  	uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle
	
	uint32_t mask;
	uint8_t load_bit;	//This bit will be loaded in CPU Rx register
	
	cnt_start_value = SYST_CVR;
	
	//LAS1 operation in C code 
	mask= 1<<bitpos;
	load_bit = (*addr & mask)>>bitpos;
	*addr |= mask;
	
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("load_bit is %d\n\r", load_bit);
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for LAS1 operation with normal C: 0x%x\n\n\r", execution_cycle);
#endif	
}

void las1_op_with_bme_macros(void)
{
  	uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle		
	
	uint8_t load_bit;
	
	BME_LAC1_W(&ADC0_SC1A, ADC_SC1_AIEN_SHIFT);	//clear the bit first
	
	cnt_start_value = SYST_CVR;
	
	//set AIEN and load current AIEN value into CPU Rx register	
	load_bit = BME_LAS1_W(&ADC0_SC1A, ADC_SC1_AIEN_SHIFT);
  
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("load bit is %d\n\r", load_bit);
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for LAS1 operation with bme macro: 0x%x\n\n\n\r", execution_cycle);
#endif	
	
}

void ubfx_op_with_normalc(uint32_t volatile *addr, uint32_t *rdata, uint8_t bitpos, uint8_t fieldwidth)
{
    uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle		
	
	uint32_t reg_val;
	uint32_t mask;
	
	cnt_start_value = SYST_CVR;
	
	reg_val = *addr;
	mask  = ((1 << (fieldwidth)) - 1) << bitpos;      // generate bit mask
	*rdata = (reg_val &  mask) >> bitpos;               // read data returned to core
	
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("extracted data is %d\n\r", *rdata);
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for UBFX operation with normal C: 0x%x\n\n\r", execution_cycle);
#endif	
}

void ubfx_op_with_bme_macros(void)
{
    uint32_t cnt_start_value;
    uint32_t cnt_end_value;
		
	uint32_t execution_cycle;	//actual execution cycle	
	
	uint32_t rdata;
	  
	cnt_start_value = SYST_CVR;
	
	rdata = BME_UBFX_W(&ADC0_CFG1, ADC_CFG1_ADIV_SHIFT, 0x2);
	
	cnt_end_value = SYST_CVR;
	
	execution_cycle = cnt_start_value - cnt_end_value - overhead;
	
#ifdef DEBUG_PRINT
	printf("extracted data is %d\n\r", rdata);
	printf("systick start value: 0x%x\n\r", cnt_start_value);
	printf("systick end value: 0x%x\n\r", cnt_end_value);
	printf("actual execution cycle for UBFX operation with bme macro: 0x%x\n\n\n\r", execution_cycle);
#endif	
}
