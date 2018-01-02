#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMPLE	 0
#define COMPOUND 2 
#define TYPE 	1

struct _para_arr
{
	void *option;
};

struct _cmd_option2_minor
{
	unsigned char pid;
	unsigned char size;
	const struct _para_arr *para_table;
};

struct _cmd_option1_major
{	
	unsigned char id;
	const struct _cmd_option2_minor *option1;
};

static const unsigned char baud_arr={0x00}; 
// 0x00 1600 0x01 115200 
static const unsigned char shk_arr={0x45,0x00,0x00}; 
// {0x00 0x00 0x00} OFF {0x00 0x00 0x01} ON  {0x00 0x01 0x00} REPLY

static const struct _para_arr BAUD_arr={&baud_arr};
static const struct _para_arr SHK_arr={&shk_arr};


static const struct _cmd_option2_minor USB[]=
{
	{0,1,&BAUD_arr},
	{1,3,&SHK_arr},
	{0,0,NULL},
};

static const struct _cmd_option2_minor RS232[]=
{
	{0,1,&BAUD_arr},
	{1,3,&SHK_arr},
	{0,0,NULL},
};

static const struct _cmd_option1_major Action[]=
{
	{0,&USB},
	{1,&RS232},
	{0,NULL},
};

int main(void)
{
	struct _cmd_option2_minor *st_p_opt2_minor;
	struct _para_arr *st_p_para_arr;

	printf("&baud_arr : %p, baud_arr : %02x\n",&baud_arr,baud_arr);
	printf("&shk_arr : %p, shk_arr : %02x\n",&shk_arr,shk_arr);
//	printf("&xtx_arr : %p , xtx_arr[1] : %p \n",&xtx_arr,&(xtx_arr[1]));
//	printf("&XTX_arr : %p ,XTX_arr : %p sizeof(XTX_arr) : %lu\n",&XTX_arr,XTX_arr,sizeof(XTX_arr));
	printf("\n");
//	printf("[0] : %02x ",baud_arr);
	return 0;	
}

