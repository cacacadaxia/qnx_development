#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>
#define INTNUM 0

//无疑这个修改之后的中断函数是成功的

void* Interrupt_function(void* notused);
void * interrupt_thread (void * data)
{
    struct sigevent event;
    int             id;

    /* fill in "event" structure */
    memset(&event, 0, sizeof(event));
    event.sigev_notify = SIGEV_INTR;

    /* Obtain I/O privileges */
    ThreadCtl( _NTO_TCTL_IO, 0 );

    /* intNum is the desired interrupt level */
    id = InterruptAttachEvent (INTNUM, &event, 0);

    /*... insert your code here ... */

    while (1) {
    	//实际上这里在不断的中断
        InterruptWait (NULL, NULL);
        /*  do something about the interrupt,
         *  perhaps updating some shared
         *  structures in the resource manager
         *
         *  unmask the interrupt when done
         */
        printf("eeeee\n");
//        sleep(1);
        InterruptUnmask(INTNUM, id);
    }
}

int main(int argc, char *argv[]) {
	printf("Interrupt process is ready, pid = %d\n", getpid());

	pthread_attr_t attr;
	int Thread_ID;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	attr.__param.__sched_priority = 21;
//	pthread_create(&Thread_ID, &attr, Interrupt_function, NULL);

    pthread_create (NULL, NULL, interrupt_thread, NULL);


	sleep(10);
	pthread_join(Thread_ID, NULL);

	return EXIT_SUCCESS;
}



//写一个中断函数？
void* Interrupt_function(void* notused){
	struct sigevent event;//sigevent 事件信号？
	int 			InitID;

	SIGEV_INTR_INIT(&event);
//	event.sigev_notify = 10;

	InitID = InterruptAttachEvent(0, &event, 0);//将中断加入到进程中

	while(1){
		InterruptWait(0, NULL);
		printf("Hit a interrupt\n");
		InterruptUnmask(0, InitID); //没有中断的结果？
	}


}
