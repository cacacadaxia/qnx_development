#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/siginfo.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <process.h>

#define ATTACH_POINT "Server_AttachPoint"



//Z__Client_Nameopen_SendPulse 调用
//重新开一个client进程
void* Server_function(void* notuse);
name_attach_t* attach;

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL + 5
#define MSG_GIVE_PULSE _IO_MAX + 4
#define MSG_USUAL _IO_MAX + 5

struct my_msg
{
   short type;
   struct sigevent event;
};

struct usual_msg
{
	short type;
	char recMsg[300];
};

union
{
      struct my_msg   mine;
      struct usual_msg usual;
}msg;


int main(int argc, char *argv[]) {
	printf("Server process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int result, Thread_ID;

	if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL)
	{
		printf("name_attach failure");
		return EXIT_FAILURE;
	}

	printf("The chid = %d\n", attach->chid);
	//开启一个进程

	result = spawnl(P_NOWAIT, "/tmp/Z__Client_Nameopen_SendPulse", "Z__Client_NameOpen", NULL);
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
	pthread_create(&Thread_ID, &attr, Server_function, NULL);

	pthread_join(Thread_ID, NULL);

	return EXIT_SUCCESS;
}

void* Server_function(void* notuse)
{
	int rcvid;

	 while(1)
	 {
		 rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		 if(rcvid == 0)
		 {
			 printf("get a pulse, the value of pulse is %d\n", ((struct _pulse*)&msg)->value.sival_int);
		 }
		 else if(rcvid > 0)
		 {
			 switch(msg.mine.type)
			 {
				 case MSG_GIVE_PULSE:	 (&msg.mine.event)->sigev_value.sival_ptr = (void *)((&msg.mine.event)->sigev_value.sival_int + 1);
										 MsgDeliverEvent(rcvid, &msg.mine.event);
										 MsgReply(rcvid, 100, NULL, 0);
										 break;
				 case MSG_USUAL:		 strcpy(msg.usual.recMsg, "this is a usual reply");
										 MsgReply(rcvid, 100, msg.usual.recMsg, strlen(msg.usual.recMsg) + 1);
										 break;
				 default: 				 printf("the rcvid is %d\n", rcvid);
										 break;
			 }
		 }
		 else
		 {
			 printf("no message\n");
		 }



		 sleep(3);
	 }

}
