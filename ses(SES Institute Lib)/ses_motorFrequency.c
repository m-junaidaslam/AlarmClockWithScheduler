/* INCLUDES ******************************************************************/

#include "ses_motorFrequency.h"
#include "ses_timer.h"
#include "ses_led.h"
#include "util/atomic.h"

/* DEFINES & MACROS **********************************************************/

#define GLOBAL_INTERRUPT_ENABLE			(1<<SREG_I)
#define INT0_ENABLE						(1<<INT0)
#define RISING_EDGE_TRIGGERED_INTERRUPT	0x03
#define NUMBER_OF_SPIKES				6
#define PRESCALAR_POSTSCALAR			250000
#define NUM_INTERVALS					19
#define CUT_OFF							3	//minimum measurable frequency.

pPortD0CallBack D0ExtIntCallBack;

uint8_t COUNT = NUMBER_OF_SPIKES;
uint16_t MY_TIME = 0;


/* Some extra functions made for implementing the other tasks */

uint16_t counter = NUM_INTERVALS - 1;
uint16_t my_Array[NUM_INTERVALS];
void motorStopped(void);
void swap();
void bubbleSort();
bool motorStatus;

/* FUNCTION DEFINITION *******************************************************/

void motorFrequency_init() {
	motorStatus = false;
	led_greenInit();
	led_greenOff();
	DDRG &= ~(1 << PD0);	//setting pin to input
	EICRA |= RISING_EDGE_TRIGGERED_INTERRUPT;
	EIMSK |= INT0_ENABLE;
	SREG |= GLOBAL_INTERRUPT_ENABLE;
	EIFR &= ~(1 << INTF0);
	timer5_start();
	timer5_setCallBack(&motorStopped);
}

uint16_t motorFrequency_getRecent() {
	if (motorStatus) {
		if (MY_TIME != 0) {
			return (600 / MY_TIME); // 1/(mytime/prescalar_postscalar)
		}
		return CUT_OFF; //Due to prescalar settings and spike settings

	}
	return 0;
}

uint16_t motorFrequency_getMedian() {

	for(int i =0; i < NUM_INTERVALS; i++) {
		my_Array[i] = motorFrequency_getRecent();
	}
	bubbleSort(my_Array, NUM_INTERVALS - 1);
	//counter = NUM_INTERVALS-1;
	return my_Array[NUM_INTERVALS / 2];
}

void set_portD0CallBack(pPortD0CallBack cb) {
	if (cb != NULL) {
		D0ExtIntCallBack = cb;
	}
}

ISR(INT0_vect) {
	if (D0ExtIntCallBack != NULL) {
		D0ExtIntCallBack();
		led_greenOff();
		motorStatus = true;
	}
	if (COUNT == 0) {
		COUNT = NUMBER_OF_SPIKES;
		MY_TIME = TCNT5;
		TCNT5 = 0;
	} else {
		COUNT--;
	}

}

void motorStopped(void) {
	led_greenOn();
	motorStatus = false;
}
/**
 * Algorithm used for sorting the array, to be used for median calculation.
 */
void swap(uint16_t *xp, uint16_t *yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

void bubbleSort(uint16_t arr[], uint16_t n) {
	int i, j;
	for (i = 0; i < n - 1; i++)

		// Last i elements are already in place
		for (j = 0; j < n - i - 1; j++)
			if (arr[j] > arr[j + 1])
				swap(&arr[j], &arr[j + 1]);
}
