#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_
#include "NetworkHandler.h"
#include "RunningTasks.h"


class Downloader : public NetworkHandler {
private:
	// pola potrzebne do transferu w obiekcie pobierającym
	bool is_RESP_received = false;								// czy odebrano jakikolwiek pakiet RESP
	struct sockaddr_in broadcast_address;		    			// adres broadcastowy, na który należy wysłać żądanie

	bool sendBroadcast(std::string filename);					// wyślij żądanie udostępnienia pliku do wszystkich węzłów
	bool connectInit();											// zainicjuj połączenie

	void handleRESPPacket(ProtocolPacket resp_packet);
	void handleDATAPacket(ProtocolPacket rd, sockaddr_in src_address);
	void handleERRPacket(ProtocolPacket err, sockaddr_in src_address);
	
	//odzidziczone funkcje wirtualne
	virtual bool bindSocket();							
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address);
	virtual void startListen(sockaddr_in address, int sockfd);			 									

public:
	Downloader(std::string filename);
	~Downloader();
	static void* run(void*);					// statyczna metoda uruchamiana w nowym wątku
};

#endif
