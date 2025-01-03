#ifndef _LED_H_
#define _LED_H_

#define TASK_LED_EVENT_OFF      (1 << 0)
#define TASK_LED_EVENT_RED_ON   (1 << 1)
#define TASK_LED_EVENT_GREEN_ON (1 << 2)
#define TASK_LED_EVENT_BLUE_ON  (1 << 3)
#define TASK_LED_EVENT_RUN_ON   (1 << 4)
#define TASK_LED_EVENT_RUN_OFF  (1 << 5)

void led_task(void *pvParameters);

#endif
