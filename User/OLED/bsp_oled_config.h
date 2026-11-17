/* HAL 硬件 I2C OLED 配置：默认 SSD1306，128x64，UTF-8。 */
#ifndef BSP_OLED_CONFIG_H
#define BSP_OLED_CONFIG_H

/* CubeMX 生成的头文件，声明 hi2c1。换工程时修改这里和句柄宏。 */
#include "i2c.h"

#define OLED_I2C_HANDLE           hi2c1
#define OLED_I2C_ADDRESS          0x3CU   /* 7 位地址；不要填 0x78 */
#define OLED_I2C_TIMEOUT_MS       50U     /* 单次传输超时 */
#define OLED_POWER_ON_DELAY_MS   100U    /* 仅初始化时等待屏幕上电 */
#define OLED_COLUMN_OFFSET       0U
#define OLED_CONTRAST            0xCFU

/* 本模块不配置 GPIO、时钟或 I2C，必须先调用 MX_I2C1_Init()。
 * 当前 car 接线：PB6=SCL，PB7=SDA；引脚由 CubeMX 管理。
 * 默认仅针对 SSD1306 128x64。SH1106 等控制器须另核对初始化指令，
 * 不能保证仅把列偏移改成 2 就兼容。
 */
#endif /* BSP_OLED_CONFIG_H */
