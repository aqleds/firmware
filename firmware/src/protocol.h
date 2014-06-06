/* 
 * File:   protocol.h
 * Author: ajo
 *
 * Created on August 13, 2013, 8:21 PM
 */

#ifndef PROTOCOL_H
#define	PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "proto.h"

// STX CMD OP DATA[1..N] CRCHI CRCLO ETX

// defines ///////////////////////////////////////////////////////////

#define PROTOCOL_MAX_BUFFER 64

#define OP_READ 'r'
#define OP_READ_RT 'R'
#define OP_WRITE 'w'

#define CMD_PING                'p'
#define CMD_RTC                 't'
#define CMD_TIMETABLE           'T'
#define CMD_CHANNEL_CURRENT_AND_VOLTS     'i'
#define CMD_CHANNEL_LAMP        'l'
#define CMD_CHANNEL_RAMP        'r'
#define CMD_TURBO_MODE          '!'
#define CMD_INPUT_VOLTS         'v'
#define CMD_CHANNEL_OVERRIDE    's'
#define CMD_SAVE_CONFIG         'S'
#define CMD_RELOAD_CONFIG       'R'

#define ERR_BAD_LEN -1
#define ERR_BAD_CRC -2

// public functions ///////////////////////////////////////////////////

void protocol_init();
int protocol_get_buffer(char *dst, int max_len);
void protocol_send_buffer(unsigned char *src, int len);
void protocol_send_reset();


// protected functions //////////////////////////////////////////

void protocol_rcv_char(char ch);


#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_H */

