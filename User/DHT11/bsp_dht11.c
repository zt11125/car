#include "bsp_dht11.h"
#include <math.h>
#include <string.h>

static uint32_t cycles_per_us = 72;
static float temperature = 0.0;
static float humidity = 0.0;
static uint32_t last_read_tick = 0;
static uint8_t valid = 1;


void DHT11_DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    cycles_per_us = SystemCoreClock / 1000000U;
    if (cycles_per_us == 0U)
    {
        cycles_per_us = 1U;
    }
}

void DHT11_DelayUs(uint32_t us)
{
    uint32_t start;
    uint32_t ticks;

    start = DWT->CYCCNT;
    ticks = us * cycles_per_us;

    while ((DWT->CYCCNT - start) < ticks)
    {
    }
}
/*微妙级延时*/

static void DHT11_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

static void DHT11_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

static void DHT11_Rec_Byte(uint8_t *value)
{
    uint8_t i;
    uint8_t data = 0;

    for (i = 0; i < 8; i++)
    {
        while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) != GPIO_PIN_SET)
        {
        }

        DHT11_DelayUs(40);

        data <<= 1;
        if (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
        {
            data |= 1U;
        }

        while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) != GPIO_PIN_RESET)
        {
        }
    }

    *value = data;

}

static uint8_t DHT11_ReadData(uint8_t data[5])
{
    uint8_t i;
    uint32_t primask;

    /* 1. 主机拉低 20ms */
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);

    /* 2. 关中断，进入时序关键区 */
    primask = __get_PRIMASK();
    __disable_irq();

    /* 3. 释放总线，等 DHT11 响应 */
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    DHT11_DelayUs(30);
    DHT11_SetInput();

    /* 4. 响应低 80us -> 响应高 80us -> 第一位低电平 */
     /* 响应低电平 */
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) != GPIO_PIN_RESET)
    {
    }

    /* 响应高电平 */
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) != GPIO_PIN_SET)
    {
    }

    /* 响应高电平结束，进入第一位数据 */
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) != GPIO_PIN_RESET)
    {
    }

    /* 5. 接收 5 个字节 */
    for (i = 0; i < 5; i++)
    {
        DHT11_Rec_Byte(&data[i]);
    }

    /* 7. 恢复中断 */
    __set_PRIMASK(primask);
    return 1;
}

HAL_StatusTypeDef DHT11_Init(void)
{
    DHT11_SetInput();
    DHT11_DWT_Init();

    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);

    temperature = 0.0;
    humidity = 0.0;
    last_read_tick = HAL_GetTick();

    return HAL_OK;
}


void DHT11_Update(void)
{
    uint8_t data[5];
    uint32_t now;

    now = HAL_GetTick();

    if ((uint32_t)(now - last_read_tick) < 2000)
    {
        return;
    }

    if (!DHT11_ReadData(data))
    {
        valid = 0;
        return;
    }

    last_read_tick = now;

    humidity = (float)data[0] + (float)data[1] * 0.1;
    temperature = (float)data[2] + (float)data[3] * 0.1;

}

float DHT11_GetTemperature(void)
{
    return temperature;
}

float DHT11_GetHumidity(void)
{
    return humidity;
}

uint8_t DHT11_IsOverTemp(void)
{
    if(!valid)
    {
        return 0;
    }
    return (temperature >= 40) ? 1 : 0;
}




