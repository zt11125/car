/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DHT11_Pin GPIO_PIN_13
#define DHT11_GPIO_Port GPIOC
#define PWMA_Pin GPIO_PIN_0
#define PWMA_GPIO_Port GPIOA
#define PWMB_Pin GPIO_PIN_1
#define PWMB_GPIO_Port GPIOA
#define AIN1_Pin GPIO_PIN_2
#define AIN1_GPIO_Port GPIOA
#define AIN2_Pin GPIO_PIN_3
#define AIN2_GPIO_Port GPIOA
#define BAT_SENSE_Pin GPIO_PIN_5
#define BAT_SENSE_GPIO_Port GPIOA
#define ENC1_A_Pin GPIO_PIN_6
#define ENC1_A_GPIO_Port GPIOA
#define ENC1_B_Pin GPIO_PIN_7
#define ENC1_B_GPIO_Port GPIOA
#define BIN1_Pin GPIO_PIN_0
#define BIN1_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_1
#define BIN2_GPIO_Port GPIOB
#define GRAY1_Pin GPIO_PIN_10
#define GRAY1_GPIO_Port GPIOB
#define GRAY2_Pin GPIO_PIN_11
#define GRAY2_GPIO_Port GPIOB
#define GRAY3_Pin GPIO_PIN_12
#define GRAY3_GPIO_Port GPIOB
#define GRAY4_Pin GPIO_PIN_13
#define GRAY4_GPIO_Port GPIOB
#define GRAY5_Pin GPIO_PIN_14
#define GRAY5_GPIO_Port GPIOB
#define BUZ_Pin GPIO_PIN_15
#define BUZ_GPIO_Port GPIOB
#define STBY_Pin GPIO_PIN_8
#define STBY_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_9
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_10
#define UART_RX_GPIO_Port GPIOA
#define KEY_START_Pin GPIO_PIN_11
#define KEY_START_GPIO_Port GPIOA
#define KEY_START_EXTI_IRQn EXTI15_10_IRQn
#define KEY_MODE_Pin GPIO_PIN_12
#define KEY_MODE_GPIO_Port GPIOA
#define KEY_MODE_EXTI_IRQn EXTI15_10_IRQn
#define KEY_STOP_Pin GPIO_PIN_15
#define KEY_STOP_GPIO_Port GPIOA
#define KEY_STOP_EXTI_IRQn EXTI15_10_IRQn
#define ENC2_A_Pin GPIO_PIN_3
#define ENC2_A_GPIO_Port GPIOB
#define ENC2_A_EXTI_IRQn EXTI3_IRQn
#define ENC2_B_Pin GPIO_PIN_4
#define ENC2_B_GPIO_Port GPIOB
#define ENC2_B_EXTI_IRQn EXTI4_IRQn
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB
#define GRAY_EN_Pin GPIO_PIN_8
#define GRAY_EN_GPIO_Port GPIOB
#define LED_PWM_Pin GPIO_PIN_9
#define LED_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
