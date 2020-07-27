#include <pthread.h>

#define MAX_TEXT_LEN    100

/* Here we define the contents of our shared memory object
 * using a structure. All processes that want to access the
 * shared memory object should use this structure to access
 * the proper contents. */

typedef struct {
	/* A mutex for synchronizing shmem access */
    pthread_mutex_t  myshmemmutex;
    /* A text string */
    char    text[MAX_TEXT_LEN+1];
    /* Anything you want can be in the shared memory */

    int baba;
} shmem_t;
