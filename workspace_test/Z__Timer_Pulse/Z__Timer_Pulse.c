#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <time.h>
#include <sys/siginfo.h>




void* Timer_SendPulse(void* notuse);
int chid, coid;

int main(int argc, char *argv[]) {
	printf("Timer_Pulse process is ready, pid = %d\n", getpid());

	pthread_attr_t 		attr;
	int 				Thread_ID;
	timer_t 			timerid;
	struct sigevent 	event;
	struct itimerspec 	timer;

	if((chid = ChannelCreate(0)) == -1){
		printf("Create channel failure\n");
		exit(EXIT_FAILURE);
	}

	if((coid = ConnectAttach(0, 0, chid, 0, 0)) == -1){
		printf("Connect to self failure\n");
		exit(EXIT_FAILURE);
	}

	//初始化Event, code = 1, value = 100
	SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, 1, 100);

	if(timer_create(CLOCK_REALTIME, &event, &timerid) == -1){
		printf("Create timer failure\n");
		exit(EXIT_FAILURE);
	}

	//设置定时器参数
	timer.it_value.tv_sec = 1;
	timer.it_value.tv_nsec = 0;
	timer.it_interval.tv_sec = 1;
	timer.it_interval.tv_nsec = 0;

	//启动定时器
	timer_settime(timerid, 0, &timer, NULL);

	//线程初始化, Joinable、FIFO调度、优先级, 线程启动
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 20;
	pthread_create(&Thread_ID, &attr, Timer_SendPulse, NULL);

	//等待线程执行完成
	pthread_join(Thread_ID, NULL);

	return EXIT_SUCCESS;
}


//意思应该是从通道中去取，获得rcvid
void* Timer_SendPulse(void* notuse){
	int 			rcvid;
	struct _pulse 	pulse;
	while(1)
	{
		rcvid = MsgReceive(chid, &pulse, sizeof(pulse), NULL);
		if(rcvid == 0)
		{
			printf("get a pulse from timer, the value of pulse is %d\n", pulse.value.sival_int);
		}
	}
}



