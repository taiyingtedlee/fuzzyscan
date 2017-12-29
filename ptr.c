#include <stdio.h>

int main(int argc, char * argv)
{
	char num = 45;
	char* ptr = &num;
	
	printf("before *ptr ...  \n");
	printf("&num=%p, num=%d\n",&num, num);
	printf("&ptr=%p, ptr= %p, *ptr=%d\n",&ptr, ptr, *ptr);

	*ptr = 60;
	printf("after *ptr ... \n");
	printf("&num=%p, num=%d\n",&num, num);
	printf("&ptr=%p, ptr= %p, ptr+4 = %p, *ptr=%d\n",&ptr, ptr,ptr+4,*ptr);
	return 0;


}
