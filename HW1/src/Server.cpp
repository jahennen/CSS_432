/*
 * Author: Jay Hennen
 * CSS_432 HW1
 */

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

#define BUFSIZE 1500

using namespace std;

int acceptSocket;
int nreps;

// Reads BUFSIZE bytes from the socket nreps times and conducts timing.
// Outputs timing data to cout, and raw data to cerr, then exits.
void sigHandler(int i) {
	struct timeval startTime, endTime;
	int count;
	char databuf[BUFSIZE];
	gettimeofday( &startTime, NULL);
	for (int i = 0; i < nreps; i++) {
		for ( int nRead = 0; ( nRead += read( acceptSocket, databuf + nRead, BUFSIZE - nRead ) ) < BUFSIZE; ++count );
	}
	gettimeofday( &endTime, NULL);
	long receiveTime;
	receiveTime = (endTime.tv_sec - startTime.tv_sec) * 1000000;      // sec to ms
	receiveTime += (endTime.tv_usec - startTime.tv_usec);   // us to ms
	cout << "data-receiving time = " << receiveTime << " usec" << endl;
	cerr << receiveTime << endl; // Output raw data
	write(acceptSocket, &count, sizeof(count));
	exit(0);
}

int main( int argc, char* argv[] ) {
    int port = atoi( argv[1] );
    nreps = atoi(argv[2]);

    Socket servSock(port);
    acceptSocket = servSock.getServerSocket();

    // Put the socket in asynchronous mode.
    signal(SIGIO, sigHandler);
    fcntl(acceptSocket, F_SETOWN, getpid());
    fcntl(acceptSocket, F_SETFL, FASYNC);

    while(true) {
    	sleep(1000);
    };
    return 0;
}

