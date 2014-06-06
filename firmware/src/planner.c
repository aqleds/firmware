#include "time_task.h"
#include "config.h"
#include "planner.h"
#include "led_channels.h"

#ifdef __i386 
#include <stdio.h>
#define dprintf(...) printf(...)
#else
#define dprintf(...)
#endif


extern unsigned long ms;

int h_m;
int s;


typedef struct {
    int ms;
    float brightness;
} tPlannerOverride;


tPlannerOverride channel_override[3];


int h_m_to_dayminute(int hm)
{
    int minute;
    int h=hm>>8;
    int m=hm&0xff;

    minute = (h*60) + m;
    return minute;
}

st_day day_state=ST_DAY;

const char *st_name[4]={"SUNRISE","DAY","SUNSET","NIGHT"};
const char *get_state_name()
{
    return st_name[day_state&0x3];
}


st_day planner_get_day_state()
{
    return day_state;
}


st_day st_prev(st_day d)
{
    return (d-1)%4;

}

st_day st_next(st_day d)
{
    return (d+1)%4;

}

#ifdef SOFTER
float last[4]={0,0,0,0};
#endif
void planner_calc_lights(int pass_ms)
{
    int now,t0,t1,tm,i;
    float w0,w1,br;
    // SUNRISE: (NIGHT----(1/2)SUNRISE------(1/1)DAY)
    //     DAY: (------------DAY--------------------)
    // SUNSET:  (DAY------(1/2)SUNSET-----(1/1)NIGHT)
    //   NIGHT: (------------NIGHT------------------)

#define MIN_24H (60*24)
    const st_day tr_matrix[4][3]={{ST_NIGHT,ST_SUNRISE, ST_DAY  },
                           {ST_DAY  ,ST_DAY    , ST_DAY  },
                           {ST_DAY  ,ST_SUNSET , ST_NIGHT},
                           {ST_NIGHT,ST_NIGHT,   ST_NIGHT}};


    f_day_night_profile *p0,*pm,*p1;

    /*
     * p0 is the start profile at t0            (no Tx, just for reference)
     * pm is the middle profile at (t1-t0)/2    (contains T0)
     * p1 is the final profile at t1            (contains T1)
     */
    p0 = &(AQCONFIG.profiles[tr_matrix[day_state][0]]);
    pm = &(AQCONFIG.profiles[tr_matrix[day_state][1]]);
    p1 = &(AQCONFIG.profiles[tr_matrix[day_state][2]]);


    if ((day_state==ST_DAY) || (day_state==ST_NIGHT))
    {
        w0=1.0;
        w1=0.0;
    }
    else
    {


        t0 = h_m_to_dayminute(pm->h_m_start);
        t1 = h_m_to_dayminute(p1->h_m_start);
        now = h_m_to_dayminute(h_m);

        /* calculate middle min */
        tm = t1;
        if (t1<t0)  tm+=MIN_24H;
        tm+=t0;
        tm=tm>>1;
        if (tm>MIN_24H) tm-=MIN_24H;

        //  3 cases:
        // [    t0   tm    t1   ]
        //       ***********        now
        // [ t1    t0      tm   ]
        //  **      ************    now
        // [ tm   t1   t0       ]
        //  *******     ********    now

        if (now<t0) now+=MIN_24H;
        if (t1 <t0) t1 +=MIN_24H;
        if (tm <t0) tm +=MIN_24H;

        if (now<tm) { p1 = pm; t1=tm;  } /*towards pm*/
        if (now>=tm) { p0 = pm; t0=tm; }  /*towards p1*/

        // now it's mapped from t0(p0) ---> to t1(p1)
        
        w1 = now-t0;
        w1 += ((float)s)/60.0;
        w1 = w1 / ((float)(1+t1-t0));

        w0 = 1.0-w1;

        dprintf("(%1.3f,%1.3f)",w0,w1);
    }

    for (i=0;i<3;i++)
    {
        if (channel_override[i].ms<=0)
        {
            br = w0*p0->brightness[i] + w1*p1->brightness[i];
            led_brightness(i,br);
        }
    }
        
}

void planner_state()
{

    st_day s,next;
    int h_m_next;
    int h_m_now;
    int h_m_this;
   
    for (s=0;s<4;s++)
    {
        next = st_next(s);
        h_m_now = h_m;
        h_m_this = AQCONFIG.profiles[s].h_m_start;
        h_m_next = AQCONFIG.profiles[next].h_m_start;

        /* convert to 24+H when clock turns around so we can compare */
        if (h_m_next<h_m_this) h_m_next += (24<<8);
        if (h_m_now <h_m_this) h_m_now  += (24<<8);
        

        dprintf("checking %s %02d:%02d < %02d:%02d < %02d:%02d?",
                get_state_name(), h_m_this>>8 , h_m_this&0xff,
                   h_m_now>>8  , h_m_now&0xff,
                   h_m_next>>8 , h_m_next&0xff);

        if ((h_m_now>=h_m_this) && (h_m_now<=h_m_next))
        {   
            dprintf(" yes\n");
            day_state = s;
            return;
        }
        else
        {
            dprintf(" no\n");
        }
        

    }
}

void planner_h_m_update()
{
    time_update();
    h_m=(get_hour()<<8)+get_minute();
    s = get_second();
    planner_state();
    
    dprintf("%s\t",get_state_name());
    
    
}




void planner_task()
{
    
    static unsigned long next_ms=0;
    int pass_ms,i;

    if (ms>next_ms) /* get the time every minute */
    {
        pass_ms = time_is_turbo() ? 10:1000;
        next_ms += pass_ms;

        planner_h_m_update();
        planner_calc_lights(pass_ms);

        for (i=0;i<3;i++)
        {
            if (channel_override[i].ms>0)
            {
                channel_override[i].ms-=pass_ms;
                led_brightness(i, channel_override[i].brightness);
            }
        }
    
    }

    
}

void planner_init()
{
    int ch;

    planner_h_m_update();

    for (ch=0; ch<3; ch++)
    {
        channel_override[ch].ms= 0 ;
    }
}

void planner_channel_override(int channel, float brightness, int seconds)
{

    channel_override[channel].brightness = brightness;
    channel_override[channel].ms = seconds * 1000;
}
