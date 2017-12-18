#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../h2d.h"

#define PRE_SUF 2
#define SIZE 1024

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
	
	//check cmd size
	fseek(fp,0,SEEK_END);
	int cmd_size = (ftell(fp)/2);
	printf("cmd.txt size : %d\n",cmd_size);

	// check ch_buf len
	int ch_buf_len=(int)strlen(ch_buf);
	printf("ch_buf_len: %d\n",ch_buf_len);
	int pre=0;
	
	if (ch_buf_len==0)
	{
		pre=-1;
	}else if(ch_buf_len!=0)
	{
		pre=0;
	}

	// 
	for(i=ch_buf_len,j=ch_buf_len;i<cmd_size;i++,j++)
	{
		// No 7eh appears
		if(pre==-1)
		{
			if(*(p_buf+i)==0x7e)
			{
				// save 0x7e to ch_buf[0] and goto 1st 7eh appears
				ch_buf[j]=*(p_buf+i);
				pre=0;
				printf("data [%d] : Prefix is read!\n",i);
			}else if ( *(p_buf+i) != 0x7e )
			{
				j--;
				// no save and contiune
				printf("data [%d] : %d is read, BUT NO Prefix!\n",i,ch_buf[j]);
			}

		// 1st 7eh appears
		}else if(pre==0){
		// if buf[i]==0x7e
			if ( *(p_buf+i) == 0x7e )
			{
				// save 0x7e to ch_buf[?] and goto 2nd 7eh appears
				ch_buf[j]=*(p_buf+i);
				printf("data [%d] : Suffix is read!\n",i);
				pre=1;
			}else if( *(p_buf+i) != 0x7e )
			{
				// save data and contiune
				ch_buf[j]=*(p_buf+i);
				printf("data [%d] : %d is saved!\n",i,ch_buf[j]);
			}
		// 2nd 7eh appears
		}else if (pre==1)
		{	
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
	
	// show array	
/*	
	for(i=0;i<len;i++)
	{
		printf("%d ",*(p_buf+i));
	}
	printf("\n");	
*/
	fclose(fp);

	return 0;
}

