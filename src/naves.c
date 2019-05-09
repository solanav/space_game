/**
 * @file naves.c
 * @author Catalin Rotaru (catalin.rotaru@estudiante.uam.es)
 * @author Antonio Solana (antonio.solana@estudiante.uam.es)
 * Grupo: 2212
 * @date
 * @brief
 */
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "naves.h"
#include "mapa.h"

int naves_main(int id, int team, int tuberia_nave[2])
{
	mqd_t queue;
	struct mq_attr attributes;
	char accion_recibida[32] = {0};

	// Create and open the queue
	attributes.mq_flags = 0;
	attributes.mq_maxmsg = 10;
	attributes.mq_curmsgs = 0;
	attributes.mq_msgsize = sizeof(Nave_orden);

	queue = mq_open(QUEUE_NAME,
					O_CREAT | O_RDWR,  /* This process is only going to send messages */
					S_IRUSR | S_IWUSR, /* The user can read and write */
					&attributes);

	if (queue == (mqd_t)-1)
	{
		printf("[ERROR] Error opening the queue\n");
		printf("%s", strerror(errno));
		return -1;
	}

	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if (tuberia_nave == 0)
		return -1;

	/* Leemos el mensaje que nos ha enviado el jefe */
	read(tuberia_nave[0], &accion_recibida, sizeof(accion_recibida));
	close(tuberia_nave[1]);

#ifdef DEBUG
	printf("\t<NAVE %d> Accion recibida: %c\n", id, accion_recibida[0]);
#endif

	// We send the corresponding order to simulador so it executes it
	Nave_orden orden;
	orden.id = id;
	orden.team = team;
	orden.dir = rand() % 4;

	if (accion_recibida[0] == 'M')
	{ // MOVE
		orden.orden = 0;
		send_msg(queue, orden);
	}
	else if (accion_recibida[0] == 'A')
	{ // ATTACK
		orden.orden = 1;
		send_msg(queue, orden);
	}
	else
	{
		printf("[ERROR] Failed to find the action sent to ship\n");
	}

	mq_close(queue);
	exit(EXIT_SUCCESS);
}

int send_msg(mqd_t queue, Nave_orden msg)
{
	if (mq_send(queue, (char *)&msg, sizeof(msg), 1) == -1)
	{
		printf("[ERROR] Cannot send a message through the queue\n");
		printf("%s", strerror(errno));
	}

	return EXIT_SUCCESS;
}