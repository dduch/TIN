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
 * Utworzenie nowego gniazda UDP
 * W przypadku sukcesu uzupełnia deskryptor gniazda
 */
bool NetworkHandler:: createSocket(){
	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		return false;
	}
	return true;
}

/*
 * Funkcja zamykająca gniazdo o podanym deskryptorze
 */
void NetworkHandler:: closeSocket(int socket){
	close(socket);
}

/*
 *Rozpoczyna nasłuchiwanie na gniezdzie serwerowym
 */
void NetworkHandler:: startListen(sockaddr_in src_address, int sock_fd){
	socklen_t address_len = sizeof(src_address);
    while(1)
    {
        if (recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&src_address, &address_len) == -1){

        }
        printf("Received packet from %s:%d\nData: %s\n\n",
        		inet_ntoa(src_address.sin_addr), ntohs(src_address.sin_port), buffer);
    }
}

