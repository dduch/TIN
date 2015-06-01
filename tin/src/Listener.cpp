#include "Listener.h"
#include <iostream>


// Początkowo nie istnieje żadna instancja Listenera, należy ją utworzyć
Listener* Listener:: instance = NULL;


Listener:: Listener() {
	// utworzenie powiązanego obiektu, zarządzającego obsługą pakietów
    prot_handler = new ProtocolHandler();
}

/*
 * Na zakończenie pracy aplikacji usuwa obiekt Listenera
 * oraz robi "porządek" z używanym gniazdem, zwalnia dynamicznie alokowaną pamieć
 */
Listener:: ~Listener() {
    closeSocket(sock_fd);
    delete(prot_handler);
}

/*
 * Zwraca wskaźnik do istniejącej instancji Listenera lub
 * jeśli takiej nie ma, tworzy nową instancję
 */
Listener* Listener:: getInstance() {
    if (instance == NULL) {
        instance = new Listener();
    }
    return instance;
}

/*
 * Dowiązanie gniazda do Listenera,
 * Listener musi nasłuchiwać na ustalonym arbitralnie porcie serwerowym
 */
bool Listener::bindSocket() {
    bzero(&my_address, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(SERVER_PORT);
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr* ) &my_address, sizeof(my_address)) == -1) {
        return false;
    }
    return true;
}

/*
 *  Funkcja wywoływana po przyjściu pakietu
 *  Po interpretacji pakietu, kieruje do obsługi przez odpowiednie funkcje strujące
 */
void Listener:: receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address) {
    ProtocolPacket packet = prot_handler->interpretDatagram(buffer, buff_len);
    if (prot_handler->isRQ(packet)) {
    	handleRQPacket(packet,src_address);
    }
    else if (prot_handler->isRD(packet)) {
    	handleRDPacket(packet,src_address);
    }
}

/*
 * Obsługa pakietu RQ, jeśli węzeł dysponuje żądanym zasobem zwraca pakiet RESP
 * scr_address to struktura opisująca źródło skąd przyszedł pakiet RQ,
 * posłuży do ewentualnej transmisji pakietów DATA
 */
void Listener::handleRQPacket(ProtocolPacket req, sockaddr_in src_address) {
    std::string file_name(req.filename);
    // jeśli węzeł posiada niepusty plik o wskazanej nazwie - odpowiada na zgłoszenie:
    if ( FileManager::checkFile(file_name)) {
        int file_size = FileManager::getFileSize(file_name);

        if (file_size > 0) {
            ProtocolPacket packet = prot_handler->prepareRESP(file_size);
            sendDatagram(packet, src_address, this, std::string());
        }
    }
}

/*
 * Obsługa pakietu RD - rozpoczyna właściwą transmisję pomiędzy węzłami,
 * req - pakiet RD, żądający rozpoczęcia własciwego transferu,
 *  src_address - adres źródłowy, stąd przyszło żądanie req, na ten adres należy kierować pakiety
 *  z danymi
 */
void Listener::handleRDPacket(ProtocolPacket req, sockaddr_in src_address) {
    Arguments arguments;
    std::string file_n(req.filename);

    int newTransferID = RunningTasks::getIstance().addNewTask("sender_" + file_n);

    if (newTransferID < 0) { // Brak miejsca na nowy watek wysyłania
    	MessagePrinter::print("Cannot send, too many threads");
        return;
    }

    memcpy(&arguments.file_name, &req.filename, MAX_FILENAME_SIZE);
    memcpy(&arguments.dest_address, &src_address, sizeof(src_address));
    memcpy(&arguments.transferID, &newTransferID, sizeof(int));

    pthread_t tid;
    if (pthread_create(&tid, NULL, Sender::run, &arguments)) {
        MessagePrinter::print("Error while initializing thread");
    }
}

/*
 * Statyczna metoda rozpoczynająca pracę Listenera jako nowego wątku,
 * pobiera instancję Listenera, na rzecz którego rozpoczyna nasłuchiwanie
 */
void* Listener:: run(void*) {
    Listener* listener = Listener::getInstance();
    if (listener->createSocket() && listener->bindSocket()) {
        listener->startListen(listener->src_address,listener->sock_fd);
    }

    return (void*)true;
}

/*
 *  Metoda nasłuchująca dla Listenera - dla listnera nasłuchiwanie uproszczone w stosunku do
 *  funkcji nasłuchującej z klasy bazowej, zatem korzystne było wydzielenie jej jako oddzielnej
 *  implementacji
 */
void Listener:: startListen(sockaddr_in src_address, int sock_fd) {
    socklen_t address_len = sizeof(src_address);
    while(1) {
        if (recvfrom(sock_fd, &received_packet, sizeof(ProtocolPacket), 0, (struct sockaddr*)&src_address, &address_len) == -1) {
           // jeśli podczas odbierania wiadomości wykryto błąd to zignoruj go i nasłuchuj dalej
           continue;
        }
        else {
            char *buffer = new char[sizeof(ProtocolPacket)];
            memcpy(buffer, &received_packet, sizeof(received_packet));

            //kieruje odebrany pakiet do dalszej obsługi
            receiveDatagram(buffer, sizeof(received_packet), src_address);
        }
    }
}
