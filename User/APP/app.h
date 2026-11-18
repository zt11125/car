#ifndef APP_H
#define APP_H
#include "main.h"
#include "tim.h"
#include "alg_pid.h"
#include "bsp_oled.h"
#include "bsp_key.h"
#include "LT_Sensor.h"
#include "bsp_encoder.h"
void App_Init(void);
void App_Task(void);
void Display(void);
#endif 
