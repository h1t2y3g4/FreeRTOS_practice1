#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TASK_CLI_STACKSIZE          (512)  // 堆栈大小是512个sizeof(StackType_t)的大小，实际占用的就是512*4=2048字节。
#define TASK_CLI_PRIO               1

#define TASK_LED_NAME               "led"
#define TASK_LED_STACKSIZE          (256)  // 堆栈大小是256个sizeof(StackType_t)的大小，实际占用的就是256*4=1024字节。
#define TASK_LED_PRIO               2

#define TASK_KEY_NAME               "key"
#define TASK_KEY_STACKSIZE          (256)  // 堆栈大小是256个sizeof(StackType_t)的大小，实际占用的就是256*4=1024字节。
#define TASK_KEY_PRIO               3

#define TASK_TOUCH_NAME             "touch"
#define TASK_TOUCH_STACKSIZE        (256)  // 堆栈大小是256个sizeof(StackType_t)的大小，实际占用的就是256*4=1024字节。
#define TASK_TOUCH_PRIO             4

typedef struct
{
    TaskHandle_t led;
    TaskHandle_t key;
    TaskHandle_t touch;
} task_handle_t;

extern task_handle_t tasks;

#endif
