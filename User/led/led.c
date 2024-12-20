#include "led.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#define LED_GPIO            GPIOB
#define LED_RED_PIN         GPIO_Pin_5
#define LED_GREEN_PIN       GPIO_Pin_0
#define LED_BLUE_PIN        GPIO_Pin_1

void led_task(void *pvParameters)
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

    while (1)
    {
        GPIO_ResetBits(LED_GPIO, LED_RED_PIN);
        vTaskDelay(1000);
        GPIO_SetBits(LED_GPIO, LED_RED_PIN);
        // vTaskDelay(1000);
        GPIO_ResetBits(LED_GPIO, LED_GREEN_PIN);
        vTaskDelay(1000);
        GPIO_SetBits(LED_GPIO, LED_GREEN_PIN);
        // vTaskDelay(1000);
        GPIO_ResetBits(LED_GPIO, LED_BLUE_PIN);
        vTaskDelay(1000);
        GPIO_SetBits(LED_GPIO, LED_BLUE_PIN);
        // vTaskDelay(1000);
        // printf("led_task\r\n");
    }
}
