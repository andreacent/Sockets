/* 
	Taller 2
	Andrea Centeno 10-10138
	Roberto Romero 

	Referencia: http://www.chuidiang.org/clinux/sockets/mensajes.php
*/
#include "svr.h"

void escribeMensaje (int socket, int idMensaje, char *mensaje, int tamanho)
{
    /* Se declara y rellena la cabecera */
    Cabecera cabecera = {idMensaje,tamanho};

    /* Se envía la cabecera */
    send(socket, &cabecera, sizeof(cabecera),0);
    //write (socket, &cabecera, sizeof(cabecera));

    printf("Mensaje de longitud %d a enviar: %s\n",tamanho,mensaje);

    /* Si el mensaje no tiene cuerpo, hemos terminado */
    if ((mensaje == NULL) || (tamanho == 0)) return;

    /* Se envía el cuerpo */
    send(socket, mensaje, tamanho,0);
    //write (socket, mensaje, tamanho);
} 

void leeMensaje (int socket, int *idMensaje, char **mensaje)
{
    Cabecera cabecera;
    *mensaje = NULL;  /* Ponemos el mensaje a NULL por defecto */

    //read (socket, &cabecera, sizeof(cabecera)); /* Se lee la cabecera */
    recv(socket, &cabecera, sizeof(cabecera),0);

    /* Rellenamos el identificador para devolverlo */
    *idMensaje = cabecera.identificador;

    /* Si hay que leer una estructura detrás */
    if (cabecera.longitud > 0)
    {
        *mensaje = (char *)malloc (cabecera.longitud);  /* Se reserva espacio para leer el mensaje */
        //read (socket, *mensaje, cabecera.longitud);
        recv(socket, *mensaje, cabecera.longitud,0);
    }

    printf("%d | Mensaje de longitud %d recibido: %s\n",cabecera.identificador,cabecera.longitud,*mensaje);
} 

/*
Crea el archivo y en caso que exista elimina su contenido
*/
void crear_archivo(char* direccion){
	int fd;
	if((fd = open(direccion,O_CREAT|O_TRUNC, 0600)) < 0){
		printf("error al crear el archivo");
		exit(-1);
	}
}

/*
Encuentra el primer pidh en 0
*/
int buscar_pidh (int pidh[]){
	int y=-1;
	for(int x=0; x<5; x+=1){
		if((pidh[x]==0)){
			y=x;
			return y;
		}
	}
	return y;
}