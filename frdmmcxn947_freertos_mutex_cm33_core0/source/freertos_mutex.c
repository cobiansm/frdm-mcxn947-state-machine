/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
SemaphoreHandle_t xMutex;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void write_task_1(void *pvParameters);
static void write_task_2(void *pvParameters);
static void write_task_3(void *pvParameters);
static void write_task_4(void *pvParameters);
static void write_task_5(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    xMutex = xSemaphoreCreateMutex();

    BOARD_InitHardware();
    if (xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 1, NULL) !=
        pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 1, NULL) !=
        pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(write_task_3, "WRITE_TASK_3", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 1, NULL) !=
            pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1)
			;
	}
    if (xTaskCreate(write_task_4, "WRITE_TASK_4", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 1, NULL) !=
            pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1)
			;
	}
    if (xTaskCreate(write_task_5, "WRITE_TASK_5", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 1, NULL) !=
            pdPASS)
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
 * @brief Write Task 1 function
 */
static void write_task_1(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE)
        {
            PRINTF("Failed to take semaphore.\r\n");
        }
        for (int i=0; i<100; i++){
        	PRINTF("A");
			taskYIELD();
        }
        PRINTF("\r\n");
        xSemaphoreGive(xMutex);
        taskYIELD();
    }
}

/*!
 * @brief Write Task 2 function
 */
static void write_task_2(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE)
        {
            PRINTF("Failed to take semaphore.\r\n");
        }
        for (int i=0; i<100; i++){
			PRINTF("B");
			taskYIELD();
		}
        PRINTF("\r\n");
        xSemaphoreGive(xMutex);
        taskYIELD();
    }
}

/*!
 * @brief Write Task 3 function
 */
static void write_task_3(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE)
        {
            PRINTF("Failed to take semaphore.\r\n");
        }
        for (int i=0; i<100; i++){
			PRINTF("C");
			taskYIELD();
		}
        PRINTF("\r\n");
        xSemaphoreGive(xMutex);
        taskYIELD();
    }
}

/*!
 * @brief Write Task 4 function
 */
static void write_task_4(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE)
        {
            PRINTF("Failed to take semaphore.\r\n");
        }
        for (int i=0; i<100; i++){
			PRINTF("D");
			taskYIELD();
		}
        PRINTF("\r\n");
        xSemaphoreGive(xMutex);
        taskYIELD();
    }
}

/*!
 * @brief Write Task 5 function
 */
static void write_task_5(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE)
        {
            PRINTF("Failed to take semaphore.\r\n");
        }
        for (int i=0; i<100; i++){
			PRINTF("E");
			taskYIELD();
		}
        PRINTF("\r\n");
        xSemaphoreGive(xMutex);
        taskYIELD();
    }
}
