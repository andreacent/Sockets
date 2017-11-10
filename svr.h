/* 
	Taller 2
	Andrea Centeno 10-10138
	Roberto Romero 
*/
#ifndef SVR_H
#define SVR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <strings.h>

#define ERROR -1
#define MAX_CLIENTS	0
#define MAX_DATA 1024 /*size of buffer*/
#define TAMBUFF 1024

typedef struct Cabecera
{
    int identificador;
    int longitud; /* Longitud del mensaje, en bytes */
} Cabecera; 

typedef struct Mensaje
{
	char fechaHora[24]; 
	char info[60];
} Mensaje;

void escribeMensaje (int socket, int idMensaje, char *mensaje, int tamanho); 
void leeMensaje (int socket, int *idMensaje, char **mensaje); 
void crear_archivo(char* direccion);
int buscar_pidh (int pidh[]);

#endif