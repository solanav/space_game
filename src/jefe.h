#ifndef JEFE_H_
#define JEFE_H_

#include "mapa.h"

/*elimina el mapa y los semaforos*/
void delete_recurses();

/*
abre la memoria compartida
devuelve NULL en caso de ERROR
devuelve el mapa si todo va bien
*/
tipo_mapa *open_shared_memory();

int jefe_main(int id, int tuberia[2]);

/*
recibe una tuberia desde el simulador para la comunicacion
devuelve -1 en caso de ERROR
devuele 1 si todo va bien
*/
void controller_jefe(int id, int naves[N_NAVES][2], char *mensaje);

/* elige la accion que va a realizar su nave */
int jefe_random_accion();

/* el jefe elige a que va a atacar */
int jefe_ataca_equipo();

/* el jefe elije la posicion a la que va a mover la nave */
int jefe_mueve_nave_();


#endif /* JEFE_H_ */