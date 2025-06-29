#ifndef CPU_H_
#define CPU_H_

#include <utils/hello.h>
#include <utils/socket.h>
#include <utils/auxiliar.h>
#include <commons/string.h>

#define NOMBRE_CPU 1

typedef enum {NOOP, WRITE, READ, GOTO} instruccion;
typedef struct {
    instruccion tipo;
    char* param1;
    char* param2;
} tipo_instruccion;

tipo_instruccion parsear_instruccion(char*);
void procesar_instruccion(tipo_instruccion, t_log*);
void liberar_instruccion(tipo_instruccion*);
void ejecutar_noop();
void ejecutar_write(char*, char*);
void ejecutar_read(char*, char*);
void ejecutar_goto();

#endif /* CPU_H_ */
