#ifndef __BUCKS_H
#define __BUCKS_H

void buck_set_control_voltage(int buck_ch, float voltage);
float buck_get_output_voltage(int buck_ch);
float buck_get_output_current(int buck_ch);
void bucks_init();
void buck_enabled(int buck_ch,int enabled);


#endif
