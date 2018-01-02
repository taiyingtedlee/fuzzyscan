#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/sp_filter.h"
#include "./include/h2d.h"

#define SIZE 256
#define TYPE 1
#define Interface_rep NULL
#define CMD_MASK 0x3F

void input(void);
int dispatch_cmd(unsigned char *cmd);

struct _param_arr
{
	void *arr;
};

struct _cmd_option2_minor
{
	unsigned char id;
	unsigned char type;
	unsigned short num;
	const struct _param_arr *arr;
};

struct _cmd_option1_major
{
	unsigned char id;
	const struct _cmd_option2_minor *option2;
};

static const struct _cmd_opcode0
{
	const struct _cmd_option1_major *option1;
};

static unsigned char xtx_arr[]={0x00,0x01};
static const struct _param_arr XTX_arr[]={{&xtx_arr}};

static unsigned char shk_arr_off[]={0x00,0x00,0x00};
static unsigned char shk_arr_on[]={0x00,0x00,0x01};
static unsigned char shk_arr_nak[]={0x00,0x01,0x00};
static const struct _param_arr SHK_arr[]={{&shk_arr_off},{&shk_arr_on},{&shk_arr_nak}};

static unsigned char baud_arr[]={0x02,0x03,0x04};
static const struct _param_arr BRT_arr[]={&baud_arr};

static const struct _cmd_option2_minor USB[]=
{
	{0,TYPE,1,(void *)XTX_arr},
	{1,TYPE,3,(void *)SHK_arr},
	{2,TYPE,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor RS232[]=
{
	{0,TYPE,1,(void *)XTX_arr},
	{1,TYPE,3,(void *)SHK_arr},
	{2,TYPE,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor Interface[]=
{
	{0,TYPE,1,(void *)RS232},
	{1,TYPE,1,NULL},
	{2,TYPE,1,NULL},
	{3,TYPE,1,(void *)USB},
	{0,0,0,NULL},
};

static const struct _cmd_option1_major Interface_set[]=
{
	{0,Interface},
	{0,NULL},
};

static const struct _cmd_option1_major Interface_get[]=
{
	{0,Interface},
	{0,NULL},
};

#define Action NULL
#define Event NULL

static const struct _cmd_opcode0 new_cmd[]=
{	{Action},
	{Event},
	{Interface_set},{Interface_get},{Interface_rep},
};


static const unsigned char opcodes[]=
{
	0x80,
	0x01,
	0x82,0xC3,0x04,
}; 



int main(void)
{	
	unsigned char str[SIZE]={'\0'};
	unsigned char cmd[SIZE]={'\0'};
	unsigned char *p_str,*p_cmd;
	int i,len,sp=0,cmd_len;
	
	printf("Input : ");
	fgets(str,sizeof(str),stdin);
	len=(int)strlen(str);

	p_str=sp_filter(str);
	sp=p_str[len-1];
	p_cmd=h2d(p_str);
	cmd_len=(len-sp)/2;

	// save p_cmd[] to cmd[] and show cmd
	for(i=0;i<cmd_len;i++){
		cmd[i]=p_cmd[i];
		printf("%02x ",cmd[i]);
	}

	dispatch_cmd(cmd);

	printf("\n");
	return 0;
}

int dispatch_cmd(unsigned char *cmd)
{
	/*
	** NOTE : cmd[0] = 7e cmd[1]=opcode0
	*/
	struct _cmd_option2_minor *op2_minor;
	struct _cmd_option1_major *op1_major,*major_macro;
	int i, ret, send_len=0, len=((cmd[5]<<8)|cmd[6])+1;
	printf("\n----------------dispatch_cmd---------------------\n");
	unsigned char cmd_id;
	unsigned char *ccd;
// 	unsigned char *buf = ccd_capture_buffer_2+4,*end;
	unsigned char *buf,*end;
	unsigned char chksum=cmd[1]^cmd[2]^cmd[3]^cmd[4]^cmd[5]^cmd[6]; // opcode to length

//  NOTE : if Didn't declare *buf initial value, segmentation fault!!! 

	printf("&buf : %p &ccd : %p \n",&buf,&ccd);
	printf("buf : %p ccd : %p \n",buf,ccd);
	buf[0]=cmd[2];
	buf[1]=cmd[3];
	buf[2]=cmd[5];
	buf[3]=cmd[6];
	end=buf+4+len-1;
	printf("&end : %p end : %p\n",&end,end);
	cmd_id = (cmd[0] & CMD_MASK);
/*	
	if((cmd_id<(sizeof(new_cmd)/sizeof(new_cmd[1]))) && (cmd[0] == opcodes[cmd_id]) && new_cmd[cmd_id].option1){
		op1_major = major_macro = new_cmd[cmd_id].option1;
		printf("&op1_major : %p op1_major.id : %d\n",&op1_major,op1_major->id);
	}
*/	
//	printf("sizeof(new_cmd) : %d sizeof(new_cmd[cmd_id]) : %d\n",(int)sizeof(new_cmd),(int)sizeof(new_cmd[cmd_id]));


	return 0;
}
