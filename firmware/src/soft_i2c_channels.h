
#ifndef __SOFT_I2C_CHANNELS_H
#define __SOFT_I2C_CHANNELS_H
void i2c_delay_soft(void);
void i2c_init(void);
void i2c_start();
unsigned char i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char do_ack);
void i2c_stop();

#endif