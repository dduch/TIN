#include "Downloader.h"
#include <iostream>

/*
 * Jeśli utworzono gniazdo, dowiązano adres i prawidłowo zainicjowano połączenie
 * wsyślij pakiet z zapytaniem do wszystkich węzłów w sieci
 */
Downloader:: Downloader(char* req) {
	if(createSocket() && bindSocket() && connectInit()){
		sendBroadcast(req);
	}
}

/*
 * Zamknij gniazdo i skojarzony port
 */
Downloader:: ~Downloader() {
	closeSocket(sock_fd);
}


/*
 * Dowiąż adres
 */
bool Downloader::bindSocket() {
	int on = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &(on), sizeof(on));
	bzero(&my_address, sizeof(my_address));
	my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    my_address.sin_port = 0;
    if(bind(sock_fd, (struct sockaddr *) &my_address, sizeof(my_address)) == -1)
    {
    	return false;
    }
    return true;
}

/*
 * Zainiciuj połączenie do wysłania broadcastowego żądania
 */
bool Downloader:: connectInit(){
	bzero(&broadcast_address, sizeof(broadcast_address));
	broadcast_address.sin_family = AF_INET;
	broadcast_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	broadcast_address.sin_port = htons(SERVER_PORT);

    if (connect(sock_fd, (struct sockaddr *) &broadcast_address, sizeof(broadcast_address)) == -1) {
        return false;
    }
    return true;
}

/*
 * Wyślij broadcastowe zapytanie
 */
bool Downloader:: sendBroadcast(char* req){
	/*if(sendto(sockfd, req, sizeof(req), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
		std::cout<<"nie wysłano";
		return false;
	}*/
	// skoro zrobilismy connect(), to mozemy skorzystac z funkcji send():
	if(send(sock_fd, req, sizeof(req), 0) == -1){
		return false;
	}

	return true;
}
