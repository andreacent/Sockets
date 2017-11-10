/* 
	Taller 2
	Andrea Centeno 10-10138
	Roberto Romero 
*/
#include "svr.h"

int main(int argc, char **argv){

	struct sockaddr_in server; /*server structure*/
	struct sockaddr_in  client; /*client structure will contain any client information conecting to us.*/
	int sockfd; /*server socket descriptor*/
	int newfd; /*client socket descriptor*/
	int sockaddr_len = sizeof(struct sockaddr_in);/*we will require this for size as input for both the bind and accept calls*/

	char *mensaje = NULL;
	int identificador; 

	char* bitacora;
	int puerto_svr_s;

	if(argc < 5){
		printf("USO CORRECTO: svr_s -l <puerto_svr_s> -b <archivo_bitacora> \n");
		exit(1); //Sale del programa si el usuario no introduce el número correcto de argumentos
	}

	for(int x=1;x<argc;x+=2){
		switch(argv[x][1]){
			case 'l':
				puerto_svr_s= atoi(argv[x+1]);
				break;
			case 'b':
				bitacora = argv[x+1];
				crear_archivo(bitacora);
				break;
		}
	}

	/*start server process, we make a socket*/
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
		perror("Server socket: ");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(puerto_svr_s);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 0);

	/*now we bind the socket to the dir:port*/
	if((bind(sockfd, (struct sockaddr *)&server, sockaddr_len)) == ERROR){
		perror("bind: ");
		exit(-1);
	}

	/* We instruct the kernel to liste on the socket sockfd*/
	if((listen(sockfd, MAX_CLIENTS))== ERROR){
		perror("listen: ");
		exit(-1);
	}

	int pid, accion, estado;
	int pidc=0;
	pid_t pidh[5]={0,0,0,0,0};

	/*Main loop, we wait for get a client conection*/
	while(1){
		/*accept is a blocking call, we will be waiting for a conection*/
		if((newfd = accept(sockfd, (struct sockaddr *)&client, &sockaddr_len)) == ERROR){
			perror("accept: ");
			exit(-1);
		}

		if (pidc==5){
			/*Rebotar conexiones entrantes*/
			/* Espero información sobre cualquier hijo*/
			pid = wait(&estado);
			for(int y=0; y<5; y+=1){
				if (pid == pidh[y]){
					pidh[y]=0;
					pidc-=1;
				}
			}
		}

		int x = buscar_pidh(pidh);

		/*Creación procesos*/
		if ((pidh[x]=fork())==-1){
			perror("Fork error");
			exit(-1);
		}
		else if (pidh[x]==0){
			/* Se lee el mensaje */
			leeMensaje (newfd, &identificador, &mensaje); 

			/* Se libera el mensaje  cuando ya no lo necesitamos */
			if (mensaje != NULL){
			    free (mensaje);
			    mensaje = NULL;
			}

			close(newfd);
			exit(0);
		}

		/*Codigo Padre*/
		else if (pidh[x]>0){
			close(newfd);
			pidc+=1;
		}

	}

	close(sockfd);	

	return 0;	
}		
