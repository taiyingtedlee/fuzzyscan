#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024
int opcode(unsigned char *cmd);
int status(unsigned char *cmd);
int length(unsigned char *cmd);

int dispatch_cmd(unsigned char *cmd,int p_len)
{	
	unsigned char *p_cmd;
	unsigned char ch_buf[BUF_SIZE]={'\0'};
	
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
	// 
	for(i=ch_buf_len,j=ch_buf_len;i<len;i++,j++)
	{
		// No 7eh appears
		if(pre==-1)
		{
			if(*(cmd+i)==0x7e)
			{
				// save 0x7e to ch_buf[0] and goto 1st 7eh appears
				ch_buf[j]=*(cmd+i);
				pre=0;
				prefix_i=i;
				printf("data [%d] : %c  => Prefix!\n",i,*(cmd+i));
			}else if ( *(cmd+i) != 0x7e )
			{
				j--;
				printf("data [%d] : %c  => read, BUT NO Prefix...\n",i,*(cmd+i));
				// no save and contiune
			}

		// 1st 7eh appears
		}else if(pre==0){
		// if buf[i]==0x7e
			if ( *(cmd+i) == 0x7e )
			{
				// save 0x7e to ch_buf[i] and goto 2nd 7eh appears
				ch_buf[j]=*(cmd+i);
				suffix_i=i;
				printf("data [%d] : %c  => Suffix!\n",suffix_i,*(cmd+i));
				pre=1;
				i--;
			}else if( *(cmd+i) != 0x7e )
			{
				// save data and contiune
				ch_buf[j]=*(cmd+i);
				printf("data [%d] : %c  => read and saved\n",i,*(cmd+i));
			}
		// 2nd 7eh appears
		}else if (pre==1)
		{	
			int ck_len;
			int Para_bytes=(suffix_i-(prefix_i+7));
			ck_len=(ch_buf[5]<<8)+ch_buf[6];
			
			// check status
			status(ch_buf);

			if((suffix_i-prefix_i)<7)
			{
				printf("Serial cmd length : %d is not complete!\n",j);
			}else if(ck_len == Para_bytes)
			{
				printf("Cmd lenght : %d\nLength : %d match Para_bytes : %d\n",j,ck_len,Para_bytes);
			}else if (ck_len != Para_bytes){
				printf("Cmd lenght : %d\nLength : %d does not match Para_bytes : %d\n",j,ck_len,Para_bytes);
			}
			break;
		}
	}

	// Check Digit LRC
	for (i=1;i<suffix_i-prefix_i;i++)
	{
		chksum^=ch_buf[i];
	}
//	printf("chksum : 0x%02x\n",chksum);

	ch_buf_len=len-prefix_i;
	// show result and Checkdigit
	for (i=0;i<ch_buf_len;i++)
	{
		if(i==(ch_buf_len-1))
		{
			printf("%02x ",chksum);
		}
		printf("%02x ",ch_buf[i]);
	}
	printf("\n");
	return 0;
}

// ch_buf[1]~ch_buf[3]
int opcode(unsigned char *cmd)
{
	return 0;
}
// ch_buf[4] 
int status(unsigned char *cmd)
{
	if (cmd[4]==0x00)
	{
		printf("LRC and ACK/NCK request!\n");
	}else if(cmd[4]!=0x00)
	{
		printf("Please check setting!\n");
	}
	return 0;
}
// ch_buf[5]~ch_buf[6]
int length(unsigned char *cmd)
{	
	int ck_len;
	int Para_bytes=(suffix_i-(prefix_i+7));
	ck_len=(ch_buf[5]<<8)+ch_buf[6];
			
	// check status
	status(ch_buf);

	if((suffix_i-prefix_i)<7)
	{	
		printf("Serial cmd length : %d is not complete!\n",j);
	}else if(ck_len == Para_bytes)
	{
		printf("Cmd lenght : %d\nLength : %d match Para_bytes : %d\n",j,ck_len,Para_bytes);
	}else if (ck_len != Para_bytes)
	{
		printf("Cmd lenght : %d\nLength : %d does not match Para_bytes : %d\n",j,ck_len,Para_bytes);
	}
	return 0;
}
