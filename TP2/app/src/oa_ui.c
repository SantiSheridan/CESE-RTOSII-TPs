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

/********************** internal data definition *****************************/
oa_ui_handle_t oa_ui_h;
extern oa_led_handle_t led_red, led_green, led_blue;


/********************** internal functions definition ************************/

static void oa_led_msg_callback_(void *pmsg)
{
    LOGGER_LOG("OA_UI: LIBERANDO MEMORIA");
    vPortFree(pmsg);
}

/********************** external functions definition ************************/

void oa_ui_init(){
    oa_ui_h.queue_h = xQueueCreate(QUEUE_LENGTH_OA_UI, QUEUE_ITEM_SIZE_OA_UI);
}

void oa_ui_(void){

    button_event_t event;

    if (pdPASS != xQueueReceive(oa_ui_h.queue_h, &event, 0)) {
        return; 
    }

    LOGGER_LOG("OA_UI: EVENT RECEIVED");

    oa_led_msg_t *pmsg = (oa_led_msg_t*)pvPortMalloc(sizeof(oa_led_msg_t));
    if (pmsg == NULL) {
        LOGGER_LOG("OA_UI: MEMORIA INSUFICIENTE");
        return;  
    }

    pmsg->action   = LED_ACTION_GO;
    pmsg->callback = oa_led_msg_callback_;

    switch (event) {
        case BUTTON_EVENT_PULSE:
            pmsg->time_ms = LED_RED_TIME;
            oa_led_send(&led_red, pmsg); 
            break;
        case BUTTON_EVENT_SHORT:
            pmsg->time_ms = LED_GREEN_TIME;
            oa_led_send(&led_green, pmsg);
            break;
        case BUTTON_EVENT_LONG:
            pmsg->time_ms = LED_BLUE_TIME;
            oa_led_send(&led_blue, pmsg);
            break;
        default:
            vPortFree(pmsg);
            break;
    }
}