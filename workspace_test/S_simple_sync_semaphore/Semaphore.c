
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

/*�źŵƵ�ʹ�÷��������ֱ�ӵ�˵�����ڹ���ʱ�������������źŵƣ���ʾֻ������������ͨ��
�������̵߳������߼�����û���ر����
Ϊʲô����ĺ��������forѭ��������ʵ���ϲ�û�����forѭ����
���ʾ��ò�Ʋ�û�н������
*/

int onBridge=0;
int saveNum = 3;
sem_t sem;

void* semFunction1(void* arg) {
	int i=0;
	for (i=0;i<20;i++)
	{
		printf("waiting a sem \n");
		sem_wait(&sem);
		printf("one soldier walking through the bridge\n");
		onBridge++;
		sleep(1);
		printf("one soldier left the bridge\n");
		onBridge--;
		printf("posting a sem \n");
		sem_post(&sem);
	}
	return 0;
}
int main(int argc, char *argv[]) {

	printf("Starting Semaphore Test\n");
	sem_init( &sem, 0, saveNum);
	pthread_create( NULL, NULL, &semFunction1, NULL );
	pthread_create( NULL, NULL, &semFunction1, NULL );
	pthread_create( NULL, NULL, &semFunction1, NULL );
	int i=0;
	while(1)
	{
		i++;
		if (onBridge>saveNum)
		{
			printf("%d people on the bridge,the bridge is falling down.\n",onBridge);
		}
		else
		{
			printf("%d people on the bridge,it is save.\n",onBridge);
		}
		delay(200);
		if(i==10)
			break;
	}
	printf("fuck you\n");
	return 0;
	return EXIT_SUCCESS;
}