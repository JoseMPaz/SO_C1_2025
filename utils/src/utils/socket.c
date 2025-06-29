#include"socket.h"

//SERVIDOR

int iniciar_servidor(char* puerto, t_log* logger)
{
    int err;
    
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
                        		 servinfo->ai_socktype,
                        		 servinfo->ai_protocol);

	if(socket_servidor == -1){
		 error_show("No se pudo crear socket del servidor");
		 abort();
	}

	// Configuro para que varios socket se puedan bindear al mismo puerto
	err = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	
	// Asociamos el socket a un puerto
	err = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	if (err == -1) {
    	error_show("No se pudo hacer bind");
    	abort();
	}

	// Escuchamos las conexiones entrantes
    err = listen(socket_servidor, SOMAXCONN);

	if (err == -1) {
    	error_show("No se pudo hacer listen");
    	abort();
	}

	freeaddrinfo(servinfo);
	log_info(logger, "Listo para escuchar cliente");

	return socket_servidor;
}



/*
void* esperar_cliente(void* parametros)
{
	parametros_conexion* argumentos = (parametros_conexion*) parametros;
	int socket_servidor = argumentos->socket_servidor;
	char* tipo_conexion = argumentos->tipo_conexion; 
	t_log* logger = argumentos->logger;
	free(parametros);

	log_info(logger, "esperar_cliente");
	
	
	// Creamos un hilo para atender cada conexion entrante
	while (1) {
        pthread_t thread;
        int *socket_cliente = malloc(sizeof(int));
        *socket_cliente = accept(socket_servidor, NULL, NULL);
		log_info(logger, "Se recibio conexion de %s", tipo_conexion);
		handshake_server(*socket_cliente, logger);

        parametros_atender* parametros = malloc(sizeof(parametros_atender));
        parametros->socket_cliente = socket_cliente;
        parametros->logger = logger;

		pthread_create(&thread, NULL, (void*) atender_cliente, (void*)parametros);
		pthread_detach(thread);
    }
	//pthread_exit (NULL);
	//return NULL;
}
*/

int* esperar_cliente(int socket_servidor, t_log* logger)
{
		// Aceptamos un nuevo cliente
		//int socket_cliente;
		int* socket_cliente = malloc(sizeof(int));
		*socket_cliente = accept(socket_servidor, NULL, NULL);
		log_info(logger, "Se conecto un cliente!");

		return socket_cliente;
}

/*
int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0){
		return cod_op;
	} else {
		close(socket_cliente);
		return -1;
	}
}
*/

void handshake_server(int socket_cliente, t_log* logger){
	
	// Variable para llevar conteo de bytes enviados/recibidos
	size_t bytes;
	
	int32_t handshake;
	int32_t resultOk = 0;
	int32_t resultError = -1;

	bytes = recv(socket_cliente, &handshake, sizeof(int32_t), MSG_WAITALL);
	log_info(logger, "bytes recibidos %lu", bytes);
	
	if (handshake == 99) {
    	bytes = send(socket_cliente, &resultOk, sizeof(int32_t), 0);
		log_info(logger, "Handshake OK");
	} else {
    	bytes = send(socket_cliente, &resultError, sizeof(int32_t), 0);
		log_info(logger, "Handshake ERROR");
	}
}


/*
void* atender_cliente(void* parametros){

	parametros_atender* argumentos = (parametros_atender*) parametros;
	int* cliente = argumentos->socket_cliente;
	t_log* logger = argumentos->logger;
	//free(parametros);
	log_info(logger, "atender_cliente");

	//recibir_paquete(*cliente, logger);
    
	while (1) {
		int cod_op = recibir_operacion(*cliente);
		log_info(logger, "cod_op: %d", cod_op);
		switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(*cliente, logger);
				break;
			case PAQUETE:
				lista = recibir_paquete(cliente_fd);
				log_info(logger, "Me llegaron los siguientes valores:\n");
				list_iterate(lista, (void*) iterator);
				break;
			
			case -1:
				log_error(logger, "el cliente se desconecto. Terminando servidor");
				free(cliente);
				free(parametros);
				return NULL;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
		}
	}

	return NULL;
}
*/

/*
void* atender_cliente(void* parametros){

	parametros_atender* argumentos = (parametros_atender*) parametros;
	int* cliente = argumentos->socket_cliente;
	t_log* logger = argumentos->logger;

	log_info(logger, "atender_cliente");
    
	while (1) {   	
		t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

		// recibo el codigo de operacion
		paquete->codigo_operacion = recibir_operacion(*cliente);
				
		// Ahora en función del código recibido procedemos a deserializar el resto
    	switch(paquete->codigo_operacion) {    
        	case PERSONA:
			    // Recibo el buffer
				recibir_buffer(*cliente, paquete);        
				t_persona* persona = deserializar_persona(paquete->buffer);
            
            	log_info(logger, "PERSONA");
            	log_info(logger, "DNI: %u", persona->dni);
            	log_info(logger, "Edad: %u", persona->edad);
            	log_info(logger, "Pasaporte: %u", persona->pasaporte);
            	log_info(logger, "Nombre (%u caracteres): %s", persona->nombre_length, persona->nombre);
            
            	free(persona);
            	break;

			case HANDSHAKE:
			    // Recibo el buffer
				recibir_buffer(*cliente, paquete);        
				t_handshake* handshake = deserializar_handshake(paquete->buffer);
            
            	log_info(logger, "HANDSHAKE");
            	log_info(logger, "Handshake: %u", handshake->handshake);
            	log_info(logger, "Nombre (%u caracteres): %s", handshake->nombre.mensaje_length, handshake->nombre.texto);
            
            	free(handshake);
            	break;	
            
        	case MENSAJE:
			    // Recibo el buffer
				recibir_buffer(*cliente, paquete); 
            	t_mensaje* mensaje = deserializar_mensaje(paquete->buffer);
            
            	log_info(logger, "MENSAJE");
            	log_info(logger, "Mensaje (%u caracteres): %s", mensaje->mensaje_length, mensaje->texto);
            
            	free(mensaje);
            	break;

			case INSTRUCCION:
			    // Recibo el buffer
				recibir_buffer(*cliente, paquete);        
				t_instruccion* instruccion = deserializar_instruccion(paquete->buffer);
            
            	log_info(logger, "INSTRUCCION");
            	log_info(logger, "PID: %u", instruccion->pid);
            	log_info(logger, "PC: %u", instruccion->pc);
            
            	free(instruccion);
            	break;
        
        	case DESCONEXION:
				log_error(logger, "El cliente se desconecto. Terminando servidor");
				eliminar_paquete(paquete);
				return NULL;
			
			default:
				log_error(logger, "Tipo de operacion desconocida");
				break;
    	}
    	// Liberamos memoria
    	eliminar_paquete(paquete);
	}
	return NULL;
}
*/

// Recibe un codigo de una operacion
t_operacion recibir_operacion(int cliente) {

	t_operacion operacion;
	// Recibo el codigo de operacion
    if(recv(cliente, &operacion, sizeof(t_operacion), 0) > 0){
		return operacion;
	} else {
		close(cliente);
		return DESCONEXION;
	}
}

// Recibe un buffer
void recibir_buffer(int cliente, t_paquete* paquete) {	
	// Recibo el tamaño y luego el contenido del buffer
	recv(cliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(cliente, paquete->buffer->stream, paquete->buffer->size, 0);
}

// Elimina un paquete
void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

/*
void iniciar_servidor(char* puerto, char* tipo_conexion, t_log* logger) {
    
    pthread_t thread;
	int socket = crear_servidor(puerto, logger);
    log_info(logger, "Servidor %s listo para recibir conexiones", tipo_conexion);
    
    parametros_conexion* parametros = malloc(sizeof(parametros_conexion));
    parametros->socket_servidor = socket;
    parametros->tipo_conexion = tipo_conexion;
    parametros->logger = logger;

    pthread_create(&thread, NULL, (void*) esperar_cliente, (void*) parametros);
	pthread_detach(thread);
}
*/

// CLIENTE

int crear_conexion(char *ip, char* puerto)
{
	int err;
	
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;

	socket_cliente = socket(server_info->ai_family,
                            server_info->ai_socktype,
                            server_info->ai_protocol);

	if(socket_cliente == -1){
		 error_show("No se pudo crear socket del cliente");
		 abort();
	}						

	// Ahora que tenemos el socket, vamos a conectarlo
    err = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	if (err == -1) {
    	error_show("No se pudo hacer connect");
    	abort();
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}



void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

int iniciar_conexion(char* ip, char* puerto, t_log* logger) {
    
    int socket = crear_conexion (ip, puerto);
    //log_info(logger, "handshake kernel -> memoria");
    //handshake_cliente(socket, logger);
	//liberar_conexion(socket);

	return socket;
}
