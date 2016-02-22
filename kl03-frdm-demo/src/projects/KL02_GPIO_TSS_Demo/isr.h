/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef ISR_H
#define ISR_H


/* Example */
/*
#undef  VECTOR_101
#define VECTOR_101 lpt_isr

// ISR(s) are defined in your project directory.
extern void lpt_isr(void);
*/

extern void TSS_HWTimerIsr(void);

 #undef  VECTOR_034
 #define VECTOR_034 TSS_HWTimerIsr


#endif  /* ISR_H */

/* End of "isr.h" */
