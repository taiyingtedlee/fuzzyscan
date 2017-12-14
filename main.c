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
	unsigned char *p_parse_ch;
	unsigned char w_hex[SIZE]={'\0'};
	unsigned char r_hex[SIZE]={'\0'};
	int i,j,sp=0;
	int len;

//	while(1){
	fp=fopen("cmd.txt","w+");
	p_hex=(char*)malloc(sizeof(char)*MAX);
	
	printf("Input : ");

	fgets(w_hex,sizeof(w_hex),stdin);
	// NOTE: fgets(string) will count '\n'(ENTER) as well; 34'\n'
	len=strlen(w_hex)-1;
	
	for(i=0,j=0,sp=0;j<len-sp;i++,j++){
		if(w_hex[i]!=' '){
			w_hex[j]=w_hex[i];
			printf("j : %d i : %d\n",j,i);
		}else if (w_hex[i]==' '){
			sp++;
			w_hex[j]=w_hex[i];
			j--;
			printf("j : %d i : %d\n",j,i);
		}
	}
	w_hex[(len-sp)]='\0';
	printf("w_hex len : %d\n",len);

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
// 	ret_len = strlen(r_hex)/2	must be ahead p_parse_ch = h2d(r_hex)  
	int ret_len=(int)strlen(r_hex)/2;
	printf("ret_len : %d\n",ret_len);

	p_parse_ch=h2d(r_hex);
	for(i=0;i<ret_len;i++){
		printf("parse ch[%d] : %c dec %d\n",i,*(p_parse_ch+i),*(p_parse_ch+i));
	}

	free(p_hex);
	fclose(fp);
//	}
	return 0;
}

