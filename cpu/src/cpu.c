#include "cpu.h"

t_log * logger = NULL;
t_config * config = NULL;

void validar_argumentos_main (int cantidad_argumentos) ;

int main(int argc, char* argv[]) 
{
	int socket_kernel_envio;
	int socket_kernel_interrupcion;
    int socket_memoria;
    //t_log* logger;
    //t_config* config;
    
    // Envio saludo
    saludar("cpu");
    //printf("%s\n", argv[NOMBRE_CPU]);

    // Inicio log y configuracion
    config = iniciar_config ("cpu.config");
    logger = iniciar_logger ( "cpu.log", "cpu", (t_log_level) config_get_int_value(config, "LOG_LEVEL"));
    
    //Flag de argumento
    validar_argumentos_main (argc); 
    
    // Creo las conexiónes de envio e interrupcion con el kernel
    socket_kernel_envio = iniciar_conexion( config_get_string_value(config,"IP_KERNEL"), config_get_string_value(config,"PUERTO_KERNEL_DISPATCH"), logger);
    enviar_handshake(socket_kernel_envio, CPU, argv[NOMBRE_CPU]);
    socket_kernel_interrupcion = iniciar_conexion( config_get_string_value(config,"IP_KERNEL"), config_get_string_value(config,"PUERTO_KERNEL_INTERRUPT"), logger);
    enviar_handshake(socket_kernel_interrupcion, CPU, argv[NOMBRE_CPU]);
    // Creo la conexión con la memoria
    socket_memoria = iniciar_conexion(config_get_string_value(config,"IP_MEMORIA"), config_get_string_value(config,"PUERTO_MEMORIA"), logger);
    
    enviar_mensaje(socket_kernel_envio, "hola kernel-envio, soy la cpu");
    enviar_mensaje(socket_kernel_interrupcion, "hola kernel-interrupcion, soy la cpu");
    enviar_mensaje(socket_memoria, "hola memoria, soy la cpu");
    //handshake_kernel ( socket_dispatch, (char *) argv[NOMBRE_CPU] ) ;
    //printf("hola 2\n");
    
    //getchar()

    /*
    // Crear instancia con inicialización dinámica
    t_persona persona = {
        .dni = 12345678,
        .edad = 30,
        .pasaporte = 87654321,
        .nombre_length = 4,
        .nombre = NULL
    };

    // Asignar memoria dinámica y copiar el nombre
    persona.nombre = (char*)malloc(persona.nombre_length + 1);

    strcpy(persona.nombre, "Juan");

    t_buffer* buffer = serializar_persona(&persona);

    //uint32_t dni = *(uint32_t *)(buffer->stream);
    //uint8_t edad = *(uint8_t *)(buffer->stream + sizeof(uint32_t));
    //uint32_t pasaporte = *(uint32_t *)(buffer->stream + sizeof(uint32_t) + sizeof(uint8_t));
    //uint32_t nombre_length = *(uint32_t *)(buffer->stream + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t));
    //char *nombre = (char *)(buffer->stream + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t));

    //printf("DNI: %u\n", dni);
    //printf("Edad: %u\n", edad);
    //printf("Pasaporte: %u\n", pasaporte);
    

    enviar_paquete(buffer, conexion_memoria, PERSONA);

    */

    /*    
    // Envio de pedido instruccion
    // Crear instancia con inicialización dinámica
    t_instruccion pedido_instruccion = {
        .pid = 10,
        .pc = 100
    };

    t_buffer* buffer3 = serializar_instruccion(&pedido_instruccion);

    uint32_t pid = *(uint32_t *)(buffer3->stream);
    uint32_t pc = *(uint32_t *)(buffer3->stream + sizeof(uint32_t));

    printf("PID: %u\n", pid);
    printf("PC: %u\n", pc);

    enviar_paquete(buffer3, conexion_memoria, INSTRUCCION);
    */

    /*********************************************************/
    /*                  INSTRUCCIONES                        */
    /*********************************************************/

    // FETCH
    // Solicitar proxima instruccion a la memoria
    // usando el PID y PC que me pasa el KERNEL

    //char linea_instruccion[] = "READ 0 20";

    //---------------------------------------------------------

    // DECODE
    // Interpretar la instruccion a ejecutar
    // Ver si requiere traduccion de direccion logica a fisica

    /*
    NOOP 
    WRITE 0 EJEMPLO_DE_ENUNCIADO 
    READ 0 20 
    GOTO 0 
    IO 25000 
    INIT_PROC proceso1 256 
    DUMP_MEMORY 
    EXIT 
    */

    // Interpreto la instruccion
    //tipo_instruccion instruccion = parsear_instruccion(linea_instruccion);

    //-------------------------------------------------------------
    
    // EXECUTE 
    // Ejecutar instruccion
    // NOOP - WRITE - READ - GOTO

    // Las siguientes se consideran SYSCALL
    // No pueden ser resueltas por el CPU y dependen del KERNEL
    // IO - INIT_PROC - DUMP_MEMORY - EXIT

    // Proceso y ejecuto la instruccion
    //procesar_instruccion(instruccion, logger);
    //liberar_instruccion(&instruccion);

    //-------------------------------------------------------------

    // CHECK INTERRUPT 
    // Chequear si el kernel nos envio una interrupcion al PID que se esta ejecutando

    
    //free(buffer->stream);
    //free(buffer);
    //free(persona.nombre);
    //free(mensaje.texto);

    // Terminar programa
    //terminar_programa(conexion_kernel_envio, logger, config); // Terminar programa solo recibe una conexion, habria que ver como modificarla
    //liberar_conexion(conexion_kernel_interrupcion); 
    //liberar_conexion(conexion_memoria);

    return 0;
}

// Interpreta la linea de instruccion que me devuelve la memoria
tipo_instruccion parsear_instruccion(char* linea_instruccion) {

    // Obtengo el primer campo
    //char* primer_campo = strtok(linea_instruccion," ");

    // Obtengo los parametros
    //char* param1 = strtok(NULL, " ");
    //char* param2 = strtok(NULL, " ");
    char** campos;

    campos = string_split(linea_instruccion, " ");
    char* tipo_inst = campos[0];
    char* param1 = campos[1];
    char* param2 = campos[2];

    instruccion tipo = -1;
    if (strcmp(tipo_inst, "NOOP") == 0) tipo = NOOP;
    if (strcmp(tipo_inst, "WRITE") == 0) tipo = WRITE;
    if (strcmp(tipo_inst, "READ") == 0) tipo = READ;
    if (strcmp(tipo_inst, "GOTO") == 0) tipo = GOTO;

    // Crear estructura con los datos
    tipo_instruccion instruccion = {tipo, param1 ? strdup(param1) : NULL, param2 ? strdup(param2) : NULL};

    string_array_destroy(campos);

    return instruccion;
}

// Procesa una instruccion y la ejecuta
void procesar_instruccion(tipo_instruccion instruccion, t_log* logger){
    
    switch (instruccion.tipo)
    {
        case NOOP:
            log_info(logger,"recibi la instruccion NOOP con param1: %s y param2: %s", instruccion.param1, instruccion.param2);
            ejecutar_noop();
            break;

        case WRITE:
            log_info(logger,"recibi la instruccion WRITE con param1: %s y param2: %s", instruccion.param1, instruccion.param2);
            ejecutar_write(instruccion.param1, instruccion.param2);
            break;

        case READ:
            log_info(logger,"recibi la instruccion READ con param1: %s y param2: %s", instruccion.param1, instruccion.param2);
            ejecutar_read(instruccion.param1, instruccion.param2);
            break;

        case GOTO:
            log_info(logger,"recibi la instruccion GOTO con param1: %s y param2: %s", instruccion.param1, instruccion.param2);
            ejecutar_goto();
            break;
    
        default:
            log_info(logger,"Instruccion desconocida");
            break;
    }
}

// Ejecuta instruccion NOOP
void ejecutar_noop() {
    sleep(1);
}

// Ejecuta instruccion WRITE
void ejecutar_write(char* param1, char* param2) {
    //enviar_mensaje(char* mensaje, int socket_cliente)
    sleep(1);
}

// Ejecuta instruccion READ
void ejecutar_read(char* param1, char* param2) {
    //enviar_mensaje(char* mensaje, int socket_cliente)
    sleep(1);
}

// Ejecuta instruccion GOTO
void ejecutar_goto() {
    sleep(1);
}

// Libera memoria de una instruccion
void liberar_instruccion(tipo_instruccion* instruccion) {
    free(instruccion->param1);
    free(instruccion->param2);
}

void validar_argumentos_main (int cantidad_argumentos) 
{
	if (cantidad_argumentos != 2)
  	{
        log_error ( logger,"Error: Se espera <./bin/cpu> <nombre_cpu>" );
    	exit (1);    
    }
    return;
}
