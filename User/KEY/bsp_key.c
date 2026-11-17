#include "bsp_key.h"

volatile uint8_t KeyNum;
volatile uint8_t value;
uint8_t Key_GetNum(void)//获得真正的按键数
{
	uint32_t mask=__get_PRIMASK();
	__disable_irq();
	value=KeyNum;  //保留一下防止被接走
	__set_PRIMASK(mask);
	return value;
}
uint8_t Key_GetState(void)//直接拿按键状态
{
	if(HAL_GPIO_ReadPin(KEY_START_GPIO_Port,KEY_START_Pin)==1) return 1;
	else if(HAL_GPIO_ReadPin(KEY_MODE_GPIO_Port,KEY_MODE_Pin)==1) return 2;
	else if(HAL_GPIO_ReadPin(KEY_STOP_GPIO_Port,KEY_STOP_Pin)==1) return 3;
	else return 0;
}
void Key_Tick(void)//20非阻塞延迟读取用来消抖
{	uint16_t count=0;
	uint8_t PreState,CurState=0;
	count++;
	if(count>20)
	{	count=0;
		PreState=CurState;
		CurState=Key_GetState();
		if(CurState==0&&PreState!=0)
		{
			KeyNum=PreState;
		}
	}

	
}