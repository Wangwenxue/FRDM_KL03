/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1


/* Example */

//#undef  VECTOR_044
//#define VECTOR_044 LPTMR0_IRQHandler


// ISR(s) are defined in your project directory.
//extern void LPTMR0_IRQHandler(void);



#endif  //__ISR_H

/* End of "isr.h" */
