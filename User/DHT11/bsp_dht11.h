#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"


void DHT11_DelayUs(uint32_t us);
void DHT11_DWT_Init(void);

HAL_StatusTypeDef DHT11_Init(void);
void DHT11_Update(void);

float DHT11_GetTemperature(void);
float DHT11_GetHumidity(void);
uint8_t DHT11_IsOverTemp(void);

#endif
