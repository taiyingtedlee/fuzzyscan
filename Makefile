CFLAGS += -DCD -DLRC #-DSHOW_CMD -DR_FILE
myapp: fuzzyscan.o
	gcc -o myapp fuzzyscan.o
fuzzyscan.o: fuzzyscan.c
	gcc -c fuzzyscan.c $(CFLAGS)
clean:
	rm *.o 
