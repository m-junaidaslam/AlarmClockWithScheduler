/* INCLUDES ******************************************************************/

#include "ses_adc.h"

/* DEFINES & MACROS **********************************************************/
#define ADC_VREF_SRC			1.6

// ADC wiring on SES board
#define ADC_PORT      			PORTF

/* FUNCTION DEFINITION *******************************************************/

void adc_init(void) {
	DDR_REGISTER(ADC_PORT) &= ~(1 << ADC_MIC_NEG_CH);
	DDR_REGISTER(ADC_PORT) &= ~(1 << ADC_MIC_POS_CH);
	DDR_REGISTER(ADC_PORT) &= ~(1 << ADC_TEMP_CH);
	DDR_REGISTER(ADC_PORT) &= ~(1 << ADC_LIGHT_CH);
	DDR_REGISTER(ADC_PORT) &= ~(1 << ADC_JOYSTICK_CH);

	ADC_PORT &= ~(1 << ADC_MIC_NEG_CH);
	ADC_PORT &= ~(1 << ADC_MIC_POS_CH);
	ADC_PORT &= ~(1 << ADC_TEMP_CH);
	ADC_PORT &= ~(1 << ADC_LIGHT_CH);
	ADC_PORT &= ~(1 << ADC_JOYSTICK_CH);

	PRR0 &= ~(1 << PRADC);


	// Set reference voltages to internal 1.6V
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << REFS1);

	ADMUX &= ~(1 << ADLAR);	// Make ADC right adjusted

	//mask it with prescale for 2Mhz (0x03)
	ADCSRA |= ADC_PRESCALE;

	//clear auto triggering
	ADCSRA &= ~(1 << ADATE);

	//Enable ADC
	ADCSRA |= (1 << ADEN);

}

/**
 * Read the raw ADC value of the given channel
 * @adc_channel The channel as element of the ADCChannels enum
 * @return The raw ADC value
 */
uint16_t adc_read(uint8_t adc_channel) {
	if(adc_channel < ADC_NUM) {
		ADMUX &= ~(0X1F);
		ADMUX |= adc_channel;
		ADCSRA |= (1 << ADSC);
		while(ADCSRA & (1 << ADSC)) {
			asm volatile ("nop");	//One cycle with no operation
		}
		uint16_t tempADC = ADCL;
		return (tempADC | (ADCH << 8));
	} else {
		return ADC_INVALID_CHANNEL;
	}
}

/**
 * Read the current joystick direction
 * @return The direction as element of the JoystickDirections enum
 */
uint8_t adc_getJoystickDirection() {
	uint16_t tempADC = adc_read(ADC_JOYSTICK_CH);
	uint8_t joyTolerance = 30;
	if((tempADC > (200 - joyTolerance)) & (tempADC < (200 + joyTolerance))) {
		return RIGHT;
	} else if ((tempADC > (400 - joyTolerance)) & (tempADC < (400 + joyTolerance))) {
		return UP;
	} else if ((tempADC > (600 - joyTolerance)) & (tempADC < (600 + joyTolerance))) {
		return LEFT;
	} else if ((tempADC > (800 - joyTolerance)) & (tempADC < (800 + joyTolerance))) {
		return DOWN;
	} else {
		return NO_DIRECTION;
	}
}

/**
 * Read the current temperature
 * @return Temperature in tenths of degree celsius
 */
	//int16_t adc = adc_read(ADC_TEMP_CH);
	/*int16_t slope = (ADC_TEMP_MAX - ADC_TEMP_MIN) / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);
	int16_t offset = ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope);
	return (adc * slope + offset) / ADC_TEMP_FACTOR;*/
	//return 0;
//int16_t adc_getTemperature() {
//	int16_t adc = adc_read(ADC_TEMP_CH);
//	/*int16_t slope = (ADC_TEMP_MAX - ADC_TEMP_MIN) / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN);
//	int16_t offset = ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope);
//	return (adc * slope + offset) / ADC_TEMP_FACTOR;*/
//	return 0;
//}
