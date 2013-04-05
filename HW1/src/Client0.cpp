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

#include <iostream>

using namespace std;

int main( int argc, char* argv[] ) {

    // need to extract port number and IP address

    int port = atoi( argv[1] );
    char* ip_addr = argv[2];

    cout << "Port: " << port << "; ip: " << ip_addr <<  endl;

    // establish a connection through a socket


    // Get server info

    struct hostent* host = gethostbyname( ip_addr );

    if (host < 0) {
    	cout << "ERROR, no such host" << endl;
    	exit(0);
    }

    sockaddr_in sendSockAddr;
    bzero( (char *) &sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET; // Address Family Internet
    sendSockAddr.sin_addr.s_addr =
    		inet_addr( inet_ntoa( *(struct in_addr *) *host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( port );  // convert host byte-order

    int clientSd = socket( AF_INET, SOCK_STREAM, 0 );

    connect( clientSd, (sockaddr *) &sendSockAddr, sizeof( sendSockAddr ) );

    // send a string to the server
    string line;
    while (line != "quit") {
    	getline(cin, line);
    	const char* msg = line.c_str();
    	write( clientSd, msg, strlen( msg ) );

    	char buffer[16];
    	bzero( buffer, sizeof( buffer ) );
    	int count = read( clientSd, buffer, 15 );

    	cout << "Confirming: " << buffer << endl;
    }
    return 0;
}
