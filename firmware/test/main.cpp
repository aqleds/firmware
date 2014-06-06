#include <iostream>
#include <stdio.h>

#include <string.h>
using namespace std;

int hour,minute,second;

// functions provided for the .C files to call during test
extern "C" {
    #include "../src/planner.h"
    #include "../src/config.h"
    float tbright=0;
    void led_brightness(int channel, float bright)
    {
        //cout << "[bright "<<channel<<" "<<bright << "] ";
        printf("%1.3f\t",bright*tbright);
    }
    void led_total_brightness(float bright)
    {
        tbright=bright;
        printf("%1.3f\t",bright);
    }
    
    long int ms=0;
    
    void time_update(float bright)
    {
        second = ms/1000;
        minute = second/60;
        hour = minute/60;
        
        second = second%60;
        minute = minute%60;
        hour = hour%24;
    }
    
    int get_hour() { return hour; }
    int get_minute() { return minute; }
    int get_second() { return second; }
    
    void rtc_eeprom_read(char *dst, int start, int len)
    {
        memset(dst,0,len);
    }
    void rtc_eeprom_write(char *src, int start, int len)
    {
    }
    
    int ui_led_planner_override()
    {
        return 0;
    }

    
    
    
}


int main(int argc, char **argv) 
{
    float h;
    
    cout << "STARTING PLANNER TEST" << endl;
    
    config_load();
    planner_init();
    
    cout << "# this file is called planner.dat" << endl;
    cout << "# Planner results across day time" << endl;
    cout << "# hour \t\tstate\tch1\tch2\tch3\tglobal" << endl;
    
    while(ms<(48*60*60*1000)) // 48h test
    {
        
        ms=ms+1000;
        //printf("%02d:%02d:%02d>",hour,minute,second);
        
        h = ((float)ms)/((float)60*60*1000);
        printf("%02.8f\t",h);
        if (h>=6.9)
        {
          //  cout << "break here";
        }
        planner_task();
        cout << endl;
    }
        
    
    return 0;
}
