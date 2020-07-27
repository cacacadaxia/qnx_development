//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/neutrino.h>
//#include <pthread.h>
//#include <sched.h>
//#include <unistd.h>
//
//
//
//void *functionA();
//void *functionB();
////void *universalthread(int parameter);
//
//
////int main () {
////	pthread_t	myID;
////	int tp = 100;
////	pthread_create(&myID, NULL, &functionA ,NULL);
////	pthread_create(NULL, NULL, &functionB ,&tp);
////
////	pthread_join(myID,NULL);
////	return EXIT_SUCCESS;
////
////
////}
//
//void *functionA()
//{
//	printf("functionA active\n");
//	sleep(5);
//	printf ("functionA is finished.\n");
//	return(NULL);
//}
//
//void *functionB(int * value)
//{
//
////	printf("functionB is working...\n");
////	sleep (7);
////	printf("functionB done.\n");
//	printf("out is %d\n" , *value);
//	return(NULL);
//}
////
////void *universalthread(int parameter)
////{
////	int localvariable;
////	int counter = 0;
////
////	localvariable = 3;
////
////
////	printf("universalthread is now running...\n");
////	while ( counter < 5) {
////		printf ("universalthread: localvariable %d * parameter %d = %d\n", localvariable, parameter, (localvariable * parameter));
////		localvariable = localvariable + parameter;
////		counter++;
////		sleep(2);
////	}
////	printf("universalthread with parameter %d finished.\n",parameter);
////	return(NULL);
////}
