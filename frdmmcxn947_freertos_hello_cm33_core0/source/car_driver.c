/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 ,2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//B-1A & B-1B: Pines de entrada para controlar la dirección de giro del Motor B.
//A-1A & A-1B: Pines de entrada para controlar la dirección de giro del Motor A.

#include "FreeRTOS.h"
#include "task.h"
#include "car_driver.h"
#include "fsl_common.h"
#include "fsl_port.h"

void carInit(void)
{
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };

    /* Write to GPIO0: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_Gpio0);
    /* Write to PORT0: Peripheral clock is enabled */
    CLOCK_EnableClock(kCLOCK_Port0);

    /* GPIO0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    /* PORT0 peripheral is released from reset */
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);

    const port_pin_config_t WHEELS = {/* Internal pull-up/down resistor is disabled */
                                       kPORT_PullDisable,
                                       /* Low internal pull resistor value is selected. */
                                       kPORT_LowPullResistor,
                                       /* Fast slew rate is configured */
                                       kPORT_FastSlewRate,
                                       /* Passive input filter is disabled */
                                       kPORT_PassiveFilterDisable,
                                       /* Open drain output is disabled */
                                       kPORT_OpenDrainDisable,
                                       /* Low drive strength is configured */
                                       kPORT_LowDriveStrength,
                                       /* Pin is configured */
                                       kPORT_MuxAlt0,
                                       /* Digital input enabled */
                                       kPORT_InputBufferEnable,
                                       /* Digital input is not inverted */
                                       kPORT_InputNormal,
                                       /* Pin Control Register fields [15:0] are not locked */
                                       kPORT_UnlockRegister};

    /* PORT0_14 is configured as P0_14 */
    PORT_SetPinConfig(PORT0, A_1A, &WHEELS);
    /* PORT0_15 is configured as P0_15 */
    PORT_SetPinConfig(PORT0, A_1B, &WHEELS);

    /* PORT0_22 is configured as P0_22 */
    PORT_SetPinConfig(PORT0, B_1A, &WHEELS);
    /* PORT0_23 is configured as P0_23 */
    PORT_SetPinConfig(PORT0, B_1B, &WHEELS);

    /* Initialize GPIO functionality on pin P0_14  */
    GPIO_PinInit(GPIO0, A_1A, &led_config);
    /* Initialize GPIO functionality on pin P0_15  */
    GPIO_PinInit(GPIO0, A_1B, &led_config);

    /* Initialize GPIO functionality on pin P0_22  */
    GPIO_PinInit(GPIO0, B_1A, &led_config);
    /* Initialize GPIO functionality on pin P0_23  */
    GPIO_PinInit(GPIO0, B_1B, &led_config);

    return;
}

void moveDelay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 80000; ++i)
    {
        __asm("NOP"); /* delay */
    }
    return;
}

void moveStop(void)
{
	GPIO_PortSet(GPIO0, 1u << A_1A);
	GPIO_PortSet(GPIO0, 1u << A_1B);
	GPIO_PortSet(GPIO0, 1u << B_1A);
	GPIO_PortSet(GPIO0, 1u << B_1B);
    return;
}

void moveForward(void)
{
	int i=0;
	for (i=0; i<100;i++) //pulse to reduce speed.
	{
		moveStop();
		moveDelay();
		GPIO_PortSet(GPIO0, 1u << A_1A);
		GPIO_PortClear(GPIO0, 1u << A_1B);
		GPIO_PortSet(GPIO0, 1u << B_1A);
		GPIO_PortClear(GPIO0, 1u << B_1B);
		moveDelay();
	}
	moveStop();
    return;
}

void moveBackward(void)
{
	int i=0;
	for (i=0; i<100;i++) //pulse to reduce speed.
	{
		moveStop();
		moveDelay();
		GPIO_PortClear(GPIO0, 1u << A_1A);
		GPIO_PortSet(GPIO0, 1u << A_1B);
		GPIO_PortClear(GPIO0, 1u << B_1A);
		GPIO_PortSet(GPIO0, 1u << B_1B);
		moveDelay();
	}
	moveStop();
    return;
}

void moveLeft(void)
{
	int i=0;
	for (i=0; i<100;i++) //pulse to reduce speed.
	{
		moveStop();
		moveDelay();
		GPIO_PortClear(GPIO0, 1u << A_1A);
		GPIO_PortSet(GPIO0, 1u << A_1B);
		GPIO_PortSet(GPIO0, 1u << B_1A);
		GPIO_PortClear(GPIO0, 1u << B_1B);
		moveDelay();
	}
	moveStop();
    return;
}

void moveRight(void)
{
	int i=0;
	for (i=0; i<100;i++) //pulse to reduce speed.
	{
		moveStop();
		moveDelay();
		GPIO_PortSet(GPIO0, 1u << A_1A);
		GPIO_PortClear(GPIO0, 1u << A_1B);
		GPIO_PortClear(GPIO0, 1u << B_1A);
		GPIO_PortSet(GPIO0, 1u << B_1B);
		moveDelay();
		moveStop();
	}
	moveStop();
    return;
}
