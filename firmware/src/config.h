/* 
 * File:   config.h
 * Author: ajo
 *
 * Created on 15 de junio de 2012, 10:07
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif
 
#define CHANNELS 3
#define DAYPROFILES 4
#define MAGIC 0xD51
#define VERSION 3

    typedef struct
    {
        float V_min;
        float V_max;
        int profile_id;

    } f_lamp_profile;
    
    typedef struct
    {
        float brightness[CHANNELS]; 
        int h_m_start;

    } f_day_night_profile;


    typedef struct
    {
        int magic;
        int version;
        f_lamp_profile lamps[CHANNELS];
        f_day_night_profile profiles[DAYPROFILES];
        float t_brightness;
    } f_config ;

    extern f_config AQCONFIG;


    void config_defaults();

    void config_save();
    void config_load();






#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

