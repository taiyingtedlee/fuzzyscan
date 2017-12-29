#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024
int opcode(unsigned char *cmd);
int status(unsigned char *cmd);
int length(unsigned char *cmd);
int ack_nak(int a);

int dispatch_cmd(unsigned char *cmd,int p_len)
{	
	unsigned char pre_suf[2]={0x7e,0x7e};
	unsigned char *p_cmd;
	unsigned char ch_buf[BUF_SIZE]={'\0'};
	unsigned char buf[BUF_SIZE]={'\0'};
	// check ch_buf len
	int ch_buf_len=0;
	int pre=0,i,j,len=p_len/2;
	
	ch_buf_len=(int)strlen(ch_buf);
	
	if (ch_buf_len==0)
	{
		pre=-1;
	}else if(ch_buf_len!=0)
	{
		pre=0;
	}
	
	int prefix_i=0;
	int suffix_i=0;
	unsigned char chksum=0;
	int buf_i=0;
	// 
	for(i=ch_buf_len,j=ch_buf_len;i<len;i++,j++)
	{
		// No 7eh appears
		if(pre==-1)
		{
			if(*(cmd+i)==0x7e)
			{
				// save 0x7e to ch_buf[0] and goto 1st 7eh appears
				pre_suf[0]=*(cmd+i);			
				ch_buf[j]=*(cmd+i);
				pre=0;
				prefix_i=i;
//				printf("data [%d] : %c  %d=> Prefix!\n",i,*(cmd+i),cmd[i]);
			}else if ( *(cmd+i) != 0x7e )
			{
				j--;
				suffix_i=i;
				buf[i]=cmd[i];
//				printf("data [%d] : %c  %d=> read, BUT NO Prefix...\n",i,*(cmd+i),cmd[i]);
				// no save and contiune
			}

		// GOT 1st 7eh prefix
		}else if(pre==0){
		// if buf[i]==0x7e
			if ( *(cmd+i) == 0x7e )
			{
				// save 0x7e to ch_buf[i] and goto 2nd 7eh appears
				pre_suf[1]=*(cmd+i);
				ch_buf[j]=*(cmd+i);
				suffix_i=i;
//				printf("data [%d] : %c  %d=> Suffix!\n",suffix_i,*(cmd+i),cmd[i]);
				pre=1;
				i--;
			}else if( *(cmd+i) != 0x7e )
			{
				// save data and contiune
				ch_buf[j]=*(cmd+i);
				suffix_i=i;
//				printf("data [%d] : %c  %d=> read and saved\n",i,*(cmd+i),cmd[i]);
			}
		// 2nd 7eh appears
		}else if (pre==1)
		{	
			int ret=0;
			int ack=0;
			// save ch_buf_len at ch_buf[0] 
			ch_buf[0]=len-prefix_i;
			// check opcode
			opcode(ch_buf);
			// check status
			ack=status(ch_buf);
			// check Length / Para_bytes
			ret=length(ch_buf);

			ch_buf_len=len-prefix_i;
//			printf("cmd len : %d\n",ch_buf_len-2);

			// Check Digit LRC
			for (i=1;i<suffix_i-prefix_i;i++)
			{
				chksum^=ch_buf[i];
			}
//			printf("chksum : 0x%02x\n",chksum);
			
			for (i=0;i<ch_buf_len;i++)
			{
				if (i==0)
				{
					printf("%02x ",pre_suf[0]);
					continue;
				}else if ((i == ch_buf_len-1))
				{
					printf("%02x ",chksum);
					printf("%02x ",pre_suf[1]);
					break;
				}
				printf("%02x ",ch_buf[i]);
			}
				
			// Status ack request or not && reply err or correct!!!
			if (ack==1)
			{
				printf("\n");
				ack_nak(ret);
			}

		}
	}

//	printf("prefix_i : %d\n",prefix_i);
//	printf("suffix_i : %d\n",suffix_i);
	/* No 7eh appears*/
	if(pre==-1)
	{	for(i=0;i<=suffix_i-prefix_i;i++)
		{
			printf("%02x ",buf[i]);
		}
	}
	
	// show result with prefix nor suffix; Checkdigit ch_buf[1]~ch_buf[ch_buf_len-1]
	/* 1st 7eh appears as prefix*/
	if(pre==0)
	{
		// check Digit LRC
		for (i=1;i<=suffix_i-prefix_i;i++)
		{
			chksum^=ch_buf[i];
		}
//		printf("chksum : 0x%02x\n",chksum);

		ch_buf_len=len-prefix_i;
//		printf("cmd len : %d\n",ch_buf_len-1);
		
		for (i=0;i<ch_buf_len;i++)
		{
			if (i==0)
			{
				printf("%02x ",pre_suf[0]);
				continue;
			}else if ((i == ch_buf_len-1))
			{
				printf("%02x ",ch_buf[i]);
				printf("%02x ",chksum);
				printf("%02x",pre_suf[1]);
				break;
			}
			printf("%02x ",ch_buf[i]);
		}
	}
	printf("\n");
	return 0;
}

// ch_buf[1]~ch_buf[3]
int opcode(unsigned char *cmd)
{
	int ck_len=(cmd[5]<<8)+cmd[6];
	//LED
	if (cmd[1]==0x80 && cmd[2]==0x00 && cmd[3]==0x00 && cmd[0]==ck_len+8)
	{	
		// RLED
		if (cmd[7]==0x01)
		{
			printf("RLED ");
			switch(cmd[8])
			{
				case 0x00:
					printf("OFF!\n");
					break;
				case 0x01:
					printf("ON!\n");
					break;
				case 0x02:
					printf("Restore to system control!\n");
					break;
				default:
					printf("check para_table!\n");
			}
		// GLED
		}else if(cmd[7]==0x02)
		{
			printf("GLED ");
			switch(cmd[8])
			{
				case 0x00:
					printf("OFF!\n");
					break;
				case 0x01:
					printf("ON!\n");
					break;
				case 0x02:
					printf("Restore to system control!\n");
					break;
				default:
					printf("check para_table!\n");
			}
		// BLED
		}else if(cmd[7]==0x03)
		{
			printf("BLED ");
			switch(cmd[8])
			{
				case 0x00:
					printf("OFF!\n");
					break;
				case 0x01:
					printf("ON!\n");
					break;
				case 0x02:
					printf("Restore to system control!\n");
					break;
				default:
					printf("check para_table!\n");
			 }
		}
	
	}
	return 0;
}
// ch_buf[4] 
int status(unsigned char *cmd)
{
	int ret=0;
	if (cmd[4]==0x00)
	{
		printf("LRC and ACK/NCK request!\n");
		ret=1;
	}else if(cmd[4]!=0x00)
	{
		printf("Please check setting!\n");
		ret=0;
	}
	return ret;
}
// ch_buf[5]~ch_buf[6] NOTE : cmd[0] is cmd's length!!!!
int length(unsigned char *cmd)
{	
	int ret=0;
	int ck_len=0;
	int Para_bytes=0;
	ck_len=(cmd[5]<<8)+cmd[6];
	Para_bytes=cmd[0]-8;
	printf("cmd lenght : %d \n",cmd[0]);
	// prefix and suffix 2 bytes; opcode 3 bytes; status 1 bytes; length 2 bytes; para_bytes variable!
	if(cmd[0]<8)
	{	
//		printf("Cmd is not complete!\n");
		ret=0;
	}else if(ck_len == Para_bytes)
	{
		printf("Length : %d match Para_bytes : %d\n",ck_len,Para_bytes);
		ret=1;
	}else if (ck_len != Para_bytes)
	{
		printf("Length : %d does not match Para_bytes : %d\n",ck_len,Para_bytes);
		ret=0;
	}
	return ret;
}

int ack_nak(int ack)
{
	int ret=0;
	if (ack==1)
	{
		printf("7e 0f 00 00 00 00 00 0f 7e");
		ret=1;
	}else if(ack ==0)
	{
		printf("7e 0e 00 xx 00 00 00 0e 7e");
		ret=0;
	}

	return ret;
}
