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

int naves_main(int id, int tuberia_nave[2])
{
	mqd_t queue;
	struct mq_attr attributes;
	char accion_recibida[32] = "X";

	// Create and open the queue
	attributes.mq_flags = 0;
	attributes.mq_maxmsg = 10;
	attributes.mq_curmsgs = 0;
	attributes.mq_msgsize = sizeof(Nave_orden);

	queue = mq_open(QUEUE_NAME,
					O_CREAT | O_RDWR, /* This process is only going to send messages */
					S_IRUSR | S_IWUSR,  /* The user can read and write */
					&attributes);

	if (queue == (mqd_t)-1)
	{
		printf("[ERROR] Error opening the queue\n");
		printf("%s", strerror(errno));
		return -1;
	}

	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if(tuberia_nave == 0)
		return -1;

	/* Leemos el mensaje que nos ha enviado el jefe */
	read(tuberia_nave[0], &accion_recibida, sizeof(accion_recibida));
	close(tuberia_nave[1]);

#ifdef DEBUG
	printf("<NAVE %d> Accion recibida: %s\n", id, accion_recibida);
#endif

	// We send the corresponding order to simulador so it executes it
	if (strcmp(accion_recibida, "M") == 0) { // MOVE
		Nave_orden orden_test;
		orden_test.orden = 0;
		orden_test.origen.x = 5;
		orden_test.origen.y = 6;
		orden_test.destino.x = 7;
		orden_test.destino.y = 8;

		send_msg(queue, orden_test);
	}
	else if (strcmp(accion_recibida, "A") == 0) { // ATTACK
		Nave_orden orden_test;
		orden_test.orden = 1;
		orden_test.origen.x = 5;
		orden_test.origen.y = 6;
		orden_test.destino.x = 7;
		orden_test.destino.y = 8;

		send_msg(queue, orden_test);
	}

	mq_close(queue);
	exit(EXIT_SUCCESS);
}

int send_msg(mqd_t queue, Nave_orden msg)
{
	if (mq_send(queue, (char *) &msg, sizeof(msg), 1) == -1) {
		printf("[ERROR] Cannot send a message through the queue\n");
		printf("%s", strerror(errno));
	}

	return EXIT_SUCCESS;
}