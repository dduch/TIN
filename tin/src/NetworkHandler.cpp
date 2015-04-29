#include "NetworkHandler.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


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
