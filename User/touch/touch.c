#include "touch.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TPAD_ARR_MAX_VAL 0XFFFF // 最大的 ARR 值
volatile uint16_t tpad_default_val = 0;      // 空载的时候(没有手按下),计数器需要的时间

// 复位一次
void TPAD_Reset(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能 PA 时钟
    // 设置 GPIOA.1 为推挽使出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;        // PA1 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                   // 初始化 GPIOA.1
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);                       // PA.1 输出 0,放电

    vTaskDelay(5);                                           // 延时 5ms

    taskENTER_CRITICAL();
    TIM_SetCounter(TIM5, 0);                                 // 归 0
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC2 | TIM_IT_Update); // 清除中断标志
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    // GPIOA.1 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    taskEXIT_CRITICAL();
}
// 得到定时器捕获值
// 如果超时,则直接返回定时器的计数值.
uint16_t TPAD_Get_Val(void)
{
    TPAD_Reset();
    while (TIM_GetITStatus(TIM5, TIM_IT_CC2) == RESET) // 等待溢出
    {
        if (TIM_GetCounter(TIM5) > TPAD_ARR_MAX_VAL - 500)
        {
            taskEXIT_CRITICAL();
            printf("TPAD_Get_Val overtime\r\n");
            return TIM_GetCounter(TIM5); // 超时了,直接返回 CNT 的值
        }
    };
    printf("TIM_GetCapture2\r\n");
    return TIM_GetCapture2(TIM5);
}
// 读取 n 次,取最大值
uint16_t TPAD_Get_MaxVal(uint8_t n)
{
    uint16_t temp = 0;
    uint16_t res = 0;
    while (n--)
    {
        temp = TPAD_Get_Val(); // 得到一次值
        printf("temp = %d\r\n", temp);
        if (temp > res)
            res = temp;
    };
    return res;
}
// 扫描触摸按键
// mode:0,不支持连续触发(按下一次必须松开才能按下一次);1,支持连续触发(可以一直按)
// 返回值:0,没有按下;1,有按下;
#define TPAD_GATE_VAL 100
// 触摸的门限值,也就是必须大于 tpad_default_val+TPAD_GATE_VAL,才认为是有效触摸.
uint8_t TPAD_Scan(uint8_t mode)
{
    static uint8_t keyen = 0; // 0,可以开始检测;>0,还不能开始检测
    uint8_t res = 0;
    uint8_t sample = 3; // 默认采样次数为 3 次
    uint16_t rval;
    if (mode)
    {
        sample = 6; // 支持连按的时候，设置采样次数为 6 次
        keyen = 0;  // 支持连按
    }
    rval = TPAD_Get_MaxVal(sample);
    if (rval > (tpad_default_val + TPAD_GATE_VAL))
    {
        // 大于 tpad_default_val+TPAD_GATE_VAL,有效
        if (keyen == 0)
        {
            res = 1; // keyen==0,有效
        }
        // printf("r:%d\r\n",rval);
        keyen = 3; // 至少要再过 3 次之后才能按键有效
    }
    if (keyen)
        keyen--;
    return res;
}
// 定时器 2 通道 2 输入捕获配置
void TIM5_CH2_Cap_Init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM5_ICInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  // 使能 TIM5 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能 PA 时钟
    // 设置 GPIOA.1 为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;             // PA1 端口配置
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 速度 50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // 初始化 GPIOA.1
    // 初始化 TIM5
    TIM_TimeBaseStructure.TIM_Period = arr;                     // 设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);             // 根据参数初始化 TIMx
    // 初始化 TIM5 通道 2
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2;            // 选择输入端 IC2 映射到 TI5 上
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿捕获
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // 配置输入分频,不分频
    TIM5_ICInitStructure.TIM_ICFilter = 0x03;              // 配置输入滤波器 8 个定时器时钟周期滤波
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);               // 初始化 I5 IC2
    TIM_Cmd(TIM5, ENABLE);                                 // 使能定时器 5
}

// 初始化触摸按键
// 获得空载的时候触摸按键的取值.
// 返回值:0,初始化成功;1,初始化失败
uint8_t TPAD_Init(uint8_t psc)
{
    uint16_t buf[10];
    uint16_t temp;
    uint8_t j, i;
    TIM5_CH2_Cap_Init(TPAD_ARR_MAX_VAL, psc - 1); // 以 1Mhz 的频率计数
    for (i = 0; i < 10; i++)                      // 连续读取 10 次
    {
        buf[i] = TPAD_Get_Val();
        vTaskDelay(10);
    }
    for (i = 0; i < 9; i++) // 排序
    {
        for (j = i + 1; j < 10; j++)
        {
            if (buf[i] > buf[j]) // 升序排列
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    temp = 0;
    for (i = 2; i < 8; i++)
    {
        temp += buf[i]; // 取中间的 8 个数据进行平均
    }
    tpad_default_val = temp / 6;
    printf("tpad_default_val:%d\r\n", tpad_default_val);
    if (tpad_default_val > TPAD_ARR_MAX_VAL / 2)
    {
        // 初始化遇到超过 TPAD_ARR_MAX_VAL/2 的数值,不正常!
        return 1;
    }
    return 0;
}

void touch_task(void *pvParameters)
{
    uint8_t t = 0;
    TPAD_Init(72);                                    // 初始化触摸按键

    while (1)
    {
        if (TPAD_Scan(0)) // 成功捕获到了一次上升沿(此函数执行时间至少 15ms)
        {
            // LED1 = !LED1; // LED1 取反
            printf("touched\r\n");
        }
        t++;
        if (t == 15)
        {
            t = 0;
            // LED0 = !LED0; // LED0 取反,提示程序正在运行
        }
        vTaskDelay(10);
    }
}
