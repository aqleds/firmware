#ifndef __PLANNER_H
#define __PLANNER_H

typedef enum
{
    ST_SUNRISE=0,
    ST_DAY=1,
    ST_SUNSET=2,
    ST_NIGHT=3

} st_day;



const char *get_state_name();

void planner_init();
void planner_task();
void planner_channel_override(int channel, float brightness, int seconds);
st_day planner_get_day_state();
#endif