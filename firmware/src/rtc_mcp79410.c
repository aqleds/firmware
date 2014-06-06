#include "soft_i2c_channels.h"
#include "rtc_mcp79410.h"
#include "systick.h"


int rtc_bcd_to_int(int data)
{
    int ret=data&0x0f;
    ret+=((data>>4))*10;
    return ret;
}

int rtc_int_to_bcd(int data)
{
    int ret=data%10;
    ret+=(data/10)<<4;
    return ret;
}


//************************************************************************************
//                      I2C RTCC DRIVERS
//************************************************************************************
 int rtc_write(unsigned char rtcc_reg,unsigned char rtcc_val)
 {
   int ack;
   i2c_start();
   ack = i2c_write(ADDR_RTCC_WRITE);
   i2c_write(rtcc_reg);
   i2c_write(rtcc_val);
   i2c_stop();

   return ack;
 }

//.....................................................................................
 int rtc_read(unsigned char rtcc_reg)
 { 
   int val;
   i2c_start();
   if (!i2c_write(ADDR_RTCC_WRITE)) return -1;
   i2c_write(rtcc_reg);
   i2c_stop();
   i2c_start();
   i2c_write(ADDR_RTCC_READ);
   val = i2c_read(0);
   i2c_stop();
   
   return val;
 }

 int rtc_bcd_read(unsigned char rtcc_reg,int mask)
 {
     return rtc_bcd_to_int(rtc_read(rtcc_reg)&mask);
 }
//...................................................................................
 void rtc_init(void)
 {                         
  int day = rtc_read(ADDR_DAY);
  rtc_write(ADDR_CTRL,SQWE+ALM_NO+MFP_01H);
  rtc_write(ADDR_DAY,day|VBATEN);
  if(!(day&OSCON))
  {
    rtc_write(ADDR_YEAR,0x10);     // initialize YEAR  register
    rtc_write(ADDR_MNTH,0x03);     // initialize MONTH register
    rtc_write(ADDR_DATE,0x01);     // initialize DATE  register
    rtc_write(ADDR_HOUR,0x09);     // initialize HOUR  register
    rtc_write(ADDR_MIN,0x00);     // initialize MIN   register
    rtc_write(ADDR_SEC,0x30|START_32KHZ);   //init SEC   register, set START bit
  }
 }
 //.........................................................................................


 void rtc_eeprom_read(char *dst, int start, int len)
 {
     i2c_start();
     i2c_write(ADDR_EEPROM_WRITE);
     i2c_write(start);
     i2c_stop();

     i2c_start();
     i2c_write(ADDR_EEPROM_READ);
     while(len--)
     {
         *dst++=i2c_read(len!=0);
     }
     i2c_stop();

 }

 void rtc_eeprom_write(char *src, int start, int len)
 {
     int last_page=0xff,
         page;

     while(len--)
     {
         page = start&(~0x07);
         if ((last_page!=page)&&(len)) // if we changed 8-byte page restart
         {
            i2c_stop();             // stop, and let it write
            delay_ms(10);
            i2c_start();            // start again
            i2c_write(ADDR_EEPROM_WRITE);
            i2c_write(start);
            last_page=page;
         }
         i2c_write(*src++);
         start++;
     }
     i2c_stop();

     delay_ms(10);

 }
