#include "p33FJ16GS502.h"
#include "systick.h"

unsigned long SysTicks_ms;

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt()
{
  SysTicks_ms++;                /* Incremetn SysTicks counter for timing */
  IFS0bits.T1IF = 0; 		/* Clear Interrupt Flag */
}

void systicks_init()
{
    SysTicks_ms = 0;
    PR1 = 0x9C40;	/* (1ms / 25ns) = 40,000 = 0x9C40 */
    IPC0bits.T1IP = 4;  /* Set Interrupt Priority lower then ADC */
    IEC0bits.T1IE = 1;	/* Enable Timer1 interrupts */
    T1CONbits.TON = 1;	/* Enable Timer1 */
}

unsigned long systicks_get_ms()
{
    unsigned long val;
    IEC0bits.T1IE = 0;	/* Disable Timer1 interrupts */
    val = SysTicks_ms;
    IEC0bits.T1IE = 1;  /* Restore Timer1 interrupts */
    return val;
}

void delay_ms(int ms)
{
    long int start_ms=systicks_get_ms();

    while ((systicks_get_ms()-start_ms) < ms);
}