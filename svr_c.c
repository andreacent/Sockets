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
 
char *time_stamp();

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
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

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
     
    server.sin_addr.s_addr = inet_addr(argv[ip]);
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto_svr_s);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //keep communicating with server
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

/*
 Returns the current time.
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