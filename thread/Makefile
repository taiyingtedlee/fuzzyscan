LIBS ?= -lpthread
app:rw_thread.o
	gcc -o app rw_thread.o $(LIBS)
rw_thread.o:rw_thread.c
	gcc -c rw_thread.c
clean:
	rm *.o app
