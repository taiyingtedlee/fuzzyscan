#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024
#define SIZE 100
#define PREFIX 0x7e
#define SUFFIX 0x7e

int host_send(unsigned char s[]);
int dev_read(unsigned char s[]);
int dev_reply(unsigned char s[]);

char * parse_ch_arr(unsigned char s[]);
int check_pre_suf(unsigned char s[],int length);
int check_opcode(unsigned char s[],int length);
int check_status(unsigned char s[],int length);
int check_length(unsigned char s[],int length);
unsigned char check_digit_fn(unsigned char s[],int length);

int led_indicator_p(unsigned char s[]);


int main(void)
{
    unsigned char cmd[SIZE];
	unsigned char f_cmd[SIZE];
	unsigned char *p_cmd;
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
   		scanf("%[a-fA-F0-9]",p_cmd); 

		printf("Len : %d\n",host_send(p_cmd));
//		printf("parse_ch_arr : %s\n",parse_ch_arr(p_cmd));
		setbuf(stdin,NULL);
	}
	
#endif
	return 0;
}

int host_send(unsigned char str[])
{
	int i;
#ifdef R_FILE
	int len=strlen(str)-1;
#else
	int len=strlen(str); 
#endif
	// TWO chars convert into ONE char 7eh -> 126d 
	unsigned char dec_cmd[SIZE]={0};
	unsigned char check_digit;
	// 1.Convert  2.Group 

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
			dec_cmd[i/2]+=str[i];

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
			dec_cmd[i/2]+=str[i];

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
			
			dec_cmd[i/2]+=str[i];
		}
		
	}
	// Check PRE 1 Byte 7eh and SUF 1 Byte 7eh
	check_pre_suf(dec_cmd,len/2);
	// Check Opcode 3 Bytes 
	check_opcode(dec_cmd,len/2);
	// Check Status 1 Byte 00h ; return Check Digit 1 Bytes or 2 Bytes
	printf("Check Digit : %d Bytes\n",check_status(dec_cmd,len/2));
	// Check Length 2 Bytes ; return Parameters n Bytes
	printf("Parameter : %d Bytes\n",check_length(dec_cmd,len/2));
	

#ifdef CD
	// check_digit CMD[1] XOR ... XOR CMD[len-1]
	check_digit=check_digit_fn(dec_cmd,len/2);
	printf("Enable C.D : %02x\n",check_digit);
#else
	printf("Disable C.D\n");
#endif	
	printf("Parse cmd: ");
	for(i=0;i<len;i++){
		printf("%d ",str[i]);
	}	
	printf("\n");
	printf("CMD str: ");
#ifdef SHOW_CMD	
	for(i=0;i<len/2;i++){
		printf("CMD[%d] : %dd = char %c = 0x%0x\n",i,dec_cmd[i],dec_cmd[i],dec_cmd[i]);
	}
#else
#endif
	for(i=0;i<len/2;i++){
#ifdef CD	
		if (i==(len/2)-1)
		{
			printf("%02x ",check_digit);
		}
#else
#endif
		printf("%02x ",dec_cmd[i]);
	}

	memset(dec_cmd,0,sizeof(dec_cmd));
	printf("\n");
	return len;
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

char * parse_ch_arr(unsigned char str[])
{
	int i;
	unsigned int len=(unsigned int)strlen(str);
	unsigned char *pparse_str2ch;
	unsigned int len_p;	
	pparse_str2ch=(unsigned char*)malloc(sizeof(char)*MAX);

	for (i=0;i<len;i++)
	{
		//Convert ASCII char 0-9 Dec 48-57
		if (str[i]>='0' && str[i]<='9')
		{
			str[i]=str[i]-48;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]<<4;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}
			
			// High  	
			*(pparse_str2ch+(i/2))+=str[i];

		//Convert ASCII char A-F Dec 65-70
		}else if (str[i]>='A' && str[i]<='F')
		{
			str[i]=str[i]-65+10;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]<<4;

			}else if ( (i%2) == 1)
			{
				str[i]=str[i];
			}

			*(pparse_str2ch+(i/2))+=str[i];

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

			*(pparse_str2ch+(i/2))+=str[i];

		}
		
	}
	
	len_p=strlen(pparse_str2ch);
	printf("len_p : %d\n",len_p);

	return pparse_str2ch;

}

int check_pre_suf(unsigned char str[],int length)
{
	if (str[0]==0x7e && str[length-1]==0x7e)
	{
		printf("Prefix & Surffix Correct!\n");
	}else if(str[0]!=0x7e || str[length-1]!=0x7e)
	{
		printf("ERROR : Prefix or Surffix!\n");
	}
	return 0;
}

int check_opcode(unsigned char str[],int length)
{
	// Check Opcode 3 Bytes 80h 00h 00h 
	if (str[1]==0x80)
	{
		printf("Opcode First Byte 80h checked!\n");
		if (str[2]==0x00 && str[3]==0x00)
		{
			printf("LED indicator!\n");
			led_indicator_p(str);
		}else if (str[2]==0x10 && str[3]==0x00)
		{
			printf("Beeping!\n");
		}else if (str[2]==0x20 && str[3]==0x00)
		{
			printf("Serial Trigger!\n");
		}else if (str[2]==0x30 && str[3]==0x00)
		{
			printf("Store Config!\n");
		}else
		{
			printf("Other setting!\n");
		}
	}else if(str[1]!=0x80)
	{
		printf("Opcode First Byte not 80h\n");
	}

	return 0;

}

int check_status(unsigned char str[],int length)
{
	int i;
	int byte_cd=0;
	if (str[4]==0x00)
	{
		printf("LRC : ");
		byte_cd=1;
	}else if(str[4]==0x08)
	{
		printf("CRC : ");
		byte_cd=2;
	}
	return byte_cd;
}

int check_length(unsigned char str[],int length)
{
	int byte_p=str[5]*256+str[6];
	if (byte_p==length-8)
	{
		printf("Parameter Bytes Correct!\n");
	}else
		printf("ERROR: Parameter Bytes!\n");
	return byte_p;
}

int led_indicator_p(unsigned char str[])
{
	if (str[7]==0x01) // RLED
	{
		switch(str[8])
		{
		case 0x00:
			printf("RED LED OFF!!!\n");
			break;
		case 0x01:
			printf("RED LED ON!!!\n");
			break;
		case 0x02:
			printf("ReStore to system control!!!\n");
			break;
		default:
			printf("CHECK LED SETTING!!!\n");
		}

	}else if (str[7]==0x02) // GLED
	{
		switch(str[8])
		{
		case 0x00:
			printf("RED LED OFF!!!\n");
			break;
		case 0x01:
			printf("RED LED ON!!!\n");
			break;
		case 0x02:
			printf("ReStore to system control!!!\n");
			break;
		default:
			printf("CHECK LED SETTING!!!\n");
		}
	}else if (str[7]==0x03) // BLED
	{
		switch(str[8])
		{
		case 0x00:
			printf("RED LED OFF!!!\n");
			break;
		case 0x01:
			printf("RED LED ON!!!\n");
			break;
		case 0x02:
			printf("ReStore to system control!!!\n");
			break;
		default:
			printf("CHECK LED SETTING!!!\n");
		}
	}
	
	return 0;
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
