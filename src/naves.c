/**
 * @file naves.c
 * @author Catalin Rotaru (catalin.rotaru@estudiante.uam.es)
 * @author Antonio Solana (antonio.solana@estudiante.uam.es)
 * Grupo: 2212
 * @date
 * @brief
 */

#include "simulador.h"
#include "naves.h"
#include "jefe.h"
#include "mapa.h"

#define MSGSIZE 10
/****************************************************************/
/****************************************************************/
/****************************************************************/
int pipe_since_jefe(int tuberia[2]){
	int accion;
	int nave_a_atacar = 0;
	int posicion_x = -1, posicion_y = -1;
	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if(tuberia == 0) return -1;

	/*en el canal [0] se permite la lectura y cerramos el canal [1]*/
	/*Acceso a la tubería en modo lectura [0]*/
	read(tuberia[0], &accion, sizeof(accion));
	close(tuberia[1]);

	/*mensaje puede ser ATACAR O MOVER_ALEATORIO*/

	if(strcmp(accion,1) == 0){
		/*si se recibe un 1 ATACAMOS*/
		equipo_a_atacar = random_equipo();
		nave_a_atacar = random_nave();
		atacar_nave(equipo_a_atacar,nave_a_atacar);
	}
	else{
		printf("Se ha recibido una accion no válida\n");
		exit(EXIT_FAILURE);
	}

	if(strcmp(accion,2) == 0){
		/*si se recibe un 2 MOVER_ALEATORIO*/
		posicion_x = random_posicion_x();
		if( posicion_x == -1)
			return exit(EXIT_FAILURE);
		posicion_y= random_posicion_y();
		if( posicion_y == -1)
			return exit(EXIT_FAILURE);

		mover_aleatorio(posicion_x,posicion_y);
	}
	else{
		printf("Se ha recibido una accion no válida\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}


/****************************************************************/
/****************************************************************/
/****************************************************************/
void atacar_nave(int equipo_a_atacar,int nave_a_atacar){
	struct mq_attr attributes;
	mqd_t queue;
	char cola_mensaje[MSGSIZE] = "/";

	attributes.mq_flags = 0;
  	attributes.mq_maxmsg = 10;
  	attributes.mq_curmsgs = 0;
  	attributes.mq_msgsize = MSGSIZE;

	/*abrimos la cola*/
	queue = mq_open(args[3],
		O_CREAT | O_RDONLY, /* This process is only going to send messages */
		S_IRUSR | S_IWUSR, /* The user can read and write */
		&attributes);
	/*comprbamos si se ha creado de forma correcta*/
	if(queue == (mqd_t)-1) {
		fprintf (stderr, "Error opening the queue\n");
		return EXIT_FAILURE;
	}

	strcpy(msg.aviso,"ATACAR");
	if(mq_send(queue, (char *)&msg, sizeof(msg), 1) == -1) {
		fprintf (stderr, "Error sending message\n");
		return EXIT_FAILURE;
	}


	/* controlamos el paso de argumentos en sus respectivas funciones */

	/* puede ser que nos queramos atacar a nosotros mismos -> eso no puede suceder */
	/* si nos atacmos a nosotros mismos informamos de que esto no se puede hacer */

	/* buscamos una nave enemiga en el mapa */


}

/****************************************************************/
/****************************************************************/
/****************************************************************/
void mover_aleatorio(int x, int y){
	/*cola de mensajes del sistema */
	struct mq_attr attributes;
	mqd_t queue;
	char cola_mensaje[MSGSIZE] = "/";

	attributes.mq_flags = 0;
  	attributes.mq_maxmsg = 10;
  	attributes.mq_curmsgs = 0;
  	attributes.mq_msgsize = MSGSIZE;

	/*abrimos la cola*/
	queue = mq_open(args[3],
		O_CREAT | O_RDONLY, /* This process is only going to send messages */
		S_IRUSR | S_IWUSR, /* The user can read and write */
		&attributes);
	/*comprbamos si se ha creado de forma correcta*/
	if(queue == (mqd_t)-1) {
		fprintf (stderr, "Error opening the queue\n");
		return EXIT_FAILURE;
	}

	/*comprobamos que dicha casilla esta dentro del mapa*/
	if(x <= MAPA_MAXX && y <= MAPA_MAXY){
		/*comprobamos que la casilla este vacia*/
		if(mapa_is_casilla_vacia(mapa,y,x) == TRUE)
		/*movemos la nave a dicha casilla*/
		mapa_set_nave(tipo_mapa *mapa, tipo_nave nave);

		/* enviamos un mensaje con la accion mover al proceso simulador */
		/* la comunicacion entre naves y el simulador se realiza mediante colas de mensajes */
		strcpy(msg.aviso,"MOVER");
		if(mq_send(queue, (char *)&msg, sizeof(msg), 1) == -1) {
			fprintf (stderr, "Error sending message\n");
			return EXIT_FAILURE;
		}
	}

	mq_close(cola_mensaje);
	mq_unlink(cola_mensaje);
}

void finaliza(){

}
