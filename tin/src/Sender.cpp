#include "Sender.h"

Sender::Sender(std::string filename, sockaddr_in dest_address){
	this->file_descriptor = FileManager::openFile(filename, READ_F);
	this->filename = filename;
	memcpy(&this->src_address, &dest_address, sizeof(dest_address));
	this->prot_handler = new ProtocolHandler();
	logger = new Logger(filename, pthread_self());

	if (createSocket()){
		bindSocket();
	}
}

Sender::~Sender(){
	//FileManager::closeFile(this->file_descriptor);
	closeSocket(sock_fd);
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
	//ProtocolPacket packet = prot_handler->interpretDatagram(buffer, buff_len);

    if(prot_handler->isACK(packet)){
    	handleACKPacket(packet, src_address);
    }
}

void* Sender:: run(void* req){
	Arguments* arg = (Arguments*)req;
	Sender* sender = new Sender(arg->file_name, arg->dest_address);

	int bytes = FileManager::readFile(sender->file_descriptor, sender->data_buffer, sizeof(data_buffer));
	std::cout << "Sender::run(): odczytalem bajtow: " << bytes << std::endl;
	ProtocolPacket packet;

	bool end = false;

	if(bytes > 0){
		 packet = sender->prot_handler->prepareDATA(++(sender->current_pacekt), DATA_MAX_SIZE, sender->data_buffer, sizeof(data_buffer));
	}
	else if(bytes == 0){
		// ???
	}

	std::string log_msg = SENT_DATA + std::to_string(sender->current_pacekt);

	sender->sendDatagram(packet, sender->src_address, sender, log_msg);

	if (!end) {
		sender->startListen(sender->my_address, sender->sock_fd, sender);
		sender->logger->logEvent(FINISH_SENDING, INFO);
		("Zakonczenie wysylania.\n");
	}
	return (void*)0;
}

void Sender:: handleACKPacket(ProtocolPacket rd, sockaddr_in src_address){
	int bytes = FileManager::readFile(this->file_descriptor, this->data_buffer, sizeof(data_buffer));
	std::cout << "Sender::handleACK: odczytalem bajtow: " << bytes << std::endl;
	ProtocolPacket packet;
	if (bytes > 0) {
		packet = prot_handler->prepareDATA(++current_pacekt, bytes, this->data_buffer, bytes);
	}
	else if(bytes == 0) {
		// Przygotuj pusty pakiet i zakoncz transfer:
		packet = prot_handler->prepareDATA(++current_pacekt, bytes, this->data_buffer, 0);
		FileManager::closeFile(this->file_descriptor);
	} else {
		printf("Sender::handleACK: Blad odczytu danych.");
		return;
	}

	logger->logEvent(ACK_RECEIVED ,INFO);
	std::string log_msg = SENT_DATA + std::to_string(current_pacekt);
	sendDatagram(packet, src_address, this, log_msg);
}


void Sender:: handleERRPacket(ProtocolPacket rd, sockaddr_in src_address){

}
