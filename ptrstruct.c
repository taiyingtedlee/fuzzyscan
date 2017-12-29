#include <stdio.h>
#include <stdlib.h>

struct Room;
struct House;

struct Room
{
	float width;
	float length;
	float height;
	char *name;
};

struct House
{
	char *address;
	struct Room *rooms[10];
};

void printHouse(struct House house);

int main(void)
{
	struct House h;
	h.address="10 Palace Road";
	for(int i=0;i<10;i++)
	{
		h.rooms[i]= NULL;
	}

	struct Room hall;
	hall.width = 10;
	hall.length = 12;
	hall.height = 9;
	hall.name = "Hall";

	h.rooms[0] = &hall;

	printHouse(h);

	return 0;
}

void printHouse(struct House house)
{
	printf("%s",house.address);
	printf("\n\r\n\r");
	for(int i= 0; i<10; i++)
	{
		if(house.rooms[i] !=NULL)
		{
			printf("Room #%d : %s\n\r",i+1,house.rooms[i]->name);
		}
	}
}
