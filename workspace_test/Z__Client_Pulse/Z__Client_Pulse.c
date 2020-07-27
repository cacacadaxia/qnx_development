#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <spawn.h>
#include <process.h>

#define ATTACH_POINT "Server_AttachPoint"

void Client_function(void);

int main(int argc, char *argv[]) {
	printf("Client process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 20;
	pthread_create(NULL, &attr, &Client_function, NULL);


	while(1)
	{

	}

	return EXIT_SUCCESS;
}

void Client_function(void)
{
	char *sendmsg = "This is a send message";
	char receivemsg[200];
	int server_coid;

	if ((server_coid = name_open(ATTACH_POINT, 0)) == -1) {
		printf("name_open failure");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		if(MsgSend(server_coid, sendmsg, strlen(sendmsg)+1, receivemsg, sizeof(receivemsg)) == -1)
		{
			fprintf(stderr,"MsgSend failure");
			exit(EXIT_FAILURE);
		}

		printf("The Response is:\%s\n", receivemsg);

		sleep(3);
	}

}

