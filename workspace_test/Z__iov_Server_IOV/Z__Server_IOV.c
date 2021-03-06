#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <process.h>

#define ATTACH_POINT "Server_AttachPoint_IOV"

void* Server_function(void* c);
name_attach_t* attach;

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
	 result = spawnl(P_NOWAIT, "/tmp/Z__iov_Client_IOV", "Z__iov_Client_IOV", NULL);

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

void* Server_function(void* c)
{
	int rcvid;
	char ServerRecmsg[300];
	char* Reply_data = "sdasdasd";
	iov_t iov, Reply_iov;
	Reply_iov.iov_base = "1";
	Reply_iov.iov_len = 2;
	 while(1)
	 {
		 rcvid = MsgReceivev(attach->chid, &iov, 1, NULL);//为什么改成3就不行了？

		 printf("rcvid is: %d\n", rcvid);

//		 MsgReadv(rcvid, &iov, 1, 0);
		 printf("Server get a message: %d\n", iov.iov_len);

		 strcpy(ServerRecmsg, "This is the reply");

		 MsgReply(rcvid, 100, ServerRecmsg, sizeof(ServerRecmsg));//100应该是预留的缓存大小
	 }

}
