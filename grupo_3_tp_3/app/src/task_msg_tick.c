
/********************** inclusions *******************************************/
#include "FreeRTOS.h"
#include "task_msg_tick.h"
#include "task.h"
#include "queue.h"
#include "app.h"
#include "cmsis_os.h"
#include "protocol.h"
#include "API_uart.h"
#include "cmsis_os.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/
extern SemaphoreHandle_t uart4_mutex;
volatile TickType_t last_tick_msg = 0;
/********************** external functions definition ************************/

    void task_tick(void *arg) {
    TickType_t LastTick = xTaskGetTickCount();
    MsgTick_t msg_tick;
    char buffer_tx[32];
    uint32_t time_s;

    for (;;) {
        /* Esperamos Hasta que pasen 100ms */
        vTaskDelayUntil(&LastTick, pdMS_TO_TICKS(100));
        
        xSemaphoreTake(uart4_mutex, portMAX_DELAY);
        /* Generamos el msj tick con el timestamp en ms actual */
        time_s = xTaskGetTickCount() * portTICK_PERIOD_MS;

        msg_tick_create(&msg_tick, time_s);
        msg_tick_write(buffer_tx, &msg_tick);
        
        /* Transmitimos por la UART */
        uart_transmit_secure((uint8_t*) buffer_tx);
        last_tick_msg = xTaskGetTickCount();
        xSemaphoreGive(uart4_mutex);
    }
}
/********************** end of file ******************************************/