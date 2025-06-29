#include "protocolo_comunicacion_memoria.h"

t_buffer* buffer_serializado_crear_proceso_memoria(char* nombre_archivo_proceso){
    
    size_t length_nombre_arhivo = strlen(nombre_archivo_proceso);

    t_buffer *buffer = crear_buffer(
        sizeof(uint32_t) + length_nombre_arhivo + 1
    );

    buffer_add_string(buffer, length_nombre_arhivo, nombre_archivo_proceso);

    return buffer;
}

// Deserializar paquete de crear proceso
t_paquete_crear_proceso_memoria* deserializar_crear_proceso_memoria(t_buffer* buffer) {
    
    t_paquete_crear_proceso_memoria * paquete_crear_proceso_memoria = malloc(sizeof(t_paquete_crear_proceso_memoria));

    // Reseteo el offset
    buffer->offset = 0;
    uint32_t longitud = paquete_crear_proceso_memoria->length_nombre_archivo;
    paquete_crear_proceso_memoria->nombre_archivo = buffer_read_string(buffer, &longitud );

    return paquete_crear_proceso_memoria;
}
