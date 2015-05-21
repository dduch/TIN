#include "Sender.h"
#include <pthread.h>


Sender::Sender(std::string filename, sockaddr_in dest_address){
	this->file_descriptor = FileManager::openFile(filename, READ_F);
	this->filename = filename;
	memcpy(&this->src_address, &dest_address, sizeof(dest_address));
	this->prot_handler = new ProtocolHandler();
	logger = new Logger(filename, pthread_self());
        lastData = false;

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
	ProtocolPacket packet = prot_handler->interpretDatagram(buffer, buff_len);

    if(prot_handler->isACK(packet)){
    	handleACKPacket(packet, src_address);
    }
}

void* Sender:: run(void* req){
	Arguments* arg = (Arguments*)req;
	Sender* sender = new Sender(arg->file_name, arg->dest_address);

	int bytes = FileManager::readFile(sender->file_descriptor, sender->data_buffer, sizeof(data_buffer));
	ProtocolPacket packet;

        bool end = false; // ???????

        /*if(bytes > 0) {
		 packet = sender->prot_handler->prepareDATA(++(sender->current_pacekt), DATA_MAX_SIZE, sender->data_buffer, sizeof(data_buffer));
	}
        else if(bytes == 0) {
                // ???
        }*/

        // blad odczytu z pliku:
        if (bytes < 0) {
            sender->logger->logEvent(FILE_READING_ERROR, ERROR);
            // robimy timeouty dla errora????
            packet = sender->prot_handler->prepareERR(0);
            sender->sendDatagram(packet, sender->src_address, sender, "");
            sender->logger->logEvent(FILE_READING_ERROR, ERROR);
            return (void*)0;
            //pthread_exit(NULL);
        }
        else if (bytes < MAX_DATA_BLOCK_SIZE) {
            sender->lastData = true;
        }

        packet = sender->prot_handler->prepareDATA(++(sender->current_pacekt), bytes, sender->data_buffer, bytes);

	std::string log_msg = SENT_DATA + std::to_string(sender->current_pacekt);

	memcpy(&sender->last_packet, &packet, sizeof(packet));
	sender->timeout_type = ACK_TO;
	sender->sent_data += bytes;
	sender->start_critical_waiting = time(NULL);
	sender->sendDatagram(packet, sender->src_address, sender, log_msg);

        if (!end) { // ?????
		sender->startListen(sender->my_address, sender->sock_fd, sender);
		sender->logger->logEvent(FINISH_SENDING, INFO);
	}
	return (void*)0;
}

void Sender:: handleACKPacket(ProtocolPacket rd, sockaddr_in src_address) {
        // jesli wyslales ostatni DATA => to jest ostatni ACK -> zakoncz:
        if (lastData) {
            FileManager::closeFile(this->file_descriptor);
            logger->logEvent(ACK_RECEIVED, INFO);
            logger->logEvent(FINISH_SENDING, INFO);
            pthread_exit(NULL);
            //return;
        }

        if (rd.number != current_pacekt)
            return;

        // czytaj kolejna porcje do wyslania:
	int bytes = FileManager::readFile(this->file_descriptor, this->data_buffer, sizeof(data_buffer));
	ProtocolPacket packet;
        // blad odczytu z pliku:
        if (bytes < 0) {
            logger->logEvent(FILE_READING_ERROR, ERROR);
            // robimy timeouty dla errora????
            packet = prot_handler->prepareERR(0);
            sendDatagram(packet, src_address, this, "");
            return;
        }
        // wczytalismy mniej niz blok => ostatnia porcja danych:
        if (bytes < MAX_DATA_BLOCK_SIZE)
            lastData = true;

        sent_data += bytes;

       // wysylamy DATA:
        packet = prot_handler->prepareDATA(++current_pacekt, bytes, this->data_buffer, bytes);
        // notujemy ze dostalismy ACK:
        logger->logEvent(ACK_RECEIVED ,INFO);
        std::string log_msg = SENT_DATA + std::to_string(current_pacekt);
		timeout_type = ACK_TO;
		start_critical_waiting = time(NULL);
		sendDatagram(packet, src_address, this, log_msg);

}


void Sender:: handleERRPacket(ProtocolPacket rd, sockaddr_in src_address){

}
