#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <errno.h>

#include "mapa.h"
#include "naves.h"
#include "jefe.h"

int jefe_pipe[N_EQUIPOS][2];

Coords random_coords(tipo_mapa *mapa, int team)
{
	Coords r_coords;
	int found_empty_space = 0;

	while (found_empty_space == 0)
	{
		r_coords.x = rand() % MAPA_MAXX;
		r_coords.y = rand() % MAPA_MAXY;

		if (mapa_is_casilla_vacia(mapa, r_coords.x, r_coords.y))
			found_empty_space = 1;
	}

	return r_coords;
}

int execute_order(tipo_mapa *mapa, Nave_orden order)
{
	switch (order.orden)
	{
	case 0: // MOVER
#ifdef DEBUG
		printf("Moving order by ship\n");
#endif
		if (!is_casilla_inside(mapa, order.destino.y, order.destino.x))
			return EXIT_FAILURE;

		if (!mapa_is_casilla_vacia(mapa, order.destino.y, order.destino.x))
			return EXIT_FAILURE;

		// realizar accion

		break;

	case 1: // ATACAR
#ifdef DEBUG
		printf("Attacking order by ship\n");
#endif
		if (!is_casilla_inside(mapa, order.destino.y, order.destino.x))
			return EXIT_FAILURE;

		if (mapa_get_distancia(order.destino.y, order.destino.x,
							   order.origen.y, order.origen.x) > ATAQUE_ALCANCE)
			return EXIT_FAILURE;

		// realizar accion

		break;

	default: // ERROR
		printf("[ERROR] Order received by ship is not understood\n");
		return EXIT_FAILURE;
		break;
	}

	return EXIT_SUCCESS;
}

int check_winner(tipo_mapa *mapa)
{
	int team_alive = -1;
	int equipos_muertos = 0;

	// For each team
	for (int i = 0; i < N_EQUIPOS; i++)
	{
		// Count dead teams
		if (mapa_get_num_naves(mapa, i) == 0)
		{
			equipos_muertos++;
		}
		else
		{
			// Save the alive team
			team_alive = i;
		}
	}

	// If only one team is alive, return it
	if (equipos_muertos == N_EQUIPOS - 1)
		return team_alive;

	// Still more than one teams alive
	return -1;
}

int receive_msg(mqd_t queue, Nave_orden *buff)
{
	if (mq_receive(queue, (char *)buff, sizeof(*buff), NULL) == -1)
	{
		printf("[ERROR] Cannot receive a message through the queue\n");
		printf("%s", strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main()
{
	int check = -1;
	sem_t *ready = NULL;
	int ret = 0;
	int i, j;
	tipo_mapa *mapa = NULL;
	tipo_nave nave;
	pid_t teams[N_EQUIPOS];
	int fd_shm;

	// Create shared memory for map
	fd_shm = shm_open(SHM_MAP_NAME,
					  O_RDWR | O_CREAT | O_EXCL,
					  S_IRUSR | S_IWUSR);

	if (fd_shm == -1)
	{
		printf("[ERROR] No se ha podido crear la memoria compartida\n");
		ret = 1;
		goto FREE_NADA;
	}

	if (ftruncate(fd_shm, sizeof(tipo_mapa)) == -1)
	{
		printf("[ERROR] No se ha podido truncar la memoria compartida\n");
		ret = 1;
		goto FREE_SHM;
	}

	mapa = mmap(NULL, sizeof(*mapa),
				PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

	if (mapa == NULL)
	{
		printf("[ERROR] No se ha podido mapear la memoria compartida\n");
	}

	// Construir mapa
	for (j = 0; j < MAPA_MAXY; j++)
	{
		for (i = 0; i < MAPA_MAXX; i++)
		{
			mapa_clean_casilla(mapa, j, i);
		}
	}

	// Construir naves y lanzarlas
	srand(time(NULL));
	for (i = 0; i < N_EQUIPOS; i++)
	{
		for (j = 0; j < N_NAVES; j++)
		{
			Coords r_coords = random_coords(mapa, i);

			nave.vida = VIDA_MAX;
			nave.posx = r_coords.x;
			nave.posy = r_coords.y;
			nave.equipo = i;
			nave.numNave = j;
			nave.viva = ALIVE;

			if (mapa_set_nave(mapa, nave) == -1)
			{
				printf("[ERROR] No se ha podido crear la nave\n");
				ret = 1;
				goto FREE_MAP;
			}
		}
	}

	// Comunicar al monitor que esta ready esto
	if ((ready = sem_open(READY_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
	{
		printf("[ERROR] Semaphore ready failed");
		goto FREE_MAP;
	}

	sem_post(ready);

	// Create pipes for jefes
	for (i = 0; i < N_EQUIPOS; i++)
	{
		if (pipe(jefe_pipe[i]) == -1)
		{
			printf("[ERROR] No se ha podido crear el pipe para la jefa\n");
			ret = 1;
			goto FREE_SEM;
		}
	}

	// Spawn jefes (one for each team)
	for (i = 0; i < N_EQUIPOS; i++)
	{
		teams[i] = fork();
		if (teams[i] < 0)
		{
			printf("[ERROR] Error creating jefe for team %d\n", i);
			ret = 1;
			goto FREE_PIPES;
		}
		else if (teams[i] == 0)
		{ // JEFE
			jefe_main(i, jefe_pipe[i]);
			goto FREE_PIPES;
		}
	}

	// Create message queue for the ships
	struct mq_attr attributes;
	mqd_t queue;

	attributes.mq_flags = 0;
	attributes.mq_maxmsg = 10;
	attributes.mq_curmsgs = 0;
	attributes.mq_msgsize = sizeof(Nave_orden);

	// Open the queue
	queue = mq_open(QUEUE_NAME,
					O_CREAT | O_RDONLY, /* This process is only going to send messages */
					S_IRUSR | S_IWUSR,  /* The user can read and write */
					&attributes);

	if (queue == (mqd_t)-1)
	{
		fprintf(stderr, "Error opening the queue\n");
		return -1;
	}

	sleep(2);

	// Main loop
	while (check == -1)
	{
		// Check if we are finished
		check = check_winner(mapa);

		// Get map tidy for next turn

		// Manda los turnos a los jefes
#ifdef DEBUG
		printf("Sending TURNO to all jefes\n");
#endif
		for (int i = 0; i < N_EQUIPOS; i++)
			write(jefe_pipe[i][1], "TURNO", strlen("TURNO"));

#ifdef DEBUG
		printf("Waiting to receive actions from ships\n");
#endif
		// Execute actions sent by ships
		for (int i = 0; i < N_NAVES; i++)
		{
			Nave_orden rec_test;
			receive_msg(queue, &rec_test);
			execute_order(mapa, rec_test);
		}

		sleep(5);
	}

	// Tell jefes it is over
	for (int i = 0; i < N_EQUIPOS; i++)
		write(jefe_pipe[i][1], "FIN", strlen("FIN"));

	//FREE_MQ:
	mq_close(queue);
	mq_unlink(QUEUE_NAME);

FREE_PIPES:
	// Liberar pipes de los jefasos
	for (i = 0; i < N_EQUIPOS; i++)
	{
		close(jefe_pipe[i][0]);
		close(jefe_pipe[i][1]);
	}

// Liberar semaforo ready
FREE_SEM:
	sem_close(ready);
	sem_unlink(READY_SEM);

// Liberar mapa
FREE_MAP:
	munmap(mapa, sizeof(*mapa));

FREE_SHM:
	shm_unlink(SHM_MAP_NAME);

// Salir sin liberar nada
FREE_NADA:
	return ret;
}