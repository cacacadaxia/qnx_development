#include "controller-ui.h"

int main(int argc, char **argv)
{
	int key;
	int controller = 0;
	char msg_buffer[BUFFER_SIZE];

	thread_state_t controller_thread_state;
	
	intersection_state_t intersection_states[INTERSECTION_COUNT];
	intersection_light_t override_state;
	message_t msg[INTERSECTION_COUNT];
	
	printf("Central Controller, %d intersection support\n---\n\n", INTERSECTION_COUNT);
	
	/* Clear override target state */
	intersection_light_safe(&override_state);
	
	thread_state_init(&controller_thread_state, intersection_states, msg);
	
	/* Create new state thread */
	if (pthread_create(&(controller_thread_state.thread), NULL, controller_state_thread, (void *) &controller_thread_state) != 0)
	{
		fprintf(stderr, "Failed to start thread!\n");
		return 1;
	}
	
	/* Enter loop waiting for input */
	while (1)
	{
		/* Blocks until a keypress is detected */
		key = key_read();
		
		/* Exit when ESC or ctrl-D is hit */
		if (key == 0x1B || key == 0x04)
		{
			printf("Exiting\n");
			controller_thread_state.run = false;
			break;
		}
		
		/* Aquire lock */
		pthread_mutex_lock(&(controller_thread_state.lock));
		
		/* *** Do something with the captured key *** */
		switch (key)
		{
		/* Car override */
		case (int) 'w':
		case (int) 'W':
		case (int) 'x':
		case (int) 'X':
			/* North traffic light */
			intersection_light_next_colour(&(override_state.l_traffic_ns));
			printf("North-South traffic light toggled to %c\n", override_state.l_traffic_ns);
			break;
		
		case (int) 'a':
		case (int) 'A':		
		case (int) 'd':
		case (int) 'D':
			/* East traffic light */
			intersection_light_next_colour(&(override_state.l_traffic_ew));
			printf("East-West traffic light toggled to %c\n", override_state.l_traffic_ew);
			break;
			
		/* Pedestrian lights */
		case (int) 'q':
		case (int) 'Q':
		case (int) 'c':
		case (int) 'C':
			/* North-South pedestrian light */
			intersection_light_next_colour(&(override_state.l_ped_ns));
			printf("North-South pedestrian light toggled to %c\n", override_state.l_ped_ns);
			break;
			
		case (int) 'e':
		case (int) 'E':
		case (int) 'z':
		case (int) 'Z':
			/* East-West pedestrian light */
			intersection_light_next_colour(&(override_state.l_ped_ew));
			printf("East-West pedestrian light toggled to %c\n", override_state.l_ped_ew);
			break;
			
		/* Tram light */
		case (int) 's':
		case (int) 'S':
			/* Tram light */
			intersection_light_next_colour(&(override_state.l_tram));
			printf("Tram light toggled to %c (if present)\n", override_state.l_tram);
			break;
		
		case 32:
			printf("Override state:\n");
			intersection_light_print(&override_state, &(intersection_states[controller]));
			printf("\n");
			break;
		
		case (int) 'm':
		case (int) 'M':
			/* Tell selected intersection to change mode */
			if (intersection_states[controller].mode == MODE_TIMER)
			{
				sprintf(msg_buffer, "m %c\n", MODE_SENSOR_CHAR);
				printf("Switched to sensor mode on controller %d\n\n", controller);
			}
			else
			{
				sprintf(msg_buffer, "m %c\n", MODE_TIMER_CHAR);
				printf("Switched to timer mode on controller %d\n\n", controller);
			}			
			
			mq_send(msg[controller].mode, msg_buffer, BUFFER_SIZE, 0);
			break;
		
		case (int) 'o':
		case (int) 'O':
			/* TODO Tell selected intersection to change override */
			intersection_states[controller].override_en = !intersection_states[controller].override_en;
			memcpy(&(intersection_states[controller].override), &override_state, sizeof(intersection_state_t));
			
			sprintf(msg_buffer, "o %c ", intersection_states[controller].override_en ? '1' : '0');
			intersection_serial(&(override_state), NULL, &(msg_buffer[4]));
			mq_send(msg[controller].mode, msg_buffer, BUFFER_SIZE, 0);
			
			if (intersection_states[controller].override_en)
			{
				printf("Override state on controller %d set\n\n", controller);
			}
			else
			{
				printf("Override state on controller %d cleared\n\n", controller);
			}
			
			break;
		
		case (int) '-':
		case (int) '_':
			controller--;
			
			if (controller < 0)
				controller = 0;
				
			printf("Selected controller %d\n\n", controller);
			break;
		
		case (int) '=':
		case (int) '+':
			controller++;
			
			if (controller >= INTERSECTION_COUNT)
				controller = (INTERSECTION_COUNT - 1);
			
			printf("Selected controller %d\n\n", controller);
			break;
		}
		
		/* Check if thread is still running */
		if (!controller_thread_state.run)
		{
			break;
		}
		
		/* Release lock */
		pthread_mutex_unlock(&(controller_thread_state.lock));
	}
	
	/* Release held lock */
	pthread_mutex_unlock(&(controller_thread_state.lock));
	
	thread_state_clean(&controller_thread_state);

	return 0;
}

bool thread_state_init(thread_state_t *state, intersection_state_t *intersection_states, message_t *msg)
{
	int i;
	
	state->run = true;
	
	state->intersection_states = intersection_states;
	state->msg = msg;
	
	if (pthread_mutex_init(&(state->lock), NULL) != 0)
	{
		fprintf(stderr, "Failed to create mutex!\n");
		return false;
	}
	
	/* Make a bi-directional message queue for each intersection */
	for (i = 0; i < INTERSECTION_COUNT; i++)
	{
		intersection_state_init(&(state->intersection_states[i]), false);
	
		if (!message_init(&(state->msg[i]), DIR_OUT, i))
		{
			return false;
		}
	}

	return false;
}

void thread_state_clean(thread_state_t *state)
{
	int i;

	pthread_mutex_destroy(&(state->lock));
	
	/* Close all message queues */
	for (i = 0; i < INTERSECTION_COUNT; i++)
	{
		message_clean(&(state->msg[i]), true);
	}
}

void *controller_state_thread(void *arg)
{
	int i;
	char msg_buffer[BUFFER_SIZE];

	thread_state_t *state = (thread_state_t *) arg;
	intersection_state_t *intersection_states = state->intersection_states;
	message_t *msg = state->msg;
	
	intersection_light_t tmp_light;
	intersection_state_t tmp_state;
	
	while (1)
	{
		/* Aquire lock on state */
		pthread_mutex_lock(&(state->lock));
		
		/* Check of thread should exit */
		if (!state->run)
		{
			pthread_mutex_unlock(&(state->lock));
			break;
		}
		
		/* Read all status message queues */
		for (i = 0; i < INTERSECTION_COUNT; i++)
		{
			if (mq_receive(msg[i].status, msg_buffer, BUFFER_SIZE, NULL) > 0)
			{
				intersection_deserial(&tmp_light, &tmp_state, &(msg_buffer[2]));
			
				printf("---\nStatus: %d\n\n", i);
				intersection_state_print(&tmp_state);
				printf("\n");
				intersection_light_print(&tmp_light, &tmp_state);
				printf("---\n\n");
				
				/* Update cache */
				memcpy(&(intersection_states[i]), &tmp_state, sizeof(intersection_state_t));
			}
		}
		
		/* Free lock on state */
		pthread_mutex_unlock(&(state->lock));
		
		usleep(INTERVAL);
	}
	
	/* Free lock on state */
	pthread_mutex_unlock(&(state->lock));
	
	return 0;
}
