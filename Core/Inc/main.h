/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define GPIO_ENTER_Pin GPIO_PIN_2
#define GPIO_ENTER_GPIO_Port GPIOE
#define GPIO_ENTER_EXTI_IRQn EXTI2_IRQn
#define GPIO_EXIT_Pin GPIO_PIN_3
#define GPIO_EXIT_GPIO_Port GPIOE
#define GPIO_EXIT_EXTI_IRQn EXTI3_IRQn
#define GPIO_CS2_Pin GPIO_PIN_0
#define GPIO_CS2_GPIO_Port GPIOA
#define GPIO_DC1_Pin GPIO_PIN_4
#define GPIO_DC1_GPIO_Port GPIOC
#define GPIO_RESET_Pin GPIO_PIN_5
#define GPIO_RESET_GPIO_Port GPIOC
#define GPIO_CS1_Pin GPIO_PIN_1
#define GPIO_CS1_GPIO_Port GPIOB
#define GPIO_UART_EN_Pin GPIO_PIN_15
#define GPIO_UART_EN_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define ST7735_RES_Pin         GPIO_PIN_5
#define ST7735_RES_GPIO_Port   GPIOC

#define ST7735_DC_Pin          GPIO_PIN_4
#define ST7735_DC_GPIO_Port    GPIOC

#define ST7735_CS_Pin          GPIO_PIN_1
#define ST7735_CS_GPIO_Port    GPIOB

#define ST7736_BL_Pin          GPIO_PIN_0
#define ST7736_BL_GPIO_Port    GPIOB
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
