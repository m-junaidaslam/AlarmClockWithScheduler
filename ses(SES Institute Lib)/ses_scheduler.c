/*INCLUDES ************************************************************/
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "util/atomic.h"

/* PRIVATE VARIABLES **************************************************/
/** list of scheduled tasks */
static taskDescriptor* taskList = NULL;
static systemTime_t currentTime = 0;

/*FUNCTION DEFINITION *************************************************/
static void scheduler_update(void) {
	currentTime++;
	if(taskList != NULL) {
		taskDescriptor* temp_td = taskList;
		do {
			if(temp_td->expire > 0) {
				temp_td->execute = true;
			}
			temp_td = temp_td->next;
		} while((temp_td != NULL));
	}
}

void scheduler_init() {
	timer2_setCallback(&scheduler_update);
	timer2_start();
	free(taskList);
}

void scheduler_run() {
	if(taskList != NULL) {
		taskDescriptor* temp_td = taskList;
		do {
			if(temp_td->execute) {
				temp_td->expire--;
				temp_td->execute = false;
			}
			if(temp_td->expire <= 0) {
				temp_td->task(temp_td->param);
				if(temp_td->period > 0) {
					temp_td->expire = temp_td->period;
				} else {
					scheduler_remove(temp_td);
				}
			}
			temp_td = temp_td->next;
		} while(temp_td != NULL);
	}
}

bool scheduler_add(taskDescriptor * toAdd) {
	if(toAdd == NULL) {
		return false;
	}

	if(taskList == NULL) {
		taskList = toAdd;
	} else {
		taskDescriptor* temp_td = taskList;
		while(temp_td->next != NULL) {
			if(temp_td->task == toAdd->task) {
				return false;
			}
			temp_td = temp_td->next;
		}
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			// this block is executed atomically
			temp_td->next = toAdd;
		}
	}
	return true;
}

void scheduler_remove(taskDescriptor * toRemove) {
	if(taskList != NULL) {
		if(taskList->task == toRemove->task) {
			taskList = taskList->next;
		} else {
			taskDescriptor* temp_tdCurrent = taskList->next;
			taskDescriptor* temp_tdLast = taskList;
			do {
				if(temp_tdCurrent->task == toRemove->task) {
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
						// this block is executed atomically
						temp_tdLast->next = temp_tdCurrent->next;
					}

				}
				temp_tdLast = temp_tdCurrent;
				temp_tdCurrent = temp_tdCurrent->next;
			} while(temp_tdCurrent != NULL);
		}
	}
}

int taskcount() {
	int taskcount = 0;
	if(taskList != NULL) {
		taskDescriptor * tempList = taskList;
		do {
			taskcount++;
			tempList = tempList->next;
		} while(tempList != NULL);
	}
	return taskcount;
}

systemTime_t scheduler_getTime() {
	return currentTime;
}

void scheduler_setTime(systemTime_t time) {
	currentTime = time * 1000;
}

myTime getReadableTime() {
	myTime readableTime;
	uint32_t tempTime = 0;
	readableTime.milli = currentTime % 1000;
	tempTime = currentTime / 1000;				// Time in seconds
	readableTime.second = tempTime % 60;
	tempTime = tempTime / 60;					// Time in minutes
	readableTime.hour = tempTime / 60;
	readableTime.minute = tempTime % 60;
	return readableTime;
}

void clearTaskList() {
	free(taskList);
}
