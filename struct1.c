#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPE 0
#define TYPE_MACRO 2

static unsigned char srlxtx_arr[]={0x00,0x01};
static unsigned char srlshk_arr[]={0x00,0x00,0x01};

struct _param_arr
{
	unsigned char *arr;
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
	const struct _cmd_option_major *option1;
};

static const struct _param_arr SRLXTX_arr= {srlxtx_arr};
static const struct _param_arr SRLHSK_arr= {srlshk_arr};

static const struct _cmd_option2_minor USBCOM_minor[]=
{
	{0,TYPE,1,(void *)SRLXTX_arr},
	{0,TYPE,3,(void *)SRLHSK_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option1_major USBCOM_macro[] =
{
	{0,USBCOM_minor},
	{0,NULL},
};

static const struct _cmd_option2_minor Interface_major[]=
{
	{0,TYPE_MACRO,1,(void*)USBCOM_macro},
//	{1,TYPE,1,(void*)PS232_macro},
	{0,0,0,NULL},
};

static const struct _cmd_option1_major Itf_set[]=
{
	{0,Interface_major},
	{0,NULL},
};

static const struct _cmd_option1_major Itf_get[]=
{
	{0,Interface_major},
	{0,NULL}
};

static const struct _cmd_opcode0 new_cmd[]=
{
	// Adding new fn here
//	{Action_cmd},
	{Itf_set},{Itf_get},
};


int main(void)
{
	printf("para options : %02x ",USBCOM_macro[0]);
	printf("\n");
	return 0;
}
