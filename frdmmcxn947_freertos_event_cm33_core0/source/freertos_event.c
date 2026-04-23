/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define B0 (1 << 0)
#define B1 (1 << 1)
#define B2 (1 << 2)
#define B3 (1 << 3)
#define B4 (1 << 4)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void write_task_1(void *pvParameters);
static void write_task_2(void *pvParameters);
static void write_task_3(void *pvParameters);
static void write_task_4(void *pvParameters);
static void write_task_5(void *pvParameters);
static void read_task(void *pvParameters);
/*******************************************************************************
 * Globals
 ******************************************************************************/
static EventGroupHandle_t event_group = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitHardware();
    event_group = xEventGroupCreate();
    if (xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 1, NULL) !=
        pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 1, NULL) !=
        pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(write_task_3, "WRITE_TASK_3", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 1, NULL) !=
            pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1)
			;
	}
    if (xTaskCreate(write_task_4, "WRITE_TASK_4", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 1, NULL) !=
            pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1)
			;
	}
    if (xTaskCreate(write_task_5, "WRITE_TASK_5", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 1, NULL) !=
            pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1)
			;
	}
    if (xTaskCreate(read_task, "READ_TASK", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief write_task_1 function
 */
static void write_task_1(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, B0);
        taskYIELD();
    }
}

/*!
 * @brief write_task_2 function
 */
static void write_task_2(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, B1);
        taskYIELD();
    }
}

/*!
 * @brief write_task_3 function
 */
static void write_task_3(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, B2);
        taskYIELD();
    }
}

/*!
 * @brief write_task_4 function
 */
static void write_task_4(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, B3);
        taskYIELD();
    }
}

/*!
 * @brief write_task_5 function
 */
static void write_task_5(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, B4);
        taskYIELD();
    }
}

/*!
 * @brief read_task function
 */
static void read_task(void *pvParameters)
{
    EventBits_t event_bits;
    while (1)
    {
        event_bits = xEventGroupWaitBits(event_group,    /* The event group handle. */
                                         B0 | B1 | B2 | B3 | B4,        /* The bit pattern the event group is waiting for. */
                                         pdTRUE,         /* B0 and B1 will be cleared automatically. */
                                         pdTRUE,        /* Don't wait for both bits, either bit unblock task. */
                                         portMAX_DELAY); /* Block indefinitely to wait for the condition to be met. */

        if ((event_bits & (B0 | B1 | B2 | B3 | B4)) == (B0 | B1 | B2 | B3 | B4))
        {
            PRINTF("All bits are set.\r\n");
        }
        else if ((event_bits & B0) == B0)
        {
            PRINTF("Bit B0 is set.\r\n");
        }
        else if ((event_bits & B1) == B1)
        {
            PRINTF("Bit B1 is set.\r\n");
        }
        else if ((event_bits & B2) == B2)
		{
			PRINTF("Bit B2 is set.\r\n");
		}
        else if ((event_bits & B3) == B3)
		{
			PRINTF("Bit B3 is set.\r\n");
		}
        else if ((event_bits & B4) == B4)
		{
			PRINTF("Bit B4 is set.\r\n");
		}
    }
}
