#include "memoria.h"



t_log * logger = NULL;
t_config* config = NULL;

int main(int argc, char* argv[]) 
{

    config = iniciar_config("memoria.config");

	logger = log_create("memoria.log", "modulo_memoria", true, LOG_LEVEL_INFO);

    //logger = iniciar_bitacora ( "memoria.log" , "memoria" , (t_log_level) config_get_string_value ( config , "LOG_LEVEL" ));

    log_info(logger, "COMIENZA ejecución de a memoria\n");

    // Cargo la configuracion

    int * socket_escucha = (int *) malloc ( sizeof(int) );

    *socket_escucha = iniciar_servidor ( config_get_string_value(config, "PUERTO_ESCUCHA"), logger);

    log_info(logger, "Se levanto el servidor.\n");

    while (1) {
        pthread_t thread;
        int *fd_conexion_ptr = malloc(sizeof(int));

        log_info(logger, "Esperando cliente...\n");

        *fd_conexion_ptr = accept(*socket_escucha, NULL, NULL);

        log_info(logger, "Se recibió conexión nueva.\n");

        pthread_create(&thread,
                       NULL,
                       atender_cliente_memoria,
                       (void*) fd_conexion_ptr
                    );

        pthread_detach(thread);
   }

	close (*socket_escucha);
    free (socket_escucha);

    printf("MEMORIA FINALIZADA\n");

    return 0;
}

void* atender_cliente_memoria(void* arg){

    int * socket_escucha = (int *) arg;
	
	log_info(logger, "atendiendo cliente. \n");
    
	while (1) {   	
		t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

		// recibo el codigo de operacion
		paquete->codigo_operacion = recibir_operacion(*socket_escucha);
				
		// Ahora en función del código recibido procedemos a deserializar el resto
    	switch(paquete->codigo_operacion) {    
        	
            case CREAR_PROCESO:

                log_info(logger, "Procesando tipo de operacion CREAR_PROCESO. \n");

				recibir_buffer(*socket_escucha, paquete);        
				
                t_paquete_crear_proceso_memoria* crear_proceso_memoria = deserializar_crear_proceso_memoria(paquete->buffer);
            
            	log_info(logger, "Nombre (%u caracteres): %s", crear_proceso_memoria->length_nombre_archivo, crear_proceso_memoria->nombre_archivo);
            
                int valor_retorno = leer_archivo_pseudocodigo(crear_proceso_memoria->nombre_archivo);
				printf ("valor de retorno: %d", valor_retorno);
                //responder_crear_proceso(*socket_escucha, valor_retorno);

            	free(crear_proceso_memoria);

                log_info(logger, "FIN CREAR PROCESO. \n");

            	break;

            case CANTIDAD_ESPACIO_LIBRE:
                log_info(logger, "SE RECIBIO OPERACION DE TIPO CANTIDAD ESPACIO LIBRE. \n");

                log_info(logger, "FIN CANTIDAD ESPACIO LIBRE. \n");
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

/*
void* responder_cantidad_espacio_libre(int * socket_escucha){
    
    t_paquete* paquete_respuesta_crear_proceso = malloc(sizeof(t_paquete));
    paquete_respuesta_crear_proceso->codigo_operacion = EXITO;

    size_t bytes = 2*sizeof(int); 

    void * response_content = malloc(bytes);

    memcpy(response_content, &(paquete_respuesta_crear_proceso->codigo_operacion), sizeof(int));

    send(socket_escucha, response_content, bytes, 0);
    
    free(response_content);
    free(paquete_respuesta_crear_proceso);
    
}
*/
/*
void* responder_crear_proceso(int socket_escucha, int respuesta_crear_proceso){
    
    t_paquete* paquete_respuesta_crear_proceso = malloc(sizeof(t_paquete));
    paquete_respuesta_crear_proceso->codigo_operacion = EXITO;


    if(respuesta_crear_proceso == 1){
        log_error(logger, "Responder archivo inexistente. \n");
        paquete_respuesta_crear_proceso->codigo_operacion = ERROR;
    } 

    size_t bytes = 2*sizeof(int); 

    void * response_content = malloc(bytes);

    memcpy(response_content, &(paquete_respuesta_crear_proceso->codigo_operacion), sizeof(int));

    send(socket_escucha, response_content, bytes, 0);
    
    free(response_content);
    free(paquete_respuesta_crear_proceso);


}
*/

int leer_archivo_pseudocodigo(char* nombre_archivo){
    
    char folder[] = "src/programas/";

    FILE *archivo;
    char cadena[255];

    char * path_file_to_search = strcat(folder, nombre_archivo);

    archivo = fopen( path_file_to_search, "r");

    if(archivo == NULL){
        log_error(logger, "No se recupero archivo para el nombre ingresado. \n");
        return 1;
    }

    while(fgets(cadena, sizeof(cadena), archivo) != NULL){
        printf("%s", cadena);
    }

    fclose(archivo);

    return 0;
}

void * esperar_recursos_memoria (void * arg)
{
	int * socket_escucha = (int *) arg;
	
	while ( true ) 
	{
		int * socket = (int *) malloc (sizeof (t_recurso_io));
		*socket = accept ( *socket_escucha , NULL , NULL);//Se queda en esta linea hasta que se le conecte un nuevo recurso
		//recurso->disponible = true;
		//Espera a que io le envie su nombre
		//recurso->nombre_io = recibir_cadena ( recurso->socket );
		//printf ("LLEGO %s de cadena\n", recurso->nombre_io);
		
		//enviar_cadena ( "OK", recurso->socket );
		printf ("Nueva conexion a memoria");
		//Lo guarda en recurso
		//list_add ( recursos_io , recurso );
    }
	pthread_exit(NULL);
}

/*
t_config_memoria cargar_configuracion(t_config* config) {
    
    log_info(logger, "Cargando la configuracion de entorno.");
    t_config_memoria config_memoria;
    
    // Cargo la configuracion de memoria.config
    config_memoria.puerto_escucha   = config_get_string_value(config, "PUERTO_ESCUCHA");
    config_memoria.tam_memoria      = config_get_string_value(config, "TAM_MEMORIA");
    config_memoria.tam_pagina       = config_get_string_value(config, "TAM_PAGINA");
    config_memoria.entradas_tabla   = config_get_string_value(config, "ENTRADAS_POR_TABLA");
    config_memoria.cantidad_niveles = config_get_string_value(config, "CANTIDAD_NIVELES");
    config_memoria.retardo_memoria  = config_get_string_value(config, "RETARDO_MEMORIA");
    config_memoria.path_swapfile    = config_get_string_value(config, "PATH_SWAPFILE");
    config_memoria.retardo_swap     = config_get_string_value(config, "RETARDO_SWAP");
    config_memoria.log_level        = config_get_string_value(config, "LOG_LEVEL");
    config_memoria.dump_path        = config_get_string_value(config, "DUMP_PATH");
    
    return config_memoria;
}
    */
