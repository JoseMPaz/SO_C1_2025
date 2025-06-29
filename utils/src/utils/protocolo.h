#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <commons/log.h>

typedef struct
{
    uint32_t size;   // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void *stream;    // Payload
} t_buffer;

typedef enum {HANDSHAKE, DESCONEXION, PERSONA, MENSAJE, INSTRUCCION, 
    
    // tipos de operacion
    CREAR_PROCESO,
    SUSPENDER_PROCESO,
    DESUSPENDER_PROCESO,
    FINALZAR_PROCESO,
    LEER_TABLA_PAGINA,
    LEER_POSICION_MEMORIA,
    ESCRIBIR_POSICION_MEMORIA,
    LEER_PAGINA,
    ACTUALIZAR_PAGINA,
    VOLCAR_MEMORIA,
    CANTIDAD_ESPACIO_LIBRE,
    EXITO,
    ERROR
} t_operacion;

typedef struct {
    t_operacion codigo_operacion;
    t_buffer* buffer;
} t_paquete;

typedef struct
{
    uint32_t dni;
    uint8_t edad;
    uint32_t pasaporte;
    uint32_t nombre_length;
    char *nombre;
} t_persona;

typedef struct
{
    uint32_t longitud;
    char *texto;
} t_mensaje;

typedef struct
{
    uint32_t pid;
    uint32_t pc;
} t_instruccion;

typedef enum {CPU, IO} t_recurso;
typedef struct
{
    uint8_t codigo;
    t_recurso recurso;
    t_mensaje nombre;
} t_handshake;



t_buffer* crear_buffer(uint32_t);
void liberar_buffer(t_buffer*);
void buffer_add_uint32(t_buffer*, uint32_t);
void buffer_add_uint8(t_buffer*, uint8_t);
void buffer_add_string(t_buffer*, uint32_t, char*);
void buffer_add(t_buffer*, void*, uint32_t);

uint32_t buffer_read_uint32(t_buffer*);
uint8_t buffer_read_uint8(t_buffer*);
char *buffer_read_string(t_buffer*, uint32_t*);
void buffer_read(t_buffer*, void*, uint32_t);

t_paquete* crear_paquete(t_operacion, t_buffer*);
void* serializar_paquete(t_paquete*);
void enviar_paquete(t_buffer*, int, t_operacion);
void recibir_paquete(int, t_log*);

t_buffer* serializar_persona(t_persona*);
t_persona* deserializar_persona(t_buffer*);

t_buffer* serializar_mensaje(t_mensaje*);
t_mensaje* deserializar_mensaje(t_buffer*);

t_buffer* serializar_instruccion(t_instruccion*);
t_instruccion* deserializar_instruccion(t_buffer*);

t_buffer* serializar_handshake(t_handshake*);
t_handshake* deserializar_handshake(t_buffer*);

void enviar_handshake(int, t_recurso, char*);
void enviar_mensaje(int, char*);

#endif /* PROTOCOLO_H_ */