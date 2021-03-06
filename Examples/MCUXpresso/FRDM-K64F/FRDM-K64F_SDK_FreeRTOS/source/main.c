/*
 * The Clear BSD License
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "FreeRTOS_Timers.h"
#include "SysView.h"


/*!
 * @brief Second task, lower priority.
 */
static void second_task(void *pvParameters) {
    while (1) {
        /* dummy code, notify HOST when int16 underflow */
        for (uint16_t j = (uint16_t)-1;; j--) {
            if (j == 0) {
                SEGGER_SYSVIEW_Warn("second task int underflow");
            }
            vTaskDelay(pdMS_TO_TICKS(5));
            __NOP();
        }
    }
}


/*!
 * @brief First task, higher priority.
 */
static void first_task(void *pvParameters) {
    if (xTaskCreate(second_task, "second_task", 500/sizeof(StackType_t), NULL, 3, NULL) != pdPASS) {
        PRINTF("Task creation failed!.\r\n");
        vTaskSuspend(NULL);
    }
    /* dummy code, print counter and delay */
    for (int counter = 0;; counter++) {
        SEGGER_SYSVIEW_PrintfTarget("first task counter: %d ", counter++);
        vTaskDelay(100);
    }
}


/*!
 * @brief Main function
 */
int main(void) {
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    SysView_Init();
    FreeRTOS_Timers_Init();

    PRINTF("Execution timer: %s\n\rTime: %u ticks %2.5f milliseconds\n\rDONE\n\r", "1 day", 86400, 86.4);
    if (xTaskCreate(first_task, "first_task", 500/sizeof(StackType_t), NULL, 4, NULL) != pdPASS)  {
        PRINTF("Task creation failed!.\r\n");
        for(;;){}
    }
    vTaskStartScheduler();
    for(;;){}
}

