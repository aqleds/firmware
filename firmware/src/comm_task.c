
#include "comm_task.h"
#include "protocol.h"
#include <string.h>
#include "rtc_mcp79410.h"
#include "config.h"
#include "q6_binary.h"
#include <stdint.h>
#include "bucks.h"
#include "led_channels.h"
#include "time_task.h"
#include "planner.h"

void comm_init()
{
    protocol_init();
}



void comm_reply_nack(unsigned char cmd, unsigned char op);
void comm_reply_ack(unsigned char cmd, unsigned char op);

void comm_cmd_ping(unsigned char*pkt_data, int pkt_len);
void comm_cmd_rtc(unsigned char*pkt_data, int pkt_len);
void comm_cmd_timetable(unsigned char*pkt_data, int pkt_len);
void comm_cmd_channel_current_and_volts(unsigned char*pkt_data, int pkt_len);
void comm_cmd_channel_lamp(unsigned char*pkt_data, int pkt_len);
void comm_cmd_channel_ramp(unsigned char*pkt_data, int pkt_len);
void comm_cmd_turbo_mode(unsigned char*pkt_data, int pkt_len);
void comm_cmd_input_volts(unsigned char*pkt_data, int pkt_len);
void comm_cmd_channel_override(unsigned char* pkt_data, int pkt_len);
void comm_cmd_save_config(unsigned char* pkt_data, int pkt_len);
void comm_cmd_reload_config(unsigned char* pktd_ata, int pkt_len);

void comm_task()
{
    int pkt_len;
    unsigned char pkt_data_buf[PROTOCOL_MAX_BUFFER];
    unsigned char *pkt_data = pkt_data_buf;
    unsigned char cmd;
    
    pkt_len = protocol_get_buffer((char*)pkt_data,PROTOCOL_MAX_BUFFER);

    if (pkt_len==ERR_BAD_CRC)
    {
        pkt_data_buf[0]=NAK;
        protocol_send_buffer(pkt_data_buf,1);
    }
    else
    if (pkt_len>=1)
    {
       cmd = *pkt_data;
       pkt_len-=1;
       pkt_data+=1;

       switch(cmd)
       {
           case CMD_PING:             comm_cmd_ping(pkt_data, pkt_len); break;
           case CMD_RTC:              comm_cmd_rtc(pkt_data, pkt_len); break;
           case CMD_TIMETABLE:        comm_cmd_timetable(pkt_data, pkt_len); break;
           case CMD_CHANNEL_CURRENT_AND_VOLTS:comm_cmd_channel_current_and_volts(pkt_data, pkt_len); break;
           case CMD_CHANNEL_LAMP:     comm_cmd_channel_lamp(pkt_data, pkt_len); break;
           case CMD_CHANNEL_RAMP:     comm_cmd_channel_ramp(pkt_data, pkt_len); break;
           case CMD_TURBO_MODE:       comm_cmd_turbo_mode(pkt_data, pkt_len); break;
           case CMD_INPUT_VOLTS:      comm_cmd_input_volts(pkt_data, pkt_len); break;
           case CMD_CHANNEL_OVERRIDE: comm_cmd_channel_override(pkt_data, pkt_len); break;
           case CMD_SAVE_CONFIG:      comm_cmd_save_config(pkt_data, pkt_len); break;
           case CMD_RELOAD_CONFIG:    comm_cmd_reload_config(pkt_data, pkt_len); break;
       }
    }
}


void comm_reply_nack(unsigned char cmd,unsigned char op)
{
    unsigned char rply[3];
    rply[0]=cmd;
    rply[1]=op;
    rply[2]=NAK;
    protocol_send_buffer(rply,3);
}

void comm_reply_ack(unsigned char cmd, unsigned char op)
{
    unsigned char rply[3];
    rply[0]=cmd;
    rply[1]=op;
    rply[2]=ACK;
    protocol_send_buffer(rply,3);
}

void comm_cmd_ping(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    rply[0]=CMD_PING;
    memcpy(rply+1,pkt_data,pkt_len);
    protocol_send_buffer(rply,pkt_len+1);
}

void comm_cmd_rtc(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    unsigned char op=*pkt_data++;

    if (op==OP_READ)
    {
        rply[0]=CMD_RTC;
        rply[1]=OP_READ;
        rply[2]=ACK;
        rply[3]=rtc_bcd_read(ADDR_HOUR,0x3F);
        rply[4]=rtc_bcd_read(ADDR_MIN,0x7F);
        rply[5]=rtc_bcd_read(ADDR_SEC,0x7F);
        rply[6]=rtc_bcd_read(ADDR_DATE,0x3F);
        rply[7]=rtc_bcd_read(ADDR_MNTH,0x1F);
        rply[8]=rtc_bcd_read(ADDR_YEAR,0xFF);
        rply[9]=rtc_bcd_read(ADDR_DAY,0x07);

        protocol_send_buffer(rply,10);
    } else if (op==OP_READ_RT)
    {
        rply[0]=CMD_RTC;
        rply[1]=OP_READ_RT;
        rply[2]=ACK;
        rply[3]=get_hour();
        rply[4]=get_minute();
        rply[5]=get_second();
        rply[6]=rtc_bcd_read(ADDR_DATE,0x3F);
        rply[7]=rtc_bcd_read(ADDR_MNTH,0x1F);
        rply[8]=rtc_bcd_read(ADDR_YEAR,0xFF);
        rply[9]=rtc_bcd_read(ADDR_DAY,0x07);
        rply[10]=planner_get_day_state();

        protocol_send_buffer(rply,11);
    }
    else
    {
	rtc_write(ADDR_HOUR, rtc_int_to_bcd(*pkt_data++));    
        rtc_write(ADDR_MIN,  rtc_int_to_bcd(*pkt_data++));
        rtc_write(ADDR_SEC,  rtc_int_to_bcd(*pkt_data++)|START_32KHZ);
        rtc_write(ADDR_DATE, rtc_int_to_bcd(*pkt_data++));
        rtc_write(ADDR_MNTH, rtc_int_to_bcd(*pkt_data++));
        rtc_write(ADDR_YEAR, rtc_int_to_bcd(*pkt_data++));
        rtc_write(ADDR_DAY,  rtc_int_to_bcd(*pkt_data++)|VBATEN);

        comm_reply_ack(CMD_RTC,OP_WRITE);
   }
    
}
void comm_cmd_timetable(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    unsigned char op=*pkt_data++;
    unsigned int h,m,i;

   
    if (op==OP_READ)
    {
        rply[0]=CMD_TIMETABLE;
        rply[1]=OP_READ;
        rply[2]=ACK;

        for (i=0;i<4;i++)
        {
            rply[i*2+3]=AQCONFIG.profiles[i].h_m_start>>8;
            rply[i*2+4]=AQCONFIG.profiles[i].h_m_start&0xFF;
        }
        
        protocol_send_buffer(rply, 3+4*2);
    }
    else
    {
        for (i=0;i<4;i++)
        {
            h = *pkt_data++;
            m = *pkt_data++;
            AQCONFIG.profiles[i].h_m_start = (h<<8)|m;
        }
        comm_reply_ack(CMD_TIMETABLE,OP_WRITE);
    }
}

void comm_cmd_channel_current_and_volts(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    unsigned char op=*pkt_data++;
    uint8_t channel =*pkt_data++;

    if ((channel>=1 && channel<=3) && op==OP_READ)
    {

        float current = buck_get_output_current(channel-1);
        float volts   = buck_get_output_voltage(channel-1);

        rply[0] = CMD_CHANNEL_CURRENT_AND_VOLTS;
        rply[1] = OP_READ;
        rply[2] = ACK;

        Q6_store(rply+3, current);
        Q6_store(rply+5, volts);

        protocol_send_buffer(rply, 7);
    }
    else
    {
        comm_reply_nack(CMD_CHANNEL_CURRENT_AND_VOLTS,op);
    }
}

void comm_cmd_channel_lamp(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    unsigned char op=*pkt_data++;
    uint8_t channel =*pkt_data++;

    if (!(channel>=1 && channel<=3))
    {
       comm_reply_nack(CMD_CHANNEL_LAMP, op);
       return;
    }

    channel--;

    if (op==OP_READ)
    {
        rply[0] = CMD_CHANNEL_LAMP;
        rply[1] = OP_READ;
        rply[2] = ACK;

        Q6_store(rply+3, AQCONFIG.lamps[channel].V_min);
        Q6_store(rply+5, AQCONFIG.lamps[channel].V_max);

        protocol_send_buffer(rply, 7);
    }
    else
    {
        AQCONFIG.lamps[channel].V_min = Q6_extract(pkt_data  );
        AQCONFIG.lamps[channel].V_max = Q6_extract(pkt_data+2);

        led_load_profiles();
        
        comm_reply_ack(CMD_CHANNEL_LAMP, OP_WRITE);
    }
}

void comm_cmd_input_volts(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    unsigned char op=*pkt_data++;

    if (op==OP_READ)
    {

        float volts = -1.0;
        
        rply[0] = CMD_INPUT_VOLTS;
        rply[1] = OP_READ;
        rply[2] = ACK;

        Q6_store(rply+3, volts);

        protocol_send_buffer(rply, 5);
    }
    else
    {
        comm_reply_nack(CMD_INPUT_VOLTS,op);
    }
}

void comm_cmd_channel_ramp(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    unsigned char op=*pkt_data++;
    uint8_t channel =*pkt_data++;

    float sunrise, day, sunset, night;

    if (!(channel>=0 && channel<=3))
    {
       comm_reply_nack(CMD_CHANNEL_RAMP,op);
       return;
    }

    if (op==OP_READ)
    {
        rply[0] = CMD_CHANNEL_RAMP;
        rply[1] = OP_READ;
        rply[2] = ACK;
        rply[3] = channel;

        if (channel == 0)
        {
            Q6_store(rply+4, AQCONFIG.t_brightness);
        }
        else
        {
            channel--;

            Q6_store(rply+4, AQCONFIG.profiles[ST_SUNRISE].brightness[channel]);
            Q6_store(rply+6, AQCONFIG.profiles[ST_DAY    ].brightness[channel]);
            Q6_store(rply+8, AQCONFIG.profiles[ST_SUNSET ].brightness[channel]);
            Q6_store(rply+10, AQCONFIG.profiles[ST_NIGHT  ].brightness[channel]);
        }

        protocol_send_buffer(rply, 12);
    }
    else
    {
        if (channel == 0)
        {
            AQCONFIG.t_brightness = Q6_extract(pkt_data);
            led_total_brightness(AQCONFIG.t_brightness);
        }
        else
        {
            channel--;
            sunrise = Q6_extract(pkt_data  );
            day     = Q6_extract(pkt_data+2);
            sunset  = Q6_extract(pkt_data+4);
            night   = Q6_extract(pkt_data+6);

            AQCONFIG.profiles[ST_SUNRISE].brightness[channel] = sunrise;
            AQCONFIG.profiles[ST_DAY    ].brightness[channel] = day;
            AQCONFIG.profiles[ST_SUNSET ].brightness[channel] = sunset;
            AQCONFIG.profiles[ST_NIGHT  ].brightness[channel] = night;
        }
        comm_reply_ack(CMD_CHANNEL_RAMP, OP_WRITE);

    }
}

void comm_cmd_turbo_mode(unsigned char*pkt_data, int pkt_len)
{
    unsigned char rply[PROTOCOL_MAX_BUFFER];
    unsigned char op=*pkt_data++;

    if (op == OP_READ)
    {
        rply[0] = CMD_TURBO_MODE;
        rply[1] = OP_READ;
        rply[2] = ACK;
        rply[3] = time_is_turbo();

        protocol_send_buffer(rply, 4);
    } else
    {
        time_turbo(pkt_data[0]);
        comm_reply_ack(CMD_TURBO_MODE, OP_WRITE);
    }


}

void comm_cmd_save_config(unsigned char*pkt_data, int pkt_len)
{
    config_save();
    comm_reply_ack(CMD_SAVE_CONFIG, pkt_data[0]);
}

void comm_cmd_reload_config(unsigned char*pkt_data, int pkt_len)
{
    config_load();
    led_load_profiles();
    comm_reply_ack(CMD_RELOAD_CONFIG, pkt_data[0]);
}

void comm_cmd_channel_override(unsigned char*pkt_data, int pkt_len)
{
    unsigned char op=*pkt_data++;
    unsigned char channel=*pkt_data++;
    int seconds;
    float brightness;

    if (!(channel>=1 && channel<=3))
    {
       comm_reply_nack(CMD_CHANNEL_RAMP,op);
       return;
    }

    if (op == OP_WRITE)
    {
        brightness = Q6_extract(pkt_data  );
        seconds = int16_extract(pkt_data+2);
        planner_channel_override(channel-1, brightness, seconds);
        comm_reply_ack(CMD_CHANNEL_OVERRIDE, OP_WRITE);
    }
    else
    {
        comm_reply_nack(CMD_CHANNEL_OVERRIDE, op);
    }
}