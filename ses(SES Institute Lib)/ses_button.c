/* INCLUDES ******************************************************************/

#include "ses_button.h"
#include "ses_timer.h"
#include "ses_lcd.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// Buttons wiring on SES board
#define BUTTON_PORT      			PORTB

#define BUTTON_ROTARY_PIN         	6

#define BUTTON_JOYSTICK_PIN	      	7


//Enabling Interrupt mask
#define MSK0_ENABLE					0x01

//Number of debounce checks
#define BUTTON_NUM_DEBOUNCE_CHECKS 	5

//Call Back
pButtonCallback buttonRotaryCallBack;

pButtonCallback buttonJoystickCallBack;
/* FUNCTION DEFINITION *******************************************************/

void button_init(bool debouncing) {
	DDR_REGISTER(BUTTON_PORT) &= ~(1 << BUTTON_ROTARY_PIN);
	DDR_REGISTER(BUTTON_PORT) &= ~(1 << BUTTON_JOYSTICK_PIN);
	BUTTON_PORT |= (1 << BUTTON_ROTARY_PIN);
	BUTTON_PORT |= (1 << BUTTON_JOYSTICK_PIN);

	if (debouncing) {
		//initialization for debouncing
		//timer1_start();
		//timer1_setCallback(&button_checkState);
	} else {
		//initialization for direct interrupts (e.g. setting up the PCICR register)
		// For interrupts
		PCICR |= MSK0_ENABLE;
		PCMSK0 |= ((1 << BUTTON_ROTARY_PIN) | (1 << BUTTON_JOYSTICK_PIN));
	}
	sei();
}

bool button_isJoystickPressed(void) {
	return !(PIN_REGISTER(BUTTON_PORT) & (1 << BUTTON_JOYSTICK_PIN));
}

bool button_isRotaryPressed(void) {
	return !(PIN_REGISTER(BUTTON_PORT) & (1 << BUTTON_ROTARY_PIN));
}

void button_setRotaryButtonCallback(pButtonCallback callback) {
	buttonRotaryCallBack = callback;
}
void button_setJoystickButtonCallback(pButtonCallback callback) {
	buttonJoystickCallBack = callback;
}

ISR(PCINT0_vect) {
	if (button_isJoystickPressed()) {
		buttonJoystickCallBack();
	}

	if (button_isRotaryPressed()) {
		buttonRotaryCallBack();
	}
}

void button_checkState() {
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = { };
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	// each bit in every state byte represents one button
	state[index] = 0;
	if (button_isJoystickPressed()) {
		state[index] |= 1;
	}
	if (button_isRotaryPressed()) {
		state[index] |= 2;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}
	// init compare value and compare with ALL reads, only if
	// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
	// array, the button at this position is considered pressed
	uint8_t j = 0xFF;
	for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}
	debouncedState = j;

	/*	if ((lastDebouncedState & 1) & (!(debouncedState & 1))) {
	 buttonJoystickCallBack();
	 }

	 if (((lastDebouncedState >> 1) & 1) & (~((debouncedState >> 1) & 1))) {
	 buttonRotaryCallBack();
	 }*/

	if ((!(lastDebouncedState & 1)) && (debouncedState & 1)) {
		if(buttonJoystickCallBack!=NULL)
			buttonJoystickCallBack();
	}

	if ((!(lastDebouncedState & 2)) && (debouncedState & 2)) {
		if(buttonRotaryCallBack!=NULL)
			buttonRotaryCallBack();
	}
}
