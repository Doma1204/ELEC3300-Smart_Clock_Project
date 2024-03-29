/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define PIR_Pin GPIO_PIN_2
#define PIR_GPIO_Port GPIOA
#define PIR_EXTI_IRQn EXTI2_IRQn
#define TFT_PEN_Pin GPIO_PIN_4
#define TFT_PEN_GPIO_Port GPIOA
#define TFT_PEN_EXTI_IRQn EXTI4_IRQn
#define TFT_CLK_Pin GPIO_PIN_5
#define TFT_CLK_GPIO_Port GPIOA
#define TFT_MISO_Pin GPIO_PIN_6
#define TFT_MISO_GPIO_Port GPIOA
#define TFT_MOSI_Pin GPIO_PIN_7
#define TFT_MOSI_GPIO_Port GPIOA
#define TFT_RES_Pin GPIO_PIN_4
#define TFT_RES_GPIO_Port GPIOC
#define TFT_DC_Pin GPIO_PIN_5
#define TFT_DC_GPIO_Port GPIOC
#define TFT_BLK_Pin GPIO_PIN_0
#define TFT_BLK_GPIO_Port GPIOB
#define TFT_CS1_Pin GPIO_PIN_1
#define TFT_CS1_GPIO_Port GPIOB
#define TFT_CS2_Pin GPIO_PIN_2
#define TFT_CS2_GPIO_Port GPIOB
#define RGB_SCL_Pin GPIO_PIN_10
#define RGB_SCL_GPIO_Port GPIOB
#define RGB_SDA_Pin GPIO_PIN_11
#define RGB_SDA_GPIO_Port GPIOB
#define RGB_GPIO2_Pin GPIO_PIN_9
#define RGB_GPIO2_GPIO_Port GPIOD
#define RGB_GPIO2_EXTI_IRQn EXTI9_5_IRQn
#define LED1_Pin GPIO_PIN_3
#define LED1_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_4
#define LED2_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_5
#define LED3_GPIO_Port GPIOD
#define BTN1_Pin GPIO_PIN_6
#define BTN1_GPIO_Port GPIOD
#define BTN2_Pin GPIO_PIN_7
#define BTN2_GPIO_Port GPIOD
#define BTN3_Pin GPIO_PIN_9
#define BTN3_GPIO_Port GPIOG
#define ESP_TX_Pin GPIO_PIN_6
#define ESP_TX_GPIO_Port GPIOB
#define ESP_RX_Pin GPIO_PIN_7
#define ESP_RX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
