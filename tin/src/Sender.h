#ifndef SENDER_H_
#define SENDER_H_
#include "NetworkHandler.h"
#include "RunningTasks.h"

class Sender : public NetworkHandler{
private:
    char data_buffer[MAX_DATA_BLOCK_SIZE];			// bufor do przechowywania danych, podlegających transmicji
    bool lastData;						// czy wysłano już ostatni pakiet

	void handleACKPacket(ProtocolPacket rd, sockaddr_in src_address);	// obsługa pakietów ACK
	void handleERRPacket(ProtocolPacket err, sockaddr_in src_address);	// obsługa pakietów ERR

	//odzidziczone funkcje wirtualne
	virtual bool bindSocket();							
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address);

public:
	int sent_data;				// jak dużo danych zostało do tej pory wysłanych															// destruktor
    static void* run(void*);	// statyczna metoda uruchamiana w nowym wątku

    /*
     *	Konstruktor, przyjmujący w argumentach:
     *	nazwę pliku bedącego przedmiotem transferu,
     *	adres źródłowy - adres gniazda skąd przychodzą pakiety
     *	identyfikator transferu
     */
    Sender(std::string filename, sockaddr_in dest_address, int transferID);
    ~Sender();
};

#endif
