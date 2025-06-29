#include "mensaje_cadena.h"

void enviar_cadena ( char * cadena, int socket )
{
	int longitud_cadena = strlen (cadena) +  1/* Para el '/0' */;
	int bytes_a_enviar = sizeof(longitud_cadena) + longitud_cadena;
	void * mensaje_serializado = malloc (bytes_a_enviar);
	/*void * memcpy (void * destino, const void * fuente, size_t tamano);*/
	memcpy ( mensaje_serializado , &longitud_cadena, sizeof (longitud_cadena) );
	memcpy ( mensaje_serializado + sizeof (longitud_cadena), cadena, longitud_cadena );
	
	send (socket, mensaje_serializado, bytes_a_enviar, 0);//Envio el mensaje serializado desde io a kernel
	
	free (mensaje_serializado);
	
	return;
}


char * recibir_cadena ( int socket )
{
	int longitud_cadena;
	void * cadena = NULL;
	
	recv (socket, &longitud_cadena, sizeof(int), MSG_WAITALL);//recibe longitud de cadena
	cadena = malloc (longitud_cadena);
	recv (socket , cadena, longitud_cadena, MSG_WAITALL);
	
	return (char *) cadena;
}

