#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "./include/h2d.h"
#include "./include/sp_filter.h"
#include "./include/dispatch_cmd.h"

#define SIZE 255
#define USB0 "/dev/ttyUSB0"
#define USB1 "/dev/ttyUSB1"
#define CMD_BUF "cmd.txt"


int main(void)
{
	unsigned char mode='h';
	FILE *fp;
	unsigned char *p;
	unsigned char *p_cmd;
	unsigned char hex[SIZE]={'\0'};
	unsigned char r_hex[SIZE]={'\0'};
	int fd,res,len,i,sp=0;	
	
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
	while(1)
	{
		res=read(fd,r_hex,SIZE);
		if(res == 1) continue;
		r_hex[res]='\0';
		len=(int)strlen(r_hex);

		if ((r_hex[0]=='@' && r_hex[1]=='c') || (r_hex[0]=='@' && r_hex[1]=='h') )
		{
			mode = r_hex[1];
		}
	

		if(mode == 'h')
		{
			if (r_hex[0]=='@' && r_hex[1]=='h')
			{
				printf("Hex Input ...\n");
//				continue;
			}
	//		printf("ttyUSB recv : %s",r_hex);
	//		printf("strlen(r_hex) : %d\n",len);

			p=sp_filter(r_hex);
	//		printf("sp_filter(r_hex) : %s\n",p);
			sp=*(p+len-1);
	//		printf("Number of SPACE : %d\n",sp);
	//		printf("Elements without sp : %d\n",len-1-sp);

			p_cmd=h2d(p);
			dispatch_cmd(p_cmd,(len-1-sp));
			// save data to cmd.txt 	
			fwrite(p,(len-1-sp),1,fp);
			fseek(fp,SEEK_SET,0);
			fread(hex,sizeof(hex),1,fp);
		
		}else if(mode == 'c')
		{
			if (r_hex[0]=='@' && r_hex[1]=='c')
			{
				printf("ASCII Input ...\n");
				continue;
			}
			printf("%s",r_hex);
		}

		if (hex[len-1] == '\n') continue;
		printf("\n");
	}
	
	
	fclose(fp);
	close(fd);
	return 0;
}
