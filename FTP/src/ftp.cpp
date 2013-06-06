
#include "Socket.h"
#include "Timer.h"
#include <sstream>
#include <vector>
#include <sys/poll.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fstream>

#define BUFLEN 1500

using namespace std;

Socket ctrl(21);
Socket data(23);

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

void cmd_cd(int& ctrlfd, string& subdir) {
	char message[BUFLEN];
	stringstream cmd;
	cmd << "CWD " << subdir << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
}

vector<string> pasv_port(int& ctrlfd) {
	char message[BUFLEN];
	stringstream cmd;
	cmd << "PASV" << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
	string res(message);
	res = res.substr(res.find('(')+1);
	res[res.find(')')]=',';
	stringstream nums(res);
	vector<string> tuple(6);
	for (int i = 0; i < 6; i++) {
		char temp[5];
		nums.getline(temp,4 ,',');
		tuple[i] = temp;
	}
	return tuple;
}

void cmd_ls(int& ctrlfd, int& datafd) {
	char message[BUFLEN];
	vector<string> tuple = pasv_port(ctrlfd);
	data = Socket(atoi(tuple[4].c_str()) * 256 + atoi(tuple[5].c_str()));
	string hostname;
	hostname.append(tuple[0]);
	for (int i = 1; i < 4; i++) {
		hostname.append(".").append(tuple[i]);
	}
	datafd = data.getClientSocket((char*)hostname.c_str());
	stringstream cmd;
	cmd << "LIST" << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;

	struct pollfd ufds;

	ufds.fd = datafd;                     // a socket descriptor to exmaine for read
	ufds.events = POLLIN;             // check if this sd is ready to read
	ufds.revents = 0;                 // simply zero-initialized

	while ( poll(&ufds, 1, 1000) >0 ) {                  // the socket is ready to read
		n = read( datafd, message, BUFLEN ); // guaranteed to return from read
		if (n == 0)
			break;
		message[n] = '\0';
		cout << message;
	}
	n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
	close(datafd);
}

void cmd_get(int& ctrlfd, int& datafd, string& remoteName, string& localName) {
	vector<string> tuple = pasv_port(ctrlfd);
	data = Socket(atoi(tuple[4].c_str()) * 256 + atoi(tuple[5].c_str()));
	string hostname;
	hostname.append(tuple[0]);
	for (int i = 1; i < 4; i++) {
		hostname.append(".").append(tuple[i]);
	}
	datafd = data.getClientSocket((char*)hostname.c_str());
	char message[BUFLEN];
	stringstream cmd;
	cmd << "TYPE I" << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;

	cmd.str("");

	cmd << "RETR " << remoteName << (char)13 << (char)10 << '\0';
	msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;

	struct pollfd ufds;

	ufds.fd = datafd;                     // a socket descriptor to exmaine for read
	ufds.events = POLLIN;             // check if this sd is ready to read
	ufds.revents = 0;                 // simply zero-initialized

	ofstream out(localName.c_str(), std::ofstream::app) ;
	while ( poll(&ufds, 1, 1000) >0 ) {                  // the socket is ready to read
		n = read( datafd, message, BUFLEN ); // guaranteed to return from read
		if (n == 0)
			break;
		message[n] = '\0';
		out << message;
	}
	out.close();
	close(datafd);
	n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
}

void cmd_put(int& ctrlfd, int& datafd, string& localName, string& remoteName) {
	vector<string> tuple = pasv_port(ctrlfd);
	data = Socket(atoi(tuple[4].c_str()) * 256 + atoi(tuple[5].c_str()));
	string hostname;
	hostname.append(tuple[0]);
	for (int i = 1; i < 4; i++) {
		hostname.append(".").append(tuple[i]);
	}
	datafd = data.getClientSocket((char*)hostname.c_str());
	char message[BUFLEN];
	stringstream cmd;
	cmd << "TYPE I" << (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;

	cmd.str("");

	cmd << "STOR " << remoteName << (char)13 << (char)10 << '\0';
	msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
	n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;

	ifstream infile(localName.c_str(), std::ifstream::in);
	while (!infile.eof()) {
		infile.read(message, BUFLEN);
		int c = infile.gcount();
		write(datafd, message, c);
	}
	infile.close();
	close(datafd);
	n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	cout << message;
}

int main(int argc, char* argv[]) {
	int ctrlfd;
	int datafd;

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
			exit(0);
		} else if (ctrlfd == 0 ) {
			cout << "Not connected" << endl;
			continue;
		} else if (cmd == "name") {
			cmd_name(ctrlfd, arg1);
		} else if (cmd == "password") {
			cmd_passwd(ctrlfd, arg1);
		} else if (cmd == "cd") {
			cmd_cd(ctrlfd, arg1);
		} else if (cmd == "ls") {
			cmd_ls(ctrlfd, datafd);
		} else if (cmd == "get") {
			if (arg1 == "") {
				cout << "(remote-file) ";
				getline(cin, arg1);
				cout << "(local-file) ";
				getline(cin, arg2);
			}
			if (arg2 == "") {
				arg2 = arg1;
			}
			cmd_get(ctrlfd, datafd, arg1, arg2);
		} else if (cmd == "put") {
			if (arg1 == "") {
				cout << "(remote-file) ";
				getline(cin, arg1);
				cout << "(local-file) ";
				getline(cin, arg2);
			}
			if (arg2 == "") {
				arg2 = arg1;
			}
			cmd_put(ctrlfd, datafd, arg1, arg2);
		} else if (cmd == "close") {
			cmd_close(ctrlfd);
		}
	}
	return 0;
}
