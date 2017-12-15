#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRE_SUF 2
#define SIZE 1024

int main(void)
{
	FILE *fp;
	unsigned char ch;
	unsigned char pre_suf[PRE_SUF]={0x7e,0x7e};
	unsigned char h_buf[SIZE]={'\0'};
	int i=0;
	fp=fopen("cmd.txt","r+");
	if(fp ==NULL)
	{
		printf("fopen failure!\n");
		exit (1);
	}
	
	do
	{
		ch=fgetc(fp);
		h_buf[i++]=ch;
	}while(ch!='\n');

	printf("h_buf : %s",h_buf);

	fclose(fp);

	return 0;
}

