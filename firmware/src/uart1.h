#ifndef __UART1_H
#define __UART1_H
void uart1_init(int BAUDRATEREG1);
void uart1_putchar(unsigned char ch);
int uart1_getchar();
int uart1_kbhit();
#endif