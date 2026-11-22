#include "UART_Transmit.h"

void UART_SendTemperature(void)
{
    char buf[32] = "";
    float  temper;

    temper =  NTC_GetTemperature();

    sprintf(buf,"Temperature :  %f",temper);

    HAL_UART_Transmit(&huart1, (uint8_t *)buf,strlen(buf), 100);
}

