/**
 * @file jefe.c
 * @author Catalin Rotaru (catalin.rotaru@estudiante.uam.es)
 * @author Antonio Solana (antonio.solana@estudiante.uam.es)
 * Grupo: 2212
 * @date
 * @brief
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/random.h>

#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

#include "jefe.h"
#include "simulador.h"

#define SEM "/sem_shared_memory"
#define MAX_MENSAJE 16

/*VARIABLES GLOBALES*/
sem_t *sem_shared_memory = NULL;

/*se elimina el semaforo y el mapeo*/
void delete_recurses()
{
	tipo_mapa *mapa = NULL;
	/* HAY QUE TENER EN CUENTA ( CERRAR Y ELIMINAR EL SEMÁFORO  Y EL MAPEO )*/
	mapa = open_shared_memory();
	if (!mapa)
		return;

	munmap(mapa, sizeof(*mapa));
	shm_unlink(SHM_MAP_NAME);

	sem_close(sem_shared_memory);
	sem_unlink(SEM);
}

tipo_mapa *open_shared_memory()
{
	int fd_shm;
	tipo_mapa *mapa = NULL;
	/* Creamos un semaforo para que dos procesos no puedan acceder a la vez */
	sem_shared_memory = sem_open(SEM, O_CREAT, S_IRUSR | S_IWUSR, 1);
	if (sem_shared_memory == SEM_FAILED)
	{
		perror("sem_shared_memory");
		return NULL;
	}

	sem_wait(sem_shared_memory);

	/* leemos de memoria compartida el mapa */
	/* We open the shared memory */
	fd_shm = shm_open(SHM_MAP_NAME,
					  O_RDONLY, /* Obtain it and open for reading */
					  0);		/* Unused */
	if (fd_shm == -1)
	{
		fprintf(stderr, "Error opening the shared memory segment \n");
		return NULL;
	}

	/* mapeo del mapa */
	mapa = mmap(NULL, sizeof(*mapa),
				PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

	if (mapa == NULL)
	{
		printf("[ERROR] No se ha podido mapear la memoria compartida\n");
	}

	sem_post(sem_shared_memory);

	return mapa;
	/*accedemos a los recursos del mapa mediante mapa->*/
}

void pipe_since_simulador(int tuberia[2])
{
	int pipe_status;
	char mensaje[MAX_MENSAJE] = {0};
	int nave[2]; /*tuberia para comunicarnos con las naves*/
	int i;
	int accion_1, accion_2;
	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if (tuberia == 0)
		return;

	read(tuberia[0], &mensaje, sizeof(mensaje));
	close(tuberia[1]);

	/* Creamos una tuberia para la comunicacion con las naves */
	pipe_status = pipe(nave);
	if (pipe_status == -1)
	{
		printf("Error al crear la tuberia para comunicarnos con las naves\n");
		return;
	}

	/*comparamos el mensaje recibido para ver que tipo de instrucciones hemos recibido */
	if (strcmp(mensaje, "TURNO") == 0)
	{
		printf("DEBUG: turno OK\n");
		/*creamos un numero aleatorio y se le enviamos a la nave */
		for (i = 0; i <= N_EQUIPOS; i++)
		{
			// Crypto-secure because that is really important for the game :)
			int seed = 0;
			getrandom(&seed, sizeof(seed), 0);
			srand(seed);

			accion_1 = jefe_random_accion();
			if (accion_1 == -1)
			{
				printf("Se ha recibido una accion invalida\n");
				return;
			}
			/* 1º TURNO le enviamos la accion a las naves */
			write(nave[1], &accion_1, sizeof(accion_1));
			close(nave[0]);

			accion_2 = jefe_random_accion();
			if (accion_2 == -1)
			{
				printf("Se ha recibido una accion invalida\n");
				return;
			}
			/* 2º TURNO  le enviamos la accion a las naves */
			write(nave[1], &accion_2, sizeof(accion_2));
			close(nave[0]);
		}
	}
	else if (strncmp(mensaje, "DESTRUIR", 7) == 0)
	{
		printf("DEBUG: destruir OK\n");
		/* si se recibe destruir se tiene que enviar otro mensaje para ver que nave se quiere destuir */
		/*comprobamos que el numero de la nave es correcto */

		printf(" %c \n", mensaje[8]);

		/* destruir la nave mensaje[8] */
		/* enviamos a la nave DESTRUIR <num_nave> */
		write(nave[1], &mensaje, sizeof(mensaje));
		close(nave[0]);
	}
	else if (strcmp(mensaje, "FIN") == 0)
	{
		printf("DEBUG: fin OK\n");
		/*FIN: Manda finalizar a los procesos nave mediante señal SIGTERM*/
		/* Destruye todas sus naves excepto a si mismo (jefe)*/
		kill(getpid(), SIGTERM);

		/* No hace falta que envie ningún mensaje
		kill(0,SIGTERM);
		write(nave[1], &mensaje, sizeof(mensaje));
		close(nave[0]);
*/
	}
	else
	{
		printf("Se ha recibido una instrcion no válida\n");
		return;
	}

	return;
}

int jefe_random_accion()
{
	int resultado;
	/*elige un numero aleatorio entre el 1,2 */

	/* el jefe solo elige mover o atacar */

	resultado = (rand() % 2) + 1;
	if (resultado == 1)
	{
		printf("DEBUG: Resultado = %d -> ATACAR\n", resultado);
		return 1;
	}
	if (resultado == 2)
	{
		printf("DEBUG: Resultado = %d -> MOVER ALEATORIO \n", resultado);
		return 2;
	}
	return -1;

	/* La accion de destuir la recibe el jefe desde el simulador pq una nave de otro equipo la ha matado y el jefe que dirije a la nave
	que se ha destruido manda destruir a la nave */
}

int jefe_ataca_equipo()
{
	int resultado;
	/*elige un numero aleatorio entre el 1,2,3 */
	resultado = (rand() % 3) + 1;
	if (resultado < 0 || resultado > 3)
		printf("ERROR al generar el numero aleatorio para elegir al equipo que queremos atacar \n");

	return resultado;
}

int jefe_mueve_nave_()
{
	int posicion_x_y = -1;
	/*elige un numero aleatorio entre el 1,2,3 */
	srand(time(NULL));
	posicion_x_y = (rand() % 4) + 1;

	return posicion_x_y;
}