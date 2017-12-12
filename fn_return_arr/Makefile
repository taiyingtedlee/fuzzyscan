#CFLAGS += -DR_FILE
myapp: return_arr.o
	gcc -o myapp return_arr.o
fuzzyscan.o: return_arr.c
	gcc -c return.c $(CFLAGS) 
clean:
	rm *.o 
