#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "ProtocolHandler.h"
#include "FileManager.h"

//Arbitralnie przydzielony port obiektom Listenere
#define SERVER_PORT 9055

// Maksymalny rozmiar pakietu danych
#define DATA_MAX_SIZE 4096

/*
 * Klasa abstrakcyjna, udostępniająca insterfejs sieciowy klasom: Downloader, Listener oraz Sender
 * Zadania: tworzenia nowego gniazda, zamykanie gniazda
 */
class NetworkHandler {

protected:
	//Pola niezbędne do transferu pliku
	ProtocolPacket received_packet;					// bufor  na obierane wiadomości
	struct sockaddr_in my_address;					// sieciowy adres dla danego obiektu
	struct sockaddr_in src_address;					// sieciowy adres zrodlowy - skad przyszedl pakiet
	int sock_fd;									// deskryptor gniazda
	int my_port;									// port dla danego obiektu sieciowego
	ProtocolHandler* prot_handler;							// wskazanie na obiekt kontrolujacy poprawnosc protokolu
	std::string filename;								// nazwa pliku, którego transfer dotyczy
	int file_descriptor;								// dekryptor pliku, którego transfer dotyczy


	//Funkcje, których implementacja jest niezmienna dla obiektów pochodnych
	bool createSocket();								// utworz gniazdo
	void closeSocket(int socket);						// zamknij gniazdo o wskazanym deskryptorze
	void sendDatagram(ProtocolPacket packet, sockaddr_in dest_address, int sock_fd);

	//FUnkcje których implementacja różni się w obiektach pochodnych
	virtual bool bindSocket() = 0;							// dowiąż adres do gniazda
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address) = 0;	// odbierz datagram i przełacz do odpowiedniej funkcji obsługującej
	virtual void startListen(sockaddr_in address, int sockfd) = 0;			// rozpocznij nasłuchiwanie

public:
	static void run();
	virtual ~NetworkHandler();
};

struct Arguments{
	std::string file_name;
	sockaddr_in dest_address;
};

#endif 
