#include <utils/hello.h>

void saludar(char* quien) 
{
    printf("Hola desde %s!!\n", quien);
}

int handshake_kernel (int socket, char * nombre)
{
	char * respuesta = NULL;

	enviar_cadena ( nombre, socket );
	respuesta = recibir_cadena ( socket );
	
	if (strcmp (respuesta,"OK") == 0)
	{
		free (respuesta);
		return 0;
	}

	free (respuesta);

	return -1;
}
