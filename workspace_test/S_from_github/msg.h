#ifndef MSG_H
#define MSG_H

#include <stdlib.h>
#include <errno.h>

#include <mqueue.h>

#include "util.h"

#define DIR_IN 0
#define DIR_OUT 1

#define MQ_PATH "/"

#define MQ_RD O_RDONLY | O_CREAT | O_NONBLOCK
#define MQ_WR O_WRONLY | O_CREAT | O_NONBLOCK
#define MQ_PERM 777

#define BUFFER_SIZE 256

typedef char dir_t;

typedef struct {
	char *file_mode;
	char *file_status;

	mqd_t mode;
	mqd_t status;
} message_t;

bool message_init(message_t *message, dir_t direction, int id);
void message_clean(message_t *message, bool clean);

#endif
