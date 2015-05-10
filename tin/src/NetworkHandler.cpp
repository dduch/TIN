#include "NetworkHandler.h"
#include "ProtocolHandler.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

NetworkHandler:: ~NetworkHandler(){

}
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

void NetworkHandler::sendDatagram(ProtocolPacket packet, sockaddr_in dest_address, int sock_fd){
	if(sendto(sock_fd, &packet, sizeof(packet), 0, (struct sockaddr *) &dest_address, sizeof(dest_address)) == -1){

	}
}
