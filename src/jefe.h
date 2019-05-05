#ifndef JEFE_H_
#define JEFE_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/types.h>
#include <time.h>

/*recibe una tuberia desde el simulador para la comunicacion */
int pipe_since_simulador(int );

/* elige una accion de forma aleatoria */
int random_accion();

/* elige una nave para atacarla de forma aleatoria */
int random_accion();

/*elegimos al equipo que queremos atacar */
int random_equipo(int n_equipo);

/*elige un numero aleatorio para mover a la nave*/
int random_posicion_x();

/*elige un numero aleatorio para mover a la nave*/
int random_posicion_y();


#endif /* JEFE_H_ */
