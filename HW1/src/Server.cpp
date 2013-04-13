#include "Socket.h"
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <sys/time.h>

using namespace std;

int acceptSocket;
int nreps;

void my_function(int i) {

	struct timeval startTime, endTime;
	gettimeofday( &startTime, NULL);
	for (int i = 0; i < nreps; i++) {

	}
}

int main( int argc, char* argv[] ) {
    int port = atoi( argv[1] );
    nreps = atoi(argv[2]);

    Socket servSock(port);
    acceptSocket = servSock.getServerSocket();

    signal(SIGIO, my_function);
    fcntl(acceptSocket, F_SETOWN, getpid());
    fcntl(acceptSocket, F_SETFL, FASYNC);
//        string line;
//        while(line != "quit" && acceptSocket) {
//        	char buffer[16];
//        	bzero( buffer, sizeof( buffer ) );
//        	int count = read( acceptSocket, buffer, 15 );
//        	line = buffer;
//
//        	cout << "Read: '" << buffer << "'" << endl;
//
//        	write( acceptSocket, buffer, count );
//        }
//    	close( acceptSocket );
        return 0;
}

