CFLAG = -DR_FILE -DCD
myapp: fuzzyscan.o
	gcc -o myapp fuzzyscan.o
fuzzyscan.o: fuzzyscan.c
	gcc -c fuzzyscan.c $(CFLAG)
clean:
	rm *.o 
default:
	
