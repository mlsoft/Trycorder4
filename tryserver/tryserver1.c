/*
    C socket server example, handles multiple clients using threads
*/
 
#include <stdio.h>
#include <fcntl.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
 
// the thread function to handle listener loop
void *listener_handler(void *);

// the thread function to handle a received connection
void *connection_handler(void *);

void sendall(char *);

// the log-file pointer
static FILE *fplog;
 
static char inputline[256]="";

int main(int argc , char *argv[])
{

    fplog=fopen("tryserver.log","a");
    
    // the thread that will handle connections from trycorders
    pthread_t listener_thread;
    
    if( pthread_create( &listener_thread , NULL ,  listener_handler , (void *)"Tryserver" ) < 0) {
            perror("could not create listener thread\n");
            return (1);
    }

    while(1) {
      char *s=fgets(inputline,200,stdin);
      if(s!=NULL) {
	// send this command to all trycorders
	sendall(inputline);
      }
    }
    
    fclose(fplog);
 
    return(0);
}

// this will handle the mechanic to send this master message to all trycorders connected
void sendall(char *line) {

  return;
}


/*
 * This will handle connection for each client
 * */

void* listener_handler(void* threadname)
{

    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
        return(NULL);
    }
    printf("Listener Socket created: %d\n",socket_desc);
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 1701 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error\n");
        return(NULL);
    }
    //puts("bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Connection accepted: Sock-%d\n",client_sock);
        printf("From:%d.%d.%d.%d\n",
            (int)(client.sin_addr.s_addr&0xFF),
            (int)((client.sin_addr.s_addr&0xFF00)>>8),
            (int)((client.sin_addr.s_addr&0xFF0000)>>16),
            (int)((client.sin_addr.s_addr&0xFF000000)>>24));
        fprintf(fplog,"From:%d.%d.%d.%d\n",
            (int)(client.sin_addr.s_addr&0xFF),
            (int)((client.sin_addr.s_addr&0xFF00)>>8),
            (int)((client.sin_addr.s_addr&0xFF0000)>>16),
            (int)((client.sin_addr.s_addr&0xFF000000)>>24));
	fflush(fplog);
         
        pthread_t connection_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &connection_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread\n");
            return (NULL);
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //puts("Handler assigned\n");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed\n");
        return (NULL);
    }
     
    return (NULL);
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
     
    //Send some messages to the client
    //message = "Greetings! I am your connection handler\n";
    //write(sock , message , strlen(message));
     
    //message = "Now type something and i shall repeat what you type \n";
    //write(sock , message , strlen(message));
     
    char runcommand[1024];

    char server_response[]="server ok\n";
    char server_response2[]="You are connected\n";

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
	if(client_message[read_size-1]=='\n') client_message[read_size-1]=0;
        // display and speak the received message
        printf("RECV:Sock-%d:%s\n",sock,client_message);
        fprintf(fplog,"RECV:Sock-%d:%s\n",sock,client_message);
	fflush(fplog);
	sprintf(runcommand,"espeak \"%s\"",client_message);
	system(runcommand);
        //Send the message back to client
	if(strncmp(client_message,"trycorder:",10)==0) {
        	write(sock , "READY\n" , 6);
        	write(sock , server_response2 , strlen(server_response2));
        	printf("Responded:%s",server_response2);
        	fprintf(fplog,"Responded:%s",server_response2);
	} else {
                write(sock , server_response , strlen(server_response));
        	printf("Responded:%s",server_response);
        	fprintf(fplog,"Responded:%s",server_response);
        }
        fflush(stdout);
        fflush(fplog);
    }
     
    if(read_size == 0)
    {
        //puts("Client disconnected\n");
    }
    else if(read_size == -1)
    {
        perror("recv failed\n");
    }
    
    free(socket_desc);
    //Free the socket pointer
    printf("Closed Socket: Sock-%d\n",sock);
    fprintf(fplog,"Closed Socket: Sock-%d\n",sock);
    fflush(stdout);
    fflush(fplog);
     
    return 0;
}


