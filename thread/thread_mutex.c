#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned char str[SIZE]={'\0'};

void *w_thread(void)
{
	int i;
	sleep(1);
	printf("USB send...\n");
	for(i=0;i<10;i++)
	{
		// Start cirtical section
		pthread_mutex_lock(&mutex);
		
		str[i]=i;
		printf("w_thread %d th ... write str[%d] : %d\n",i,i,str[i]);
		pthread_mutex_unlock(&mutex);
		// End cirtical section
	}
	sleep(1);
}

void *r_thread(void)
{
	sleep(2);
	printf("USB recv...\n");
	int i;
	for(i=0;i<10;i++)
	{
		//Start Cirtical section 
		pthread_mutex_lock(&mutex);
		printf("r_thread %d th... str[%d] : %d\n",i,i,str[i]);
		pthread_mutex_unlock(&mutex);
		// End Critical section
	}
	sleep(1);
}

int main(void)
{
	FILE *fp;
	unsigned char *p_hex;
	unsigned char hex[SIZE]={'\0'};

	int ret1,ret2;
	pthread_t r_th,w_th;
	while(1){
	ret1=pthread_create(&w_th,NULL,(void*)w_thread,NULL);
	if (ret1!=0)
	{
		printf("pthread_create failure!\n");
		exit(1);
	}

	ret2=pthread_create(&r_th,NULL,(void*)r_thread,NULL);
	if(ret2)
	{
		printf("pthread_create failure!\n");
		exit (1);
	}
	
	printf("MAIN thread...\n");
		
	pthread_join(w_th,NULL);
	pthread_join(r_th,NULL);
	printf("pthreads task finished!\n");
	}
	return 0;
}
