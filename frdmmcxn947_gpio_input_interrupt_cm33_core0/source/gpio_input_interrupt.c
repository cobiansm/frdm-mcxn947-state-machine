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

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);
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
		GPIO_PinClearInterruptFlag(GPIO0, BOARD_SW3_GPIO_PIN);
		sw3_ButtonPress = true;
	} if (GPIO_PinGetInterruptFlag(GPIO0, BOARD_SW2_GPIO_PIN)) {
		GPIO_PinClearInterruptFlag(GPIO0, BOARD_SW2_GPIO_PIN);
		sw2_ButtonPress = true;
	}

    SDK_ISR_EXIT_BARRIER;
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
    PRINTF("\r\n GPIO Driver example\r\n");

    /* Init input switch GPIO. */
    GPIO_PinInit(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN, &sw_config);
    GPIO_PinInit(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN, &sw_config);

    GPIO_SetPinInterruptConfig(GPIO0, BOARD_SW3_GPIO_PIN, kGPIO_InterruptFallingEdge);
    GPIO_SetPinInterruptConfig(GPIO0, BOARD_SW2_GPIO_PIN, kGPIO_InterruptFallingEdge);

    GPIO_PinClearInterruptFlag(GPIO0, 0xFFFFFFFF);

    EnableIRQ(GPIO00_IRQn);

    /* Init output LED GPIO. */
    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, &led_config);
    GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &led_config);

    GPIO_PortSet(GPIO0, 1U << BOARD_LED_RED_GPIO_PIN);
    GPIO_PortSet(GPIO0, 1U << BOARD_LED_BLUE_GPIO_PIN);

    GPIO_PinWrite(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, 1);
    GPIO_PinWrite(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, 1);

    while (1)
    {
        if (sw3_ButtonPress)
        {
            PRINTF(" %s is pressed \r\n", BOARD_SW3_NAME);
            /* Toggle LED. */
            GPIO_PortToggle(BOARD_LED_RED_GPIO, 1U << BOARD_LED_RED_GPIO_PIN);
            /* Reset state of button. */
            sw3_ButtonPress = false;
        } if (sw2_ButtonPress) {
        	PRINTF(" %s is pressed \r\n", BOARD_SW2_NAME);
			/* Toggle LED. */
			GPIO_PortToggle(BOARD_LED_BLUE_GPIO, 1U << BOARD_LED_BLUE_GPIO_PIN);
			/* Reset state of button. */
			sw2_ButtonPress = false;
        }
    }
}
