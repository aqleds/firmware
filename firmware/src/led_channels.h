#ifndef __LED_CHANNELS_H
#define __LED_CHANNELS_H

typedef struct
{
    float V_min;    // < Voltage for 0% brightness
    float V_max;    // < Voltage for 100% brightness
    float final_value; // < final desired value 0..1
    float brightness;
    float speed;       // < increment speed     0..1
    float value;       // < current value       0..1
    int on;
    int profile;
} st_led_channel;

typedef struct
{
    float V_min;
    float V_max;
    const char name[16];
} st_lamp_profile;

extern st_lamp_profile profiles[];


#define MAX_LED_CHANNELS 3

int     is_led_ch_disabled(int channel);
void    led_task();
void    led_brightness(int channel, float brightness);
void    led_speed(int channel,float speed);
float   led_get_brightness(int channel);
void    led_init();
float   led_get_current(int channel);
float   led_get_voltage(int channel);

void    led_total_brightness(float brightness);
float   led_get_total_brightness();
int     led_profiles_n();
void    led_load_profiles();

#endif
