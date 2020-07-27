

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/siginfo.h>
#include <stdio.h>
#include <sys/siginfo.h>
// pulses
#define CODE_TIMER          1       // pulse from timer
int main(){
    timer_t  timerid;
	struct sigevent ev;
	ev.sigev_notify = SIGEV_PULSE;


	SIGEV_PULSE_INIT(&ev,NULL, SIGEV_PULSE_PRIO_INHERIT, CODE_TIMER, 0);
	timer_create(CLOCK_REALTIME,&ev,&timerid);

	while(1){
		printf("%f\n",timerid);
		sleep(1);
	}

}
