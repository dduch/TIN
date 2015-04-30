#include "NetworkHandler.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>


/*
 * create socket for udp connection
 */
bool NetworkHandler:: createSocket(){
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		return false;
	}
	return true;
}

/*
 * close created socked
 */
void NetworkHandler:: closeSocket(int socket){
	close(socket);
}

/*
 *
 */
void NetworkHandler:: startListen(sockaddr_in srcAddress, int sockfd){
	socklen_t addressLen = sizeof(srcAddress);
    while(1)
    {
    	std::cout<<"slucham";
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&srcAddress, &addressLen) == -1){
        }
        printf("Received packet from %s:%d\nData: %s\n\n",
               inet_ntoa(srcAddress.sin_addr), ntohs(srcAddress.sin_port), buffer);
    }
}

