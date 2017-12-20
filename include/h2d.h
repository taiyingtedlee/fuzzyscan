#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define H2D_SIZE 255

unsigned char * h2d(unsigned char hex[])
{
	static unsigned char ch_dec[H2D_SIZE]={'\0'};
	int len = strlen(hex);
	int i;

	memset(ch_dec,'\0',H2D_SIZE);

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
	
	// clean up and save length at ch_dec[len/2] (end of string);
	for(i=len/2;i<len;i++)
	{
		ch_dec[i]='\0';
	}
	ch_dec[len/2]=len;

/*  // ckeck results 
	for (i=0;i<len/2;i++)
	{
		printf("Char[%d] : %c dec : %d\n",i,ch_dec[i],ch_dec[i]);
	}
*/
	return ch_dec;

}

