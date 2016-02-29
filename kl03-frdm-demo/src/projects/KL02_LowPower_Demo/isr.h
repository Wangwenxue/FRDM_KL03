/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef ISR_H
#define ISR_H

/*  
  #undef  VECTOR_042
  #define VECTOR_042 TSS_TSI0Isr
*/

#undef  VECTOR_044
#define VECTOR_044 LPTMR_ISR

extern void LPTMR_ISR(void);

/* Example */
/*
#undef  VECTOR_101
#define VECTOR_101 lpt_isr

// ISR(s) are defined in your project directory.
extern void lpt_isr(void);
*/

#endif  /* ISR_H */

/* End of "isr.h" */
