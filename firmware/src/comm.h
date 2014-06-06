/* 
 * File:   comm.h
 * Author: ajo
 *
 * Created on 12 de junio de 2012, 17:01
 */

#ifndef COMM_H
#define	COMM_H
#include "uart1.h"
#include "dsp.h"
#ifdef	__cplusplus
extern "C" {
#endif


    void comm_task();
    int key_is_value(unsigned char k);
    fractional key_to_value(unsigned char k);

#define KEY_V_DIV 127
#define KEY_V_DIV_FLOAT 127.0


#ifdef	__cplusplus
}
#endif

#endif	/* COMM_H */

