/**
 * @file jefe.c
 * @author Catalin Rotaru (catalin.rotaru@estudiante.uam.es)
 * @author Antonio Solana (antonio.solana@estudiante.uam.es)
 * Grupo: 2212
 * @date
 * @brief
 */
#include "jefe.h"

#define SEM "/sem_shared_memory"

/*VARIABLES GLOBALES*/
sem_t *sem_shared_memory = NULL;

/*se elimina el semaforo y el mapeo*/
void delete_recurses(){
	tipo_mapa * mapa = NULL;
	/* HAY QUE TENER EN CUENTA ( CERRAR Y ELIMINAR EL SEMÁFORO  Y EL MAPEO )*/
	mapa = open_shared_memory();
	if(!mapa) return;
	
	munmap(mapa, sizeof(*mapa));
	shm_unlink(SHM_MAP_NAME);

	sem_close(sem_shared_memory);
	sem_unlink(SEM);
}

/****************************************************************/
/****************************************************************/
/****************************************************************/
tipo_mapa *open_shared_memory(){
	int fd_shm;
	tipo_mapa * mapa = NULL;
	/* Creamos un semaforo para que dos procesos no puedan acceder a la vez */
   sem_shared_memory= sem_open(SEM, O_CREAT , S_IRUSR | S_IWUSR , 1);
   if(sem_shared_memory == SEM_FAILED){
	   perror("sem_shared_memory");
	   return NULL;
   }

	sem_wait(sem_shared_memory);

	/* leemos de memoria compartida el mapa */
	/* We open the shared memory */
	fd_shm = shm_open(SHM_MAP_NAME,
		O_RDONLY, /* Obtain it and open for reading */
		0); /* Unused */
	if(fd_shm == -1){
		fprintf (stderr, "Error opening the shared memory segment \n");
		return NULL;
	}

	/* mapeo del mapa */
	mapa = mmap(NULL, sizeof(*mapa),
			PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

	if (mapa == NULL) {
		printf("[ERROR] No se ha podido mapear la memoria compartida\n");
	}

	sem_post(sem_shared_memory);

	return mapa;
	/*accedemos a los recursos del mapa mediante mapa->*/
}

/****************************************************************/
/****************************************************************/
/****************************************************************/
int pipe_since_simulador(int tuberia[2]){
	int pipe_status;
	/*va a ser una cadena de caracteres donde se reciben las instrucciones */
	char *mensaje = NULL;
	int nave[2]; /*tuberia para comunicarnos con las naves*/
	int n_nave_destruir;
	int accion_a_realizar;
	tipo_mapa * mapa = NULL;

	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if(tuberia == 0) return -1;

	/* obtenemos el mapa que se encuentra en memoria compartida */
	mapa = open_shared_memory();
	if(!mapa){
		printf("La memoria compartida la esta usando otro proceso\n");
		return -1;
	}

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
			return -1;
		}

		/* Se elige una accion de forma aleatoria */
		accion_a_realizar = random_accion();
		if(accion_a_realizar < 0 || accion_a_realizar > 3){
			printf("ERROR en la llamada a random_accion\n");
			return -1;
		}
		/* enviamos la primera accion */
		write(nave[1], &accion_a_realizar, sizeof(accion_a_realizar));
		close(nave[0]);

		/* Se elige una accion de forma aleatoria */
		accion_a_realizar = random_accion();
		if(accion_a_realizar < 0 || accion_a_realizar > 3){
			printf("ERROR en la llamada a random_accion\n");
			return -1;
		}

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
			return -1;
		}

		/*nos comunicamos con las naves enviando el nº nave a destruir*/
		write(nave[1], &n_nave_destruir, sizeof(n_nave_destruir));
		close(nave[0]);
	}
	else if(strcmp(mensaje,"FIN") == 0){
		printf("DEBUG: fin OK\n");
		/*kill(pid,SIGTERM);*/

	}
	else{
		printf("Se ha recibido una instrcion no válida\n");
		return -1;
	}

	return 1;
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
	else if(resultado == 2){
		printf("DEBUG: Resultado = %d -> MOVER ALEATORIO",resultado);
		return 2;
	}
	else if(resultado == 3){
		printf("DEBUG: Resultado = %d -> DESTRUIR",resultado);
		return 3;
	}
	return -1;

}

/****************************************************************/
/****************************************************************/
/****************************************************************/

int random_equipo_nave(){
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

int random_posicion_x_y(){
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
