#ifndef KERNEL_H_
#define KERNEL_H_

#include <utils/hello.h>
#include <utils/socket.h>
#include <utils/auxiliar.h>
#include "recurso.h"

#include <time.h> //Para medir rafagas de ejecucion
#include <sys/time.h>
#include <unistd.h> //Usleep

#define ESTADOS 7

#define ENVIO 0
#define INTERRUPCION 1

//definicion de estructura pcb
typedef enum {
    NEW = 0,
    READY,
    EXECUTE,
    BLOCK,
    SUSP_READY,
    SUSP_BLOCK,
    EXIT
} ProcessState;

typedef struct {
    int pid;
    unsigned int pc;
    unsigned int size;
    unsigned int RAF;              // Ráfaga actual
    unsigned int UltRAF;           // Última ráfaga ejecutada
    ProcessState state;
    unsigned int ContTime[ESTADOS];  // Tiempo total por estado (en ms)
    unsigned int ContState[ESTADOS]; // Cantidad de veces por estado
} PCB;

typedef struct {
    PCB pcb;
    char *archivo_pseudocodigo;    // ruta al archivo fuente (simulado)
    unsigned int tamano_proceso;   // tamaño requerido en memoria
    struct PROCESO* next;
} PROCESO;

PROCESO* init_proc(const char* archivo, unsigned int size);
void agregar_a_cola_new(PROCESO* proceso);
void imprimir_cola_new();
void* atender_conexion_cpu(void* socket);
void* atender_conexion_io(void* socket);
void* atender_cliente_kernel(void* socket_cliente);
t_handshake* recibir_handshake(int cliente);

#endif /* KERNEL_H_ */
//o;
//    struct PROCESO* next;