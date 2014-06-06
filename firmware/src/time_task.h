/* 
 * File:   time_task.h
 * Author: ajo
 *
 * Created on 14 de junio de 2012, 16:06
 */

#ifndef TIME_TASK_H
#define	TIME_TASK_H

#ifdef	__cplusplus
extern "C" {
#endif
int time_is_turbo();
void time_turbo(int on);
void time_update();
int get_hour();
int get_minute();
int get_second();

void set_time(int h,int m);




#ifdef	__cplusplus
}
#endif

#endif	/* TIME_TASK_H */

