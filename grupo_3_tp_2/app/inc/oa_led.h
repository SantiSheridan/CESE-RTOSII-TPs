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

#ifndef OA_LED_H_
#define OA_LED_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/
#define QUEUE_LENGTH 	5
#define LED_RED_TIME 	1000
#define LED_GREEN_TIME 	1000
#define LED_BLUE_TIME 	1000




/********************** typedef **********************************************/
typedef enum{
  	LED_RED_OA,
  	LED_GREEN_OA,
  	LED_BLUE_OA,
  	LED_OA__N,
} oa_led_color_t;

typedef enum{
  	LED_ACTION_GO,
  	LED_ACTION_OFF,
  	LED_ACTION_N,
} oa_led_action_t;

typedef struct {
  	oa_led_color_t    color;
  	GPIO_TypeDef*     port;
  	uint16_t          pin;
  	QueueHandle_t     queue_h;
} oa_led_handle_t;

typedef struct {
	  GPIO_TypeDef*     port;
    uint16_t          pin;
} leds_t;

typedef void (*oa_led_msg_callback_t)(void *pmsg);

typedef struct {
  	oa_led_action_t action;
  	uint32_t        time_ms;
	oa_led_msg_callback_t   callback;
} oa_led_msg_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

void oa_led_init(oa_led_handle_t *oa_led, oa_led_color_t led_color);
void oa_led_(oa_led_handle_t *oa_led);
bool oa_led_send(oa_led_handle_t *oa_led, oa_led_msg_t *pmsg);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* OA_LED_H_ */
/********************** end of file ******************************************/

