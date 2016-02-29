/*
<<<<<<< HEAD
 * File:	  	platinum.c
 * Purpose:		Demo UART Function
=======
 * File:		platinum.c
 * Purpose:		Main process
>>>>>>> 30d1d1c78d66d5ee57ac2021dbd87b186e5c0235
 *
 */

#include "common.h"

/********************************************************************/
int main (void)
{
	char ch;
        
<<<<<<< HEAD
        printf("\nRunning platinum example project.\n");

	while(1)
	{
          ch = in_char();
	  out_char(ch);
=======
  printf("\nRunning the platinum project.\n");

	while(1)
	{
		ch = in_char();
		out_char(ch);
>>>>>>> 30d1d1c78d66d5ee57ac2021dbd87b186e5c0235
	} 
}
/********************************************************************/