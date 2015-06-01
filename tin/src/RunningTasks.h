#ifndef RUNNINGTASKS_H_
#define RUNNINGTASKS_H_

#include <list>
#include <iostream>
#include <pthread.h>
#include <string>

/*
 * Zmodyfikowalem nieco strukture RunningTask w stosunku do dokumentacji:
 * 1. Przyjalem bardziej obiektowa konwencje: char* -> string
 * 2. Zamiast liczby blokow przechowuje rozmiar w bajtach, co powinno byc bardziej przyjazne dla uzytkownika
 * 3. Brak indentyfikatora watku w RunningTask, pthread udostepnia mechanizm sprawdzenia identyfikatorow watkow
 */

/*
 * Klasa zarzadcy biezacych transferow
 */
class RunningTasks
{
private:

    // Pojedynczy element tablicy biezacych zadan (tak jak w dok. wstepnej)
    struct RunningTask
    {
        bool dirty;                     	 // Flaga zkonczenia watku
        bool interruptReq;              	 // Flaga zadania przerwania watku
        std::string filename;           	 // Nazwa poberianego pliku
        unsigned long int downloadedBytes;   // Liczba pobranych bajtow danych
        unsigned long int allBytes;          // Rozmiar pliku w bajtach

        RunningTask() : dirty(false), interruptReq(false), filename(), downloadedBytes(0), allBytes(0) {}
    };

    static int ntasks;                  // Rozmiar tablicy transferow

    int lastTakenSlot;                  // Ostatnio zajete miejsce w tablicy transferow
    RunningTask* tasks;                 // Tablica transferow
    pthread_mutex_t *mutexes;           // Tablica muteksow chroniacych dostepu do tablicy transferow

    RunningTasks();
    ~RunningTasks();

public:

    // Ustala maksymalna liczbe obslugiwanych jednoczesnie transferow
    static void setMaxTask(int maxTask);

    // Zwraca instancje zarzadcy transferow
    static RunningTasks& getIstance();

    // Przydziela identyfikator dla transferu pliku o nazwie zadanej parametrem file
    // Zwraca: id transferu, lub -1 gdy zbyt wiele aktywnych transferow
    int addNewTask(const std::string& file);

    // Zwalnia miejsce w tablicy transferow - wywolywana na zakonczenie dzialania watku
    void freeTaskSlot(int transferID);

    // Ustawia flage zadania zakonczenia watku
    // Zwraca: false gdy niepoprawny identyfikator
    bool terminateTask(int transferID);

    // Ustawia rozmiar pliku pobieranego w transferze transferID na filesize
    void updateFileSize(int transferID, unsigned int filesize);

    // Akutalizuje liczbe pobranych w transferze transferID bajtow, dodajac deltaSize do obecnej wartosci
    // Uwaga : zwraca true, gdy ustawiona jest flaga zadania przerwania watku
    bool updateTaskProgress(int transferID, unsigned int deltaSize);

    // Udostepnia informacje o statusie transferu:
    // - liczbe pobranych bajtow (downloadedB)
    // - rozmiar pliku (fsize)
    // Zwraca: true - gdy transfer w toku, false - gdy transfer zakonczony/nieaktywny/niepoprawny identyfikator
    bool chceckTaskProgress(int transferID, unsigned long int* downloadedB, unsigned long int* fsize);

    // Umieszcza w fname nazwe pliku transferu transferID
    // Zwraca: false gdy niepoprawny identyfikator
    bool checkFileName(int transferID, std::string& fname);

    // Umieszcza w obiekcie transfersList identyfikatory aktywnych transferow
    void getActiveTransfersID(std::list<int>& transfersList);

    // Funkca testowa, drukuje zawartosc tablicy transferow na stdout
    // Format: transferID: status "nazwa pliku" bajty_pobrane of rozmiar_pliku B [INT - jesli ustawiona zadanie przerwania]
    void debug__printAllTasks();

    //Zwraca informacje czy transfer o zadanym transferID ma się zakończyć
    bool checkTerminateFlag(int transferID);

	// Sprawdza czy wszystkie pracujące wątki już sie zakończyły
	bool checkThreads();

	// Funkcja, która realizuje cancel dla wszystkich aktywnych wątków jednocześnie
	void cancelAllThreads();
};

#endif
