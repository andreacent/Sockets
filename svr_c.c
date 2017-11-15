/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
 
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
    puts("Socket created");
     
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
        scanf("%s" , message);
         
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
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
         
        puts("Server reply :");
        puts(server_reply);
    }
     
    close(sock);
    return 0;
}
