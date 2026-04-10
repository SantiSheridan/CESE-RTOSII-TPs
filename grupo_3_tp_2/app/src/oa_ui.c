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

/********************** internal functions definition ************************/

static void oa_led_msg_callback_(void *pmsg)
{
    LOGGER_LOG("OA_UI: LIBERANDO MEMORIA");
    vPortFree(pmsg);
}

/********************** external functions definition ************************/

void oa_ui_init(){
    oa_ui_h.queue_h = xQueueCreate(QUEUE_LENGTH_OA_UI, QUEUE_ITEM_SIZE_OA_UI);
    if (oa_ui_h.queue_h == NULL) {
        LOGGER_LOG("OA_UI: ERROR AL CREAR LA COLA");
    }
    oa_ui_h.state = UI_STATE_STANDBY;
}

static bool oa_ui_send_led_off_(oa_led_color_t oa_idx)
{
    oa_led_msg_t *pmsg = pvPortMalloc(sizeof(oa_led_msg_t));
    if (pmsg == NULL) return false;
    pmsg->action   = LED_ACTION_OFF;
    pmsg->callback = oa_led_msg_callback_;
    if (!oa_led_send(oa_idx, pmsg)) {
        vPortFree(pmsg); 
        return false;
    }
    return true;
}

void oa_ui_(void){

    button_event_t event;
    oa_led_color_t led = LED_OA__N;
    ui_state_t       state = UI_STATE_STANDBY;

    if (pdPASS != xQueueReceive(oa_ui_h.queue_h, &event, 0)) {
        return; 
    }

    LOGGER_LOG("OA_UI: EVENT RECEIVED");

    // Si el evento recibido es el mismo que el estado actual, no hago nada directamente.
    if ((event == BUTTON_EVENT_PULSE && oa_ui_h.state == UI_STATE_RED)   ||
        (event == BUTTON_EVENT_SHORT && oa_ui_h.state == UI_STATE_GREEN) ||
        (event == BUTTON_EVENT_LONG  && oa_ui_h.state == UI_STATE_BLUE)) {
        return;
    }

    // Apago estado previo
    switch (oa_ui_h.state) {
        case UI_STATE_RED:   oa_ui_send_led_off_(LED_RED_OA);   break;
        case UI_STATE_GREEN: oa_ui_send_led_off_(LED_GREEN_OA); break;
        case UI_STATE_BLUE:  oa_ui_send_led_off_(LED_BLUE_OA);  break;
        default: break;
    }

    oa_led_msg_t *pmsg = (oa_led_msg_t*)pvPortMalloc(sizeof(oa_led_msg_t));
    if (pmsg == NULL) {
        LOGGER_LOG("OA_UI: MEMORIA INSUFICIENTE");
        return;  
    }

    pmsg->action   = LED_ACTION_ON;
    pmsg->callback = oa_led_msg_callback_;

    switch (event) {
        case BUTTON_EVENT_PULSE:
            led   = LED_RED_OA;
            state = UI_STATE_RED;
            break;
        case BUTTON_EVENT_SHORT:
            led   = LED_GREEN_OA;
            state = UI_STATE_GREEN;
            break;
        case BUTTON_EVENT_LONG:
            led   = LED_BLUE_OA;
            state = UI_STATE_BLUE;
            break;
        default:
            vPortFree(pmsg);
            return;
    }
    
    if (oa_led_send(led, pmsg)) {
        oa_ui_h.state = state;
    } else {
        vPortFree(pmsg);
    }
}