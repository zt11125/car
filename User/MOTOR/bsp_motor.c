#include "bsp_motor.h"

void Set_LeftMotor(int16_t PWM)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PWM);

}
void Set_RightMotor(int16_t PWM)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, PWM);

}
