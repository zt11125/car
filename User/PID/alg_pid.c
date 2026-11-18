#include "alg_pid.h"


void pid_compute(PID_t* PID_Data)
{
	float kp=PID_Data->kp, ki=PID_Data->ki, kd=PID_Data->kd;
	PID_Data->Error1=PID_Data->Error0;
	PID_Data->Error0=PID_Data->Target-PID_Data->Actual;
	PID_Data->ErrorInt+=PID_Data->Error0;
	PID_Data->Out=kp*PID_Data->Error0+ki*PID_Data->ErrorInt+kd*(PID_Data->Error0-PID_Data->Error1);
	if(PID_Data->Out>PID_Data->OutMax)PID_Data->Out=PID_Data->OutMax;
	else if(PID_Data->Out<PID_Data->OutMin)PID_Data->Out=PID_Data->OutMin;
}
