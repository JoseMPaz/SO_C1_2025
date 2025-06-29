#include "recurso.h"

void * esperar_recursos_io (void * arg)
{
	int * socket_escucha = (int *) arg;
	while ( true ) 
	{
		t_recurso_io * recurso = (t_recurso_io *) malloc (sizeof (t_recurso_io));
		recurso->socket = accept ( *socket_escucha , NULL , NULL);//Se queda en esta linea hasta que se le conecte un nuevo recurso
		recurso->disponible = true;
		//Espera a que io le envie su nombre
		recurso->nombre_io = recibir_cadena ( recurso->socket );
		//printf ("LLEGO %s de cadena\n", recurso->nombre_io);
		
		enviar_cadena ( "OK", recurso->socket );

		//Lo guarda en recurso
		list_add ( recursos_io , recurso );
    }
	pthread_exit(NULL);
}

/*
void * esperar_recursos_cpu (void * arg)
{
	int * socket_escucha_cpu = (int *) arg;
	
	
	
	while ( true ) 
	{
		t_recurso_cpu * recurso_cpu = (t_recurso_cpu *) malloc (sizeof (t_recurso_cpu));
		
		recurso_cpu->socket_dispatch = accept ( socket_escucha_cpu[DISPATCH] , NULL , NULL);//Espera cliente cpu dispatch
		recurso_cpu->socket_interrupt = accept ( socket_escucha_cpu[INTERRUPT] , NULL , NULL);//Espera cliente cpu dispatch
		recurso_cpu->disponible = true;
		//Espera a que io le envie su nombre
		recurso_cpu->nombre_cpu = recibir_cadena ( recurso_cpu->socket_dispatch );
		enviar_cadena ( "OK", recurso_cpu->socket_dispatch );

		//Lo guarda en recurso
		list_add ( recursos_cpu , recurso_cpu );
    }
	pthread_exit(NULL);
}
*/




