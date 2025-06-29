#include "io.h"

t_log * logger = NULL;
t_config * config = NULL;


/*
El módulo IO es el encargado de simular las operaciones de Entrada/Salida, para ello al iniciar deberá
leer como parámetro de entrada el nombre_io el cual identificará a la interfaz de IO.
nombre_io == argv[1]
*/
int main ( int argc, char * argv[] ) 
{    
    int socket_kernel;
    unsigned int tiempo;
    uint32_t pid;
    int32_t fin_io = 0 ;
    
    //Saludo
    saludar("io");
    
    // Inicio log y configuracion
    config = iniciar_config ("io.config");
	logger = iniciar_logger ( "io.log", "io", (t_log_level) config_get_int_value (config, "LOG_LEVEL"));
	    
    //Flag de argumento
    validar_argumentos_main (argc); 
    
    /*se conectará al Kernel y en el handshake inicial le enviará su nombre para que el kernel lo pueda identificar*/
    // Creo la conexión de envio con el kernel
    socket_kernel = iniciar_conexion(config_get_string_value(config,"IP_KERNEL"), config_get_string_value(config,"PUERTO_KERNEL"), logger);    
    enviar_handshake(socket_kernel, IO, argv[NOMBRE_IO]);// Envio handshake con nombre del dispositivo
    
    // Envio mensaje
    //enviar_mensaje(socket_kernel, "Hola kernel, soy io");

	//hago un handshake para pasarle el nombre_io en el formato:
	//<TAMANO_CADENA> <CADENA>
	//handshake_kernel ( socket, (char *) argv[NOMBRE_IO] ) ;
	
	
	while (true)
	{
		/*quedará esperando las peticiones del mismo.*/
		if( recv(socket_kernel , &pid, sizeof(uint32_t), 0) > 0 )
		{
			if( recv(socket_kernel , &tiempo, sizeof(unsigned int), 0) > 0 ) //LLego pid y tiempo OK
			{
				log_trace ( logger, "## PID: %u - Inicio de IO - Tiempo: %u", (unsigned int)pid, tiempo);//Logs mínimos y obligatorios 1
				usleep(tiempo * 1000);//Paso el tiempo de milisegundos a microsegundos
				log_trace ( logger, "## PID: %u - Fin de IO", (unsigned int)pid);// Logs mínimos y obligatorios 2
				send (socket_kernel, &fin_io, sizeof(fin_io), 0);//Avisar que termino
			}
			else
			{
				close (socket_kernel);
				return 1;
			}	
		} 
		else 
		{
			close (socket_kernel);
			return 1;
		}
	}
	
		
	//getchar();
    
    close(socket_kernel);
	log_destroy (logger);
    return 0;
}

void validar_argumentos_main (int cantidad_argumentos) 
{
	if (cantidad_argumentos != 2)
  	{
        log_error ( logger,"Error: Se espera <./bin/io> <nombre_io>" );
    	exit (1);    
    }
    return;
}
