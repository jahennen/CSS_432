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
#include <sstream>

#define BUFSIZE 1500

using namespace std;

int acceptSocket;
int nreps;

void sigHandler(int i) {

	cout << "signal received" << endl;
	struct timeval startTime, endTime;

	int count;
	char databuf[BUFSIZE];
	gettimeofday( &startTime, NULL);
	for (int i = 0; i < nreps; i++) {
		for ( int nRead = 0; ( nRead += read( acceptSocket, databuf, BUFSIZE - nRead ) ) < BUFSIZE; ++count );
	}
	gettimeofday( &endTime, NULL);
	double elapsedTime;
	elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;      // sec to ms
	elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;   // us to ms
	cout << elapsedTime << " ms.\n";
	stringstream s;
	s << count;
	write(acceptSocket, s.str().c_str(), sizeof(s.str().c_str()));
	exit(0);
}

int main( int argc, char* argv[] ) {
    int port = atoi( argv[1] );
    nreps = atoi(argv[2]);

    Socket servSock(port);
    acceptSocket = servSock.getServerSocket();

    signal(SIGIO, sigHandler);
    fcntl(acceptSocket, F_SETOWN, getpid());
    fcntl(acceptSocket, F_SETFL, FASYNC);
    while(true) {};
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

