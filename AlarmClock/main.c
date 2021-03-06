#include "ses_button.h"
#include "ses_led.h"
#include "ses_pwm.h"
#include "ses_scheduler.h"
#include "ses_timer.h"
#include "ses_fsm.h"
//#include "ses_uart.h"
#include "ses_lcd.h"
#include "ses_rotary.h"

#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION);

Fsm fsmAlarmClock;			// Alarm clock state machine
myTime systemTime;
myTime alarmTime;
bool alarmTimeSet = false;
taskDescriptor tdDisplaySystemTime;
taskDescriptor tdCompareAlarmSystemTime;
taskDescriptor tdTimer5Sec;
taskDescriptor tdRedLedBlink4Hz;
taskDescriptor tdRedLedOff;

void joystickPressed(void * param);
void rotaryPressed(void * param);
void rotaryClockwise(void * param);
void rotaryCounterClockwise(void * param);
void displaySystemTime(void * param);
void compareAlarmSystemTime(void * param);
void timer5SecExpired(void * param);
void toggleRedLED(void * param);
void offRedLED(void * param);

void updateLCDTime(bool, bool);

// States of alarm clock
fsmReturnStatus setSystemTime(Fsm *, const Event *);
fsmReturnStatus systemHoursSet(Fsm *, const Event *);
fsmReturnStatus systemTimeDisplayAlarmOff(Fsm *, const Event *);
fsmReturnStatus systemTimeDisplayAlarmOnNotSet(Fsm *, const Event *);
fsmReturnStatus systemTimeDisplayAlarmOnSet(Fsm *, const Event *);
fsmReturnStatus alarmOccured(Fsm *, const Event *);
fsmReturnStatus setAlarmTime(Fsm *, const Event *);
fsmReturnStatus alarmHoursSet(Fsm *, const Event *);

int main(void) {
	button_init(true);
	led_greenInit();
	led_redInit();
	led_yellowInit();
	lcd_init();
	//uart_init(57600);
	scheduler_init();
	fsmAlarmClock.isAlarmEnabled = false;
	fsm_init(&fsmAlarmClock, setSystemTime);

	button_setRotaryButtonCallback(&rotaryPressed);
	button_setJoystickButtonCallback(&joystickPressed);

	tdDisplaySystemTime.task = &displaySystemTime;
	tdDisplaySystemTime.period = 1000;

	tdCompareAlarmSystemTime.task = &compareAlarmSystemTime;
	tdCompareAlarmSystemTime.period = 1000;

	tdTimer5Sec.task = &timer5SecExpired;
	tdTimer5Sec.period = 0;

	tdRedLedBlink4Hz.task = &toggleRedLED;
	tdRedLedBlink4Hz.period = 250;

	tdRedLedOff.task = &offRedLED;
	tdRedLedOff.expire = 0;

	taskDescriptor tdCheckButtonsState;
	tdCheckButtonsState.task = &button_checkState;
	tdCheckButtonsState.period = 10;
	scheduler_add(&tdCheckButtonsState);

	taskDescriptor tdCheckRotaryState;
	tdCheckRotaryState.task = &rotary_checkState;
	tdCheckRotaryState.period = 10;
	scheduler_add(&tdCheckRotaryState);

	while(1) {
		scheduler_run();
	}
	return 0;
}

void joystickPressed(void * param) {
	Event e = {.signal = JOYSTICK_PRESSED};
	fsm_dispatch(&fsmAlarmClock, &e);
}

void rotaryPressed(void * param) {
	Event e = {.signal = ROTARY_PRESSED};
	fsm_dispatch(&fsmAlarmClock, &e);
}

void rotaryClockwise(void * param) {
	Event e = {.signal = ROTARY_CLOCKWISE};
	fsm_dispatch(&fsmAlarmClock, &e);
}

void rotaryCounterClockwise(void * param) {
	Event e = {.signal = ROTARY_COUNTERCLCOCKWISE};
	fsm_dispatch(&fsmAlarmClock, &e);
}

/**
 * Displays time on LCD in the formt of HH:MM:SS
 * and can also differentiate what format to display if alarm is enabled or not.
 */
void displaySystemTime(void * param) {
	bool lineOne = (bool) param;
	if(lineOne) {
		lcd_setCursor(0, 0);
	} else {	
		lcd_setCursor(0, 1);
	}

	systemTime = getReadableTime();
	fprintf(lcdout, "%d:%d:%d            ", systemTime.hour, systemTime.minute, systemTime.second);
	if(lineOne) {
		if(fsmAlarmClock.isAlarmEnabled) {
			lcd_setCursor(0, 1);
			fprintf(lcdout, "Alarm: %d:%d        ", alarmTime.hour, alarmTime.minute);
		}

	}
	led_greenToggle();
}
/**
 * It acts whenever the current time matches the alarm time and acts according to the alarm conditions
 */
void compareAlarmSystemTime(void * param) {
	if((systemTime.hour == alarmTime.hour) && (systemTime.minute == alarmTime.minute)) {
		Event e = {.signal = TIME_MATCHED};
		fsm_dispatch(&fsmAlarmClock, &e);
	}
}
/**
 * waits for 5 seconds beofre timer expires
 */
void timer5SecExpired(void * param) {
	Event e = {.signal = TIMER_EXPIRED};
	fsm_dispatch(&fsmAlarmClock, &e);
}

void toggleRedLED(void * param) {
	led_redToggle();
}

void offRedLED(void * param) {
	led_redOff();
}

void updateLCDTime(bool isSystemTime, bool isHourChanged) {
	lcd_setCursor(0, 0);
	if(isHourChanged & isSystemTime) {
		fprintf(lcdout, "%d:MM                ", systemTime.hour);
	} else if(isHourChanged & (!isSystemTime)) {
		fprintf(lcdout, "%d:MM                ", alarmTime.hour);
	} else if((!isHourChanged) & isSystemTime) {
		fprintf(lcdout, "%d:%d                ", systemTime.hour, systemTime.minute);
	} else if((!isHourChanged) & (!isSystemTime)) {
		fprintf(lcdout, "%d:%d                ", alarmTime.hour, alarmTime.minute);
	}

	if(isSystemTime) {
		lcd_setCursor(0, 1);
		fprintf(lcdout, "Enter Time!         ");
	}
}

inline static void fsm_dispatch(Fsm* fsm, const Event* event) {
	static Event entryEvent = {.signal = ENTRY};
	static Event exitEvent = {.signal = EXIT};
	State s = fsm->state;
	fsmReturnStatus r = fsm->state(fsm, event);
	if(r == RET_TRANSITION) {
		s(fsm, &exitEvent);				//< Call exit action of last state
		fsm->state(fsm, &entryEvent);	//< Call entry action of new state
	}
}

inline static void fsm_init(Fsm* fsm, State init) {
	// ... Other initialization
	Event entryEvent = {.signal = ENTRY};
	fsm->state = init;
	fsm->state(fsm, &entryEvent);
}

/**
 * prompts the user to enter the time, and then sets the time accordingly to the
 * events that are happening. it also displays the time on the LCD in the required format.
 */
fsmReturnStatus setSystemTime(Fsm * fsm, const Event * event) {
	switch(event->signal) {
	case ENTRY:
		lcd_setCursor(0, 0);
		fprintf(lcdout, "HH:MM                ");
		lcd_setCursor(0, 1);
		fprintf(lcdout, "Enter Time!         ");
		systemTime.hour = 0;
		systemTime.minute = 0;
		systemTime.second = 0;
		systemTime.milli = 0;
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		return TRANSITION(systemHoursSet);
	case ROTARY_PRESSED:
		if(systemTime.hour < 24) {
			systemTime.hour++;
		} else {
			systemTime.hour = 0;
		}
		updateLCDTime(true, true);
		return RET_HANDLED;
	case ROTARY_CLOCKWISE:
		if(systemTime.hour < 24) {
			systemTime.hour++;
		} else {
			systemTime.hour = 0;
		}
		updateLCDTime(true, true);
		return RET_HANDLED;
	case ROTARY_COUNTERCLCOCKWISE:
		if(systemTime.hour > 0) {
			systemTime.hour--;
		} else {
			systemTime.hour = 23;
		}
		updateLCDTime(true, true);
		return RET_HANDLED;
	case EXIT:
		alarmTimeSet = false;
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}


/**
 * this state is entered after the system hours are set and now the seconds want to be set.
 */
fsmReturnStatus systemHoursSet(Fsm * fsm, const Event * event) {
	systemTime_t tempTime;
	switch(event->signal) {
	case ENTRY:
		lcd_setCursor(0, 0);
		fprintf(lcdout, "%d:MM                ", systemTime.hour);
		lcd_setCursor(0, 1);
		fprintf(lcdout, "Enter Time!         ");
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		tempTime = ((systemTime.hour * 3600) + (systemTime.minute * 60));
		scheduler_setTime(tempTime);
		tdDisplaySystemTime.param = (void *) true;
		scheduler_add(&tdDisplaySystemTime);
		return TRANSITION(systemTimeDisplayAlarmOff);
	case ROTARY_PRESSED:
		if(systemTime.minute < 60) {
			systemTime.minute++;
		} else {
			systemTime.minute = 0;
		}
		updateLCDTime(true, false);
		return RET_HANDLED;
	case ROTARY_CLOCKWISE:
		if(systemTime.minute < 60) {
			systemTime.minute++;
		} else {
			systemTime.minute = 0;
		}
		updateLCDTime(true, false);
		return RET_HANDLED;
	case ROTARY_COUNTERCLCOCKWISE:
		if(systemTime.minute > 0) {
			systemTime.minute--;
		} else {
			systemTime.minute = 59;
		}
		updateLCDTime(true, false);
		return RET_HANDLED;
	case EXIT:
		lcd_clear();
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}
/**
 *
 */
fsmReturnStatus systemTimeDisplayAlarmOff(Fsm * fsm, const Event * event) {
	switch(event->signal) {
	case ENTRY:
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		return TRANSITION(setAlarmTime);
	case ROTARY_PRESSED:
		if(alarmTimeSet) {
			return TRANSITION(systemTimeDisplayAlarmOnSet);
		} else {
			return TRANSITION(systemTimeDisplayAlarmOnNotSet);
		}
		return RET_HANDLED;
	case EXIT:
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}
/**
 * clock display is in the required format but alarm time is not set.
 */
fsmReturnStatus systemTimeDisplayAlarmOnNotSet(Fsm * fsm, const Event * event) {
	switch(event->signal) {
	case ENTRY:
		fsmAlarmClock.isAlarmEnabled = true;
		led_yellowOn();
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		return TRANSITION(setAlarmTime);
	case ROTARY_PRESSED:
		return TRANSITION(systemTimeDisplayAlarmOff);
	case EXIT:
		fsmAlarmClock.isAlarmEnabled = false;
		led_yellowOff();
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}
/**
 * Displaying time and the alarm time also after the time of alarm is sset.
 */
fsmReturnStatus systemTimeDisplayAlarmOnSet(Fsm * fsm, const Event * event) {
	switch(event->signal) {
	case ENTRY:
		fsmAlarmClock.isAlarmEnabled = true;
		led_yellowOn();
		scheduler_add(&tdCompareAlarmSystemTime);
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		return TRANSITION(setAlarmTime);
	case ROTARY_PRESSED:
		return TRANSITION(systemTimeDisplayAlarmOff);
	case TIME_MATCHED:
		return TRANSITION(alarmOccured);
	case EXIT:
		scheduler_remove(&tdCompareAlarmSystemTime);
		fsmAlarmClock.isAlarmEnabled = false;
		led_yellowOff();
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}
/**
 * When alarm occurs, now the system waits for any button press or 5 seconds passed whatever comes first.
 */
fsmReturnStatus alarmOccured(Fsm * fsm, const Event * event) {
	switch(event->signal) {
	case ENTRY:
		tdTimer5Sec.expire = 5000;
		scheduler_add(&tdTimer5Sec);
		scheduler_add(&tdRedLedBlink4Hz);
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		return TRANSITION(systemTimeDisplayAlarmOff);
	case ROTARY_PRESSED:
		return TRANSITION(systemTimeDisplayAlarmOff);
	case TIMER_EXPIRED:
		return TRANSITION(systemTimeDisplayAlarmOff);
	case EXIT:
		scheduler_remove(&tdRedLedBlink4Hz);
		scheduler_remove(&tdTimer5Sec);
		scheduler_add(&tdRedLedOff);
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}
/**
 * after alarm is enabled, now the time at which we want the alarm is to be set.
 * hors are handled here/.
 */
fsmReturnStatus setAlarmTime(Fsm * fsm, const Event * event) {
	switch(event->signal) {
	case ENTRY:
		alarmTime.hour = 0;
		alarmTime.minute = 0;
		alarmTime.second = 0;
		alarmTime.milli = 0;
		lcd_setCursor(0, 0);
		fprintf(lcdout, "HH:MM                ");
		scheduler_remove(&tdDisplaySystemTime);
		tdDisplaySystemTime.param = (void *) false;
		scheduler_add(&tdDisplaySystemTime);
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		return TRANSITION(alarmHoursSet);
	case ROTARY_PRESSED:
		if(alarmTime.hour < 24) {
			alarmTime.hour++;
		} else {
			alarmTime.hour = 0;
		}
		updateLCDTime(false, true);
		return RET_HANDLED;
	case ROTARY_CLOCKWISE:
		if(alarmTime.hour < 24) {
			alarmTime.hour++;
		} else {
			alarmTime.hour = 0;
		}
		updateLCDTime(false, true);
		return RET_HANDLED;
	case ROTARY_COUNTERCLCOCKWISE:
		if(alarmTime.hour > 0) {
			alarmTime.hour--;
		} else {
			alarmTime.hour = 23;
		}
		updateLCDTime(false, true);
		return RET_HANDLED;
	case EXIT:
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}
/**
 * after the hours are set the minutes can be set here in this state.
 */
fsmReturnStatus alarmHoursSet(Fsm * fsm, const Event * event) {
	switch(event->signal) {
	case ENTRY:
		lcd_setCursor(0, 0);
		fprintf(lcdout, "%d:MM                ", alarmTime.hour);
		return RET_HANDLED;
	case JOYSTICK_PRESSED:
		return TRANSITION(systemTimeDisplayAlarmOff);
	case ROTARY_PRESSED:
		if(alarmTime.minute < 60) {
			alarmTime.minute++;
		} else {
			alarmTime.minute = 0;
		}
		updateLCDTime(false, false);
		return RET_HANDLED;
	case ROTARY_CLOCKWISE:
		if(alarmTime.minute < 60) {
			alarmTime.minute++;
		} else {
			alarmTime.minute = 0;
		}
		updateLCDTime(false, false);
		return RET_HANDLED;
	case ROTARY_COUNTERCLCOCKWISE:
		if(alarmTime.minute > 0) {
			alarmTime.minute--;
		} else {
			alarmTime.minute = 59;
		}
		updateLCDTime(false, false);
		return RET_HANDLED;
	case EXIT:
		scheduler_remove(&tdDisplaySystemTime);
		alarmTimeSet = true;
		lcd_clear();
		tdDisplaySystemTime.param = (void *) true;
		scheduler_add(&tdDisplaySystemTime);
		return RET_HANDLED;
	default:
		return RET_IGNORED;
	}
}
