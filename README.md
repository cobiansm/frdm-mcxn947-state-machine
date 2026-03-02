# FIFO scheduler

## Overview

This project implements a Non-Preemptive FIFO (First-In, First-Out) Task Scheduler for an RGB LED system using function pointers. This scheduler manages a queue of tasks, each with a specific Burst Time, and executes them sequentially until completion.

- **SW3:** Resets all completed tasks back to the READY state to restart the cycle
- **SW2:** Creates and adds a new task to the queue (up to 10 tasks)

Each task executes its specific LED behavior for a defined burst time. The project demonstrates non-preemptive scheduling, interrupt-driven task management, and a sequential execution flow.

## Scheduling states

The scheduler manages tasks through three primary states:
- **READY:** The task is in the queue waiting for its turn.
- **RUNNING:** The task currently has control of the MCU.
- **BLOCKED:** The task has finished its execution and is waiting for a system reset.

## Scheduler

| Task ID | Next Task (FIFO) | Burst Time  | LEDs ON                     | Perceived Color |
|:-------:|:----------------:|:------------------:|:----------------------------|:---------------:|
| **T1**  | T2               | 5                  | Red                         | Red             |
| **T2**  | T3               | 3                  | Green                       | Green           |
| **T3**  | T4               | 1                  | Blue                        | Blue            |
| **T4**  | T5               | 4                  | Green + Blue                | Cyan            |
| **T5**  | T6               | 2                  | Red + Blue                  | Magenta         |
| **T6**  | T7               | 6                  | Red + Green                 | Yellow          |
| **T7**  | T8               | 7                  | Red + Green + Blue          | White          
| **T8-10**  | END              | Random                  | None                        | Off             |

