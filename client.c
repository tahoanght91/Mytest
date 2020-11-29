// This is program requesting distance data. This one is stored on PC or laptopn// as a client 
//In my case, serverIP (IP addr of Rasp is 10.42.0.138
// You can change serverIP follow your case

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/time.h>

void error(char *msg) {
    perror(msg);
    exit(0);
}

void sendData( int sockfd, float x ) {
  int n;

  char buffer[32];
  sprintf( buffer, "%f\n", x );
  if ( (n = write( sockfd, buffer, strlen(buffer) ) ) < 0 )
      error( const_cast<char *>( "ERROR writing to socket") );
  buffer[n] = '\0';
}

float getData( int sockfd ) {
  char buffer[32];
  int n;

  if ( (n = read(sockfd,buffer,31) ) < 0 )
       error( const_cast<char *>( "ERROR reading from socket") );
  buffer[n] = '\0';
  return atof(buffer);
}

int main(int argc, char *argv[])
{
    int sockfd, portno = 51717, n;
    char serverIp[] = "10.42.0.138";
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    int data;
    float dist_to_center;


    if (argc < 3) {
      // error( const_cast<char *>( "usage myClient2 hostname port\n" ) );
      printf( "contacting %s on port %d\n", serverIp, portno );
      // exit(0);
    }
    if ( ( sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
        error( const_cast<char *>( "ERROR opening socket") );

    if ( ( server = gethostbyname( serverIp ) ) == NULL ) 
        error( const_cast<char *>("ERROR, no such host\n") );
    
    bzero( (char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if ( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error( const_cast<char *>( "ERROR connecting") );
    n =1;
    while(1){
     
   sendData( sockfd, n );
     
      dist_to_center = getData( sockfd );
      
      printf("Measured distance at time %d is ->  %f\n",n, dist_to_center);
   

    n++;
    
    }
    return 0;
}
