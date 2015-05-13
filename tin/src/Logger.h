#ifndef LOGGER_H_
#define LOGGER_H_

#include <ctime>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "MessagePrinter.h"

class Logger{

private:
    int logfd; // Deskryptor pliku logu
    bool loggingEnabled;

public:
    //Konstruktor tworzy plik logu w folderze "log" aplikacji
    //Wynikowa nazwa pliku logu:
    //logfileName + logfileNumber(jeśli >= 0) + znacznik czasu
    Logger(const std::string& logfileName, int logfileNumber);

    //W destruktorze nastepuje zamkniecie pliku logu
    ~Logger();

    // Zapisuje wiadomosc do pliku logu w oddzielnej linii, poprzedzajac ja obecnym znacznikiem czasu
    // Dziala tylko gdy zezwolono na logowanie (domysnie logowanie jest odblokowane)
    void logEvent(const std::string& logMessage);

    // Zezwol na logowanie
    void enableLogging();

    // Zablokuj logowanie  - metoda logEvent nie bedzie zapsisywala komunikatow do pliku
    void disableLogging();
};
#endif

