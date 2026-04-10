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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "task_button.h"
#include "oa_ui.h"
#include "app.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         			(10)

#define BUTTON_PERIOD_MS         			(TASK_PERIOD_MS_)
#define BUTTON_EVENT_PULSE_TIMEOUT     		(200)
#define BUTTON_EVENT_SHORT_TIMEOUT     		(1000)
#define BUTTON_EVENT_LONG_TIMEOUT      		(2000)


/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern oa_ui_handle_t oa_ui_h;

/********************** internal functions definition ************************/
static void send_event_(button_event_t event)
{
    if (xQueueSend(oa_ui_h.queue_h, &event, 0) == pdPASS) {
        xTaskNotifyGive(task_aos_handle);
    } else {
        LOGGER_LOG("TASK_BUTTON: QUEUE FULL");
    }
}

/********************** external functions definition ************************/

/* 1.  Detectar estados
 * a.Pulso -> 200 y 1000 ms   --> enciende led ROJO  (D3 --> CH0)
 * b.Corto -> 1000 y 2000 ms  --> enciende led VERDE (D4 --> CH2)
 * c.Largo -> >2000ms		      --> enciende led AZUL  (D5 --> CH4)
 * d. No presionado cualquier otro caso --> NINGUNO
 * 
 * */
void task_button(void* arg)
{
	TickType_t time_s = 0;

  	while(true) {
		GPIO_PinState button_state = HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN);

		if (button_state == GPIO_PIN_RESET) {
			if (time_s == 0) {
				time_s = xTaskGetTickCount();
			}
		} else {
			if (time_s != 0) {
				TickType_t time = xTaskGetTickCount() - time_s;
				time_s = 0;

				if (time >= pdMS_TO_TICKS(BUTTON_EVENT_PULSE_TIMEOUT) && time < pdMS_TO_TICKS(BUTTON_EVENT_SHORT_TIMEOUT)){
					LOGGER_LOG("TASK_BUTTON: SEND EVENT PULSE");
					send_event_(BUTTON_EVENT_PULSE);
				} else if (time >= pdMS_TO_TICKS(BUTTON_EVENT_SHORT_TIMEOUT) && time < pdMS_TO_TICKS(BUTTON_EVENT_LONG_TIMEOUT)){
					LOGGER_LOG("TASK_BUTTON: SEND EVENT SHORT");
					send_event_(BUTTON_EVENT_SHORT);
				} else if (time >= pdMS_TO_TICKS(BUTTON_EVENT_LONG_TIMEOUT)){
					LOGGER_LOG("TASK_BUTTON: SEND EVENT LONG");
					send_event_(BUTTON_EVENT_LONG);
				}
			}
		}
		vTaskDelay(pdMS_TO_TICKS(BUTTON_PERIOD_MS));
  	}
}

/********************** end of file ******************************************/
