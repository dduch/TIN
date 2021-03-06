#include "Downloader.h"
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 * Tworzy obiekt downloadera, ustawia pola: pole z nazwą pliku, będącego podmiotem transferu
 * oraz pole ze wskaźnikiem na obiekt Protocol Handlera
 */
Downloader:: Downloader(std::string filename, int transferID) {
    this->filename = filename;
    this->transferID = transferID;

    //utwórz powiązane obiekty: logera i protocol handlera
    this->prot_handler = new ProtocolHandler();
    this->logger = new Logger("Downloader", filename, pthread_self());
}

/*
 * Zamknij gniazdo i skojarzony port
 */
Downloader:: ~Downloader() {
        RunningTasks::getIstance().freeTaskSlot(transferID);
    closeSocket(sock_fd);
    delete(prot_handler);
    delete(logger);
}

/*
 * Inicjalizacja adresu - przyznanie losowego portu.
 * Ustawienie opcji wysyłania broadcastowego
 */
bool Downloader::bindSocket() {
    // ustawienie możliwośći wysłania broadcastu
    int on = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &(on), sizeof(on));
    // ustalenie adresu i portu:
    bzero(&my_address, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    my_address.sin_port = 0;
    if (bind(sock_fd, (struct sockaddr *) &my_address, sizeof(my_address)) == -1) {
    	return false;
    }
    return true;
}

/*
 * Inicjalizacja połączenia do wysłania broadcastowego żądania
 * żądanie wysyłane na adres broadcastowy oraz na ustalony arbitralnie port serwerowy
 */
bool Downloader:: connectInit() {
    // dla połączenia hamachi adres broadcastowy 25.255.255.255
    struct hostent *hp, *gethostbyname(const char *name);
    hp = gethostbyname("25.255.255.255");
    bzero(&broadcast_address, sizeof(broadcast_address));
    memcpy((char *) &broadcast_address.sin_addr, (char *) hp->h_addr, hp->h_length);
    broadcast_address.sin_family = AF_INET;

    //broadcast_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcast_address.sin_port = htons(SERVER_PORT);

    return true;
}

/*
 * Wyślij broadcastowe zapytanie, w przypadku niepowodzenia zwróć false
 */
bool Downloader:: sendBroadcast(std::string filename) {
    ProtocolPacket req_packet = prot_handler->prepareRQ(strlen(filename.c_str()) + 1, filename.c_str(), strlen(filename.c_str()) +1);
    //zapamiętanie pakietu jako ostatnio wysyłanego - na wypadek konieczności retransmisji:
    memcpy(&last_packet, &req_packet, sizeof(req_packet));

    if (sendto(this->sock_fd, &req_packet, sizeof(req_packet), 0, (struct sockaddr *) &broadcast_address, sizeof(broadcast_address)) == -1) {
        return false;
    }
    return true;
}

/*
 * Implementacja wirtualnej metody. Interpretuje otrzymane dane - z bufora znaków tworzy
 * wiadomość w postaci struktury. W zależności od typu otrzymanej wiadomości
 */
void Downloader:: receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address) {
    ProtocolPacket packet = prot_handler->interpretDatagram(buffer, buff_len);
    if (prot_handler->isRESP(packet)) {
    	handleRESPPacket(packet, src_address);
    }
    else if (prot_handler->isDATA(packet)) {
    	handleDATAPacket(packet, src_address);
    }
    else if (prot_handler->isERR(packet)) {
       	handleERRPacket(packet, src_address);
    }
}

/*
 * Statyczna metoda startowa nowego wątku.
 * Tworzy nowy obiekt downloadera. Po udanym utworzeniu i zainicjowaniu gniazda wsysyła broadcastowe
 * zapytanie do wszystkich węzłów w sieci, po czym przechodzi do trubu nasłuchiwania na ewentualne
 * odpowiedzi.
 */
void* Downloader:: run(void* req) {
    int transferID = *(int*)req;
    std::string filename;

    RunningTasks::getIstance().checkFileName(transferID, filename);
    Downloader * downloader = new Downloader(filename, transferID);

    if (downloader->createSocket() && downloader->bindSocket() && downloader->connectInit()) {
        if (downloader->sendBroadcast(filename)) {
            // oczekuj na odpowiedz:
            downloader->timeout_type = RESP_TO;
            downloader->startListen(downloader->my_address, downloader->sock_fd, downloader);

            // Nastąpił krytyczny timeout - zakończenie transferu:
            if (downloader->is_crit_to) {
                MessagePrinter::print("Downloading stopped. TransferID = " + std::to_string(transferID)
                							+ " No such file or unexpected error");
                RunningTasks::getIstance().freeTaskSlot(transferID);
                FileManager::unlinkFile(filename);
            }
        }
    }

    delete(downloader);
    return (void*)0;
}

/*
 * Obsługa pakietu RESPONSE. Flaga  is_RESP_received umożliwia obsługę tylko pierwszej z
 * otrzymanych odpowiedzi. Tworzy nowy plik do którego będą zapsisywane otrzymywane pakiety DATA.
 * Przygotowuje odpowiedni pakiet RD oraz zleca wysłanie go na adres, z którego przyszła pierwsza
 * wiadomość RESPONSE
 */
void Downloader:: handleRESPPacket(ProtocolPacket resp_packet, sockaddr_in src_address) {
    //odrzuć wszystkie zgłoszenia poza pierwszym otrzymanym RESP
    if (this->is_RESP_received) {
        return;
    }
    // oznacz, że odebrano już jakiś pakiet RESP - ignoruj wszystkie inne
    this->is_RESP_received = true;

    ProtocolPacket packet;
    this->file_descriptor = FileManager::createFile(this->filename);
    memcpy(&this->src_address, &src_address, sizeof(sockaddr_in));

    //bład podczas tworzenia pliku - wyślij paket ERROR, transmisja zakończona
    if (this->file_descriptor < 0) {
    	this->HanldeFileError(this, FILE_WRITINIG_ERROR, ERROR_CODE2);
    }

    // zaktualizuj informacje o rozmiarze pliku, będącego przedmiotem transferu
    this->file_size = resp_packet.data_size;
    RunningTasks::getIstance().updateFileSize(this->transferID, this->file_size);

    // przygotuj pakiet RD do wyslania oraz komunikat dla logera z informacją o adresie, z którym rozpocznie się wymiana pliku:
    packet = prot_handler->prepareRD(this->filename.length() + 1, filename.c_str(), this->filename.length() + 1);
    timeout_type = DATA_TO;

    std::string address(inet_ntoa(src_address.sin_addr));
    std::string port =  std::to_string(ntohs(src_address.sin_port));
    std::string log_msg = START_DOWNLOADING + address + ":" + port;
    sendDatagram(packet, src_address, this, log_msg);
}

/*
 * Obsługa pakietu DATA. Zapisuje dane do pliku, przygotowuje odpowiedni pakiet ACK,
 * zleca wysłanie go na adres skąd zostały odebrane dane.
 */
void Downloader:: handleDATAPacket(ProtocolPacket data_packet, sockaddr_in src_address) {
    bool lastData = false;

    //  jeśli odebrano ostatni blok danych...
    if (data_packet.data_size < MAX_DATA_BLOCK_SIZE){
        lastData = true;
    }

	// jesli to pierwszy pakiet z danymi, zapamietaj skad przyszedł
    if(data_packet.number == 1)
    {
    	  memcpy(&this->src_address, &src_address, sizeof(sockaddr_in));
    }

    ProtocolPacket packet;

    // jesli przyszedl pakiet o numerze o jeden większym niż aktualnie odebrany - wszystko OK:
    if (data_packet.number == current_packet + 1) {

    	if(FileManager::appendFile(this->file_descriptor, data_packet.data, data_packet.data_size) < 0){
    		this->HanldeFileError(this, FILE_WRITINIG_ERROR, ERROR_CODE2);
        }

        received_data += data_packet.data_size;
        RunningTasks::getIstance().updateTaskProgress(this->transferID, data_packet.data_size);
        packet = prot_handler->prepareACK(++current_packet);
    }
    // w p.p nastąpiło zdublowanie pakietu, zignoruj go i wyślij ponownie ACK:
    else {
        timeout_type = DATA_TO;
        packet = prot_handler->prepareACK(current_packet);
        sendDatagram(packet, src_address, this, RETRANSMISION);
        return;
    }

    // jesli ostatni blok DATA to zamknij plik, loguj koniec, wyslij ACK, koncz watek:
    if (lastData) {
        FileManager::closeFile(this->file_descriptor);
        RunningTasks::getIstance().freeTaskSlot(this->transferID);
        sendDatagram(packet, src_address, this, RECEIVED_DATA + std::to_string(current_packet));
        FileManager::renameFile(filename);
        MessagePrinter::print("Download finished. TransferID: " + std::to_string(transferID));
        logger->logEvent(FINISH_RECEIVING, INFO);
        closeSocket(sock_fd);
        delete(prot_handler);
        delete(logger);
        pthread_exit(NULL);
    }
    // w p.p. wyslij ACK i licz timeout, watek nadal trwa:
    else {
        timeout_type = DATA_TO;
        sendDatagram(packet, src_address, this, RECEIVED_DATA + std::to_string(current_packet));
    }
}

/*
 * Obsługuje pakiet ERROR - loguje do pliku informacje o nieudanym trasnferze, końćzy wątek
 */
void Downloader:: handleERRPacket(ProtocolPacket rd, sockaddr_in src_address) {

	// Wyświetlenie i zapsianie informacji o otrzymanym pakiecie błędu
	this->logger->logEvent("Transfer zakończony niepowodzeniem - pakiet ERROR kod: " + rd.number + ProtocolHandler::errors_code[rd.number], ERROR);
    MessagePrinter::print("Downloading stopped - ERROR packet. TransferID = " + std::to_string(transferID));

    // Kontrolowane zakończenie wątku, usunięcie fragmentu pliku, do którego pisano dane
    RunningTasks::getIstance().freeTaskSlot(transferID);
    FileManager::unlinkFile(filename);
    closeSocket(sock_fd);
    delete(prot_handler);
    delete(logger);
    pthread_exit(NULL);
}

