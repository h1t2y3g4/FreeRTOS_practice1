#include "stm32f10x.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "config.h"
#include "cli.h"


void hardware_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  // 非常重要。如果是运行FreeRTOS必须要有，并且程序运行中不准变。
}

void led_task(void *pvParameters)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能 PB 端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;         // PB.5 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO 口速度为 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // 初始化 GPIOB.5
    GPIO_SetBits(GPIOB, GPIO_Pin_5);                  // PB.5 输出高

    while (1)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);
        vTaskDelay(1000);
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
        vTaskDelay(1000);
        printf("led_task\r\n");
    }
}

void tasks_init(void)
{
    TaskHandle_t led_task_handle;
    xTaskCreate(led_task, "led_task", 256, NULL, 2, &led_task_handle);

    vUARTCommandConsoleStart(512, 1);
}

int main(void)
{
    hardware_init();
    tasks_init();
    vTaskStartScheduler();
}
