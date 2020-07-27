//simple client
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>

int main(int argc, char **argv)
{
	pid_t spid;
	int chid, coid, i;
	char buf[128];

//	if (argc < 3) {
//		fprintf(stderr, "Usage: simple_client <pid> <chid>\n");
//		return -1;
//	}

	spid = 1314847;
	chid = 1;

	if ((coid = ConnectAttach(0, spid, chid, 0, 0)) == -1) {
		perror("ConnectAttach");
		return -1;
	}

	/* sent 3 pairs of "Hello" and "Ni Hao" */
	for (i = 0; i < 3; i++) {
		sprintf(buf, "Hello");
		printf("client: sent '%s'\n", buf);
		if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != 0) {
			perror("MsgSend");
			return -1;
		}
		printf("client: returned '%s'\n", buf);

		sprintf(buf, "Ni Hao");
		printf("client: sent '%s'\n", buf);
		if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != 0) {
			perror("MsgSend");
			return -1;
		}
		printf("client: returned '%s'\n", buf);
	}

	/* sent a bad message, see if we get an error */
	sprintf(buf, "Unknown");
	printf("client: sent '%s'\n", buf);
	if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != 0) {
		perror("MsgSend");
		return -1;
	}

	ConnectDetach(coid);//把这个连接分离掉

	return 0;
}
