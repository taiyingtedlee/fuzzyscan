#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/h2d.h"

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
	int Para_len;
	
	fp=fopen("../cmd.txt","r+");

	if(fp ==NULL)
	{
		printf("fopen failure!\n");
		exit (1);
	}
	
	fread(buf,sizeof(buf),1,fp);
	len=(int)strlen(buf)/2;
	printf("len : %d\n",len);

	p_buf=h2d(buf);
	
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
	
	int prefix_i=0;
	int suffix_i=0;
	// 
	for(i=ch_buf_len,j=ch_buf_len;i<len;i++,j++)
	{
		// No 7eh appears
		if(pre==-1)
		{
			if(*(p_buf+i)==0x7e)
			{
				// save 0x7e to ch_buf[0] and goto 1st 7eh appears
				ch_buf[j]=*(p_buf+i);
				pre=0;
				prefix_i=i;
				printf("data [%d] : %c  => Prefix!\n",i,*(p_buf+i));
			}else if ( *(p_buf+i) != 0x7e )
			{
				j--;
				printf("data [%d] : %c  => read, BUT NO Prefix...\n",i,*(p_buf+i));
				// no save and contiune
			}

		// 1st 7eh appears
		}else if(pre==0){
		// if buf[i]==0x7e
			if ( *(p_buf+i) == 0x7e )
			{
				// save 0x7e to ch_buf[?] and goto 2nd 7eh appears
				ch_buf[j]=*(p_buf+i);
				suffix_i=i;
				printf("data [%d] : %c  => Suffix!\n",suffix_i,*(p_buf+i));
				pre=1;
				i--;
			}else if( *(p_buf+i) != 0x7e )
			{
				// save data and contiune
				ch_buf[j]=*(p_buf+i);
				printf("data [%d] : %c  => read and saved\n",i,*(p_buf+i));
			}
		// 2nd 7eh appears
		}else if (pre==1)
		{	
			int ck_len;
			int Para_bytes=(suffix_i-(prefix_i+7));
			ck_len=(ch_buf[5]<<8)+ch_buf[6];
			if(i<7)
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
	
	ch_buf_len=len-prefix_i;
	// show result
	for (i=0;i<ch_buf_len;i++)
	{
		printf("%d %c\n",ch_buf[i],ch_buf[i]);
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

