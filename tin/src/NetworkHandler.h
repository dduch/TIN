#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define SERVER_PORT 9055

/*
 * Abstract class which is an interface for other
 * classes responisble for network connection and transfer
 */
class NetworkHandler
{
protected:
	void startListen(sockaddr_in address, int sockfd);
	bool createSocket();
	void closeSocket(int socket);
	virtual bool bindSocket() = 0;
	char buffer[500];
	struct sockaddr_in myAddress,srcAddress;
	int sockfd, myPort;

public:
	void sendDatagram();
	void reciveDatagram();
	static void run();
};

#endif 
