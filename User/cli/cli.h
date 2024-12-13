#ifndef _CLI_H_
#define _CLI_H_

#include "FreeRTOS.h"
#include "task.h"

void vUARTCommandConsoleStart(unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength);

#endif
