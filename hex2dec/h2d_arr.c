#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 128

unsigned char * h2d(unsigned char s[]);

int main(void)
{
	unsigned char *p_ch;
	unsigned char hex[SIZE]={'\0'};
	while(1){	
		printf("Input Char in Hex (ex: 7e 45 09 80 ...): \n");
		scanf("%[0-9a-zA-Z]",hex);

		printf("Input Length : %d\n",(int)strlen(hex));
		printf("Output Str: %s\n",h2d(hex));
		
		setbuf(stdin,NULL);
	}
	return 0;
}

unsigned char * h2d(unsigned char hex[])
{
	static unsigned char ch_dec[SIZE]={'\0'};
	int len = strlen(hex);
	int i;

	memset(ch_dec,'\0',SIZE);

	for (i=0;i<len;i++)
	{
		// ASCII char 0-9 Dec 48-57
		if (hex[i]>='0' && hex[i]<='9')
		{
			hex[i]=hex[i]-48;
			if ((i%2)==0)
			{
				hex[i]=hex[i]<<4;
			
			}else if ((i%2)==1)
			{
				hex[i]=hex[i];
			}

			ch_dec[i/2]+=hex[i];

		// ASCII char A-F Dec 65-70
		}else if (hex[i]>='A' && hex[i]<='F')
		{
			hex[i]=hex[i]-65+10;
			if ((i%2)==0)
			{
				hex[i]=hex[i]<<4;
			
			}else if ((i%2)==1)
			{
				hex[i]=hex[i];
			}

			ch_dec[i/2]+=hex[i];

		// ASCII char a-f Dec 97-102
		}else if (hex[i]>='a' && hex[i]<='f')
		{
			hex[i]=hex[i]-97+10;
			if ((i%2)==0)
			{
				hex[i]=hex[i]<<4;
			
			}else if ((i%2)==1)
			{
				hex[i]=hex[i];
			}

			ch_dec[i/2]+=hex[i];
		}

	}

	for (i=0;i<len/2;i++)
	{
		printf("Char[%d] : %c dec : %d\n",i,ch_dec[i],ch_dec[i]);
	}

	return ch_dec;

}

