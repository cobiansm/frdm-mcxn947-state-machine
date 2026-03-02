/*
 * Scheduler FIFO
 * Marlene Cobian
 */

/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#if defined(FSL_FEATURE_SOC_PORT_COUNT) && (FSL_FEATURE_SOC_PORT_COUNT)
#include "fsl_port.h"
#endif
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "board.h"

#define MAX_TASKS 10
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);

// state functions' prototypes
void red_led_state(void *ptr);
void green_led_state(void *ptr);
void blue_led_state(void *ptr);
void cyan_led_state(void *ptr);
void magenta_led_state(void *ptr);
void yellow_led_state(void *ptr);
void white_led_state(void *ptr);
void disco_led_state(void *ptr);
void off_led_state(void *ptr);
/*******************************************************************************
 * Definitions
 ******************************************************************************/
// enum for states
typedef enum {
    READY,
    RUNNING,
    BLOCKED 
} state_t;

 // function type for each state
typedef void (*active_state_s)(void *ptr);

// structure of a state
typedef struct {
    active_state_s led_state;
    int burst_time;
    int ID;
    state_t state;
} task;

// task array
task task_array[MAX_TASKS] = {
    { red_led_state, 5, 1, READY},
    { green_led_state, 3, 2, READY},
    { blue_led_state, 2, 3, READY},
    { cyan_led_state, 4, 4, READY},
    { magenta_led_state, 8, 5, READY},
    { yellow_led_state, 6, 6, READY},
    { white_led_state, 7, 7, READY}
};
/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool sw3_ButtonPress = false;
volatile bool sw2_ButtonPress = false;
int current_tasks = 7;
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Interrupt service function of switch.
 *
 * This function toggles the LED
 */
void GPIO00_IRQHandler(void)
{
	if (GPIO_PinGetInterruptFlag(GPIO0, BOARD_SW3_GPIO_PIN)) {
		sw3_ButtonPress = true;
        GPIO_PinClearInterruptFlag(GPIO0, BOARD_SW3_GPIO_PIN);
	} if (GPIO_PinGetInterruptFlag(GPIO0, BOARD_SW2_GPIO_PIN)) {
		sw2_ButtonPress = true;
        GPIO_PinClearInterruptFlag(GPIO0, BOARD_SW2_GPIO_PIN);
	}

    SDK_ISR_EXIT_BARRIER;
}

/*
* Red LED state
*/
void red_led_state(void *ptr) {
    LED_RED_ON();
    LED_BLUE_OFF();
    LED_GREEN_OFF();
}

/*
* Green LED state
*/
void green_led_state(void *ptr) {
    LED_GREEN_ON();
    LED_RED_OFF();
    LED_BLUE_OFF();
}

/*
* Blue LED state
*/
void blue_led_state(void *ptr) {
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_ON();
}

/*
* Cyan LED state
*/
void cyan_led_state(void *ptr) {
    LED_RED_OFF();
    LED_GREEN_ON();
    LED_BLUE_ON();
}

/*
* Magenta LED state
*/
void magenta_led_state(void *ptr) {
    LED_RED_ON();
    LED_GREEN_OFF();
    LED_BLUE_ON();
}

/*
* Yellow LED state
*/
void yellow_led_state(void *ptr) {
    LED_RED_ON();
    LED_GREEN_ON();
    LED_BLUE_OFF();
}

/*
* White LED state
*/
void white_led_state(void *ptr) {
    LED_RED_ON();
    LED_GREEN_ON();
    LED_BLUE_ON();
}

/*
* Disco LED state
*/
void disco_led_state(void *ptr) {
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_OFF();

    static uint32_t counter = 0;
    const uint32_t threshold = 200;

    counter++;
    if (counter >= threshold)
    {
        // Pick a random color
        int c = rand() % 7; // 0-6: RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN
        switch(c)
        {
            case 0: LED_RED_ON(); break;
            case 1: LED_GREEN_ON(); break;
            case 2: LED_BLUE_ON(); break;
            case 3: LED_RED_ON(); LED_GREEN_ON(); break;   // YELLOW
            case 4: LED_RED_ON(); LED_BLUE_ON(); break;    // MAGENTA
            case 5: LED_GREEN_ON(); LED_BLUE_ON(); break;  // CYAN
            case 6: LED_RED_ON(); LED_GREEN_ON(); LED_BLUE_ON(); break; // WHITE (optional)
        }
        SDK_DelayAtLeastUs(200000, SystemCoreClock);
        counter = 0;
    }
}

/*
* Off LED state
*/
void off_led_state(void *ptr) {
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_OFF();
}

/*
* Scheduler implementation 
*/
void fifo_scheduler(void)
{
    for (int i = 0; i < current_tasks; i++) {
        task *t = &task_array[i];
        if (t->state == READY) {
            t->state = RUNNING;
            t->led_state(NULL); 
            for (int j = 0; j < t->burst_time; j++) {
                PRINTF("Task %d is running, burst time left: %d\r\n", t->ID, t->burst_time - j);
                for(volatile int delay = 0; delay < 10000000; delay++);
				if (sw2_ButtonPress || sw3_ButtonPress) return;
            }
            t->state = BLOCKED;
            PRINTF("Task %d completed and blocked\r\n", t->ID);
        }
    }
}

/*
* Task table
*/
void print_task_table(void) {
    PRINTF("\r\n====== TASK TABLE ======\r\n");
    PRINTF("ID\tBurst\tState\r\n");
    for (int i = 0; i < current_tasks; i++) {
        char* status = (task_array[i].state == READY) ? "READY" : "BLOCKED";
        PRINTF("%d\t%d\t%s\r\n", task_array[i].ID, task_array[i].burst_time, status);
    }
    PRINTF("========================\r\n\n");
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Define the init structure for the input switch pin */
    gpio_pin_config_t sw_config = {
        kGPIO_DigitalInput,
        0,
    };

    /* Define the init structure for the output LED pin */
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };

    BOARD_InitHardware();

    /* Print a note to terminal. */
    PRINTF("\r\n FIFO Scheduler \r\n");

    /* Init input switch GPIO. */
    GPIO_PinInit(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN, &sw_config);
    GPIO_PinInit(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN, &sw_config);

    GPIO_SetPinInterruptConfig(GPIO0, BOARD_SW3_GPIO_PIN, kGPIO_InterruptFallingEdge);
    GPIO_SetPinInterruptConfig(GPIO0, BOARD_SW2_GPIO_PIN, kGPIO_InterruptFallingEdge);

    GPIO_PinClearInterruptFlag(GPIO0, 0xFFFFFFFF);

    EnableIRQ(GPIO00_IRQn);

    /* Init output LED GPIO. */
    LED_RED_INIT(LOGIC_LED_OFF);
    LED_GREEN_INIT(LOGIC_LED_OFF);
    GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &led_config);

    while (1)
    {
    	if (sw2_ButtonPress) {
        	if (current_tasks < MAX_TASKS) {
        		task_array[current_tasks].led_state = off_led_state;
				task_array[current_tasks].burst_time = (rand() % 5) + 2;
				task_array[current_tasks].ID = current_tasks + 1;
				task_array[current_tasks].state = READY;

				PRINTF(">>SW3 Pressed: Task %d created\r\n", task_array[current_tasks].ID);
				current_tasks++;
				print_task_table();
			} else {
				PRINTF("Max tasks reached\r\n");
			}
			sw2_ButtonPress = false;
		}
    	if (sw3_ButtonPress) {
			for (int k = 0; k < current_tasks; k++) {
				task_array[k].state = READY;
			}
			PRINTF(">>SW3 Pressed: All tasks READY\r\n");
			print_task_table();
			sw3_ButtonPress = false;
		}
    	fifo_scheduler();
    }
}
