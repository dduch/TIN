#ifndef LISTNER_H_
#define LISTENER_H_
#include "NetworkHandler.h"
#include <stdio.h>

/*
 * Listener singleton class with private constructor
 * internal reference and static run method for new Thread.
 * Every node should has at most one working Listener
 */

class Listener : public NetworkHandler {
private:
	Listener();
	void startListen();
	static Listener* instance;

protected:
	bool bindSocket();

public:
	char buffer[500];
	struct sockaddr_in srcAddress;
	~Listener();
	static void* run(void*);
	static Listener* getInstance();
};
#endif
