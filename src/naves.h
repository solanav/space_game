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


/*recibe una tuberia desde el simulador para la comunicacion */
int pipe_since_jefe(int );

/*
funcion mediante la cual se atacan las naves
recibe como parametros el equipo al que vamos a atacar y a que nave queremos atacar
*/
void atacar_nave(int equipo_a_atacar,int nave_a_atacar);

/*mueve la nave a la posición x,y*/
void mover_aleatorio(int x, int y);

void finaliza();

#endif /* NAVES_H_ */
