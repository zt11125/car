# car 小车工程

本工程以 STM32CubeMX 生成的工程为基础，芯片为 STM32F103C8Tx。

- 修改硬件配置：打开根目录的 `car.ioc`。
- 编译和下载：打开 `MDK-ARM/car.uvprojx`。
- 开发环境：Keil MDK，ARM Compiler 5.06 update 5。
- `Core`：MX 管理的引脚、时钟、外设初始化和中断。
- `Drivers`：MX 管理的 HAL 和 CMSIS。
- `User/APP`：应用入口 `App_Init()` 和循环任务 `App_Task()`。
- `User/PID`：保留原 PID 模块文件。
- `User/Line Tracking Sensor`：保留原循迹传感器模块文件。

`main.c` 在全部外设初始化后调用 `App_Init()`，在主循环中调用 `App_Task()`。目前这两个函数仅提供开发入口，尚未添加电机控制等业务逻辑。

后续修改引脚或外设，使用 `car.ioc` 重新生成。保持启用 `Keep User Code when re-generating`。在生成文件中，只在原有的 `USER CODE BEGIN/END` 区域填写自己的代码；主要业务代码放在独立的 `User` 文件中。

外设生成文件继续使用 `adc.c/adc.h`、`tim.c/tim.h` 等原名。不要再加入旧模板的 `bsp_adc.c`、`bsp_tim.c` 等重复初始化文件。

迁移前的两份原始工程已备份到：`D:\program\stm\stmproject\car_backup_20261116_144716`。其中 `car_legacy` 为旧模板工程，`temption_original` 为原 MX 工程。
