#ifndef SES_PWM_H_
#define SES_PWM_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initialises PWM Speed control
 */
void pwm_init(void);

/**
 * Sets a required Duty Cycle
 */
void pwm_setDutyCycle(uint8_t dutyCycle);

#endif /* SES_BUTTON_H_ */
