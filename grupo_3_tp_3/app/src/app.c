/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "app.h"
#include "task_msg_tick.h"
#include "API_uart.h"
#include "protocol.h"
#include "task_msg_response.h"

/********************** macros and definitions *******************************/
#define UART_QUEUE_RX_SIZE 10
/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration *****************************/
QueueHandle_t uart_rx_queue;
SemaphoreHandle_t uart4_mutex;


/********************** external functions definition ************************/
void app_init(void)
{
	BaseType_t status;

	uart_rx_queue = xQueueCreate(UART_QUEUE_RX_SIZE, sizeof(MsgRequest_t));
    if( uart_rx_queue == NULL )
    {
        LOGGER_LOG("[ERROR] Fail to create uart_rx_queue\n");
    }

	uart4_mutex = xSemaphoreCreateMutex();

	status = xTaskCreate(task_tick, "TASK_MSG_TICK", 1024, NULL, tskIDLE_PRIORITY + 3, NULL);
	if (status != pdPASS) {
		LOGGER_LOG("[ERROR] Fail to create TASK_MSG_TICK\n");
	}
	LOGGER_LOG("TASK_MSG_TICK Started");

	status = xTaskCreate(task_response, "TASK_MSG_RESPONSE", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
	if (status != pdPASS) {
		LOGGER_LOG("[ERROR] Fail to create TASK_MSG_RESPONSE\n");
	}
	LOGGER_LOG("TASK_MSG_RESPONSE Started");

	cycle_counter_init();
}

/********************** end of file ******************************************/
