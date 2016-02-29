/*
 * File:		sample_with_bootloader.c
 * Purpose:		Main process
 *
 */

#include "common.h"

/********************************************************************/
int main (void)
{
	char ch;
        
        printf("\nHello World.\n\n");
        
  	printf("\nRunning the sample_with_bootloader project.\n");

	while(1)
	{
		ch = in_char();
		out_char(ch);
	} 
}
/********************************************************************/