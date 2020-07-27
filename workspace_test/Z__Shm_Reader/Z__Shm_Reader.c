#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>
#include <process.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>

#define Shm_Attach_POINT "Server_AttachPoint"

void* read_function(void* notuse);

int main(int argc, char *argv[]) {
	printf("Shm Read process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int result, Thread_ID;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 20;
	pthread_create(&Thread_ID, &attr, read_function, NULL);

	pthread_join(Thread_ID, NULL);

	return EXIT_SUCCESS;
}

void* read_function(void* notuse)
{
	int fd;
	unsigned * addr;

	fd = shm_open(Shm_Attach_POINT, O_RDONLY, 0777);
	if(fd == -1)
	{
		printf("shm open failure\n");
		return EXIT_FAILURE;
	}

	addr = mmap(0, sizeof(*addr), PROT_READ, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED)
	{
		printf("mmap failure\n");
		return EXIT_FAILURE;
	}

	while(1)
	{
		printf("read process the data is %d\n", *addr);
		sleep(3);
	}

}
