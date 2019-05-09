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
	int i;
	pid_t naves[N_NAVES];
	int nave_pipe[N_EQUIPOS][2];
	char mensaje[MAX_MENSAJE] = {0};

	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if (tuberia == 0)
		return EXIT_FAILURE;

	// Create pipes for naves
	for (i = 0; i < N_NAVES; i++)
	{
		if (pipe(nave_pipe[i]) == -1) {
			printf("[ERROR] Could not create pipe");
			goto FREE_ALL;
		}
	}

	// Launch the ships
	for (i = 0; i < N_NAVES; i++)
	{
		naves[i] = fork();
		if (naves[i] < 0)
		{
			printf("[ERROR] Error creating nave for nave %d\n", i);
			goto FREE_ALL;
		}
		else if (naves[i] == 0)
		{ // NAVE
#ifdef DEBUG
			printf("~~~ [JEFE %d] Launching ship %d\n", id, i);
#endif
			naves_main(i, nave_pipe[i]);
			exit(EXIT_SUCCESS);
		}
	}

	// Get into the main loop for jefe, getting msg and processing them with the controller
	while (1)
	{
		memset(mensaje, 0, sizeof(mensaje));

		// Read message from simulator
		read(tuberia[0], &mensaje, sizeof(mensaje));
		close(tuberia[1]);

		// Receive orders from simulator and send others to ships
		controller_jefe(id, nave_pipe, mensaje);
	}

FREE_ALL:

	// Liberar pipes de las naves
	for (i = 0; i < N_NAVES; i++)
	{
		close(nave_pipe[i][0]);
		close(nave_pipe[i][1]);
		wait(NULL);
	}

	return EXIT_SUCCESS;
}

void controller_jefe(int id, int naves[N_NAVES][2], char *mensaje)
{
	int i;
	int accion_1, accion_2;
	char envio_accion_1[MAX_MENSAJE] = {0};
	char envio_accion_2[MAX_MENSAJE] = {0};

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
			accion_1 = rand() % 2;
			accion_1 == 1 ? strcpy(envio_accion_1, "ATACAR") : strcpy(envio_accion_1, "MOVER_ALEATORIO");

			for (int j = 0; j < N_NAVES; j++)
				write(naves[j][1], envio_accion_1, sizeof(envio_accion_1));

			// Mandar accion 2 a las naves
			accion_2 = rand() % 2;
			accion_2 == 1 ? strcpy(envio_accion_1, "ATACAR") : strcpy(envio_accion_1, "MOVER_ALEATORIO");

			for (int j = 0; j < N_NAVES; j++)
				write(naves[j][1], envio_accion_2, sizeof(envio_accion_2));
		}
	}
	else if (strncmp(mensaje, "DESTRUIR", 7) == 0)
	{
#ifdef DEBUG
		printf("[JEFE %d] Executing DESTRUIR\n", id);
#endif
		int num_nave = mensaje[8] - '0';
		write(naves[num_nave][1], &mensaje, sizeof(mensaje));
	}
	else if (strcmp(mensaje, "FIN") == 0)
	{
#ifdef DEBUG
		printf("[JEFE %d] Executing FIN\n", id);
#endif
		/*FIN: Manda finalizar a los procesos nave mediante señal SIGTERM*/
		/* Destruye todas sus naves excepto a si mismo (jefe)*/
		kill(getpid(), SIGTERM);
	}

	return;
}