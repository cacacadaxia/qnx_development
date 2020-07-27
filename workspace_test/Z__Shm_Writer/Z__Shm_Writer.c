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

void* Write_function(void* notuse);

int main(int argc, char *argv[]) {
	printf("Shm Write process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int result, Thread_ID;

	result = spawnl(P_NOWAIT, "/tmp/Z__Shm_Reader", "Z__Client_NameOpen", NULL);
	if (result == -1)
	{
		perror("Error from spawnl");
		exit(1);
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 21;
	pthread_create(&Thread_ID, &attr, Write_function, NULL);

	pthread_join(Thread_ID, NULL);

	return EXIT_SUCCESS;
}

void* Write_function(void* notuse)
{
	int fd;
	unsigned * addr;

	fd = shm_open(Shm_Attach_POINT, O_RDWR|O_CREAT, 0777);
	if(fd == -1)
	{
		printf("shm open failure\n");
		return EXIT_FAILURE;
	}

	if(ftruncate(fd, sizeof(*addr)) == -1)
	{
		printf("ftruncate failure\n");
		return EXIT_FAILURE;
	}

	addr = mmap(0, sizeof(*addr), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED)
	{
		printf("mmap failure\n");
		return EXIT_FAILURE;
	}

	*addr = 0;
	while(1)
	{
		*addr += 1;
		sleep(3);
		printf("write process the data is %d\n", *addr);
	}

}
