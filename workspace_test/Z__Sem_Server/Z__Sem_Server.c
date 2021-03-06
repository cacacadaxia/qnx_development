#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>



void* Server_function(void* notuse);
void* Client_function(void* notuse);

int main(int argc, char *argv[]) {
	printf("111");
	printf("Sem server process is ready, pid = %d\n", getpid());
	pthread_attr_t attr;
	int result, Thread_ID;

//	result = spawnl(P_NOWAIT, "/tmp/Z__Shm_Reader", "Z__Client_NameOpen", NULL);
//	if (result == -1)
//	{
//		perror("Error from spawnl");
//		exit(1);
//	}

//	pthread_attr_init(&attr);//这里依然是相关的线程的配置
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
//	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
//	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
//	attr.__param.__sched_priority = 21;
////	pthread_create(&Thread_ID, &attr, Server_function, NULL);

	pthread_attr_t attr1;
	int Thread_ID1;

	pthread_attr_init(&attr1);//这里依然是相关的线程的配置
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr1, SCHED_FIFO);
	attr1.__param.__sched_priority = 22;
	pthread_create(&Thread_ID1, &attr1, Client_function, NULL);

	//下面分离出程序
	pthread_join(Thread_ID, NULL);//为什么join只需要值就行了而不需要地址？
	pthread_join(Thread_ID1, NULL);//为什么join只需要值就行了而不需要地址？
	return EXIT_SUCCESS;
}

void* Server_function(void* notuse)//貌似只需要发消息就行了
{
	sem_t *sem;
	sem = sem_open("global_Sem", O_CREAT, S_IRWXU, 1);//打开一段共享内存？
	while(1)
	{
		sem_post(sem);
		sleep(3);
		printf("send a sem\n");
	}
}

void* Client_function(void* notuse)
{
	sem_t *sem;
	sem = sem_open("global_Sem", O_EXCL);
	while(1)
	{
		sem_wait(sem);
		printf("get a sem\n");
		sleep(1);
	}
}
