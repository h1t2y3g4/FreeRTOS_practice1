#include "led.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "config.h"


#define LED_GPIO            GPIOB
#define LED_RED_PIN         GPIO_Pin_5
#define LED_GREEN_PIN       GPIO_Pin_0
#define LED_BLUE_PIN        GPIO_Pin_1

#define LED_RUN_TIMER_CMD_START  1
#define LED_RUN_TIMER_CMD_STOP   0

#define LED_RUN_TIMER_PERIOD    1000

int led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能 PB 端口时钟

    GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;          // PB.5 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // IO 口速度为 50MHz
    GPIO_Init(LED_GPIO, &GPIO_InitStructure);           // 初始化 GPIOB.5
    GPIO_SetBits(LED_GPIO, LED_RED_PIN);                 // PB.5 输出高

    GPIO_InitStructure.GPIO_Pin = LED_GREEN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(LED_GPIO, LED_GREEN_PIN);

    GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(LED_GPIO, LED_BLUE_PIN);

    return 0;
}

static void led_run_timer_handler(TimerHandle_t xTimer)
{
    static int state = TASK_LED_EVENT_RED_ON;
    uint32_t event;
    switch (state)
    {
    case TASK_LED_EVENT_RED_ON:
        event = TASK_LED_EVENT_GREEN_ON;
        xQueueSend(queues.led, (void *)&event, 0);
        state = TASK_LED_EVENT_GREEN_ON;
        break;
    case TASK_LED_EVENT_GREEN_ON:
        event = TASK_LED_EVENT_BLUE_ON;
        xQueueSend(queues.led, (void *)&event, 0);
        state = TASK_LED_EVENT_BLUE_ON;
        break;
    case TASK_LED_EVENT_BLUE_ON:
        event = TASK_LED_EVENT_RED_ON;
        xQueueSend(queues.led, (void *)&event, 0);
        state = TASK_LED_EVENT_RED_ON;
        break;

    default:
        break;
    }
}

int led_run_timer(int cmd)
{
    static xTimerHandle led_run_timer = NULL;
    switch (cmd)
    {
    case LED_RUN_TIMER_CMD_START:
        if (!led_run_timer) {
            led_run_timer = xTimerCreate("led_run_timer", LED_RUN_TIMER_PERIOD, pdTRUE, NULL, led_run_timer_handler);
            if (!led_run_timer) {
                printf("create led_run_timer fail\r\n");
                return -1;
            }
            xTimerStart(led_run_timer, 0);
        }

        xTimerReset(led_run_timer, 0);
        break;
    case LED_RUN_TIMER_CMD_STOP:
        if (led_run_timer) {
            xTimerStop(led_run_timer, 0);
        }
        break;
    
    default:
        break;
    }

    return 0;
}

void led_task(void *pvParameters)
{
    uint32_t event = 0;
    led_init();
    led_run_timer(LED_RUN_TIMER_CMD_START);

    while (1)
    {
        if (xQueueReceive(queues.led, (void *)&event, portMAX_DELAY) != pdTRUE) {
            printf("receive led event fail\r\n");
            continue;
        }
        printf("event: %d\r\n", event);
        
        if (event & TASK_LED_EVENT_OFF)
        {
            printf("TASK_LED_EVENT_OFF\r\n");
            GPIO_SetBits(LED_GPIO, LED_RED_PIN);
            GPIO_SetBits(LED_GPIO, LED_GREEN_PIN);
            GPIO_SetBits(LED_GPIO, LED_BLUE_PIN);
        }

        if (event & TASK_LED_EVENT_RED_ON)
        {
            printf("TASK_LED_EVENT_RED_ON\r\n");
            GPIO_ResetBits(LED_GPIO, LED_RED_PIN);
            GPIO_SetBits(LED_GPIO, LED_GREEN_PIN);
            GPIO_SetBits(LED_GPIO, LED_BLUE_PIN);
        }

        if (event & TASK_LED_EVENT_GREEN_ON)
        {
            printf("TASK_LED_EVENT_GREEN_ON\r\n");
            GPIO_SetBits(LED_GPIO, LED_RED_PIN);
            GPIO_ResetBits(LED_GPIO, LED_GREEN_PIN);
            GPIO_SetBits(LED_GPIO, LED_BLUE_PIN);
        }

        if (event & TASK_LED_EVENT_BLUE_ON)
        {
            printf("TASK_LED_EVENT_BLUE_ON\r\n");
            GPIO_SetBits(LED_GPIO, LED_RED_PIN);
            GPIO_SetBits(LED_GPIO, LED_GREEN_PIN);
            GPIO_ResetBits(LED_GPIO, LED_BLUE_PIN);
        }

        if (event & TASK_LED_EVENT_RUN_ON)
        {
            printf("TASK_LED_EVENT_RUN_ON\r\n");
            led_run_timer(LED_RUN_TIMER_CMD_START);
        }

        if (event & TASK_LED_EVENT_RUN_OFF)
        {
            printf("TASK_LED_EVENT_RUN_OFF\r\n");
            led_run_timer(LED_RUN_TIMER_CMD_STOP);
        }
        
        // vTaskDelay(1000);
        // printf("led_task\r\n");
    }
}

// int send_led_event(uint32_t event)
// {
//     return xQueueSend(queues.led, (void *)&event, 0);
// }

// int send_led_event_from_isr(uint32_t event)
// {
//     xQueueSendFromISR(queues.led, (void *)&event, NULL);
// }

