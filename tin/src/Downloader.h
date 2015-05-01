#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_
#include "NetworkHandler.h"

/*
 * Instancja downloadera tworzona dla każdego nowego zapytania jako oddzielny wątek
 */
class Downloader : public NetworkHandler {
private:
	bool sendBroadcast(char* request);				// wyślij żądanie udostępnienia pliku do wszystkich węzłów
	bool bindSocket();								// dowiąż gniazdo do adresu węzła
	bool connectInit();								// zainicjuj połączenie
	
	struct sockaddr_in broadcast_address;		    // adres broadcastowy, na który należy wysłać żądanie
public:
	Downloader(char* req);							// na podstawie żadania rozpoczyna jego obsługę
	~Downloader();
};

#endif
