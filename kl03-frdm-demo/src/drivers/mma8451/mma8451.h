
#define MMA8451_I2C_ADDRESS (0x1d<<1)
#define I2C0_B  I2C0_BASE_PTR

#define MAG8451_DEVICE_ID			0x1A

#define MAG8451_STATUS		  0x00
#define MAG8451_OUT_X_MSB		0x01
#define MAG8451_OUT_X_LSB		0x02
#define MAG8451_OUT_Y_MSB		0x03
#define MAG8451_OUT_Y_LSB		0x04
#define MAG8451_OUT_Z_MSB		0x05
#define MAG8451_OUT_Z_LSB		0x06
#define MAG8451_F_SETUP		  0x09
#define MAG8451_TRIG_CFG    0x0A
#define MAG8451_SYSMOD      0x0B
#define MAG8451_INT_SOURCE  0x0C
#define MAG8451_WHO_AM_I    0x0D




void hal_dev_mma8451_init(void); //prepare I2C port
uint8   hal_dev_mma8451_read_reg(uint8 addr);
void hal_dev_mma8451_write_reg(uint8 addr, uint8 data);
uint16 hal_dev_mma8451_test_reg_rw(void);
void hal_dev_mma8451_test_function(void);

