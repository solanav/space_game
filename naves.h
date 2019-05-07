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
#include "jefe.h"
#include "mapa.h"

/* abre una cola de mensajes
 devuelve 0 en caso de error
 devuelve al cola si todo va bien
*/
mqd_t open_queue_system();

/*recibe una tuberia desde el simulador para la comunicacion */
int pipe_since_jefe(int tuberia[2]);

/*
funcion mediante la cual se atacan las naves
recibe como parametros el equipo al que vamos a atacar y a que nave queremos atacar
*/
void nave_ataca();

/*mueve la nave a la posición x,y*/
void mover_aleatorio();

/*
 comprbamos si se puede realizar la accion de mover
 deveuelve  -1 si hay ERROR
 devuelve 1 si la distancia obtenida se encuentra en el rango de ataque
*/
int distancia_naves(int x, int y);

/**/
void destruir_nave();

#endif /* NAVES_H_ */
