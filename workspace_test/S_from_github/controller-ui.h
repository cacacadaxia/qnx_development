#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdio.h>

#include <pthread.h>

#include "msg.h"
#include "timing.h"
#include "util.h"

#define INTERVAL 100000

#define INTERSECTION_COUNT 3

typedef struct {
	pthread_t thread;
	pthread_mutex_t lock;
	bool run;
	
	intersection_state_t *intersection_states;
	message_t *msg;
} thread_state_t;

bool thread_state_init(thread_state_t *state, intersection_state_t *intersection_states, message_t *msg);
void thread_state_clean(thread_state_t *state);

void *controller_state_thread(void *arg);

#endif
