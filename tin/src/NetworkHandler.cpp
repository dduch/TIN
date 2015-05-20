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

/*
 * Rozpoczyna nasłuchiwanie na wskazanym gnieździe danego obiektu. Po otrzymaniu wiadomości zleca
 * parsowanie i obsługę funkcji receiveDatagram, odpowiedniej dla danego obiektu
 */
void NetworkHandler:: startListen(sockaddr_in src_address, int sock_fd, NetworkHandler* object){
	socklen_t address_len = sizeof(src_address);
	fcntl(sock_fd, F_SETFL, O_NONBLOCK);

    while(1)
    {
    	if(object->start_critical_waiting != 0 && (time(NULL) - object->start_critical_waiting) > CRITICAL_TO){
    		object->logger->logEvent(CRITICAL_TO_ERROR, ERROR);
    		return;
    	}

    	if(object->start_waiting != 0 && (time(NULL) - object->start_waiting) > object-> timeout_type){
    		repeatSending(object);
		}

        if (recvfrom(sock_fd, &received_packet, sizeof(ProtocolPacket), 0, (struct sockaddr*)&src_address, &address_len) == -1){

        }
        else{
            printf("Received packet from %s:%d\nData: %d\n\n",
             		inet_ntoa(src_address.sin_addr), ntohs(src_address.sin_port), received_packet.type);
            char *buffer = new char[sizeof(ProtocolPacket)];
            memcpy(buffer, &received_packet, sizeof(received_packet));

            object->start_critical_waiting = 0;
            object->start_waiting = 0;
            object->receiveDatagram(buffer, sizeof(received_packet), src_address);
        }
    }
}

/*
 * Wysyła podany pakiet na wskazany adres
 */
void NetworkHandler::sendDatagram(ProtocolPacket packet, sockaddr_in dest_address, NetworkHandler* object, std::string log_msg){

	memcpy(&last_packet, &packet, sizeof(packet));
	object->start_waiting = time(NULL);

	if(sendto(object->sock_fd, &packet, sizeof(packet), 0, (struct sockaddr *) &dest_address, sizeof(dest_address)) == -1){

	}

	if(log_msg.length() > 0 && log_msg != RETRANSMISION){
		object->logger->logEvent(log_msg, INFO);
	}
	else if(log_msg.length() > 0 && log_msg == RETRANSMISION){
		object->logger->logEvent(log_msg, WARNING);
	}
}

void NetworkHandler:: repeatSending(NetworkHandler* object)
{
	std::string log_msg = RETRANSMISION;

	sendDatagram(object->last_packet, object->src_address, object, log_msg);
}


