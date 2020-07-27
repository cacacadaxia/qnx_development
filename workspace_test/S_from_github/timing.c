#include "timing.h"

void intersection_light_init(intersection_light_t *light_states)
{
	/* Safe, all red */
	light_states[0].l_traffic_ns = LIGHT_RED;
	light_states[0].l_traffic_ew = LIGHT_RED;
	light_states[0].l_ped_ns = LIGHT_RED;
	light_states[0].l_ped_ew = LIGHT_RED;
	light_states[0].l_tram = LIGHT_RED;
	light_states[0].timeout = DELAY_STOP;
	
	/* EW green, EW pedestrian green */
	light_states[1].l_traffic_ns = LIGHT_RED;
	light_states[1].l_traffic_ew = LIGHT_GRN;
	light_states[1].l_ped_ns = LIGHT_GRN;
	light_states[1].l_ped_ew = LIGHT_RED;
	light_states[1].l_tram = LIGHT_RED;
	light_states[1].timeout = DELAY_GO;
	
	/* EW green, EW pedestrian flashing */
	light_states[2].l_traffic_ns = LIGHT_RED;
	light_states[2].l_traffic_ew = LIGHT_GRN;
	light_states[2].l_ped_ns = LIGHT_YLW;
	light_states[2].l_ped_ew = LIGHT_RED;
	light_states[2].l_tram = LIGHT_RED;
	light_states[2].timeout = DELAY_FLASHING;
	
	/* EW yellow, pedestrian stop */
	light_states[3].l_traffic_ns = LIGHT_RED;
	light_states[3].l_traffic_ew = LIGHT_YLW;
	light_states[3].l_ped_ns = LIGHT_RED;
	light_states[3].l_ped_ew = LIGHT_RED;
	light_states[3].l_tram = LIGHT_RED;
	light_states[3].timeout = DELAY_YELLOW;
	
	/* Safe, check tram, all red */
	light_states[4].l_traffic_ns = LIGHT_RED;
	light_states[4].l_traffic_ew = LIGHT_RED;
	light_states[4].l_ped_ns = LIGHT_RED;
	light_states[4].l_ped_ew = LIGHT_RED;
	light_states[4].l_tram = LIGHT_RED;
	light_states[4].timeout = DELAY_STOP;
	
	/* Tram */
	light_states[5].l_traffic_ns = LIGHT_RED;
	light_states[5].l_traffic_ew = LIGHT_RED;
	light_states[5].l_ped_ns = LIGHT_RED;
	light_states[5].l_ped_ew = LIGHT_RED;
	light_states[5].l_tram = LIGHT_GRN;
	light_states[5].timeout = DELAY_TRAM;
	
	/* NS green, NS pedestrian green */
	light_states[6].l_traffic_ns = LIGHT_GRN;
	light_states[6].l_traffic_ew = LIGHT_RED;
	light_states[6].l_ped_ns = LIGHT_RED;
	light_states[6].l_ped_ew = LIGHT_GRN;
	light_states[6].l_tram = LIGHT_RED;
	light_states[6].timeout = DELAY_GO;
	
	/* NS green, NS pedestrian flashing */
	light_states[7].l_traffic_ns = LIGHT_GRN;
	light_states[7].l_traffic_ew = LIGHT_RED;
	light_states[7].l_ped_ns = LIGHT_RED;
	light_states[7].l_ped_ew = LIGHT_YLW;
	light_states[7].l_tram = LIGHT_RED;
	light_states[7].timeout = DELAY_FLASHING;
	
	/* NS yellow, pedestrian stop */
	light_states[8].l_traffic_ns = LIGHT_YLW;
	light_states[8].l_traffic_ew = LIGHT_RED;
	light_states[8].l_ped_ns = LIGHT_RED;
	light_states[8].l_ped_ew = LIGHT_RED;
	light_states[8].l_tram = LIGHT_RED;
	light_states[8].timeout = DELAY_YELLOW;
}

bool intersection_light_next(intersection_light_t *light_states, intersection_state_t *intersection, int *light_current, light_mode_t *mode_current)
{
	switch (*light_current)
	{
	case 0:
		if (*mode_current != intersection->mode)
		{
			*mode_current = intersection->mode;
			intersection_state_print(intersection);
		}
	
		*light_current = 1;
		return true;
		
	case 1:
		if (*mode_current == MODE_TIMER || intersection->mode == MODE_TIMER || intersection->s_car_n || intersection->s_car_s || intersection->s_ped_e || intersection->s_ped_w)
		{
			/* Clear sensors */
			intersection->s_car_n = false;
			intersection->s_car_s = false;
			intersection->s_ped_e = false;
			intersection->s_ped_w = false;
			
			*light_current = 2;
			return true;
		}
		break;
		
	case 2:
		*light_current = 3;
		return true;
		
	case 3:
		*light_current = 4;
		return true;
		
	case 4:
		if (*mode_current != intersection->mode)
		{
			*mode_current = intersection->mode;
			intersection_state_print(intersection);
		}
	
		if (intersection->tram_en)
		{
			if (intersection->s_tram)
			{
				/* Clear sensor */
				intersection->s_tram = false;
				
				*light_current = 5;
			}
			else
			{
				*light_current = 6;
			}
		}
		else
		{
			*light_current = 6;
		}
		
		return true;
		break;
		
	case 5:
		*light_current = 6;
		return true;
		
	case 6:
		if (*mode_current == MODE_TIMER || intersection->mode == MODE_TIMER || intersection->s_car_e || intersection->s_car_w || intersection->s_ped_n || intersection->s_ped_s)
		{
			/* Clear sensors */
			intersection->s_car_e = false;
			intersection->s_car_w = false;
			intersection->s_ped_n = false;
			intersection->s_ped_s = false;
		
			*light_current = 7;
			return true;
		}
		break;
		
	case 7:
		*light_current = 8;
		return true;
		
	case 8:
		*light_current = 0;
		return true;
		
	default:
	
		break;
	}
	
	return false;
}

void intersection_light_safe(intersection_light_t *light_state)
{
	light_state->l_traffic_ns = LIGHT_YLW;
	light_state->l_traffic_ew = LIGHT_YLW;
	light_state->l_ped_ns = LIGHT_YLW;
	light_state->l_ped_ew = LIGHT_YLW;
	light_state->l_tram = LIGHT_YLW;
}

void intersection_light_next_colour(light_t *light)
{
	switch (*light)
	{
	case LIGHT_RED:
		*light = LIGHT_GRN;
		break;
	
	case LIGHT_YLW:
		*light = LIGHT_RED;
		break;
		
	case LIGHT_GRN:
		*light = LIGHT_YLW;
		break;
	}
}

void intersection_light_print(intersection_light_t *light_state, intersection_state_t *intersection)
{
	printf("Lights:\n");
	printf("\tNS: %c\tEW: %c\n", light_state->l_traffic_ns, light_state->l_traffic_ew);
	
	printf("Pedestrian:\n");
	printf("\tNS: %c\tEW: %c\n", light_state->l_ped_ns, light_state->l_ped_ew);
	
	if (intersection->tram_en)
	{
		printf("Tram:\n");
		printf("\tNS: %c\n", light_state->l_tram);
	}
}

void intersection_serial(intersection_light_t *light_state, intersection_state_t *intersection, char *buffer)
{
	int i = 0;
	
	buffer[i++] = light_state->l_traffic_ns;
	buffer[i++] = light_state->l_traffic_ew;
	buffer[i++] = light_state->l_ped_ns;
	buffer[i++] = light_state->l_ped_ew;
	buffer[i++] = light_state->l_tram;
	
	if (intersection != NULL)
	{
		buffer[i++] = intersection->s_car_n ? '1' : '0';
		buffer[i++] = intersection->s_car_e ? '1' : '0';
		buffer[i++] = intersection->s_car_s ? '1' : '0';
		buffer[i++] = intersection->s_car_w ? '1' : '0';
		buffer[i++] = intersection->s_ped_n ? '1' : '0';
		buffer[i++] = intersection->s_ped_e ? '1' : '0';
		buffer[i++] = intersection->s_ped_s ? '1' : '0';
		buffer[i++] = intersection->s_ped_w ? '1' : '0';
		buffer[i++] = intersection->s_tram  ? '1' : '0';
		buffer[i++] = intersection->tram_en ? '1' : '0';
		
		buffer[i++] = intersection->mode == MODE_SENSOR ? 'S' : 'T';
	}
	
	buffer[i++] = '\n';
	buffer[i++] = '\0';
}

void intersection_deserial(intersection_light_t *light_state, intersection_state_t *intersection, char *buffer)
{
	int i = 0;
	
	light_state->l_traffic_ns = buffer[i++];
	light_state->l_traffic_ew = buffer[i++];
	light_state->l_ped_ns = buffer[i++];
	light_state->l_ped_ew = buffer[i++];
	light_state->l_tram = buffer[i++];
	
	if (intersection != NULL)
	{
		intersection->s_car_n = buffer[i++] == '1' ? true : false;
		intersection->s_car_e = buffer[i++] == '1' ? true : false;
		intersection->s_car_s = buffer[i++] == '1' ? true : false;
		intersection->s_car_w = buffer[i++] == '1' ? true : false;
		intersection->s_ped_n = buffer[i++] == '1' ? true : false;
		intersection->s_ped_e = buffer[i++] == '1' ? true : false;
		intersection->s_ped_s = buffer[i++] == '1' ? true : false;
		intersection->s_ped_w = buffer[i++] == '1' ? true : false;
		intersection->s_tram  = buffer[i++] == '1' ? true : false;
		intersection->tram_en = buffer[i++] == '1' ? true : false;
		
		intersection->mode = buffer[i++] == 'S' ? MODE_SENSOR : MODE_TIMER;
	}
}

void intersection_state_init(intersection_state_t *intersection, bool tram_en)
{
	/* Clear sensors */
	intersection->s_car_n = false;
	intersection->s_car_e = false;
	intersection->s_car_s = false;
	intersection->s_car_w = false;
	intersection->s_ped_n = false;
	intersection->s_ped_e = false;
	intersection->s_ped_s = false;
	intersection->s_ped_w = false;
	intersection->s_tram  = false;
	
	intersection->tram_en = tram_en;
	
	/* Default override is all yellow */
	intersection->override_en = false;
	
	intersection->override.l_traffic_ns = LIGHT_YLW;
	intersection->override.l_traffic_ew = LIGHT_YLW;
	intersection->override.l_ped_ns = LIGHT_YLW;
	intersection->override.l_ped_ew = LIGHT_YLW;
	intersection->override.l_tram = LIGHT_YLW;
	
	/* Start in timed mode */
	intersection->mode = MODE_TIMER;
}

void intersection_state_print(intersection_state_t *intersection)
{
	switch (intersection->mode)
	{
		case MODE_TIMER:
			printf("Mode: Timed\n");
			break;
		
		case MODE_SENSOR:
			printf("Mode: Sensor\n");
			break;
	}
}
