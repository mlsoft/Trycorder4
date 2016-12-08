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
void listconnclient();

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

    // the main thread loop to input commands from command-line 
    // and ask to send it to all connected sockets
    while(1) {
      char *s=fgets(inputline,200,stdin);
      if(s!=NULL) {
	if(strncmp(inputline,"list",4) ==0) {
	  listconnclient();
	}
	// send this command to all trycorders
	sendall(inputline);
      }
    }
    
    fclose(fplog);
 
    return(0);
}

// data block passed to the connection thread
// with all data to handle a client connection
struct ConnClient {
  struct sockaddr_in client;
  int clientlen;
  int client_sock;
  char ipaddr[64];
};

static struct ConnClient *connclient[256];
static int nbconnclient=0;

void addconnclient(struct ConnClient *conn) {
  if(nbconnclient>255) return;
  connclient[nbconnclient]=conn;
  nbconnclient++;
}

void delconnclient(struct ConnClient *conn) {
  if(nbconnclient==0) return;
  int i;
  for(i=0;i<nbconnclient;++i) {
    if(conn==connclient[i]) {
      int j;
      for(j=i;j<(nbconnclient-1);++j) {
	connclient[j]=connclient[j+1];
      }
      nbconnclient--;
      break;
    }
  }
}

void listconnclient() {
  int i;
  for (i=0;i<nbconnclient;++i) {
    printf("Client:%s:Sock:%d\n",connclient[i]->ipaddr,connclient[i]->client_sock);
  }
}

// this will handle the mechanic to send this master message 
// to all trycorders connected
void sendall(char *line) {
  if(nbconnclient<=0) return;
  int i;
  for (i=0;i<nbconnclient;++i) {
    int sock=connclient[i]->client_sock;
    int res=write(sock , line , strlen(line));
    printf("Send:%s:%s\n",connclient[i]->ipaddr,line);
    fprintf(fplog,"Send:%s:%s\n",connclient[i]->ipaddr,line);
  }
  fflush(stdout);
  fflush(fplog);
  return;
}


/*
 * This will wait for connection from each client
 * */

void* listener_handler(void* threadname)
{

    int socket_desc ;
    struct sockaddr_in server ;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
        exit(1);
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
        exit(1);
    }
    //puts("bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...\n");
    
    // loop to accept incoming client connections
    
    struct ConnClient *conn;
    
    while(1) {
	conn=(struct ConnClient *) malloc(sizeof(struct ConnClient));
	conn->clientlen=sizeof(struct sockaddr_in);
	
	conn->client_sock = accept(socket_desc, (struct sockaddr *)&(conn->client), (socklen_t*)&(conn->clientlen));
	if(conn->client_sock<=0) break;
	
        printf("\nConnection accepted: Sock-%d\n",conn->client_sock);
        fprintf(fplog,"\nConnection accepted: Sock-%d\n",conn->client_sock);
        sprintf(conn->ipaddr,"%d.%d.%d.%d",
            (int)(conn->client.sin_addr.s_addr&0xFF),
            (int)((conn->client.sin_addr.s_addr&0xFF00)>>8),
            (int)((conn->client.sin_addr.s_addr&0xFF0000)>>16),
            (int)((conn->client.sin_addr.s_addr&0xFF000000)>>24));
         
        pthread_t connection_thread;
         
        if( pthread_create( &connection_thread , NULL ,  connection_handler , (void*) conn) < 0)
        {
            perror("could not create thread\n");
            return (NULL);
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //puts("Handler assigned\n");
    }
     
    if (conn->client_sock < 0)
    {
        perror("accept failed\n");
        return (NULL);
    }
     
    return (NULL);
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *connvoid)
{
    //Get the socket descriptor
    struct ConnClient *conn = (struct ConnClient *)connvoid;
    int sock = conn->client_sock;
    int read_size;
    char client_message[2000];
    char runcommand[1024];
    char server_response[]="server ok\n";
    char server_response2[]="You are connected\n";

    // log connection from
    printf("From:%s\n",conn->ipaddr);
    fprintf(fplog,"From:%s\n",conn->ipaddr);
    fflush(stdout);
    fflush(fplog);
     
    // Receive a message from client, 
    // and continue to wait for more until the client close the connection
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
	// remove the last '\n' from the message
	if(client_message[read_size-1]=='\n') client_message[read_size-1]=0;

	// display and speak the received message
        printf("RECV:Sock-%d:%s\n",sock,client_message);
        fprintf(fplog,"RECV:Sock-%d:%s\n",sock,client_message);
	fflush(fplog);
	sprintf(runcommand,"espeak \"%s\"",client_message);
	int res;
	res=system(runcommand);

	//Send the response back to client
	if(strncmp(client_message,"trycorder:",10)==0) {
        	// this is a permanent link with the client
		res=write(sock , "READY\n" , 6);
        	res=write(sock , server_response2 , strlen(server_response2));
        	printf("Responded:%s",server_response2);
        	fprintf(fplog,"Responded:%s",server_response2);
		// add this thread/connection to the list of permanent threads
		addconnclient(conn);
	} else {
		// this is a temporary, one-shot message from the client
                res=write(sock , server_response , strlen(server_response));
        	printf("Responded:%s",server_response);
        	fprintf(fplog,"Responded:%s",server_response);
        }
        fflush(stdout);
        fflush(fplog);
	
	// send command back to all clients except the sender
	if(nbconnclient>0) {
	  int i;
	  for (i=0;i<nbconnclient;++i) {
	    if(connclient[i]==conn) continue;
	    int sock=connclient[i]->client_sock;
	    int res=write(sock , client_message , strlen(client_message));
	    printf("Send:%s:%s\n",connclient[i]->ipaddr,client_message);
	    fprintf(fplog,"Send:%s:%s\n",connclient[i]->ipaddr,client_message);
	  }
	  fflush(stdout);
	  fflush(fplog);
	}
	
    }
     
    if(read_size == 0)
    {
        //puts("Client disconnected\n");
    }
    else if(read_size == -1)
    {
        perror("recv failed\n");
    }
    
    // delete our structure from the list,
    // and free the structure 
    delconnclient(conn);
    free(conn);
    
    //Free the socket pointer
    printf("Closed Socket: Sock-%d\n",sock);
    fprintf(fplog,"Closed Socket: Sock-%d\n",sock);
    fflush(stdout);
    fflush(fplog);
     
    return 0;
}


