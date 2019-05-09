#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

#include "simulador.h"
#include "gamescreen.h"
#include "mapa.h"

tipo_mapa *mapa_open() {
	tipo_mapa *mapa = NULL;
	int fd_shm = 0;

	fd_shm = shm_open(SHM_MAP_NAME, O_RDONLY, 0);
	if (fd_shm == -1) {
		printf("[ERROR] Failed to open map\n");
		exit(EXIT_FAILURE);
	}

	mapa = mmap(NULL, sizeof(*mapa), PROT_READ, MAP_SHARED, fd_shm, 0);
	if (mapa == MAP_FAILED) {
		printf("[ERROR] Mapa could not be mapped\n");
		exit(EXIT_FAILURE);
	}

	return mapa;
}

void mapa_print(tipo_mapa *mapa)
{
	int i,j;

	for(j=0;j<MAPA_MAXY;j++) {
		for(i=0;i<MAPA_MAXX;i++) {
			screen_addch(j, i, mapa_get_symbol(mapa, j, i));
			screen_refresh();
		}
	}
}

int main()
{
	tipo_mapa *mapa = NULL;
	sem_t *ready = NULL;

	// Active wait for the semaphore to open
	if ((ready = sem_open(READY_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0))
		== SEM_FAILED)
	{
		printf("[ERROR] Failed to open ready semaphore\n");
		goto FREE_SEM;
	}

	sem_wait(ready);

	mapa = mapa_open();

	// Get screen ready
	screen_init();

	while (1) {
		mapa_print(mapa);
		sleep(1);
	}

	screen_end();

	FREE_SEM:
	sem_close(ready);

	exit(EXIT_SUCCESS);
}
