#include "Downloader.h"
#include <iostream>


Downloader:: Downloader(std::string filename) {
	this->filename = filename;
	prot_handler = new ProtocolHandler();
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

    if (connect(this->sock_fd, (struct sockaddr *) &broadcast_address, sizeof(broadcast_address)) == -1) {
        return false;
    }
    return true;
}

/*
 * Wyślij broadcastowe zapytanie
 */
bool Downloader:: sendBroadcast(std::string filename){
	/*if(sendto(sockfd, req, sizeof(req), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
		std::cout<<"nie wysłano";
		return false;
	}*/
	// skoro zrobilismy connect(), to mozemy skorzystac z funkcji send():

	ProtocolPacket req_packet = prot_handler->prepareRQ(strlen(filename.c_str()) + 1, filename.c_str(), strlen(filename.c_str()) +1 );
	if(send(this->sock_fd, &req_packet, sizeof(ProtocolPacket), 0) == -1){
		return false;
	}

	return true;
}

void Downloader:: startListen(sockaddr_in src_address, int sock_fd){
	socklen_t address_len = sizeof(src_address);
    while(1)
    {
        if (recvfrom(sock_fd, &received_packet, sizeof(ProtocolPacket), 0, (struct sockaddr*)&src_address, &address_len) == -1){

        }
        //receiveDatagram(buffer, src_address);
    }
}

void Downloader:: receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address){
	ProtocolPacket packet = prot_handler->interpretDatagram(buffer, sizeof(buffer));

    if(prot_handler->isRESP(packet) == RESP){
    	handleRESPPacket(packet);
    }
    if(prot_handler->isDATA(packet) == DATA){
    	handleDATAPacket(packet,src_address);
    }
}

void* Downloader:: run(void* req){
	std::string filename;
	std::string& file_ref = filename;

	RunningTasks::getIstance().checkFileName(*(int*)req, file_ref);
	Downloader* downloader = new Downloader(filename);

	if(downloader->createSocket() && downloader->bindSocket() && downloader->connectInit()){
		if(downloader->sendBroadcast(filename)){
			//downloader->startListen(downloader->my_address, downloader->sock_fd);
		}
	}
	return (void*)0;
}

void Downloader:: handleRESPPacket(ProtocolPacket resp_packet){

	//odrzuć wszystkie zgłoszenia poza pierwszym otrzymanym RESP
	if(this->is_RESP_received){
		return;
	}

	this->is_RESP_received = true;
	this->file_descriptor = FileManager::openFile(this->filename, WRITE_F);
}


void Downloader:: handleDATAPacket(ProtocolPacket data_packet, sockaddr_in src_address){
	FileManager::appendFile(this->file_descriptor,data_packet.data, strlen(data_packet.data));

	//prot_handler->prepareACK();
}
