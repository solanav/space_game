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
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

#include "jefe.h"
#include "simulador.h"
#include "naves.h"

#define SEM "/sem_shared_memory"
#define MAX_MENSAJE 32

int jefe_main(int id, int tuberia[2])
{
	int ret;
	int i;
	pid_t naves[N_NAVES];
	int nave_pipe[N_EQUIPOS][2];
	char mensaje[MAX_MENSAJE] = {0};

	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if (tuberia == 0)
		return;

	// Read message from simulator
	read(tuberia[0], &mensaje, sizeof(mensaje));
	close(tuberia[1]);

	// Crete the ships with forks
	for (i = 0; i < N_NAVES; i++) {
		naves[i] = fork();
		if (naves[i] < 0) {
			printf("[ERROR] Error creating nave for nave %d\n", i);
			ret = 1;
			goto FREE_ALL;
		}
		else if (naves[i] == 0) { // NAVE
#ifdef DEBUG
			printf("[JEFE %d] Launching ship %d\n", id, i);
#endif
			naves_main(i, nave_pipe[i]);
			goto FREE_ALL;
		}
	}

	// Receive orders from simulator and send others to ships
	controller_jefe(id, nave_pipe, mensaje);

	FREE_ALL:

	// Liberar pipes de las naves
	for (i = 0; i < N_NAVES; i++) {
		close(nave_pipe[i][0]);
		close(nave_pipe[i][1]);
		wait(NULL);
	}
	return ret;
}

void controller_jefe(int id, int naves[N_NAVES][2], char *mensaje)
{
	int nave[2]; /*tuberia para comunicarnos con las naves*/
	int i;
	int accion_1, accion_2;
	char envio_accion_1[MAX_MENSAJE] = {0};
	char envio_accion_2[MAX_MENSAJE] = {0};

	/* Creamos una tuberia para la comunicacion con las naves */
	if (pipe(nave) == -1)
	{
		printf("[ERROR] Error al crear la tuberia para comunicarnos con las naves\n");
		return;
	}

	/*comparamos el mensaje recibido para ver que tipo de instrucciones hemos recibido */
	if (strcmp(mensaje, "TURNO") == 0)
	{
#ifdef DEBUG
		printf("[JEFE %d] Executing TURNO\n", id);
#endif
		/*creamos un numero aleatorio y se le enviamos a la nave */
		for (i = 0; i <= N_EQUIPOS; i++)
		{
			// Crypto-secure because that is really important for the game :)
			int seed = 0;
			getrandom(&seed, sizeof(seed), 0);
			srand(seed);

			// Mandar accion 1 a las naves
			accion_1 = jefe_random_accion(id);
			if(accion_1 == -1){
				printf("[ERROR] Se ha recibido una accion invalida\n");
				return;
			}

			if(accion_1 == 1) strcpy(envio_accion_1,"ATACAR");
			else strcpy(envio_accion_1,"MOVER_ALEATORIO");

			for (int j = 0; j < N_NAVES; j++)
				write(nave[1], envio_accion_1, sizeof(envio_accion_1));

			// Mandar accion 2 a las naves
			accion_2 = jefe_random_accion(id);
			if(accion_2 == -1) {
				printf("[ERROR] Se ha recibido una accion invalida\n");
				return;
			}
			if(accion_2 == 1) strcpy(envio_accion_2,"ATACAR");
			else strcpy(envio_accion_2,"MOVER_ALEATORIO");

			for (int j = 0; j < N_NAVES; j++)
				write(nave[1], envio_accion_2, sizeof(envio_accion_2));
		}
	}
	else if (strncmp(mensaje, "DESTRUIR", 7) == 0)
	{
#ifdef DEBUG
		printf("[JEFE %d] Executing DESTRUIR\n", id);
#endif
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
#ifdef DEBUG
		printf("[JEFE %d] Executing FIN\n", id);
#endif
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
		printf("[ERROR] Se ha recibido una instrcion no válida\n");
		return;
	}

	return;
}

int jefe_random_accion(int id)
{
	int resultado;

	resultado = (rand() % 2) + 1;
	if (resultado == 1)
	{
#ifdef DEBUG
		printf("[JEFE %d] Rand Resultado = %d -> ATACAR\n", resultado, id);
#endif
		return 0;
	}
	if (resultado == 2)
	{
#ifdef DEBUG
		printf("[JEFE %d] Rand Resultado = %d -> MOVER ALEATORIO \n", resultado, id);
#endif
		return 1;
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