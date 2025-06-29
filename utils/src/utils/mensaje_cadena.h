#ifndef _MENSAJE_CADENA_H_
#define _MENSAJE_CADENA_H_

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <string.h>
#include <commons/error.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <stdbool.h>

void enviar_cadena ( char * cadena, int socket );
char * recibir_cadena ( int socket );

#endif
