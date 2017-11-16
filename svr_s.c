/*
    Taller 2
    Andrea Centeno 10-10138
    Roberto Romero 10-10642
*/
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <sys/types.h>

FILE *bitacora;

//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
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
                bitacora = fopen(argv[x+1], "wb");
                break;
        }
    }

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(puerto_svr_s);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    fclose(bitacora);

    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
          
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        puts(client_message);

        //Send the message back to client
        message = "ACK";
        write(sock , message , strlen(message));

        //pid_t tid = gettid();
        char *p,tuple[2000];

        //(serial, fecha, hora, identificación del ATM, código del evento, patrón reconocido, información recibida)
        strcpy(tuple, "serial,");  
        strcat(tuple, strtok(client_message, "|")); 
        strcat(tuple, ","); 
        //strcat(tuple, tid);
        strcat(tuple, ", 0,"); 
        strcat(tuple, "patron,");
        strcat(tuple, strtok(NULL,"|")); 

        puts(tuple);

        char buffer[] = { 'x' , 'y' , 'z' };
        fwrite(buffer , sizeof(char), sizeof(buffer) , bitacora );
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}