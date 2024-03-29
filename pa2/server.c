
// server.c
// created by: Amlak T
//TCP socket programming 
//Purpose:
// creates a client/server and evaluates the number of reads and writes
// made by client/server. 


#define _XOPEN_SOURCE 600 // for issues with incomplete struct type
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
#include <pthread.h>    // for threads


const int BUFSIZE = 1500;

//structure for server_connection_handler parameters
// to be passed as an argument to pthread_create function
struct iteration_sd
{
    int num_iterations;
    int socket_fd;
};


// helper function for server
void server_connection_handler( int iterations,int sd)
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

    gettimeofday(&stop, NULL);   //Stop the timer 
    data_receiving_time = (((stop.tv_sec * 1000000) + stop.tv_usec) - 
     ((start.tv_sec * 1000000) + start.tv_usec)); //where stop - start = data-receiving time
  
    //Send the number of read calls(i.e count) made to client as an acknowledgement.
   void* count_ptr = &count;
    write(sd, count_ptr, sizeof(count)); // write count to server socket
    printf("data-receiving time = %lli usec, read count = %d\n",data_receiving_time, count);

    close(sd); // close this connection
}

// function in thread that calls server_connection_handler function in it
void* thread_function(void* func_parameters) 
{
    struct iteration_sd * arguments = (struct iteration_sd *)func_parameters;
    int number_of_iterations = arguments->num_iterations; //get iterations from struct
    int s_fd = arguments->socket_fd; // get socket from struct
    server_connection_handler(number_of_iterations, s_fd);  // call server helper function and excute it

    pthread_exit(NULL); // close thread
    return NULL;
}



int main(int argc, char *argv[])
{

    if(argc != 3)
    {
        printf("Error: Invalid number of arguments\n");
        exit(1);
    }

    // prog_name, port number, number of iterations(must be same with client)
    char* port_number = argv[1];
    int iterations = atoi(argv[2]);

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
    int n = 50;
    listen( serverSd,  n );
    printf("listening for incoming request...\n");

    pthread_t tid = 0; //thread id
    struct iteration_sd itr_sd_args; //struct for input iteration and new socket
    itr_sd_args.num_iterations = iterations; // assign input iteration to struct iteration
    
    while(1) //Loop back to the accept command and wait for a new connection
    {
        //Receive a request from a client and create a new socket for this connection
        struct sockaddr_storage newSockAddr;
        socklen_t newSockAddrSize = sizeof( newSockAddr );
        int newSd = accept( serverSd, (struct sockaddr *)&newSockAddr, &newSockAddrSize );

        itr_sd_args.socket_fd = newSd; // assign new sockt to struct socket
        printf("accepted a request.\n");

        //Create a new thread
        // the new thread will excute sever helper functions
        if(pthread_create(&tid, NULL, &thread_function, (void*)&itr_sd_args) != 0)
        {
            printf("Error: can't create thread\n");
            exit(1);
        }
        tid++;
    }
    return 0;
}