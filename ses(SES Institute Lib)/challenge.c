#include "ses_common.h"
/**This is a so-called "naked" interrupt and thus the compiler
 * creates no additional assembler code when entering/ leaving
 * the function. Note that a "reti" (return from interrupt)
 * becomes necessary.
 *
 */
ISR(TIMER1_COMPA_vect, ISR_NAKED) {
	asm volatile (
			"reti \n\t"
	);

}

//------------------------------

/**In preemptive multi-tasking each
 * task needs an own stack.
 */
#define THREAD_STACK_SIZE	256


//------------------------------


/**The state of a task must be stored in
 * some struct
 */
struct context_t {
	uint8_t stack[THREAD_STACK_SIZE];
	void *pstack;
	struct context_t *next;
};

