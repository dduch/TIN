#ifndef LISTNER_H_
#define LISTENER_H_
#include "NetworkHandler.h"
#include <stdio.h>

/*
 * W każdym węźle sensowne jest istnienie tylko pojedynczego Listenera, wzorzec singleton
 * gwarantuje unikalność instancji klasy, uruchamiany tylko w odzielnym wątku
 */
class Listener : public NetworkHandler {
private:
	Listener();									// prywatny konstruktor
	static Listener* instance;					// statyczny wskaźnik do instancji klasy

protected:
	bool bindSocket();							// dowiąż adres do gniazda

public:
	~Listener();								// zamknij gniazdo - zwolnij port
	static void* run(void*);					// statyczna metoda uruchamiana w nowym wątku
	static Listener* getInstance();				// Zwraca wskaźnik do istniejącej instancji lub tworzy
};												// nową jęsli takowej nie było
#endif
