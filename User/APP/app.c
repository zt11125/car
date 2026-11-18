#include "app.h"


volatile uint16_t count1,count2,count3;
volatile uint8_t Mode=1;
int16_t LeftSpeed,RightSpeed,StandardSpeed;
uint16_t encoder_cnt,encoder_last;
int16_t encoder_cnt1=0;
uint16_t Sensor_Data[5];

PID_t circle=
{
	.kp=0,
	.ki=0,
	.kd=0,
	.OutMax=100,
	.OutMin=-100
};
PID_t square=
{
	.kp=0,
	.ki=0,
	.kd=0,
	.OutMax=100,
	.OutMin=-100

};
PID_t oval=
{
	.kp=0,
	.ki=0,
	.kd=0,
	.OutMax=100,
	.OutMin=-100

};
PID_t speed=//双环轨迹加速度
{
	.kp=0,
	.ki=0,
	.kd=0,
	.OutMax=100,
	.OutMin=-100
};

void App_Init(void)//所有外接需要初始值的初始化部分
{
    if (OLED_Init() == HAL_OK)
    {
        Display();
        OLED_Update();
    }
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_1|TIM_CHANNEL_2);
	encoder_last=Encoder_GetLeftCnt();
}
void ModeApp(void)
{
	if(Mode==2)
	pid_compute(&circle);
	if(Mode==3)
	pid_compute(&square);
	if(Mode==4)
	pid_compute(&oval);
}
void Display(void)//给屏幕留的显示函数
{	
	OLED_Clear();
	if(Key_GetNum()==2)
	{
		if(Mode<4)Mode++;
		else Mode=1;
		
		
	}
	if(Mode==1)
	OLED_Printf(1,1,OLED_6X8,"mode:default");
	OLED_Printf(1,10,OLED_6X8,"channel1~5: %d %d %d %d %d",Sensor_Data[0],Sensor_Data[1],Sensor_Data[2],Sensor_Data[3],Sensor_Data[4]);
	OLED_Printf(1,20,OLED_6X8,"Left Speed:%d ",LeftSpeed);
	OLED_Printf(1,30,OLED_6X8,"Right Speed:%d ",RightSpeed);
	if(Mode==2)
	OLED_Printf(1,4,OLED_6X8,"mode:circle");
	if(Mode==3)
	OLED_Printf(1,4,OLED_6X8,"mode:square");
	if(Mode==4)
	OLED_Printf(1,4,OLED_6X8,"mode:oval");
}
void App_Task(void)//所有东西的应用层
{
	ModeApp();
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//总计时器
{
	if(htim->Instance==TIM1)
	{	
		count1++;
		count2++;
		count3++;
		if(count1>=5)
		{	
			count1=0;
			encoder_cnt=encoder_last=Encoder_GetLeftCnt();
			LeftSpeed=(int16_t)(encoder_cnt-encoder_last);//这种后就不用__HAL_is那个方向检查
			encoder_last=encoder_cnt;
			
			RightSpeed = Encoder_GetRightSpeed(&encoder_cnt1);
			
		}
		if(count3>=10)
		{
			LT_Sensor_Get(Sensor_Data);
			
			
			
		}
		if(count2>=200)
		{
			count2=0;
			Display();
			OLED_Update();
		}
		
		Key_Tick();
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 if (GPIO_Pin != ENC2_A_Pin && GPIO_Pin != ENC2_B_Pin)
        return;

    uint32_t pins = GPIOB->IDR;//换一个方法读引脚
    int a = (pins & ENC2_A_Pin) != 0;
    int b = (pins & ENC2_B_Pin) != 0;

    if (GPIO_Pin == ENC2_A_Pin)//双边沿四种加四种减
        encoder_cnt1 += (a != b) ? 1 : -1;
    else
        encoder_cnt1 += (a == b) ? 1 : -1;
	
}
