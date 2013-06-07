/*	CSS 432 Final Project
 * 	6/6/2013
 * 	Authors: Jay Hennen & Chris Rouse
 */


#include "Socket.h"
#include "Timer.h"
#include <sstream>
#include <vector>
#include <sys/poll.h>
#include <cstdlib>
#include <ios>
#include <fstream>

#define BUFLEN 1500

using namespace std;

// Socket and fd information
Socket ctrl(21);
Socket data(23);
int ctrlfd;
int datafd;

// Gets a response from the server on the ctrl socket
string get_res() {
	char message[BUFLEN];
	int n = read(ctrlfd, message, BUFLEN);
	message[n] = '\0';
	string temp(message);
	return temp;
}

// Sends a message to the server on the ctrl socket, given a type and arg.
void send_msg(string type, string& arg) {
	char message[BUFLEN];
	stringstream cmd;
	cmd << type;
	if (arg != "")
		cmd << " " << arg << (char)13 << (char)10 << '\0';
	else
		cmd <<  (char)13 << (char)10 << '\0';
	string msg = cmd.str();
	strncpy(message,msg.c_str(), msg.length());
	write(ctrlfd, message, strlen(message));
}

// Overloaded send_msg for single param use (no arg).
void send_msg(string type) {
	string temp = "";
	send_msg(type, temp);
}

// Reads data from a specified socket into an ostream until no more data is available.
void sock_to_ostream(int& sock, ostream& s) {
	char message[BUFLEN];
	struct pollfd ufds;
	ufds.fd = sock;                     // a socket descriptor to exmaine for read
	ufds.events = POLLIN;             // check if this sd is ready to read
	ufds.revents = 0;                 // simply zero-initialized
	while ( poll(&ufds, 1, 1000) >0 ) {                  // the socket is ready to read
		int n = read( sock, message, BUFLEN ); // guaranteed to return from read
		if (n == 0)
			break;
		message[n] = '\0';
		string msg(message);
		s << msg;
	}
}

void cmd_name(string& name);
void cmd_passwd(string& passwd);

// open command will connect to the specified server on specified port and initiate control connection
// It will then ask for username and password
//currently WILL FREEZE if server does not accept ftp connections on the port specified
void cmd_open(string& hostname, string& port) {
	ctrl = Socket(atoi(port.c_str()));
	ctrlfd = ctrl.getClientSocket((char*)(hostname.c_str()));
	cout << get_res();
	string name;
	cout << "Name: ";
	getline (cin, name);
	cmd_name(name);
}

// name command will set the current username on the server, then ask for password
void cmd_name(string& name) {
	send_msg("USER", name);
	cout << get_res();
	string passwd;
	cout.clear();
	cout << "Password: ";
	getline(cin, passwd);
	cmd_passwd(passwd);
}

// password command will send password to server for authentication, and read any MOTD to cout
void cmd_passwd(string& passwd) {
	send_msg("PASS", passwd);
	sock_to_ostream(ctrlfd, cout);
}

// close will close the connection to the server and reset the control file descriptor.
void cmd_close() {
	send_msg("QUIT");
	cout << get_res();
	close(ctrlfd);
	ctrlfd = 0;
}

// cd sends a change directory request to the server.
void cmd_cd(string& subdir) {
	send_msg("CWD");
	cout << get_res();
}

// This helper method requests PASV mode from the server and returns a string vector of information
// The format is (IP_oct1,IP_oct2,IP_oct3,IP_oct4,Port%256,Remainder)
vector<string> pasv_port() {
	send_msg("PASV");
	string res = get_res();
	cout << res;
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

// Initializes the data socket with the server
void init_datafd() {
	vector<string> tuple = pasv_port();
	data = Socket(atoi(tuple[4].c_str()) * 256 + atoi(tuple[5].c_str()));
	string hostname;
	hostname.append(tuple[0]);
	for (int i = 1; i < 4; i++) {
		hostname.append(".").append(tuple[i]);
	}
	datafd = data.getClientSocket((char*)hostname.c_str());
}

// ls command will request the current directory listing form the server, which will be
// transmitted over the data socket.
void cmd_ls() {
	init_datafd();
	send_msg("LIST");
	cout << get_res();
	sock_to_ostream(datafd, cout);
	cout << get_res();
	close(datafd);
}

// get receives the remoteName file from the server and saves it as localName on the client.
void cmd_get(string& remoteName, string& localName) {
	Timer t;
	t.start();
	init_datafd();
	send_msg("TYPE I");
	cout << get_res();
	send_msg("RETR", remoteName);
	cout << get_res();
	ofstream out(localName.c_str(), std::ofstream::app) ;
	sock_to_ostream(datafd, out);
	out.close();
	close(datafd);
	cout << get_res();
	long int elapsed = t.lap();
	cout << "Elapsed time: " << elapsed << " usec" << endl;
}

// put sends the localName file to the server, which saves it as remoteName.
void cmd_put(string& localName, string& remoteName) {
	Timer t;
	t.start();
	init_datafd();
	send_msg("TYPE I");
	cout << get_res();
	send_msg("STOR", remoteName);
	cout << get_res();
	ifstream infile(localName.c_str(), std::ifstream::in);
	char message[BUFLEN];
	while (!infile.eof()) {
		infile.read(message, BUFLEN);
		int c = infile.gcount();
		write(datafd, message, c);
	}
	infile.close();
	close(datafd);
	cout << get_res();
	long int elapsed = t.lap();
	cout << "Elapsed time: " << elapsed << " usec" << endl;
}

// helper in case insufficient args are given to get and put
void get_more_args(string& arg1, string& arg2) {
	cout << "(remote-file) ";
	getline(cin, arg1);
	cout << "(local-file) ";
	getline(cin, arg2);
}

// Loops and interprets user commands by calling appropriate sub-commands.
int main(int argc, char* argv[]) {
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
				cmd_open(arg1, arg2);
			else
				cout << "usage: open <hostname> <port>" << endl;
		}
		if (cmd == "quit") {
			if (ctrlfd != 0)
				cmd_close();
			exit(0);
		}
		if (ctrlfd == 0 ) {
			cout << "Not connected" << endl;
			continue;
		} else if (cmd == "name") {
			cmd_name(arg1);
		} else if (cmd == "password") {
			cmd_passwd(arg1);
		} else if (cmd == "cd") {
			cmd_cd(arg1);
		} else if (cmd == "ls") {
			cmd_ls();
		} else if (cmd == "get") {
			if (arg1 == "") {
				get_more_args(arg1, arg2);
			}
			if (arg2 == "") {
				arg2 = arg1;
			}
			cmd_get(arg1, arg2);
		} else if (cmd == "put") {
			if (arg1 == "") {
				get_more_args(arg1, arg2);
			}
			if (arg2 == "") {
				arg2 = arg1;
			}
			cmd_put(arg1, arg2);
		} else if (cmd == "close") {
			cmd_close();
		} else {
			cout << "Invalid command" << endl;
		}
	}
	return 0;
}
