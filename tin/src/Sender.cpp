#include "Sender.h"

Sender::Sender(std::string filename, sockaddr_in dest_address){
	this->file_descriptor = FileManager::openFile(filename, READ_F);
	this->filename = filename;
	this->src_address = src_address;

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

void Sender:: startListen(sockaddr_in src_address, int sock_fd){
	socklen_t address_len = sizeof(src_address);
    while(1)
    {
        if (recvfrom(sock_fd, &received_packet, sizeof(ProtocolPacket), 0, (struct sockaddr*)&src_address, &address_len) == -1){

        }
        //receiveDatagram(buffer, src_address);
        /*printf("Received packet from %s:%d\nData: %s\n\n",
        		inet_ntoa(src_address.sin_addr), ntohs(src_address.sin_port), buffer);*/
    }
}

void Sender:: receiveDatagram(char* buffer, int buff_len,  sockaddr_in src_address){
	ProtocolPacket packet = prot_handler->interpretDatagram(buffer, strlen(buffer));

    if(prot_handler->isACK(packet) == ACK){
    	handleACKPacket(packet);
    }
}

void* Sender:: run(void* req){
	Arguments* arg = (Arguments*)req;
	Sender* sender = new Sender(arg->file_name, arg->dest_address);
	FileManager::readFile(sender->file_descriptor, sender->data_buffer, sizeof(data_buffer));

	return (void*)0;
}

void Sender:: handleACKPacket(ProtocolPacket rd){
	FileManager::readFile(this->file_descriptor, this->data_buffer, sizeof(data_buffer));

}

void Sender:: handleERRPacket(ProtocolPacket rd){

}

