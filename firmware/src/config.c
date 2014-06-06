#include "config.h"
#include "planner.h"
#include "rtc_mcp79410.h"
#include <string.h>


f_config AQCONFIG;

void config_defaults()
{
    int i;
    for (i=0;i<CHANNELS;i++) /* safe profile by default, all off 0 volts*/
    {
        AQCONFIG.lamps[i].V_max=12;
        AQCONFIG.lamps[i].V_min=6;
        AQCONFIG.lamps[i].profile_id=-1;
    }

    for (i=0;i<DAYPROFILES;i++)
    {
        AQCONFIG.profiles[i].brightness[0]=0.0;
        AQCONFIG.profiles[i].brightness[1]=0.0;
        AQCONFIG.profiles[i].brightness[2]=0.0;
    }

    AQCONFIG.profiles[ST_SUNRISE].brightness[1]=0.7;
    AQCONFIG.profiles[ST_DAY].brightness[0]=1.0;
    AQCONFIG.profiles[ST_DAY].brightness[1]=0.9;
    AQCONFIG.profiles[ST_SUNSET].brightness[2]=0.7;
    AQCONFIG.profiles[ST_SUNSET].brightness[1]=0.7;
    AQCONFIG.profiles[ST_NIGHT].brightness[0]=0.05;

    AQCONFIG.t_brightness=0.9;
    
    /* SUNRISE */
    AQCONFIG.profiles[0].h_m_start = 7<<8;

    /* DAY */
    AQCONFIG.profiles[1].h_m_start = 11<<8;
  
    /* TWILIGHT */
    AQCONFIG.profiles[2].h_m_start = 19<<8;

    /* NIGHT */
    AQCONFIG.profiles[3].h_m_start = 23<<8;
    
    

    AQCONFIG.version=VERSION;
    AQCONFIG.magic=MAGIC;

    #ifdef __i386
    AQCONFIG.profiles[0].brightness[0]=0.5;
    AQCONFIG.profiles[0].brightness[1]=0.3;
    AQCONFIG.profiles[0].brightness[2]=0.0;

    AQCONFIG.profiles[1].brightness[0]=1.0;
    AQCONFIG.profiles[1].brightness[1]=0.3;
    AQCONFIG.profiles[1].brightness[2]=0.2;
    
    AQCONFIG.profiles[2].brightness[0]=1.0;
    AQCONFIG.profiles[2].brightness[1]=0.3;
    AQCONFIG.profiles[2].brightness[2]=0.2;

    AQCONFIG.profiles[3].brightness[0]=0.0;
    AQCONFIG.profiles[3].brightness[1]=0.1;
    AQCONFIG.profiles[3].brightness[2]=0.0;

    #endif


}

void config_save()
{
    rtc_eeprom_write((char*)&AQCONFIG,0,sizeof(AQCONFIG));
}

void config_read()
{
    rtc_eeprom_read((char*)&AQCONFIG,0,sizeof(AQCONFIG));

}

void config_load()
{
    memset(&AQCONFIG,0,sizeof(AQCONFIG));
    config_read();
    if ((AQCONFIG.magic!=MAGIC) || (AQCONFIG.version!=VERSION))
    {
        config_defaults();
        config_save();
    }
    
}


