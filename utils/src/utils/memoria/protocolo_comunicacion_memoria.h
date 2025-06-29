#ifndef PROTOCOLO_COMUNICACION_MEMORIA_H_
#define PROTOCOLO_COMUNICACION_MEMORIA_H_

#include <stdint.h>
#include "../protocolo.h"

typedef struct {
    uint8_t length_nombre_archivo;
    char *nombre_archivo;
} t_paquete_crear_proceso_memoria;

typedef struct {
    uint espacio_libre;
} t_paquete_espacio_libre_memoria;

typedef struct {
    uint8_t cantidad_linea_instrucciones;
    char *instrucciones;
} t_proceso;

t_buffer* buffer_serializado_crear_proceso_memoria(char* nombre_archivo_proceso);

t_paquete_crear_proceso_memoria* deserializar_crear_proceso_memoria(t_buffer* buffer);

t_buffer* buffer_serializado_espacio_libre_en_memoria();

t_paquete_espacio_libre_memoria* deserializar_espacio_libre_mememoria(t_buffer* buffer);

#endif