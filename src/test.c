/********************************************************************/
/********************************************************************/
/********************************************************************/
int simulador_atacar_nave()
{
	/* Saber si la casilla se encuentra en el mapa */
	int posy = -1, posx = -1;
	tipo_mapa *mapa = NULL;
	int distancia = -1;
	mapa = open_shared_memory();
	if (!mapa)
		return 0;
	/* obtenemos la posicion a la que nos queremos mover */
	posx = random_posicion_x_y();
	posy = random_posicion_x_y();

	/* requisitos1: casilla dentro del rango del mapa
     casilla debe estar dento de la distancia de ataque
 */
	mapa->casillas[posx][posy] = mapa_get_casilla(mapa, posy, posx);
	if (mapa->casillas[posx][posy] >= MAPA_MAXX || mapa->casillas[posx][posy] >= MAPA_MAXY)
	{
		printf("Te quieres mover a una casilla que no se encuentra en el mapa\n");
	}
	distancia = mapa_get_distancia(mapa, posy, posx, targety, targetx);
	if (distancia == -1)
	{
		printf("No se ha obtenido obtener la distancia entre naves\n");
		return 0;
	}

	/* comprobamos si la casilla se encuentra dentro del mapa */
	/* obtenermos la distancia de la nave que va a efecturar el ataque a la nave que va a recibir el ataque */

	if (distancia <= ATAQUE_ALCANCE)
	{
		/* Accion: atacar la casilla objetivo (mapa_send_misil) */
		mapa_send_misil(mapa, posy, posx, targety, targetx);
		/*al enviar el misil comprobamos miramos las posibles consecuencias*/
		/*mirar si la nave objetivo se ha movido -> comprobar que en la casilla destino haya una nave y no sea de nuestro equipo*/
		// mapa_get_casilla()
		/*si no hay nave marcamos como disparo agua y mapa_set_symbol (SYMB_AGUA)*/
		/* si hay nave y no es de nuestro equipo */
		/* restamos vida */
		/* vida <=0 ,matamos a la nave , set_num_naves -1
     1º marcar la casilla objetivo con SYMB_DESTRUIDO
     2º ordenamos destruir la nave
     3º se actualiza la Información del mapa relativo al numero de naves vivas (mapa_set_num_naves) y el estado de la naves
         (nivel de vida, atributo, vida...)
     tipo_nave.vida = tipo_nave.vida - ATAQUE_DANO;
     if(tipo_nave.vida <= 0)
      execl(jefe, destruir la nave) kill(pid[equipo][nave])
      mapa_set_num_naves() restamos una nave al total de naves del equipo al que hemos eliminado la nave
      if(numero de naves del equipo[i] == 0)
       mapa_num_equipos --;(maoa_set_num_naves)

     comprobar que no se hayan matado a todas las naves del equipo-> si estan todas muertas eliminamos al equipo
     volvemos a comprobar que sigan equipos con naves
    */

		/*
    if(vida > 0)
    1º marcamos la casilla objetivo como SYMB_TOCADO
    mapa_set_symbol(mapa,posy,posx,SYMB_TOCADO)

    */
	}
	return 1;
}