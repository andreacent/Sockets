/**
* Taller cerrado 3.
*
* Aplicación en la red usando la API de sockets de Berkeley.
* Archivo que simula la aplicación que se ejecuta en el ATM.
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @version 1.0
* @since   2017-11-12 
*/

#include <stdlib.h> //exit(1)
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h> // for close
#include <time.h>
#include <netdb.h>

// Declaraciones
#define h_addr h_addr_list[0]
 
char *time_stamp();
char *ip_local(char name[]);


/**
* Funcion main()
*
* Funcion principal del programa
*
* @author  Silver Moon - http://www.binarytides.com/server-client-example-c-sockets-linux/
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @version 1.0
* @since   2017-11-12 
*/
int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
    int puerto_svr_s,puerto_local;
    char *ip_host,*ip;

    if(argc != 5 && argc != 7){
        printf("USO CORRECTO: svr_c -d <ip_módulo_central> -p <puerto_svr_s> [-l <puerto_local>] [-h <nombre_módulo_central>] \n");
        exit(1); //Sale del programa si el usuario no introduce el número correcto de argumentos
    }

    char attr = 0;
    // lectura y asignacion de los argumentos
    for(int x=1; x < argc ; x+=2){
        switch(argv[x][1]){
            case 'p':
                puerto_svr_s= atoi(argv[x+1]) ;
                attr++;
                break;
            case 'd':
                ip = argv[x+1];
                attr++;
                break;
            case 'l':
                puerto_local = atoi(argv[x+1]);
                break;
            case 'h':
                ip_host = ip_local(argv[x+1]);
                attr++;
                break;
        }
    }

    if(attr < 2){
        printf("Debe especificar ID o nombre del módulo central con los flags -p o -h\n");
        exit(1); //Sale del programa si el usuario no introduce el número correcto de argumentos
    }

    if (!ip) strcpy(ip,ip_host);

    //Crea el socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return 1;
    }
     
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto_svr_s);

    //Se conecta al servidor remoto
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //mantiene la comunicacion con el servidor
    while(1)
    {
        printf("Enter message : ");
        int i = 0;
        char *a = (char *) malloc(sizeof(char) * 1000);
        while (1) {
            scanf("%c", &a[i]);
            if (a[i] == '\n') break;
            else i++;
        }
        a[i] = '\0';
        
        char msg[1024];
        strcpy(msg, time_stamp());  
        strcat(msg, a );

        //Send some data
        if( send(sock , msg , strlen(msg) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
         
        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
         
        memset(server_reply, 0, strlen(server_reply));
        //memset(server_reply, 0, sizeof 2000);
    }
     
    close(sock);
    return 0;
}

/**
* Funcion time_stamp()
*
* Funcion que devuelve la hora y fecha actual
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @return retorna la hora y fecha actual
*
* @version 1.0
* @since   2017-11-12 
*/

char *time_stamp(){    
    time_t ltime;
    ltime=time(NULL);

    struct tm *tm;
    tm=localtime(&ltime);

    char *timestamp = (char *)malloc(sizeof(char) * 16);
    sprintf(timestamp,"%02d/%02d/%04d, %02d:%02d:%02d|", tm->tm_mon, tm->tm_mday, tm->tm_year+1900, 
           tm->tm_hour, tm->tm_min, tm->tm_sec);

    return timestamp;
}


/**
* Funcion ip_local()
*
* Funcion que obtiene la ip de un nombre de dominio
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @return retorna la ip de un nombre de dominio
*
* @version 1.0
* @since   2017-11-12 
*/
char *ip_local(char name[]) {
    struct sockaddr_in host;
    char *ip;

    struct hostent* h=gethostbyname(name);
    if (!h) return "";

    host.sin_addr = * (struct in_addr*) h->h_addr;
    ip = inet_ntoa(host.sin_addr);
     
    return ip;
}