TARGET = ../target

CC = gcc
CFLAGS = -g -Wall -pthread -pedantic -I.
LDLIBS = -lrt -lncurses

BOLD=\e[1m
NC=\e[0m

all: simulador monitor

call: clean all

clean:
	rm -r -f $(TARGET)
	sudo rm -r -f /dev/shm/*

simulador:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c simulador.c -o $(TARGET)/simulador -lrt -lm

monitor:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) gamescreen.c mapa.c monitor.c -o $(TARGET)/monitor -lrt -lncurses -lm
