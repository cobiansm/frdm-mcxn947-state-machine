/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS includes. */
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
#define TASK_PRIO          (configMAX_PRIORITIES - 1)
#define CONSUMER_LINE_SIZE 3
#define BUFFER_SIZE 5
#define MESSAGE_SIZE    11

SemaphoreHandle_t xSemaphore_producer;
SemaphoreHandle_t xSemaphore_consumer;

char buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;
const char *message = "ITESO RULES";
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void producer_task(void *pvParameters);
static void consumer_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_InitHardware();

    /* Create semaphores */
    xSemaphore_producer = xSemaphoreCreateBinary();
	xSemaphore_consumer = xSemaphoreCreateBinary();

	/* Let the producer start */
	xSemaphoreGive(xSemaphore_producer);

    if (xTaskCreate(producer_task, "PRODUCER_TASK", configMINIMAL_STACK_SIZE + 128, NULL, TASK_PRIO, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(consumer_task, "CONSUMER_TASK", configMINIMAL_STACK_SIZE + 128, NULL, TASK_PRIO, NULL) != pdPASS)
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
 * @brief Task producer_task.
 */
static void producer_task(void *pvParameters)
{
    uint32_t i = 0;

    PRINTF("Producer_task created.\r\n");
    while (i < MESSAGE_SIZE)
    {
        /* Producer is waiting when consumer will be ready to accept item. */
        if (xSemaphoreTake(xSemaphore_producer, portMAX_DELAY) == pdTRUE)
        {
        	/* Write and advance */
        	buffer[head] = message[i];
			head = (head + 1) % BUFFER_SIZE;
			i = (i + 1) % MESSAGE_SIZE;

			xSemaphoreGive(xSemaphore_consumer);
        }
        else
        {
            PRINTF("Producer is waiting for customer.\r\n");
        }
    }
    PRINTF("Producer: Finished writing the message.\r\n");
	vTaskDelete(NULL);
}

/*!
 * @brief Task consumer_task.
 */
static void consumer_task(void *pvParameters)
{
	uint32_t j = 0;
	char final_message[MESSAGE_SIZE + 1];

	PRINTF("Consumer_task created.\r\n");
    while (j < MESSAGE_SIZE)
    {
        /* Consumer is waiting when producer will be ready to produce item. */
    	if (xSemaphoreTake(xSemaphore_consumer, portMAX_DELAY) == pdTRUE)
        {
            char read_letter = buffer[tail];
            final_message[j] = read_letter;
            PRINTF("Read letter: '%c'\r\n", read_letter);


			tail = (tail + 1) % BUFFER_SIZE;
			j++;
			PRINTF("Buffer: [%c][%c][%c][%c][%c]\r\n",
					buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
			PRINTF("------------------------------\r\n");

			xSemaphoreGive(xSemaphore_producer);

			/* Delay to read the terminal */
			vTaskDelay(pdMS_TO_TICKS(500));
        }
        else
        {
            PRINTF("Consumer is waiting for producer.\r\n", pvParameters);
        }
    }
    PRINTF("Consumer: Finished reading the message.\r\n");
    final_message[MESSAGE_SIZE] = '\0';
    PRINTF("Final message: %s", final_message);
	vTaskDelete(NULL);
}
