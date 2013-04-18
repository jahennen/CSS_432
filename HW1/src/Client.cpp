/*
 * Author: Jay Hennen
 * CSS_432 HW1
 */

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
#include <sys/time.h>

#include <iostream>

using namespace std;

// Usage ./client [port] [nreps] [nbufs] [bufsize] [ip/hostname] [type]
// Connects to a server process with the above parameters and transmits nbufs
// buffers of bufsize size nreps times, while timing the transmit and
// confirmation time. Outputs timing data to cout and raw data to cerr.
int main( int argc, char* argv[] ) {

	if (argc < 7 || atoi(argv[6]) > 3 || atoi(argv[6]) < 1) {
		cout << "Bad parameters" << endl;
		exit(1);
	}
    int port = atoi( argv[1] );
    int nreps = atoi(argv[2]);
    int nbufs = atoi(argv[3]);
    int bufsize = atoi(argv[4]);
    char* ip_addr = argv[5];
    int type = atoi(argv[6]);

    if (nbufs * bufsize != 1500) {
    	cout << "nbufs * bufsize must equal 1500" << endl;
    	exit(1);
    }

    char databuf[nbufs][bufsize];

    // Get socket file descriptor
    Socket cliSock(port);
    int clientFd = cliSock.getClientSocket( ip_addr);

    struct timeval startTime, endTime, lap;
    gettimeofday(&startTime, NULL);
    for (int i = 0; i < nreps; i++) {
    		if (type == 1) {			// nbufs buffers of bufsize
    			for (int j = 0; j < nbufs; j++)
    			write(clientFd, databuf[j], bufsize);
    		} else if (type == 2) {		// writev of a vector of nbufs
    			struct iovec vector[nbufs];
    			for (int j = 0; j < nbufs; j++) {
    			vector[j].iov_base = databuf[j];
    			vector[j].iov_len = bufsize;
    			}
    			writev(clientFd, vector, nbufs);
    		} else {					// write nbufs*bufsize all at once
    			write( clientFd, databuf, nbufs * bufsize );
    		}
    }
    gettimeofday(&lap, NULL);

    int count;
    read(clientFd, &count, sizeof(count)); // get the time result
    gettimeofday(&endTime, NULL);
	long lapTime, elapsedTime;
	elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000000;      // sec to ms
	elapsedTime += (endTime.tv_usec - startTime.tv_usec);   // us to ms

	lapTime = (lap.tv_sec - startTime.tv_sec) * 1000000;      // sec to ms
	lapTime += (lap.tv_usec - startTime.tv_usec);   // us to ms

	// Test 1: data-sending time = xxx usec, round-trip time = yyy usec, # reads = zzz
	cout << "Test " << type << ": ";
	cout << "data-sending time = " << lapTime << " usec, ";
	cout << "round-trip time = " << elapsedTime << " usec, ";
	cout << "# reads = " << count << endl;

    cerr << lapTime << "\t" << elapsedTime << "\t" << count << endl;
    return 0;
}
