/**
 * @file naves.c
 * @author Catalin Rotaru (catalin.rotaru@estudiante.uam.es)
 * @author Antonio Solana (antonio.solana@estudiante.uam.es)
 * Grupo: 2212
 * @date
 * @brief
 */

#include "naves.h"

#define MSGSIZE 10
#define QUEUE_NAME "/naves_queue"

/****************************************************************/
/****************************************************************/
/****************************************************************/
mqd_t open_queue_system(){
	struct mq_attr attributes;
	mqd_t queue;

	attributes.mq_flags = 0;
	attributes.mq_maxmsg = 10;
	attributes.mq_curmsgs = 0;
	attributes.mq_msgsize = MSGSIZE;

	/*abrimos la cola*/
	queue = mq_open(QUEUE_NAME,
		O_CREAT | O_RDONLY, /* This process is only going to send messages */
		S_IRUSR | S_IWUSR, /* The user can read and write */
		&attributes);
	/*comprbamos si se ha creado de forma correcta*/
	if(queue == (mqd_t)-1) {
		fprintf (stderr, "Error opening the queue\n");
		return 0;
	}
	return queue;

	/*
	mq_close(queue);
	mq_unlink(QUEUE_NAME);
	*/

}

/****************************************************************/
/****************************************************************/
/****************************************************************/

int pipe_since_jefe(int tuberia[2]){
	tipo_mapa * mapa = NULL;
	int accion;
	/*comprbamos si se ha recibido de forma correcta la tuberia para poder realizar su lecturas*/
	if(tuberia == 0) return -1;

	/* leemos de memoria compartida el mapa */

	mapa = open_shared_memory();
	if(!mapa) return -1;
	/*ahora podemos obtener los recursos del mapa mediante mapa->*/


	/*en el canal [0] se permite la lectura y cerramos el canal [1]*/
	/*Acceso a la tubería en modo lectura [0]*/
	read(tuberia[0], &accion, sizeof(accion));
	close(tuberia[1]);

	/*mensaje puede ser ATACAR O MOVER_ALEATORIO*/
	if(accion == 1){
		/*si se recibe un 1 ATACAMOS*/
		/*equipo al que atacar */
		//mapa->casillas[posicion_y][posicion_x].equipo = random_equipo_nave();
		/* nave del equipo al que atacar */
		//mapa->info_naves[equipo][num_nave] = random_equipo_nave();
		/*nave_ataca();*/
	}

	else if(accion == 2){
		/*si se recibe un 2 MOVER_ALEATORIO*/

		mover_aleatorio();
	}
	else if (accion == 3){
		destruir_nave();
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
void nave_ataca(){
	char *aviso = NULL;
	mqd_t queue;

	queue = open_queue_system();
	if(queue == 0) return;

	strcpy(aviso,"ATACAR");
	if(mq_send(queue, (char *)&aviso, sizeof(aviso), 1) == -1) {
		fprintf (stderr, "Error sending message\n");
		exit(EXIT_FAILURE);
	}

	/* controlamos el paso de argumentos en sus respectivas funciones */
	/* puede ser que nos queramos atacar a nosotros mismos -> eso no puede suceder */
	/* si nos atacmos a nosotros mismos informamos de que esto no se puede hacer */
	/* buscamos una nave enemiga en el mapa */


}

/****************************************************************/
/****************************************************************/
/****************************************************************/
void mover_aleatorio(){
	char * aviso = NULL;

	//int posicion_x = -1, posicion_y = -1;

	mqd_t queue;

	queue = open_queue_system();
	if(queue == 0) return;

	/* obte a la posición(x,y) que se va a mover generada por el jefe
	posicion_x = random_posicion_x();
	if( posicion_x == -1)
		return -1;
	posicion_y= random_posicion_y();
	if( posicion_y == -1)
		return -1;
	queue = open_queue_system();
	if(queue == 0) return;
	*/
	/* enviamos un mensaje con la accion mover al proceso simulador */
	/* la comunicacion entre naves y el simulador se realiza mediante colas de mensajes */
	strcpy(aviso,"MOVER");
	if(mq_send(queue, (char *)&aviso, sizeof(aviso), 1) == -1) {
		fprintf (stderr, "Error sending message\n");
		exit(EXIT_FAILURE);
	}


	/*comprobamos que dicha casilla esta dentro del mapa*/
	/*comprobamos que la casilla este vacia*/
	/*movemos la nave a dicha casilla*/


}
/****************************************************************/
/****************************************************************/
/****************************************************************/
void destruir_nave(){
	char * aviso = NULL;
	mqd_t queue;

	queue = open_queue_system();
	if(queue == 0) return;

	strcpy(aviso,"MOVER");
	if(mq_send(queue, (char *)&aviso, sizeof(aviso), 1) == -1) {
		fprintf (stderr, "Error sending message\n");
		exit(EXIT_FAILURE);
	}

}


/****************************************************************/
/****************************************************************/
/****************************************************************/
int distancia_naves(int x, int y){
	int nave_posicion_x = -1;
	int nave_posicion_y = -1;
	int destino_x = -1, destino_y = -1;
	float distancia = -1;
	int equipo = -1;
	int num_nave = -1;
	tipo_nave nave;
	tipo_mapa * mapa = NULL;

	mapa = open_shared_memory();
	if(!mapa){
		printf("Error al abrir la memoria compartida en el proceso nave\n");
		return -1;
	}

	/* pasamos a equipo y a nave sus cosas correspondientes */

	equipo = mapa->casillas[x][y].equipo;
	num_nave = mapa->casillas[x][y].numNave;

	/* miramos a ver en que casilla se encuentra la nave */
	nave = mapa_get_nave(mapa,equipo,num_nave);


	nave_posicion_x = mapa->info_naves[nave.posx][nave.numNave];
	nave_posicion_y = mapa->info_naves[nave.posy][nave.numNave];


	/*distancia = sqrt((destino_x- origen_x)² + (destino_y- origen_y)²)*/
	destino_x = nave_posicion_x - x;
	destino_y = nave_posicion_y - y;

	distancia = sqrt((destino_x * destino_x) + (destino_y * destino_y));

	if (distancia <= ATAQUE_ALCANCE)
		return 1;
	else return -1;
}
