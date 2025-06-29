#ifndef SOCKET_H_
#define SOCKET_H_

//#include<stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
//#include<commons/collections/list.h>
#include <string.h>
//#include<assert.h>
#include <commons/error.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include "protocolo.h"

// Estructura auxiliar para poder pasar dos parametros a la vez a pthread_create
/*
typedef struct{
    int socket_servidor;
    char* tipo_conexion;
    t_log* logger;
} parametros_conexion;
*/
typedef struct{
    int* socket_envio;
	int* socket_interrupcion;
    t_log* logger;
} parametros_conexion_cpu;

typedef struct{
    int* socket_cliente;
    t_log* logger;
} parametros_conexion;

typedef struct
{
	int socket;
	bool disponible;
	char * nombre_io;
}t_recurso_io;

typedef struct
{
	int socket_envio;
	int socket_interrupcion;
	bool disponible;
	char * nombre_cpu;
}t_recurso_cpu;


int iniciar_servidor(char*, t_log*);
int* esperar_cliente(int, t_log*);
t_operacion recibir_operacion(int);
void recibir_buffer(int, t_paquete*); 
void handshake_server(int, t_log*);
void* atender_cliente(void*);
//void iniciar_servidor(char*, char*, t_log*);
void eliminar_paquete(t_paquete*);


int crear_conexion(char *, char*);
void handshake_cliente(int, t_log*);
void liberar_conexion(int);
int iniciar_conexion(char*, char*, t_log*);


#endif /* SOCKET_H_ */
