#ifndef SENDER_H_
#define SENDER_H_
#include "NetworkHandler.h"

class Sender : public NetworkHandler{
private:
	char data_buffer[DATA_MAX_SIZE];

        bool lastData;

	void handleACKPacket(ProtocolPacket rd, sockaddr_in src_address);
	void handleERRPacket(ProtocolPacket err, sockaddr_in src_address);

	//odzidziczone funkcje wirtualne
	virtual bool bindSocket();							
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address);

public:
	int sent_data;
	Sender(std::string filename, sockaddr_in dest_address);
	~Sender();
	static void* run(void*);					// statyczna metoda uruchamiana w nowym wątku
};

#endif
