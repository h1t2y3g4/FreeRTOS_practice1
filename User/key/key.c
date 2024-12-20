#include "key.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define USE_PC13_AS_KEY 1

#define KEY_QUEUE_SIZE 10
QueueHandle_t keyQueue;

#define KEY_TASK_EVENT_CLICK_KEY1       1
#define KEY_TASK_EVENT_CLICK_KEY2       2

void key_init(void)
{
    /* 
    PA0 - PA7 分别对应 EXTI0 - EXTI7，这些映射是芯片硬件默认设置好的，不需要通过 AFIO 进行重新映射。

    虽然 PC13 的默认功能是普通 IO 口，但当将其配置为外部中断时，涉及到外部中断线的配置。在 STM32 中，外部中断线的配置与 AFIO（Alternate - Function I/O）相关。
    即使 PC13 本身作为普通 IO 口的基本功能不需要 AFIO，但将其作为外部中断输入时，需要通过 AFIO 来配置外部中断线的映射和触发方式等。
     */
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能 GPIOA 和 GPIOC 时钟
#if USE_PC13_AS_KEY
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
#else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
#endif

    // 配置 PA0 为输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#if USE_PC13_AS_KEY
    // 配置 PC13 为输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 使能 AFIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // 配置 EXTI 线
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
#endif

    // 配置外部中断线 0 和 13
    EXTI_ClearITPendingBit(EXTI_Line0);
#if USE_PC13_AS_KEY
    EXTI_ClearITPendingBit(EXTI_Line13);
#endif

#if USE_PC13_AS_KEY
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line13;
#else
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
#endif
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#if USE_PC13_AS_KEY
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

void key_task(void *pvParameters)
{
    uint32_t keyEvent;
    key_init();
    keyQueue = xQueueCreate(KEY_QUEUE_SIZE, sizeof(uint32_t));
    if (keyQueue == NULL) {
        // 队列创建失败，处理错误
        while (1) {}
    }

    while (1)
    {
        if (xQueueReceive(keyQueue, &keyEvent, portMAX_DELAY) != pdPASS) {
            continue;
        }

        // 处理接收到的按键事件
        switch (keyEvent) {
            case KEY_TASK_EVENT_CLICK_KEY1:
                printf("KEY1 press\r\n");
                break;
#if USE_PC13_AS_KEY
            case KEY_TASK_EVENT_CLICK_KEY2:
                printf("KEY2 press\r\n");
                break;
#endif
            default:
                // 处理其他事件
                break;
        }

        // printf("key_task\r\n");
    }
}

void EXTI0_IRQHandler(void)
{
    uint32_t keyEvent;
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        // 在这里添加 PA0 按钮按下的处理代码
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        keyEvent = KEY_TASK_EVENT_CLICK_KEY1;
        xQueueSendFromISR(keyQueue, (void *)&keyEvent, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE) {
            // 如果发送队列消息导致更高优先级的任务就绪，进行任务切换
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        // 清除中断标志
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

#if USE_PC13_AS_KEY
void EXTI15_10_IRQHandler(void)
{
    uint32_t keyEvent;
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        // 在这里添加 PC13 按钮按下的处理代码
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        keyEvent = KEY_TASK_EVENT_CLICK_KEY2;
        xQueueSendFromISR(keyQueue, (void *)&keyEvent, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE) {
            // 如果发送队列消息导致更高优先级的任务就绪，进行任务切换
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        // 清除中断标志
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}
#endif
