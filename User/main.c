#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "config.h"
#include "cli.h"
#include "led.h"
#include "key.h"
#include "touch.h"


void hardware_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  // 非常重要。如果是运行FreeRTOS必须要有，并且程序运行中不准变。
}

void tasks_init(void)
{
    vUARTCommandConsoleStart(TASK_CLI_STACKSIZE, TASK_CLI_PRIO);
    xTaskCreate(led_task, TASK_LED_NAME, TASK_LED_STACKSIZE, NULL, TASK_LED_PRIO, &tasks.led);
    xTaskCreate(key_task, TASK_KEY_NAME, TASK_KEY_STACKSIZE, NULL, TASK_KEY_PRIO, &tasks.key);
    xTaskCreate(touch_task, TASK_TOUCH_NAME, TASK_TOUCH_STACKSIZE, NULL, TASK_TOUCH_PRIO, &tasks.touch);
}

int main(void)
{
    hardware_init();
    tasks_init();
    vTaskStartScheduler();
}
