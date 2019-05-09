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

#include "mapa.h"

coords random_coords(tipo_mapa *mapa, int team)
{
	coords r_coords;
	int found_empty_space = 0;

	while (found_empty_space == 0) {
		r_coords.x = rand() % 10;
		r_coords.y = rand() % 10;

		if (team == 1){
			r_coords.x += 10;
		}
		else if (team == 2) {
			r_coords.y += 10;
		}
		else if (team == 3) {
			r_coords.x += 10;
			r_coords.y += 10;
		}

		if (mapa_is_casilla_vacia(mapa, r_coords.x, r_coords.y))
			found_empty_space = 1;
	}

	return r_coords;
}

int main()
{
	sem_t *ready = NULL;
	int ret = 0;
	int i, j;
	tipo_mapa *mapa = NULL;
	tipo_nave nave;
	pid_t teams[N_EQUIPOS];
	int fd_shm;
	int jefe_pipe[N_EQUIPOS][2];

	// Create shared memory for map
	fd_shm = shm_open(SHM_MAP_NAME,
		O_RDWR | O_CREAT | O_EXCL,
		S_IRUSR | S_IWUSR);

	if (fd_shm == -1) {
		printf("[ERROR] No se ha podido crear la memoria compartida\n");
		ret = 1;
		goto FREE_NADA;
	}

	if (ftruncate(fd_shm, sizeof(tipo_mapa)) == -1) {
		printf("[ERROR] No se ha podido truncar la memoria compartida\n");
		ret = 1;
		goto FREE_SHM;
	}

	mapa = mmap(NULL, sizeof(*mapa),
			PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

	if (mapa == NULL) {
		printf("[ERROR] No se ha podido mapear la memoria compartida\n");
	}

	// Construir mapa
	for(j=0;j<MAPA_MAXY;j++) {
		for(i=0;i<MAPA_MAXX;i++) {
			mapa_clean_casilla(mapa, j, i);
		}
	}

	// Construir naves y lanzarlas
	srand(time(NULL));
	for(i=0;i<N_EQUIPOS;i++) {
		for(j=0;j<N_NAVES;j++) {
			coords r_coords = random_coords(mapa, i);

			nave.vida = VIDA_MAX;
			nave.posx = r_coords.x;
			nave.posy = r_coords.y;
			nave.equipo = i;
			nave.numNave = j;
			nave.viva = ALIVE;

			if (mapa_set_nave(mapa, nave) == -1) {
				printf("[ERROR] No se ha podido crear la nave\n");
				ret = 1;
				goto FREE_MAP;
			}
		}
	}

	// Comunicar al monitor que esta ready esto
	if((ready = sem_open(READY_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0))
		== SEM_FAILED)
	{
		printf("[ERROR] Semaphore ready failed");
		goto FREE_MAP;
	}

	sem_post(ready);

	// Create pipes for jefes
	for (i = 0; i < N_EQUIPOS; i++) {
		if (pipe(jefe_pipe[i]) == -1) {
			printf("[ERROR] No se ha podido crear el pipe para la jefa\n");
			ret = 1;
			goto FREE_SEM;
		}
	}

	// Spawn jefes (one for each team)
	char buff[] = "Hola que tal";
	for (i = 0; i < N_EQUIPOS; i++) {
		teams[i] = fork();
		if (teams[i] < 0) {
			printf("[ERROR] Error creating jefe for team %d\n", i);
			ret = 1;
			goto FREE_ALL;
		}
		else if (teams[i] == 0) { // JEFE
			// Avisar de que estoy ready
			write(jefe_pipe[i][1], buff, strlen(buff));
			goto FREE_ALL;
		} else {
			memset(buff, 0, sizeof(buff));
			read(jefe_pipe[i][0], buff, sizeof(buff));
			printf("[%d] RECEIVED >>> %s\n", getpid(), buff);
		}
	}

	FREE_ALL:

	// Liberar pipes de los jefasos
	for (i = 0; i < N_EQUIPOS; i++) {
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