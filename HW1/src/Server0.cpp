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


    //Declare a sockaddr_in structure, zero-initialize it by calling bzero(), and set its data members as follows (htonl() converts an unsigned long from host byte order to network byte order):

        sockaddr_in acceptSockAddr;
        bzero( (char *) &acceptSockAddr, sizeof( acceptSockAddr ) );
        acceptSockAddr.sin_family      = AF_INET; // Address Family Internet
        acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
        acceptSockAddr.sin_port        = htons( port );

    //Open a stream-oriented socket() with the Internet address family.

        int serverSd = socket( AF_INET, SOCK_STREAM, 0 );

    //Set the SO_REUSEADDR option (defined in the socket() interface).
    //(Note this option is useful to prompt OS to release the server port as soon as your server process is terminated.)

        const int on = 1;
        setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof( int ) );

    /*Bind this socket to its local address by calling bind() as passing the following arguments:
        the socket descriptor (serverSd)
        the sockaddr_in structure defined above
        the size of the sockaddr_in structure
        */

        bind( serverSd, (sockaddr *) &acceptSockAddr, sizeof( acceptSockAddr ) );

    //Instruct the operating system to listen for up to 5 client connection requests at a time by calling listen().

        listen( serverSd, 5 );

    //Receive a request from a client by calling accept(), which will return a new socket descriptor specific to each connection request.

        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof( newSockAddr );
        int newSd = accept( serverSd, (sockaddr *) &newSockAddr, &newSockAddrSize );

    /*Use the read() system call to receive data from the client. (Use newSd rather than serverSd in the above code example.)
    Use the write() system call to send back a response to the client. (Use newSd rather than serverSd in the above code example.)
    Close the socket by calling close().
    */

        // establish a connection through a socket

        Socket socket( port );
        int fd = socket.getServerSocket();

        cout << "Socket at fd " << fd <<  endl;

        // receive a string from the client

        char buffer[16];
        bzero( buffer, sizeof( buffer ) );
        int count = read( fd, buffer, 15 );

        cout << "Read: '" << buffer << "'" << endl;

        write( fd, buffer, count );

        close( newSd );

    return 0;
}
