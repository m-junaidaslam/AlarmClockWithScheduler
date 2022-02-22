#ifndef SES_ROTARY_H_
#define SES_ROTARY_H_

/* INCLUDES ******************************************************************/
#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder
 */
void rotary_init();

/** 
 * Get the state of the rotary encoder input A.
 */
bool rotary_isInputAHigh(void);

/** 
 * Get the state of the rotary encoder input B.
 */
bool rotary_isInputBHigh(void);


typedef void (*pTypeRotaryCallback)();
void rotary_setClockwiseCallback(pTypeRotaryCallback);
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback);

/**
 * Check button state using debouncing technique
 */
void rotary_checkState();

#endif /* SES_ROTARY_H_ */
