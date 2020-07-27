#include "intersection-ui.h"
#include "util.h"

int main(int argc, char **argv)
{
	int i, key;
	
	thread_state_t light_thread_state;
	
	intersection_state_t intersection;
	intersection_light_t light_states[9];
	
	/* Default parameters */
	bool tram = false;
	int id = 0;
	
	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tram") == 0)
		{
			tram = true;
		}
		else
		{
			id = atoi(argv[i]);
		}
	}
	
	printf("Intersection Controller %d, %s tram line\n---\n\n", id, tram ? "with" : "without");
	
	/* Initialise light states */
	intersection_light_init(light_states);
	
	/* Initialise thread state */
	intersection_state_init(&intersection, tram);
	
	/* Get command line parameters */
	
	
	if (!thread_state_init(&light_thread_state, light_states, &intersection, id))
	{
		fprintf(stderr, "Failed to initialize thread state!\n");
		return 1;
	}

	/* Create new state thread */
	if (pthread_create(&(light_thread_state.thread), NULL, light_state_thread, (void *) &light_thread_state) != 0)
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
			light_thread_state.run = false;
			break;
		}
		
		/* Aquire lock */
		pthread_mutex_lock(&(light_thread_state.lock));
		
		/* *** Do something with the captured key *** */
		switch (key)
		{
		/* Car sensors */
		case (int) 'w':
		case (int) 'W':
			/* North car sensor */
			if (!light_thread_state.intersection->s_car_n)
			{
				printf("North car sensor triggered\n");
				light_thread_state.intersection->s_car_n = true;
			}
			break;
			
		case (int) 'd':
		case (int) 'D':
			/* East car sensor */
			if (!light_thread_state.intersection->s_car_e)
			{
				printf("East car sensor triggered\n");
				light_thread_state.intersection->s_car_e = true;
			}
			break;
			
		case (int) 'x':
		case (int) 'X':
			/* South car sensor */
			if (!light_thread_state.intersection->s_car_s)
			{
				printf("South car sensor triggered\n");
				light_thread_state.intersection->s_car_s = true;
			}
			break;
			
		case (int) 'a':
		case (int) 'A':
			/* South car sensor */
			if (!light_thread_state.intersection->s_car_w)
			{
				printf("West car sensor triggered\n");
				light_thread_state.intersection->s_car_w = true;
			}
			break;
			
		/* Pedestrian sensors */
		case (int) 'q':
		case (int) 'Q':
			/* North pedestrian sensor */
			if (!light_thread_state.intersection->s_ped_n)
			{
				printf("North->East pedestrian sensor triggered\n");
				light_thread_state.intersection->s_ped_n = true;
			}
			break;
			
		case (int) 'e':
		case (int) 'E':
			/* East pedestrian sensor */
			if (!light_thread_state.intersection->s_ped_e)
			{
				printf("East->South pedestrian sensor triggered\n");
				light_thread_state.intersection->s_ped_e = true;
			}
			break;
			
		case (int) 'c':
		case (int) 'C':
			/* South pedestrian sensor */
			if (!light_thread_state.intersection->s_ped_s)
			{
				printf("South->West pedestrian sensor triggered\n");
				light_thread_state.intersection->s_ped_s = true;
			}
			break;
			
		case (int) 'z':
		case (int) 'Z':
			/* South pedestrian sensor */
			if (!light_thread_state.intersection->s_ped_w)
			{
				printf("West->North pedestrian sensor triggered\n");
				light_thread_state.intersection->s_ped_w = true;
			}
			break;
			
		/* Tram sensor */
		case (int) 's':
		case (int) 'S':
			/* Tram sensor */
			if (light_thread_state.intersection->tram_en)
			{
				if (!light_thread_state.intersection->s_tram)
				{
					printf("Tram sensor triggered\n");
					light_thread_state.intersection->s_tram = true;
				}
			}
			else
			{
				printf("No tram sensor in this intersection\n");
			}
			break;

		case (int) 'm':
		case (int) 'M':
			/* Mode switch */
			printf("Mode switched (will update on next stop state)\n");
			light_thread_state.intersection->mode = (light_thread_state.intersection->mode + 1) % MODE_COUNT;
			break;
		
		case (int) 'o':
		case (int) 'O':
			/* Override switch */
			light_thread_state.intersection->override_en = !light_thread_state.intersection->override_en;			
			break;
		
		default:
			printf("Unassigned key: %d\n", key);
			break;
		}
		
		/* Check if thread is still running */
		if (!light_thread_state.run)
		{
			break;
		}
		
		/* Release lock */
		pthread_mutex_unlock(&(light_thread_state.lock));
	}
	
	/* Release held lock */
	pthread_mutex_unlock(&(light_thread_state.lock));
	
	/* Wait for state thread to finish */
	pthread_join(light_thread_state.thread, NULL);
	
	/* Cleanup */
	thread_state_clean(&light_thread_state);

	return 0;
}

bool thread_state_init(thread_state_t *state, intersection_light_t *light_states, intersection_state_t *intersection, int id)
{
	state->run = true;
	state->light_states = light_states;
	state->intersection = intersection;
	
	if (pthread_mutex_init(&(state->lock), NULL) != 0)
	{
		fprintf(stderr, "Failed to create mutex!\n");
		return false;
	}
	
	/* Open message queues */
	if (!message_init(&(state->msg), DIR_IN, id))
	{
		return false;
	}
	
	return true;
}

void thread_state_clean(thread_state_t *state)
{
	pthread_mutex_destroy(&(state->lock));
	
	message_clean(&(state->msg), false);
}

void *light_state_thread(void *arg)
{
	char msg_buffer[BUFFER_SIZE];

	thread_state_t *state = (thread_state_t *) arg;
	intersection_state_t *intersection = state->intersection;
	intersection_light_t *light_states = state->light_states;
	
	int light_current = 0;
	light_mode_t mode_current = MODE_INIT;
	
	int timeout = 3;
	
	bool override_current = intersection->override_en;
	
	printf("Mode: Startup\n");
	intersection_light_print(&(light_states[light_current]), intersection);
	printf("\n");

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
		
		/* TODO Check mode switch message queue */		
		if (mq_receive(state->msg.mode, msg_buffer, BUFFER_SIZE, NULL) > 0)
		{
			printf("Received message: %s\n", msg_buffer);
		
			switch (msg_buffer[0])
			{
			case 'm':
				/* Change mode */
				intersection->mode = msg_buffer[2] == MODE_TIMER_CHAR ? MODE_TIMER : MODE_SENSOR;
				break;
				
			case 'o':
				/* Change override */
				intersection->override_en = msg_buffer[2] == '1' ? true : false;
				
				intersection_deserial(&(intersection->override), NULL, &(msg_buffer[4]));
				
				break;
			}
		}
		
		/* *** Do traffic light stuff *** */
		if (override_current != intersection->override_en)
		{
			if (intersection->override_en)
			{
				printf("Override: Enabled\n");
				intersection_light_print(&(intersection->override), intersection);
				printf("\n");
			}
			else
			{
				printf("Override: Disabled\n");
				intersection_light_print(&(light_states[light_current]), intersection);
				printf("\n");
			}
			
			override_current = intersection->override_en;
		}
		
		if (!override_current)
		{
			if (timeout <= 0)
			{
				/* Light state */
				if (intersection_light_next(light_states, intersection, &light_current, &mode_current))
				{
					/* Wait at least the predefined interval */
					intersection_light_print(&(light_states[light_current]), intersection);
					printf("\n");
					
					/* Output status to message queue */
					sprintf(msg_buffer, "s ");
					intersection_serial(&(light_states[light_current]), intersection, &(msg_buffer[2]));
					mq_send(state->msg.status, msg_buffer, BUFFER_SIZE, 0);
					
					timeout = (light_states[light_current].timeout - 1);
				}
			}
			else
			{
				timeout--;
			}
		}
		
		/* Free lock on state */
		pthread_mutex_unlock(&(state->lock));
		
		sleep(INTERVAL);
	}
	
	/* Free lock on state */
	pthread_mutex_unlock(&(state->lock));
	
	return NULL;
}
