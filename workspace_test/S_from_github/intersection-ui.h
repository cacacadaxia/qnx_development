#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <stdio.h>

#include <pthread.h>

#include "msg.h"
#include "timing.h"
#include "util.h"

#define INTERVAL 1

typedef struct {
	pthread_t thread;
	pthread_mutex_t lock;
	bool run;
	
	/* Light states (global) and intersection state */
	intersection_light_t *light_states;
	intersection_state_t *intersection;
	
	/* Message queues */
	message_t msg;
} thread_state_t;

bool thread_state_init(thread_state_t *state, intersection_light_t *light_states, intersection_state_t *intersection, int id);
void thread_state_clean(thread_state_t *state);

void *light_state_thread(void *arg);

#endif
