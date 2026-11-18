# STM32 循迹小车

本项目是基于 STM32F103C8T6 的双电机循迹小车固件，使用 STM32CubeMX 配置外设，以 Keil MDK 编译和下载。当前主要用于联调 OLED、五路数字灰度传感器和左右轮编码器，电机与循迹控制仍在开发中。

## 功能进度

| 模块 | 当前状态 |
| --- | --- |
| OLED | 通过 I²C 初始化并刷新 SSD1306 屏幕，显示灰度通道电平和左右轮计数差，预留模式标签。 |
| 五路灰度传感器 | 读取 `GRAY1` 至 `GRAY5` 的数字电平，数据为 `0` 或 `1`。 |
| 左右轮编码器 | 左轮用 TIM3 硬件编码器模式计数，右轮用 EXTI 双边沿中断解码。显示值是采样窗口内的计数差，采样间隔设定为 5 ms。 |
| 按键与模式 | 已配置启动、模式、停止三个按键。`default`、`circle`、`square`、`oval` 是界面上的模式标签，对应的切换和行驶逻辑待联调。 |
| 电机与 PID | 已配置两路 20 kHz PWM，并添加电机驱动接口和 PID 运算模块。PWM 启动、驱动使能、闭环调速和自动循迹待接入应用层。 |
| 温度与电池电压 | ADC1 已配置两个采样通道和 DMA，采样启动及温度、电压换算待接入。 |

当前联调时，左轮读数恒为 0，因为差分前当前值和上次值被写成了同一个计数。按键消抖计数在每次调用时重置，无法产生按键事件。灰度采样计数在读取后未清零，达到阈值后会在接下来的中断中重复读取，10 ms 采样间隔待修正。相关代码见 [app.c](User/APP/app.c) 和 [bsp_key.c](User/KEY/bsp_key.c)。

## 硬件与接线

主控为 STM32F103C8T6（LQFP48），使用 8 MHz 外部晶振，系统时钟为 72 MHz。下表的引脚和信号来自 [car.ioc](car.ioc) 与 [main.h](Core/Inc/main.h)。

| 模块 | 信号与 MCU 引脚 | 配置 |
| --- | --- | --- |
| 左电机驱动 | `PWMA` → `PA0`，`AIN1` → `PA2`，`AIN2` → `PA3` | TIM2_CH1 |
| 右电机驱动 | `PWMB` → `PA1`，`BIN1` → `PB0`，`BIN2` → `PB1` | TIM2_CH2 |
| 电机驱动使能 | `STBY` → `PA8` | 初始化为低电平 |
| 左轮编码器 | `ENC1_A` → `PA6`，`ENC1_B` → `PA7` | TIM3_CH1/CH2，上拉输入 |
| 右轮编码器 | `ENC2_A` → `PB3`，`ENC2_B` → `PB4` | EXTI3/4，上拉输入，双边沿触发 |
| 五路灰度 | `GRAY1` ～ `GRAY5` → `PB10` ～ `PB14`，`GRAY_EN` → `PB8` | 信号输入上拉，使能脚初始化为高电平 |
| OLED | `SCL` → `PB6`，`SDA` → `PB7` | I²C1，100 kHz，SSD1306 128×64，7 位地址 `0x3C`，需要总线上拉 |
| 按键 | `KEY_START` → `PA11`，`KEY_MODE` → `PA12`，`KEY_STOP` → `PA15` | 上拉输入 |
| 模拟采样 | `NTC_ADC` → `PA4`，`BAT_SENSE` → `PA5` | ADC1_IN4/IN5，DMA1_Channel1 |
| 串口 | `TX` → `PA9`，`RX` → `PA10` | USART1，115200 baud，8N1 |
| LED 与蜂鸣器 | `LED_PC13` → `PC13`，`LED_PWM` → `PB9`，`BUZ` → `PB15` | `LED_PWM` 使用 TIM4_CH4 |
| SWD 调试 | `SWDIO` → `PA13`，`SWCLK` → `PA14` | ST-LINK 与目标板共地 |

### AT8236 电机驱动

AT8236 是单路有刷直流电机驱动器，左、右轮各用一颗。每颗芯片的 `OUT1`、`OUT2` 接同一个电机的两根线，`VM` 接电机电源，驱动板与 STM32 共地。`IN1`、`IN2` 可接 STM32 的 3.3 V 逻辑输出。中科微 AT8236 的 `VM` 推荐范围为 5.5～36 V，实际电源还要符合电机额定电压和驱动模块的规格。

使用裸芯片时，将散热焊盘接地，在 `VM` 附近做电源去耦，并按典型应用电路接 `VREF` 和 `ISEN`。需要限流时，`ISEN` 通过采样电阻接地，限流值按 `I_TRIP = VREF / (10 × R_SENSE)` 估算。拿到成品模块时，按模块丝印和原理图核对输入、电源和使能接线。芯片引脚、工作条件与典型电路见 [中科微 AT8236 数据手册](https://www.lcsc.com/datasheet/C2827823.pdf)。

| 操作 | IN1 | IN2 |
| --- | --- | --- |
| 正转调速 | PWM | 低 |
| 反转调速 | 低 | PWM |
| 滑行停止 | 低 | 低 |
| 刹车 | 高 | 高 |

PWM 占空比决定驱动时间比例，车轮的实际正转方向取决于电机接线与安装。两个输入持续为低约 1 ms 后芯片会休眠。从休眠启动时，至少一路需保持高电平 5 µs，随后约 1 ms 恢复工作。以 20 kHz 调速时，低于 10% 的 PWM 高脉冲短于 5 µs，低速起步时应先满足唤醒条件再切换到目标占空比。时序见 [中科微 AT8236 数据手册](https://www.lcsc.com/datasheet/C2827823.pdf)。

当前 `car.ioc` 的电机接口使用两路 PWM、方向 GPIO 和 `STBY`。AT8236 芯片通过 `IN1`、`IN2` 控制，没有独立的 `STBY` 脚。能重新布线时，可在 CubeMX 保留 TIM2 的 20 kHz 配置，将 `PA2`、`PA3` 改为另外两路 PWM。若电机驱动已焊在 PCB 上，以原理图里的走线为准。

| 驱动 | IN1 | IN2 |
| --- | --- | --- |
| 左轮 AT8236 | `PA0`（TIM2_CH1） | `PA2`（TIM2_CH3） |
| 右轮 AT8236 | `PA1`（TIM2_CH2） | `PA3`（TIM2_CH4） |

固件适配时，四路 PWM 的初始比较值设为 0，在 `App_Init()` 中用 `HAL_TIM_PWM_Start()` 逐路启动。修改 [bsp_motor.c](User/MOTOR/bsp_motor.c)，让每个轮子的有符号目标值选择一路输出，另一路保持为 0。例如 TIM2 的 `ARR = 3599` 时，左轮正转 25% 可设置 `CCR1 = 900`、`CCR3 = 0`，反转则交换这两个值。换向前先把两路置零，待电机减速后再施加反向 PWM。首次通电时架空车轮并逐个验证方向。本工程的 AT8236 接线方案和驱动逻辑仍待上板验证。

## 编译与烧录

1. 安装 Keil MDK-ARM、Arm Compiler 5.06 update 5（build 528）和 `Keil.STM32F1xx_DFP` 2.2.0。HAL 与 CMSIS 已包含在仓库的 `Drivers/` 中。
2. 打开 [MDK-ARM/car.uvprojx](MDK-ARM/car.uvprojx)，选择目标 `car`，执行 **Project → Rebuild all target files**。编译成功后生成 `MDK-ARM/car/car.axf` 和 `MDK-ARM/car/car.hex`。
3. 给目标板供电，将 ST-LINK 接到目标板的 `SWDIO`、`SWCLK` 和 `GND`。在 **Options for Target → Debug** 中选择 `ST-Link Debugger`，在 **Settings** 中确认调试接口为 `SW`。
4. 执行 **Flash → Download**，下载后复位目标板。连接 OLED 时，可用屏幕上的灰度通道和轮速读数检查外设输入，左轮与按键的当前限制见上文。

## 代码结构

| 路径 | 内容 |
| --- | --- |
| [car.ioc](car.ioc) | CubeMX 的引脚、时钟和外设配置 |
| [Core/](Core/) | 主程序、外设初始化、中断入口 |
| [Drivers/](Drivers/) | STM32F1 HAL 与 CMSIS |
| [User/APP/](User/APP/) | 应用初始化、定时回调、编码器中断处理和显示 |
| [User/ENCODER/](User/ENCODER/)、[User/Line Tracking Sensor/](User/Line%20Tracking%20Sensor/) | 编码器计数读取与五路灰度采样 |
| [User/OLED/](User/OLED/)、[User/KEY/](User/KEY/) | OLED 驱动与按键状态处理 |
| [User/MOTOR/](User/MOTOR/)、[User/PID/](User/PID/)、[User/NTC/](User/NTC/) | 电机接口、PID 运算与 NTC 模块预留 |
| [MDK-ARM/](MDK-ARM/) | Keil 工程与启动文件 |

[main.c](Core/Src/main.c) 完成外设初始化后调用 `App_Init()`，随后在主循环中调用 `App_Task()`。当前应用使用 TIM1 的 1 ms 更新中断调用按键扫描、编码器采样和灰度读取，并每累计 200 次更新回调刷新 OLED。`App_Task()` 预留给主循环任务。

## 修改外设配置

1. 用 STM32CubeMX 打开 [car.ioc](car.ioc)。仓库记录的生成环境为 CubeMX 6.18.1 和 STM32Cube FW_F1 V1.8.7，目标工具链为 MDK-ARM。
2. 保持 **Keep User Code when re-generating** 启用。在生成文件中把自定义代码放在 `USER CODE BEGIN/END` 区域内，应用与模块代码放在 `User/` 下。外设初始化统一由生成的 `adc.c`、`tim.c` 等文件提供，避免与旧模板的同类初始化文件重复。
3. 重新生成后检查 Keil 的文件分组、`User/` 源文件和头文件搜索路径，再重新编译 `car` 目标。

本机迁移前的工程备份位于 `D:\program\stm\stmproject\car_backup_20261116_144716`，其中 `car_legacy` 是旧模板工程，`temption_original` 是原 CubeMX 工程。
