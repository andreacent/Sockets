/*
    Taller 2
    Andrea Centeno 10-10138
    Roberto Romero 10-10642
*/
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <ctype.h> //tolower
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread

#include <sys/socket.h>

char* filename; 

//the thread function
void *connection_handler(void *);
int get_event_id(char message[]);
 
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
                filename=argv[x+1];
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
    
    //fclose(bitacora);

    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    //pid_t tid = gettid();
    //pthread_id_np_t   tid;
   // tid = pthread_getthreadid_np();

    pthread_t tid = pthread_self();
    char tid_str[256];
    sprintf(tid_str, "%lld", tid);

    int sock = *(int*)socket_desc;
    int read_size, event;
    char *message, client_message[2000];
    char eventID[2];

    FILE *bitacora = fopen(filename, "a");
    if (!bitacora) bitacora = fopen(filename, "w");
          
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        write(sock , "ACK" , 3);

        char tuple[2000];

        //(serial, fecha, hora, identificación del ATM, código del evento, 
        // patron reconocido, información recibida)
        strcpy(tuple, "serial");  
        strcat(tuple, ", ");
        strcat(tuple, strtok(client_message, "|")); 
        strcat(tuple, ", ");
        strcat(tuple, tid_str);
        strcat(tuple, ", ");

        message = strtok(NULL,"|");
        event = get_event_id(message);
        sprintf(eventID, "%d", event);
        
        if (event != 0)  write(sock , "ALERTA" , 6);
 
        strcat(tuple, eventID);
        strcat(tuple, ", ");
        if (event != 0) strcat(tuple, message); else strcat(tuple, "none");
        strcat(tuple, ", ");
        strcat(tuple, message); 

        puts(tuple);

        fprintf(bitacora, "%s\n",tuple );

        memset(client_message, 0, sizeof(client_message));
    }
    
    fclose(bitacora);
     
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

/*
 * Obtiene id del evento o devuelve 0
 * */
int get_event_id(char client_message[])
{
    char message[2000];

    for (int i=0; client_message[i]; i++) 
        message[i] = tolower((unsigned char)client_message[i]);

    if (strcmp(message, "communication offline") == 0) return 1;
    else if (strcmp(message, "communication error") == 0) return 2;
    else if (strcmp(message, "low cash alert") == 0) return 3;
    else if (strcmp(message, "running out of notes in cassette") == 0) return 4;
    else if (strcmp(message, "empty") == 0) return 5;
    else if (strcmp(message, "service mode entered") == 0) return 6;
    else if (strcmp(message, "service mode left") == 0) return 7;
    else if (strcmp(message, "device did not answer as expected") == 0) return 8;
    else if (strcmp(message, "the protocol was cancelled") == 0) return 9;
    else if (strcmp(message, "low paper warning") == 0) return 10;
    else if (strcmp(message, "printer error") == 0) return 11;
    else if (strcmp(message, "paper-out condition") == 0) return 12;
    
    return 0;
}