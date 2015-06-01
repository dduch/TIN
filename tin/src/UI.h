#ifndef UI_H_
#define UI_H_

#include <list>
#include <stdexcept>
#include <string>
#include "Downloader.h"
#include "MessagePrinter.h"
#include "RunningTasks.h"
#include <unistd.h>

/*
 * Klasa tekstowego interfejsu uzytkownika
 * Wczytuje, interpretuje i wykonuje polecenia uzytkownika
 * Uzycie: utworz obiekt i wywolaj metode run()
 */

class UI{
private:
    std::string command;
    std::string filename;
    int chosenTransferID;

    /*
     * zbiór komend, jakie po wprowadzeniu przez użytkownika zostaną zinterpretowane jako
     * poprawne polecenia
     */
    enum CommandType {
        GET, CANCEL, SHOW, TRANSFERINFO, EXIT, UNKNOWN, HELP
    };

     // Czeka na polecenie: command = "polecenie"
    void waitForCommand();

    // Czeka na nazwe pliku filename = "filename"
    void getFileName();

    // Czeka na identyfikator transferu
    // filename = "nazwa_pobieranego_pliku"
    // chosenTransferID = nr_transferu
    // Zwraca: true - poprawne dane transferu, false - w przeciwnym przypadku
    bool getTransferCredentials();

    // Interpretuje wczytane polecenie
	CommandType parseCommand();

	// Rozpoczyna nowy transfer
	void startNewTransfer();

	// Wysyla zadanie zakonczenia transferu
	void stopTransfer();

	// Wypisuje biezace transfery
	void showTransfers();

	// Wypisuje postepy wybranego transferu
	void chceckTransferProgress();

	// Kończy działanie aplikacji w uporządkowany sposób
	void closeApplication();

public:
	// Watek UI
	void run();
};

#endif
