#ifndef _RECURSO_H_
#define _RECURSO_H_

#include <pthread.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include <utils/socket.h>
#include <utils/mensaje_cadena.h>

#define DISPATCH 0
#define INTERRUPT 1


extern t_list * recursos_io;
extern t_list * recursos_cpu;

void * esperar_recursos_io (void * arg);
void * esperar_recursos_cpu (void * arg);

#endif
