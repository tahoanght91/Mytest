// This is program sending measured distance to PC client. This program is stored and running on Rasp as a server
// Need to install librealsense of intel realsense camera
// to build this programe into executable file, let use command as: 
//  g++ -o server server.c -lrealsense2
//  to run it:  ./server

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include </home/ubuntu/test/rs_net.hpp>
#include <librealsense2/rs.hpp>



void error( char *msg ) {
  perror(  msg );
  exit(1);
}


void sendData( int sockfd, float x ) {
  int n;

  char buffer[32];
  sprintf( buffer, "%f\n", x );
  if ( (n = write( sockfd, buffer, strlen(buffer) ) ) < 0 )
    error( const_cast<char *>( "ERROR writing to socket") );
  buffer[n] = '\0';
}

int getData( int sockfd ) {
  char buffer[32];
  int n;

  if ( (n = read(sockfd,buffer,31) ) < 0 )
    error( const_cast<char *>( "ERROR reading from socket") );
  buffer[n] = '\0';
  return atoi( buffer );
}

int main(int argc, char *argv[]) {
     int sockfd, newsockfd, portno = 51717, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int data;


   //  rs2::net_device dev("10.42.0.1");
   //  rs2::context ctx; dev.add_to(ctx);

     printf( "using port #%d\n", portno );
    
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
         error( const_cast<char *>("ERROR opening socket") );
     bzero((char *) &serv_addr, sizeof(serv_addr));

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons( portno );
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
       error( const_cast<char *>( "ERROR on binding" ) );
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
  
     rs2::pipeline p;
     p.start();

     //--- infinite wait on a connection ---
     while ( 1 ) {
        printf( "waiting for new client...\n" );
        if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 )
            error( const_cast<char *>("ERROR on accept") );
        printf( "opened new communication with client\n" );
        while ( 1 ) {
	     //---- wait for a number from client ---
             data = getData( newsockfd );
             printf( "got %d\n", data );
             if ( data < 0 ) 
                break;
                

                rs2::frameset frame = p.wait_for_frames();
                rs2::depth_frame depth = frame.get_depth_frame();
                float width = depth.get_width();
                float height = depth.get_height();
                float dist_to_center = depth.get_distance(width / 2, height / 2);
               

             //--- send new data back --- 
	     printf( "sending back %f \n", dist_to_center );
             sendData( newsockfd, dist_to_center);
	}
        close( newsockfd );

        //--- if -2 sent by client, we can quit ---
        if ( data == -2 )
          break;
     }
     return 0; 
}
