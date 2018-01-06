
/*
 *  ======== pwmled.c ========
 */
/* For usleep() */
#include <unistd.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/PWM.h>

/* Example/Board Header files */
#include "Board.h"

//uint16_t   duty;

/*
 *  ======== mainThread ========
 *  Task periodically increments the PWM duty for the on board LED.
 */
void *pwmledMainThread(void *args)
{
    /* Period and duty in microseconds */
    uint16_t   pwmPeriod = 3000;
    uint16_t   duty = 0;
//    uint16_t   dutyInc = 100;
    uint32_t   time = 50000;

    PWM_Handle pwm1;
    PWM_Handle pwm2;

    /* Sleep time in microseconds */
    pwm1 = NULL;
    pwm2 = NULL;
    PWM_Params params;

    /* Call driver init functions. */
    PWM_init();

    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;

    pwm1 = PWM_open(Board_PWM0, &params);

    pwm2 = PWM_open(Board_PWM1, &params);

    if ( pwm1 != NULL && pwm2 != NULL ) {

        PWM_start(pwm1);
        PWM_start(pwm2);

        while ( 1 ) {
            PWM_setDuty(pwm1, duty);
            PWM_setDuty(pwm2, duty);

            usleep(time);
        }
    }
    return (NULL);
}
/*
 *  ======== mainThread ========
 *  Task periodically increments the PWM duty for the on board LED.
 */
void pwmled(uint16_t   newDuty)
{
/*
    if ( pwm1 == NULL || pwm2 == NULL )
        return;

    PWM_setDuty(pwm1, duty);
    PWM_setDuty(pwm2, duty);
*/
//    duty = newDuty;
}
