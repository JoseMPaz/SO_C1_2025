#include "protocolo.h"

// Crea un buffer vacío de tamaño size y offset 0
t_buffer* crear_buffer(uint32_t tamaño)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

    buffer->size = tamaño;
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
    
    return buffer;
}

// Libera la memoria asociada al buffer
void liberar_buffer(t_buffer *buffer)
{
    free(buffer->stream);
    free(buffer);
}

// Agrega un stream al buffer en la posición actual y avanza el offset
void buffer_add(t_buffer *buffer, void *data, uint32_t size)
{
    memcpy(buffer->stream + buffer->offset, data, size);
    buffer->offset += size;
}

// Agrega un uint32_t al buffer
void buffer_add_uint32(t_buffer* buffer, uint32_t data)
{
    memcpy(buffer->stream + buffer->offset, &data, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

// Agrega un uint8_t al buffer
void buffer_add_uint8(t_buffer* buffer, uint8_t data)
{
    memcpy(buffer->stream + buffer->offset, &data, sizeof(uint8_t));
    buffer->offset += sizeof(uint8_t);
}

// Agrega string al buffer con un uint32_t adelante indicando su longitud
void buffer_add_string(t_buffer* buffer, uint32_t length, char *string)
{
    // Para el nombre primero mandamos el tamaño y luego el texto en sí:
    memcpy(buffer->stream + buffer->offset, &length, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
    memcpy(buffer->stream + buffer->offset, string, length + 1); // Copio tambien el centinela

    //uint8_t hand = *(uint8_t *)(buffer->stream);
    //uint32_t mensaje_length = *(uint32_t *)(buffer->stream);
    //char *msj = (char *)(buffer->stream + sizeof(uint32_t));
}

// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
void buffer_read(t_buffer *buffer, void *data, uint32_t size)
{
    memcpy(data, buffer->stream + buffer->offset, size);
    buffer->offset += size;
}

// Lee un uint32_t del buffer y avanza el offset
uint32_t buffer_read_uint32(t_buffer* buffer)
{
    uint32_t valor;

    //printf("Offset antes de leer: %d\n", buffer->offset);
    //printf("Contenido del buffer antes de leer: %u\n", *((uint32_t*)(buffer->stream + buffer->offset)));

    memcpy(&valor, buffer->stream + buffer->offset, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    //printf("Valor leído: %u\n", valor);
    //printf("Nuevo offset: %d\n", buffer->offset);

    return valor;
}

// Lee un uint8_t del buffer y avanza el offset
uint8_t buffer_read_uint8(t_buffer* buffer)
{
    uint8_t valor;

    //printf("Offset antes de leer: %d\n", buffer->offset);
    //printf("Contenido del buffer antes de leer: %u\n", *((uint8_t*)(buffer->stream + buffer->offset)));

    memcpy(&valor, buffer->stream + buffer->offset, sizeof(uint8_t));
    buffer->offset += sizeof(uint8_t);

    //printf("Valor leído: %u\n", valor);
    //printf("Nuevo offset: %d\n", buffer->offset);

    return valor;
}

// Lee un string y su longitud del buffer y avanza el offset
char* buffer_read_string(t_buffer* buffer, uint32_t* length)
{
    //printf("Offset antes de leer: %d\n", buffer->offset);
    //printf("Contenido del buffer antes de leer: %u\n", *((uint32_t*)(buffer->stream + buffer->offset)));

    memcpy(length, buffer->stream + buffer->offset, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    //printf("Valor leído: %u\n", *length);
    //printf("Nuevo offset: %d\n", buffer->offset);
    
    //printf("Offset antes de leer: %d\n", buffer->offset);
    //printf("Contenido del buffer antes de leer: %s\n", (char*)(buffer->stream + buffer->offset));

    // Leo el string que ya viene con el centinela
    char* string = malloc(*length + 1);
    //string = calloc(*length, sizeof(char));

    // El stream ya viene con el '\0' por eso leo *length + 1 y ya lo almaceno en string
    // Otra alternativa es agregarlo a mano pero en ese caso no tiene mucho sentido enviarlo
    memcpy(string, buffer->stream + buffer->offset, *length + 1);
    //string[*length] = '\0';

    //printf("Valor leído: %s\n", string);
    //printf("Nuevo offset: %d\n", buffer->offset);

    return string;
}

// Crea un paquete
t_paquete* crear_paquete(t_operacion codigo, t_buffer* buffer)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	paquete->buffer = buffer;
	return paquete;
}

// Serializa el paquete a enviar
void* serializar_paquete(t_paquete* paquete){
    
    // Armamos el stream a enviar
    void* a_enviar = malloc(paquete->buffer->size + sizeof(t_operacion) + sizeof(uint32_t));
    int offset = 0;
        
    memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(t_operacion));
    offset += sizeof(t_operacion);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    return a_enviar;
}

// Envia paquete con el buffer creado
void enviar_paquete(t_buffer* buffer, int unSocket, t_operacion codigo)
{
    // Creo el paquete
    t_paquete* paquete = crear_paquete(codigo, buffer);

    // Serializo el paquete
    void* a_enviar = serializar_paquete(paquete);

    // Envio el paquete
    send(unSocket, a_enviar, buffer->size + sizeof(t_operacion) + sizeof(uint32_t), 0);

    // Liberamos la memoria
    liberar_buffer(buffer);
    free(a_enviar);
    free(paquete);
}

/*
// Recibe un paquete
void recibir_paquete(int unSocket, t_log* logger){

    log_info(logger, "recibir_paquete");

    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    // Primero recibimos el codigo de operacion
    recv(unSocket, &(paquete->codigo_operacion), sizeof(t_operacion), 0);

    // Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
    recv(unSocket, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(unSocket, paquete->buffer->stream, paquete->buffer->size, 0);

    // Ahora en función del código recibido procedemos a deserializar el resto
    switch(paquete->codigo_operacion) {
        
        case PERSONA:
            
            t_persona* persona = deserializar_persona(paquete->buffer);
            
            log_info(logger, "PERSONA");
            log_info(logger, "DNI: %u", persona->dni);
            log_info(logger, "Edad: %u", persona->edad);
            log_info(logger, "Pasaporte: %u", persona->pasaporte);
            log_info(logger, "Nombre (%u caracteres): %s", persona->nombre_length, persona->nombre);
            
            free(persona);
            break;
            
        case MENSAJE:

            t_mensaje* mensaje = deserializar_mensaje(paquete->buffer);
            
            log_info(logger, "MENSAJE");
            log_info(logger, "Mensaje (%u caracteres): %s", mensaje->mensaje_length, mensaje->texto);
            
            free(mensaje);
            break;
        
        case DESCONEXION:

			log_error(logger, "el cliente se desconecto. Terminando servidor");
			//return NULL;
    }

    // Liberamos memoria
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
*/

// Agrega una persona al buffer
t_buffer* serializar_persona(t_persona* persona) {
    
    t_buffer *buffer = crear_buffer(
      sizeof(uint32_t) * 2 +                        // DNI y Pasaporte
      sizeof(uint8_t) +                             // Edad
      sizeof(uint32_t) + persona->nombre_length + 1 // Longitud del nombre, y el propio nombre mas el centinela
    );

    buffer_add_uint32(buffer, persona->dni);
    buffer_add_uint8(buffer, persona->edad);
    buffer_add_uint32(buffer, persona->pasaporte);
    buffer_add_string(buffer, persona->nombre_length, persona->nombre);

    return buffer;
}

// Agrega un mensaje al buffer
t_buffer* serializar_mensaje(t_mensaje* mensaje) {
    
    t_buffer *buffer = crear_buffer(
        sizeof(uint32_t) + mensaje->longitud + 1 // Longitud del nombre, y el propio nombre mas el centinela
    );

    buffer_add_string(buffer, mensaje->longitud, mensaje->texto);

    return buffer;
}

// Agrega una persona al buffer
t_buffer* serializar_instruccion(t_instruccion* instruccion) {
    
    t_buffer *buffer = crear_buffer(
      sizeof(uint32_t) * 2 // pid y pc
    );

    buffer_add_uint32(buffer, instruccion->pid);
    buffer_add_uint32(buffer, instruccion->pc);

    return buffer;
}

// Deserializa una persona
t_persona* deserializar_persona(t_buffer* buffer) {
    
    t_persona* persona = malloc(sizeof(t_persona));

    // Reseteo el offset
    buffer->offset = 0;
    
    persona->dni = buffer_read_uint32(buffer);
    persona->edad = buffer_read_uint8(buffer);
    persona->pasaporte = buffer_read_uint32(buffer);

    // Pasamos por referencia la longitud para que la función nos devuelva ahí el tamaño del string
    persona->nombre = buffer_read_string(buffer, &persona->nombre_length);

    return persona;
}

// Deserializa un mensaje
t_mensaje* deserializar_mensaje(t_buffer* buffer) {
    
    t_mensaje* mensaje = malloc(sizeof(t_mensaje));

    // Reseteo el offset
    buffer->offset = 0;

    // Pasamos por referencia la longitud para que la función nos devuelva ahí el tamaño del string
    mensaje->texto = buffer_read_string(buffer, &mensaje->longitud);

    return mensaje;
}

// Deserializa una persona
t_instruccion* deserializar_instruccion(t_buffer* buffer) {
    
    t_instruccion* instruccion = malloc(sizeof(t_instruccion));

    // Reseteo el offset
    buffer->offset = 0;
    
    instruccion->pid = buffer_read_uint32(buffer);
    instruccion->pc = buffer_read_uint32(buffer);

    return instruccion;
}

// Agrega una persona al buffer
t_buffer* serializar_handshake(t_handshake* handshake) {
    
    t_buffer *buffer = crear_buffer(
      sizeof(uint8_t) +                                 // codigo handshake
      sizeof(t_recurso) +                               // tipo de recurso
      sizeof(uint32_t) + handshake->nombre.longitud + 1 // Longitud del mensaje, y el propio nombre mas el centinela
    );

    buffer_add_uint8(buffer, handshake->codigo);
    buffer_add(buffer, &handshake->recurso, sizeof(t_recurso));
    buffer_add_string(buffer, handshake->nombre.longitud, handshake->nombre.texto);

    return buffer;
}

// Deserializa una persona
t_handshake* deserializar_handshake(t_buffer* buffer) {
    
    t_handshake* handshake = malloc(sizeof(t_handshake));
    t_recurso* recurso = malloc(sizeof(t_recurso));

    // Reseteo el offset
    buffer->offset = 0;
    
    handshake->codigo = buffer_read_uint8(buffer);
    
    buffer_read(buffer, recurso, sizeof(t_recurso));
    handshake->recurso = *recurso;

    // Pasamos por referencia la longitud para que la función nos devuelva ahí el tamaño del string
    handshake->nombre.texto = buffer_read_string(buffer, &handshake->nombre.longitud);

    return handshake;
}

void enviar_handshake(int socket, t_recurso recurso, char* nombre){

        t_handshake handshake;
            
        handshake.codigo = 99,
        handshake.recurso = IO,
        handshake.nombre.longitud = strlen(nombre);
        // Asignar memoria dinámica y copiar el nombre
        handshake.nombre.texto = (char*)malloc(handshake.nombre.longitud + 1);
        strcpy(handshake.nombre.texto, nombre);
        
        // Serializo el handshake
        t_buffer* buffer = serializar_handshake(&handshake);
        
        uint8_t codigo = *(uint8_t *)(buffer->stream);
        t_recurso recurso2 = *(t_recurso*)(buffer->stream + sizeof(uint8_t));
        uint32_t longitud = *(uint32_t *)(buffer->stream + sizeof(uint8_t) + sizeof(t_recurso));
        char *msj = (char *)(buffer->stream + sizeof(uint8_t) + sizeof(t_recurso) + sizeof(uint32_t));
        printf("Codigo: %u, Recurso: %d, Nombre (%u caracteres): %s\n", codigo, recurso2, longitud, msj);
        
        // Envio el handshake
        enviar_paquete(buffer, socket, HANDSHAKE);

        // Libero memoria
        free(handshake.nombre.texto); 
}

void enviar_mensaje(int socket, char* texto){

    t_mensaje mensaje;

    // Obtengo la longitud
    mensaje.longitud = strlen(texto); 

    // Asignar memoria dinámica y copiar el mensaje
    mensaje.texto = (char*)malloc(mensaje.longitud + 1);
    strcpy(mensaje.texto, texto);
    
    // Serializo el mensaje
    t_buffer* buffer = serializar_mensaje(&mensaje);
    
    /*
    uint32_t longitud = *(uint32_t *)(buffer->stream);
    char *msj = (char *)(buffer->stream + sizeof(uint32_t));
    printf("Mensaje (%u caracteres): %s\n", longitud, msj);
    */

    // Envio el mensaje
    enviar_paquete(buffer, socket, MENSAJE);
}