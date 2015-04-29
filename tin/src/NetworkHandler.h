#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define SERVER_PORT 6055

/*
 * Abstract class which is an interface for other
 * classes responisble for network connection and transfer
 */
class NetworkHandler
{
protected:
	bool createSocket();
	void closeSocket(int socket);
	virtual bool bindSocket() = 0;
	struct sockaddr_in myAddress;
	int sockfd, myPort;

public:
	void sendDatagram();
	void reciveDatagram();
	static void run();
};

#endif 
