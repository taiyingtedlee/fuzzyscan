#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRE 0x7e
#define SUF 0x7e

int host_send(unsigned char s[]);
int dev_read(unsigned char s[]);
int dev_reply(unsigned char s[]);

int main(void)
{
    unsigned char cmd[50];
	unsigned char f_cmd[50];
	FILE *fp;
	int i;
	
/*

	fp=fopen("test.txt","r");
	if (fp == NULL)
	{
		printf("fopen read Failure!!!\n");
	}

	for(i=0;i<50;i++)
	{
		fscanf(fp,"%c",&f_cmd[i]);
	}
	
	fclose(fp);
*/
	printf("Input cmd :\n");
   	scanf("%[a-zA-Z0-9]",f_cmd); 
	
	printf("Len : %d\n",host_send(f_cmd));
	
	return 0;
}

int host_send(unsigned char str[])
{
	int i;
	int len=strlen(str); 
	
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
	if (dec_cmd[0]==126 && dec_cmd[9]==126)
	{
		printf("prefix & suffix checked!\n");
		// Check Opcode 3 Bytes 80h 00h 00h 
		if (dec_cmd[1]==128 && dec_cmd[2]==0 && dec_cmd[3]==0)
		{
			printf("Opcode checked!\n");
			// Check Status 1 Byte 00h and Length 2 Bytes00h 02h
			if (dec_cmd[4]==0 && dec_cmd[5]==0 && dec_cmd[6]==2)
			{
				printf("Status and Length checked!\n");
				// LED Selection
				if (dec_cmd[7]==1) //RLED
				{
						switch(dec_cmd[8]) //LED Status
						{
							case 0:
								printf("RED LED OFF!!!\n");
								break;
							case 1:
								printf("RED LED ON!!!\n");
								break;
							case 2:
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}

				}else if (dec_cmd[7]==2) //GLED
				{
					
						switch(dec_cmd[8]) // LED Status
						{
							case 0:
								printf("GREEN LED OFF!!!\n");
								break;
							case 1:
								printf("GREEN LED ON!!!\n");
								break;
							case 2:
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}

				}else if (dec_cmd[7]==3) //BLED
				{
						switch(dec_cmd[8]) // LED Status
						{
							case 0:
								printf("BLUE LED OFF!!!\n");
								break;
							case 1:
								printf("BLUE LED ON!!!\n");
								break;
							case 2:
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}	
				}	

			}else if (dec_cmd[4]!=0 || dec_cmd[5]!=0 || dec_cmd[6]!=2)
			{
				printf("ERROR: Status and Length!\n");
			}

		}else if (dec_cmd[1]!=128 || dec_cmd[2]!=0 || dec_cmd[3]!=0) 
		{
			printf("NOT LED setting!\n");
		}
		
	}else if (len<20)
	{
		printf("ERROR: Insufficient cmd length!\n");
		if (  dec_cmd[0]!=126 || dec_cmd[9]!=126)
		{
			printf("ERROR: Prefix and Suffix must be 7e!\n");
		}
	}else if ( dec_cmd[0]!=126 || dec_cmd[9]!=126)
	{
		printf("ERROR: Prefix and Suffix must be 7e!\n");
	}

	// check_digit CMD[1] XOR ... XOR CMD[len-1]
	for (i=1;i<(len/2)-1;i++)
	{
		check_digit^=dec_cmd[i];
	}
	
	printf("C.D : 0x%02x\n",check_digit);

	printf("Parse cmd: ");
	for(i=0;i<len;i++){
		printf("%d",str[i]);
	}
	
	printf("\n");
	
	printf("CMD str:\n");
	for(i=0;i<len/2;i++){
		printf("CMD[%d] : %dd = char %c\n",i,dec_cmd[i],dec_cmd[i]);
	}

	printf("\n");

	return len;

}

int dev_read(unsigned char str[])
{
	int len=strlen(str)-1;
	int i;
	// Check PRE 1 Byte 7eh and SUF 1 Byte 7eh
	if (str[0]=='7' && str[1]=='e' && str[18]=='7' && str[19]=='e')
	{
		printf("prefix & suffix checked!\n");
		// Check Opcode 3 Bytes 80h 00h 00h 
		if (str[2]=='8' && str[3]=='0' && str[4]=='0' && str[5]=='0' && str[6]=='0' && str[7]=='0')
		{
			printf("LED indicator Setting!\n");
			// Check Status 1 Byte 00h and Length 2 Bytes00h 02h
			if (str[8]=='0' && str[9]=='0' && str[10]=='0' && str[11]=='0' && str[12]=='0' && str[13]=='2')
			{
				printf("Status and Length checked!\n");
				// LED Selection
				if (str[14]=='0' && str[15]=='1') 
				{
					if (str[16]=='0') // RLED
					{
						switch(str[17]) //LED Status
						{
							case '0':
								printf("RED LED OFF!!!\n");
								break;
							case '1':
								printf("RED LED ON!!!\n");
								break;
							case '2':
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}
					}

				}else if (str[14]=='0' && str[15]=='2') 
				{
					if (str[16]=='0') //GLED
					{
						switch(str[17])
						{
							case '0':
								printf("GREEN LED OFF!!!\n");
								break;
							case '1':
								printf("GREEN LED ON!!!\n");
								break;
							case '2':
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}
					}

				}else if (str[14]=='0' && str[15]=='3') 
				{
					if (str[16]=='0') //BLED
					{
						switch(str[17])
						{
							case '0':
								printf("BLUE LED OFF!!!\n");
								break;
							case '1':
								printf("BLUE LED ON!!!\n");
								break;
							case '2':
								printf("ReStore to system control!!!\n");
								break;
							default:
								printf("CHECK LED SETTING!!!\n");
						}
					}	
				}	
				
			}else if (str[8]!='0' || str[9]!='0' || str[10]!='0' || str[11]!='0' || str[12]!='0' || str[13]!='2')
			{
				printf("Wrong Status and Length!\n");
			}

		}else if (str[2]!='8' || str[3]!='0' || str[4]!='0' || str[5]!='0' || str[6]!='0' || str[7]!='0') 
		{
			printf("NOT LED setting!\n");
		}
		
	}else if (len<20)
	{
		printf("ERROR: Insufficient cmd length!\n");
		if ( str[0]!='7' || str[1]!='e' || str[len-3]!='7' || str[len-2]!='e')
		{
			printf("ERROR: CMD Prefix and Suffix must be 7e!\n");
		}
	}else if( str[0]!='7' || str[1]!='e' || str[len-3]!='7' || str[len-2]!='e')
	{
		printf("ERROR: CMD Prefix and Suffix must be 7e!\n");
	}

	return len;

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


