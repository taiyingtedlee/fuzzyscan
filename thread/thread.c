#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024

void *w_thread(void)
{
	int i;
	for(i=0;i<10;i++)
	{
		printf("w_thread %d th ......\n",i);
		sleep(1);
	}
}

void *r_thread(void)
{
	int i;
	for(i=0;i<10;i++)
	{
		printf("r_thread %d th......\n",i);
		sleep(2);
	}	
}

int main(void)
{
	FILE *fp;
	unsigned char *p_hex;
	unsigned char hex[SIZE]={'\0'};

	int ret1,ret2;
	pthread_t r_th,w_th;
	
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
	sleep(3);
	pthread_join(w_th,NULL);
	pthread_join(r_th,NULL);
	

	return 0;
}
