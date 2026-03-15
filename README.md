# Priority scheduler

## Overview

This project implements a Priority-Based Task Scheduler for an RGB LED system using function pointers and the Bubble Sort algorithm. The scheduler organizes tasks based on their priority level, ensuring that the most urgent tasks run first, while allowing each task to complete its full Burst Time before switching.

- **SW3:** Dynamically creates a new task with maximum priority (1) and a randomized burst time
- **SW2:** Deletes a random task that is currently in the READY state

Each task executes a specific LED behavior for a defined burst time. The project demonstrates non-preemptive priority scheduling, interrupt-driven task management (allowing dynamic creation and deletion of tasks via hardware buttons), and a priority-ordered execution flow

## Scheduling states

The scheduler manages tasks through three primary states:
- **READY:** The task is in the queue waiting for its turn.
- **RUNNING:** The task currently has control of the MCU.
- **BLOCKED:** The task has finished its execution and is waiting for a system reset.

## Scheduler

| Task ID | Priority | Burst Time | LEDs ON                | Perceived Color |
|--------|----------|-----------|------------------------|----------------|
| T1     | 2        | 5         | Red                    | Red            |
| T2     | 4        | 3         | Green                  | Green          |
| T3     | 3        | 2         | Blue                   | Blue           |
| T4     | 7        | 4         | Green + Blue           | Cyan           |
| T5     | 2        | 8         | Red + Blue             | Magenta        |
| T6     | 5        | 6         | Red + Green            | Yellow         |
| T7     | 3        | 7         | Red + Green + Blue     | White          |
| T8-10  | 1        | Random    | None                   | Off / Dynamic  |Off             |

