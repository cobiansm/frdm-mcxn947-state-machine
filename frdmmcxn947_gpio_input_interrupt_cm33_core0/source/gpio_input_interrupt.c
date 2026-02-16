/*
 * State Machine
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
 * Definitions
 ******************************************************************************/
// function type for each state
typedef void (*active_state_s)(void *);

// structure of a state
typedef struct ledState{
    active_state_s led_state;
    struct ledState *next;
    struct ledState *prev;
    const char *name;
} ledState;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);

// state functions' prototypes
void red_led_state(void *ptr);
void green_led_state(void *ptr);
void blue_led_state(void *ptr);
/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool sw3_ButtonPress = false;
volatile bool sw2_ButtonPress = false;

// declaration of the states' variables
static ledState red_led_state_s;
static ledState green_led_state_s;
static ledState blue_led_state_s;
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
* Function to initialize the states of the state machine
*/
static void states_init(void)
{
    red_led_state_s.led_state  = red_led_state;
    red_led_state_s.next    = &green_led_state_s;    
    red_led_state_s.prev    = &blue_led_state_s;   
    red_led_state_s.name    = "RED";

    green_led_state_s.led_state  = green_led_state;
    green_led_state_s.next    = &blue_led_state_s;    
    green_led_state_s.prev    = &red_led_state_s;  
    green_led_state_s.name    = "GREEN";

    blue_led_state_s.led_state  = blue_led_state;
    blue_led_state_s.next    = &red_led_state_s;    
    blue_led_state_s.prev    = &green_led_state_s;  
    blue_led_state_s.name    = "BLUE";
}

/*
* Change between states, SW3 goes forward, SW2 goes backwards. No switch statement, just pointers.
*/
static void state_machine_step(ledState **current, bool sw2_pressed, bool sw3_pressed, void *ptr)
{
    if (current == NULL || *current == NULL) return;

    /* Execute current state */
    PRINTF("Entering state: %s\r\n", (*current)->name);
    (*current)->led_state(ptr);

    /* Change forwards or backwards */
    if (sw3_pressed && (*current)->next) {
        *current = (*current)->next;
        sw3_ButtonPress = false;
    } else if (sw2_pressed && (*current)->prev) {
        *current = (*current)->prev;
        sw2_ButtonPress = false;
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

    states_init();
    ledState *current_state = &red_led_state_s;

    while (1)
    {
        state_machine_step(&current_state, sw2_ButtonPress, sw3_ButtonPress, NULL);
    }
}
