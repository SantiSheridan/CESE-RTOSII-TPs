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

#include "task.h"

#include "oa_led.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
static const leds_t leds[LED_OA__N] = 
{
	{LEDs_PORT, LED_ROJO_PIN},  // LED_RED
	{LEDs_PORT, LED_VERDE_PIN},   // LED_GREEN
	{LEDs_PORT, LED_AZUL_PIN},   // LED_BLUE
};

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void oa_led_init(oa_led_handle_t *oa_led, oa_led_color_t led_color, const char *TaskName)
{
	oa_led->color = led_color;
	oa_led->port = leds[led_color].port;
	oa_led->pin  = leds[led_color].pin;
	oa_led->queue_h = xQueueCreate(QUEUE_LENGTH, sizeof(oa_led_msg_t));
	BaseType_t status = xTaskCreate(oa_led_task, TaskName, 128, oa_led, tskIDLE_PRIORITY, NULL);
	if (status != pdPASS) {
    	LOGGER_LOG("FAIL LED INIT");
  	}
}

void oa_led_task(void *arg)
{
	oa_led_handle_t *oa_led = (oa_led_handle_t *)arg;
	oa_led_msg_t msg;
	LOGGER_LOG("OA_LED_TASK: TASK STARTED");

	while(1){
		if (pdPASS == xQueueReceive(oa_led->queue_h, &msg, portMAX_DELAY)){
			LOGGER_LOG("OA_LED_TASK: QUEUE RECEIVED");
			if (msg.action == LED_ACTION_GO){
				HAL_GPIO_WritePin(oa_led->port, oa_led->pin, GPIO_PIN_SET);
				vTaskDelay(pdMS_TO_TICKS(msg.time_ms));
				HAL_GPIO_WritePin(oa_led->port, oa_led->pin, GPIO_PIN_RESET);
			}
			else if (msg.action == LED_ACTION_OFF){
				HAL_GPIO_WritePin(oa_led->port, oa_led->pin, GPIO_PIN_RESET);
			}
		}
	}
}

/********************** end of file ******************************************/
