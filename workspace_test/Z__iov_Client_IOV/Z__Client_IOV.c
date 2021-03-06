#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <spawn.h>
#include <process.h>

/***********************************
 * 注意：
 *       1.  这两块函数始终对不上，client准备的消息在server端接收不到
 *       2.
 *       3.
**************************/
#define SETIOV(_iov, _addr, _len) ((_iov)->iov_base = (void*)(_addr), (_iov)->iov_len = (_len))

#define ATTACH_POINT "Server_AttachPoint_IOV"//连接的点，实际上就是名称（通道的？）

void* Client_function(void *c);

typedef struct{
	int type;
}data;

int main(int argc, char *argv[]) {
	printf("Client process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int Thread_ID;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 20;
	pthread_create(&Thread_ID, &attr, Client_function, NULL);

	pthread_join(Thread_ID, NULL);
	return EXIT_SUCCESS;

}

void* Client_function(void *c)
{
	char* sendmsg = "This is the send message\n";
	char RecMsg[300];
	iov_t Send_iov;
	iov_t Rec_iov;
	int server_coid;

	// iov
	char a[100],b[100];
	strcpy(a,"fuck you");
	strcpy(b,"end");



	// head
    io_write_t whdr;
    whdr.i.type = _IO_WRITE;
    whdr.i.nbytes =100;
    SETIOV(&Send_iov+0, &whdr, sizeof(whdr) );

    //	SETIOV(&Send_iov+0, sendmsg, sizeof(sendmsg) + 1);
    SETIOV(&Send_iov+1, a , 100);
    SETIOV(&Send_iov+2, b , 100);



	if ((server_coid = name_open(ATTACH_POINT, 0)) == -1)
	{
		printf("name_open failure");
		exit(EXIT_FAILURE);
	}
	while(1)
	{
		if(MsgSendvs(server_coid, &Send_iov, 1, RecMsg, sizeof(RecMsg)) == -1)
		{
			fprintf(stderr,"MsgSend failure\n");
			printf("end\n");
			exit(EXIT_FAILURE);
		}

		printf("The Response is:%s\n", RecMsg);

		sleep(3);
	}

}

