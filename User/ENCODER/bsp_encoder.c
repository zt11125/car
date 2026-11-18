#include "bsp_encoder.h"

uint16_t encoder_cnt0;
uint16_t Encoder_GetLeftCnt(void)//只有左边用编码器通道
{	
	encoder_cnt0=(uint16_t)__HAL_TIM_GetCounter(&htim3);
	return encoder_cnt0;
}
int16_t Encoder_GetRightSpeed(int16_t* encoder_cn)//但是我右边也可以放进来
{
	int16_t Speed=*encoder_cn;
	*encoder_cn=0;
	return Speed;
}
