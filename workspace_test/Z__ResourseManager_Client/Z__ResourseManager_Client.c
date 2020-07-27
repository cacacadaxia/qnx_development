


#include <stdio.h>
#include <sys/dispatch.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void* Client_Readfunction(void* notuse);
void* Client_Writefunction(void* notuse);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
	printf("ResourseManager client process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int Thread_ID_1, Thread_ID_2;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 20;
	pthread_create(&Thread_ID_1, &attr, Client_Readfunction, NULL);
	pthread_create(&Thread_ID_2, &attr, Client_Writefunction, NULL);

	pthread_join(Thread_ID_1, NULL);
	pthread_join(Thread_ID_2, NULL);

	return EXIT_SUCCESS;
}

void* Client_Readfunction(void* notuse){
	int fd = 0, data, num = 0;
	fd = open("dev/sample", O_RDONLY);
	while(1){
		pthread_mutex_lock(&mutex);
		lseek(fd, 100, SEEK_SET);
		num = read(fd, &data, sizeof(data));
		pthread_mutex_unlock(&mutex);

		printf("read nbytes is %d\tread data is %d\n", num, data);
		sleep(4);
	}
}

void* Client_Writefunction(void* notuse){
	int fd = 0, data = 4, num = 0;
	fd = open("dev/sample", O_WRONLY);
	while(1){
		pthread_mutex_lock(&mutex);
		lseek(fd, 100, SEEK_SET);
		num = write(fd, &data, sizeof(data));
		pthread_mutex_unlock(&mutex);

		printf("write nbytes is %d\twrite data is %d\n", num, data);
		sleep(4);
		data += 1;
	}
}
