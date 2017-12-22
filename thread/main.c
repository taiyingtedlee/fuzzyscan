#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/sp_filter.h"
#include "../include/h2d.h"
#include "../include/dispatch_cmd.h"
#define USB0 "/dev/ttyUSB0"
#define USB1 "/dev/ttyUSB1"
#define SIZE 1024


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned char str[SIZE]={'\0'};
FILE *fp;
int fd;

void *w_thread(void)
{
	int len;
	usleep(100);
	// Start cirtical section
	pthread_mutex_lock(&mutex);
	
	memset(str,'\0',SIZE);
	
	fp=fopen(USB0,"r+");
	printf("Input cmd : ");
	fgets(str,sizeof(str),stdin);
	len=(int)strlen(str);
	if(fp==NULL)
	{
		printf("fopen failure!\n");
		exit(1);
	}else
		fwrite(str,len,1,fp);
	
	fflush(stdin);

	printf("----------------------------\n");
	
	pthread_mutex_unlock(&mutex);
	// End cirtical section
	pthread_exit(NULL);
}

void *r_thread(void)
{
	sleep(1);
	int i=0,len,sp=0,res;
	unsigned char r_str[SIZE]={'\0'};
	unsigned char *p_str;	
	unsigned char *p_cmd;
	
	fd=open(USB1,O_RDWR|O_NOCTTY);
	if (fd<0)
	{
		printf("open() failure!\n");
		exit(1);
	}
	// Start Critical section
	pthread_mutex_lock(&mutex);

	res=read(fd,r_str,SIZE);
	
	printf("Data recv : %s",str);
	len=(int)strlen(r_str);
	p_str=sp_filter(r_str);
	sp=p_str[len-1];
	p_cmd=h2d(p_str);
	dispatch_cmd(p_cmd,(len-1-sp));
	
	pthread_mutex_unlock(&mutex);
	// End Critical section
	pthread_exit(NULL);
}

int main(void)
{
	unsigned char *p_hex;
	unsigned char hex[SIZE]={'\0'};
	unsigned char r_hex[SIZE]={'\0'};
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
	

	printf("---------M A I N------------\n");
		
	pthread_join(w_th,NULL);
	pthread_join(r_th,NULL);

	printf("\n----------E N D-------------\n\n\n\n\n\n\n");
	fclose(fp);
	close(fd);
	}

	return 0;
}
