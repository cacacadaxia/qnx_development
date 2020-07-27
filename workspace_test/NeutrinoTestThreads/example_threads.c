/*  T H R E A D  C R E A T I O N  */

/* Project Name: "example_threads" */

/* What are threads under QNX?
 * Under QNX Neutrino, there are processes and threads. It's
 * not a process that actually runs, it's (at least) one thread
 * that runs inside.
 * Threads are the ones being scheduled, they are consuming
 * CPU time when they run.
 * Per default, a process has one thread, the main thread.
 * To create this main thread, you don't need to do anything,
 * it is created automatically for you.
 * You can create further threads within your process, for
 * example to parallelize tasks. A process has its own virtual
 * address space. All threads share the memory of the process.
 * In this example program, we will create some additional
 * threads using the appropriate functions.
 */

/* In a second thread related example,
 * "example_sched.c", we will play with priorities
 * and scheduling algorithms.
 */

/* More information:
 * See "Processes and Threads" in the Programmer's Guide,
 * chapter "Programming Overview".
 * You will find it in the Help Viewer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>



void *functionA();
void *functionB();
void *universalthread(int parameter);


/* At this point our process goes live and has one thread, the
 * main thread. All setup for this was done by the OS.
 */

int main () {
	pthread_t	our_thread_id;


/* Next, we will create own, additional threads within this
 * process. All our threads will print something to the console,
 * so don't be surprised when it looks a little mixed-up. :-)
 * The function pthread_create is used to create new threads.
 * Parameters: a place for the thread ID, thread attributes,
 * address of the function for the thread to execute, and a
 * place to possibly pass in a parameter to that function.
 */

/* Let's create a thread!
 * Its thread ID will be put inside "our_thread_id".
 * For the Thread attributes (priority, scheduling) we use
 * the default values (Prio 10, Round Robin Scheduling). To
 * do this we just need to pass in NULL as parameter.
 * The function our first thread shall execute is functionA().
 * As a parameter for this function we don't pass anything in,
 * so we use NULL. */

	printf("Creating thread now.\n");
	pthread_create(&our_thread_id, NULL, &functionA ,NULL);
	printf("Thread was created.\n");

/* The thread is created now. It will print something from
 * time to time. Our main thread continues to run aswell, and
 * will run until it has consumed its time slice (round robin
 * scheduling). The newly created thread will run afterwards,
 * as it was placed at the end of the queue of threads that want
 * to get the CPU (the "ready queue").
 */

	printf("The main thread is going on...\n");
	printf("... as long as it hasn't used up its timeslice.\n");

/* Let's create another thread, this time with a different
 * function: functionB(). This time we don't care about the
 * thread ID, so we pass in NULL for that.
 */

	printf("Creating another thread.\n");
	pthread_create(NULL,NULL, &functionB ,NULL);
	printf("Thread was created.\n");
	printf("The main thread continues...\n");


/* You can provide own code (an individual function) for each
 * thread, but you don't have to at all. You can also create
 * a whole bunch of threads all using the same code.
 * This works very well, because every thread has its own stack.
 */


/* Now let's create a third and a forth thread, this time both
 * will use the same code (function). We will call the function
 * "universalthread()". We will pass a value to the function.
 * Inside the thread we will use this value to determine which
 * thread is wich upon console output.
 */

	printf("We are now creating two more threads...\n");
	pthread_create(NULL,NULL, (void *) &universalthread, (void *) 12);
	pthread_create(NULL,NULL, (void *) &universalthread, (void *) 59);
	printf("Threads created.\n");


/* Now, what shall we do in the main thread? In fact you can do
 * anything you want. It can do further work, or it can wait
 * for another thread to complete (thread exits its function).
 * Let's wait for a thread to complete! For this we need its
 * thread ID and the function pthread_join().
 */

	printf("Main thread now waiting for first thread to finish\n");

/* When we created the first thread, we stored the thread ID
 * in "our_thread_id". We use it now. The function pthread_join()
 * will not return until the corresponding thread completes.
 */
	pthread_join(our_thread_id, NULL);//这里结束线程
	printf("Thread %d completed its function!\n",our_thread_id);


/* Now let's have the main thread wait for cancellation through
 * user (Ctrl+C). The other threads will run until they are
 * completed. If you press Ctrl+C before they are completed,
 * they are terminated along with the main thread. If you wait
 * a while, the threads will finish on their own. Type "pidin"
 * on a different console or look in the System Information
 * Perspective - the finished threads will be labelled 'dead'.
 */

	printf("Main thread now waiting for termination through user.\n");
	printf("Other threads keep running in the background until\n");
	printf("they finish or are terminated together with the main thread.\n");

/* Hitting Ctrl+C sends a SIGTERM signal to the process. If the
 * process receives this signal, it is killed along with all of
 * its threads. We now use the pause() function to wait for
 * the signal.
 */
 	pause();

/* Hitting Ctrl+C will end the whole program, thus any lines
 * after the pause() and the } will not be executed.
 * If you want the main thread exit after the others threads
 * finished, use pthread_join() as shown above.
 * Have fun while exploring!
 */

	return EXIT_SUCCESS;
}


void *functionA()
{

	printf("functionA active\n");
	sleep(5);
	printf ("functionA is finished.\n");
	return(NULL);
}

void *functionB()
{

	printf("functionB is working...\n");
	sleep (7);
	printf("functionB done.\n");
	return(NULL);
}

void *universalthread(int parameter)
{
	int localvariable;
	int counter = 0;

	localvariable = 3;


	printf("universalthread is now running...\n");
	while ( counter < 5) {
		printf ("universalthread: localvariable %d * parameter %d = %d\n", localvariable, parameter, (localvariable * parameter));
		localvariable = localvariable + parameter;
		counter++;
		sleep(2);
	}
	printf("universalthread with parameter %d finished.\n",parameter);
	return(NULL);
}
