/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    API_uart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __API_UART_H__
#define __API_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Macros y constantes -------------------------------------------------------*/

// Tamaño máximo del buffer para enviar datos por UART
#define UART_MSG_RX_MAX_LEN 64

/* Funciones públicas --------------------------------------------------------*/
/**
 * @brief Inicializa la UART.
 * @return true si se inicializó bien, false si hubo algún problema en la configuracion.
 */
bool uart_init();

/**
 * @brief Desinicializa la UART.
 * @return true si se finalizo correctamente, false si hubo algún problema.
 */
bool uart_deinit();

/**
 * @brief  Transmite una cadena de texto por UART4
 * @note   Bloquea hasta obtener el mutex. Solo válido para datos terminados en '\0'.
 * @param  pData Puntero a la cadena a transmitir.
 */

void uart_transmit_secure(uint8_t *pData);

void task_uart(void *arg);
#ifdef __cplusplus
}
#endif

#endif /* __API_UART_H__ */

