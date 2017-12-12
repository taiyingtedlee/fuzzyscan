#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024
#define SIZE 100

unsigned char * parse_ch_arr(unsigned char s[]);

int main(void)
{
    unsigned char cmd[SIZE];
	unsigned char f_cmd[SIZE];
	unsigned char *p_cmd;
	unsigned char *ret_arr;
	FILE *fp;
	int i;
	
#ifdef R_FILE
	fp=fopen("test.txt","r");
	if (fp == NULL)
	{
		printf("fopen read Failure!!!\n");
	}

	while (fgets(f_cmd,SIZE,fp)!=NULL)
	{
		printf("Len : %s\n",parse_ch_arr(f_cmd));	
		ret_arr=parse_ch_arr(f_cmd);
	}

	fclose(fp);

#else

	p_cmd=(unsigned char *)malloc(sizeof(char)*MAX);
	while(1){
		printf("Input cmd :\n");
   		scanf("%[a-fA-F0-9]",p_cmd); 
		printf("p_cmd : %s\n",p_cmd);
		
		ret_arr=parse_ch_arr(p_cmd);
		printf("ret_arr : %s\n",ret_arr);
		setbuf(stdin,NULL);

	}
	free(ret_arr);
#endif
	free(p_cmd);
	return 0;
}

unsigned char * parse_ch_arr(unsigned char str[])
{
	int i;
	unsigned int len=(unsigned int)strlen(str);
	unsigned char *pparse_str2ch;	
	pparse_str2ch=(unsigned char*)malloc(sizeof(char)*MAX);

	for (i=0;i<len;i++)
	{
		
		//Convert ASCII char 0-9 Dec 48-57
		if (str[i]>='0' && str[i]<='9')
		{
			str[i]=str[i]-48;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]<<4;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}
			
			// High  	
			*(pparse_str2ch+(i/2))+=str[i];

		//Convert ASCII char A-F Dec 65-70
		}else if (str[i]>='A' && str[i]<='F')
		{
			str[i]=str[i]-65+10;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]<<4;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}

			*(pparse_str2ch+(i/2))+=str[i];

		//Convert ASCII char a-f Dec 97-102	
		}else if (str[i]>='a' && str[i]<='f')
		{
			str[i]=str[i]-97+10;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]*16;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}

			*(pparse_str2ch+(i/2))+=str[i];

		}
		
	}

	return pparse_str2ch;

}

