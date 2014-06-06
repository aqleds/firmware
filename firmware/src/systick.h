#ifndef __SYSTICKS_H
#define __SYSTICKS_H
void systicks_init();
unsigned long systicks_get_ms();
void delay_ms(int ms);

#endif
