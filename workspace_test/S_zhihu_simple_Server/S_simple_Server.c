// Simple server
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>


struct tmp{
	int a;
	char b[100];
};

int main()
{
	int chid, rcvid, status;
	char buf[128];

	if ((chid = ChannelCreate(0)) == -1) {//随便产生一个吧
		perror("ChannelCreate");
		return -1;
	}

	printf("Server is ready, pid = %d, chid = %d\n", getpid(), chid);

	for (;;) {
		if ((rcvid = MsgReceive(chid, buf, sizeof(buf), NULL)) == -1) {
			perror("MsgReceive");
			return -1;
		}
		printf("Server: Received '%s'\n", buf);
		/* Based on what we receive, return some message */
		if (strcmp(buf, "Hello") == 0) {
			MsgReply(rcvid, 0, "World", strlen("World") + 1);
		} else if (strcmp(buf, "Ni Hao") == 0) {
			MsgReply(rcvid, 0, "Zhong Guo", strlen("Zhong Guo") + 1);
		}
		//是不是只能传输str？传输结构体行不行？
//		else if(strcmp(buf, "mine") == 0){
//			struct tmp temp1;
//			temp1.a = 100;
//			MsgReply(rcvid , 0, temp1,)
//		}
		else {
			MsgError(rcvid, EINVAL);
		}
	}

	ChannelDestroy(chid);
	return 0;
}
