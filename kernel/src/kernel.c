#include "kernel.h"

t_log * logger = NULL;
t_config* config = NULL;

t_list * recursos_io = NULL;//nodos: int socket; bool disponible; char * nombre_io
t_list * recursos_cpu = NULL;

void imprimir_recurso_io (void * arg) ;
void imprimir_recurso_cpu (void * arg) ;

int main(int argc, char* argv[]) 
{
    //pthread_t hilo_para_conectar_io;
    //pthread_t hilo_para_conectar_cpu;
    int conexion_memoria;
    pthread_t hilo_atender_io;
    pthread_t hilo_atender_cpu;
    
    int* socket_kernel_io = (int*)malloc(sizeof(int));
    int* socket_kernel_cpu = (int*)malloc(sizeof(int)*2);
    //int* socket_kernel_interrupcion = (int*)malloc(sizeof(int)*2);
    
    //int * socket_escucha_io = (int *) malloc ( sizeof(int) );
    //int * socket_escucha_cpu = (int *) malloc ( sizeof(int) * 2/*dispatch e interrupt*/ );//Vector de 2 socket
    
    // Envio saludo
    saludar("kernel");

    // Inicializo configuracion y la bitacora
    config = iniciar_config("kernel.config");
    logger = iniciar_logger("kernel.log" , "kernel" , (t_log_level)config_get_int_value(config , "LOG_LEVEL"));
    
    recursos_io = list_create ();//Se genera una lista vacia generica
    recursos_cpu = list_create ();//Se genera una lista vacia generica
    
    // Creo conexion con la memoria
    conexion_memoria = iniciar_conexion(config_get_string_value (config, "IP_MEMORIA"), config_get_string_value (config, "PUERTO_MEMORIA"), logger);
    // Envio handshake
    
    // Envio mensaje
    enviar_mensaje(conexion_memoria, "Hola memoria, soy el kernel");

    // Inicio los socket de escucha
    //*socket_escucha_io = crear_servidor ( config_get_string_value ( config , "PUERTO_ESCUCHA_IO") , logger);//Genero Patovica io único
	//socket_escucha_cpu[DISPATCH] = crear_servidor ( config_get_string_value (config, "PUERTO_ESCUCHA_DISPATCH") , logger);//Genero Patovica cpu dispatch único
	//socket_escucha_cpu[INTERRUPT] = crear_servidor ( config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT") , logger);//Genero Patovica cpu interrupt único

    *socket_kernel_io = iniciar_servidor(config_get_string_value (config, "PUERTO_ESCUCHA_IO"), logger);
    socket_kernel_cpu[ENVIO] = iniciar_servidor(config_get_string_value (config, "PUERTO_ESCUCHA_DISPATCH"), logger);
    socket_kernel_cpu[INTERRUPCION] = iniciar_servidor(config_get_string_value (config, "PUERTO_ESCUCHA_INTERRUPT"), logger);

	// Espero conexiones
    pthread_create(&hilo_atender_io, NULL, atender_conexion_io, (void*) socket_kernel_io);
    pthread_detach(hilo_atender_io);
    pthread_create(&hilo_atender_cpu, NULL, atender_conexion_cpu, (void*) socket_kernel_cpu);
    pthread_detach(hilo_atender_cpu);
    
    //Su funcion es aceptar conexiones de io y agregarla a la lista recursos_io que es global
    //pthread_create( &hilo_para_conectar_io , NULL , esperar_recursos_io , (void*) socket_escucha_io );
    //pthread_detach ( hilo_para_conectar_io );
    
    //pthread_create( &hilo_para_conectar_cpu , NULL , esperar_recursos_cpu , (void*) socket_escucha_cpu );
    //pthread_detach ( hilo_para_conectar_cpu );
    
    //printf ("hizo detach\n"); //   //pthread_exit(NULL); lo hace en la funcion esperar_recursos_io
    //getchar ();//esto es de prueba para esperar que se conecten clientes io
    
    //printf ("Cantidad de io conectados fueron %d\n", list_size(recursos_io));
    //printf ("Cantidad de cpu conectados fueron %d\n", list_size(recursos_cpu));
    //list_iterate ( recursos_io , imprimir_recurso_io);
    //list_iterate ( recursos_cpu , imprimir_recurso_cpu);
    /*
    
    ACA SI HAY AL MENOS 1 IO y 1 CPU CONECTADOS EMPIEZA A FUNCIONAR EL SISTEMA DISTRIBUIDO
    ESTOS IO Y CPU ESTARAN EN LISTAS DE RECURSOS PARA SER USADOS
    
    */
    
    //Hay que eliminar las listas de recurso al terminar el programa
    //printf("KERNEL FINALIZADO\n");
    
    //close (*socket_kernel_io);
    //free (socket_kernel_io);
    //close (*socket_kernel_cpu);
    //free (socket_kernel_io);
    
    //liberar_conexion(conexion_memoria);
    
    pthread_exit(NULL);
    
    return 0;
}

void imprimir_recurso_io (void * arg) 
{
    t_recurso_io * recurso = ( t_recurso_io * ) arg;
    
    printf("Socket: %d, Disponible: %s, Nombre de io: %s\n", recurso->socket, recurso->disponible ? "true" : "false", recurso->nombre_io);
    return;
}

void imprimir_recurso_cpu (void * arg) 
{
    t_recurso_cpu * recurso = ( t_recurso_cpu * ) arg;
    
    printf("Socket Dispatch: %d, Socket Dispatch: %d, Disponible: %s, Nombre de cpu: %s\n", recurso->socket_envio, recurso->socket_interrupcion,
    recurso->disponible ? "true" : "false", recurso->nombre_cpu);
    return;
}

/*#PCB definicion de estructura e inicializacion de planificadores*/
//static int next_pid = 0;

t_list* cola_new = NULL; // Declaración de la lista NEW

static int next_pid = 0;

PROCESO* init_proc(const char* archivo, unsigned int size) {
    PROCESO* nuevo = malloc(sizeof(PROCESO));
    if (!nuevo) return NULL;

    nuevo->pcb.pid = next_pid++;
    nuevo->pcb.pc = 0;
    nuevo->pcb.size = size;
    nuevo->pcb.RAF = config_get_int_value(config,"ESTIMACION_INICIAL");
    nuevo->pcb.UltRAF = 0;
    nuevo->pcb.state = NEW;

    memset(nuevo->pcb.ContTime, 0, sizeof(nuevo->pcb.ContTime));
    memset(nuevo->pcb.ContState, 0, sizeof(nuevo->pcb.ContState));

    nuevo->archivo_pseudocodigo = strdup(archivo);
    nuevo->tamano_proceso = size;

    if (!cola_new)
        cola_new = list_create();

    list_add(cola_new, nuevo);

    return nuevo;
}

void* atender_conexion_cpu(void* socket){
	
	int* socket_kernel = (int*) socket;
    pthread_t hilo_atender_envio;
    pthread_t hilo_atender_interrupcion;

    while(1){
        
        int* socket_cliente_envio = esperar_cliente(socket_kernel[ENVIO], logger);
        t_handshake* handshake_cpu_envio = recibir_handshake(*socket_cliente_envio);
        int* socket_cliente_interrupcion = esperar_cliente(socket_kernel[INTERRUPCION], logger);
        //t_handshake* handshake_cpu_interrupion = recibir_handshake(*socket_cliente_interrupcion);
        
        // Obtengo info del recurso
        t_recurso_cpu* recurso = (t_recurso_cpu*)malloc(sizeof(t_recurso_cpu));
        recurso->socket_envio = *socket_cliente_envio;
        recurso->socket_interrupcion = *socket_cliente_interrupcion;
        recurso->disponible = true;
        recurso->nombre_cpu = handshake_cpu_envio->nombre.texto;
        
        // Guardo recurso en la lista
        list_add(recursos_cpu, recurso);

        printf ("Cantidad de cpu conectados fueron %d\n", list_size(recursos_cpu));
        list_iterate (recursos_cpu , imprimir_recurso_cpu);

        pthread_create(&hilo_atender_envio, NULL, (void*) atender_cliente_kernel, (void*) socket_cliente_envio);
	    pthread_detach(hilo_atender_envio);

        pthread_create(&hilo_atender_interrupcion, NULL, (void*) atender_cliente_kernel, (void*) socket_cliente_interrupcion);
	    pthread_detach(hilo_atender_interrupcion);
    } 
}

void* atender_conexion_io(void* socket){
	
	int* socket_kernel = (int*) socket;
    pthread_t hilo_atender_io;
    
    while(1){
        
        int* socket_cliente_io = esperar_cliente(*socket_kernel, logger);
        t_handshake* handshake_io = recibir_handshake(*socket_cliente_io);

        // Obtengo info del recurso
        t_recurso_io* recurso = (t_recurso_io*)malloc(sizeof(t_recurso_io));
        recurso->socket = *socket_cliente_io;
        recurso->disponible = true;
        recurso->nombre_io = handshake_io->nombre.texto;
        
        // Guardo recurso en la lista
        list_add(recursos_io, recurso);

        printf ("Cantidad de io conectados fueron %d\n", list_size(recursos_io));
        list_iterate (recursos_io, imprimir_recurso_io);
    

        pthread_create(&hilo_atender_io, NULL, (void*) atender_cliente_kernel, (void*) socket_cliente_io);
	    pthread_detach(hilo_atender_io);
    } 
}

void* atender_cliente_kernel(void* socket_cliente){

    int* cliente = (int*) socket_cliente;

    while (1) {   	
		t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

		// recibo el codigo de operacion
		paquete->codigo_operacion = recibir_operacion(*cliente);	

		// Ahora en función del código recibido procedemos a deserializar el resto
    	switch(paquete->codigo_operacion) {   
            
			case HANDSHAKE:
			    // Recibo el buffer
				
                recibir_buffer(*cliente, paquete);       
				t_handshake* handshake = deserializar_handshake(paquete->buffer);
            
            	log_info(logger, "HANDSHAKE");
            	log_info(logger, "codigo: %u", handshake->codigo);
                log_info(logger, "recurso: %d", handshake->recurso);
            	log_info(logger, "Nombre (%u caracteres): %s", handshake->nombre.longitud, handshake->nombre.texto);

            	break;	
    
        	case MENSAJE:
			    // Recibo el buffer
				recibir_buffer(*cliente, paquete); 
            	t_mensaje* mensaje = deserializar_mensaje(paquete->buffer);
            
            	log_info(logger, "MENSAJE");
            	log_info(logger, "Mensaje (%u caracteres): %s", mensaje->longitud, mensaje->texto);
            
            	free(mensaje);
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
	//return NULL;
}

t_handshake* recibir_handshake(int cliente){
    
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    
    paquete->codigo_operacion = recibir_operacion(cliente);

    // Recibo el buffer
	recibir_buffer(cliente, paquete);       
	t_handshake* handshake = deserializar_handshake(paquete->buffer);
    
    log_info(logger, "HANDSHAKE");
    log_info(logger, "codigo: %u", handshake->codigo);
    log_info(logger, "recurso: %d", handshake->recurso);
    log_info(logger, "Nombre (%u caracteres): %s", handshake->nombre.longitud, handshake->nombre.texto);

    return handshake;
}
