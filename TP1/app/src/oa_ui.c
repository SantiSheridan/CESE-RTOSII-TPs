#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "oa_ui.h"
#include "oa_led.h"
#include "task_button.h"
#include "app.h"

oa_ui_handle_t oa_ui_h;
extern oa_led_handle_t led_red, led_green, led_blue;

void oa_ui_init(){
    oa_ui_h.queue_h = xQueueCreate(QUEUE_LENGTH_OA_UI, QUEUE_ITEM_SIZE_OA_UI);

    BaseType_t status = xTaskCreate(oa_ui_task, "TASK_UI", 128, &oa_ui_h, tskIDLE_PRIORITY, NULL);
    if (status != pdPASS) {
        LOGGER_LOG("FAIL OA UI TASK CREATE");
    }
}

void oa_ui_task(void *args){
    oa_ui_handle_t *oa_ui_h = (oa_ui_handle_t *)args;
    button_event_t event;
    while(true) {
        if (xQueueReceive(oa_ui_h->queue_h, &event, portMAX_DELAY)) {
            LOGGER_LOG("OA_UI: QUEUE RECEIVED");
            oa_led_msg_t msg;
            msg.action  = LED_ACTION_GO;
            switch (event) {
                case BUTTON_EVENT_PULSE:
                    msg.time_ms =  LED_RED_TIME;
                    xQueueSend(led_red.queue_h, &msg, portMAX_DELAY);
                    break;
                case BUTTON_EVENT_SHORT:
                    msg.time_ms =  LED_GREEN_TIME;
                    xQueueSend(led_green.queue_h, &msg, portMAX_DELAY);
                    break;
                case BUTTON_EVENT_LONG:
                    msg.time_ms =  LED_BLUE_TIME;
                    xQueueSend(led_blue.queue_h, &msg, portMAX_DELAY);
                    break;
                default:
                    break;
            }
        }
    }

}