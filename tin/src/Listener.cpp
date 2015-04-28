#include "Listener.h"


Listener:: Listener(){
	if(createSocket() && bindSocket()){
		startListen();
	}
}

Listener:: ~Listener(){

}

void Listener:: startListen(){
	socklen_t srcAddressLen = sizeof(srcAddress);
    while(1)
    {
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&srcAddress, &srcAddressLen)==-1){
        	perror("recvfrom()");
        }
        printf("Received packet from %s:%d\nData: %s\n\n",
               inet_ntoa(srcAddress.sin_addr), ntohs(srcAddress.sin_port), buffer);
    }
}

bool Listener::bindSocket() {
	bzero(&myAddress, sizeof(myAddress));
	myAddress.sin_family = AF_INET;
	myAddress.sin_port = htons(SERVER_PORT);
	myAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr* ) &myAddress, sizeof(myAddress))==-1)
	{
		return false;
	}

	return true;
}
