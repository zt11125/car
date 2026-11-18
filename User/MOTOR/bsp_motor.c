#include "bsp_motor.h"

void Set_LeftMotor(int16_t PWM)
{
	if(PWM>0)
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PWM);
		HAL_GPIO_WritePin(AIN1_GPIO_Port,BIN1_Pin,1);
		HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,0);
	}
	else if(PWM<0)
	{	
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PWM);
		HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,1);
		HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,0);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
	}

}
void Set_RightMotor(int16_t PWM)
{
	if(PWM>0)
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, PWM);
		HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,1);
		HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,0);
	}
	else if(PWM<0)
	{	
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, PWM);
		HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,1);
		HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,0);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
	}
}
