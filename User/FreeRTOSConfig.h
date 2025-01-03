/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f10x.h"
#include "stdio.h"

//断言
#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#define configUSE_PREEMPTION                                        1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION                     0  // 为0用软件方法选择下一个要运行的任务，为1用硬件方法选择下一个要运行的任务。STM32可以为1。
#define configUSE_TICKLESS_IDLE                                     0
#define configCPU_CLOCK_HZ                                          ( ( unsigned long ) 72000000 )
// #define configSYSTICK_CLOCK_HZ                                      1000000
#define configTICK_RATE_HZ                                          ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                                        10
#define configMINIMAL_STACK_SIZE                                    ( ( unsigned short ) 128 )  // 字为单位，STM32中一个字为4个字节
#define configMAX_TASK_NAME_LEN                                     16
#define configUSE_16_BIT_TICKS                                      0  // 为 1 的时候 TickType_t 就是 16 位的，为 0 的话 TickType_t 就是 32 位的
#define configIDLE_SHOULD_YIELD                                     0  // 当为 0 的时候空闲任务不会为其他处于同优先级的任务让出 CPU 使用权
// #define configUSE_TASK_NOTIFICATIONS                                1
// #define configTASK_NOTIFICATION_ARRAY_ENTRIES                       3
#define configUSE_MUTEXES                                           1
// #define configUSE_RECURSIVE_MUTEXES                                 0
// #define configUSE_COUNTING_SEMAPHORES                               0
// #define configUSE_ALTERNATIVE_API                                   0 /* Deprecated! */
#define configQUEUE_REGISTRY_SIZE                                   10  // 队列集的数量
#define configUSE_QUEUE_SETS                                        1  // 为1的时候可以使用队列集，为0的时候不可以使用队列集
#define configUSE_TIME_SLICING                                      1  // 为1多个任务可以共用同一个优先级，为0则不可以
// #define configUSE_NEWLIB_REENTRANT                                  0
// #define configENABLE_BACKWARD_COMPATIBILITY                         0
// #define configNUM_THREAD_LOCAL_STORAGE_POINTERS                     5
// #define configUSE_MINI_LIST_ITEM                                    1
// #define configSTACK_DEPTH_TYPE                                      uint16_t
// #define configMESSAGE_BUFFER_LENGTH_TYPE                            size_t
// #define configHEAP_CLEAR_MEMORY_ON_FREE                             1

/* cli */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE                           1024

/* Memory allocation related definitions. */
// #define configSUPPORT_STATIC_ALLOCATION                             0  // 1表示支持静态创建任务的方法，同时要自己申请堆栈。
#define configSUPPORT_DYNAMIC_ALLOCATION                            1
#define configTOTAL_HEAP_SIZE                                       ( ( size_t ) ( 40 * 1024 ) )
// #define configAPPLICATION_ALLOCATED_HEAP                            0  // 如果为1就要自己实现堆分配函数
// #define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP                   1

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                                 0
#define configUSE_TICK_HOOK                                 0
#define configCHECK_FOR_STACK_OVERFLOW                      0
#define configUSE_MALLOC_FAILED_HOOK                        0
#define configUSE_DAEMON_TASK_STARTUP_HOOK                  0
#define configUSE_SB_COMPLETED_CALLBACK                     0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS                       0
#define configUSE_TRACE_FACILITY                            1
#define configUSE_STATS_FORMATTING_FUNCTIONS                1

/* Co-routine related definitions. */
// #define configUSE_CO_ROUTINES                               0
// #define configMAX_CO_ROUTINE_PRIORITIES                     1

/* Software timer related definitions. */
#define configUSE_TIMERS                                    1
#define configTIMER_TASK_PRIORITY                           configMAX_PRIORITIES - 1
#define configTIMER_QUEUE_LENGTH                            10
#define configTIMER_TASK_STACK_DEPTH                        configMINIMAL_STACK_SIZE

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_uxTaskGetStackHighWaterMark2    0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          0
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskGetHandle                  0
#define INCLUDE_xTaskResumeFromISR              1

/* Use the system definition, if there is one. */
#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS								__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS								4	 /* 4 priority levels. */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0x0F  // 设置最低优先级。stm32用了4位优先级，同时数字最大优先级最低，所以4位都是1时优先级最低，换成十进制就是15.
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5  // FreeRTOS可管理的最大优先级，STM32的中断里数字更小的就管不了。
/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )  // 优先级寄存器高四位有效，所以移动到高四位去
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )


/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY		(configLIBRARY_LOWEST_INTERRUPT_PRIORITY)

#endif /* FREERTOS_CONFIG_H */

