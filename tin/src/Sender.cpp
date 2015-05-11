#include "Sender.h"

Sender::Sender(std::string filename, sockaddr_in dest_address){
	this->file_descriptor = FileManager::openFile(filename, READ_F);
	this->filename = filename;
	memcpy(&this->src_address, &dest_address, sizeof(dest_address));
	this->prot_handler = new ProtocolHandler();

	if(createSocket()){
		bindSocket();
	}
}

Sender::~Sender(){

}

bool Sender::bindSocket() {
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


void Sender:: receiveDatagram(char* buffer, int buff_len,  sockaddr_in src_address){
	ProtocolPacket packet = prot_handler->interpretDatagram(buffer, strlen(buffer));

    if(prot_handler->isACK(packet) == ACK){
    	handleACKPacket(packet, src_address);
    }
}

void* Sender:: run(void* req){
	Arguments* arg = (Arguments*)req;
	Sender* sender = new Sender(arg->file_name, arg->dest_address);
	FileManager::readFile(sender->file_descriptor, sender->data_buffer, sizeof(data_buffer));

	ProtocolPacket packet = sender->prot_handler->prepareDATA(++(sender->current_pacekt), DATA_MAX_SIZE, sender->data_buffer, sizeof(data_buffer));
	sender->sendDatagram(packet, sender->src_address, sender->sock_fd);

	sender->startListen(sender->my_address, sender->sock_fd, sender);
	return (void*)0;
}

void Sender:: handleACKPacket(ProtocolPacket rd, sockaddr_in src_address){
	FileManager::readFile(this->file_descriptor, this->data_buffer, sizeof(data_buffer));

	ProtocolPacket packet = prot_handler->prepareDATA(++(this->current_pacekt), DATA_MAX_SIZE,this->data_buffer, sizeof(data_buffer));

	sendDatagram(packet, src_address, sock_fd);
}


void Sender:: handleERRPacket(ProtocolPacket rd, sockaddr_in src_address){

}

