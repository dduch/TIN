#include "Listener.h"
#include <iostream>

Listener* Listener:: instance = NULL;

Listener:: Listener(){

}

Listener:: ~Listener(){
	closeSocket(this->sockfd);
}

/*
 * return instance of Listener
 */
Listener* Listener:: getInstance(){
	if(instance == NULL)
	{
		instance = new Listener();
		return instance;
	}
	return instance;
}

/*
 * bind socket for Listener
 */
bool Listener::bindSocket() {
    bzero(&myAddress, sizeof(myAddress));
    myAddress.sin_family = AF_INET;
    myAddress.sin_port = htons(SERVER_PORT);
    myAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr* ) &myAddress, sizeof(myAddress)) == -1)
	{
		return false;
	}
	return true;
}


void Listener:: startListen(){
	socklen_t srcAddressLen = sizeof(srcAddress);
    while(1)
    {
    	std::cout<<"slucham";
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&srcAddress, &srcAddressLen)==-1){
        }
        printf("Received packet from %s:%d\nData: %s\n\n",
               inet_ntoa(srcAddress.sin_addr), ntohs(srcAddress.sin_port), buffer);
    }
}

void* Listener:: run(void*){
	Listener* listener = Listener::getInstance();
	if(listener->createSocket() && listener->bindSocket())
	{
		listener->startListen();
	}
}

