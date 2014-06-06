#include "p33FJ16GS502.h"
#include "dsp.h"

tPID Buck1VoltagePID;
tPID Buck2VoltagePID;
tPID Buck3VoltagePID;

fractional Buck1Current;
fractional Buck2Current;
fractional Buck3Current;

fractional Buck1VoltageABC[3] __attribute__((section(".xbss, bss, xmemory")));
fractional Buck1VoltageHistory[3] __attribute__((section(".ybss, bss, ymemory")));

fractional Buck2VoltageABC[3] __attribute__((section(".xbss, bss, xmemory")));
fractional Buck2VoltageHistory[3] __attribute__((section(".ybss, bss, ymemory")));

fractional Buck3VoltageABC[3] __attribute__((section(".xbss, bss, xmemory")));
fractional Buck3VoltageHistory[3] __attribute__((section(".ybss, bss, ymemory")));



#define KP 0.23
#define KI 0.0016
#define KD 0.0032



#define PID_BUCK1_KP KP
#define PID_BUCK1_KI KI
#define PID_BUCK1_KD KD

/* Buck2 Voltage control */
#define PID_BUCK2_KP KP
#define PID_BUCK2_KI KI
#define PID_BUCK2_KD KD

/* Buck3 Voltage control */
#define PID_BUCK3_KP KP
#define PID_BUCK3_KI KI
#define PID_BUCK3_KD KD

#define PID_BUCK1_A Q15(PID_BUCK1_KP + PID_BUCK1_KI + PID_BUCK1_KD)
#define PID_BUCK1_B Q15(-1 *(PID_BUCK1_KP + 2 * PID_BUCK1_KD))
#define PID_BUCK1_C Q15(PID_BUCK1_KD)

#define PID_BUCK2_A Q15(PID_BUCK2_KP + PID_BUCK2_KI + PID_BUCK2_KD)
#define PID_BUCK2_B Q15(-1 *(PID_BUCK2_KP + 2 * PID_BUCK2_KD))
#define PID_BUCK2_C Q15(PID_BUCK2_KD)

#define PID_BUCK3_A Q15(PID_BUCK3_KP + PID_BUCK3_KI + PID_BUCK3_KD)
#define PID_BUCK3_B Q15(-1 *(PID_BUCK3_KP + 2 * PID_BUCK3_KD))
#define PID_BUCK3_C Q15(PID_BUCK3_KD)

#define PID_BUCK1_VOLTAGE_REFERENCE 0
#define PID_BUCK2_VOLTAGE_REFERENCE 0
#define PID_BUCK3_VOLTAGE_REFERENCE 0

#define PID_BUCK1_VOLTAGE_MIN 	      2304  /* Minimum duty cycle is total dead time (72) left shifted by 5 bits*/
#define PID_BUCK2_VOLTAGE_MIN 	      2304  /* Minimum duty cycle is total dead time (72) left shifted by 5 bits*/
#define PID_BUCK3_VOLTAGE_MIN 	      2304  /* Minimum duty cycle is total dead time (72) left shifted by 5 bits*/

void buck1_setup(void)
{

    IOCON1bits.PENH = 1; /* PWM1H is controlled by PWM module */
    IOCON1bits.PENL = 1; /* PWM1L is controlled by PWM module */

    IOCON1bits.PMOD = 0; /* Complementary Mode */

    IOCON1bits.POLH = 0; /* Drive signals are active-high */
    IOCON1bits.POLL = 0; /* Drive signals are active-high */

    IOCON1bits.OVRENH = 0; /* Disable Override feature for shutdown PWM */
    IOCON1bits.OVRENL = 0; /* Disable Override feature for shutdown PWM */
    IOCON1bits.OVRDAT = 0b00; /* Shut down PWM with Over ride 0 on PWMH and PWML */

    PWMCON1bits.DTC = 0; /* Positive Deadtime enabled */

    DTR1 = 0x18; /* DTR = (25ns / 1.04ns), where desired dead time is 25ns.
										     Mask upper two bits since DTR<13:0> */
    ALTDTR1 = 0x30; /* ALTDTR = (50ns / 1.04ns), where desired dead time is 50ns.
										     Mask upper two bits since ALTDTR<13:0> */

    PWMCON1bits.IUE = 0; /* Disable Immediate duty cycle updates */
    PWMCON1bits.ITB = 0; /* Select Primary Timebase mode */

    FCLCON1bits.FLTMOD = 3; /* Fault Disabled */

    TRGCON1bits.TRGDIV = 2; /* Trigger interrupt generated once every 3 PWM cycle */
    TRGCON1bits.TRGSTRT = 0; /* Trigger generated after waiting 0 PWM cycles */

    PDC1 = 72; /* Initial pulse-width = minimum deadtime required (DTR1 + ALDTR1)*/
    TRIG1 = 1200; /* Trigger generated at beginning of PWM active period */


}

void buck2_setup(void) {

    /* Buck2 converter setup to output 3.3V */

    IOCON2bits.PENH = 1; /* PWM2H is controlled by PWM module */
    IOCON2bits.PENL = 1; /* PWM2L is controlled by PWM module */

    IOCON2bits.PMOD = 0; /* Complementary Mode */

    IOCON2bits.POLH = 0; /* Drive signals are active-high */
    IOCON2bits.POLL = 0; /* Drive signals are active-high */

    IOCON2bits.OVRENH = 0; /* Disable Override feature for shutdown PWM */
    IOCON2bits.OVRENL = 0; /* Disable Override feature for shutdown PWM */
    IOCON2bits.OVRDAT = 0b00; /* Shut down PWM with Over ride 0 on PWMH and PWML */


    PWMCON2bits.DTC = 0; /* Positive Deadtime enabled */

    DTR2 = 0x18; /* DTR = (25ns / 1.04ns), where desired dead time is 25ns.
									     Mask upper two bits since DTR<13:0> */
    ALTDTR2 = 0x30; /* ALTDTR = (50ns / 1.04ns), where desired dead time is 50ns.
									     Mask upper two bits since ALTDTR<13:0> */

    PWMCON2bits.IUE = 0; /* Disable Immediate duty cycle updates */
    PWMCON2bits.ITB = 0; /* Select Primary Timebase mode */

    FCLCON2bits.FLTMOD = 3; /* Fault Disabled */

    TRGCON2bits.TRGDIV = 2; /* Trigger interrupt generated every 3 PWM cycle*/
    TRGCON2bits.TRGSTRT = 1; /* Trigger generated after waiting 1 PWM cycles */

    PDC2 = 72; /* Initial pulse-width = minimum deadtime required (DTR2 + ALDTR2)*/
    TRIG2 = 1200; /* Trigger generated at beginning of PWM active period */
}

void buck3_setup(void) {
    /* Buck1 converter setup to output 5V */

    IOCON3bits.PENH = 1; /* PWM1H is controlled by PWM module */
    IOCON3bits.PENL = 1; /* PWM1L is controlled by PWM module */

    IOCON3bits.PMOD = 0; /* Complementary Mode */

    IOCON3bits.POLH = 0; /* Drive signals are active-high */
    IOCON3bits.POLL = 0; /* Drive signals are active-high */

    IOCON3bits.OVRENH = 0; /* Disable Override feature for shutdown PWM */
    IOCON3bits.OVRENL = 0; /* Disable Override feature for shutdown PWM */
    IOCON3bits.OVRDAT = 0b00; /* Shut down PWM with Over ride 0 on PWMH and PWML */

    PWMCON3bits.DTC = 0; /* Positive Deadtime enabled */

    DTR3 = 0x18; /* DTR = (25ns / 1.04ns), where desired dead time is 25ns.
										     Mask upper two bits since DTR<13:0> */
    ALTDTR3 = 0x30; /* ALTDTR = (50ns / 1.04ns), where desired dead time is 50ns.
										     Mask upper two bits since ALTDTR<13:0> */

    PWMCON3bits.IUE = 0; /* Disable Immediate duty cycle updates */
    PWMCON3bits.ITB = 0; /* Select Primary Timebase mode */

    FCLCON3bits.FLTMOD = 3; /* Fault Disabled */

    TRGCON3bits.TRGDIV = 2; /* Trigger interrupt generated once every 3 PWM cycle */
    TRGCON3bits.TRGSTRT = 0; /* Trigger generated after waiting 0 PWM cycles */

    PDC3 = 72; /* Initial pulse-width = minimum deadtime required (DTR1 + ALDTR1)*/
    TRIG3 = 1200; /* Trigger generated at beginning of PWM active period */


}

void bucks_adc_init()
{
    /* Now setup the ADC and PWM clock for 120MHz
       ((FRC * 16) / APSTSCLR ) = (7.37MHz * 16) / 1 = 117.9MHz*/

    ACLKCONbits.FRCSEL = 1; /* FRC provides input for Auxiliary PLL (x16) */
    ACLKCONbits.SELACLK = 1; /* Auxiliary Ocillator provides clock source for PWM & ADC */
    ACLKCONbits.APSTSCLR = 7; /* Divide Auxiliary clock by 1 */
    ACLKCONbits.ENAPLL = 1; /* Enable Auxiliary PLL */

    while (ACLKCONbits.APLLCK != 1); /* Wait for Auxiliary PLL to Lock */

    /* current and voltage measurements */
    // PAIR1 : AN0 = BUCK1_VREF, AN1 = BUCK1_IREF
    // PAIR2 : AN2 = BUCK2_VREF, AN3 = BUCK2_IREF
    // PAIR3 : AN4 = BUCK3_VREF, AN5 = BUCK3_IREF

    ADCONbits.FORM = 0; /* Integer data format */
    ADCONbits.EIE = 0; /* Early Interrupt disabled */
    ADCONbits.ORDER = 0; /* Convert even channel first */
    ADCONbits.SEQSAMP = 0; /* Select simultaneous sampling */
    ADCONbits.ADCS = 5; /* ADC clock = FADC/6 = 120MHz / 6 = 20MHz,
  			12*Tad = 1.6 MSPS, two SARs = 3.2 MSPS */

    IFS6bits.ADCP0IF = 0; /* Clear ADC interrupt flag */
    IPC27bits.ADCP0IP = 5; /* Set ADC interrupt priority */
    IEC6bits.ADCP0IE = 1; /* Enable the ADC Pair 0 interrupt */

    IFS6bits.ADCP1IF = 0; /* Clear ADC interrupt flag */
    IPC27bits.ADCP1IP = 5; /* Set ADC interrupt priority */
    IEC6bits.ADCP1IE = 1; /* Enable the ADC Pair 1 interrupt */

    IFS7bits.ADCP2IF = 0; /* Clear ADC interrupt flag */
    IPC28bits.ADCP2IP = 5; /* Set ADC interrupt priority */
    IEC7bits.ADCP2IE = 1; /* Enable the ADC Pair 2 interrupt */


    ADPCFGbits.PCFG0 = 0;
    ADPCFGbits.PCFG1 = 0;

    ADPCFGbits.PCFG2 = 0;
    ADPCFGbits.PCFG3 = 0;

    ADPCFGbits.PCFG4 = 0;
    ADPCFGbits.PCFG5 = 0;

   

    ADSTATbits.P0RDY = 0; /* Clear Pair 0 data ready bit */
    ADCPC0bits.IRQEN0 = 1; /* Enable ADC Interrupt for Buck 1 control loop */
    ADCPC0bits.TRGSRC0 = 4; /* ADC Pair 0 triggered by PWM1 */

    ADSTATbits.P1RDY = 0; /* Clear Pair 1 data ready bit */
    ADCPC0bits.IRQEN1 = 1; /* Enable ADC Interrupt for Buck 2 control loop */
    ADCPC0bits.TRGSRC1 = 5; /* ADC Pair 1 triggered by PWM2 */

    ADSTATbits.P2RDY = 0; /* Clear Pair 2 data ready bit */
    ADCPC1bits.IRQEN2 = 0; /* Disable ADC Interrupt for input voltage measurment */
    ADCPC1bits.TRGSRC2 = 6; /* ADC Pair 2 triggered by PWM3 */


}



void buck1_pid_init(void) {
    Buck1VoltagePID.abcCoefficients = Buck1VoltageABC; /* Set up pointer to derived coefficients */
    Buck1VoltagePID.controlHistory = Buck1VoltageHistory; /* Set up pointer to controller history samples */

    PIDInit(&Buck1VoltagePID);

    if ((PID_BUCK1_A == 0x7FFF || PID_BUCK1_A == 0x8000) ||
        (PID_BUCK1_B == 0x7FFF || PID_BUCK1_B == 0x8000) ||
        (PID_BUCK1_C == 0x7FFF || PID_BUCK1_C == 0x8000)) {
        while (1); /* This is a check for PID Coefficients being saturated */
    }

    Buck1VoltagePID.abcCoefficients[0] = PID_BUCK1_A; /* Load calculated coefficients */
    Buck1VoltagePID.abcCoefficients[1] = PID_BUCK1_B;
    Buck1VoltagePID.abcCoefficients[2] = PID_BUCK1_C;

    Buck1VoltagePID.controlReference = PID_BUCK1_VOLTAGE_MIN;

    Buck1VoltagePID.measuredOutput = 0;

}

void buck2_pid_init(void) {
    Buck2VoltagePID.abcCoefficients = Buck2VoltageABC; /* Set up pointer to derived coefficients */
    Buck2VoltagePID.controlHistory = Buck2VoltageHistory; /* Set up pointer to controller history samples */

    PIDInit(&Buck2VoltagePID);

    if ((PID_BUCK2_A == 0x7FFF || PID_BUCK2_A == 0x8000) ||
            (PID_BUCK2_B == 0x7FFF || PID_BUCK2_B == 0x8000) ||
            (PID_BUCK2_C == 0x7FFF || PID_BUCK2_C == 0x8000)) {
        while (1); /* This is a check for PID Coefficients being saturated */
    }

    Buck2VoltagePID.abcCoefficients[0] = PID_BUCK2_A; /* Load calculated coefficients */
    Buck2VoltagePID.abcCoefficients[1] = PID_BUCK2_B;
    Buck2VoltagePID.abcCoefficients[2] = PID_BUCK2_C;

    Buck2VoltagePID.controlReference = PID_BUCK2_VOLTAGE_MIN;

    Buck2VoltagePID.measuredOutput = 0;

}

void buck3_pid_init(void) {
    Buck3VoltagePID.abcCoefficients = Buck3VoltageABC; /* Set up pointer to derived coefficients */
    Buck3VoltagePID.controlHistory = Buck3VoltageHistory; /* Set up pointer to controller history samples */

    PIDInit(&Buck3VoltagePID);

    if ((PID_BUCK3_A == 0x7FFF || PID_BUCK3_A == 0x8000) ||
            (PID_BUCK3_B == 0x7FFF || PID_BUCK3_B == 0x8000) ||
            (PID_BUCK3_C == 0x7FFF || PID_BUCK3_C == 0x8000)) {
        while (1); /* This is a check for PID Coefficients being saturated */
    }

    Buck3VoltagePID.abcCoefficients[0] = PID_BUCK3_A; /* Load calculated coefficients */
    Buck3VoltagePID.abcCoefficients[1] = PID_BUCK3_B;
    Buck3VoltagePID.abcCoefficients[2] = PID_BUCK3_C;

    Buck3VoltagePID.measuredOutput = 0;

}

/* PTPER = ((1 / 400kHz) / 1.04ns) = 2404, 
 * where 400kHz is the desired switching 
 * frequency and 1.04ns is PWM resolution. */

#define CONST_PWM 961538461L   // 1/1.04ns
#define PWM_HZ 400000L

void bucks_init()
{
    PTPER = (CONST_PWM/PWM_HZ); 
    bucks_adc_init();
    buck1_setup();
    buck2_setup();
    buck3_setup();
    buck1_pid_init();
    buck2_pid_init();
    buck3_pid_init();
}


#define d_R2 10000
#define d_R1 2400

void bucks_set_control_ref_native(int buck_ch, fractional n_v)
{
    switch(buck_ch)
    {
        case 0: Buck1VoltagePID.controlReference = n_v; break;
        case 1: Buck2VoltagePID.controlReference = n_v; break;
        case 2: Buck3VoltagePID.controlReference = n_v; break;
        default: break;
    }
}

void buck_enabled(int buck_ch,int enabled)
{
    switch (buck_ch)
    {
        case 0: IOCON1bits.OVRENH = !enabled;
                IOCON1bits.OVRENL = !enabled;
                break;

        case 1: IOCON2bits.OVRENH = !enabled;
                IOCON2bits.OVRENL = !enabled;
                break;

        case 2: IOCON3bits.OVRENH = !enabled;
                IOCON3bits.OVRENL = !enabled;
                break;
    }
}

void buck_set_control_voltage(int buck_ch, float voltage)
{
    fractional n_v;
    float val;

    // we have R1+R2 voltage divider, so calculate the reference in volts
    // using this voltage divider
    val = ((float)d_R1 * voltage / (float)(d_R1+d_R2));

    // now just consider that 3.3V reference is at 1023 ADC value
    val = val* (1023.0  / 3.3);

    // convert our reference into Q15 format
    n_v = (val * 32.0);

    bucks_set_control_ref_native(buck_ch,n_v);
}

float buck_get_output_voltage(int buck_ch)
{
    fractional v=0;
    float f;

    switch(buck_ch)
    {
        case 0: v = Buck1VoltagePID.measuredOutput; break;
        case 1: v = Buck2VoltagePID.measuredOutput; break;
        case 2: v = Buck3VoltagePID.measuredOutput; break;

    }

    f = v;
    f = (f / 32.0)*(3.3/1023.0)*(float)(d_R1+d_R2)/ (float)(d_R1);

    return f;
}

float buck_get_output_current(int buck_ch)
{
    fractional v=0;
    float f;

    switch(buck_ch)
    {
        case 0: v = Buck1Current; break;
        case 1: v = Buck2Current; break;
        case 2: v = Buck3Current; break;

    }

    f = v*(3.3/1023.0)*20.0; // this is voltage * 1/0.05
    
    return f;
}