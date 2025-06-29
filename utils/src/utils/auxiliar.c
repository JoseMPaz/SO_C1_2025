#include "auxiliar.h"

t_log* iniciar_logger(char* ruta, char* modulo, t_log_level detalle)
{
	t_log* nuevo_logger;

	nuevo_logger = log_create(ruta, modulo, true, detalle);

	return nuevo_logger;
}

t_config* iniciar_config(char* ruta)
{
	t_config* nuevo_config;

	nuevo_config = config_create(ruta);

	if (nuevo_config == NULL) {
    	// ¡No se pudo crear el config!
    	// Terminemos el programa
		abort();
	}

	return nuevo_config;
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	// Finalizo conexion, log y configuracion
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}

//Valida cada recurso. En caso de haber error en la inicialización el programa abortará la ejecución.
void validar_recurso(void* recurso, const char* nombre, const char* tipo) {
    if (recurso == NULL) {
        fprintf(stderr, "Error al crear %s: %s\n", tipo, nombre);
        abort();
    }
}
