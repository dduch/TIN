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

<<<<<<< HEAD

// Typowe komunikaty logowania zdarzen
const std::string START_SENDING = "Rozpoczęto wysyłanie pliku ";
const std::string START_DOWNLOADING = "Rozpoczęto pobieranie pliku ";
const std::string RECEIVED_ACK = "Odebrano potwierdzenie odebrania pakietu numer ";
const std::string SENT_DATA = "Wysłano pakiet z danymi o numerze ";
const std::string RECEIVED_DATA = "Odebrano pakiet z danymi o numerze ";
const std::string FINISH_RECEIVING = "Zakończono odbieranie pliku ";
const std::string FINISH_SENDING = "Zakończono wysyłanie pliku ";

// Typowe prefixy dla komunikatów
const std::string INFO = "INFO: ";
const std::string WARNING = "INFO: ";
const std::string ERROR = "ERROR: ";

=======
>>>>>>> 662ae1261bb4995e3bd47e4f013ebb588018d695
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
<<<<<<< HEAD
    void logEvent(const std::string& logMessage, std::string type);
=======
    void logEvent(const std::string& logMessage);
>>>>>>> 662ae1261bb4995e3bd47e4f013ebb588018d695

    // Zezwol na logowanie
    void enableLogging();

    // Zablokuj logowanie  - metoda logEvent nie bedzie zapsisywala komunikatow do pliku
    void disableLogging();
};
#endif

