#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
};

struct _cmd_option2_minor
{
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
};

static const struct _cmd_option1_major Action[]=
{
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
	return 0;	
}

int dispatch_cmd(unsigned char *cmd)
{
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
