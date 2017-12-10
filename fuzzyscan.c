#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PREFIX 0x7e
#define SUFFIX 0x7e

int host_send(unsigned char s[]);
int dev_read(unsigned char s[]);
int dev_reply(unsigned char s[]);

int main(void)
{
    unsigned char cmd[50];
	unsigned char f_cmd[50];
	unsigned char *p_cmd;
	FILE *fp;
	int i;
	
#ifdef R_FILE
	fp=fopen("test.txt","r");
	if (fp == NULL)
	{
		printf("fopen read Failure!!!\n");
	}

	while (fgets(f_cmd,50,fp)!=NULL)
	{
		printf("Len : %d\n",host_send(f_cmd));	
	}

	fclose(fp);

#else
	while(1){
		printf("Input cmd :\n");
   		scanf("%[a-fA-F0-9]",f_cmd); 

		printf("Len : %d\n",host_send(f_cmd));
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
	int ch_len;
	// TWO chars convert into ONE char 7eh -> 126d 
	unsigned char dec_cmd[50]={'\0'};
	unsigned char check_digit={'\0'};
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

			}else if ( (i/2) == 1)
			{
				str[i]=str[i];
			}
			
			// High * 	
			dec_cmd[i/2]+=str[i];

		//Convert ASCII char A-F Dec 65-70
		}else if (str[i]>='A' && str[i]<='F')
		{
			str[i]=str[i]-65+10;
			// High *16  Low *1 
			if ( (i%2) == 0)
			{
				str[i]=str[i]*16;

			}else if ( (i/2) == 1)
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

			}else if ( (i/2) == 1)
			{
				str[i]=str[i];
			}

			dec_cmd[i/2]+=str[i];

		}
		
	}
	
	// Check PRE 1 Byte 7eh and SUF 1 Byte 7eh
	if (dec_cmd[0]==0x7e && dec_cmd[9]==0x7e)
	{
		printf("prefix & suffix checked!\n");
		// Check Opcode 3 Bytes 80h 00h 00h 
		if (dec_cmd[1]==0x80 && dec_cmd[2]==0x00 && dec_cmd[3]==0x00)
		{
			printf("Opcode checked!\n");
			// Check Status 1 Byte 00h 
			if (dec_cmd[4]==0x00)
			{
				printf("Length checked!\n");
				// Check Length 2 Byte 00h 02h 
				if (dec_cmd[5]==0x00 && dec_cmd[6]==0x02)
				{	
					// LED Selection
					if (dec_cmd[7]==0x01) //RLED
					{
						switch(dec_cmd[8]) //LED Status
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
					}else if (dec_cmd[7]==0x02) //GLED
					{
			
						switch(dec_cmd[8]) // LED Status
						{
							case 0x00:
								printf("GREEN LED OFF!!!\n");
								break;
							case 0x01:
								printf("GREEN LED ON!!!\n");
								break;
							case 0x02:
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}
	
					}else if (dec_cmd[7]==0x03) //BLED
					{
						switch(dec_cmd[8]) // LED Status
						{
							case 0x00:
								printf("BLUE LED OFF!!!\n");
								break;
							case 0x01:
								printf("BLUE LED ON!!!\n");
								break;
							case 0x02:
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}	
					}	

				}else if(dec_cmd[5]!=0x00 || dec_cmd[6]!=0x02)
				{
					printf("ERROR: Length!\n");
				}
			}else if (dec_cmd[4]!=0x00)
			{
				printf("ERROR: Status!\n");
			}	
	
		}else if (dec_cmd[1]!=0x80 || dec_cmd[2]!=0x00 || dec_cmd[3]!=0x00) 
		{
			printf("NOT LED setting!\n");
		}
		
	}else if (len<20)
	{
		printf("ERROR: Insufficient cmd length!\n");
		if (  dec_cmd[0]!=0x7e || dec_cmd[9]!=0x7e)
		{
			printf("ERROR: Prefix and Suffix must be 7e!\n");
		}
	}else if ( dec_cmd[0]!=0x7e || dec_cmd[9]!=0x7e)
	{
		printf("ERROR: Prefix and Suffix must be 7e!\n");
	}

#ifdef CD
	// check_digit CMD[1] XOR ... XOR CMD[len-1]
	for (i=1;i<(len/2)-1;i++)
	{
		check_digit^=dec_cmd[i];
	}
	printf("C.D enable\n");
#else
	printf("C.D disable\n");
#endif
	printf("C.D : 0x%02x\n",check_digit);
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
	for(i=0;i<len/2;i++){
		if (i==(len/2)-1){
			printf("%02x ",check_digit);
		}
		printf("%02x ",dec_cmd[i]);
	}
#endif
	printf("\n");
	return len;

}

int dev_read(unsigned char str[])
{
	
}

int dev_reply(unsigned char str[])
{	
	int len=strlen(str);
	int i;
	int chech_digit=0;


	printf("Dev received cmd (char): ");
	for(i=0;i<len;i++){
		printf("%c",str[i]);
	}
	
	printf("\n");
		
	return len;
}


