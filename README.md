# Round-Robin scheduler

## Overview

This project implements a Preemptive Round-Robin Task Scheduler. This scheduler uses the SysTick timer to enforce a specific Time Slice (50ms) for each task, ensuring that all 5 tasks receive an equal share of the CPU execution time.

The system manages a set of 5 concurrent tasks. The core of the project is the manual management of the Task Control Block (TCB) and the execution of Context Switching using the Main Stack Pointer (MSP) and Process Stack Pointer (PSP) through PendSV and SysTick exceptions.

## Scheduling States

The scheduler manages the lifecycle of each task through the following primary states:
- **READY**: The task is initialized and resides in the FIFO queue, waiting for its allocated time slice.
- **RUNNING**: The task currently has control of the MCU. Its context has been loaded into the CPU registers.
- **END**: Should never reach this since they are endless-loop tasks.

## Scheduler Implementation
The heart of the system relies on:
- **SysTick**: Configured to trigger every 50ms. It acts as the system heartbeat, marking the end of a time slice and triggering the PendSV flag, which will save the current tasks' context, select the next task from the queue and loas its context.
