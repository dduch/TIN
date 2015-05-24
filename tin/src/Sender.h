#ifndef SENDER_H_
#define SENDER_H_
#include "NetworkHandler.h"

class Sender : public NetworkHandler{
private:
	char data_buffer[DATA_MAX_SIZE];				// bufor do przechowywania danych, podlegających transmicji
    bool lastData;									// czy wysłano już ostatni pakiet

	void handleACKPacket(ProtocolPacket rd, sockaddr_in src_address);	// obsługa pakietów ACK
	void handleERRPacket(ProtocolPacket err, sockaddr_in src_address);	// obsługa pakietów ERR

	//odzidziczone funkcje wirtualne
	virtual bool bindSocket();							
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address);

public:
	int sent_data;												// jak dużo danych zostało do tej pory wysłanych

	Sender(std::string filename, sockaddr_in dest_address);		// konstruktor
	~Sender();													// destruktor
	static void* run(void*);									// statyczna metoda uruchamiana w nowym wątku
};

#endif
