#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMPLE 1
#define COMPOUND 2

static unsigned char xtx_arr[]={0x00,0x01};
static unsigned char shk_arr[]={0x00,0x00,0x00};

struct _cmd_opcode 
{
	unsigned char opcode[3];
};

struct _cmd_para_table
{
	unsigned char id;
	unsigned short size;
	unsigned char *arr;
};

static const struct _cmd_para_table USBCOM[]=
{
	{0,1,{&xtx_arr}},
	{1,3,{&shk_arr}},
};

static const struct _cmd_para_table RS232[]=
{
	{0,1,{&xtx_arr}},
	{1,3,{&shk_arr}},
};

int main(void)
{
	printf("Size: %d size: %d",USBCOM[0].size[1],RS232[1].size[1]);

	printf("\n");
	return 0;
}
