// server.c
// created by: Amlak T

#define _XOPEN_SOURCE 600 //?? for issues with incoplete struct type....
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>	// socket, bind
#include <sys/socket.h>  // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>	// inet_ntoa
#include <netdb.h>   	// gethostbyname
#include <unistd.h>   	// read, write, close
#include <strings.h>  	// bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>  	// writev
#include <sys/time.h>  // for gettimeofday()




const int BUFSIZE = 1500;

// function

void server_connection_handler(int iterations, int sd)
{

    char databuf[BUFSIZE]; //Allocate databuf[BUFSIZE]
    struct timeval start, stop;
    long long int data_receiving_time; // in micro seconds
    gettimeofday(&start, NULL); //Start a timer by calling gettimeofday

    //Repeat reading data from the client into databuf[BUFSIZE]
    int count = 0;
    for ( int i = 0 ; i < iterations; i++ ) 
    {
        for ( int nRead = 0;
        ( nRead += read( sd, databuf, BUFSIZE - nRead ) ) < BUFSIZE;    
 	    ++count );
    }

    gettimeofday(&stop, NULL);   //Stop the timer by calling gettimeofday
    data_receiving_time = (((stop.tv_sec * 1000000) + stop.tv_usec) - 
     ((start.tv_sec * 1000000) + start.tv_usec)); //where stop - start = data-receiving time
  
    //Send the number of read( ) calls made, (i.e., count in the above) as an acknowledgement.
   void* count_ptr = &count;
    write(sd, count_ptr, sizeof(count)); // write count to sd 

    //Print out as data-receiving time = xxx usec
    printf("read count = %d\n",count);

    printf("data-receiving time = %d usec\n",data_receiving_time);

    //Close this connection.
    close(sd); // close newSd
    //Optionally, terminate the server process by calling exit( 0 ).  (This might make it easier for debugging at first. You would not want to do this on a multi-threaded server).
}



int main(int argc, char *argv[])
{

    if(argc != 3)
    {
        printf("Error: Invalid number of arguments\n");
        exit(1);
    }

    // prog_name, port number, number of iterations(must be same wiht client)
    char* prog_name = argv[0];
    char* port_number = argv[1];
    int number_of_iterations = atoi(argv[2]);

    //Declare an addrinfo structure, zero-initialize it, and set its data members
   
    struct addrinfo hints, *res;
    memset( &hints, 0, sizeof(hints));
    hints.ai_family  	= AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    getaddrinfo(NULL, port_number, &hints, &res);

    //Open a stream-oriented socket with the Internet address family.
    int serverSd = socket(res->ai_family, res->ai_socktype, res->ai_protocol );

    //Set the SO_REUSEADDR option
    const int yes = 1;
    setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof( yes ) );
    
    // bind socket to local address
    bind( serverSd, res->ai_addr, res->ai_addrlen );

    //listen to up to n connection requests
    int n = 10;
    listen( serverSd,  n );
    printf("listening for incoming request...\n");

    
    
    while(1) ////Loop back to the accept command and wait for a new connection
    {
        //Receive a request from a client and create a new socket for this connestion
        struct sockaddr_storage newSockAddr;
        socklen_t newSockAddrSize = sizeof( newSockAddr );
        int newSd = accept( serverSd, (struct sockaddr *)&newSockAddr, &newSockAddrSize );
        printf("accepted a request...\n\n");

        //Accept a new connection and create a new socket ->done above
        //Execute your_function - read from client and write back to client
        server_connection_handler(number_of_iterations, newSd);
    }
    
    //Loop back to the accept command and wait for a new connection

    return 0;
}