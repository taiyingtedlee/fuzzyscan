#include <stdio.h>
#include <stdlib.h>
#include <string.h>
<<<<<<< HEAD
#include "./include/rm_sp.h"
#include "./include/h2d.h"

#define SIMPLE	 0
#define COMPOUND 2 
#define TYPE 	1
#define SIZE 256

int dispatch_cmd(unsigned char *cmd);

struct _para_arr
{
	void *arr;
=======
#define SIZE 10
#define SIMPLE	 0
#define COMPOUND 2 
#define TYPE 	1

struct _para_arr
{
	unsigned char arr[SIZE];
>>>>>>> 0e3938ec6beca35d98874807f3e2a79838465a33
};

struct _cmd_option2_minor
{
<<<<<<< HEAD
	unsigned char id;
	unsigned short size;
	const struct _para_arr *para_arr;
};

struct _cmd_option1_major
{
	unsigned char id;
	const struct _cmd_option2_minor *option_1;
};

static const struct _para_arr XTX_arr[]=
{
	{0x00},
	{0x01},
	{NULL},
};
/*
static const struct _para_arr BAUD_arr[]=
{
	{0x02},
	{0x03},
	{0x04},
	{0x05},
	{0x06},
	{0x07},
	{0x08},
	{0x09},
};
*/

static const struct _para_arr SHK_arr[]=
{
	{0x00},
	{0x01},
	{0x02},
	{NULL},
};
	
static const struct _cmd_option2_minor RS232[]=
{
	{0,1,&XTX_arr},	
	{1,3,&SHK_arr},	
//	{2,1,&BAUD_arr},	
};

static const struct _cmd_option2_minor USB[]=
{
	{0,1,&XTX_arr},	
	{1,3,&SHK_arr},	
//	{2,1,&BAUD_arr},	
=======
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
>>>>>>> 0e3938ec6beca35d98874807f3e2a79838465a33
};

static const struct _cmd_option1_major Action[]=
{
<<<<<<< HEAD
	{0,&RS232},
	{1,&USB},
	{0,NULL},
};


int main(void)
{
	unsigned char str[SIZE]={'\0'};
	unsigned char cmd[SIZE]={'\0'};
	unsigned char *p_str,*p_cmd;
	int len,i,sp=0,cmd_len;

	printf("Input : ");
	fgets(str,sizeof(str),stdin);

	len=(int)strlen(str);
	
	p_str=sp_filter(str);
	//space number
	sp=p_str[len-1];
	p_cmd=h2d(p_str);
	cmd_len=(len-sp)/2;
	// parse p_cmd;
	for (i=0;i<cmd_len;i++){
		cmd[i]=p_cmd[i];
		printf("%d ",p_cmd[i]);
	}
	printf("\n");
	// show cmd;
	for(i=0;i<cmd_len;i++){
		printf("%02x ",cmd[i]);
	}
	printf("\n");

	dispatch_cmd(cmd);	

	printf("\n");
=======
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
>>>>>>> 0e3938ec6beca35d98874807f3e2a79838465a33
	return 0;	
}

int dispatch_cmd(unsigned char *cmd)
{
<<<<<<< HEAD
	struct _cmd_option2_minor *op2;
	struct _cmd_option1_major *opt1;
	int i, ret, send_len=0, len=((cmd[5]<<8)|cmd[6])+1;

	unsigned char cmd_id;
//	unsigned char *buf= ccd_catprue_buffer_2+4,*end;
	unsigned char *buf;
	unsigned chksum= cmd[1]^cmd[2]^cmd[3]^cmd[4]^cmd[5]^cmd[6];


	printf("length : %d\n",len);
	printf("chksum : %02x\n",chksum);
	return 0;
}
=======
	int i;
	if (cmd[1]==)
}

>>>>>>> 0e3938ec6beca35d98874807f3e2a79838465a33
