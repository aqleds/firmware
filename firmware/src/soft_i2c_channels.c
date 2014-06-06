#include <p33FJ16GS502.h>
#include "soft_i2c_channels.h"

#define I2C2_SDA_TRIS (TRISBbits.TRISB5)
#define I2C2_SDA_O   (LATBbits.LATB5)
#define I2C2_SDA_I   (PORTBbits.RB5)

#define I2C2_SCL_TRIS (TRISBbits.TRISB15)
#define I2C2_SCL_O   (LATBbits.LATB15)
#define I2C2_SCL_I   (PORTBbits.RB15)








void i2c_delay_soft()
{

    int i;
    for (i=0;i<40;i++)
    {
    asm("NOP");asm("NOP");asm("NOP");
    asm("NOP");asm("NOP");asm("NOP");
    }

}

/*****************************************************************************
 *
 *			I2C1 BUS "I2C_SDA/I2C_SCL in schematic"
 *
 *****************************************************************************/
void i2c_clock_cycle()
{

		i2c_delay_soft();
		I2C2_SCL_TRIS = 1;	/* rise clock */
		i2c_delay_soft();
		I2C2_SCL_TRIS = 0;	/* fall clock */
}


void i2c_init()
{
	I2C2_SCL_O = 0;
	I2C2_SCL_TRIS = 1;  /* tristate clock */
	I2C2_SDA_O = 0;
	I2C2_SDA_TRIS = 1; /* tristate data -> HIGH */

        
}



void i2c_start()
{
        I2C2_SDA_O=0;
	I2C2_SDA_TRIS = 0;
	i2c_delay_soft();
	I2C2_SCL_TRIS = 0;
	i2c_delay_soft();
}


unsigned char i2c_write(unsigned char data)
{
    I2C2_SDA_O=0;
	unsigned char i;
	for (i=8;i;i--)
	{
		if (data&0x80) I2C2_SDA_TRIS = 1;
		else	       I2C2_SDA_TRIS = 0;
		
		i2c_clock_cycle();

		data = data << 1;
		
	}

	I2C2_SDA_TRIS = 1; /* float clock to read ACK */
	i2c_delay_soft();
	i = !(I2C2_SDA_I);
	i2c_clock_cycle();

	return i; // ACK
}

unsigned char i2c_read(unsigned char do_ack)
{

	unsigned char i,data=0;

	I2C2_SDA_TRIS = 1; /* float data to read data */

	for (i=8;i;i--)
	{
                i2c_delay_soft();
		data = data << 1;
		if (I2C2_SDA_I) data |= 1;
		i2c_clock_cycle();
	}

	i2c_delay_soft();
	
	if (do_ack) 	I2C2_SDA_TRIS = 0; /* do ack */
	
	i2c_clock_cycle();
	
	I2C2_SDA_TRIS = 1; 
	return data; // ACK

}


void i2c_stop()
{
        I2C2_SDA_O=0;
        i2c_delay_soft();
	I2C2_SDA_TRIS = 0;
	i2c_delay_soft();
	I2C2_SCL_TRIS = 1;
	i2c_delay_soft();
	I2C2_SDA_TRIS = 1;
        i2c_delay_soft();
        i2c_delay_soft();
}

