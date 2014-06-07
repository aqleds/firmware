#include <p33FJ16GS502.h>
#include <pps.h>


_FOSCSEL(FNOSC_FRC)
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON)
_FWDT(FWDTEN_OFF)
_FPOR(FPWRT_PWR128) /* BOREN_OFF */
_FICD(ICS_PGD1 & JTAGEN_OFF)

typedef short s16;
typedef unsigned short u16;
typedef long s32;
typedef unsigned long u32;

typedef union tUnionReg32 {
    u32 v32;

    struct {
        u16 low_word;
        u16 high_word;
    } s;

    char v[4];
} r32_union;

extern u32 read_pgm_latch(u16, u16);
extern u32 write_pgm_latch(u16 addrH, u16 addrL, u16 dataH,
                               u16 dataL);
extern u32 write_pgm_memory(u16);
extern u32 erase_pgm_page(u16, u16, u16);

void send_char(char);
char recv_char();
void send_buffer(char *, int);
void read_pgm_mem(char *, r32_union, int);
void write_pgm_mem(char *, r32_union);
extern void enter_user_code();



#define CMD_NACK      0x00
#define CMD_ACK       0x01
#define CMD_READ_PM   0x02
#define CMD_WRITE_PM  0x03
#define CMD_WRITE_PMC 0x0C
#define CMD_WRITE_CM  0x07
#define CMD_RESET     0x08
#define CMD_READ_ID   0x09


#define PM_ROW_SIZE 64 * 8
#define CM_ROW_SIZE 8
#define CONFIG_WORD_SIZE 1
#define PM_ROW_ERASE 		0x4042
#define PM_ROW_WRITE 		0x4001
#define CONFIG_WORD_WRITE	0X4000

#define UART1_BPS 115200
#define FCY 40000000L


char buffer[PM_ROW_SIZE * 3 + 1];
char buffer2[2 * 3 +1];
 

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


void init()
{
    RCONbits.SWDTEN = 0; /* Disable Watch Dog Timer*/
    pll_init();
   
}

/* cleanup pin setups and go out */
void ExitToUserCode()
{

    /* recover RP2 as Analog input */
    ADPCFGbits.PCFG7 = 0;
    /* set Uart1 RX from ?? */
    RPINR18bits.U1RXR = 0; /* UART_RX = RP2 */
    RPOR1bits.RP3R=0b0000;/* UART TX = RP3 */
    TRISB |= (1<<3); /* set pin as output */

    asm("goto 0x200");
}

/* decryption */
typedef unsigned long int  DWORD;
typedef unsigned short      WORD;
extern   DWORD        cbc[2];
void XTEA_Decode(DWORD* data, WORD len);
void XTEA_InitCBC();
#define ENCRYPTION_ON 1
#define ENCRYPTION_OFF 0

char cmd_write_pgm_mem(int decryption)
{
                r32_union src_addr;
                int size;

                src_addr.v[0]==recv_char();
                src_addr.v[1]==recv_char();
                src_addr.v[2]==recv_char();
                src_addr.v[3] = 0;

                for (size = 0; size < PM_ROW_SIZE * 3; size++) {
                    buffer[size] = recv_char();
                }

                if (decryption)
                    XTEA_Decode((DWORD*)buffer,PM_ROW_SIZE*3);

                /* if this is the first sector, then copy the reset vector
                 * that jumps into the bootloader
                 */

                if (src_addr.v32==0x0000)
                {
                    read_pgm_mem(buffer2, src_addr,2);

                    // Keep the jump to the bootloader
                    buffer[0]=buffer2[2];
                    buffer[1]=buffer2[1];
                    buffer[2]=buffer2[0];
                    buffer[3]=buffer2[5];
                    buffer[4]=buffer2[4];
                    buffer[5]=buffer2[3];

                }

                /* don't overwrite ourselves */
                if ((src_addr.v32>=0x2800) && (src_addr.v32<=0x2C00))
                {
                    return CMD_NACK;
                }

                erase_pgm_page(src_addr.s.high_word,
                               src_addr.s.low_word, PM_ROW_ERASE);

                write_pgm_mem(buffer, src_addr); /*program page */

                return CMD_ACK; /*Send Acknowledgement */

}

int main(void) {

    r32_union Delay;

    init();

    T2CONbits.T32 = 1; /* to increment every instruction cycle */
    IFS0bits.T3IF = 0; /* Clear the Timer3 Interrupt Flag */
    IEC0bits.T3IE = 0; /* Disable Timer3 Interrup Service Routine */


    Delay.v32 = ((u32) (FCY)*2);

    PR3 = Delay.s.high_word;
    PR2 = Delay.s.low_word;

    /* Enable Timer */
    T2CONbits.TON = 1;
    
#if 0
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
    RPINR18bits.U1RXR = 4; /* UART_RX = RP2 */
    /* set pin as input */
    TRISB |= 1 << 4;

   // PPSInput(PPS_U1RX, PPS_RP4); /* U1RX on RP4 */
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP4;
#endif

    RPOR1bits.RP3R=0b00011;/* UART TX = RP3 */
    TRISB &= ~(1<<3); /* set pin as output */

    U1BRG = 40000000L / (16L * UART1_BPS) - 1; /*  BAUD Rate Setting of Uart1  (0XEF for 9600)*/
    U1MODE = 0x8000; /* Reset UART to 8-n-1, alt pins, and enable */
    U1STA = 0x0400; /* Reset status register and enable TX */


    while (1) {
        char Command = recv_char();

        switch (Command) {

            case CMD_WRITE_PM: /* tested */
            {
                send_char(cmd_write_pgm_mem(ENCRYPTION_ON));
                break;
            }

            case CMD_WRITE_PMC: /* tested */
            {
                send_char(cmd_write_pgm_mem(ENCRYPTION_OFF));
                break;
            }

            case CMD_READ_ID:
                buffer[0]=0x03;
                buffer[1]=0xC0;
                send_buffer(buffer,2);
                XTEA_InitCBC();
                break;
                
            case CMD_WRITE_CM:
            {
                int size;

                for (size = 0; size < CM_ROW_SIZE * 3;) {
                    buffer[size++] = recv_char();
                    buffer[size++] = recv_char();
                    buffer[size++] = recv_char();
          
                    send_char(CMD_ACK); /*Send Acknowledgement */
                }


                break;
            }
            case CMD_RESET:
            case CMD_NACK:
            {
                ExitToUserCode();
            } 


            default:
                send_char(CMD_NACK);
                break;
        }

    }

}

char recv_char() {
    while (1) {
        /* if timer expired, signal to application to jump to user code */
        if (IFS0bits.T3IF == 1) {
            return CMD_NACK;
        }

        /* must clear the overrun error to keep uart receiving */
        if (U1STAbits.OERR == 1) {
            U1STAbits.OERR = 0;
            continue;
        }

        /* check for receive errors */
        if (U1STAbits.FERR == 1) {
            continue;
        }


        /* get the data */
        if (U1STAbits.URXDA == 1) {
            T2CONbits.TON = 0; /* Disable timer countdown */
            return U1RXREG;
        }
    }
}

void read_pgm_mem(char * ptrData, r32_union src_addr, int len) {
    int size;
    r32_union temp_;

    for (size = 0; size < len; size++) {
        temp_.v32 = read_pgm_latch(src_addr.s.high_word, src_addr.s.low_word);

        *ptrData++ = temp_.v[2];
        *ptrData++ = temp_.v[1];
        *ptrData++ = temp_.v[0];

        src_addr.v32 += 2;
    }
}


void send_buffer(char * ptrData, int size) {
    int DataCount;

    for (DataCount = 0; DataCount < size; DataCount++) {
        send_char(ptrData[DataCount]);
    }
}

void send_char(char Char) {
    while (!U1STAbits.TRMT);

    U1TXREG = Char;
}

void write_pgm_mem(char * ptrData, r32_union src_addr) {
    int size, size1;
    r32_union temp_;
    r32_union temp_Addr;
    r32_union temp_Data;

    for (size = 0, size1 = 0; size < PM_ROW_SIZE; size++) {

        temp_.v[0] = ptrData[size1 + 0];
        temp_.v[1] = ptrData[size1 + 1];
        temp_.v[2] = ptrData[size1 + 2];
        temp_.v[3] = 0;
        size1 += 3;

        write_pgm_latch(src_addr.s.high_word,
                        src_addr.s.low_word,
                        temp_.s.high_word,
                        temp_.s.low_word);

        // Device ID errata workaround:
        // save data at any address that has LSB 0x18
        if ((src_addr.v32 & 0x0000001F) == 0x18) {
            temp_Addr.v32 = src_addr.v32;
            temp_Data.v32 = temp_.v32;
        }

        if ((size != 0) &&
            (((size + 1)&0x3f) == 0) // size is multiple of 64
           ) {
            // Device ID errata workaround:
            // reload data at address with LSB of 0x18
            write_pgm_latch(temp_Addr.s.high_word,
                            temp_Addr.s.low_word,
                            temp_Data.s.high_word,
                            temp_Data.s.low_word);

            write_pgm_memory(PM_ROW_WRITE);
        }

        src_addr.v32 = src_addr.v32 + 2;
    }


}

/******************************************************************************/



