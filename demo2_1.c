#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/h2d.h"
#include "./include/sp_filter.h"


#define SIZE 2048
#define TYPE_ADDR 0
#define TYPE_MACRO 5
#define TYPE_FUNCTION 6
#define Interface_rep NULL
#define CMD_MASK 0x3F

#define SERCMD_CINO_USE (1<<7)
#define SERCMD_NO_ACKNAK 0x02

static unsigned char ccd[SIZE]={'\0'}; 
static unsigned char saved_setting[SIZE]={'\0'};
static int saved_len=0;

void input(void);
int dispatch_cmd(unsigned char *cmd);
void hal_putchar(int ch);
void send_ack_packet(unsigned char status);
void send_nak_packet(unsigned char status,unsigned char err);

static inline int trans_ch(unsigned char ch,unsigned char *p)
{
	int n=0;
	*p=ch;

	if (ch == 0x7e){ // '~'
		n=0;
	}else if(ch == 0x5c){ // '\\'
		n=1;
	}else if(ch == 0x06){ // 'ACK'
		n=2;
	}else if(ch == 0x15){ // 'NAK'
		n=3;
	}else if(ch == 0x11){ // 'XON'
		n=4;
	}else if(ch == 0x13){ // 'XOFF'
		n=5;
	}else{
		n=-1;
	}

	if(n>=0){
		*(p+0)= 0x5c; // '\\'
		*(p+1)=n;
		return 2;
	}
	return 1;
}
static inline void hal_trans_putchar(int ch)
{
	unsigned char trans[2];
	int n = trans_ch((unsigned char)ch,trans);

	hal_putchar(trans[0]);
	if (n==2)
	{
		hal_putchar(trans[1]);
	}
}

typedef int (*fn_act_ctrl)(unsigned char *opt);
typedef void (*fn_opt)(unsigned char *opt);

static int led_ctrl(unsigned char *opt);
static int beep_ctrl(unsigned char *opt);
static int scan_ctrl(unsigned char *opt);
static int store_ctrl(unsigned char *opt);

static void xtx_opt(unsigned char *opt);
static void shk_opt(unsigned char *opt);
static void baud_opt(unsigned char *opt);

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

struct _cmd_opcode0
{
	const struct _cmd_option1_major *option1;
};

static const struct _param_arr LED_arr[]={&led_ctrl}; 
static const struct _param_arr BEEP_arr[]={&beep_ctrl};
static const struct _param_arr SCAN_arr[]={&scan_ctrl};
static const struct _param_arr STORE_arr[]={&store_ctrl};
static const struct _param_arr XTX_arr[]={&xtx_opt};
static const struct _param_arr SHK_arr[]={&shk_opt};
static const struct _param_arr BRT_arr[]={&baud_opt};

static const struct _cmd_option2_minor RS232_minor[]=
{
	{0,TYPE_ADDR,1,(void *)XTX_arr},
	{1,TYPE_ADDR,3,(void *)SHK_arr},
	{2,TYPE_ADDR,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor WIFI_minor[]=
{
	{0,TYPE_ADDR,1,(void *)XTX_arr},
	{1,TYPE_ADDR,3,(void *)SHK_arr},
	{2,TYPE_ADDR,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor BT_minor[]=
{
	{0,TYPE_ADDR,1,(void *)XTX_arr},
	{1,TYPE_ADDR,3,(void *)SHK_arr},
	{2,TYPE_ADDR,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor USB_minor[]=
{
	{0,TYPE_ADDR,1,(void *)XTX_arr},
	{1,TYPE_ADDR,3,(void *)SHK_arr},
	{2,TYPE_ADDR,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option1_major RS232[]=
{
	{0,RS232_minor},
	{0,NULL}
};

static const struct _cmd_option1_major WIFI[]=
{
	{0,WIFI_minor},
	{0,NULL}
};

static const struct _cmd_option1_major BT[]=
{
	{0,BT_minor},
	{0,NULL}
};

static const struct _cmd_option1_major USB[]=
{
	{0,USB_minor},
	{0,NULL}
};

static const struct _cmd_option2_minor Interface[]=
{
	{0,TYPE_MACRO,1,(void *)RS232},
	{1,TYPE_MACRO,1,(void *)WIFI},
	{2,TYPE_MACRO,1,(void *)BT},
	{3,TYPE_MACRO,1,(void *)USB},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor LED[]=
{
	{0,TYPE_ADDR,2,(void *)LED_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor BEEP[]=
{
	{0,TYPE_ADDR,64,(void *)BEEP_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor SCAN[]=
{
	{0,TYPE_ADDR,1,(void *)SCAN_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor STORE[]=
{
	{0,TYPE_ADDR,0,(void *)STORE_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option1_major Action[]=
{
	{0,LED},
	{1,BEEP},
	{2,SCAN},
	{3,STORE},
	{0,NULL},
};

static const struct  _cmd_option1_major Interface_set[]=
{
	{0,Interface},
	{0,NULL},
};

static const struct _cmd_option1_major Interface_get[]=
{
	{0,Interface},
	{0,NULL},
};
#define Event NULL
#define Interface_rep NULL

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
//	unsigned char cmd[]={0x7e,0x80,0x00,0x00,0x00,0x00,0x02,0x02,0x01,0x7e};
	unsigned char *p_str,*p_cmd;
	int i,len,sp=0,cmd_len;
	while(1){		
	printf("Input : ");
	fgets(str,sizeof(str),stdin);
	len=(int)strlen(str);
	printf("\n----------------parse_cmd---------------------\n");
	//remove spaces
	p_str=sp_filter(str);
	sp=p_str[len-1];
	// str to dec
	p_cmd=h2d(p_str);
	cmd_len=(len-sp)/2;

	// save p_cmd[] to cmd[] and show cmd
	for(i=0;i<cmd_len;i++){
		cmd[i]=p_cmd[i];
		printf("%02x ",cmd[i]);
	}
	
	dispatch_cmd(cmd);

	printf("\n");
	}
	return 0;
}

int dispatch_cmd(unsigned char *cmd)
{
	/*
	** NOTE : cmd[0] = 7e cmd[1]=opcode0
	*/
	const struct _cmd_option2_minor *op2_minor;
	const struct _cmd_option1_major *op1_major,*op1_macro;
	int i, ret,err,send_len=0, len=((cmd[5]<<8)|cmd[6])+1;
	int opcode1_count=0,opcode2_count=0,opcode3_count=0;
	printf("\n----------------dispatch_cmd---------------------\n");
	unsigned char cmd_id;
//	unsigned char ccd[SIZE]={'\0'};
// 	unsigned char *buf = ccd_capture_buffer_2+4,*end;
	unsigned char *buf=ccd+4,*end;
	unsigned char chksum=cmd[1]^cmd[2]^cmd[3]^cmd[4]^cmd[5]^cmd[6]; // opcode to length
	unsigned char result_buf1[2048];

	unsigned char *buf_tmp,*cmd_tmp=&cmd[7];

	/* NOTE : if *buf initial value is not declared, segmentation fault!!! ?!
	** if, Opcode 82h 00h 03h  cmd[1]: 82h 
	*/
	for(i=0;i<len-1;i++){
		buf[i]=cmd_tmp[i];
	}

	buf_tmp=buf;
	buf=ccd;
	// saved cmd[2] : 00h cmd[3] : 03h ; Length cmd[5] High byte cmd[6] Low byte
	buf[0]=cmd[2];
	buf[1]=cmd[3];
	buf[2]=cmd[5];
	buf[3]=cmd[6];
	/*
	** +4 : buf[0] buf[1] buf[2] buf[3]  ; +(len-1) : Length == Parameters Bytes 
	*/
	end=buf+4+len-1;
	/*
	** define CMD_MASK 0x3F ; cmd_id is for static const struct _cmd_opcode new_cmd[]={......};
	*/
	cmd_id = (cmd[1] & CMD_MASK);
	/*
	** if cmd_id is in new_cmd[]={......} && Opcode[0](cmd[1]) is match opcodes[cmd_id] && 
	*/
	
	if((cmd_id<(sizeof(new_cmd)/sizeof(new_cmd[0]))) && (cmd[1] == opcodes[cmd_id]) && new_cmd[cmd_id].option1){
//	printf("sizeof(new_cmd) : %d sizeof(new_cmd[cmd_id]) : %d\n",(int)sizeof(new_cmd),(int)sizeof(new_cmd[cmd_id]));

		/* assignment discards 'const' qualifier from pointer target type ?! 
		** ANS: struct _cmd_option1_major *op1_major, *op1_macro 
				const struct _cmd_option1_major *op1_major, *op1_macro 
		*/	
		opcode1_count++;

		op1_major = op1_macro = new_cmd[cmd_id].option1;
//		printf("&new_cmd[cmd_id].option1 : %p\n",&(new_cmd[cmd_id].option1)); 
//		printf("new_cmd[cmd_id].option1->id : %d\n",new_cmd[cmd_id].option1->id);
		/* 
			 NOTE : In powertool.c cmd[]{0x82,0x00,0x03,0x00,0x00,0x01,0x7e};

								    buf[0]   buf[1]         buf[2] buf[3]
					cmd[0] cmd[1]   cmd[2]   cmd[3]  cmd[4] cmd[5] cmd[6] cmd[7]
		**   cmd[]={0x7e,  0x82,    0x00,    0x03,   0x00,  0x00,  0x01,  0x7e};
						   opcode0  opcode1  opcode2
						   opcode 	major	 minor
		*/
check_macro:
		while(op1_major->option2 !=NULL){  // check if option2 exists,e.g., parameter table exists 
			if(op1_major->id == buf[0]){   // check if id == buf[0], whereas, cmd[2]
				opcode2_count++;
				op2_minor = op1_major->option2; // Assign op1_major->option2 to opinter op2_minor 
				
				while(op2_minor->arr!=0){      // NOTE: check if arr exists,e.g., parameter table options exists 
					if (op2_minor->id == buf[1]){ // check if minor id == buf[1], whereas, cmd[3] 
						// CHECK TYPE ,whereas, #define TYPE_MACRO 5 
						opcode3_count++;
						if(op2_minor->type == TYPE_MACRO){  				
							buf += 4; // buf +4  !!!!!! address shift 4 
							// check if buf(addrs) exceeds end(addrs),e.g., CHECK PARAMETERS BYTES
							if(buf >= end){  // if buf buf exceeds end,?!
								err=7;       
								goto err_nak; // goto err_nak; 
							}
							op1_major = op1_macro = (struct _cmd_option1_major *)(op2_minor->arr);
							
							goto check_macro; // check macro again
						}	
						else{ // ELSE CHECK NO TYPE
							int l = (buf[2]<<8 | buf[3]); // check length again, using buf[2] buf[3] 
							if(cmd[1] == opcodes[0]){ // Action 0x80 ; Action Simple cmd, length == num param bytes, 
								if(l != op2_minor->num){ // if length does not match num of param bytes,
									err=8; 	
									goto err_nak;
								}
								if((buf+4+l) != end) // buf+4+l(addrs) does not match end(addrs), more or less than end.
								{
									err=9;
									goto err_nak;
								}
							}
							else{ // ?! cmd[1] & 0b 0100 0000 check get cmd; 0x4,5,6,7,C,D,E,F
								if ((cmd[1]& 0x40) && l){ // if so,true, && l 
									err=10;
									goto err_nak;
								}
								else if (!(cmd[1] & 0x40) && (l !=op2_minor->num)){ // if not,false, && l != num;
									err=11;
									goto err_nak;
								}
							}
							// ADD LENGTH, expect PARAMETER BYTES
							buf += 4+l; // buf +4+l ; 
							if(buf>end){ // if buf exceeds end, error
								err=12;
								goto err_nak;
							}
							else if(buf == end){ // IF buf == end, whereas, buf += 4+l;
								goto do_command; // do_command
							}
							op1_major = op1_macro;  //?! buf < end, CHECK COMPOUND PARAMETERS
							goto check_macro;

						}
					}
					// if(op2_minor->id == buf[1]){}
					// else op2_minor->id != buf[1]
					op2_minor ++; //?!  struct _cmd_option2_minor opinter shift 


				}
				// while (op2_minor->arr !=0) {} 
				// else op2_minor->arr ==0
				err =13;
				goto err_nak;
			}
			// if(op1_major->id ==buf[0]){}
			// else op1_major->id != buf[0]
			op1_major ++; // ?! struct _cmd_option1_major  pointer shift 1 unit (bytes)
		}
		// while(op1_major->option2 !=NULL)
		// else op1_major->option2 ==NULL 
		err=14;
		goto err_nak;
	}
	//	if((cmd_id<(sizeof(new_cmd)/sizeof(new_cmd[0]))) && (cmd[1] == opcodes[cmd_id]) && new_cmd[cmd_id].option1){}
	//  else ......
	else{
		err=15;
		goto err_nak;
	}

do_command:
	buf = ccd; // buf = ccd_capture_buffer_2;
	op1_major = op1_macro = new_cmd[cmd_id].option1;
do_macro:
	while(op1_major->option2 !=NULL){ // if option2 exists, that is, parameter table exists
		if(op1_major->id == buf[0]){ //	if id == buf[0],that is, cmd[2] major opcode1
			op2_minor = op1_major->option2; // assign op1_major->option2 to struct _cmd_option2_minor pointer
			while(op2_minor->arr !=0){ // if arr exists, that is, parameter table option exists
				if(op2_minor->id == buf[1]) // if id == buf[1],that is, cmd[3] minor opcode2
				{
					if(op2_minor->type == TYPE_MACRO) { // Check TYPE_MACRO,that is, compound cmd
						buf +=4;

						op1_major = op1_macro = (struct _cmd_option1_major*)(op2_minor->arr); 
						goto do_macro;
					}
					else{ //Check NOT TYPE_MACRO, that is, simple cmd
						int e, l= (buf[2]<<8 | buf[3]);
						if(cmd[1]==opcodes[0] || cmd[1] == 0x90 || cmd[1] == 0x91){ //Action :0x80 HTD_NAK :0x90 HTD_ACK :0x91
//exec_action()  *func_ptr to func, {0,type,2 ,(void *)sys_led_ctrl} ; static in sys_led_ctrl(unsigned char *p, int len,unsigned char flags)//
							// exec_action() is replaced by ((fn_act_ctrl)op2_minor->arr->arr)(buf_tmp)
							for(i=0;i<op2_minor->num;i++){	
								buf_tmp[i]=*(buf+4+i);
							}
							e=((fn_act_ctrl)op2_minor->arr->arr)(buf_tmp);
							if(e < 0 ){ // LOOKUP func e=exec_action <0 ; false 
								err = 16;
								goto err_nak;
							}
							if((e == 0)) {// LOOKUP func e=exec_action = 0 ; true if(e==0)
								goto no_ack;
							}
							goto do_finish;
						}
						else{
							if(cmd[1]& 0x40){ // 0x4,5,6,7,C,D,E,F ; if cmd[1] is GET cmd
								int rn;
//??? send_bytes() ???//
//								rn = send_bytes(op1_major,op2_minor, result_buf1, send_len); 
								
								if(rn<0){  // send_bytes <0
									err=21;
									goto err_nak;
								}
								if(op2_minor->type == TYPE_FUNCTION){ //  #define TYPE_FUNCTION 6
									rn=4; // !!!!!!!!! Must update : send_bytes() !!!!!!!!! 
									send_len +=rn;
								}
								else{
									send_len += 4 + op2_minor->num;
								}
							}
							else{ // if cmd[1] is NOT GET cmd ; 0x4,5,6,7,C,D,E,F 
// ??? eTriggerMode	???//		
								/*
								** void (*fn_opt)(unsigned char *);
								   fn_opt=op2_minor->arr->arr;
								   
								   ((fn_opt)(buf_tmp));
								   
								  //  Using "typedef " is for better understanding of which function is pointed to by a func ptr;
								   
								   typedef void (*fn_opt)(unsigned char *opt).
								   
								   ((fn_opt)op2_minor->arr->arr)(buf_tmp);
								*/

								// saved setting is  tmp_buf for get cmd simulation purpose only!!! trial ver.

								for(i=0;i<op2_minor->num;i++){	
									buf_tmp[i]=*(buf+4+i);
								}
								// typedef void (*fn_opt)(unsigned char *opt);							
								((fn_opt)op2_minor->arr->arr)(buf_tmp);

								/*
								   #define eTriggerMode ?
								   #define PARAM_ADDR(a) (parameter_base + (unsigned long)(&a))
								** #define PARAM_VAL(a) (*(PARAM_ADDR(a)))
								   extern unsigned long get_trigger_mode_mask(void); 

								*/
								
								/*
								unsigned long trigger_mode_mask = get_trigger_mode_mask();//?!LOOKUP fn get_trigger_mode_mask()
								unsigned char mode= PARAM_VAL(eTriggerMode); // ?! LOOKUP PARAM_VAL(eTriggerMode)

								if(recv_bytes(op1_major,op2_minor,&buf[4]) <0){ // ?! LOOKUP recv_bytes()
									err=17;
									goto err_nak;
								}
								
								if((trigger_mode_mask & (1<<PARAM_VAL(eTriggerMode))) == 0){ // ?! LOOKUP trigger_mode_mask
									write_param_to_flash(PARAM_ADDR(eTriggerMode),mode,USER_TEMP_PARA_ID); // ?! LOOKUP 
									err=18;
									goto err_nak;
								}

								if(mode != PARAM_VAL(eTriggerMode)){ // ?! LOOKUP fn extern void opt_mode_init(void) 
									extern void opt_mode_init(void);
									opt_mode_init();
								}
								*/
							}

						}
						buf +=4+l;
						if(buf == end){
							goto do_finish;
						}
						op1_major = op1_macro;
						goto do_macro;
					} 
				}
				// if(op2_minor->id == buf[1])
				// else op2_minor->id != buf[1]
				op2_minor++; // ?! struct _cmd_option2_minor opinter shift 1 unit(?! bytes) 
			}
			// while(op2_minor->arr !=0){}
			// else op2_minor->arr ==0
			err=21;		
			goto err_nak;
		}
		// if(op1_major->id == buf[0]){}
		// else op1_major->id != buf[0]
		op1_major++; // ?! struct _cmd_option1_major opinter shift 1 unit(?! bytes)

	}
	// while(op1_major->option2 !=NULL)
	// else op1_major->option2 ==NULL
	err=22;
	goto err_nak;

do_finish:	
	if(cmd[1] & 0x40){ // 0x4,5,6,7,C,D,E,F ; if cmd[1] is GET cmd
		unsigned char tmp;
		chksum =0;
		buf = saved_setting;
		//buf= result_buf1;
	//	extern unsigned char is_bt_connect_with_cradle(void);
//??? hal_putchar ??? //
//??? hal_trans_putchar ???//
		hal_putchar('~'); // ?! LOOKUP func hal_putchar();
		hal_trans_putchar(tmp= opcodes[(saved_setting[1] & CMD_MASK)+1]); // ?! LOOKUP func hal_trans_putchar()
		chksum ^=tmp;
		hal_trans_putchar(tmp= saved_setting[2]);
		chksum ^=tmp;
		hal_trans_putchar(tmp= saved_setting[3]);
		chksum ^=tmp;
		// if not a "internal serial cmd" or "N2c serial cmd"; SERCMD_CINO_USE=(1<<7); SERCMD_INTERNAL =(1<<6);
		hal_trans_putchar(tmp= saved_setting[4] & SERCMD_CINO_USE); 
		chksum ^=tmp;
//??? send_len >> 8 & 0xff ???//
		hal_trans_putchar(tmp = ((send_len >> 8) & 0xff)); // ?!
		chksum ^=tmp;
//??? send_len >> 0 & 0xff ???// 		
		hal_trans_putchar(tmp= ((send_len >> 0) &0xff)); //?!
		chksum ^=tmp;	
		for(i=0; i<send_len; i++){
			hal_trans_putchar(tmp=saved_setting[7+i]);
			chksum^=tmp;
		}
		hal_trans_putchar(chksum);
		hal_putchar('~');
	}
	else { // if cmd[1] is NOT GET cmd
		// if not GET cmd, saved_setting!!!
		for (i=0;i<7+len;i++)
		{
			saved_setting[i]=cmd[i];
		}
		saved_len=i;
		int active =0;
		if ((cmd[4] & SERCMD_NO_ACKNAK) == 0){ // ?! SERCMD_NO_ACKNAK 0x02 ; Bit 2 ACK/NAK 0 = Request ; 1 = Do NOT Request
//			printf("line:%d send_ack_packet : 7e 0f 00 00 00 00 00 0f 7e\n",__LINE__);
			send_ack_packet(cmd[4] & SERCMD_CINO_USE); // send_ack_packet; 
		}
		if (cmd[1] == 0x82){ // Interface SET cmd			
			switch(cmd[3])  // ?! LOOKUP PARAM_VAL eOutputInterface switch(PARAM_VAL(eOutputInterface)) Modified!!!
			{
				case 0:  
					active =1;
					break;
				case 1:
					active=1;
					break;
				case 2:
					active =1;
					break;
				case 3:
					active =1;
					break;
				case 4:
					active =1;
					break;
			}
			if(active){
//				printf("line:%d Interface set active!!!\n",__LINE__);
				/*
					usleep(500*1000); // ?! LOOKUP 
				tx_init(); // ?! LOOKUP tx_init();
				*/
			}
		}

	}

no_ack:
	if(cmd[4]!=0x00){
		printf("line:%d if (hal_putchar == _hal_putchar)\n",__LINE__);
		printf("line:%d NO ACK!!!\n",__LINE__);
	}
/*
	if(hal_putchar == _hal_putchar) // ?! LOOKUP _hal_putchar
		hal_putchar(-1);
	hal_putchar = _hal_putchar;
*/
//??? ???//	
//	if (lock) pthread_mutex_unlock(&send_event_mutex); //?! LOOKUP lock & pthread_mutex_unlock(&send_event_mutex)
	return 1;

err_nak:

//	nak_reason = err; //  LOOKUP nak_reason :  static unsigned char nak_reason; is used in static int get_nak_status (){};
	printf("line:%d ......err_nak......\n",__LINE__);
	if ((cmd[4] & SERCMD_NO_ACKNAK) == 0)		
		send_nak_packet(cmd[4] & SERCMD_CINO_USE, err); // ?! LOOKUP send_nak_packet()
//		printf("line:%d if (hal_putchar == _hal_putchar)\n",__LINE__);
/*
	if(hal_putchar == _hal_putchar)
		hal_putchar(-1);
	hal_putchar = _hal_putchar;
*/
//??? ???//
//	if (lock) pthread_mutex_unlock(&send_event_mutex);


	return 0;
}

void hal_putchar(int ch)
{
	printf("%02x",ch);
}

void send_ack_packet(unsigned char status)
{
	hal_putchar(0x7e);
	hal_putchar(0x0F);
	hal_putchar(0x00);
	hal_putchar(0x00);
	hal_putchar(status);
	hal_putchar(0x00);
	hal_putchar(0x00);
	hal_putchar(0x0F ^ status);
	hal_putchar(0x7e);
}

void send_nak_packet(unsigned char status,unsigned char err)
{
	hal_putchar(0x7e);
	hal_putchar(0x0E);
	hal_putchar(err);
	hal_putchar(status);
	hal_putchar(0x00);
	hal_putchar(0x00);
	hal_putchar(0x0E ^ err ^ status);
	hal_putchar(0x7e);
}
int led_ctrl(unsigned char *opt)
{
	int ret=1;
	switch(opt[0])
	{
		case 0x01: 
			printf("RLED ");
			if(opt[1] == 0x00)
				printf("OFF!\n");
			else if(opt[1] == 0x01)
				printf("ON!\n");
			else if(opt[1] == 0x02)
				printf("Restore to system control!\n");
			else{
				printf("Error!\n");
				ret=-1;
			}
			break;
		case 0x02: 
			printf("GLED ");
			if(opt[1] == 0x00)
				printf("OFF!\n");
			else if(opt[1] == 0x01)
				printf("ON!\n");
			else if(opt[1] == 0x02)
				printf("Restore to system control!\n");
			else{
				printf("Error!\n");
				ret=-1;
			}
			break;
		case 0x03: 
			printf("BLED ");
			if(opt[1] == 0x00)
				printf("OFF!\n");
			else if(opt[1] == 0x01)
				printf("ON!\n");
			else if(opt[1] == 0x02)
				printf("Restore to system control!\n");
			else{
				printf("Error!\n");
				ret=-1;
			}
			break;
		default:
			printf("Error!\n");
			ret=-1;
			break;
	}
	return ret;
}

int beep_ctrl(unsigned char *opt)
{
	int i;
	for (i=0;i<64;i++)
	{
		if(opt[i] == 0xff){
			printf("%d byte: 0xff\n",i);
			break;
		}
		if(opt[i] > 0x10)
		{
			printf("%d byte: greater than 10h",i);
			return -1;
		}
	}
	if ((i == 64) || (i == 0)){
		return -1;
	}
	return 1;	
}

int scan_ctrl(unsigned char *opt)
{	
	int ret=1;
	if(opt[0] == 0x00)
		printf("Serial Trigger : OFF!\n");
	else if (opt[0] == 0x01)
		printf("Serial Trigger : ON!\n");
	else{
		printf("Serial Trigger : Error!\n");
		ret = -1;
	}
	return ret;
}

int store_ctrl(unsigned char *opt)
{
	printf("Store Configuration!\n");
	return 1;	
}

void xtx_opt(unsigned char *opt)
{
	if(opt[0] == 0x00)
		printf("\nSTX/ETX Trans : Disable!\n");
	else if (opt[0] == 0x01) 
		printf("\nSTX/ETX Trans : Enable!\n");
	else
		printf("\nSTX/ETX Trans : Error! Check Paramter table!");
}

void shk_opt(unsigned char *opt)
{
	if(opt[0] == 0x00)
	{
		if (opt[1] == 0x00)
		{
			if (opt[2] == 0x00)
			{
				printf("\nHandshaking : None!\n");
			}else if (opt[2] == 0x01)
			{
				printf("\nHandshaking : ACK/NAK!\n");
			}else 
				printf("\nHandshaking : Error! Check Parameter table!\n");
		}else if(opt[1] == 0x01)
		{
			if (opt[2]== 0x00)
				printf("\nHandshaking : Xon/Xoff\n");
			else
				printf("\nHandshaking : Error! Check Parameter table!\n");
		}
	}else
		printf("\nHandshaking : Error! Check Parameter table!\n");
}

void baud_opt(unsigned char *opt)
{
	if (opt[0] !=0x00)
	{
		switch (opt[0])
		{
			case 0x02:
				printf("\nBaud Rate: 1200 BPS\n");
				break;
			case 0x03:
				printf("\nBaud Rate: 2400 BPS\n");
				break;
			case 0x04:
				printf("\nBaud Rate: 4800 BPS\n");
				break;
			case 0x05:
				printf("\nBaud Rate: 9600 BPS\n");
				break;
			case 0x06:
				printf("\nBaud Rate: 19200 BPS\n");
				break;
			case 0x07:
				printf("\nBaud Rate: 38400 BPS\n");
				break;
		}
	}else if (opt[0] == 0x00)
	{
		printf("\nBaud Rate : Error! Check Parameter table!\n");
	}

}
