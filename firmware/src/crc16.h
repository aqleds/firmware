/* 
 * File:   crc16.h
 * Author: ajo
 *
 * Created on August 13, 2013, 8:41 AM
 */

#ifndef CRC16_H
#define	CRC16_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

uint16_t crc16_calc ( unsigned char *data, int len );


#ifdef	__cplusplus
}
#endif

#endif	/* CRC16_H */

