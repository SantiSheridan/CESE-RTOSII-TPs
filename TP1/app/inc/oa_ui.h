#ifndef __OA_UI_H
#define __OA_UI_H

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"

/********************** macros ***********************************************/
#define QUEUE_LENGTH_OA_UI        (10)
#define QUEUE_ITEM_SIZE_OA_UI     (sizeof(button_event_t))

/********************** typedef **********************************************/
typedef struct {
    QueueHandle_t queue_h;
} oa_ui_handle_t;

/********************** external data declaration ****************************/
extern oa_ui_handle_t oa_ui_h;

/********************** external functions declaration ***********************/
void oa_ui_init();
void oa_ui_task(void *args);
/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif


#endif /* __OA_UI_H */