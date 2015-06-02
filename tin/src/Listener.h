#ifndef LISTNER_H_
#define LISTENER_H_

#include "NetworkHandler.h"
#include "RunningTasks.h"
#include "Sender.h"
#include <stdio.h>



/*
 * W każdym węźle sensowne jest istnienie tylko pojedynczego Listenera, wzorzec singleton
 * gwarantuje unikalność instancji klasy, uruchamiany tylko w odzielnym wątku
 */
class Listener : public NetworkHandler {
private:
	Listener();                         // prywatny konstruktor
    static Listener* instance;          // statyczny wskaźnik do instancji klasy
                                        // wynika z przyjętego wzorca Singleton

    // Rozpoczyna nasłuchiwanie na wskazanym gnieździe, do src_address zapisuje informację skąd pochodzi odebrany pakiet:
	void startListen(sockaddr_in src_address, int sock_fd);

protected:
	//Funkcje obsługjące pakiety w zależności od jego typu
	void handleRQPacket(ProtocolPacket req, sockaddr_in src_address);
	void handleRDPacket(ProtocolPacket req, sockaddr_in src_address);

    //odziedziczone funkcje wirtualne
	virtual bool bindSocket();							
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address);


public:
    ~Listener();                         // zamknij gniazdo - zwolnij port
    static void* run(void*);            // statyczna metoda startowa uruchamiana w nowym wątku
    static Listener* getInstance();     // Zwraca wskaźnik do istniejącej instancji lub tworzy
                                         // nową jesli takowej nie było
};

#endif
