/*
 * File:	  	main.c
 * Purpose:		Demo iic Function
 * File:	  	main.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "iic.h"
#include "mma8451.h"

unsigned short accel_x, accel_y, accel_z;

/********************************************************************/
int main (void)
{
	char ch;
        
  printf("\nRunning iic example project.\n");
	
	hal_dev_mma8451_init();
	
	if(MAG8451_DEVICE_ID == hal_dev_mma8451_read_reg(MAG8451_WHO_AM_I))
		 printf("\nMMA8451 found.\n");
	
	ch=hal_dev_mma8451_read_reg(0x2a);
	ch = ch|0x01;
  hal_dev_mma8451_write_reg(0x2a,ch);
	
	while(1)
	{
	if((hal_dev_mma8451_read_reg(0x00)&0xf) != 0)
    {
        accel_x   = hal_dev_mma8451_read_reg(0x01)<<8;
        accel_x  |= hal_dev_mma8451_read_reg(0x02);
        accel_x >>= 2;

        accel_y   = hal_dev_mma8451_read_reg(0x03)<<8;
        accel_y  |= hal_dev_mma8451_read_reg(0x04);
        accel_y >>= 2;

        accel_z   = hal_dev_mma8451_read_reg(0x05)<<8;
        accel_z  |= hal_dev_mma8451_read_reg(0x06);
        accel_z >>= 2;
		
			printf("accel_x=0x%04x\r\n",accel_x); 
		  printf("accel_y=0x%04x\r\n",accel_y);
	  	printf("accel_z=0x%04x\r\n",accel_z);
			printf("\r\n\r\n");
    }
		else
		{
		  printf("(hal_dev_mma8451_read_reg(0x00)&0xf) == 0");
		}
	}
	

}
/********************************************************************/