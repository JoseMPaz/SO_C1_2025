#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <utils/hello.h>
#include <utils/socket.h>
#include <utils/auxiliar.h>
#include <utils/protocolo.h>
#include <utils/memoria/protocolo_comunicacion_memoria.h>
#include <stdio.h>

typedef struct {
    
    char* puerto_escucha;
    char* tam_memoria;
    char* tam_pagina;
    char* entradas_tabla;
    char* cantidad_niveles;
    char* retardo_memoria;
    char* path_swapfile;   
    char* retardo_swap;        
    char* log_level;               
    char* dump_path;

} t_config_memoria;

int atender_conexion_memoria(int socket_servidor, t_log* logger);
void* atender_cliente_memoria(void* parametros);

int leer_archivo_pseudocodigo(char*);

void* atender_cliente_memoria(void* parametros);

#endif /* MEMORIA_H_ */
