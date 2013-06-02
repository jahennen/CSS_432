
#include "Socket.h"
#include "Timer.h"

using namespace std;

void main(int argc, char* argv[]) {
	int ctrl;	//Socket constructor returns an int to the location of the socket
	int data;

	string cmd;
	while(1) {
		cin >> cmd;
		switch (cmd) {

		case "open":
			break;

		case "password":
			break;

		case "cd":
			break;

		case "ls":
			break;

		case "get":
			break;

		case "put":
			break;

		case "close":
			close(data);
			break;

		case "quit":
			exit(0);
			break;

		}
	}


}
//
