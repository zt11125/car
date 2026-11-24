#include "UART_Transmit.h"

void UART_SendTemperature(void)
{
    char buf[32] = "";//z字符串
    float  temper;

    temper =  NTC_GetTemperature();//获取温度

    sprintf(buf,"Temperature :  %.1f\r\n",temper);//拼接字符串

    HAL_UART_Transmit(&huart1, (uint8_t *)buf,strlen(buf), 100);//输出
}

