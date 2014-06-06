#include "led_channels.h"
#include "bucks.h"
#include "config.h"

st_led_channel led_channels[MAX_LED_CHANNELS];
void update_channel(int channel);

float total_brightness=1.0;

void led_load_profiles()
{
    int i;
    for (i=0;i<MAX_LED_CHANNELS;i++)
    {
     
        led_channels[i].V_max = AQCONFIG.lamps[i].V_max;
        led_channels[i].V_min = AQCONFIG.lamps[i].V_min;
    }
}

void led_init()
{
    int i;
    st_led_channel *lc;
    for (i=0;i<MAX_LED_CHANNELS;i++)
    {
        lc=&led_channels[i];
        lc->V_min=0;
        lc->V_max=1;
        lc->final_value=0;
        lc->value=0;
        lc->speed=0.01;
        lc->on=0;
        lc->profile=-1;
    }
}
void led_total_brightness(float brightness)
{
    int i;
    total_brightness = brightness;

    for (i=0;i<MAX_LED_CHANNELS;i++)
    {
        led_brightness(i,led_channels[i].brightness);
    }
}

float led_get_total_brightness()
{
    return total_brightness;
}

void led_brightness(int channel, float brightness)
{
    st_led_channel *ch = &led_channels[channel];
    float sp;
    if (brightness>1.0) brightness=1.0;
    if (brightness<0.0) brightness=0.0;

    ch->brightness = brightness;
    brightness *= total_brightness;
    sp = (ch->value-brightness)/50.0;
    if (sp<0) sp=-sp;

    ch->speed=sp;
    ch->final_value = brightness;
}

int is_led_ch_disabled(int channel)
{
    return !(led_channels[channel].on);
}
void update_channel(int channel)
{
    st_led_channel *ch = &led_channels[channel];
    float V_span = ch->V_max-ch->V_min;
    float value,brightness;

    int last_ch_on;

    /* must increase the value? */
    if (ch->value<ch->final_value)
    {
        ch->value += ch->speed;
        if (ch->value>ch->final_value)
            ch->value = ch->final_value;
    }

    /* must decrease the value? */
    if (ch->value>ch->final_value)
    {
        ch->value -= ch->speed;
        if (ch->value<ch->final_value)
            ch->value = ch->final_value;
    }

    last_ch_on = ch->on;
    if (ch->value <= 0.0)  ch->on = 0;
    else                 ch->on = 1;


    brightness=ch->value*ch->value; // unlinearize with x^2

    value = ch->V_min + (V_span*brightness);
    /* saturate to V_max */
    if (value>(ch->V_max)) value=ch->V_max;
    if (value<=(ch->V_min)) value = 0;

    buck_set_control_voltage(channel, value);
    
    if (last_ch_on!=ch->on)
        buck_enabled(channel,ch->on);
    

}

void led_task()
{


    int ch;
    for (ch=0;ch<MAX_LED_CHANNELS;ch++)
    {
        update_channel(ch);
    }
 



}

float led_get_brightness(int channel)
{
    return led_channels[channel].value;
}

float led_get_current(int channel)
{
    return buck_get_output_current(channel);
}

float led_get_voltage(int channel)
{
    return buck_get_output_voltage(channel);
}