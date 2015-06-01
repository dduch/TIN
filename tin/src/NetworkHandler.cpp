#include "NetworkHandler.h"
#include "ProtocolHandler.h"
#include "RunningTasks.h"
#include "Downloader.h"
#include "Sender.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cerrno>
#include <typeinfo>


NetworkHandler:: ~NetworkHandler() {}

/*
 * Utworzenie nowego gniazda UDP
 * W przypadku sukcesu uzupełnia deskryptor gniazda
 */
bool NetworkHandler:: createSocket() {
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return false;
    }
    return true;
}

/*
 * Funkcja zamykająca gniazdo o podanym deskryptorze
 */
void NetworkHandler:: closeSocket(int socket) {
    close(socket);
}

/*
 * Rozpoczyna nasłuchiwanie na wskazanym gnieździe danego obiektu. Po otrzymaniu wiadomości zleca
 * parsowanie i obsługę funkcji receiveDatagram, odpowiedniej dla danego obiektu
 */
void NetworkHandler:: startListen(sockaddr_in src_address, int sock_fd, NetworkHandler* object) {
    socklen_t address_len = sizeof(src_address);
    fd_set readset;
    int result;
    struct timeval timeout;

    while (1) {
        FD_ZERO (&readset);
        FD_SET (sock_fd, &readset);

        // czekaj na zdarzenie lub timeout:
        timeout.tv_sec = object->timeout_type;
        timeout.tv_usec = 0;
        result = select(sock_fd+1, &readset, NULL, NULL, &timeout);

        // po odblokowaniu najpierw sprawdzamy czy moze watek nie dostal polecenia zakonczenia od uzytkownika:
        if (RunningTasks::getIstance().checkTerminateFlag(object->transferID)) {

        	ProtocolPacket packet;

        	Downloader* downloader = dynamic_cast<Downloader*>(object);
        	Sender* sender = dynamic_cast<Sender*>(object);
        	std::string log_msg = SENT_ERROR_PACKET;

        	if(downloader != nullptr){
        		packet = object->prot_handler->prepareERR(ERROR_CODE1);
        		log_msg += std::to_string(ERROR_CODE1);
        	}
        	else if(sender != nullptr){
        		packet = object->prot_handler->prepareERR(ERROR_CODE0);
        		log_msg += std::to_string(ERROR_CODE0);
        	}


        	sendDatagram(packet, object->src_address, object, log_msg);
        	object->logger->logEvent(TERMINATE_REQUEST, WARNING);
            RunningTasks::getIstance().freeTaskSlot(object->transferID);
            FileManager::unlinkFile(object->filename);
            pthread_exit(NULL);
        }

        // error:
        if (result == -1) {
            object->logger->logEvent("select() error", ERROR);
            return;
        }

        // timeout:
        else if (result == 0) {
            // timeout krytyczny?:
            object->critical_waiting += object->timeout_type;
            if (object->critical_waiting >= CRITICAL_TO) {
                object->logger->logEvent(CRITICAL_TO_ERROR, ERROR);
                object->is_crit_to = true;
                return;
            }
            // zwykly timeout:
            repeatSending(object);
        }
        // otrzymalismy pakiet:
        else if (FD_ISSET(sock_fd, &readset)) {
            object->critical_waiting = 0;

            if (recvfrom(sock_fd, &received_packet, sizeof(ProtocolPacket), 0, (struct sockaddr*)&src_address, &address_len) == -1) {
                object->logger->logEvent("recvfrom(): error - errno: " + std::string(strerror(errno)), ERROR);
            }
            else {
                char *buffer = new char[sizeof(ProtocolPacket)];
                memcpy(buffer, &received_packet, sizeof(received_packet));

                // skierowanie odebranego pakietu do dalszej obsługi
                object->receiveDatagram(buffer, sizeof(received_packet), src_address);
                delete buffer;
            }
        }
    }
}

/*
 * Wysyła podany pakiet na wskazany adres
 */
void NetworkHandler::sendDatagram(ProtocolPacket packet, sockaddr_in dest_address, NetworkHandler* object, std::string log_msg) {
    //zapamiętaj wysyłany pakiet, jako ostatnio wysłany
    memcpy(&last_packet, &packet, sizeof(packet));

    int packet_size = getPacketSize(packet);

    // wyslij datagram:
    if (sendto(object->sock_fd, &packet, packet_size, 0, (struct sockaddr *) &dest_address, sizeof(dest_address)) == -1) {
        object->logger->logEvent("sendto(): error - errno: " + std::string(strerror(errno)), ERROR);
        //return;
    }
    // loguj informacje o wysłaniu pakietu lub retransmisji
    if (log_msg.length() > 0 && log_msg != RETRANSMISION) {
        object->logger->logEvent(log_msg, INFO);
    }
    else if (log_msg.length() > 0 && log_msg == RETRANSMISION) {
        object->logger->logEvent(log_msg, WARNING);
    }
}

/*
 * Metoda, wywoływana w przypadku konieczności retransmisji pakietu 
 */
void NetworkHandler:: repeatSending(NetworkHandler* object) {
    std::string log_msg = RETRANSMISION;
    sendDatagram(object->last_packet, object->src_address, object, log_msg);
}
/*
 * Obsługuje sytuacje, błędnego odczytu/zapisu do pliku. Np na skutek usunięcia zasobu
 * podczas trawania trasnferu
 */
void NetworkHandler:: HanldeFileError(NetworkHandler* object){
	 object->logger->logEvent(FILE_READING_ERROR, ERROR);
	 ProtocolPacket packet = object->prot_handler->prepareERR(0);
	 object->sendDatagram(packet, object->src_address, object, "");
	 if(object->transferID >= 0){
		 MessagePrinter::print("Transfer stopped. TransferID = " + std::to_string(transferID));
	 }
}

/*
 *  Przyjmuje jako argument pakiet, kótry poddaje interpretacji
 *  sprawdza ile jest w nim użytecznych danych na podstawie typu.
 *  Przykładowo pakiet ACK, zawiera tylko type oraz number należy wysłać tylko pierwsze 4
 *  bajty struktury ProtocolPacket, reszta jest redundantna
 */
int NetworkHandler:: getPacketSize(ProtocolPacket packet) {
    ProtocolHandler handler;

    // 4 bajty użytecznych danych dla ACK
    if(handler.isACK(packet)){
        return 4;
    }
    // 4156 - max, wszystkie dane użyteczne dla DATA
    else if(handler.isDATA(packet)){
        return 4156;
    }
    // 4 bajty użytecznych danych dla ERR
    else if(handler.isERR(packet)){
        return 4;
    }
    // 60 bajtów użytecznych danych dla RQ
    else if(handler.isRD(packet)){
        return 60;
    }
    // 8 bajtów użytecznych danych dla RESP
    else if(handler.isRESP(packet)){
        return 8;
    }
    // 60 bajtów użytecznych danych dla RQ
    else if(handler.isRQ(packet)){
        return 60;
    }

    // maksymalny rozmiar datagramu
    return 4156;
}
