# FreeRTOS car driver

## Overview

This project implements a sequential car movement system using Task Suspension and Priorities in FreeRTOS.

## How it Works

- Control Task: Reads a sequence string (e.g., "FBLR"). Based on each character, it wakes up the corresponding task using vTaskResume().
- Movement Tasks: Since they have higher priority, they immediately preempt the control task when resumed. They perform the movement, stop the car, and then call vTaskSuspend(NULL).
