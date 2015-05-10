#ifndef SENDER_H_
#define SENDER_H_
#include "NetworkHandler.h"

class Sender : public NetworkHandler{
private:
	unsigned int current_pacekt = 0x00000;
	char data_buffer[DATA_MAX_SIZE];

	void handleACKPacket(ProtocolPacket rd);
	void handleERRPacket(ProtocolPacket err);

	//odzidziczone funkcje wirtualne
	virtual bool bindSocket();							
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address);
	virtual void startListen(sockaddr_in address, int sockfd);	

public:
	Sender(std::string filename, sockaddr_in dest_address);
	~Sender();
	static void* run(void*);					// statyczna metoda uruchamiana w nowym wątku
};

#endif
