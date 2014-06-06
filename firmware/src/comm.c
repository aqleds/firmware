#include "comm.h"
#include "led_channels.h"


int key_is_value(unsigned char k)
{
   if (k>=128 && k<=255) return 1;
   if (k>='0' && k<='9') return 1;
   else return 0;
}

fractional key_to_value(unsigned char k)
{
    fractional res;

    if (k>='0' && k<='9')
    {

        if (k=='0') k='9'; /* rearrange 1..9 , 0*/
        else k--;

        res = (k-'0')*128;
        res = res/9;

    }
    else
    {
        res = k-128;
        return res;
    }
    return res;
}


void comm_task()
{
    int t;


    t = uart1_getchar();

    if (t<0) return;

    
}

