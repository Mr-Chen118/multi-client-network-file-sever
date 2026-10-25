CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lpthread

.PHONY:all clean

all: server

server: server.c transmission.h
	$(CC) $(CFLAGS) -o $@ server.c transmission.c $(LIBS)

client:client.c transmission.h
	$(CC) $(CFLAGS) -o $@ client.c transmission.c $(LIBS)

clean:
	rm -f server client