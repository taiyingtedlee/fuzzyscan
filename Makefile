myapp: fuzzyscan.o
	gcc -o myapp fuzzyscan.o
fuzzyscan.o: fuzzyscan.c
	gcc -c fuzzyscan.c
clean:
	rm *.o 

