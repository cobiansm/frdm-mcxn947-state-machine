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

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "car_driver.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
#define task_PRIORITY    (2)
#define control_PRIORITY (1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void delay(void);

TaskHandle_t xForwardHandle = NULL;
TaskHandle_t xBackwardHandle = NULL;
TaskHandle_t xLeftHandle = NULL;
TaskHandle_t xRightHandle = NULL;

void vForwardTask(void *pvParameters);
void vBackwardTask(void *pvParameters);
void vLeftTask(void *pvParameters);
void vRightTask(void *pvParameters);
void vControlTask(void *pvParameters);
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
	/* Define the init structure for the output LED pin*/
	gpio_pin_config_t led_config = {
		kGPIO_DigitalOutput,
		0,
	};

	/* Board pin, clock, debug console init */
	BOARD_InitHardware();
	carInit();

	/* Print a note to terminal. */
	PRINTF("\r\n Wheel Driver example\r\n");
	PRINTF("\r\n The car will move:\r\n");

	xTaskCreate(vForwardTask, "Forward", configMINIMAL_STACK_SIZE + 100, NULL, task_PRIORITY, &xForwardHandle);
	xTaskCreate(vBackwardTask, "Backward", configMINIMAL_STACK_SIZE + 100, NULL, task_PRIORITY, &xBackwardHandle);
	xTaskCreate(vLeftTask, "Left", configMINIMAL_STACK_SIZE + 100, NULL, task_PRIORITY, &xLeftHandle);
	xTaskCreate(vRightTask, "Right", configMINIMAL_STACK_SIZE + 100, NULL, task_PRIORITY, &xRightHandle);

	xTaskCreate(vControlTask, "Control", configMINIMAL_STACK_SIZE + 100, NULL, control_PRIORITY, NULL);

	vTaskSuspend(xForwardHandle);
	vTaskSuspend(xBackwardHandle);
	vTaskSuspend(xLeftHandle);
	vTaskSuspend(xRightHandle);

	vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Tasks
 */
void vForwardTask(void *pvParameters) {
    for (;;) {
        PRINTF("Forward\r\n");
        delay();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		moveForward();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		delay();
		moveStop();
		vTaskSuspend(NULL);
    }
}

void vBackwardTask(void *pvParameters) {
    for (;;) {
        PRINTF("Backward\r\n");
        delay();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		moveBackward();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		delay();
		moveStop();
		vTaskSuspend(NULL);
    }
}

void vLeftTask(void *pvParameters) {
    for (;;) {
        PRINTF("Left\r\n");
        delay();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		moveLeft();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		delay();
		moveStop();
		vTaskSuspend(NULL);
    }
}

void vRightTask(void *pvParameters) {
    for (;;) {
        PRINTF("Right\r\n");
        delay();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		moveRight();
		GPIO_PortToggle(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
		delay();
		moveStop();
		vTaskSuspend(NULL);
    }
}

void vControlTask(void *pvParameters) {
	const char *secuencia = "FBLR";
	for (;;) {
		for(int i = 0; secuencia[i] != '\0'; i++) {
			if (secuencia[i] == 'F') vTaskResume(xForwardHandle);
			else if (secuencia[i] == 'B') vTaskResume(xBackwardHandle);
			else if (secuencia[i] == 'L') vTaskResume(xLeftHandle);
			else if (secuencia[i] == 'R') vTaskResume(xRightHandle);
		}
		vTaskSuspend(NULL);
	}
}
