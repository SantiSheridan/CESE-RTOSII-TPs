/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    API_uart.c
  * @brief   This file provides code for the configuration
  *          of the UART.
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
/* Includes ------------------------------------------------------------------*/
#include "app.h"
#include "API_uart.h"
#include "protocol.h"
#include "main.h"
#include "cmsis_os.h"
#include "logger.h"

extern QueueHandle_t uart_rx_queue;
extern SemaphoreHandle_t uart4_mutex;

static UART_HandleTypeDef huart4;
static uint8_t byte_rx;
static char buffer_rx[UART_MSG_RX_MAX_LEN] = {0};

ProtocolParser_t protocol_parser;

bool uart_init(void)
{
    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;

    // Si la inicialización de la UART falla, retorna false.
    if (HAL_UART_Init(&huart4) != HAL_OK)
    	return false;

    HAL_UART_MspInit(&huart4);
    HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

	protocol_parser_init(&protocol_parser, buffer_rx, UART_MSG_RX_MAX_LEN);
	HAL_UART_Receive_IT(&huart4, &byte_rx, 1);
    return true;
}

bool uart_deinit(void)
{
  // Si la desinicialización de la UART falla, retorna false.
	if (HAL_UART_DeInit(&huart4) != HAL_OK)
	{
		return false;
	}
	return true;
}

void UART4_IRQHandler(void) {
  	HAL_UART_IRQHandler(&huart4);
}

void notify_msg_response(ProtocolParser_t *protocol_parser) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	MsgRequest_t msg_request;

	msg_request_parser(protocol_parser->buffer, &msg_request);
	xQueueSendToBackFromISR(uart_rx_queue, &msg_request, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == UART4) {
		ProtocolParserState_t status = protocol_parser_feed(&protocol_parser, (char) byte_rx, notify_msg_response);
		if (status == PROTOCOL_PARSER_STATE_OVERFLOW || status == PROTOCOL_PARSER_STATE_END ) {
			protocol_parser_init(&protocol_parser, buffer_rx, UART_MSG_RX_MAX_LEN);
		}
		
		HAL_UART_Receive_IT(&huart4, &byte_rx, 1);
	}
}

void uart_transmit_secure(uint8_t *pData)
{
    xSemaphoreTake(uart4_mutex, portMAX_DELAY);
    HAL_UART_Transmit(&huart4, pData, strlen((char*) pData), HAL_MAX_DELAY);
    xSemaphoreGive(uart4_mutex);
}