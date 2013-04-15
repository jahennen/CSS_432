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

    cout << "Port: " << port << "; ip: " << ip_addr <<  endl;

    if (nbufs * bufsize != 1500) {
    	cout << "nbufs * bufsize must equal 1500" << endl;
    	exit(1);
    }

    char databuf[nbufs][bufsize];

    // Get socket file descriptor
    Socket cliSock(port);
    int clientFd = cliSock.getClientSocket( ip_addr);
    for (int i = 0; i < nreps; i++) {
    	for (int j = 0; j < nbufs; j++) {
    		if (type == 1) {
    			write(clientFd, databuf[j], bufsize);
    		} else if (type == 2) {
    			struct iovec vector[nbufs];
    			vector[j].iov_base = databuf[j];
    			vector[j].iov_len = bufsize;
    			writev(clientFd, vector, nbufs);
    		} else {
    			write( clientFd, databuf, nbufs * bufsize );
    		}
    	}
    }

    char countbuf[4];

    read(clientFd, countbuf, 4); // get the time result
    if (atoi(countbuf) != nbufs)
    	cout << "reads != writes" << endl;
    cout << atoi(countbuf) << endl;

//    // send a string to the server
//    string line;
//    while (line != "quit") {
//    	getline(cin, line);
//    	const char* msg = line.c_str();
//    	write( clientFd, msg, strlen( msg ) );
//
//    	char buffer[16];
//    	bzero( buffer, sizeof( buffer ) );
//    	int count = read( clientFd, buffer, 15 );
//
//    	cout << "Confirming: " << buffer << endl;
//    }
    return 0;
}
