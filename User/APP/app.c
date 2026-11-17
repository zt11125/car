#include "app.h"
volatile uint16_t count1,count2;
volatile uint8_t Mode;
void App_Init(void)//所有外接需要初始值的初始化部分
{

}

void Display(void)//给屏幕留的显示函数
{	
	if(Mode==1)
	OLED_Printf(1,4,OLED_6X8,"mode:default");
	
	if(Mode==2)
	OLED_Printf(1,4,OLED_6X8,"mode:default");
	if(Mode==3)
	OLED_Printf(1,4,OLED_6X8,"mode:default");
	if(Mode==4)
	OLED_Printf(1,4,OLED_6X8,"mode:default");
}
void App_Task(void)//所有东西的应用层
{
  
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//总计时器
{
	if(htim->Instance==TIM1)
	{	
		count1++;
		count2++;
		if(count1>=5)
		{	count1=0;
		
		}
		if(count2>=20)
		{
		
		}
		
		kEY_Tick();
	}
}
