#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	unsigned char a= 2;
	unsigned char b= 2;

	int len = (a<<8) | b;
	printf("%02x %d ",a,len);
	return 0;
}
