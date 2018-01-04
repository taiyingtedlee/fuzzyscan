#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 256
#define TYPE_ADDR 0
#define TYPE_MACRO 5
#define TYPE_FUNCTION 6
#define Interface_rep NULL
#define CMD_MASK 0x3F

#define SERCMD_CINO_USE (1<<7)
#define SERCMD_NO_ACKNAK 0x02

void input(void);
int dispatch_cmd(unsigned char *cmd);
void hal_putchar(int ch);
void hal_trans_putchar(int ch);

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
	{0,TYPE_ADDR,1,(void *)XTX_arr},
	{1,TYPE_ADDR,3,(void *)SHK_arr},
	{2,TYPE_ADDR,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor RS232[]=
{
	{0,TYPE_ADDR,1,(void *)XTX_arr},
	{1,TYPE_ADDR,3,(void *)SHK_arr},
	{2,TYPE_ADDR,1,(void *)BRT_arr},
	{0,0,0,NULL},
};

static const struct _cmd_option2_minor Interface[]=
{
	{0,TYPE_MACRO,1,(void *)RS232},
	{1,TYPE_MACRO,1,NULL},
	{2,TYPE_MACRO,1,NULL},
	{3,TYPE_MACRO,1,(void *)USB},
	{0,0,0,NULL},
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
//	unsigned char cmd[SIZE]={'\0'};
	unsigned char cmd[]={0x7e,0x82,0x00,0x03,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x01,0x7e};
	unsigned char *p_str,*p_cmd;
	int i,len,sp=0,cmd_len;
	/*
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
	*/
	printf("cmd : ");
	len=(int)sizeof(cmd);
	for(i=0;i<len;i++){
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
	const struct _cmd_option2_minor *op2_minor;
	const struct _cmd_option1_major *op1_major,*op1_macro;
	int i, ret,err,send_len=0, len=((cmd[5]<<8)|cmd[6])+1;
	printf("\n----------------dispatch_cmd---------------------\n");
	unsigned char cmd_id;
	unsigned char ccd[SIZE]={'\0'};
// 	unsigned char *buf = ccd_capture_buffer_2+4,*end;
	unsigned char *buf,*end;
	unsigned char chksum=cmd[1]^cmd[2]^cmd[3]^cmd[4]^cmd[5]^cmd[6]; // opcode to length
	unsigned char result_buf1[2048];
	
	/* NOTE : if *buf initial value is not declared, segmentation fault!!! ?!
	** if, Opcode 82h 00h 03h  cmd[1]: 82h 
	*/
	buf=ccd;
	printf("line:%d &buf : %p &ccd : %p \n",__LINE__,&buf,&ccd);
	printf("line:%d  buf : %p  ccd : %p \n",__LINE__,buf,ccd);
	// saved cmd[2] : 00h cmd[3] : 03h ; Length cmd[5] High byte cmd[6] Low byte
	buf[0]=cmd[2];
	buf[1]=cmd[3];
	buf[2]=cmd[5];
	buf[3]=cmd[6];
	/*
	** +4 : buf[0] buf[1] buf[2] buf[3]  ; +(len-1) : Length == Parameters Bytes 
	*/
	end=buf+4+len-1;
	printf("line:%d &end : %p end : %p\n",__LINE__,&end,end);
	/*
	** define CMD_MASK 0x3F ; cmd_id is for static const struct _cmd_opcode new_cmd[]={......};
	*/
	cmd_id = (cmd[1] & CMD_MASK);
	printf("line:%d cmd_id : %d\n",__LINE__,cmd_id);
	/*
	** if cmd_id is in new_cmd[]={......} && Opcode[0](cmd[1]) is match opcodes[cmd_id] && 
	*/
	
	if((cmd_id<(sizeof(new_cmd)/sizeof(new_cmd[0]))) && (cmd[1] == opcodes[cmd_id]) && new_cmd[cmd_id].option1){
//	printf("sizeof(new_cmd) : %d sizeof(new_cmd[cmd_id]) : %d\n",(int)sizeof(new_cmd),(int)sizeof(new_cmd[cmd_id]));

		/* assignment discards 'const' qualifier from pointer target type ?! 
		** ANS: struct _cmd_option1_major *op1_major, *op1_macro 
				const struct _cmd_option1_major *op1_major, *op1_macro 
		*/	
		op1_major = op1_macro = new_cmd[cmd_id].option1;
		printf("line:%d opcodes[cmd_id] : %02x\n",__LINE__,opcodes[cmd_id]);
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
				op2_minor = op1_major->option2; // Assign op1_major->option2 to opinter op2_minor 
				printf("line:%d &op2_minor : %p\n",__LINE__,&op2_minor);
				printf("line:%d buf[0]: %02x\n",__LINE__,buf[0]);   
				printf("line:%d op1_major->id : %d\n",__LINE__,op1_major->id);
				while(op2_minor->arr!=0){      // NOTE: check if arr exists,e.g., parameter table options exists 
					printf("line:%d op_minor->arr != 0\n",__LINE__);
					if (op2_minor->id == buf[1]){ // check if minor id == buf[1], whereas, cmd[3] 
						// CHECK TYPE ,whereas, #define TYPE_MACRO 5 
						printf("line:%d op_minor->id == buf[1] :%02x\n",__LINE__,buf[1]);
						if(op2_minor->type == TYPE_MACRO){  
							buf += 4; // buf +4  
							printf("line:%d &buf +4 : %p\n",__LINE__,&buf);
							// check if buf(addrs) exceeds end(addrs),e.g., CHECK PARAMETERS BYTES
							if(buf >= end){  // if buf buf exceeds end,?!
								err=7;       
								goto err_nak; // goto err_nak; 
							}
// ??? different struct ptr type ???//							
							op1_major = op1_macro = (struct _cmd_option_major *)(op2_minor->arr);//?! different struct ptr type
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
					printf("line:%d op2_minor->id : %02x != buf[1] :%02x\n",__LINE__,op2_minor->id,buf[1]);
					printf("line:%d &op2_minor :%p\n",__LINE__,&op2_minor);
					printf("line:%d op2_minor->type : %d\n:",__LINE__,op2_minor->type);
					op2_minor ++; //?!  struct _cmd_option2_minor opinter shift 
					printf("line:%d &op2_minor++ :%p\n",__LINE__,&op2_minor);
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
		err=4;
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
//??? different struct ptr type ???//
						op1_major = op1_macro = (struct _cmd_option_major*)(op2_minor->arr); // ?! different struct pointer type
						goto do_macro;
					}
					else{ //Check TYPE_MACRO,that is, simple cmd
						int e, l= (buf[2]<<8 | buf[3]);

						if(cmd[1]==opcodes[0] || cmd[1] == 0x90 || cmd[1] == 0x91){ //Action :0x80 HTD_NAK :0x90 HTD_ACK :0x91
//exec_action()  *func_ptr to func, {0,type,2 ,(void *)sys_led_ctrl} ; static in sys_led_ctrl(unsigned char *p, int len,unsigned char flags)//
							if((buf[4]) <0 ){ // ?! LOOKUP func e=exec_action <0 ; false 
								printf("exec_action()<0\n");
								err = 16;
								goto err_nak;
							}
							if((cmd[4] & 0x02)) {// LOOKUP func e=exec_action = 0 ; true if(e==0)
								goto no_ack;
							}
							goto do_finish;
						}
						else{
							if(cmd[1]& 0x40){ // 0x4,5,6,7,C,D,E,F ; if cmd[1] is GET cmd
								printf("GET cmd !!!\n");
								int rn;
//??? send_bytes() ???//
//								rn = send_bytes(op1_major,op2_minor, result_buf1, send_len); // ?! LOOKUP func send_bytes() ret 0; ret -1; ret n
								
								if(rn<0){  // send_bytes <0
									err=21;
									goto err_nak;
								}
								if(op2_minor->type == TYPE_FUNCTION){ //  #define TYPE_FUNCTION 6
									rn=4; // !!!!!!!!! random number !!!!!!!!! 
									send_len +=rn;
								}
								else{
									send_len += 4 + op2_minor->num;
									printf("send_len: %d\n",send_len);
								}
							}
							else{ // if cmd[1] is NOT GET cmd ; 0x4,5,6,7,C,D,E,F 
								printf("NOT GET cmd!!!\n");
// ??? eTriggerMode	???//							
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
				op2_minor ++; // ?! struct _cmd_option2_minor opinter shift 1 unit(?! bytes) 
			}
			// while(op2_minor->arr !=0){}
			// else op2_minor->arr ==0
			err=21;		
			goto err_nak;
		}
		// if(op1_major->id == buf[0]){}
		// else op1_major->id != buf[0]
		op1_major ++; // ?! struct _cmd_option1_major opinter shift 1 unit(?! bytes)

	}
	// while(op1_major->option2 !=NULL)
	// else op1_major->option2 ==NULL
	err=22;
	goto err_nak;

do_finish:
	if(cmd[1] & 0x40){ // 0x4,5,6,7,C,D,E,F ; if cmd[1] is GET cmd
		unsigned char tmp;
		chksum =0;
		buf= result_buf1;
	//	extern unsigned char is_bt_connect_with_cradle(void);
//??? hal_putchar ??? //
//??? hal_trans_putchar ???//
		hal_putchar('~'); // ?! LOOKUP func hal_putchar();
		hal_trans_putchar(tmp=opcodes[(cmd[1] & CMD_MASK)+1]); // ?! LOOKUP func hal_trans_putchar()
		chksum ^=tmp;
		hal_trans_putchar(tmp= cmd[2]);
		chksum ^=tmp;
		hal_trans_putchar(tmp= cmd[3]);
		chksum ^=tmp;
		// if not a "internal serial cmd" or "N2c serial cmd"; SERCMD_CINO_USE=(1<<7); SERCMD_INTERNAL =(1<<6);
		hal_trans_putchar(tmp= cmd[4] & SERCMD_CINO_USE); 
		chksum ^=tmp;
//??? send_len >> 8 & 0xff ???//
		hal_trans_putchar(tmp = ((send_len >> 8) & 0xff)); // ?!
		chksum ^=tmp;
//??? send_len >> 0 & 0xff ???// 		
		hal_trans_putchar(tmp= ((send_len >> 0) &0xff)); //?!
		chksum ^=tmp;	
		for(i=0; i<send_len; i++){
			hal_trans_putchar(tmp=buf[i]);
			chksum^=tmp;
		}
		hal_trans_putchar(chksum);
		hal_putchar('~');
	}
	else { // if cmd[1] is NOT GET cmd
		int active =0;
		if ((cmd[4] & SERCMD_NO_ACKNAK) == 0){ // ?! SERCMD_NO_ACKNAK 0x02 ; Bit 2 ACK/NAK 0 = Request ; 1 = Do NOT Request
			printf("line:%d send_ack_packet : 7e 0f 00 00 00 00 00 0f 7e\n",__LINE__);
//			send_ack_packet(cmd[4] & SERCMD_CINO_USE); // send_ack_packet; 
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
				printf("line:%d Interface set active!!!\n",__LINE__);
				/*
					usleep(500*1000); // ?! LOOKUP 
				tx_init(); // ?! LOOKUP tx_init();
				*/
			}
		}

	}

no_ack:
	printf("line:%d if (hal_putchar == _hal_putchar)\n",__LINE__);
	printf("line:%d NO ACK!!!\n",__LINE__);
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

	if ((cmd[4] & SERCMD_NO_ACKNAK) == 0)
		printf("line:%d send_nak_packet : 7e 0e 00 %02x 00 00 00 0e 7e\n",__LINE__,err);
//		send_nak_packet(cmd[4] & SERCMD_CINO_USE, err); // ?! LOOKUP send_nak_packet()
		printf("line:%d if (hal_putchar == _hal_putchar)\n",__LINE__);
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
	unsigned char c=ch;
	printf("%02x",c);
}

void hal_trans_putchar(int ch)
{	
	unsigned char c=ch;
	printf("%02x",c);
}
