#ifndef TIMING_H
#define TIMING_H

#include "util.h"

#define DELAY_GO 10
#define DELAY_TRAM 10
#define DELAY_YELLOW 3
#define DELAY_FLASHING 5
#define DELAY_STOP 3

#define LIGHT_RED 'R'
#define LIGHT_YLW 'Y'
#define LIGHT_GRN 'G'

#define MODE_COUNT 2
#define MODE_INIT -1
#define MODE_TIMER 0
#define MODE_TIMER_CHAR 'T'
#define MODE_SENSOR 1
#define MODE_SENSOR_CHAR 'S'

typedef char light_t;
typedef char light_mode_t;
typedef bool sensor_t;
typedef int timeout_t;

typedef struct {
	light_t l_traffic_ns, l_traffic_ew;
	light_t l_ped_ns, l_ped_ew;
	light_t l_tram;
	
	timeout_t timeout;
} intersection_light_t;

typedef struct {
	sensor_t s_car_n, s_car_e, s_car_s, s_car_w;
	sensor_t s_ped_n, s_ped_e, s_ped_s, s_ped_w;
	sensor_t s_tram;
	
	bool override_en;
	bool tram_en;
	
	light_mode_t mode;
	
	intersection_light_t override;
} intersection_state_t;

void intersection_light_init(intersection_light_t *light_states);
bool intersection_light_next(intersection_light_t *light_states, intersection_state_t *intersection, int *light_current, light_mode_t *mode_current);
void intersection_light_print(intersection_light_t *light_state, intersection_state_t *intersection);
void intersection_light_safe(intersection_light_t *light_state);

void intersection_light_next_colour(light_t *light);

void intersection_serial(intersection_light_t *light_state, intersection_state_t *intersection, char *buffer);
void intersection_deserial(intersection_light_t *light_state, intersection_state_t *intersection, char *buffer);

void intersection_state_init(intersection_state_t *intersection, bool tram_en);
void intersection_state_print(intersection_state_t *intersection);

#endif
