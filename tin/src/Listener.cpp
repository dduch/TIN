#include "Listener.h"
#include <iostream>

// Początkowo nie istnieje żadna instancja Listenera, należy ją utworzyć
Listener* Listener:: instance = NULL;

Listener:: Listener(){
	prot_handler = new ProtocolHandler();
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
 *Rozpoczyna nasłuchiwanie na gniezdzie serwerowym
 */
void Listener:: startListen(sockaddr_in src_address, int sock_fd){
	socklen_t address_len = sizeof(src_address);
    while(1)
    {
        if (recvfrom(sock_fd, &received_packet, sizeof(ProtocolPacket), 0, (struct sockaddr*)&src_address, &address_len) == -1){

        }
        else{
            printf("Received packet from %s:%d\nData: %s\n\n",
             		inet_ntoa(src_address.sin_addr), ntohs(src_address.sin_port), received_packet.filename);

            char *buffer = new char[sizeof(ProtocolPacket)];
            memcpy(buffer, &received_packet, sizeof(received_packet));

            receiveDatagram(buffer, sizeof(received_packet), src_address);
        }
    }
}

void Listener:: receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address){
    ProtocolPacket packet = prot_handler->interpretDatagram(buffer, buff_len);

    if(prot_handler->isRQ(packet)){
    	handleRQPacket(packet,src_address);
    }
    if(prot_handler->isRD(packet)){
    	handleRDPacket(packet,src_address);
    }
}

void Listener::handleRQPacket(ProtocolPacket req, sockaddr_in src_address){
	std::string file_name(req.filename);
	if(FileManager::checkFile(file_name)){
		ProtocolPacket packet = prot_handler->prepareRESP(0);
		sendDatagram(packet,src_address, sock_fd);
	}
}

void Listener::handleRDPacket(ProtocolPacket req, sockaddr_in src_address){
	Arguments arguments;
	std::string name (req.filename);
	arguments.file_name = name.c_str();
	arguments.dest_address = src_address;

    pthread_t tid;
    if (pthread_create(&tid, NULL, Sender::run, &arguments)) {
       // MessagePrinter::print("Error while initializing thread");
    }
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

