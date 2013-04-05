#include "Socket.h"
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

using namespace std;

int main( int argc, char* argv[] ) {

    // need to extract port number and IP address

    int port = atoi( argv[1] );

    cout << "Port: " << port << endl;

    //declare the socket to listen on, and the socket to use after accepting
    int listenSocket, acceptSocket;
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);


    //Declare a sockaddr_in structure, zero-initialize it by calling bzero(), and set its data
    // members as follows (htonl() converts an unsigned long from host byte order to network byte order):
    sockaddr_in acceptSockAddr;
    bzero( (char *) &acceptSockAddr, sizeof( acceptSockAddr ) );
    acceptSockAddr.sin_family      = AF_INET; // Address Family Internet
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port        = htons( port );
    /*Bind this socket to its local address by calling bind() as passing the following arguments:
        the socket descriptor (listenSocket)
        the sockaddr_in structure defined above
        the size of the sockaddr_in structure
     */
    bind( listenSocket, (sockaddr *) &acceptSockAddr, sizeof( acceptSockAddr ) );

    //Set the SO_REUSEADDR option (defined in the socket() interface).
    //(Note this option is useful to prompt OS to release the server port as soon as your server process is terminated.)
        const int on = 1;
        setsockopt( listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof( int ) );

    //Instruct the operating system to listen for up to 5 client connection requests at a time by calling listen().
        listen( listenSocket, 5 );

        //Receive a request from a client by calling accept(), which will return a new socket descriptor specific to each connection request.
        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof( newSockAddr );

        acceptSocket = accept( listenSocket, (sockaddr *) &newSockAddr, &newSockAddrSize );

        //No longer need to listen on this process
        close(listenSocket);
        string line;
        while(line != "quit" && acceptSocket) {
        	char buffer[16];
        	bzero( buffer, sizeof( buffer ) );
        	int count = read( acceptSocket, buffer, 15 );
        	line = buffer;

        	cout << "Read: '" << buffer << "'" << endl;

        	write( acceptSocket, buffer, count );
        }
    	close( acceptSocket );
        return 0;
}
