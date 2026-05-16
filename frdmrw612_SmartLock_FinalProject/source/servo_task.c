#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "fsl_ctimer.h"
#include "servo_task.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef CTIMER_MAT_PWM_PERIOD_CHANNEL
#define CTIMER_MAT_PWM_PERIOD_CHANNEL kCTIMER_Match_3
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint32_t timerClock;
volatile uint32_t g_pwmPeriod   = 0U;
volatile uint32_t g_pulsePeriod = 0U;
//TODO DSOAE extern the Queue Handler for the servo queue

#define CLOSESERVO   0
#define OPENSERVO    1
/*******************************************************************************
 * Code
 ******************************************************************************/
status_t CTIMER_GetPwmPeriodValue(uint32_t pwmFreqHz, uint8_t dutyCyclePercent, uint32_t timerClock_Hz)
{
    /* Calculate PWM period match value */
    g_pwmPeriod = (timerClock_Hz / pwmFreqHz) - 1U;

    /* Calculate pulse width match value */
    g_pulsePeriod = (g_pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

void init_pwm()
{
    ctimer_config_t config;
    uint32_t srcClock_Hz;

    /* CTimer0 counter uses the AHB clock, some CTimer1 modules use the Aysnc clock */
    srcClock_Hz = CTIMER_CLK_FREQ;

    PRINTF("CTimer example to generate a PWM signal\r\n");

    CTIMER_GetDefaultConfig(&config);
    timerClock = srcClock_Hz / (config.prescale + 1);

    CTIMER_Init(CTIMER, &config);

    /* Get the PWM period match value and pulse width match value of 20Khz PWM signal with 20% dutycycle */
    CTIMER_GetPwmPeriodValue(50, (uint8_t)DUTY_CYCLE_10, timerClock);
    CTIMER_SetupPwmPeriod(CTIMER, CTIMER_MAT_PWM_PERIOD_CHANNEL, CTIMER_MAT_OUT, g_pwmPeriod, g_pulsePeriod, false);
    CTIMER_StartTimer(CTIMER);
}

void move_Servo(char value)
{

	//Stop PWM signel before changing
	CTIMER_StopTimer(CTIMER);

	//Configure with new value depending on the parameter
	if (value == OPENSERVO)
	{
		/* Get the PWM period match value and pulse width match value of 50hz PWM signal with 20% dutycycle */
		CTIMER_GetPwmPeriodValue(50, (uint8_t)DUTY_CYCLE_2, timerClock);
	}
	else
	{
		/* Get the PWM period match value and pulse width match value of 50hz PWM signal with 90% dutycycle */
		CTIMER_GetPwmPeriodValue(50, (uint8_t)DUTY_CYCLE_10, timerClock);
	}
    CTIMER_SetupPwmPeriod(CTIMER, CTIMER_MAT_PWM_PERIOD_CHANNEL, CTIMER_MAT_OUT, g_pwmPeriod, g_pulsePeriod, false);
    CTIMER_StartTimer(CTIMER);
}

void servo_task(void *param)
{
	char cmd;

	PRINTF("Servo Task Started.\r\n");
    init_pwm();

    while (1)
    {
    	//TODO DSOAE wait for new messages on servo_queue using xQueueReceive
		//while (xQueueReceive(servo_queue, &cmd, portMAX_DELAY) == pdTRUE)
    	while (0)
		{
			switch (cmd)
			{
				case 'o':
					PRINTF("Open servo\r\n");
					move_Servo (OPENSERVO);
					vTaskDelay(2000);
					PRINTF("Closing servo...\r\n");
					move_Servo (CLOSESERVO);
				break;
				case 'c':
					PRINTF("Close servo\r\n");
					move_Servo (CLOSESERVO);
				break;
				default:
				break;
			}
		}

    }
}
