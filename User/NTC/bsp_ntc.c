#include "bsp_ntc.h"
#include <math.h>

static uint16_t s_adc_dma[NTC_ADC_BUF_LEN];// DMA 目标缓冲区，长度 2
static uint32_t s_adc_filtered = 0;        //滤波ADC值
static uint16_t s_raw_adc = 0;             //转回16位
static float s_temperature_c = 0.0;        //最终温度值（摄氏度）

HAL_StatusTypeDef NTC_Init(void)
{
    s_adc_filtered = 0;
    s_raw_adc = 0;
    s_temperature_c = 0.0;

    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)//校准ADC
    {
        return HAL_ERROR;
    }


    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)s_adc_dma, NTC_ADC_BUF_LEN) != HAL_OK)//启动 ADC 连续转换，用DMA搬运至s_adc_dma
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

void NTC_Update(void)
{
    uint16_t adc;
    float ratio;
    float r_ntc;
    float temp_k;

    adc = s_adc_dma[NTC_ADC_INDEX];

    if (s_adc_filtered == 0)
    {
        s_adc_filtered = adc;//第一次读初始值
    }
    else
    {
        s_adc_filtered = (s_adc_filtered * 7 + adc) / 8;//一阶低通滤波，权重1/8
    }
    s_raw_adc = (uint16_t)s_adc_filtered;

    if (s_raw_adc <= 5)//防止 log 出错
    {
        s_temperature_c = -40.0;//最小
        return;
    }
    if (s_raw_adc >= 4090)
    {
        s_temperature_c = 125.0;//最大
        return;
    }

    ratio = (float)s_raw_adc / (NTC_ADC_FULL_SCALE - (float)s_raw_adc); // 算出比值
    r_ntc = NTC_SERIES_RESISTOR * ratio; // ntc阻值


    temp_k = 1.0f / ((1.0f / NTC_NOMINAL_TEMPERATURE)+ (logf(r_ntc / NTC_NOMINAL_RESISTANCE) / NTC_BETA));//计算温度

    s_temperature_c = temp_k - 273.15f; 
}

float NTC_GetTemperature(void)
{
    return s_temperature_c;
}

uint16_t NTC_GetRawAdc(void)
{
    return s_adc_dma[NTC_ADC_INDEX];//读最最初始的值
}
