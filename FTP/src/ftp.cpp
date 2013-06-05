
#include "Socket.h"
#include "Timer.h"
#include <sstream>
#include <sys/poll.h>

#define BUFLEN 1500

using namespace std;

Socket ctrl(21);

void cmd_name(int& ctrlfd, string& name);
void cmd_passwd(int& ctrlfd, string& passwd);

//currently WILL FREEZE if server does not accept ftp connections on the port specified
void cmd_open(int& ctrlfd, Socket& ctrl, string& hostname, string& port) {
	char message[BUFLEN];
	ctrl = Socket(atoi(port.c_str()));
	ctrlfd = ctrl.getClientSocket((char*)(hostname.c_str()));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
	string name;
	cout << "Name: ";
	getline (cin, name);
	cmd_name(ctrlfd, name);
}

void cmd_name(int& ctrlfd, string& name) {
	char message[BUFLEN];
	stringstream cmd;
	cmd << "USER " << name << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
	string passwd;
	cout.clear();
	cout << "Password: ";
	getline(cin, passwd);
	cmd_passwd(ctrlfd, passwd);
}

void cmd_passwd(int& ctrlfd, string& passwd) {
	char message[BUFLEN];
	stringstream cmd;
	cmd << "PASS " << passwd << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));

	struct pollfd ufds;

	ufds.fd = ctrlfd;                     // a socket descriptor to exmaine for read
	ufds.events = POLLIN;             // check if this sd is ready to read
	ufds.revents = 0;                 // simply zero-initialized

	while ( poll(&ufds, 1, 1000) >0 ) {                  // the socket is ready to read
		int n = read( ctrlfd, message, BUFLEN ); // guaranteed to return from read
		message[n] = '\0';
		cout << message;
	}
}

void cmd_close(int& ctrlfd) {
	char message[BUFLEN];
	stringstream cmd;
	cmd << "QUIT" << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
	close(ctrlfd);
	ctrlfd = 0;
}

int main(int argc, char* argv[]) {
	int ctrlfd;

//	Socket data(23);
//	int datafd;

	while(1) {
		cout << "ftp> ";
		string input;
		stringstream inputss;
		string cmd, arg1, arg2;
		getline(cin, input);
		inputss.str(input);
		inputss >> cmd >> arg1 >> arg2;

		if (cmd=="open") {
			if (arg1!="" && arg2!="")
				cmd_open(ctrlfd, ctrl, arg1, arg2);
			else
				cout << "usage: open <hostname> <port>" << endl;
		} else if (cmd == "quit") {
			if (ctrlfd != 0)
				cmd_close(ctrlfd);
			break;
		} else if (ctrlfd == 0 ) {
			cout << "Not connected" << endl;
			continue;
		} else if (cmd == "name") {
			cmd_name(ctrlfd, arg1);
		} else if (cmd == "password") {
			cmd_passwd(ctrlfd, arg1);
		} else if (cmd == "cd") {

		} else if (cmd == "ls") {

		} else if (cmd == "close") {
			cmd_close(ctrlfd);
		}
	}
	return 0;
}
