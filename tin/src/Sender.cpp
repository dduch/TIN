#include "Sender.h"
#include <pthread.h>


/*
 * Tworzy nowy wątek Sendera - rozpoczyna wysyłanie pliku
 * otwiera plik do transmisji, zapamietuj adres, z którym sender ma prowadzić transmisję
 */
Sender::Sender(std::string filename, sockaddr_in dest_address, int transferID) {
    this->file_descriptor = FileManager::openFile(filename, READ_F);
    this->prot_handler = new ProtocolHandler();
    this->logger = new Logger("Sender", filename, pthread_self());
    this->filename = filename;
    this->transferID = transferID;
    lastData = false;
    // zapamiętaj z jakiego adresu przyszło żądanie transferu - gdzie należy wysyłać pakiety
    memcpy(&this->src_address, &dest_address, sizeof(dest_address));

    // stworz gniazdo:
    if (createSocket()) {
        bindSocket();
    }
}


Sender::~Sender() {
    // zamknij plik i gniazdo, usun uzywane obiekty:
	RunningTasks::getIstance().freeTaskSlot(transferID);
    FileManager::closeFile(this->file_descriptor);
    closeSocket(sock_fd);
    delete(prot_handler);
    delete(logger);
}


bool Sender::bindSocket() {
    bzero(&my_address, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    my_address.sin_port = 0;
    if (bind(sock_fd, (struct sockaddr *) &my_address, sizeof(my_address)) == -1) {
    	return false;
    }
    return true;
}


void Sender:: receiveDatagram(char* buffer, int buff_len,  sockaddr_in src_address) {
    ProtocolPacket packet = prot_handler->interpretDatagram(buffer, buff_len);

    if (prot_handler->isACK(packet)) {
    	handleACKPacket(packet, src_address);
    }
    else if(prot_handler->isERR(packet)) {
       	handleERRPacket(packet, src_address);
    }
}


void* Sender:: run(void* req) {
    ProtocolPacket packet;

    Arguments* arg = (Arguments*)req;
    Sender* sender = new Sender(arg->file_name, arg->dest_address, arg->transferID);

    // czytaj z pliku:
    int bytes = FileManager::readFile(sender->file_descriptor, sender->data_buffer, sizeof(data_buffer));

    // blad odczytu pliku:
    if (bytes < 0) {
    	sender->HanldeFileError(sender, FILE_READING_ERROR, ERROR_CODE3);
    }
    // odczytal mniej niz blok => koniec pliku:
    else if (bytes < MAX_DATA_BLOCK_SIZE) {
        sender->lastData = true;
    }
    // przygotuj pakiet z danymi do wyslania:
    packet = sender->prot_handler->prepareDATA(++(sender->current_packet), bytes, sender->data_buffer, bytes);
    std::string log_msg = SENT_DATA + std::to_string(sender->current_packet);

    // jesli to pierwszy blok danych, to zapisz w logu na jaki adres IP wysylasz:
    if (sender->current_packet == 1) {
    	std::string address(inet_ntoa(sender->src_address.sin_addr));
    	std::string port =  std::to_string(ntohs(sender->src_address.sin_port));
    	log_msg += (" na adres " + address + ":" + port);
    }

    // zapamietaj ostatnio wyslany pakiet i ustaw timeout na ACK:
    memcpy(&sender->last_packet, &packet, sizeof(packet));
    sender->timeout_type = ACK_TO;
    sender->sent_data += bytes;
    sender->sendDatagram(packet, sender->src_address, sender, log_msg);

    // nasluchuj ACK:
    sender->startListen(sender->my_address, sender->sock_fd, sender);

    // koniec pracy sendera:
    RunningTasks::getIstance().freeTaskSlot(sender->transferID);
    sender->logger->logEvent(FINISH_SENDING, INFO);
    delete(sender);

    return (void*)0;
}


void Sender:: handleACKPacket(ProtocolPacket rd, sockaddr_in src_address) {
    // jesli wyslales ostatni DATA => to jest ostatni ACK -> zakoncz watek:
    if (lastData) {
        RunningTasks::getIstance().freeTaskSlot(transferID);
    	FileManager::closeFile(this->file_descriptor);
        logger->logEvent(ACK_RECEIVED, INFO);
        logger->logEvent(FINISH_SENDING, INFO);
        closeSocket(sock_fd);
        delete(prot_handler);
        delete(logger);
        pthread_exit(NULL);
    }

    // zle ACK -> retransmituj ostatni pakiet:
    if (rd.number != current_packet) {
        timeout_type = ACK_TO;
        repeatSending(this);
        return;
    }

    ProtocolPacket packet;
    // czytaj kolejna porcje pliku do wyslania:
    int bytes = FileManager::readFile(this->file_descriptor, this->data_buffer, sizeof(data_buffer));

    // blad odczytu z pliku:
    if (bytes < 0) {
    	this->HanldeFileError(this, FILE_READING_ERROR, ERROR_CODE3);
        return;
    }
    // wczytalismy mniej niz blok => ostatnia porcja danych:
    else if (bytes < MAX_DATA_BLOCK_SIZE) {
    	lastData = true;
    }
    sent_data += bytes;

    // wysylamy DATA, logujemy ze dostalismy ACK, ustawiamy timeout:
    packet = prot_handler->prepareDATA(++current_packet, bytes, this->data_buffer, bytes);
    logger->logEvent(ACK_RECEIVED, INFO);
    std::string log_msg = SENT_DATA + std::to_string(current_packet);
    timeout_type = ACK_TO;
    sendDatagram(packet, src_address, this, log_msg);
}

/*
 * Obsługuje pakiet ERROR - loguje do pliku informacje o nieudanym trasnferze, kończy wątek
 */
void Sender:: handleERRPacket(ProtocolPacket rd, sockaddr_in src_address) {
    this->logger->logEvent
    			("Transfer zakończony niepowodzeniem -pakiet ERROR kod" + rd.number + ProtocolHandler::errors_code[rd.number], ERROR);

    // Kontrolowane zakończenie wątku
    MessagePrinter::print("Sending stopped - ERROR packet. TransferID = " + std::to_string(transferID));
    RunningTasks::getIstance().freeTaskSlot(transferID);
    FileManager::closeFile(this->file_descriptor);
    closeSocket(sock_fd);
    delete(prot_handler);
    delete(logger);
    pthread_exit(NULL);
}
