CC = gcc
CFLAGS = -g -Wall

all: cliente.o servidor.o
	$(CC) $(CFLAGS) cliente.o -o cliente
	$(CC) $(CFLAGS) servidor.o -o servidor
	rm *.o

.c.o:
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm -f servidor cliente