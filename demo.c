#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 10
#define SIMPLE	 0
#define COMPOUND 2 
#define TYPE 	1

struct _para_arr
{
	unsigned char arr[SIZE];
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

//static const unsigned char baud_arr[]r={{0x00},{0x01}}; 
// 0x00 1600 0x01 115200 
//static const unsigned char shk_arr[]={{0x00,0x00,0x00},{0x00,0x00,0x01},{0x00,0x01,0x00}}; 
// {0x00 0x00 0x00} OFF {0x00 0x00 0x01} ON  {0x00 0x01 0x00} REPLY

static const struct _para_arr XTX_arr[]=
{
	{0x00}, //Disable
	{0x01}, //Enable
};


static const struct _para_arr BAUD_arr[]=
{
	{0x02}, //1200 BPS
	{0x03}, //2400
	{0x04}, //4800
	{0x05}, //9600
	{0x06}, //19200
	{0x07}, //38400
	{0x08}, //57600
	{0x09}, // 115200
};
static const struct _para_arr SHK_arr[]=
{
	{0x00,0x00,0x00}, // None
	{0x00,0x00,0x01}, // ACK/NAK
	{0x00,0x01,0x00}, // Xon/Xoff
};

static const struct _cmd_option2_minor USB[]=
{
	{0,1,&XTX_arr},
	{1,3,&SHK_arr},
	{2,1,&BAUD_arr},
	{0,0,NULL},
};

static const struct _cmd_option2_minor RS232[]=
{
	{0,1,&XTX_arr},
	{1,3,&SHK_arr},
	{2,1,&BAUD_arr},
	{0,0,NULL},
};

static const struct _cmd_option1_major Action[]=
{
	{0x80,&RS232},
	{0x81,NULL},
	{0x82,NULL},
	{0x83,&USB},
	{0,NULL},
};

int main(void)
{
	unsigned char cmd[]={0x7e,0x83,0x00,0x03,0x00,0x01,0x01,0x7e};
	struct _cmd_option2_minor *st_p_opt2_minor;
	struct _cmd_option1_major *opt1;
	int len = (cmd[5]<<8) |cmd[6];	
	
	
	printf("len : %d\n",len);
	printf("&baud_arr : %p, baud_arr : %02x\n",&BAUD_arr,BAUD_arr[1]);
	printf("&shk_arr : %p, shk_arr : %p\n",&SHK_arr,SHK_arr[1]);
	printf("&xtx_arr : %p, xtx_arr : %02x\n",&XTX_arr,XTX_arr[1]);
//	printf("&xtx_arr : %p , xtx_arr[1] : %p \n",&xtx_arr,&(xtx_arr[1]));
//	printf("&XTX_arr : %p ,XTX_arr : %p sizeof(XTX_arr) : %lu\n",&XTX_arr,XTX_arr,sizeof(XTX_arr));
	printf("\n");
//	printf("[0] : %02x ",baud_arr);
	return 0;	
}

int dispatch_cmd(unsigned char *cmd)
{
	int i;
	if (cmd[1]==)
}

