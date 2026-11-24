#ifndef __BSP_NTC_H
#define __BSP_NTC_H

#include "main.h"
#include "adc.h"
#include "dma.h"


#define NTC_ADC_BUF_LEN             2       //扫描序列长度
#define NTC_ADC_INDEX               0       //ntc的下标
#define NTC_ADC_FULL_SCALE          4095.0 //默认

#define NTC_SERIES_RESISTOR         10000.0//分压电阻
#define NTC_NOMINAL_RESISTANCE      10000.0//ntc阻值
#define NTC_NOMINAL_TEMPERATURE     298.15 //25度
#define NTC_BETA                    3950.0 //b值
#define NTC_TEMP_ALARM_HIGH         60.0   //最大
#define NTC_TEMP_ALARM_HYST         2.0    //防抖

uint8_t NTC_IsOverTemp(void);               //1超温0正常
HAL_StatusTypeDef NTC_Init(void);           //模块初始化
void NTC_Update(void);                      //周期性调用，读取最新 ADC 值并换算温度
float NTC_GetTemperature(void);             // 获取最新温度
uint16_t NTC_GetRawAdc(void);               //获取ADC原始值（没有滤波）

#endif
