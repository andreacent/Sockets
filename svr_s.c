/**
* Taller cerrado 3.
*
* Aplicación en la red usando la API de sockets de Berkeley.
* Archivo que simula la aplicación que se ejecuta en el módulo central.
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @version 1.0
* @since   2017-11-12 
*/

#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <ctype.h> //tolower
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>
#include <sys/socket.h>
#include <time.h>

// Declaraciones
char* filename; 
FILE *bitacora;

int get_event_id(char message[]);
void INThandler(int sig);
void handle_alarm(int sig);
void send_mail(char body[]);
void create_tuple(char datetime[], char tid[], int event, char message[], char pattern[]);
char *time_stamp();
//the thread function
void *connection_handler(void *);


/**
* Funcion main()
*
* Funcion principal del programa
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @version 1.0
* @since   2017-11-12 
*/
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    int puerto_svr_s;
     
    if(argc < 5){
        printf("USO CORRECTO: svr_s -l <puerto_svr_s> -b <archivo_bitacora> \n");
        exit(1); //Sale del programa si el usuario no introduce el número correcto de argumentos 
                 // mostrandole el uso correcto de la sintaxis del programa
    }

    // lectura y asignacion de los argumentos
    for(int x=1;x<argc;x+=2){
        switch(argv[x][1]){
            case 'l':
                puerto_svr_s= atoi(argv[x+1]);
                break;
            case 'b':
                filename=argv[x+1];
                bitacora = fopen(filename, "a");
                if (!bitacora) bitacora = fopen(filename, "w");
                break;
        }
    }

    //Se crea el socket
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
        //imprime el mensaje de error
        perror("bind failed. Error");
        return 1;
    }
     
    // 
    listen(socket_desc , 3);
     
    //Acepta una conexion entrante
    puts("Working...");
    c = sizeof(struct sockaddr_in);
    
    signal(SIGINT, INThandler);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {         
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
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    return 0;
}


/**
* Funcion connection_handler()
*
* Funcion que maneja la conexion con cada cliente
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @version 1.0
* @since   2017-11-12 
*/
void *connection_handler(void *socket_desc)
{
    pthread_t tid = pthread_self();
    char tid_str[256];
    sprintf(tid_str, "%lld", tid);

    int sock = *(int*)socket_desc;
    int read_size, event;
    char *datetime, *message, client_message[2000];

    //escribe en bitacora que se acepto la conexion
    create_tuple(time_stamp(), tid_str, 0, "Connection accepted", "N/A");

    //alarma para avisar a los 5 minutos si no hay intercambio
    signal( SIGALRM, handle_alarm );
    alarm( 300 ); 

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        write(sock , "ACK" , 3);

        datetime = strtok(client_message, "|");        
        message = strtok(NULL,"|");
        event = get_event_id(message);

        //escribe en bitacora el mensaje
        if (event > 0)
            create_tuple(datetime, tid_str, event, message, message);
        else 
            create_tuple(datetime, tid_str, event, message, "N/A");

        //vacia el string client_message
        memset(client_message, 0, sizeof(client_message));

        //reinicio la alarma
        alarm( 300 ); 
    }
    
    if(read_size == 0)
    {
        //escribe en bitacora cuando el cliente de desconecta
        create_tuple(time_stamp(), tid_str, 0, "Client disconnected","N/A");
        fflush(stdout);
    }
    //else if(read_size == -1) perror("recv failed");

    //Libera el apuntador al socket
    free(socket_desc);
     
    return 0;
}

/**
* Funcion get_event_id()
*
* Funcion que devuelve el id del evento y en caso de error o de no encontrar el evento devuelve 0
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @param client_message[] array del mensaje del cliente
* @return retorna el id del evento o en su defecto retorna 0
*
* @version 1.0
* @since   2017-11-12 
*/
int get_event_id(char message[])
{
    for (int i=0; message[i]; i++) message[i] = tolower(message[i]);

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

/**
* Funcion create_tuple()
*
* Funcion que crea tupla a escribir en el archivo
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
* @version 1.0
* @since   2017-11-12 
*/
void create_tuple(char datetime[], char tid[], int event, char message[], char pattern[])
{
    char tuple[2000];

    //(serial, fecha, hora, identificación del ATM, código del evento, 
    // patron reconocido, información recibida)
    sprintf(tuple,"%s, %s, %s, %d, %s, %s",tid,datetime,tid,event,pattern,message);

    if (event != 0) {
        //puts(tuple);
        send_mail(tuple);
    }

    fprintf(bitacora, "%s\n", tuple);
}

/**
* Funcion send_mail()
*
* Funcion que envia correo de alerta
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
*
* @version 1.0
* @since   2017-11-12 
*/
void send_mail(char body[])
{
    char cmd[100];  // to hold the command.
    char to[] = "andreacent8@gmail.com"; // email id of the recepient.
    char tempFile[100];     // name of tempfile.

    strcpy(tempFile,"/tmp/sendmail"); // generate temp file name.

    FILE *fp = fopen(tempFile,"w"); // open it for writing.
    fprintf(fp,"%s\n",body);        // write body to it.
    fclose(fp);             // close it.

    //sprintf(cmd,"sendmail -s 'SVR' %s < %s",to,tempFile); // prepare command.
    sprintf(cmd,"mail -s \"SVR\" %s < %s",to,tempFile); // prepare command.
    system(cmd);     // execute it.
}


/**
* Funcion INThandler()
*
* Funcion que se encarga de las manejar las senales
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
*
* @version 1.0
* @since   2017-11-12 
*/
void  INThandler(int sig)
{
    char  c;

    signal(sig, SIG_IGN);
    printf("\nDo you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y') {
        fclose(bitacora);
        exit(0);
    }
    else signal(SIGINT, INThandler);
}


/**
* Funcion handle_alarm()
*
* Funcion que se encarga de la alarma cuando el tiempo se cumple
*
* @author  Andrea Centeno 10-10138
* @author  Roberto Romero 10-10642
*
*
* @version 1.0
* @since   2017-11-12 
*/
void handle_alarm( int sig ) {
    signal(sig, SIG_IGN);

    pthread_t tid = pthread_self();
    char tid_str[256];
    sprintf(tid_str, "%lld", tid);

    create_tuple(time_stamp(), tid_str, 13, "5 minutes without data exchange", "5 minutes");
    send_mail("client connection - 5 minutes without data exchange");
    signal( SIGALRM, handle_alarm );
    alarm( 300 ); //reinicio la alarma
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
    sprintf(timestamp,"%02d/%02d/%04d, %02d:%02d:%02d", tm->tm_mon, tm->tm_mday, tm->tm_year+1900, 
           tm->tm_hour, tm->tm_min, tm->tm_sec);

    return timestamp;
}