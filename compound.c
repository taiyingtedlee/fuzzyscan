#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_1_MASK 0x3F

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
	const struct _cmd_option2_minor *option_2;
};

struct _cmd_opcode
{
	unsigned char _cmd_option1_major *option_1;
};

int main(void)
{
	
	return 0;
}

int dispatch_cmd(unsigned char *cmd)	
{
	const struct _cmd_option1_major *opt1_major_opt1, *opt1_major_macro;
	const struct _cmd_option2_minor *opt2_minor_opt2;

	int i,ret,send_len=0,len=((cmd[4]<<8) |cmd[5])+1;
	unsigned char cmd_id;
	
	unsigned char *buf,*end; // *buf = ccd_capture_buffer_2 + 4 ; where as , unsigned char *ccd_capture_buffer_2;

	unsigned char result_buf1[2048];
	int lock =0;
	
//	buf=ccd_capture_buffer_2; 
	
	buf[0]=cmd[1];
	buf[1]=cmd[2];	
	buf[2]=cmd[4];
	buf[3]=cmd[5];
	end= buf+4+len-1; // end of memory address 
	// USBCOM SET 82h 00h 03h   cmd[0]=82h cmd[1]=00h cmd[2]=03h
	cmd_id=(cmd_id[0] & CMD_1_MASK);
	if ((cmd_id<(sizeof(new_cmd)/sizeof(new_cmd[0]))) && (cmd[0]==opcodes[cmd_id]) && new_cmd[cmd_id].option1) 
	{
		opt1_major_opt1 = opt1_major_macro = new_cmd[cmd_id].option1; 
check marco:
		while(opt1_major_opt1->option_2 !=NULL) 
		{
			if (opt1_major_opt1->id == buf[0]) // 00h buf[0] = cmd[1] 
			{
				opt2_minor_opt2 = opt1_major_opt1->option_2;
				while(opt2_minor_opt2->arr !=0) // if para_arr exist 
				{
					if (opt2_minor_opt2->id == buf[1]) // 03h buf[1] = cmd[2]    match
					{
						if (opt2_minor_opt2->type == TYPE_MACRO) // 03h  buf[1] = cmd[2]   match
						{
							buf +=4; // move memory address by 4 units ; but why?!
							if(buf >= end)
							{
								err=7;
								goto err_nak;
							}
							opt1_major_opt1 = opt1_major_macro = (struct _cmd_option1_major *) (opt2_minor_opt2->arr); // 
							goto check_macro;
						}else                              // 03 buf[1] =cmd[2]  not match
						{
							int l = (buf[2]<<8 |buf[3]); // buf[2] =cmd[4] buf[3] =cmd[5]  Length High bytes *256 + Low Byte
							if (cmd[0] == opcode[0]) // check 82h 
							{
								if(l !=opt2->num)  // check length size with parameters table SIZE 
								{
									err =8;
									goto err_nak;
								}if((buf+4+l) !=end) // check length size ; if not match the end
								{
									err =9;
									goto err_nak;
								}
							}else
							{
								if ((cmd[0] & 0x40) && l)
								{
									err =10;
									goto err_nak;

								}else if (!( cmd[0]& 0x40) && (l!=opt2_minor_opt2->num))
								{
									err=11;
									goto err_nak;
								}
							}

							buf +=4+l;
							if(buf > end)
							{
								err =12;
								goto err_nak;
							}
							else if(buf == end)
							{
								goto do_command;
							}
							opt1_major_opt1 = opt1_major_macro;
							goto check_macro;
						}
					}
					opt2_minor_opt2 ++; // ?!
				}
				err=13;
				goto err_nak;
			}
			opt1_major_opt1 ++;
		}
		err=14;
		goto err_nak;
	}
	else
	{
		err=15;
		goto err_nak;
	}

do_command:
	buf = ccd_capture_buffer_2;
	opt1=macro = new_cmd[cmd_id].option_1;
do_macro:
	while(opt1_major_opt1->option_2 !=NULL) // if parameter table exist and get parameter table successfully
	{
		if(opt1_major_opt1->id == buf[0]) //   check 00h buf[0] = cmd[1] 
		{
			opt2_minor_opt2 = opt1->option_2; // get parameter table
			while(opt2_minor_opt2->arr !=0)  // if paramter table option exist and get option successfully
			{
				if(opt2_minor_opt2->id == buf[1]) // check 03h buf[1]=cmd[2]
				{
					if(opt2_minor_opt2->type == TYPE_MACRO)
					{
						buf +=4;
						opt1_major_opt1 = macro = (struct _cmd_option1_major *) (opt2->arr); // ?!
						goto do_macro;
					}else
					{
						int e, l = (buf[2]<<8 |buf[3]);
						if (cmd[0] == opcodes[0] || cmd[0] ==0x90 || cmd[0]==0x91)
						{
							if ((e = exec_action(opt2_minor_opt2,&buf[4],cmd[3]))<0)
							{
								err=16;
								goto err_nak;
							}
							if(e==0)
								goto no_ack;
							goto do_finish;
						}else
						{
							if (cmd[0]&0x40)
							{
								int rn;
								rn = send_bytes(opt1_major_opt1,opt2_minor_opt2,result_buf1,send_len);
								if(rn<0)
								{
									err=21;
									goto err_nak;
								}
								if(opt2_minor_opt2->type == TYPE_FUNCTION)
									send_len += rn;
								else
									send_len += 4 + opt2_minor_opt2->num;
							}else
							{
								unsigned long trigger_mode_mask = get_trigger_mode_mask();
								unsigned char mode = PARAM_VAL(eTriggerMode);

								if(rece_bytes(opt1,opt2,&buf[4]<0))
								{
									err = 17;
									goto err_nak;
								}
								
								if((trigger_mode_mask) & (1<<PARAM_VAL(eTriggerMode))) == 0 
								{
									write_param_to_flash(PARAM_ADDR(eTriggerMode),mode,USER_TEMP_PARA_ID);
									err=18;
									goto err_nak;
								}

								if(mode != PARAM_VAL(eTriggerMode))
								{
									extern void opt_mode_init(void);
									opt_mode_inti();
								}
							}
						}
						buf += 4+l;
						if(buf == end)
							goto do_finish;
						opt1 = macro;
						goto do_macro;
					}
				}
				opt2 ++;
			}
			err=21;
			goto err_nak;
		}
		opt1 ++;
	}
	err = 22;
	goto err_nak;


}
