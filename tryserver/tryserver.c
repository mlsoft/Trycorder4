/*
    C socket server example, handles multiple clients using threads
*/
 
#include<stdio.h>
#include<fcntl.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
 
//the thread function
void *connection_handler(void *);
static FILE *fp;
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
        return(1);
    }
    //puts("Socket created\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 1701 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error\n");
        return 1;
    }
    //puts("bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    //puts("Waiting for incoming connections...");
    //c = sizeof(struct sockaddr_in);
     
    fp=fopen("tryserver.log","a");
    
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        //puts("Connection accepted\n");
        printf("From:%d.%d.%d.%d\n",
            (int)(client.sin_addr.s_addr&0xFF),
            (int)((client.sin_addr.s_addr&0xFF00)>>8),
            (int)((client.sin_addr.s_addr&0xFF0000)>>16),
            (int)((client.sin_addr.s_addr&0xFF000000)>>24));
        fprintf(fp,"From:%d.%d.%d.%d\n",
            (int)(client.sin_addr.s_addr&0xFF),
            (int)((client.sin_addr.s_addr&0xFF00)>>8),
            (int)((client.sin_addr.s_addr&0xFF0000)>>16),
            (int)((client.sin_addr.s_addr&0xFF000000)>>24));
	fflush(fp);
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread\n");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //puts("Handler assigned\n");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed\n");
        return 1;
    }
     
    fclose(fp);
    
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
     
    //Send some messages to the client
    //message = "Greetings! I am your connection handler\n";
    //write(sock , message , strlen(message));
     
    //message = "Now type something and i shall repeat what you type \n";
    //write(sock , message , strlen(message));
     
    char runcommand[1024];

    char server_response[]="server ok\n";

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , server_response , strlen(server_response));
        printf("RECV:%s\n",client_message);
        fprintf(fp,"RECV:%s\n",client_message);
	fflush(fp);
	sprintf(runcommand,"espeak \"%s\"",client_message);
	system(runcommand);
    }
     
    if(read_size == 0)
    {
        //puts("Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed\n");
    }
    
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}


