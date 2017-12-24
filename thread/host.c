#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/sp_filter.h"
#include "../include/h2d.h"
#include "../include/dispatch_cmd.h"

#define USB0 "./USB0"
#define USB1 "./USB1"
#define SIZE 1024


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned char str[SIZE]={'\0'};
FILE *fp_host,*fp_dev;
int fd;

void *hostw_thread(void)
{
	int len;
	usleep(100);
	// Start cirtical section
	pthread_mutex_lock(&mutex);
	
	memset(str,'\0',SIZE);
	
	fp_host=fopen(USB0,"w+");
	printf("Input cmd : ");
	fgets(str,sizeof(str),stdin);
	len=(int)strlen(str);
	if(fp_host==NULL)
	{
		printf("fopen failure!\n");
		exit(1);
	}else
		fwrite(str,len,1,fp_host);
	
	fflush(stdin);

	printf("----------------------------\n");
	
	pthread_mutex_unlock(&mutex);
	// End cirtical section
	pthread_exit(NULL);
}

void *devr_thread(void)
{
	sleep(1);
	int i=0,len,sp=0,res;
	unsigned char r_str[SIZE]={'\0'};
	unsigned char *p_str;	
	unsigned char *p_cmd;
	
	if (fp_host==NULL)
	{
		printf("fopen() failure!\n");
		exit(1);
	}

	// Start Critical section
	pthread_mutex_lock(&mutex);

	fp_dev=fopen(USB1,"w+");
	if(fp_dev==NULL)
	{
		printf("fopen() failure!\n");
		exit(1);
	}

	printf("DEV Data recv : %s",str);
	
	fseek(fp_host,SEEK_SET,0);
	fread(r_str,strlen(str),1,fp_host);
	
	len=(int)strlen(r_str);
	p_str=sp_filter(r_str);
	sp=p_str[len-1];
	p_cmd=h2d(p_str);
	dispatch_cmd(p_cmd,(len-1-sp));
	printf("p_cmd[0] : %d\n",p_cmd[0]);	
	printf("DEV write ...\n");
	fwrite(r_str,strlen(r_str),1,fp_dev);

	pthread_mutex_unlock(&mutex);
	// End Critical section
	pthread_exit(NULL);
}

void *hostr_thread(void)
{
	unsigned char buf[SIZE]={'\0'};
	sleep(2);
	// Start cirtical section
	pthread_mutex_lock(&mutex);
	
	fseek(fp_dev,SEEK_SET,0);
	fread(buf,sizeof(buf),1,fp_dev);
	
	printf("host read from dev :\n");
	printf("%s\n",buf);
	
	
	
	pthread_mutex_unlock(&mutex);
	// End cirtical section
	pthread_exit(NULL);
}






int main(void)
{
	int ret1,ret2,ret3;
	pthread_t devr_th,hostw_th,hostr_th;
	

	while(1){
	ret1=pthread_create(&hostw_th,NULL,(void*)hostw_thread,NULL);
	if (ret1!=0)
	{
		printf("pthread_create failure!\n");
		exit(1);
	}

	ret2=pthread_create(&devr_th,NULL,(void*)devr_thread,NULL);
	if(ret2)
	{
		printf("pthread_create failure!\n");
		exit (1);
	}	
	ret3=pthread_create(&hostr_th,NULL,(void*)hostr_thread,NULL);
	if(ret3)
	{
		printf("pthread_create failure!\n");
		exit (1);
	}



	printf("---------M A I N------------\n");
		
	pthread_join(hostw_th,NULL);
	pthread_join(devr_th,NULL);
	pthread_join(hostr_th,NULL);

	printf("\n----------E N D-------------\n\n\n\n\n\n\n");
	fclose(fp_host);
	fclose(fp_dev);
	}

	return 0;
}

