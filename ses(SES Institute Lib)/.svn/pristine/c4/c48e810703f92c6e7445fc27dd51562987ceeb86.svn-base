#ifndef SES_MOTORFREQUENCY_H_
#define SES_MOTORFREQUENCY_H_

/* INCLUDES ******************************************************************/
#include "ses_common.h"

typedef void (*pPortD0CallBack)();

/* FUNCTION PROTOTYPES *******************************************************/
/**
 *initialises the required timers and other peripherals for tasks related to the motor
 *and other requirements.
 */
void motorFrequency_init(void);
/**
 *returns the most recent frequency captured in Hertz
 *@return recent frequency(Hz)
 */
uint16_t motorFrequency_getRecent(void);
/**
 *returns the median calculated using the data structure and algorithm
 *@return median Frequency(Hz)
 */
uint16_t motorFrequency_getMedian(void);

void set_portD0CallBack(pPortD0CallBack cb);

#endif /* SES_MOTORFREQUENCY_H_ */
