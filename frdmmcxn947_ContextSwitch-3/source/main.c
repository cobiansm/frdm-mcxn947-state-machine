/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <main.h>
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "tasks.h"
#include "app.h"

// system tick frequency
#define SYSTICK_FREQUENCY_HZ  20  //20 hertz is 50 ms for a systick.

static void delay(volatile int count)
{
	count *= 1000;
	while (count--);
}

/* endless loop tasks */
void task1(void *userdata)
{
	while (1) {
		PRINTF(userdata);
		PRINTF(": Running...1\n\r");
		delay(100);
	}
	//task_self_terminal();
}

void task2(void *userdata)
{
	PRINTF(userdata);
	PRINTF(": Running...2\n\r");
	delay(100);
	task_self_terminal();
}

void task3(void *userdata)
{
	PRINTF(userdata);
	PRINTF(": Running...3\n\r");
	delay(100);
	task_self_terminal();
}

void task4(void *userdata)
{
	PRINTF(userdata);
	PRINTF(": Running...4\n\r");
	delay(100);
	task_self_terminal();
}

void task5(void *userdata)
{
	PRINTF(userdata);
	PRINTF(": Running...5\n\r");
	delay(100);
	task_self_terminal();
}
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
	const char *str1 = "Task1";
	const char *str2 = "Task2";
	const char *str3 = "Task3";
	const char *str4 = "Task4";
	const char *str5 = "Task5";
	uint32_t sysClock = CLOCK_GetMainClkFreq();
	uint32_t value = sysClock/SYSTICK_FREQUENCY_HZ;

    /* Init board hardware. */
    BOARD_InitHardware();

	if (task_create(task1, (void *) str1) == -1)
		PRINTF("task 1 creation failed\r\n");

	if (task_create(task2, (void *) str2) == -1)
		PRINTF("task 2 creation failed\r\n");

	if (task_create(task3, (void *) str3) == -1)
		PRINTF("task 3 creation failed\r\n");

	if (task_create(task4, (void *) str4) == -1)
		PRINTF("task 4 creation failed\r\n");

	if (task_create(task5, (void *) str5) == -1)
		PRINTF("task 5 creation failed\r\n");

	NVIC_SetPriority(PendSV_IRQn, 0xF); // Set PendSV to lowest possible priority
	//SysTick_Config(SystemCoreClock/SYSTICK_FREQUENCY_HZ); //20 hertz is 50 ms for a systick.
	SysTick_Config(value); // x = 96MHz / 20Hz, x = 0x493E00. SystemTick every 50 ms.

	task_start();

	PRINTF("The execution of all the tasks ended!\n\r");

	while (1)
		{};
	return 0;
}
