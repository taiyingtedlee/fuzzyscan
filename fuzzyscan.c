#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 2048
#define SIZE 100
#define PREFIX 0x7e
#define SUFFIX 0x7e

typedef struct Command_struct{
	unsigned char dec_cmd[SIZE];
}CMD_STRUCT;

CMD_STRUCT parse_arr_s(unsigned char str[])
{
	CMD_STRUCT output;
	int i;
	unsigned int len=(unsigned int)strlen(str);

	for (i=0;i<len;i++)
	{
		//Convert ASCII char 0-9 Dec 48-57
		if (str[i]>='0' && str[i]<='9')
		{
			str[i]=str[i]-48;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]*16;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}
			// dec_cmd[i/2]=High[] + Low[i+1]
			output.dec_cmd[i/2]+=str[i];

		//Convert ASCII char A-F Dec 65-70
		}else if (str[i]>='A' && str[i]<='F')
		{
			str[i]=str[i]-65+10;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]*16;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}
			output.dec_cmd[i/2]+=str[i];

		//Convert ASCII char a-f Dec 97-102	
		}else if (str[i]>='a' && str[i]<='f')
		{
			str[i]=str[i]-97+10;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]*16;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}
			output.dec_cmd[i/2]+=str[i];
		}
	}
	return output;
}



int host_parse(unsigned char s[]);
int dev_read(unsigned char s[]);
int dev_reply(unsigned char s[]);

int check_pre(unsigned char s[]);
int check_opcode(unsigned char s[],int p);
int check_status(unsigned char s[],int p);
int check_length(unsigned char s[],int p);
int ret_para_bytes(unsigned char s[],int p);
int check_parameter(unsigned char s[],int p,int bytes);

unsigned char check_digit_fn(unsigned char s[],int length);

int led_indicator_p(unsigned char s[]);

int main(void)
{
    unsigned char buf[SIZE];
	unsigned char f_cmd[SIZE];
	unsigned char *p_cmd;
	int len;
	CMD_STRUCT cmd_struct;
	FILE *fp;
	int i;
	
#ifdef R_FILE
	fp=fopen("test.txt","r");
	if (fp == NULL)
	{
		printf("fopen read Failure!!!\n");
	}

	while (fgets(f_cmd,SIZE,fp)!=NULL)
	{
		printf("Len : %d\n",host_send(f_cmd));	
	}

	fclose(fp);

#else
	p_cmd=(unsigned char *)malloc(sizeof(char)*MAX);
	while(1){
		printf("Input cmd :\n");
   		scanf("%[^\n]",p_cmd); 
		
		host_parse(p_cmd);
		setbuf(stdin,NULL);
	}
	free(p_cmd);
#endif
	return 0;
}

int host_parse(unsigned char str[])
{
	int len=strlen(str);
	int pre_p=check_pre(str);
	int opcode_p=check_opcode(str,pre_p);
	int status_p=check_status(str,opcode_p);
	
	int length_p=check_length(str,status_p);
	int para_bytes=ret_para_bytes(str,status_p);

//	int para_p=check_parameter(str,length_p,para_bytes);

	printf("Input len :%d\n",len);
	// check prefix	
	// check Opcode 
	// check Status
	// check Length
	if (pre_p>=0)
	{
		printf("7e start : [%d],[%d]\n",pre_p,pre_p+1);
		check_opcode(str,pre_p);
		check_status(str,opcode_p);
		check_length(str,status_p);
		ret_para_bytes(str,status_p);
		check_parameter(str,length_p,para_bytes);
//		printf("suffix 7e start: %d\n",para_p);

	}else
		printf("Need 7e as prefix!\n");

	return 0;
}

int dev_read(unsigned char str[])
{
	
}

int dev_reply(unsigned char str[])
{	
	int len=strlen(str);
	int i=0;
	int chech_digit=0;

	printf("Dev received cmd (char): ");
	for(i=0;i<len;i++){
		printf("%c",str[i]);
	}	
	printf("\n");
		
	return len;
}

int check_pre(unsigned char str[])
{
	int i;
	int p=-1;
	int len=strlen(str);
	for (i=0;i<len;i++){
		if(str[i]=='7' && str[i+1]=='e'){
			p=i;
			break;
		}else
			p=-1;
	}
	return  p;
}

int check_opcode(unsigned char str[],int pre_p)
{
	int len=strlen(str);
	int p=pre_p+1;
	int bit_count=0;
	do
	{
			if( (str[p]>='0' && str[p]<='9') || (str[p]>='a' && str[p]<='f') || (str[p]>='A' && str[p]<='F'))
			{
				bit_count++;
				
			}
			p++;

	}while(bit_count<6 && pre_p!=(len-2));
	
	printf("Opcode end : [%d]\n",p);

	return p;

}

int check_status(unsigned char str[],int opcode_p)
{
	int len=strlen(str);
	int dec_cmd[2]={0};
	int bit_count=0;
	int p=opcode_p+1;
	int check_digit=1;
	do
	{	

		if( (str[p]>='0' && str[p]<='9') || (str[p]>='a' && str[p]<='f') || (str[p]>='A' && str[p]<='F'))
		{	
			//Convert ASCII char 0-9 Dec 48-57
			if (str[p]>='0' && str[p]<='9')
			{
				dec_cmd[bit_count]=str[p]-48;
			//Convert ASCII char A-F Dec 65-70
			}else if (str[p]>='A' && str[p]<='F')
			{
				dec_cmd[bit_count]=str[p]-65+10;
			//Convert ASCII char a-f Dec 97-102	
			}else if (str[p]>='a' && str[p]<='f')
			{
				dec_cmd[bit_count]=str[p]-97+10;
			}
			bit_count++;
		}
		p++;
	}while(bit_count<2 && opcode_p<len);

	printf("Status end : [%d]\n",p-1);

	return p-1;
}

int check_length(unsigned char str[],int status_p)
{
	int len=strlen(str);
	int dec_cmd[4]={0};
	int bit_count=0;
	int p=status_p+1;
	int i;
	do
	{
		if( (str[p]>='0' && str[p]<='9') || (str[p]>='a' && str[p]<='f') || (str[p]>='A' && str[p]<='F'))
		{
			if (str[p]>='0' && str[p]<='9')
			{
				dec_cmd[bit_count]=str[p]-48;
			//Convert ASCII char A-F Dec 65-70
			}else if (str[p]>='A' && str[p]<='F')
			{
				dec_cmd[bit_count]=str[p]-65+10;
			//Convert ASCII char a-f Dec 97-102	
			}else if (str[p]>='a' && str[p]<='f')
			{
				dec_cmd[bit_count]=str[p]-97+10;
			}

			bit_count++;
		}
		p++;
	
	}while(bit_count<4 && status_p<len);
	
	printf("Length end : [%d]\n",p-1);	

	return p-1;
}

int ret_para_bytes(unsigned char str[],int status_p)
{
	int len=strlen(str);
	int dec_cmd[4]={0};
	int bit_count=0;
	int p=status_p+1;
	int para_bytes=0;
	int i;
	do
	{
		if( (str[p]>='0' && str[p]<='9') || (str[p]>='a' && str[p]<='f') || (str[p]>='A' && str[p]<='F'))
		{
			if (str[p]>='0' && str[p]<='9')
			{
				dec_cmd[bit_count]=str[p]-48;
			//Convert ASCII char A-F Dec 65-70
			}else if (str[p]>='A' && str[p]<='F')
			{
				dec_cmd[bit_count]=str[p]-65+10;
			//Convert ASCII char a-f Dec 97-102	
			}else if (str[p]>='a' && str[p]<='f')
			{
				dec_cmd[bit_count]=str[p]-97+10;
			}

			bit_count++;
		}
		p++;
	}while(bit_count<4 && status_p<len);
	
	para_bytes=(dec_cmd[0]<<12)+(dec_cmd[1]<<8)+(dec_cmd[2]<<4)+(dec_cmd[3]);
	printf("Parameter Bytes : %d\n",para_bytes);

	return para_bytes;
}

int check_parameter(unsigned char str[],int length_p,int para_bytes)
{
	int len=strlen(str);
	int dec_cmd[MAX]={'\n'};
	int para_bits=para_bytes*2;
	int bit_count=0;
	int p=length_p+1;
	int i;
	do
	{
				
		if( (str[p]>='0' && str[p]<='9') || (str[p]>='a' && str[p]<='f') || (str[p]>='A' && str[p]<='F'))
		{
			if (str[p]>='0' && str[p]<='9')
			{
				dec_cmd[bit_count]=str[p]-48;
			//Convert ASCII char A-F Dec 65-70
			}else if (str[p]>='A' && str[p]<='F')
			{
				dec_cmd[bit_count]=str[p]-65+10;
			//Convert ASCII char a-f Dec 97-102	
			}else if (str[p]>='a' && str[p]<='f')
			{
				dec_cmd[bit_count]=str[p]-97+10;
			}
			bit_count++;
		}
		p++;
	}while(bit_count<len && length_p<len && p<len);
	
	if ((bit_count/2)==para_bytes){
		printf("Parameter input : %d\n",bit_count/2);
	}else
		printf("ERROR : input parameter Bytes : %d\n",bit_count/2);
		
	printf("Parameter end : [%d]\n",p);

	return p;
}

unsigned char check_digit_fn(unsigned char str[],int length)
{
	int i;
	int len=length-1;
	unsigned char check_digit=0;
	for (i=1;i<len;i++)
	{
		check_digit^=str[i];
	}
	return check_digit;
}
