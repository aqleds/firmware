/* 
 * File:   proto.h
 * Author: ajo
 *
 * Created on August 13, 2013, 8:53 AM
 */

#ifndef PROTO_H
#define	PROTO_H

#ifdef	__cplusplus
extern "C" {
#endif


#define ESC    0x1b
#define STX    0x02
#define ETX    0x03
#define NAK    0x15
#define RESETC 0x09

#define ENQ    0x05
#define ACK    0x06
#define UNK    0x07

#define PSTX   0x04
#define PETX   0x05
#define PRESET 0x06

#define DC1    0x17
#define DC2    0x18
#define DC3    0x19
#define DC4    0x1A



#ifdef	__cplusplus
}
#endif

#endif	/* PROTO_H */

