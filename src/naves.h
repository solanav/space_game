#ifndef NAVES_H_
#define NAVES_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "simulador.h"
#include "mapa.h"

#define QUEUE_NAME "/naves_queue"
#define MSGSIZE 256

typedef struct {
	int orden;
	Coords destino;
	Coords origen;
} Nave_orden;

// Main function
int naves_main(int id, int tuberia_nave[2]);

// Funcion para crear la cola de mensajes nave->simulador
int create_queue();

// Wrapper para mandar mensajes a una queue facilmente
int send_msg(mqd_t queue, Nave_orden msg);

#endif /* NAVES_H_ */