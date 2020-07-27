#include "msg.h"

bool message_init(message_t *message, dir_t direction, int id)
{
	bool flag = true;
	char buffer[BUFFER_SIZE];
	
	struct mq_attr attr;
	
	attr.mq_maxmsg = 128;
	attr.mq_msgsize = BUFFER_SIZE;
	attr.mq_flags = 0;
	
	message->file_mode = NULL;
	message->file_status = NULL;
	
	sprintf(buffer, "/root/mode%d", id);
	message->file_mode = malloc((strlen(buffer) + 1) * sizeof(char));
	
	if (message->file_mode == NULL)
	{
		fprintf(stderr, "Failed to allocate memory!\n");
		return false;
	}
	
	strcpy(message->file_mode, buffer);
	
	sprintf(buffer, "/root/status%d", id);
	message->file_status = malloc((strlen(buffer) + 1) * sizeof(char));
	
	if (message->file_status == NULL)
	{
		fprintf(stderr, "Failed to allocate memory!\n");
		free(message->file_mode);
		return false;
	}
	
	strcpy(message->file_status, buffer);

	/*printf("Message: %s\n", message->file_mode);
	printf("Status:  %s\n", message->file_status);*/
	
	if (direction == DIR_IN)
	{
		if ((message->mode = mq_open(message->file_mode, MQ_RD, MQ_PERM, &attr)) == -1)
		{
			fprintf(stderr, "Failed to open message queue from contral controller (%s, %d)!\n", message->file_mode, errno);
			flag = false;
		}
		
		if ((message->status = mq_open(message->file_status, MQ_WR, MQ_PERM, &attr)) == -1)
		{
			fprintf(stderr, "Failed to open message queue to contral controller (%s, %d)!\n", message->file_status, errno);
			flag = false;
		}
	}
	else
	{
		if ((message->mode = mq_open(message->file_mode, MQ_WR, MQ_PERM, &attr)) == -1)
		{
			fprintf(stderr, "Failed to open message queue to intersection (%s, %d)!\n", message->file_mode, errno);
			flag = false;
		}
		
		if ((message->status = mq_open(message->file_status, MQ_RD, MQ_PERM, &attr)) == -1)
		{
			fprintf(stderr, "Failed to open message queue from intersection (%s, %d)!\n", message->file_status, errno);
			flag = false;
		}
	}
	
	if (!flag)
	{
		/* Close good message queues even if one is good */
		message_clean(message, false);
	}
	
	return flag;
}

void message_clean(message_t *message, bool clean)
{
	if (message->mode != -1)
	{
		mq_close(message->mode);
		mq_unlink(message->file_mode);
	}
		
	if (message->status != -1)
	{
		mq_close(message->status);
		mq_unlink(message->file_status);
	}
	
	if (clean)
	{
		if (message->file_mode != NULL)
			free(message->file_mode);
		
		if (message->file_status != NULL)
			free(message->file_status);
	}
}
