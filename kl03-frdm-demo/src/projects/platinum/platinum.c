/*
 * File:	  	platinum.c
 * Purpose:		Demo UART Function
 *
 */

#include "common.h"

/********************************************************************/
int main (void)
{
	char ch;
        
        printf("\nRunning platinum example project.\n");

	while(1)
	{
          ch = in_char();
	  out_char(ch);
	} 
}
/********************************************************************/