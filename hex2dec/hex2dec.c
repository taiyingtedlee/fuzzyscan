#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 50

int main(void)
{
	char hex[SIZE]={'\0'};
	int buf[2]={'\0'};
	int i,j,len,ascii_d,decimal_val=0;
	
	printf("Input Cmd in Hex!\n");
	scanf("%[a-zA-Z0-9]",hex);

	len=strlen(hex);

	for (i=0,j=len;i<len;i++,j--)
	{
		// ASCII char 0-9 Dec 48-57
		if (hex[i]>='0' && hex[i]<='9')
		{
			ascii_d=hex[i]-48;
			buf[j-1]=ascii_d;
			printf("Dec (0-9):%d\n",buf[j-1]);

		// ASCII char A-F Dec 65-70
		}else if (hex[i]>='A' && hex[i]<='F')
		{
			ascii_d=hex[i]-65+10;
			buf[j-1]=ascii_d;
			printf("Dec (A-F)):%d\n",buf[j-1]);

		// ASCII char a-f Dec 97-102
		}else if (hex[i]>='a' && hex[i]<='f')
		{
			ascii_d=hex[i]-97+10;
			buf[j-1]=ascii_d;
			printf("Dec (a-f):%d\n",buf[j-1]);
		}

	}
	
	decimal_val=buf[1]*16+buf[0];
	
	printf("Len : %d\nDec : %d\n",len,decimal_val);
	printf("buf[0]: %d buf[1]: %d\n",buf[0],buf[1]);

	memset(buf,0,2);

	return 0;
}
