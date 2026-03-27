º#include <tasks.h>
#include "main.h"
#include <queuelib.h>
#include "stdlib.h"
#include <stdint.h>
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

QUEUE ready;

//Naked
void PendSV_Handler( void ) __attribute__ (( naked ));
//PendSV_Handler:
//0000120c:   mrs     r0, PSP

//Not Naked
//void PendSV_Handler( void );
//0000120a:   vraddhn.i<illegal       width 128>      d27, <illegal reg q15.5>, q0
//0000120e:   add     r7, sp, #0

#define TASK_PSP	0xFFFFFFFD

enum taskstatus
{
	INACTIVE = 0,
	BLOCK,
	READY,
	RUNNING,
	END
};

/* task Control Block */
typedef struct {
	void *stack;
	void *orig_stack;
	//uint8_t in_use;
	uint8_t status;

} tcb_t;

static tcb_t tasks[MAX_TASKS];
static int lastTask;
static int first = 1;
static int readyqueuestarted = 0;
static int taskended = 0;

/*
 The compiler does not generate prologue and epilogue sequences for functions with
 __attribute__((naked)).

Without naked attribute, GCC will corrupt R7 which is used for stack pointer.
If so, after restoring the tasks' context, we will get wrong stack pointer.
PendSV is not only an interrupt handler but where the context switch happens.
The R7 is usually used to preserve system call number (ISR number).

Without __attribute__((naked)), it looks like:
0000120a:   vraddhn.i<illegal       width 128>      d27, <illegal reg q15.5>, q0
0000120e:   add     r7, sp, #0

//Save the old task's context
	asm volatile("MRS      R0, PSP\n");
0000120c:   mrs     r0, PSP

But our pendsv_handler has context switch code at end of

   			// Move the task's stack pointer address into r0
40			asm volatile("MOV     R0, %0\n" : : "r" (tasks[lastTask].stack));
			// Restore the new task's context and jump to the task
41  		asm volatile("LDMIA   R0!, {R4-r11, LR}\n");
42			asm volatile("MSR     PSP, R0\n");
43			asm volatile("BX      LR\n");

The core register was changed here and we even have our own return code
(line 41 to 43)
that the compiler will never know this.

Compiler don't know we handle core register and stack in C function in
our own and will never assume user "corrupt" calling convention.
Compiler is doing its jobs performing regular push R7 and pop R7
Even in 7.2.1 arm-eabi compiler, there is still push R7 in
pendsv_handler

So it's reasonable to have naked attribute where we do the context
switch or have our own register/stack handling in C function.
 */

/* To get the task pointer address from result r0 */
//asm volatile("MOV      %0, R0\n" : "=r" (tasks[lastTask].stack));

void PendSV_Handler( void )
{
	if (tasks[lastTask].status == END)
	{
		/* o aqui, no se */
		/* solo cambian de ready a running */
		if (!_isqueueempty(&ready)) //If the ready queue is not empty then we look for more tasks.
		{
			/* Falta guardar el contexto de la tarea aqui */
			asm volatile("MOV      %0, R0\n" : "=r" (tasks[lastTask].stack));
			/* aprovechar el queue fifo */

			/* Find a new task to run */
			lastTask = _dequeue(&ready);
			tasks[lastTask].status = RUNNING;

			/* Move the task's stack pointer address into r0 */
			asm volatile("MOV     R0, %0\n" : : "r" (tasks[lastTask].stack));
			/* Restore the new task's context and jump to the task */
			asm volatile("LDMIA   R0!, {R4-R11, LR}\n");
			asm volatile("MSR     PSP, R0\n");
			asm volatile("BX      LR\n");
		}
		else //We return to the kernel (main() function) in case we finished all the tasks.
		{
			taskended = 1;
			/* load kernel state */
			asm volatile("POP     {R4, R5, R6, R7, R8, R9, R10, R11, IP, LR}  \n");
			asm volatile("MSR     PSR_NZCVQ, IP \n");
			asm volatile("BX      LR \n");
		}
	}
	else
	{
		//The current task continue execution
	}
}

void SysTick_Handler(void)
{
	//PRINTF("tick\r\n");
	if (tasks[lastTask].status == END)
	{
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}
}

void task_start()
{
	lastTask = _dequeue(&ready);
	tasks[lastTask].status = RUNNING;

	if (taskended)
	{
		return;
	}
//	int x = __get_CONTROL(); // Reads the current value of CONTROL
//	__set_CONTROL(x); // Sets the CONTROL value to x

	/* Save kernel context */
	asm volatile ("MRS    IP, PSR \n"); //ip and/or IP - Intra procedure call scratch register. This is a synonym for R12.
	asm volatile ("PUSH   {R4, R5, R6, R7, R8, R9, R10, R11, IP, LR} \n");

	/* To bridge the variable in C and the register in ASM,
	 * move the task's stack pointer address into r0.
	 * http://www.ethernut.de/en/documents/arm-inline-asm.html
	 */
	asm volatile("MOV     R0, %0\n" : : "r" (tasks[lastTask].stack));
	/* Load user task's context and jump to the task */
	asm volatile("MSR     PSP, R0\n");
	asm volatile("MOV     R0, #2\n");
	asm volatile("MSR     CONTROL, R0\n");
	asm volatile("ISB \n");
	asm volatile("POP     {R4-R11, LR}\n");
	asm volatile("POP     {R0}\n");
	asm volatile("BX      LR\n");
}

int task_create(void (*run)(void *), void *userdata)
{
	/* Find a free thing */
	int taskId = 0;
	uint32_t *stack;

	if (readyqueuestarted == 0)
	{
		//init ready queue
		_initqueue(&ready);
		readyqueuestarted = 1;
	}

	for (taskId = 0; taskId < MAX_TASKS; taskId++)
	{
		if (tasks[taskId].status == INACTIVE)
			break;
	}

	if (taskId == MAX_TASKS)
		return -1;

	/* Create the stack */
	stack = (uint32_t *) malloc(STACK_SIZE * sizeof(uint32_t));
	tasks[taskId].orig_stack = stack;
	if (stack == 0)
		return -1;

	stack += STACK_SIZE - 32; /* End of stack, minus what we are about to push */
	if (first) {
		stack[8] =  (unsigned int) run;
		stack[9] =  (unsigned int) userdata;
		first = 0;
	} else {
		stack[8] =  (unsigned int) TASK_PSP;
		stack[9] =  (unsigned int) userdata;
		stack[14] = (unsigned) &task_self_terminal;
		stack[15] = (unsigned int) run;
		stack[16] = (unsigned int) 0x01000000; /* PSR Thumb bit */
	}

	/* Construct the control block */
	tasks[taskId].stack = stack;
	tasks[taskId].status = READY;

	//add to the ready queue
	_enqueue(&ready, taskId);

	return taskId;
}

void task_kill(int task_id)
{
	tasks[task_id].status = END; //We end the task

	/* Free the stack */
	free(tasks[task_id].orig_stack);
}

void task_self_terminal()
{
	/* This will kill the stack.
	 * For now, disable context switches to save ourselves.

	CPS - Change Processor State.
	ID - Interrupt or abort disable.
	IE - Interrupt or abort enable.
	I - Enables or disables IRQ interrupts.
	*/
	asm volatile("CPSID   I\n"); //Change the state of the Cortex-M4 and dissable the interrupts.
	task_kill(lastTask);
	asm volatile("CPSIE   I\n"); //Change the state of the Cortex-M4 and enable the interrupts.

	/* And now wait for death to kick in */
	while (1);
}
