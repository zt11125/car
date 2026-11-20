#ifndef __ALG_PID_H
#define __ALG_PID_H
#include "main.h"

typedef struct
{
	float Target;       
	float Actual;
	float kp;           
	float ki;
	float kd;
	int16_t Error0;
	int16_t Error1;
	float ErrorInt;
	int16_t OutMax;
	int16_t OutMin;
	int16_t Out;
}PID_t;
void pid_compute(PID_t* PID_Data);

#endif
