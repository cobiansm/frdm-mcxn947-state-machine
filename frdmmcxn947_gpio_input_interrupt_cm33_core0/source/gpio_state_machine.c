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
task task_array[] = {
    { red_led_state, 5, 1, READY},
    { green_led_state, 3, 2, READY},
    { blue_led_state, 1, 3, READY},
    { cyan_led_state, 4, 4, READY},
    { magenta_led_state, 2, 5, READY},
    { yellow_led_state, 6, 6, READY},
    { white_led_state, 7, 7, READY},
    { disco_led_state, 8, 8, READY},
    { off_led_state, 9, 9, READY}
};
/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool sw3_ButtonPress = false;
volatile bool sw2_ButtonPress = false;
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
    for (int i = 0; i < (sizeof(task_array)/sizeof(task)); i++) {
        task *t = &task_array[i];
        if (t->state == READY) {
            t->state = RUNNING;
            t->led_state(NULL); 
            for (int j = 0; j < t->burst_time; j++) {
                PRINTF("Task %d is running, burst time left: %d\r\n", t->ID, t->burst_time - j);
                SDK_DelayAtLeastUs(1000000, SystemCoreClock); 
            }
            t->state = BLOCKED;
            PRINTF("Task %d completed and blocked\r\n", t->ID);
        }
    }
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
    PRINTF("\r\n State Machine \r\n");

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
        fifo_scheduler();
    }
}
