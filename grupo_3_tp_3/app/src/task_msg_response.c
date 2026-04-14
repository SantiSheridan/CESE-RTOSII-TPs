/********************** inclusions *******************************************/
#include "task_msg_tick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "app.h"
#include "protocol.h"
#include "API_uart.h"

/********************** macros and definitions *******************************/
#define MSG_REQUEST_BUFFER_SIZE 32

typedef struct {
    MsgRequest_t data[MSG_REQUEST_BUFFER_SIZE];
    uint32_t    head;
    uint32_t    tail;
    uint32_t    count;
} MsgBuffer_t;

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
static MsgBuffer_t msg_buffer = {0};
/********************** external data declaration ****************************/
extern QueueHandle_t uart_rx_queue;
/********************** internal functions definition ************************/

static uint32_t load_queue_into_buffer(void) {
    MsgRequest_t msg;
    uint32_t added = 0;

    /* Cargamos los elementos de la queue en el ring buffer, eliminando los mas viejos */
    while (xQueueReceive(uart_rx_queue, &msg, 0) == pdPASS){

        if (msg_buffer.count == MSG_REQUEST_BUFFER_SIZE)
        {
            msg_buffer.head = (msg_buffer.head + 1) % MSG_REQUEST_BUFFER_SIZE;
            msg_buffer.count--;
        }

        msg_buffer.data[msg_buffer.tail] = msg;
        msg_buffer.tail = (msg_buffer.tail + 1) % MSG_REQUEST_BUFFER_SIZE;
        msg_buffer.count++;
        added++;
    }

    return added;
}

static bool pop_msg_from_buffer(MsgRequest_t *msg_out)
{
    if (msg_buffer.count == 0 || msg_out == NULL) {
        return false;
    }

    /* Buscamos msg_request con maxima prioridad en el ring buffer */
    uint32_t first_idx = msg_buffer.head % MSG_REQUEST_BUFFER_SIZE;
    uint32_t max_priority = msg_buffer.data[first_idx].priority;

    for (uint32_t i = 0; i <msg_buffer.count; i++) {
        uint32_t idx = (msg_buffer.head + i) % MSG_REQUEST_BUFFER_SIZE;

        if (msg_buffer.data[idx].priority > max_priority)
            max_priority = msg_buffer.data[idx].priority;
    }


    /* Obtenemos el msg_request de max prioridad y mas antiguo, luego lo cargamos en msg_out */
    int32_t best_logical = -1;
    uint32_t best_slot = 0;

    for (uint32_t i = 0; i < (msg_buffer.count); i++) {
        uint32_t idx = (msg_buffer.head + i) % MSG_REQUEST_BUFFER_SIZE;

        if (msg_buffer.data[idx].priority == max_priority)
        {
            best_logical = (int32_t)i;
            best_slot    = idx;
            break;
        }
    }
    *msg_out = msg_buffer.data[best_slot];

    /* Ordenamos ring buffer teniendo en cuenta que el elemento a eliminar puede estar en medio */
    for (int32_t i = best_logical; i > 0; i--) {
        uint32_t curr = (msg_buffer.head + i ) % MSG_REQUEST_BUFFER_SIZE;
        uint32_t prev = (msg_buffer.head + i - 1) % MSG_REQUEST_BUFFER_SIZE;
        msg_buffer.data[curr] = msg_buffer.data[prev];
    }

    msg_buffer.head = (msg_buffer.head + 1) % MSG_REQUEST_BUFFER_SIZE;
    msg_buffer.count--;

    return true;
}

/********************** external functions definition ************************/
void task_response(void *arg) {
    MsgRequest_t msg_request;
    MsgResponse_t msg_response;
    char buffer_tx[320];
    
    while(true) {
        /* Cargamos todos los elementos disponibles en la queue dentro del ring buffer */
        uint32_t added = load_queue_into_buffer();

        /* Sacamos el msg_request de mas alta prioridad y mas antiguo */
        bool status = pop_msg_from_buffer(&msg_request);

        if (status) {
            /* creamos el msg_response */
            msg_response_create(&msg_response, msg_request.id);
            msg_response_write(buffer_tx, &msg_response);

            /* Transmitimos por la UART */
            uart_transmit_secure((uint8_t*) buffer_tx);
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }

    }
}
/********************** end of file ******************************************/