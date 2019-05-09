TARGET = ../target

CC = gcc
CFLAGS = -g -Wall -pthread -pedantic -I.
LDLIBS = -lrt -lncurses

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
	$(CC) $(CFLAGS) jefe.c mapa.c simulador.c -o $(TARGET)/simulador -lrt -lm

monitor:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) gamescreen.c mapa.c monitor.c -o $(TARGET)/monitor -lrt -lncurses -lm

jefe:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c simulador.c jefe.c -o $(TARGET)/monitor -lrt -lncurses -lm

naves:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) naves.c -o $(TARGET)/naves -lrt