#CFLAGS += -DCD -DLRC #-DSHOW_CMD -DR_FILE
CFLAGS += -g
myapp: main.o
	gcc -o myapp main.o
main.o: main.c
	gcc -c main.c $(CFLAGS)
clean:
	rm *.o myapp 
