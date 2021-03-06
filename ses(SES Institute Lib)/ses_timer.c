/* INCLUDES ******************************************************************/
#include "ses_timer.h"
#include "ses_scheduler.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC  	0x4E2
#define TIMER2_CYC_FOR_1MILLISEC	0xFA

#define CTC_MODE_MSK_TIM2			0x02
#define PRESCALER_64_MSK_TIM2	 	0x04

#define CTC_MODE_MSK_TIM1_A			(1<<WGM10)|(1<<WGM11)
#define CTC_MODE_MSK_TIM1_B			0x08
#define PRESCALER_64_MSK_TIM1	 	0x03

#define PRESCALAR_64_TIM5			0x03
#define CTC_MODE_MSK_TIM5_A			(1<<WGM51)|(1<<WGM50)
#define CTC_MODE_MSK_TIM5_B			0x03
#define TIMER5_CYC					0x6400

pTimerCallback timer2CallBack;
pTimerCallback timer1CallBack;
pTimerCallback timer5CallBack;

/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb) {
	timer2CallBack = cb;
}

void timer2_start() {
	PRR0 &= ~(1 << PRTIM2);
	TCCR2A |= CTC_MODE_MSK_TIM2;
	TCCR2B &= ~(1 << WGM22);
	TCCR2B |= PRESCALER_64_MSK_TIM2;
	TIFR2 |= (1 << OCF2A);
	OCR2A = TIMER2_CYC_FOR_1MILLISEC;
	TIMSK2 |= (1 << OCIE2A);
	sei();
}

void timer2_stop() {
	PRR0 |= (1 << PRTIM2);
	TIMSK2 &= ~(1 << OCIE2A);
}

void timer1_setCallback(pTimerCallback cb) {
	timer1CallBack = cb;
}

void timer1_start() {
	PRR0 &= ~(1 << PRTIM1);
	TCCR1A &= ~CTC_MODE_MSK_TIM1_A;
	TCCR1B |= CTC_MODE_MSK_TIM1_B;
	TCCR1B |= PRESCALER_64_MSK_TIM1;
	TIFR1 |= (1 << OCF1A);
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;
	TIMSK1 |= (1 << OCIE1A);
	sei();
}

void timer1_stop() {
	PRR0 |= (1 << PRTIM1);
	TIMSK1 &= ~(1 << OCIE1A);
}

void timer5_setCallBack(pTimerCallback cb) {
	timer5CallBack = cb;
}

void timer5_start() {
	PRR1 &= ~(1 << PRTIM5);
	TCCR5A &= ~CTC_MODE_MSK_TIM5_A;
	TCCR5B |= (1 << WGM52);
	TCCR5B &= ~(1 << WGM53);
	TCCR5B |= PRESCALAR_64_TIM5;
	TIFR5 |= (1 << OCF5A);
	OCR5A = TIMER5_CYC;
	TIMSK5 |= (1 << OCIE5A);
	sei();
}

void timer5_stop() {
	PRR1 |= (1 << PRTIM5);
	TIMSK5 &= ~(1 << OCIE5A);
}

ISR(TIMER1_COMPA_vect) {
	if (timer1CallBack != NULL) {
		timer1CallBack();
	}
}

ISR(TIMER2_COMPA_vect) {
	if (timer2CallBack != NULL) {
		timer2CallBack();
	}
}
ISR(TIMER5_COMPA_vect) {
	if (timer5CallBack != NULL) {
		timer5CallBack();
	}
}
