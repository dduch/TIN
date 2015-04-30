#include "Downloader.h"
#include <iostream>

Downloader:: Downloader(char* req) {
	if(createSocket() && bindSocket() && connectInit()){
		sendBroadcast(req);
	}
}
Downloader:: ~Downloader() {}

bool Downloader::bindSocket() {
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &(on), sizeof(on));
	myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = INADDR_ANY;
    myAddress.sin_port = 0;
    if(bind(sockfd, (struct sockaddr *) &myAddress, sizeof myAddress) == -1)
    {
    	return false;
    }
    return true;
}

bool Downloader:: connectInit(){
	memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_BROADCAST;
	serverAddress.sin_port = SERVER_PORT;

    if (connect(sockfd, (struct sockaddr *) &serverAddress, sizeof serverAddress) == -1) {
        return false;
    }
    return true;
}


bool Downloader:: sendBroadcast(char* req){
	if(sendto(sockfd, req, sizeof(req), 0, (struct sockaddr *) &serverAddress, sizeof serverAddress) == -1){
		std::cout<<"nie wysłano";
		return false;
	}

	return true;
}
