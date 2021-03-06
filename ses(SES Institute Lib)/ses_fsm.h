#ifndef SES_FSM_H_
#define SES_FSM_H_

/*INCLUDES *******************************************************************/
#include "ses_common.h"
#include "ses_scheduler.h"

/* TYPES ********************************************************************/
typedef struct event_s Event;		//< typedef for alarm clock state machine
typedef struct fsm_s Fsm;			//< event type for alarm clock fsm
typedef uint8_t fsmReturnStatus;	//< typedef to be used with enum below

enum returnStatus {
	RET_HANDLED,	//< event was handled
	RET_IGNORED,	//< event was ignored; not used in this implementation
	RET_TRANSITION	//< event was handled and a state transition occurred
};

enum eventsList {
	ENTRY,
	EXIT,
	JOYSTICK_PRESSED,
	ROTARY_PRESSED,
	ROTARY_CLOCKWISE,
	ROTARY_COUNTERCLCOCKWISE,
	TIME_MATCHED,
	TIMER_EXPIRED
};

/**
 * A state is represented by a function pointer,
 * called for each transition emanating in this state
 */
typedef fsmReturnStatus (*State) (Fsm *, const Event *);

/**
 * Base type for events
 */
struct event_s {
	int signal;		//< Identifies the type of event
};

/**
 * Base type for state machine
 */
struct fsm_s {
	State state;			//< Current state, pointer to event handler
	bool isAlarmEnabled;	//< Flag for the alarm status
	struct time_t timeSet;	//< Multi-purpose variable for system time and alarm time
};

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Dispatches events to state machine, called in application
 */
inline static void fsm_dispatch(Fsm*, const Event*);

/**
 * Sets and calls initial state of state machine
 */
inline static void fsm_init(Fsm*, State);

#endif /* SES_FSM_H_ */
