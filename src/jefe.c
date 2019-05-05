/**
 * @file jefe.c
 * @author Catalin Rotaru (catalin.rotaru@estudiante.uam.es)
 * @author Antonio Solana (antonio.solana@estudiante.uam.es)
 * Grupo: 2212
 * @date
 * @brief
 */

#include "simulador.h"
#include "jefe.h"

/****************************************************************/
/****************************************************************/
/****************************************************************/
int pipe_since_simulador(int tuberia[2]){
	int pipe_status;
	int tuberia[2] = {0};
	/*va a ser una cadena de caracteres donde se reciben las instrucciones */
	char *mensaje = NULL;
	int nave[2]; /*tuberia para comunicarnos con las naves*/
	int n_nave_destruir;
	int accion_a_realizar;

	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if(tuberia == 0) return -1;

	/*en el canal [0] se permite la lectura y cerramos el canal [1]*/
	/*Acceso a la tubería en modo lectura [0]*/
	read(tuberia[0], &mensaje, sizeof(mensaje));
	close(tuberia[1]);

	/*comparamos el mensaje recibido para ver que tipo de instrucciones hemos recibido */
	if(strcmp(mensaje,"TURNO") == 0){
		printf("DEBUG: turno OK\n");
		/*creamos la tuberia para la comunicacion con las naves*/
		pipe_status = pipe(nave);
		if (pipe_status == -1){
			printf("Error al crear la tuberia para comunicarnos con las naves\n");
			exit(EXIT_FAILURE);
		}

		/* Se elige una accion de forma aleatoria */
		accion_a_realizar = random_accion();
		if(accion_a_realizar < 0 || accion_a_realizar > 3){
			printf("ERROR en la llamada a random_accion\n");
			exit(EXIT_FAILURE);
		}

		/* enviamos la primera accion */
		write(nave[1], &accion_a_realizar, sizeof(accion_a_realizar));
		close(nave[0]);

		/* enviamos la segunda accion */
		write(nave[1], &accion_a_realizar, sizeof(accion_a_realizar));
		close(nave[0]);
	}
	else if(strcmp(mensaje,"DESTRUIR") == 0){
		printf("DEBUG: destuir OK\n");
		/*despues de recibir la instrcion de destuir leemos el nª nave que vamos a destruir */
		read(tuberia[0], &n_nave_destruir, sizeof(n_nave_destruir));
		close(tuberia[1]);

		/*creamos la tuberia para la comunicacion con las naves*/
		pipe_status = pipe(nave);
		if (pipe_status == -1){
			printf("Error al crear la tuberia para comunicarnos con las naves\n");
			exit(EXIT_FAILURE);
		}

		/*nos comunicamos con las naves enviando el nº nave a destruir*/
		write(nave[1], &n_nave_destruir, sizeof(n_nave_destruir));
		close(nave[0]);
	}
	else if(strcmp(mensaje,"FIN") == 0){
		printf("DEBUG: fin OK\n");
		kill(pid,SIGTERM);

	}
	else{
		printf("Se ha recibido una instrcion no válida\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}


/****************************************************************/
/****************************************************************/
/****************************************************************/

int random_accion(){
	int resultado;
	/*elige un numero aleatorio entre el 1,2,3 */
	srand(time(NULL));
	resultado = (rand() % 3 )+ 1;
	if(resultado == 1){
		printf("DEBUG: Resultado = %d -> ATACAR",resultado);
		return 1;
	}
	if(resultado == 2){
		printf("DEBUG: Resultado = %d -> MOVER ALEATORIO",resultado);
		return 2;
	}
	if(resultado == 3){
		printf("DEBUG: Resultado = %d -> DESTRUIR",resultado);
		return 3;
	}

}

/****************************************************************/
/****************************************************************/
/****************************************************************/

int random_nave(){
	int resultado;
	/*elige un numero aleatorio entre el 1,2,3 */
	srand(time(NULL));
	resultado = (rand() % 3 )+ 1;
	if(resultado < 0 || resultado > 3)
		printf("ERROR al generar el numero aleatorio para atacar a la nave \n");

	return resultado;

}

/****************************************************************/
/****************************************************************/
/****************************************************************/

int random_equipo(int n_equipo){
	int resultado;
	/*elige un numero aleatorio entre el 1,2,3 */
	srand(time(NULL));
	resultado = (rand() % 3 )+ 1;
	if(resultado < 0 || resultado > 3)
		printf("ERROR al generar el numero aleatorio para elegir al equipo que queremos atacar \n");

	return resultado;

}



/****************************************************************/
/****************************************************************/
/****************************************************************/

int random_posicion_x(){
	int posicion_x = -1;
	/*elige un numero aleatorio entre el 1,2,3 */
	srand(time(NULL));
	posicion_x = (rand() % MAPA_MAXX )+ 1;

	return posicion_x;
}

/****************************************************************/
/****************************************************************/
/****************************************************************/

int random_posicion_y(){
	int posicion_y = -1;
	/*elige un numero aleatorio entre el 1,2,3 */
	srand(time(NULL));
	posicion_y = (rand() % MAPA_MAXY )+ 1;

	return posicion_y;
}
