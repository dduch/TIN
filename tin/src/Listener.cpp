#include "Listener.h"

Listener:: Listener(){
	if(createSocket() && bindSocket()){
		startListen();
	}
}

Listener:: ~Listener(){

}

void Listener:: startListen(){
	  /*while(1)
	    {
	        if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1)
	            err("recvfrom()");
	        printf("Received packet from %s:%d\nData: %s\n\n",
	               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);
	    }*/
}

bool Listener::NetworkHandler::bindSocket() {
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
