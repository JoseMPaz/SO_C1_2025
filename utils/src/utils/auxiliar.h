#ifndef AUXILIAR_H_
#define AUXILIAR_H_

#include <string.h>

#include <commons/log.h>
#include <commons/config.h>

#include <utils/socket.h>

t_log* iniciar_logger(char*, char*, t_log_level);
t_config* iniciar_config(char*);
void terminar_programa(int conexion, t_log*, t_config*);
void validar_recurso(void*, const char*, const char*);
t_log* iniciar_bitacora (char * nombre_archivo, char * nombre_modulo, t_log_level nivel_detalle);

#endif /* AUXILIAR_H_ */
