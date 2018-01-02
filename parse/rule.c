#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/h2d.h"

#define PRE_SUF 2
#define BUF_SIZE 1024

<<<<<<< HEAD
int dispatch_cmd(unsigned char *cmd,int p_len)
{	
	unsigned char *p_cmd;
	unsigned char ch_buf[BUF_SIZE]={'\0'};
=======
int main(void)
{
	FILE *fp;
	unsigned char ch;
	unsigned char *p_buf;
	unsigned char pre_suf[PRE_SUF]={'\0','\0'};
	unsigned char buf[SIZE]={'\0'};
	unsigned char ch_buf[SIZE]={'\0'};
	int i=0,j,len;
	int para_len;
	
	fp=fopen("../cmd.txt","r+");

	if(fp ==NULL)
	{
		printf("fopen failure!\n");
		exit (1);
	}
	
	fread(buf,sizeof(buf),1,fp);
		
	p_buf=h2d(buf);
>>>>>>> 529ed6501e0ffcc0ba3efe687d491ebd23725842
	
	//check cmd size
	fseek(fp,0,SEEK_END);
	int cmd_size = (ftell(fp)/2);
	printf("cmd.txt size : %d\n",cmd_size);

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
<<<<<<< HEAD
	for(i=ch_buf_len,j=ch_buf_len;i<len;i++,j++)
=======
	for(i=ch_buf_len,j=ch_buf_len;i<cmd_size;i++,j++)
>>>>>>> 529ed6501e0ffcc0ba3efe687d491ebd23725842
	{
		// No 7eh appears
		if(pre==-1)
		{
			if(*(cmd+i)==0x7e)
			{
				// save 0x7e to ch_buf[0] and goto 1st 7eh appears
				ch_buf[j]=*(cmd+i);
				pre=0;
<<<<<<< HEAD
				prefix_i=i;
				printf("data [%d] : %c  => Prefix!\n",i,*(cmd+i));
			}else if ( *(cmd+i) != 0x7e )
=======
				printf("data [%d] : Prefix is read!\n",i);
			}else if ( *(p_buf+i) != 0x7e )
>>>>>>> 529ed6501e0ffcc0ba3efe687d491ebd23725842
			{
				j--;
				printf("data [%d] : %c  => read, BUT NO Prefix...\n",i,*(cmd+i));
				// no save and contiune
				printf("data [%d] : %d is read, BUT NO Prefix!\n",i,ch_buf[j]);
			}

		// 1st 7eh appears
		}else if(pre==0){
		// if buf[i]==0x7e
			if ( *(cmd+i) == 0x7e )
			{
				// save 0x7e to ch_buf[?] and goto 2nd 7eh appears
<<<<<<< HEAD
				ch_buf[j]=*(cmd+i);
				suffix_i=i;
				printf("data [%d] : %c  => Suffix!\n",suffix_i,*(cmd+i));
=======
				ch_buf[j]=*(p_buf+i);
				printf("data [%d] : Suffix is read!\n",i);
>>>>>>> 529ed6501e0ffcc0ba3efe687d491ebd23725842
				pre=1;
				i--;
			}else if( *(cmd+i) != 0x7e )
			{
				// save data and contiune
<<<<<<< HEAD
				ch_buf[j]=*(cmd+i);
				printf("data [%d] : %c  => read and saved\n",i,*(cmd+i));
=======
				ch_buf[j]=*(p_buf+i);
				printf("data [%d] : %d is saved!\n",i,ch_buf[j]);
>>>>>>> 529ed6501e0ffcc0ba3efe687d491ebd23725842
			}
		// 2nd 7eh appears
		}else if (pre==1)
		{	
<<<<<<< HEAD
			int ck_len;
			int Para_bytes=(suffix_i-(prefix_i+7));
			ck_len=(ch_buf[5]<<8)+ch_buf[6];
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


int main(void)
{
	FILE *fp;
	unsigned char *p_buf;
	unsigned char pre_suf[PRE_SUF]={'\0','\0'};
	unsigned char buf[BUF_SIZE]={'\0'};
	int i=0,j,len;
=======
			printf("CMD complete!\n");
			break;
		}
	}
	
	// show result NOTE: 0x00 in ch_buf[], show nothing, strlen() read '\0' as ending value
	ch_buf_len=(int)strlen(ch_buf);
	printf("after ch_buf_len: %d\n",ch_buf_len);
	// 
	for (i=0;i<cmd_size;i++)
	{
		printf("[%d] : %d %c\n",i,ch_buf[i],ch_buf[i]);
	}
>>>>>>> 529ed6501e0ffcc0ba3efe687d491ebd23725842
	
	fp=fopen("../cmd.txt","r+");

	if(fp ==NULL)
	{
		printf("fopen failure!\n");
		exit (1);
	}
	
	fread(buf,sizeof(buf),1,fp);
	len=(int)strlen(buf);
	printf("len : %d\n",len);

	p_buf=h2d(buf);
	
	dispatch_cmd(p_buf,len);
	fclose(fp);

	return 0;
}

