#include "p33FJ16GS502.h"
#include "protocol.h"


void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;    // Interrupt flag reset
    IEC0bits.U1TXIE = 0;    // interrupt enable reset
}


void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void)
{
    unsigned char rcv_char;

     /* must clear the overrun error to keep uart receiving */
    if(U1STAbits.OERR == 1)
    {
        U1STAbits.OERR = 0;
    } else if(U1STAbits.FERR == 1)
    {
        rcv_char = U1RXREG;
        return;
    } else if(U1STAbits.URXDA == 1)
    {
        rcv_char = U1RXREG;
        protocol_rcv_char(rcv_char);
    }
  

    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;
}


void uart1_init(int BAUDRATEREG1)
{
   U1MODEbits.STSEL = 0; // 1 Stop bit
   U1MODEbits.PDSEL = 0; // No Parity, 8 data bits
   U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
   U1MODEbits.BRGH = 0; // Low Speed mode
   U1MODEbits.LPBACK=0;


   U1BRG = BAUDRATEREG1;	//set baud speed

  U1STAbits.UTXISEL0 = 0; // Interrupt after one TX Character is transmitted
  U1STAbits.UTXISEL1 = 0;
  U1STAbits.URXISEL = 1; // Interrupt after one RX character is received;

  IFS0bits.U1RXIF = 0;    // Clear the Recieve Interrupt Flag
  IEC0bits.U1RXIE = 1; // Enable Uart RX Interrupt
  IEC0bits.U1TXIE = 0; // Disable Uart TX Interrupt


  U1MODEbits.UARTEN = 1; // Enable UART
  U1STAbits.UTXEN = 1; // Enable UART TX



}

//UART transmit function, parameter Ch is the character to send
void uart1_putchar(unsigned char ch)
{
   //transmit ONLY if TX buffer is empty
   while(U1STAbits.UTXBF == 1);
   U1TXREG = ch;
}

void uart1_write(char*buffer, int len)
{
    while(len--) uart1_putchar(*buffer++);
}
