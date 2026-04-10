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
static oa_led_handle_t oa_leds[LED_OA__N] = {
    /*	.color,			.port		.pin			.queue_h */
	{	LED_RED_OA, 	LEDs_PORT, 	LED_RED_PIN, 	NULL	  },
	{	LED_GREEN_OA,	LEDs_PORT, 	LED_GREEN_PIN, 	NULL	  },
	{	LED_BLUE_OA, 	LEDs_PORT, 	LED_BLUE_PIN, 	NULL	  },
};

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/
bool oa_led_send(oa_led_color_t oa_idx, oa_led_msg_t *pmsg)
{
	if (oa_idx >= LED_OA__N)
		return false;

	oa_led_handle_t *oa_led = &oa_leds[oa_idx];
    if (oa_led->queue_h == NULL) {
        oa_led->queue_h = xQueueCreate(QUEUE_LENGTH, sizeof(oa_led_msg_t*));
        if (oa_led->queue_h == NULL) {
            LOGGER_LOG("OA_LED: FALLO CREACION DE QUEUE");
            return false;
        }
        LOGGER_LOG("OA_LED: QUEUE CREADO");
    }

    if (pdPASS != xQueueSend(oa_led->queue_h, &pmsg, 0)) {
        LOGGER_LOG("OA_LED: QUEUE FULL");
        return false;
    }

    return true;
}

void oa_led_(oa_led_color_t oa_idx)
{
	if (oa_idx >= LED_OA__N)
		return;
	 
	oa_led_handle_t *oa_led = &oa_leds[oa_idx];
	
	if (oa_led->queue_h == NULL)
		return;

	oa_led_msg_t *pmsg;
	
	if (pdPASS == xQueueReceive(oa_led->queue_h, &pmsg, 0)){
		LOGGER_LOG("OA_LED_TASK: MSG RECEIVED");

		if (pmsg->action == LED_ACTION_ON){
			HAL_GPIO_WritePin(oa_led->port, oa_led->pin, GPIO_PIN_SET);
		}
		else if (pmsg->action == LED_ACTION_OFF){
			HAL_GPIO_WritePin(oa_led->port, oa_led->pin, GPIO_PIN_RESET);
		}

		if (pmsg->callback != NULL){
			pmsg->callback(pmsg);
		}

		// Genero este nuevo pmsg_extra por recomendacion, para asegurarme de procesar todos los mensajes antes de eliminar la cola. 
		oa_led_msg_t *pmsg_extra;
        while (xQueueReceive(oa_led->queue_h, &pmsg_extra, 0) == pdPASS) {
            if (pmsg_extra->callback != NULL)
                pmsg_extra->callback(pmsg_extra);
        }

		vQueueDelete(oa_led->queue_h);
        oa_led->queue_h = NULL;
	}
}

/********************** end of file ******************************************/
