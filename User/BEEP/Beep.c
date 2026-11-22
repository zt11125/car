#include "Beep.h"

static volatile uint16_t beep_ms = 0; 

void Beep(uint16_t ms)//开
{
    if (ms == 0) return;
    HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_SET);
    beep_ms = ms;
}

void Beep_Tick(void)//数秒
{
    if (beep_ms)
    {
        beep_ms--;
        if (beep_ms == 0)
            HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_RESET);
    }
}
