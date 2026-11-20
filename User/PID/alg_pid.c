#include "alg_pid.h"

float Out;
void pid_compute(PID_t* PID_Data)
{
	float kp=PID_Data->kp, ki=PID_Data->ki, kd=PID_Data->kd;
	PID_Data->Error1=PID_Data->Error0;
	PID_Data->Error0=PID_Data->Target-PID_Data->Actual;
	PID_Data->ErrorInt+=PID_Data->Error0;
	if(ki*PID_Data->ErrorInt>20)PID_Data->ErrorInt=20.0f/ki;
	else if(ki*PID_Data->ErrorInt<-20)PID_Data->ErrorInt=-20.0f/ki;
	Out=(kp*PID_Data->Error0+ki*PID_Data->ErrorInt+kd*(PID_Data->Error0-PID_Data->Error1));
	if(Out>PID_Data->OutMax)PID_Data->Out=(int)PID_Data->OutMax;
	else if(Out<PID_Data->OutMin)PID_Data->Out=(int)PID_Data->OutMin;
	else PID_Data->Out=(int)Out;
}
