#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "./include/sp_filter.h"
#include "./include/h2d.h"

#define SIZE 1024
#define USB0 "/dev/ttyUSB0"
#define USB1 "/dev/ttyUSB1"
#define CMD "cmd.txt"


int main(void)
{
	FILE *fp;
	unsigned char *p_hex;
	unsigned char *p_parse;
	unsigned char hex[SIZE]={'\0'};
	unsigned char r_hex[SIZE]={'\0'};
	int i=0,len,sp=0;	

	printf("Sending data to ttyUSB0...\n");
	
	fp=fopen(CMD,"w+");

	while(1)
	{	
		printf("Input :");
		fgets(hex,sizeof(hex),stdin);
	
		len=(int)strlen(hex);

		p_hex=sp_filter(hex);
		sp=*(p_hex+len-1);
		printf("Number of SPACE : %d\n",sp);
		printf("Elements without sp : %d\n",len-1-sp);

		if(fp==NULL)
		{
			printf("fopen failure!\n");
			exit (1);
		}else
		{
			fwrite(p_hex,len,1,fp);
		}
	
		fseek(fp,SEEK_SET,0);
		
		fread(r_hex,sizeof(hex),1,fp);
		
		p_parse=h2d(r_hex);
		for (i=0;i<(len-1-sp)/2;i++){
			printf("%d ",p_parse[i]);
		}
//		printf("len : %d\n",(int)strlen(r_hex));
//		printf("%s\n",r_hex);
		fflush(stdin);
	}
	fclose(fp);
	return 0;
}
