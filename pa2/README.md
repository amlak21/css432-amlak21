#How to compile/build

Step 1: open two different csslab machines in a terminal. One for server and the other for client.

    e.g. user@csslab4 pa2% and user@csslab10 pa2%
    
Step 2: run make in either one of them. It will compile and create excutable file client from client.c and server from server.c.

    e.g. user@csslab4 pa2%make

#How to run a test

First run server on one csslab machine, then client on differnt csslab machine.

For server: ./server port_number iterations

    e.g. user@csslab10 pa2%./server 51091 20000
    
For client: ./client port_number servername iterations nbufs bufsize type

    e.g. user@csslab4 pa2%./client 51091 csslab10.uwb.edu 20000 15 100 1

Note: port_number and iterations must be the same for client and server

#For multithread testing

run multiple clients simultaneously
