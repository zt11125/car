#include "LT_Sensor.h"


void LT_Sensor_Get(uint16_t Data[])
{
	uint32_t pin=GPIOB->IDR;
	for(int i=0;i<5;i++)
	{
		Data[i]=(uint16_t)((pin>>(10+i))&1);
	}
}
