#include "app.h"
#include "tim.h"
#include "alg_pid.h"
#include "bsp_oled.h"
#include "bsp_key.h"
#include "LT_Sensor.h"
#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "Motor_config.h"
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "UART_Transmit.h"
#include "Beep.h"

volatile uint16_t count1,count2,count3;
volatile uint8_t Mode=ST_STAIGHT,Start=0;//START为0时强制进入s0，为1时取消状态位固定
float DeltaSpeed;
float Dist;//距离
volatile uint16_t encoder_cnt,encoder_last;
volatile int16_t encoder_cnt1=0;
uint16_t Sensor_Data[5];
static uint32_t last_oled_tick = 0;
static uint32_t last_uart_tick = 0;
static uint16_t line_lost_ms = 0;
char line[20] = "";

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

	LED_Init();
    DHT11_Init();

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
	Beep(100);
}

void Display(void)//给屏幕留的显示函数
{	
	OLED_Clear();
	if(Mode==ST_FAULT)
	{
		OLED_Printf(1,20,OLED_6X8,"errot error error");
		return ;
	}
	if(Start)
	{
		if(Mode==ST_STAIGHT)
		OLED_Printf(1,1,OLED_6X8,"mode:default");
		if(Mode==ST_CIRCLE)
		OLED_Printf(1,1,OLED_6X8,"mode:circle");
		if(Mode==ST_SQUARE)
		OLED_Printf(1,1,OLED_6X8,"mode:square");
		if(Mode==ST_OVAL)
		OLED_Printf(1,1,OLED_6X8,"mode:oval");

		OLED_Printf(1,10,OLED_6X8,"channel1~5: %d %d %d %d %d",Sensor_Data[0],Sensor_Data[1],Sensor_Data[2],Sensor_Data[3],Sensor_Data[4]);
		OLED_Printf(1,20,OLED_6X8,"Left Speed:%.2f ",Leftspeed.Actual);
		OLED_Printf(1,30,OLED_6X8,"Right Speed:%.2f ",Rightspeed.Actual);
		OLED_Printf(1,50,OLED_6X8,"%-20s",line);//故障码显示
	}
	else
	{
		OLED_Printf(1,  8, OLED_6X8, "default");
		OLED_Printf(64, 8, OLED_6X8, "circle");

		OLED_Printf(1,  24, OLED_6X8, "square");
		OLED_Printf(64, 24, OLED_6X8, "oval");

		OLED_Printf(1, 48, OLED_6X8, "Temp:%.1f C", (double)DHT11_GetTemperature());
		OLED_Printf(1, 56, OLED_6X8, "RH:%.1f %%",  (double)DHT11_GetHumidity());

		if (Mode == ST_STAIGHT)
		{
			OLED_ReverseArea(1, 8, 6 * 7, 8);
		}
		else if (Mode == ST_CIRCLE)
		{
			OLED_ReverseArea(64, 8, 6 * 6, 8);
		}
		else if (Mode == ST_SQUARE)
		{
			OLED_ReverseArea(1, 24, 6 * 6, 8);
		}
		else if (Mode == ST_OVAL)
		{
			OLED_ReverseArea(64, 24, 6 * 4, 8);
		}
	}
}

void StopAndClear(void)      //停车 + 清控制状态
{
    Start=0;                      
    Set_LeftMotor(0);
    Set_RightMotor(0);

    encoder_last = Encoder_GetLeftCnt();          
    (void)Encoder_GetRightSpeed(&encoder_cnt1);  

    Leftspeed.ErrorInt=0;  Rightspeed.ErrorInt=0;
    Leftspeed.Error0=0;    Leftspeed.Error1=0;    
    Rightspeed.Error0=0;   Rightspeed.Error1=0;

    circle.ErrorInt=0;  circle.Error0=0;  circle.Error1=0;
    square.ErrorInt=0;  square.Error0=0;  square.Error1=0;
    oval.ErrorInt=0;    oval.Error0=0;    oval.Error1=0;

    count1=0;  count3=0;
    line_lost_ms=0;
    DeltaSpeed=0;
	
	
}

void Key_Get(void)
{
	switch (Key_GetNum())
		{
		
		 case 3:
			
				Start=0;		//先清防止卡中间导致清完后start非0
				Set_LeftMotor(0);
				Set_RightMotor(0);
				StopAndClear();
				break;
		 case 1: 
			 if (Start == 0)
				{
					StopAndClear();
					Start = 1;
				}
			break;
		 case 2:
		 		if (Start) StopAndClear(); 
				if(Mode<ST_OVAL)Mode++;
				else Mode=ST_STAIGHT;
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
		DHT11_Update();
		LED_SetTemperature(DHT11_GetTemperature());
        Display();
        OLED_Update();
    }
	if ((uint32_t)(now - last_uart_tick) >= 1000)  //1s一次发送
    {
        last_uart_tick = now;
        UART_SendTemperature();

		if (DHT11_IsOverTemp()) Beep(100); 	//复用1s模块
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//总计时器
{
	
	if(htim->Instance==TIM1)
	{
		Beep_Tick(); 
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
				Dist=(float)(encoder_cnt+encoder_cnt0)/2;
			}
			if(count3>=10)
			{
				count3=0;
				LT_Sensor_Get(Sensor_Data);
				if((Sensor_Data[0]||Sensor_Data[1]||Sensor_Data[2]||Sensor_Data[3]||Sensor_Data[4])==0)
				{
					line_lost_ms += 10;   
					if(line_lost_ms >= 200)//持续200ms才认为脱线
					{
						Start=0;
						Set_LeftMotor(0);
						Set_RightMotor(0);
						line_lost_ms=0;
						Mode=ST_FAULT;
					}
				}
				else{
				line_lost_ms=0;   
				square.Actual=Sensor_Data[0]*(-2)+Sensor_Data[1]*(-1)+Sensor_Data[2]*(0)+Sensor_Data[3]*(1)+Sensor_Data[4]*(2);//权重计算
				oval.Actual=Sensor_Data[0]*(-2)+Sensor_Data[1]*(-1)+Sensor_Data[2]*(0)+Sensor_Data[3]*(1)+Sensor_Data[4]*(2);
				circle.Actual=Sensor_Data[0]*(-2)+Sensor_Data[1]*(-1)+Sensor_Data[2]*(0)+Sensor_Data[3]*(1)+Sensor_Data[4]*(2);
				switch (Mode)							//这里放循迹环
				{
					case ST_STAIGHT:
						DeltaSpeed=0;
						break;
					case ST_CIRCLE:pid_compute(&circle);
					DeltaSpeed=(float)(circle.Out*SPEED_RAGE/circle.OutMax);//因为最大和最小一般是相反数所以我偷懒了
						break;
					case ST_SQUARE:pid_compute(&square);
						DeltaSpeed=(float)(square.Out*SPEED_RAGE/square.OutMax);
						break;
					case ST_OVAL:pid_compute(&oval);
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

