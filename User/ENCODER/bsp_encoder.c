#include "bsp_encoder.h"

uint16_t encoder_cnt0;
uint16_t Encoder_GetLeftCnt(void)//只有左边用编码器通道
{	
	encoder_cnt0=(uint16_t)__HAL_TIM_GetCounter(&htim3);
	return encoder_cnt0;
}
int16_t Encoder_GetRightSpeed(volatile int16_t* encoder_cn)//但是我右边也可以放进来
{
	uint32_t mask=__get_PRIMASK();
	__disable_irq();
	int16_t Speed=*encoder_cn;    //tip:这种在中断中读东西清0，如果不中断中断可能在读和清中间增加1次会被清掉
	*encoder_cn=0;
	__set_PRIMASK(mask);
	return Speed;
}
