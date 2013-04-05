#include "Socket.h"

using namespace std;

int main( int argc, char* argv[] ) {

    // need to extract port number and IP address

    int port = atoi( argv[1] );
    char* ip_addr = argv[2];

    cout << "Port: " << port << "; ip: " << ip_addr <<  endl;

    // establish a connection through a socket

    Socket socket( port );
    int fd = socket.getClientSocket( ip_addr );

    cout << "Socket at fd " << fd <<  endl;

    // send a string to the server
    char* msg = "Test";
    write( fd, msg, strlen( msg ) );

    char buffer[16];
    bzero( buffer, sizeof( buffer ) );
    int count = read( fd, buffer, 15 );

    cout << "Confirming: " << buffer << endl;

    return 0;
}
