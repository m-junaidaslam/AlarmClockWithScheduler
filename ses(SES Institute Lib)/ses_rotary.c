/* INCLUDES ******************************************************************/

#include "ses_rotary.h"
#include "ses_timer.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// Buttons wiring on SES board
#define A_ROTARY_PORT      			PORTB
#define A_ROTARY_PIN         		5

#define B_ROTARY_PORT      			PORTG
#define B_ROTARY_PIN         		2

//Enabling Interrupt mask
#define MSK0_ENABLE					0x01

//Number of rotary state checks
#define ROTARY_NUM_DEBOUNCE_CHECKS 	5

//Call Back
pTypeRotaryCallback rotaryClockwiseCallback;
pTypeRotaryCallback rotaryCounterClockwiseCallback;
/* FUNCTION DEFINITION *******************************************************/

void rotary_init() {
	DDR_REGISTER(A_ROTARY_PORT) &= ~(1 << A_ROTARY_PIN);
	DDR_REGISTER(B_ROTARY_PORT) &= ~(1 << B_ROTARY_PIN);
	A_ROTARY_PORT |= (1 << A_ROTARY_PIN);
	B_ROTARY_PORT |= (1 << B_ROTARY_PIN);

	sei();
}

bool rotary_isInputAHigh(void) {
	return !(PIN_REGISTER(A_ROTARY_PORT) & (1 << A_ROTARY_PIN));
}

bool rotary_isInputBHigh(void) {
	return !(PIN_REGISTER(B_ROTARY_PORT) & (1 << B_ROTARY_PIN));
}

void rotary_setClockwiseCallback(pTypeRotaryCallback callback) {
	rotaryClockwiseCallback = callback;
}
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback) {
	rotaryCounterClockwiseCallback = callback;
}

void rotary_checkState() {
	static uint8_t state[ROTARY_NUM_DEBOUNCE_CHECKS] = { };
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	// each bit in every state byte represents one button
	state[index] = 0;
	if (rotary_isInputAHigh()) {
		state[index] |= 1;
	}
	if (rotary_isInputBHigh()) {
		state[index] |= 2;
	}
	index++;
	if (index == ROTARY_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}
	// init compare value and compare with ALL reads, only if
	// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
	// array, the button at this position is considered pressed
	uint8_t j = 0xFF;
	for (uint8_t i = 0; i < ROTARY_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}
	debouncedState = j;

	if (((lastDebouncedState ^ (0x01)) && (lastDebouncedState ^ (0x02))
			&& (debouncedState & (0x01))) && (debouncedState ^ (0x02))) {
		if (rotaryClockwiseCallback != NULL) {
			rotaryClockwiseCallback();
		}
	}
	if (((lastDebouncedState & (0x01)) && (lastDebouncedState ^ (0x02)) && (debouncedState & (0x02)))
			&& (debouncedState ^ (0x01))) {
		if (rotaryCounterClockwiseCallback != NULL) {
			rotaryCounterClockwiseCallback();
		}
	}
}
