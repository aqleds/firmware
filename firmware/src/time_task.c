#include "rtc_mcp79410.h"
#include "time_task.h"
#include "systick.h"

long int t_hour=7,t_minute=24, t_second=0;
int turbo=0;
long int last_upd_ms=0;

int time_is_turbo()
{
    return turbo;
}

void time_turbo(int on)
{
    turbo = on;
}

void time_update()
{
    if (!turbo)
    {
       long int ms = systicks_get_ms();
       if ((ms-last_upd_ms)<1000) return;
       last_upd_ms=ms;

       t_hour   = rtc_bcd_read(ADDR_HOUR,0x3F);
       t_minute = rtc_bcd_read(ADDR_MIN,0x7F);
       t_second = rtc_bcd_read(ADDR_SEC,0x7F);
       
    } else { /* fast time simulation */
       long int ms = systicks_get_ms();

       t_second = ms/2;
       t_minute = t_second/60;
       t_hour = t_minute/60;

       t_second=t_second%60;
       t_minute=t_minute%60;
       t_hour=t_hour%24;
    }

}

void set_time(int h,int m)
{
    t_hour=h;
    t_minute =m;
    t_second = 30;

    rtc_write(ADDR_HOUR,rtc_int_to_bcd(t_hour));    
    rtc_write(ADDR_SEC,rtc_int_to_bcd(t_second)|START_32KHZ);   
    rtc_write(ADDR_MIN,rtc_int_to_bcd(t_minute));     

}

int get_hour()
{
    return t_hour;
}

int get_minute()
{
    return t_minute;
}

int get_second()
{
    return t_second;
}

