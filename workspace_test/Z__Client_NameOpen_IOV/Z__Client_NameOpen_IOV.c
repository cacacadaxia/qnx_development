#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <spawn.h>
#include <process.h>

#define SETIOV(_iov, _addr, _len) ((_iov)->iov_base = (void*)(_addr), (_iov)->iov_len = (_len))

#define ATTACH_POINT "Server_AttachPoint_IOV"

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
	io_write_t whdr;
	iov_t* Send_iov;
	iov_t* Rec_iov;
	char *sendmsg = "This is a send message";
	char receivemsg[200];
	int server_coid;

	SETIOV(Send_iov, &whdr, sizeof(whdr));
	whdr.i.type = _IO_WRITE;
	whdr.i.nbytes = 100;

	if ((server_coid = name_open(ATTACH_POINT, 0)) == -1) {
		printf("name_open failure");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		if(MsgSendv(server_coid, Send_iov, 1, Rec_iov, 1) == -1)
		{
			fprintf(stderr,"MsgSend failure");
			exit(EXIT_FAILURE);
		}

		//printf("The Response is:\%s\n", receivemsg);

		sleep(3);
	}

}
