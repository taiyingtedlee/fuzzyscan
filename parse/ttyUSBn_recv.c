#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../h2d.h"


#define SIZE 255
#define USB0 "/dev/ttyUSB0"
#define USB1 "/dev/ttyUSB1"
#define CMD_BUF "cmd.txt"


int main(void)
{
	FILE *fp;
	unsigned char *p;
	unsigned char hex[SIZE]={'\0'};
	unsigned char r_hex[SIZE]={'\0'};
	int fd,res,len,i;	
	
	printf("Reading ttyUSB1...\n");
	fd=open(USB1,O_RDWR|O_NOCTTY);
	if (fd<0)
	{
		printf("open() failure!\n");
		exit (1);
	}

	fp=fopen(CMD_BUF,"w+");
	if(fp==NULL)
	{
		printf("fopen failure!\n");
		exit (1);
	}

	// read(int fd, void *buffer,size_t count) ; read '\n' as well;	
	do
	{
		res=read(fd,r_hex,SIZE);
		if(res == 1) continue;
		r_hex[res]='\0';
		len=(int)strlen(r_hex);
		printf("strlen(r_hex) : %d\n",len);
/*
		for (i=0;i<res;i++)	
		{	
			if(r_hex[i] == '\n') 
			{
				break;
			}else
				printf("res : %d buf : %c\n",res,r_hex[i]);
		}
*/		
		printf("buf : %s\n",r_hex);

		// save data to cmd.txt 	
		fwrite(r_hex,len,1,fp);
		fseek(fp,SEEK_SET,0);
		fread(hex,sizeof(hex),1,fp);
		if (hex[len-1] == '\n') continue;
	
	}while(r_hex[0] != '@');

	
	fclose(fp);
	close(fd);
	return 0;
}
