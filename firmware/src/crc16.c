#include <stdint.h>

unsigned short crc16tab_hi[16] = {
        0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
        0x81, 0x91, 0xA1, 0xB1, 0xC1, 0xD1, 0xE1, 0xF1
};
unsigned short crc16tb_lo[16] = {
        0x00, 0x21, 0x42, 0x63, 0x84, 0xA5, 0xC6, 0xE7,
        0x08, 0x29, 0x4A, 0x6B, 0x8C, 0xAD, 0xCE, 0xEF
};


unsigned char crc16hi;
unsigned char crc16lo;


void crc16_init( void )
{
	crc16hi = 0xFF;
	crc16lo = 0xFF;
}


void crc16_update_4_bits( unsigned char val )
{
	unsigned char	t;

	
	t = crc16hi >> 4;

	t = t ^ val;

	crc16hi = (crc16hi << 4) | (crc16lo >> 4);
	crc16lo = crc16lo << 4;

	crc16hi = crc16hi ^ crc16tab_hi[t];
	crc16lo = crc16lo ^ crc16tb_lo[t];
}


void crc16_update( unsigned char val )
{
	crc16_update_4_bits( val >> 4 );		
	crc16_update_4_bits( val & 0x0F );
}

uint16_t crc16_calc ( unsigned char *data, int len )
{
    uint16_t crc16;
    crc16_init();
    
    while(len--)
        crc16_update(*data++);

    crc16 =  crc16hi;
    crc16 = (crc16 << 8) | crc16lo;
    return crc16;
}

