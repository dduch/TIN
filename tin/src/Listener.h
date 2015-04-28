#ifndef LISTNER_H_
#define LISTENER_H_
#include "NetworkHandler.h"
#include <stdio.h>

class Listener : public NetworkHandler {
private:
	void startListen();
protected:
	bool bindSocket();
public:
	char buffer[500];
	struct sockaddr_in srcAddress;
	Listener();
	~Listener();
};
#endif
