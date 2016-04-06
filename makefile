CC=gcc -pthread -std=c90
CFLAGS = -g -Wall

all: driver

driver: main.c
	$(CC) -o driver main.c -g -Wall


clean:
	rm -f *.o a.out main driver
