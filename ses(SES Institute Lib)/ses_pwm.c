/* INCLUDES ******************************************************************/

#include "ses_pwm.h"
#include "ses_button.h"
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/

#define FAST_PWM_MODE_TIM0 		0x03
#define NO_PRESCALER_TIM0		0x01
#define TCCR_VAL_FOR_OCOB_SET	0x30
#define MOTOR_STOP 				0xFF

/* FUNCTION DEFINITION *******************************************************/
void pwm_init(void) {
	PRR0 &= ~(1 << PRTIM0);	//Start Timer0
	TCCR0A |= FAST_PWM_MODE_TIM0;	//Fast PWM Mode
	TCCR0B &= ~(1 << WGM02); //Forcefully setting the bit to zero to maintain the value of 0x03 for fast PWM mode.
	TCCR0B |= NO_PRESCALER_TIM0;//Using the original clock frequency Prescalar is Zero
	TCCR0A |= TCCR_VAL_FOR_OCOB_SET;//configurating Timer Control Register to set bit OC0B when timer value matches OCR0B
	DDRG |= (1 << PG5);	//Setting Pin 5 of PortG as output.
	OCR0B = MOTOR_STOP;

}

void pwm_setDutyCycle(uint8_t dutyCycle) {
	OCR0B = dutyCycle;	//Setting the Duty Cycle
}
