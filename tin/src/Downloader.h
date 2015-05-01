#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_
#include "NetworkHandler.h"

class Downloader : public NetworkHandler {
private:
	bool getBroadcastAddress();
	bool sendBroadcast(char* request);
	bool bindBroadcastSocket();
	bool bindSocket();
	bool connectInit();
	
	struct sockaddr_in serverAddress;
public:
	Downloader(char* req);
	~Downloader();
};

#endif
