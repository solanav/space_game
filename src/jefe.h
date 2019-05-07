#ifndef JEFE_H_
#define JEFE_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

#include "simulador.h"
#include "naves.h"
#include "mapa.h"

/*elimina el mapa y los semaforos*/
void delete_recurses();

/*
abre la memoria compartida
devuelve NULL en caso de ERROR
devuelve el mapa si todo va bien
*/
tipo_mapa *open_shared_memory();

/*
recibe una tuberia desde el simulador para la comunicacion
devuelve -1 en caso de ERROR
devuele 1 si todo va bien
*/
int pipe_since_simulador(int tuberia[2] );

/*
elige una accion de forma aleatoria
devuelve 1 se se va a realizar la accion de atacar
devuelve 2 se se va a realizar la accion de mover_aleatorio
devuelve 3 se se va a realizar la accion de destruir
devuelve -1 en caso de ERROR
 */
int random_accion();

/*
elegimos al equipo que queremos atacar
devuelve el numero de equipo/nave que vamos a atacar/destruir
*/
int random_equipo_nave();

/*elige un numero aleatorio para mover a la nave*/
int random_posicion_x();

/*elige un numero aleatorio para mover a la nave*/
int random_posicion_y();

#endif /* JEFE_H_ */
