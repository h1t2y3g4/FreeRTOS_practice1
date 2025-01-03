#include "config.h"
#include "stdio.h"


task_handle_t tasks;
queue_handle_t queues;

void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
        int cnt = 10000;
        while (cnt--) {;}
        printf("HardFault\r\n");
    }
}
