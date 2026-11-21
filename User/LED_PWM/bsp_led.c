#include "bsp_led.h"

static uint16_t s_duty = 0;                     //当前占空比
static uint16_t s_tick = 0;                     //距上次步进已过的毫秒数
static volatile uint16_t s_interval_ms = 15;    //每步间隔
static uint8_t  s_dir_up = 1;                   //呼吸方向

void LED_Init(void)
{
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
}

void LED_SetTemperature(float temp_c)
{
    int32_t t;
    int32_t interval;

    t = (int32_t)(temp_c + 0.5f);//四舍五入，避免浮点计算

    if (t < 5)//控制边界5-15
    {
        t = 5;
    }
    if (t > 35)
    {
        t = 35;
    }

    interval = 15 - (t - 5) * 12 / 30;//从5-15映射到15ms-3ms,但是都是整数,会损失精度，但应该看不出来？

    if (interval < 3)//再次控制
    {
        interval = 3;
    }
    if (interval > 15)
    {
        interval = 15;
    }

    s_interval_ms = (uint16_t)interval;//输出到变量
}

void LED_Breath_Tick(void)
{
    s_tick++;
    if (s_tick < s_interval_ms)
    {
        return;
    }
    s_tick = 0;//计数器，控制频率

    if (s_dir_up)//控制灯亮度即占空比
    {
        s_duty += LED_BREATH_STEP;
        if (s_duty >= LED_PWM_MAX)
        {
            s_duty = LED_PWM_MAX;
            s_dir_up = 0;
        }
    }
    else
    {
        if (s_duty <= LED_BREATH_STEP)
        {
            s_duty = 0;
            s_dir_up = 1;
        }
        else
        {
            s_duty -= LED_BREATH_STEP;
        }
    }

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, s_duty);
}
