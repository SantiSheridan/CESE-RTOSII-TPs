/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    task_msg_response.h
  * @brief   Prototypes for the message response task handler.
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
#ifndef __TASK_MSG_RESPONSE_H__
#define __TASK_MSG_RESPONSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Funciones públicas --------------------------------------------------------*/
/**
 * @brief Task que procesa y responde mensajes recibidos por UART.
 * @param arg Puntero al argumento pasado por el scheduler de FreeRTOS.
 */
void task_response(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_MSG_RESPONSE_H__ */