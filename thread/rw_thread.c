#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024
// pthread_mutex_t 

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int lock_var;

void *w_thread(void)
{
// mutex lock
	if(pthread_mutex_lock(&mutex)!=0)
	{
		printf("pthread_mutex_lock!\n");
	}else
		printf("W_THREAD LOCK the var\n");
// mutex lock
	int i;
	for(i=0;i<10;i++)
	{
		printf("w_thread %d th ......\n",i);
		sleep(1);
		// lock var		
		lock_var;
	}
// mutex unlock
	if (pthread_mutex_unlock(&mutex)!=0)
	{
		printf("pthread_mutex_unlock!\n");
	}else
		printf("W_THREAD UNLOCK the var\n");
// mutex unlock	
	sleep(1);
}

void *r_thread(void)
{
	int nolock=0;
	int ret,i;
// mutex trylock  EBUSY == 16 errono.h
	ret=pthread_mutex_trylock(&mutex);
	if(ret==16)
	{
		printf("r_thread: the var is locked by w_thread\n");
	}else
	{
		if(ret!=0)
		{
			printf("pthread_mutex_trylock\n");
			exit(1);
		}else
			printf("r_thread: GOT lock! Var: %d\n",lock_var);
// mutex trylock 

// mutex unlock
		if (pthread_mutex_unlock(&mutex)!=0)
		{
			printf("pthread_mutex_unlock\n");
		}else
		{		
			printf("r_thread: unlock the var\n");
			for(i=0;i<10;i++)
			{
				printf("r_thread %d th",i);
				sleep(1);
			}
		}

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
// pthread_mutex_init 
	pthread_mutex_init(&mutex,NULL);
	
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
	

	return 0;
}
