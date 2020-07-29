#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <spawn.h>
#include <process.h>
#include <openssl/md5.h>

#define ATTACH_POINT "Server_AttachPoint"

void Client_function(void);

#define MSG_USUAL _IO_MAX + 5

struct usual_msg
{
	short type;
	char sendMsg[300];
};

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
	char receivemsg[200];
	char* sendmsg = "This is a send message";
	int server_coid;
	struct usual_msg msg;

	if ((server_coid = name_open(ATTACH_POINT, 0)) == -1) {
		printf("name_open failure");
		exit(EXIT_FAILURE);
	}
	msg.type = MSG_USUAL;
	strcpy(msg.sendMsg, sendmsg);
	while(1)
	{//告诉服务器，这样的事件给你，并且用这种格式的消息回复我
		if(MsgSend(server_coid, &msg, sizeof(msg), receivemsg, sizeof(receivemsg)) == -1)
		{
			fprintf(stderr,"MsgSend failure");
			exit(EXIT_FAILURE);
		}

		printf("The Response is:\%s\n", receivemsg);

		sleep(2);
	}

}
