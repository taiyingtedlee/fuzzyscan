#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

unsigned char * sp_filter(unsigned char str[])
{
	static unsigned char buf[BUF_SIZE]={'\0'};
	int len = strlen(str)-1;
	int i,j,sp=0;
	
	memset(buf,'\0',BUF_SIZE);

	for(i=0,j=0,sp=0;j<len-sp;i++,j++)
	{
		if(str[i]!=' ')
		{
			buf[j]=str[i];
		}else if (str[i]==' ')
		{
			sp++;
			buf[j]=str[i];
			j--;
		}
	}
	for(i=len-sp;i<len;i++){
		buf[i]='\0';
	}
//	printf("j: %d\n",j);
//	printf("len-sp : %d\n",len-sp);
	// Number of SPACE save at buf[len]	
	buf[len]=sp;
//	printf("Num of SPACE : %d save at *(ptr+%d)\n",sp,len);

	return buf;
}
