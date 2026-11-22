#include "app.h"
#include "tim.h"
#include "alg_pid.h"
#include "bsp_oled.h"
#include "bsp_key.h"
#include "LT_Sensor.h"
#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "Motor_config.h"
#include "bsp_ntc.h"
#include "bsp_led.h"
#include "UART_Transmit.h"

volatile uint16_t count1,count2,count3;
volatile uint8_t Mode=1,Start=0;
float DeltaSpeed;
volatile uint16_t encoder_cnt,encoder_last;
volatile int16_t encoder_cnt1=0;
uint16_t Sensor_Data[5];
static uint32_t last_oled_tick = 0;
static uint32_t last_uart_tick = 0;

PID_t circle=
{
	.kp=0,
	.ki=0,
	.kd=0,
	.Target=0,//轨迹环要居中
	.OutMax=100,
	.OutMin=-100
};
PID_t square=
{
	.kp=0,
	.ki=0,
	.kd=0,
	.Target=0,
	.OutMax=100,
	.OutMin=-100

};
PID_t oval=
{
	.kp=0,
	.ki=0,
	.kd=0,
	.Target=0,
	.OutMax=100,
	.OutMin=-100

};
PID_t Leftspeed=//双环轨迹加速度
{
	.kp=0,
	.ki=0,
	.kd=0,
	.OutMax=100,
	.OutMin=-100
};
PID_t Rightspeed=
{
	.kp=0,
	.ki=0,
	.kd=0,
	.OutMax=100,
	.OutMin=-100
};

void App_Init(void)//所有外接需要初始值的初始化部分
{

	(void)NTC_Init();
	LED_Init();
    
	if (OLED_Init() == HAL_OK)
    {
        Display();
        OLED_Update();
    }
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	encoder_last=Encoder_GetLeftCnt();
}

void Display(void)//给屏幕留的显示函数
{	
	OLED_Clear();
	if(Mode==1)
	OLED_Printf(1,1,OLED_6X8,"mode:default");
	if(Mode==2)
	OLED_Printf(1,4,OLED_6X8,"mode:circle");
	if(Mode==3)
	OLED_Printf(1,4,OLED_6X8,"mode:square");
	if(Mode==4)
	OLED_Printf(1,4,OLED_6X8,"mode:oval");

	OLED_Printf(1,10,OLED_6X8,"channel1~5: %d %d %d %d %d",Sensor_Data[0],Sensor_Data[1],Sensor_Data[2],Sensor_Data[3],Sensor_Data[4]);
	OLED_Printf(1,20,OLED_6X8,"Left Speed:%f ",Leftspeed.Actual);
	OLED_Printf(1,30,OLED_6X8,"Right Speed:%f ",Rightspeed.Actual);
	OLED_Printf(1, 40, OLED_6X8, "Temp:%.1f C", (double)NTC_GetTemperature());
}


void Key_Get(void)
{
	switch (Key_GetNum())
		{
		
		 case 3:
			
				Start=0;		//先清防止卡中间导致清完后start非0
				Set_LeftMotor(0);
				Set_RightMotor(0);
				break;
		 case 1: 
			 if (Start == 0)
				{
					encoder_last = Encoder_GetLeftCnt();//所有清零防止遗留,接口不会有问题只要保存最后一次就行，不会随便累计
					(void)Encoder_GetRightSpeed(&encoder_cnt1);//这个调用后会清零cnt，void这里是抛弃旧函数标记
					Rightspeed.ErrorInt=0;
					Leftspeed.ErrorInt=0;
					circle.ErrorInt=0;
					oval.ErrorInt=0;
					square.ErrorInt=0;
					count1 = 0;
					Start = 1;
				}
			break;
		 case 2:
				if(Mode<4)Mode++;
				else Mode=1;
			break;
		 default:
			 break;
	 }
	
}
void App_Task(void)//所有东西的应用层
{

	Key_Get();
	
    uint32_t now = HAL_GetTick();
    if ((uint32_t)(now - last_oled_tick) >= 200U)//放tim里优先级高会卡住systick
    {
        last_oled_tick = now;
		NTC_Update();
		LED_SetTemperature(NTC_GetTemperature());
        Display();
        OLED_Update();
    }
	    if ((uint32_t)(now - last_uart_tick) >= 500)  
    {
        last_uart_tick = now;
        UART_SendTemperature();
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//总计时器
{
	
	if(htim->Instance==TIM1)
	{
		LED_Breath_Tick();

		if(Start)
		{	
			count1++;
			count3++;
			if(count1>=5)
			{	
				count1=0;
				encoder_cnt=Encoder_GetLeftCnt();
				Leftspeed.Actual=(int16_t)(encoder_cnt-encoder_last);//这种后就不用__HAL_is那个方向检查
				encoder_last=encoder_cnt;
			
				Rightspeed.Actual = Encoder_GetRightSpeed(&encoder_cnt1);
				

				Leftspeed.Target=STANDARD_SPEED-DeltaSpeed/2;//float可以是负数，已经判断转向了
				Rightspeed.Target=STANDARD_SPEED+DeltaSpeed/2;
				pid_compute(&Leftspeed);			//这里速度环
				pid_compute(&Rightspeed);
				Set_LeftMotor(Leftspeed.Out);//差速记得写
				Set_RightMotor(Rightspeed.Out);
			}
			if(count3>=10)
			{
				count3=0;
				LT_Sensor_Get(Sensor_Data);
				if((Sensor_Data[0]||Sensor_Data[1]||Sensor_Data[2]||Sensor_Data[3]||Sensor_Data[4])==0)
					{
						Start=0;
						Set_LeftMotor(0);
						Set_RightMotor(0);
					}
				else{
				square.Actual=Sensor_Data[0]*(-2)+Sensor_Data[1]*(-1)+Sensor_Data[2]*(0)+Sensor_Data[3]*(1)+Sensor_Data[4]*(2);//权重计算
				oval.Actual=Sensor_Data[0]*(-2)+Sensor_Data[1]*(-1)+Sensor_Data[2]*(0)+Sensor_Data[3]*(1)+Sensor_Data[4]*(2);
				circle.Actual=Sensor_Data[0]*(-2)+Sensor_Data[1]*(-1)+Sensor_Data[2]*(0)+Sensor_Data[3]*(1)+Sensor_Data[4]*(2);
				switch (Mode)							//这里放循迹环
				{
					case 1:
						DeltaSpeed=0;
						break;
					case 2:pid_compute(&circle);
					DeltaSpeed=(float)(circle.Out*SPEED_RAGE/circle.OutMax);//因为最大和最小一般是相反数所以我偷懒了
						break;
					case 3:pid_compute(&square);
						DeltaSpeed=(float)(square.Out*SPEED_RAGE/square.OutMax);
						break;
					case 4:pid_compute(&oval);
						DeltaSpeed=(float)(oval.Out*SPEED_RAGE/oval.OutMax);
						break;
					default:
						break;
				}
				}
			}
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
