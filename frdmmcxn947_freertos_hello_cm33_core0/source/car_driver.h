/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 ,2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CAR_DRIVER_H_
#define _CAR_DRIVER_H_


#include "fsl_gpio.h"
#include "fsl_port.h"
/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

//B-1A & B-1B: Pines de entrada para controlar la dirección de giro del Motor B.
//A-1A & A-1B: Pines de entrada para controlar la dirección de giro del Motor A.

#define A_1A 14U
#define A_1B 15U

#define B_1A 22U
#define B_1B 23U

void carInit(void);
void moveDelay(void);
void moveStop(void);
void moveForward(void);
void moveBackward(void);
void moveLeft(void);
void moveRight(void);

/*!
 * @}
 */
#endif /* _CAR_DRIVER_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
