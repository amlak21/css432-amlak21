// TCP socket programming 
// client.c
//created by:Amlak T 

#define _XOPEN_SOURCE 600 //for issues with incoplete struct type....
#include <sys/types.h>	// socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>	// inet_ntoa
#include <netdb.h>    	// gethostbyname
#include <unistd.h>   	// read, write, close
#include <strings.h>  	// bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>  	// writev
#include <sys/time.h>  // for gettimeofday()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main(int argc, char *argv[])
{
     if(argc != 7)
    {
        printf("Error: Invalid number of arguments\n");
    }
    
    char* serverPort = argv[1]; // server's port number
    char* serverName = argv[2]; // server's IP address or host name
    int iterations = atoi(argv[3]); //the number of iterations for "single write",
                                    // "writev" or "multiple writes".

    int nbufs = atoi(argv[4]); // the number of data buffers
    int bufsize = atoi(argv[5]); // the size of each data buffer (in bytes)
    int type = atoi(argv[6]); // the type of transfer scenario: 1, 2, or 3

   //Declare an addrinfo structure hints, zero-initialize it
    struct addrinfo hints;
    struct addrinfo *servInfo;
    memset(&hints, 0, sizeof(hints) );
    hints.ai_family  	= AF_UNSPEC; // Address Family Internet
    hints.ai_socktype	= SOCK_STREAM; // TCP
    getaddrinfo(serverName, serverPort, &hints, &servInfo );

    //open a stream-oriented socket with the Internet address family.
    int clientSd = socket( servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol );

    //Connect socket to the server
    connect( clientSd, servInfo->ai_addr, servInfo->ai_addrlen);

    //Allocate databuf[nbufs][bufsize].
    char databuf[nbufs][bufsize]; // where nbufs * bufsize = 1500

    struct timeval start, lap, stop;
    long long int data_transmission_time,  round_trip_time; // in micro seconds
    gettimeofday(&start, NULL); //Start a timer 

    //Repeat the iteration times of data transfers
    //specified type as 1: multiple writes, 2: writev, or 3: single write
    if(type == 1) // multiple writes
    {
        for ( int i = 0; i < iterations; i++)
        {
            for ( int j = 0; j < nbufs; j++ )
            {
                write( clientSd, databuf[j], bufsize );	// sd: socket descriptor
            } 
        }
    }

    else if(type == 2) // writev
    {
        for ( int i = 0; i < iterations; i++)
        {
            struct iovec vector[nbufs];
            for ( int j = 0; j < nbufs; j++ ) 
            {
                vector[j].iov_base = databuf[j];
                vector[j].iov_len = bufsize;
            }
            writev( clientSd, vector, nbufs );       	
        }
    }

    else if(type == 3) // single write
    {
        for ( int i = 0; i < iterations; i++)
        {
            write( clientSd, databuf, nbufs * bufsize ); 
        }
    }
    
    else
    {
        printf("Error: Unknown type of data transfer\n");
        exit(1);
    }

     gettimeofday(&lap, NULL); //Lap the timer 
     data_transmission_time = (((lap.tv_sec * 1000000) + lap.tv_usec) - 
     ((start.tv_sec * 1000000) + start.tv_usec)); // where lap - start = data-transmission time

    //Receive the number of reads from the server as an acknowledgement 
    int reads;
    void* reads_ptr = &reads;
    read(clientSd, reads_ptr, sizeof(reads)); // read number of count from client socket 
    
    gettimeofday(&stop, NULL); //Stop the timer
    round_trip_time = (((stop.tv_sec * 1000000) + stop.tv_usec) - 
     ((start.tv_sec * 1000000) + start.tv_usec)); //where stop - start = round-trip time.

    //Print out the statistics
    printf("iterations = %d, nbufs = %d, bufsize = %d\n",iterations, nbufs, bufsize);
    printf("Test %d: data-transmission time = %lli usec, round-trip time = %lli usec, #reads = %d times\n ", type,
    data_transmission_time, round_trip_time, reads);

    close(clientSd); //Close the socket.
   



    return 0;
}




