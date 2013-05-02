/*
 * udp.cpp
 *
 *  Created on: Apr 18, 2013
 *      Author: jay
 */

#include "Timer.h"
#include "UdpSocket.h"
#include <vector>

int ackPkt(int n, UdpSocket &sock) {
	return sock.ackTo((char*)&n, sizeof(int));
}

void getAck(int &lar, UdpSocket &sock) {
	sock.recvFrom((char*)&lar, sizeof(lar));
}

int clientStopWait( UdpSocket &sock, const int max, int message[] ) {
	cerr << "Stop-and-wait test:" << endl;
	Timer t;
	int retransmits = 0;
	for ( int i = 0; i < max; i++ ) {					// transfer message[] max times
		message[0] = i;                            		// message[0] has a sequence #
		sock.sendTo((char*) message, MSGSIZE);			// send message
		t.start();										// start timer
		long lap;
		while ((lap = t.lap()) < 1500) {
			if(sock.pollRecvFrom() > 0) {				// if ack received before timer ends
				sock.recvFrom((char*)message, MSGSIZE);
				break;
			}
		}
		if (lap >= 1500) {								// timeout, so retransmit
			i--;
			retransmits++;
			cerr << retransmits << endl;
		}
	}
	return retransmits;
}
void serverReliable( UdpSocket &sock, const int max, int message[] ) {
	cerr << "server stop and wait:" << endl;

	// receive message[] max times
	int seq;
	while(seq != max - 1) {
		while( sock.pollRecvFrom() <=0 ){}				// pause until message
		sock.recvFrom( ( char * ) message, MSGSIZE );   // udp message receive
		seq = *message;									// get the seq # from beginning of msg
		sock.ackTo((char*)&seq, sizeof(seq));
	}
}
int clientSlidingWindow( UdpSocket &sock, const int max, int message[], int windowSize ) {
	cerr << "Sliding-window test:" << endl;
	int lar = -1;										// last acknowledgment received
	int seq = 0;										// current seq #
	int retransmits = 0;
	if(sock.pollRecvFrom() > 0) {						// check for any old acks
		sock.recvFrom( ( char * ) message, MSGSIZE );		// clean out the socket
	}
	vector<int> window(windowSize);
	for(int i = 0; i < windowSize; i++) { 				//init window
		window[i] = -1;
	}
	Timer t;
	while( seq < max) {
		while(seq - lar <= windowSize && seq < max) {		// while seq is within window
			message[0] = seq;									// append seq to message
			sock.sendTo((char*) message, MSGSIZE);				// send message
			seq++;												// increment seq
		}
		long lap;
		t.start();											// timer start
		while ((lap = t.lap()) < 1500) {
			if(sock.pollRecvFrom() > 0) {					// while not timed out
				getAck(lar, sock);								// check for an ack
				break;
			}
		}
		if (lap >=1500) {									// if no ack
			seq = lar+1;										// reset sequence #
			retransmits++;
		}
	}
	return retransmits;
}
void serverEarlyRetrans( UdpSocket &sock, const int max, int message[], int windowSize ) {
	cerr << "server sliding window:" << endl;
	int seq = 0;
	int lastSeq = -1;
	int windowLoc = 0;
	vector<int> window(windowSize);
	for(int i = 0; i < windowSize; i++) { 				//init window
		window[i] = -1;
	}
	while(seq < max) {
		while( sock.pollRecvFrom() <=0 ){}				// pause until message
		sock.recvFrom( ( char * ) message, MSGSIZE );   	// udp message receive
		seq = *message;										// get the seq # from beginning of msg
		int index = seq % windowSize;
		if (index < windowLoc || index >= windowSize) { // seq is outside window
			ackPkt(lastSeq, sock);							// ack last contiguous packet.
			continue;										// drop packet
		} else if(index == windowLoc) {					// seq is next contiguous packet
			window[index] = seq;							// store seq in 1st slot
			lastSeq = seq;
			while(window[windowLoc] > -1) {					// while contiguous
				lastSeq = window[windowLoc];				// store value in lastSeq
				window[windowLoc] = -1;						// clean up value to -1
				windowLoc++;								// increment window
				windowLoc = windowLoc % windowSize;			// mod window if needed
				if (window[windowLoc] == -1) {				// if next slot is empty
					ackPkt(lastSeq, sock);					// ack last contiguous packet
				}
			}
		} else {										// seq is within window, so store
			window[index] = seq;							// store seq in correct slot
			ackPkt(lastSeq, sock);							// ack last contiguous packet
		}
		if (seq == max-1)
			break;
	}
}


