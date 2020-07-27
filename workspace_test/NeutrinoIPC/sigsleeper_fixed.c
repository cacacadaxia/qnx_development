
/*
 *  sigsleeper.c
 *
 *  This module demonstrates signal handling,
 *  and some of the problems you can run into.
 *
 */

#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <process.h>
#include <signal.h>
#include <stdlib.h>

char    *progname = "sigsleeper_fixed";

void    myHandler(int signo);
int     critical_operation(int);

struct  sigaction   action;

/* This thread only listens for signals */
void *sig_thread(void *notused) {
	sigset_t set;
	siginfo_t siginf;

	sigemptyset(&set);
	sigaddset(&set, SIGTERM);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	while (1) {
		sigwaitinfo(&set, &siginf);
		myHandler(siginf.si_signo);
	}
}

main() {
	time_t          now, then;
	unsigned int    timeleft;
	sigset_t set, old_set;
	const unsigned int TIMEOUT = 20;

	setvbuf(stdout, NULL, _IOLBF, 0);

	printf("You have 60 seconds to interrupt me! (pid %d)\n", getpid());
	printf("Hit me with SIGTERM by doing 'slay sigsleeper'.\n");
	printf("Or by using a target navigator to send a signal.\n");

	// created thread for processing signals
	pthread_create(NULL, NULL, sig_thread, NULL);


	// do not want to receive signals by this thread
	sigemptyset(&set);
	sigaddset(&set, SIGTERM);
	pthread_sigmask(SIG_UNBLOCK, &set, &old_set);

	time(&now);
	timeleft = critical_operation(TIMEOUT);
	time(&then);

	// restore signal mask
	pthread_sigmask(SIG_SETMASK, &old_set, NULL);

	printf("Return from critical_operation was %d\n", timeleft);

	if (then - now < TIMEOUT) {
		printf("Hmmm.... we didn't complete, only did %d seconds!\n",
				then - now);
	} else {
		printf("Done!\n");
	}
	printf("\nDone!\nExitting...\n");
	exit(0);
}

/* Signal handling function */
void
myHandler(int signo) {
	printf("Inside myHandler().  Got hit with signal %d.\n", signo);
}

int critical_operation(int seconds) {
	return sleep(seconds);
}
