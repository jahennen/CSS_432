
#include "Socket.h"
#include "Timer.h"

using namespace std;

void main(int argc, char* argv[]) {
	Socket ctrl;
	int ctrlint;

	Socket data;
	int dataint;

	string cmd;
	while(1) {
		cin >> cmd;
		switch (cmd) {

		case "open":
			if(argv[2]){
				ctrl.Socket(21);
				ctrlint = ctrl.getClientSocket(argv[1]);

				data.Socket(argv[2]);
				dataint = data.getClientSocket(argv[1]);
			}

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
