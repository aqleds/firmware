
#include "p33FJ16GS502.h"
#include "dsp.h"

void pll_init()
{
    /* Configure Oscillator to operate the device at 40Mhz
       Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
       Fosc= 7.37*(43)/(2*2)=80Mhz for Fosc, Fcy = 40Mhz */

    PLLFBD = 41; /* M = PLLFBD + 2 */
    CLKDIVbits.PLLPOST = 0; /* N1 = 2 */
    CLKDIVbits.PLLPRE = 0; /* N2 = 2 */

    __builtin_write_OSCCONH(0x01); /* New Oscillator selection FRC w/ PLL */
    __builtin_write_OSCCONL(0x01); /* Enable Switch */

    while (OSCCONbits.COSC != 0b001); /* Wait for Oscillator to switch to FRC w/ PLL */
    while (OSCCONbits.LOCK != 1); /* Wait for Pll to Lock */
}

