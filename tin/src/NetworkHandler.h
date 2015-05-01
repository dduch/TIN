#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

//Arbitralnie przydzielony port obiektom Listenere
#define SERVER_PORT 9055

/*
 * Klasa abstrakcyjna, udostępniająca insterfejs sieciowy klasom: Downloader, Listener oraz Sender
 * Zadania: tworzenia nowego gniazda, zamykanie gniazda
 */
class NetworkHandler {

protected:
	char buffer[500];									// bufor 500B na obierane wiadomości
	struct sockaddr_in my_address;						// sieciowy adres dla danego obiektu
	struct sockaddr_in src_address;						// sieciowy adres zrodlowy - skad przyszedl pakiet
	int sock_fd;										// deskryptor gniazda
	int my_port;										// port dla danego obiektu sieciowego

	void startListen(sockaddr_in address, int sockfd);	// zacznij nasłuchiwac na gniezdzie
	bool createSocket();								// utworz gniazdo
	void closeSocket(int socket);						// zamknij gniazdo o wskazanym deskryptorze
	virtual bool bindSocket() = 0;						// dowiąż adres do gniazda, funckcja zdeklarowana
														// jako wirtualna z uwagi na rożnice w klasach pochodnych

public:
	void sendDatagram();
	void receiveDatagram();
	static void run();
};

#endif 
