#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define SIZE 1024
int main(void)
{
	FILE *fp;
	unsigned char *p_hex;
	unsigned char hex[SIZE]={'\0'};
	unsigned char r_hex[SIZE]={'\0'};

	fp=fopen("1.txt","w+");
	
	printf("Input :");
	fgets(hex,sizeof(hex),stdin);

	if(fp==NULL)
	{
		printf("fopen failure!\n");
		exit (1);
	}else
	{
		fwrite(hex,sizeof(hex),1,fp);
	}
	
	fseek(fp,SEEK_SET,0);

	fflush(stdin);
	
	fread(r_hex,sizeof(hex),1,fp);
	printf("%s",r_hex);
	printf("len : %d\n",(int)strlen(r_hex));

	fclose(fp);

	return 0;
}
