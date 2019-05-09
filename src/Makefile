TARGET = ../target

CC = gcc
CFLAGS = -g -Wall -pthread -pedantic -I.
LDLIBS = -lrt -lncurses -lm -D DEBUG

BOLD=\e[1m
NC=\e[0m

all: simulador monitor naves

call: clean all

clean:
	rm -r -f $(TARGET)
	sudo rm -r -f /dev/shm/*
	sudo rm -r -f /dev/mqueue/*

simulador:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c jefe.c simulador.c -o $(TARGET)/simulador $(LDLIBS)

monitor:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c gamescreen.c monitor.c -o $(TARGET)/monitor $(LDLIBS)

jefe:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c simulador.c jefe.c -o $(TARGET)/monitor $(LDLIBS)

naves:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c naves.c -o $(TARGET)/naves $(LDLIBS)