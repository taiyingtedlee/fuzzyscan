#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024
#define SIZE 100

typedef struct Parse_arr
{
	unsigned char dec_cmd[SIZE];

}Ret_arr;

Ret_arr parse_ch_arr_s(unsigned char str[])
{
	Ret_arr output;
	
	int i;
	unsigned int len=(unsigned int)strlen(str);

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
			
			output.dec_cmd[i/2]+=str[i];

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

			output.dec_cmd[i/2]+=str[i];

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

			output.dec_cmd[i/2]+=str[i];
		}
		
	}

	return output;
}


int main(void)
{
    unsigned char cmd[SIZE];
	unsigned char f_cmd[SIZE];
	unsigned char *p_cmd;
	unsigned char *ret_p_arr;
	unsigned int len;
	Ret_arr ret_arr;
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
		ret_arr=parse_ch_arr_s(f_cmd);
		printf("f_cmd len: %d\n",(int)strlen(f_cmd));
		for(i=0;i<strlen(f_cmd)/2;i++)
		{
			printf("%d",ret_arr.dec_cmd[i]);
		}
		printf("\n");
	}
	
	fclose(fp);

#else

	p_cmd=(unsigned char *)malloc(sizeof(char)*MAX);

	while(1){
		printf("Input cmd :\n");
   		scanf("%[a-fA-F0-9]",p_cmd); 
		printf("p_cmd : %s\n",p_cmd);
		
		len=strlen(p_cmd);
		printf("input len : %d\n",len);
		
		ret_arr=parse_ch_arr_s(p_cmd);
		printf("par_a : ");
		for (i=0;i<len/2;i++)
		{
			printf("%c ",ret_arr.dec_cmd[i]);
		}
		printf("\n");
		setbuf(stdin,NULL);
	}
	free(p_cmd);
#endif

	return 0;
}

