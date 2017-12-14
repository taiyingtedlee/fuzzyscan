#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "h2d.h"

#define SIZE 1024
#define MAX	4096

int main(void)
{	
	FILE *fp;
	unsigned char *p_hex;
	unsigned char w_hex[SIZE]={'\0'};
	unsigned char r_hex[SIZE]={'\0'};


//	while(1){
	fp=fopen("cmd.txt","w+");
	p_hex=(char*)malloc(sizeof(char)*MAX);
	
	printf("Input : ");

	fgets(w_hex,sizeof(w_hex),stdin);

	if(fp==NULL)
	{
		printf("fopen failure!\n");
		exit(1);
	}else
	{
		fwrite(w_hex,sizeof(w_hex),1,fp);
	}

	setbuf(stdin,NULL);
//	fflush(stdin);

	fseek(fp,SEEK_SET,0);
	
	fread(r_hex,sizeof(r_hex),1,fp);
	printf("%s\n",r_hex);
	printf("r_hex len : %d\n",(int)strlen(r_hex));
	
	printf("parse ch : %s\n",h2d(r_hex));

	free(p_hex);
	fclose(fp);
//	}
	return 0;
}

