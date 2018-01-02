#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPE 1
#define Interface_rep NULL

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

static const struct _cmd_opcode0 new_cmd[]=
{
	{Interface_set},{Interface_get},{Interface_rep},
};

static const unsigned char opcodes[]=
{
	0x83,0xC3,0x04,
}; 

int main(void)
{
	struct _cmd_opcode0 Opcode,*p_opcode;
	struct _cmd_option1_major Op_major,*p_op_major;
	struct _cmd_option2_minor Op_minor,*p_op_minor;
	struct _param_arr Para_arr,*ptr;
	unsigned char *p=xtx_arr;
	void *pp;
	int *a;



/*
	printf("sizeof(Op_code) : %d sizeof(_cmd_opcode0 *ptr) : %d\n",(int)sizeof(Opcode),(int)sizeof(p_opcode));
	printf("sizeof(Op_minor) : %d sizeof(_cmd_option2_minor *ptr) : %d\n",(int)sizeof(Op_minor),(int)sizeof(p_op_major));
	printf("sizeof(Op_major) : %d sizeof(_cmd_option1_major *ptr) : %d\n",(int)sizeof(Op_major),(int)sizeof(p_op_major));
	printf("sizeof(Para_arr) : %d sizeof(_param_arr *ptr) : %d\n",(int)sizeof(Para_arr),(int)sizeof(ptr));
	printf("&Opcode : %p &Opcode.option1 : %p\n",&Opcode,&(Opcode.option1));
	printf("&Op_major : %p &Op_major.id : %p &Op_major.option2 :%p\n",&Op_major,&(Op_major.id),&(Op_major.option2));
	printf("&Op_minor : %p  &Op_minor.id : %p &Op_minor.type: %p &Op_minor.num : %p &Op_minor.arr :%p\n",&Op_minor,&(Op_minor.id),&(Op_minor.type),&(Op_minor.num),&(Op_minor.arr));
	printf("&Para_arr : %p &Parr_arr.arr : %p Para_arr.arr: %p\n",&Para_arr,&(Para_arr.arr),Para_arr.arr);
	printf("&xtx_arr : %p &p : %p p: %p *p : %02x p[1] %02x\n",&xtx_arr,&p,p,*p,p[1]);

	printf("sizeof(p) : %d\n",(int)sizeof(p));
	printf("sizeof(void *pp) : %d\n",(int)sizeof(pp));
	printf("sizeof(xtx_arr_off) : %d\n",(int)sizeof(xtx_arr_off));

	printf("sizeof(int n) : %d\n",(int)sizeof(n));
	printf("sizeof( Op_major.option2 ) : %d\n",(int)sizeof(Op_major.option2));
	printf("sizeof( Op_major.id ) : %d\n",(int)sizeof(Op_major.id));
	printf("sizeof( Op_minor.id ) : %d\n",(int)sizeof(Op_minor.id));
	printf("&ptr : %p *ptr : %p ptr->arr : %p Para_arr.arr : %02x",&ptr,*ptr,ptr->arr,Para_arr.arr);
*/	
	printf("\n");
	return 0;
}
