#include "Listener.h"
#include <iostream>

// Początkowo nie istnieje żadna instancja Listenera, należy ją utworzyć
Listener* Listener:: instance = NULL;

Listener:: Listener(){
	prot_handler = new ProtocolHandler();
}

Listener:: ~Listener(){
	//std::cout<<"destruktor";
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


void Listener:: receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address){
    ProtocolPacket packet = prot_handler->interpretDatagram(buffer, buff_len);

    if(prot_handler->isRQ(packet)){
    	handleRQPacket(packet,src_address);
    }
    else if(prot_handler->isRD(packet)){
    	handleRDPacket(packet,src_address);
    }
	else {
		printf("Listener: dostalem cos innego niz RQ i RD.\n");
	}
}

void Listener::handleRQPacket(ProtocolPacket req, sockaddr_in src_address){
	std::string file_name(req.filename);
	if(FileManager::checkFile(file_name)){
		int file_size = FileManager::getFileSize(file_name);
		std::cout << "handleRQPacket: filesize = " << file_size << std::endl;
		if (file_size > 0)
		{
			ProtocolPacket packet = prot_handler->prepareRESP(file_size);
			sendDatagram(packet,src_address, this, std::string());
		}
	}
}

void Listener::handleRDPacket(ProtocolPacket req, sockaddr_in src_address){
	Arguments arguments;
	//std::string name (req.filename);
	memcpy(&arguments.file_name, &req.filename, MAX_FILENAME_SIZE);
	memcpy(&arguments.dest_address, &src_address, sizeof(src_address));

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
		listener->startListen(listener->src_address,listener->sock_fd, listener);
	}

	return (void*)true;
}

