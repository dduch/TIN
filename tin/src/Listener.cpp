#include "Listener.h"
#include <iostream>

// Początkowo nie istnieje żadna instancja Listenera, należy ją utworzyć
Listener* Listener:: instance = NULL;

Listener:: Listener(){

}

Listener:: ~Listener(){
	std::cout<<"destruktor";
	closeSocket(sock_fd);
}

/*
 * Zwraca wskaźnik do istniejącej instancji Listenera lub
 * jeśli takiej nie ma, tworzy nową instancję
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
 * Dowiązanie gniazda do Listenera
 */
bool Listener::bindSocket() {
    bzero(&my_address, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(SERVER_PORT);
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock_fd, (struct sockaddr* ) &my_address, sizeof(my_address)) == -1)
	{
		return false;
	}
	return true;
}


/*
 * Statyczna metoda rozpoczynająca pracę Listenera jako nowego wątku
 */
void* Listener:: run(void*){
	Listener* listener = Listener::getInstance();
	if(listener->createSocket() && listener->bindSocket())
	{
		listener->startListen(listener->src_address,listener->sock_fd);
	}

	return (void*)true;
}

