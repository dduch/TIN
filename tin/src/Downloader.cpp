#include "Downloader.h"
#include <iostream>

/*
 * Tworzy obiekt downloadera, ustawia pola: pole z nazwą pliku, będącego podmiotem transferu
 * oraz pole ze wskaźnikiem na obiekt Protocol Handlera
 */
Downloader:: Downloader(std::string filename) {
	this->filename = filename;
	this->prot_handler = new ProtocolHandler();
}

/*
 * Zamknij gniazdo i skojarzony port
 */
Downloader:: ~Downloader() {
	closeSocket(sock_fd);
}

/*
 * Inicjalizacja adresu - przyznanie losowego portu.
 * Ustawienie opcji wysyłania broadcastowego
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
 * Inicjalizacja połączenia do wysłania broadcastowego żądania
 * żądanie wysyłane na adres broadcastowy oraz na ustalony arbitralnie port serwerowy
 */
bool Downloader:: connectInit(){
	bzero(&broadcast_address, sizeof(broadcast_address));
	broadcast_address.sin_family = AF_INET;
	broadcast_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	broadcast_address.sin_port = htons(SERVER_PORT);

	return true;
}

/*
 * Wyślij broadcastowe zapytanie, w przypadku niepowodzenia zwróć false
 */
bool Downloader:: sendBroadcast(std::string filename){
	ProtocolPacket req_packet = prot_handler->prepareRQ(strlen(filename.c_str()) + 1, filename.c_str(), strlen(filename.c_str()) +1 );

	if(sendto(this->sock_fd, &req_packet, sizeof(req_packet), 0, (struct sockaddr *) &broadcast_address, sizeof(broadcast_address)) == -1){
		std::cout<<"nie wysłano";
		return false;
	}
	return true;
}

/*
 * Implementacja wirtualnej metody. Interpretuje otrzymane dane - z bufora znaków tworzy
 * wiadomość w postaci struktury. W zależności od typu otrzymanej wiadomości
 */
void Downloader:: receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address){
	ProtocolPacket packet = prot_handler->interpretDatagram(buffer, sizeof(buffer));

    if(prot_handler->isRESP(packet)){
    	handleRESPPacket(packet, src_address);
    }
    else if(prot_handler->isDATA(packet)){
    	handleDATAPacket(packet, src_address);
    }
}

/*
 * Statyczna metoda startowa nowego wątku.
 * Tworzy nowy obiekt downloadera. Po udanym utworzeniu i zainicjowaniu gniazda wsysyła broadcastowe
 * zapytanie do wszystkich węzłów w sieci, po czym przechodzi do trubu nasłuchiwania na ewentualne
 * odpowiedzi.
 */
void* Downloader:: run(void* req){
	std::string filename;
	std::string& file_ref = filename;

	RunningTasks::getIstance().checkFileName(*(int*)req, file_ref);
	Downloader* downloader = new Downloader(filename);

	if(downloader->createSocket() && downloader->bindSocket() && downloader->connectInit()){
		if(downloader->sendBroadcast(filename)){
			int off = 1;
			setsockopt(downloader->sock_fd, SOL_SOCKET, SO_BROADCAST, &(off), sizeof(off));
			downloader->startListen(downloader->my_address, downloader->sock_fd, downloader);
		}
	}
	return (void*)0;
}

/*
 * Obsługa pakietu RESPONSE. Flaga  is_RESP_received umożliwia obsługę tylko pierwszej z
 * otrzymanych odpowiedzi. Tworzy nowy plik do którego będą zapsisywane otrzymywane pakiety DATA.
 * Przygotowuje odpowiedni pakiet RD oraz zleca wysłanie go na adres, z którego przyszła pierwsza
 * wiadomość RESPONSE
 */
void Downloader:: handleRESPPacket(ProtocolPacket resp_packet, sockaddr_in src_address){

	//odrzuć wszystkie zgłoszenia poza pierwszym otrzymanym RESP
	if(this->is_RESP_received){
		return;
	}

	this->is_RESP_received = true;
	this->file_descriptor = FileManager::openFile(this->filename, WRITE_F);

	const char* file_name = filename.c_str();

	ProtocolPacket packet = prot_handler->prepareRD(this->filename.length() + 1, file_name, this->filename.length() + 1);
	sendDatagram(packet,src_address, sock_fd);
}

/*
 * Obsługa pakietu DATA. Zapisuje dane do pliku, przygotowuje odpowiedni pakiet ACK,
 * zleca wysłanie go na adres skąd zostały odebrane dane.
 */
void Downloader:: handleDATAPacket(ProtocolPacket data_packet, sockaddr_in src_address){
	FileManager::appendFile(this->file_descriptor,data_packet.data, strlen(data_packet.data));

	ProtocolPacket packet = prot_handler->prepareACK(this->current_pacekt +1);
	sendDatagram(packet, src_address, sock_fd);
}
