
#include "common.h"


void i2c_set_tx_mode(I2C_MemMapPtr p)
{
    p->C1 |= I2C_C1_TX_MASK;

}
void i2c_set_rx_mode(I2C_MemMapPtr p)
{
    p->C1 &= ~I2C_C1_TX_MASK;
}

void i2c_set_slave_mode(I2C_MemMapPtr p)
{
    p->C1  &= ~I2C_C1_MST_MASK;
}
void i2c_set_master_mode(I2C_MemMapPtr p)
{
    p->C1  |=  I2C_C1_MST_MASK;
}

// i2c general

void i2c_give_nack(I2C_MemMapPtr p)
{
    p->C1 |= I2C_C1_TXAK_MASK;
}
void i2c_give_ack(I2C_MemMapPtr p)
{
    p->C1 &= ~I2C_C1_TXAK_MASK;
}
void i2c_repeated_start(I2C_MemMapPtr p)
{
    p->C1     |= 0x04;
}
void i2c_write_byte(I2C_MemMapPtr p, uint8 data)
{
    p->D = data;
}
uint8 i2c_read_byte(I2C_MemMapPtr p)
{
    return p->D;
}
void i2c_start(I2C_MemMapPtr p)
{
    i2c_set_master_mode(p);
    i2c_set_tx_mode(p);
}
void i2c_stop(I2C_MemMapPtr p)
{
    i2c_set_slave_mode(p);
    i2c_set_rx_mode(p);
}
void i2c_wait(I2C_MemMapPtr p)
{
    // wait flag
    while((p->S & I2C_S_IICIF_MASK)==0)
        ;
    // clear flag
    p->S |= I2C_S_IICIF_MASK;
}
uint16 i2c_get_ack(I2C_MemMapPtr p)
{
    if((p->S & I2C_S_RXAK_MASK) == 0)
        return TRUE;
    else
        return FALSE;
}

// -------------------------------------------------
void hal_i2c_init(I2C_MemMapPtr p)
{
   // SIM_SCGC4 |= SIM_SCGC4_I2C1_MASK;

    SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
    
    // configure GPIO for I2C function
    PORTB_PCR3 = PORT_PCR_MUX(2);
    PORTB_PCR4 = PORT_PCR_MUX(2);

 
    I2C0_F  = (I2C_F_MULT(0x01) | I2C_F_ICR(0x2D));      /* set MULT and ICR */
    I2C0_C1 = I2C_C1_IICEN_MASK;       /* enable IIC */
}

void hal_i2c_deinit(I2C_MemMapPtr p)
{
    p->C1 = 0x00;
    
    SIM_SCGC4 &= ~SIM_SCGC4_I2C0_MASK;
}



