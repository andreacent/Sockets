/* 
	Taller 2
	Andrea Centeno 10-10138
	Roberto Romero 
*/
#include "svr.h"

int main(int argc, char *argv[]){
	struct sockaddr_in remote_server;
	int sockfd;
	int puerto_svr_s,ip,puerto_local;

	if(argc != 5 && argc != 7){
		printf("USO CORRECTO: svr_c -d <nombre_módulo_central> -p <puerto_svr_s> [-l <puerto_local>] \n");
		exit(1); //Sale del programa si el usuario no introduce el número correcto de argumentos
	}

	for(int x=1; x < argc ; x+=2){
		switch(argv[x][1]){
			case 'p':
				puerto_svr_s= atoi(argv[x+1]) ;
				break;
			case 'd':
				ip = x+1;
				//si es dominio, hay que obtener ip
				break;
			case 'l':
				puerto_local = atoi(argv[x+1]);
				break;
		}
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){		
		perror("Socket: ");
		exit(-1);
	}

	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(puerto_svr_s);
	remote_server.sin_addr.s_addr = inet_addr(argv[ip]);
	bzero(&remote_server.sin_zero, 0);
	
	int con = 0;
	if ((connect(sockfd, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in))) == ERROR){
		perror("connect: ");
		sleep(2);
		while (con<2 && ((connect(sockfd, (struct sockaddr *)&remote_server, sizeof(struct sockaddr_in))) == ERROR)){
			perror("connect: ");
			sleep(2);
			con++;
		}
		exit(-1);
	}
	else{
		printf("Conected to server (%s:%d)\n", inet_ntoa(remote_server.sin_addr),ntohs(remote_server.sin_port));
		/* Se declara y rellena el mensaje que queremos enviar */
		Mensaje mensaje = {"003:28:2012:10:38:200004","Response of device not completely received"};

		int identificador = 2;
		/* Se envía llamando a nuestra función de librería */
		escribeMensaje (sockfd, identificador, (char *)&mensaje, sizeof(mensaje));
	}
	close(sockfd);

	return 0;
}
