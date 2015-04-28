#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define SERVER_PORT 9930


class NetworkHandler
{
protected:
	bool createSocket();
	void closeSocket(int socket);
	virtual bool bindSocket() = 0;
	int sockfd;
	int port;
	struct sockaddr_in myAddress;
		
public:
	NetworkHandler();
	~NetworkHandler();
	void sendDatagram();
	void reciveDatagram();
	void sendBroadcast();
};

#endif 
