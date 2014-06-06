
#include <p33FJ16GS502.h>
#include <pps.h>
#include "systick.h"
#include "bucks.h"
#include "fuses.h"
#include <string.h>
#include <math.h>
#include "uart1.h"
#include "led_channels.h"
#include "comm.h"
#include "config.h"
#include "soft_i2c_channels.h"
#include "rtc_mcp79410.h"
#include "planner.h"
#include "comm_task.h"

/*
int INPUTUNDERVOLTAGE=0;
int INPUTOVERVOLTAGE=0;
 */
#include "led_channels.h"
#include "time_task.h"

int InputVoltage;
int InputButtons;

/*
void task_input_voltage()
{
    int disable_ch0 = is_led_ch_disabled(0);
    int disable_ch1 = is_led_ch_disabled(1);
    int disable_ch2 = is_led_ch_disabled(2);

    if ((InputVoltage <= INPUTUNDERVOLTAGE) || (InputVoltage >= INPUTOVERVOLTAGE))
    {
         disable_ch0=1;
         disable_ch1=1;
         disable_ch2=1;
    }


    IOCON1bits.OVRENH = disable_ch0;
    IOCON1bits.OVRENL = disable_ch0;

    IOCON2bits.OVRENH = disable_ch1;
    IOCON2bits.OVRENL = disable_ch1;

    IOCON3bits.OVRENH = disable_ch2;
    IOCON3bits.OVRENL = disable_ch2;

}

 */
void __attribute__((__interrupt__, no_auto_psv)) _ADCP3Interrupt() {

    InputVoltage = ADCBUF6;
    InputButtons = ADCBUF7;

    ADSTATbits.P3RDY = 0; /* Clear Pair 3 RDY flag*/
    IFS7bits.ADCP3IF = 0; /* Clear Interrupt Flag */

}

void adc_p3_setup() /* pair 3 for VIN + BUTTON */ {
    IFS7bits.ADCP3IF = 0; /* Clear ADC interrupt flag */
    IPC28bits.ADCP3IP = 6; /* Set ADC interrupt priority */
    IEC7bits.ADCP3IE = 1; /* Enable the ADC Pair 3 interrupt */

    ADSTATbits.P3RDY = 0; /* Clear Pair 3 data ready bit */
    ADCPC1bits.IRQEN3 = 1; /* Enable ADC Interrupt for Buck3 control loop */
    ADCPC1bits.TRGSRC3 = 4; /* ADC Pair 3 triggered by PWM1 */

    ADPCFGbits.PCFG6 = 0;
    ADPCFGbits.PCFG7 = 1;
}

#define UART1_BPS 115200L

void uart_rxtx_init() {

#ifdef RX_ON_RP2
    /* disable RP2 as Analog input */
    ADPCFGbits.PCFG7 = 1;
    /* set Uart1 RX from RP2 */
    RPINR18bits.U1RXR = 2; /* UART_RX = RP2 */
    /* set pin as input */
    TRISB |= 1 << 2;
#else 
          /* disable RP2 as Analog input */
    ADPCFGbits.PCFG7 = 1;
    /* set Uart1 RX from RP2 */
    RPINR18bits.U1RXR = 4; /* UART_RX = RP4 */
    /* set pin as input */
    TRISB |= 1 << 4;
#endif
    
    //PPSInput(PPS_U1RX, PPS_RP4); /* U1RX on RP4 */
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP4;

    RPOR1bits.RP3R=0b00011;/* UART TX = RP3 */
    // RPOR1bits.RP2R=0b00011;/* UART TX = RP2 */
    TRISB &= ~(1<<3);



    uart1_init(40000000L / (16L * UART1_BPS) - 1);

}

float get_input_voltage() {

    float f;
    f = InputVoltage;
    f = f * (3.3 / 1023.0)*(float) (10000 + 2400) / (float) (2400);

    return f;

}


unsigned long ms;


void pll_init();

void init(void)
{
    pll_init();
    systicks_init();

    bucks_init(); /* buck setups the PWM and ADC p0 p1 p2 pairs */
    adc_p3_setup(); /* pair 3 used for */
    uart_rxtx_init();

    ADCONbits.ADON = 1; /* Enable the ADC */
    PTCONbits.PTEN = 1; /* Enable the PWM */

    i2c_init();
    config_load();
    led_load_profiles();
    rtc_init();
    planner_init();
    comm_init();
    


}

int main(void) {
 

    int cycle=0;
    init();

    delay_ms(500);
   

    while (1) {
        ms = systicks_get_ms();
        if (ms % 5) continue;

        switch (cycle) {
            case 0: time_update();break;
            case 1: led_task();     break;
            case 2: comm_task();    break;
            case 3: planner_task(); break;
        }
        cycle++;
        if (cycle > 3) cycle = 0;

    }

}
