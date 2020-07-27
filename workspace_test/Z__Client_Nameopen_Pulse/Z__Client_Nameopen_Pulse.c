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

void* Client_function(void* notuse);

struct my_msg
{
   short type;
   struct sigevent event;
};

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL + 5
#define MSG_GIVE_PULSE _IO_MAX + 4


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

void* Client_function(void* notuse)
{
	int server_coid, chid, coid, rcvid;
	struct my_msg msg;
	struct _pulse pulse;

	/* we need a channel to receive the pulse notification on */
	chid = ChannelCreate(0);

	/* and we need a connection to that channel for the pulse to be
	 	 delivered on */
	coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0 );

	SIGEV_PULSE_INIT(&msg.event, coid, SIGEV_PULSE_PRIO_INHERIT, MY_PULSE_CODE, 100);
	msg.type = MSG_GIVE_PULSE;

	if ((server_coid = name_open(ATTACH_POINT, 0)) == -1)
	{
		printf("name_open failure");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		printf("pulse client deliver a msg\n");
		if(MsgSend(server_coid, &msg, sizeof(msg), NULL, 0) == -1)
		{
			printf("MessageSend failure");
			exit(EXIT_FAILURE);
		}

		rcvid = MsgReceivePulse(chid, &pulse, sizeof(pulse), NULL);
		(&msg.event)->sigev_value.sival_ptr = (void *)(pulse.value.sival_int);

		printf("get a pulse, the value is %d\n", pulse.value.sival_int);
	}

}
