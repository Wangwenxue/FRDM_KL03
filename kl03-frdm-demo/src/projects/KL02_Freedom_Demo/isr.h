/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef ISR_H
#define ISR_H

#include "TSS_API.h"
#include "app_init.h"
#include "main.h"

#undef  VECTOR_044
#define VECTOR_044  LPTMR_ISR

#undef  VECTOR_033
#define VECTOR_033 TSS_HWTimerIsr

extern void LPTMR_ISR(void);
extern void TSS_HWTimerIsr(void);

/* Example */
/*
#undef  VECTOR_101
#define VECTOR_101 lpt_isr

// ISR(s) are defined in your project directory.
extern void lpt_isr(void);
*/

#endif  /* ISR_H */

/* End of "isr.h" */
