#ifndef LISTNER_H_
#define LISTENER_H_
#include "NetworkHandler.h"

class Listener : public NetworkHandler {
private:
	void startListen();
public:
	char buffer[500];
	struct sockaddr_storage src_addr;
	socklen_t src_addr_len=sizeof(src_addr);
	Listener();
	~Listener();
};
#endif
