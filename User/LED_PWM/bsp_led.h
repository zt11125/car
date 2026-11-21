#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "main.h"
#include "tim.h"

#define LED_PWM_MAX       999   //自动重装载
#define LED_BREATH_STEP   10    //步进

void LED_Init(void);                    //初始化
void LED_SetTemperature(float temp_c);  //根据温度调频率
void LED_Breath_Tick(void);             //1ms一次
#endif
