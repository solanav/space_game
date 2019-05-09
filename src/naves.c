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

int main()
{
	mqd_t queue;
	struct mq_attr attributes;

	attributes.mq_flags = 0;
	attributes.mq_maxmsg = 10;
	attributes.mq_curmsgs = 0;
	attributes.mq_msgsize = sizeof(Nave_orden);

	// Open the queue
	queue = mq_open(QUEUE_NAME,
					O_CREAT | O_RDWR, /* This process is only going to send messages */
					S_IRUSR | S_IWUSR,  /* The user can read and write */
					&attributes);

	if (queue == (mqd_t)-1)
	{
		fprintf(stderr, "Error opening the queue\n");
		printf("%s", strerror(errno));
		return -1;
	}

	Nave_orden orden_test;
	orden_test.orden = 0;
	orden_test.objective = 69;
	send_msg(queue, orden_test);

	getchar();

	mq_close(queue);

	return 1;
}

int send_msg(mqd_t queue, Nave_orden msg)
{
	if (mq_send(queue, (char *) &msg, sizeof(msg), 1) == -1) {
		printf("[ERROR] Cannot send a message through the queue\n");
		printf("%s", strerror(errno));
	}

	return EXIT_SUCCESS;
}