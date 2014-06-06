

// STX
// CMD
// DATA[1..N]
// CRCHI
// CRCLO
// ETX


#include "proto.h"
#include "protocol.h"
#include "uart1.h"
#include "crc16.h"
#include <string.h>
typedef enum {
                    ST_WAIT_STX,
                    ST_WAIT_DATA,
                    ST_WAIT_ESC_DATA,
                    ST_STOPPED

                            
} t_protocol_state;

t_protocol_state protocol_state = ST_WAIT_STX;


#define PROTOCOL_MAX_BUFFER 64


unsigned char protocol_buffer[PROTOCOL_MAX_BUFFER];
int protocol_buffer_p=0;

void protocol_buffer_clr();

void protocol_init()
{
    protocol_state = ST_WAIT_STX;
    protocol_buffer_clr();
}

// public functions ///////////////////////////////////////////////////

int protocol_get_buffer(char *dst, int max_len)
{
    uint16_t crc16;
    uint16_t packet_crc16;
    int cpy_len = protocol_buffer_p;
    if (cpy_len>max_len) cpy_len = max_len;

    if (!cpy_len) return 0;

    /*crc takes 2 bytes*/
    if (cpy_len<2) return ERR_BAD_LEN;


    crc16 = crc16_calc (protocol_buffer,cpy_len-2 );

    packet_crc16 = protocol_buffer[cpy_len-2];
    packet_crc16 = packet_crc16<<8;
    packet_crc16 |= protocol_buffer[cpy_len-1];

    /* 0xBEEF is used for DEBUG CRC*/
    if (packet_crc16!=0xBEEF && (packet_crc16!=crc16))
    {
        protocol_buffer_clr();
        return ERR_BAD_CRC;
    }


    memcpy(dst,protocol_buffer,cpy_len-2);

    protocol_buffer_clr();

    return cpy_len-2;
}

void protocol_send_char(unsigned char ch)
{
    switch(ch)
    {
        case STX: uart1_putchar(ESC); uart1_putchar(PSTX); break;
        case ETX: uart1_putchar(ESC); uart1_putchar(PETX); break;
        case RESETC: uart1_putchar(ESC); uart1_putchar(PRESET); break;
        case ESC: uart1_putchar(ESC); uart1_putchar(ESC); break;
        default: uart1_putchar(ch);
    }

}

void protocol_send_buffer(unsigned char *src, int len)
{
    uint16_t crc16;
    
    crc16 = crc16_calc(src,len);

    uart1_putchar(STX);

    while(len--)
        protocol_send_char(*src++);

    protocol_send_char(crc16>>8);
    protocol_send_char(crc16&0xff);

    uart1_putchar(ETX);
}


// private functions //////////////////////////////////////////

void protocol_buffer_clr()
{
    protocol_buffer_p=0;
    protocol_state = ST_WAIT_STX;
}

void protocol_buffer_add(char ch)
{
    if (protocol_buffer_p>=PROTOCOL_MAX_BUFFER) return;
    protocol_buffer[protocol_buffer_p++]=ch;
}





void protocol_rcv_char(char ch)
{

    if (ch==RESETC) {
        asm("RESET");
    }

    if (ch==ETX && protocol_buffer_p>0)
    {
        protocol_state = ST_STOPPED;
        return;
    }

    if (ch==STX) 
    {
        protocol_state = ST_WAIT_DATA;
        protocol_buffer_p=0;
        return;
    }

    switch(protocol_state)
    {
        case ST_STOPPED:
            // we discard new data until main loop fetches buffer
            break;


        case ST_WAIT_DATA:
            if (ch==ESC) {
                protocol_state++;
                break;
            } else {
                protocol_buffer_add(ch);
            }
            break;

        case ST_WAIT_ESC_DATA:
            if (ch==PSTX) protocol_buffer_add(STX);
            else if (ch==PETX) protocol_buffer_add(ETX);
            else if (ch==PRESET) protocol_buffer_add(RESETC);
            else if (ch==ESC) protocol_buffer_add(ESC);
            protocol_state = ST_WAIT_DATA;
            break;

        default: break;

    }
}
