#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sync.h>
#include <sched.h>
#include <fcntl.h>

void* barrierTest1(void* notuse);
void* barrierTest2(void* notuse);
void* barrierTest3(void* notuse);

pthread_barrier_t barrier;
pthread_barrier_t barrier1;
pthread_barrierattr_t barrier_attr;


int count = 0;
int main(int argc, char *argv[]) {
	printf("barrier process is ready, pid = %d\n", getpid());

	pthread_attr_t attr1, attr2, attr3;
	int Thread1_ID, Thread2_ID, Thread3_ID;
	int result;

	result = pthread_barrier_init(&barrier, NULL, 2);//为什么是3？有三个需要同步
	result = pthread_barrier_init(&barrier1, NULL, 2);
	printf("barrier init result is %d\n", result);

	pthread_attr_init(&attr1);
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr1, SCHED_FIFO);
	attr1.__param.__sched_priority = 22;
	pthread_create(&Thread1_ID, &attr1, barrierTest1, NULL);

	pthread_attr_init(&attr2);
	pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);
	attr2.__param.__sched_priority = 22;
	pthread_create(&Thread2_ID, &attr2, barrierTest2, NULL);

	pthread_attr_init(&attr3);
	pthread_attr_setdetachstate(&attr3, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr3, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr3, SCHED_FIFO);
	attr3.__param.__sched_priority = 22;
	pthread_create(&Thread3_ID, &attr3, barrierTest3, NULL);


	count++;
	if(count ==4){
		printf("fuck you");
	}
	pthread_join(Thread1_ID, NULL);
	pthread_join(Thread2_ID, NULL);
	pthread_join(Thread3_ID, NULL);




	return EXIT_SUCCESS;
}

void* barrierTest1(void* notuse)
{
	printf("start thread 1\n");
	while(1)
	{
		sleep(2);
		pthread_barrier_wait(&barrier);
		printf("thread 1 wait barrier\n");
//		pthread_barrier_wait(&barrier1);
	}
}

void* barrierTest2(void* notuse)
{
	printf("start thread 2\n");
	while(1)
	{
		sleep(4);
		pthread_barrier_wait(&barrier);
		printf("thread 2 wait barrier\n");
//		pthread_barrier_wait(&barrier1);//为什么这里换了一个？
	}
}

void* barrierTest3(void* notuse)
{
	printf("start thread 3\n");
	while(1)
	{
		sleep(8);
//		pthread_barrier_wait(&barrier);//
		printf("thread 3 wait barrier\n");
//		pthread_barrier_wait(&barrier1);
	}
}
