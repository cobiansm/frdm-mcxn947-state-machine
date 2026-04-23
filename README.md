# Semaphores

## Overview

This project implements a Producer-Consumer model using the FreeRTOS real-time operating system Semaphore SDK. The primary objective is to transmit the message "ITESO RULES" through a circular buffer limited to 5 elements.
The system utilizes two synchronized tasks:
- Producer: Fetches a character from the original message and places it into the next available slot in the buffer.
- Consumer: Extracts the character from the buffer and prints it to the debug console.

Even though the message contains 11 characters and the buffer only holds 5, the system never overflows. This is achieved through the use of modular arithmetic and binary semaphores.
