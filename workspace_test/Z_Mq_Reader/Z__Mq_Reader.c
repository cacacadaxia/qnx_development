#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>
#include <process.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>

#define Mq_Attach_POINT "Mq_AttachPoint_Entry_my"

void* mq_Read_function(void* notuse);

int main(int argc, char *argv[]) {
	printf("Mq read process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int Thread_ID;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 20;
	pthread_create(&Thread_ID, &attr, mq_Read_function, NULL);

	pthread_join(Thread_ID, NULL);

	return EXIT_SUCCESS;
}

void* mq_Read_function(void* notuse)
{
	int fd, prio;
	char Rec_data[4096];
	struct mq_attr attr;

	fd = mq_open(Mq_Attach_POINT, O_RDONLY);
	//EAGAIN

	if(fd == -1)
	{
		printf("mq open failure\n");
		return NULL;
	}
	printf("fd is %d\n", fd);

	while(1)
	{
		if(mq_receive(fd, Rec_data, sizeof(Rec_data), &prio) == -1)
		{
			printf("read  data from mq error, errno is %d\n", errno);
		}
		printf("read process the data is %s\n", Rec_data);
		sleep(1);
	}

}
