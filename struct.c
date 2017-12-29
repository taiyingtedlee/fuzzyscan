#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMPLE	 0
#define COMPOUND 2 
#define TYPE 	1

static unsigned char xtx_arr[]={0x30,0x45,0x33,0x44,0x65,0x38};

struct _para_arr
{
	void *arr;
};

struct _cmd_option2_minor
{
	unsigned char id;
 	/* why const struct _para_arr ""*arr""
	**	not const struct _parr_arr ""Arr""
	*/ 
	const struct _para_arr *para_arr;
};

static const struct _para_arr XTX_arr={xtx_arr};

static const struct _cmd_option2_minor USBCOM[]=
{
	{0,&XTX_arr},
	{0,NULL},
};

int main(void)
{
	struct _cmd_option2_minor *st_p_opt2_minor;
	struct _para_arr *st_p_para_arr;
	printf("&xtx_arr : %p , xtx_arr[1] : %p \n",&xtx_arr,&(xtx_arr[1]));
	printf("&XTX_arr : %p ,XTX_arr : %p sizeof(XTX_arr) : %lu\n",&XTX_arr,XTX_arr,sizeof(XTX_arr));
	
	st_p_opt2_minor = USBCOM[0].para_arr;
	

	printf("\n");
	printf("[0] : %02x ",xtx_arr[1]);
	return 0;	
}

/*
**struct.c 29:5: error : initializer element is not constant
	{0,XTX_arr},
**struct.c 29.5: note: (near initialization for 'USBCOM[0].arr')
*/


