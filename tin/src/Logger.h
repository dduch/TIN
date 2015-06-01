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

/*
 * Typowe komunikaty logowania zdarzen
*/
const std::string START_SENDING = "Rozpoczęto wysyłanie pliku ";
const std::string START_DOWNLOADING = "Rozpoczęto pobieranie pliku z wezła o adresie ";
const std::string RECEIVED_ACK = "Odebrano ACK";
const std::string SENT_DATA = "Wysłano DATA o numerze ";
const std::string RECEIVED_DATA = "Odebrano DATA o numerze ";
const std::string FINISH_RECEIVING = "Zakończono odbieranie pliku ";
const std::string FINISH_SENDING = "Zakończono wysyłanie pliku ";
const std::string ACK_RECEIVED = "Odebrano ACK dla DATA ";
const std::string RETRANSMISION = "Retransmisja pakietu";
const std::string CRITICAL_TO_ERROR = "Wystąpił krytyczny time - out, transfer zakończony niepowodzeniem";
const std::string FILE_READING_ERROR = "Problem z przygotowaniem pakietu, transfer zakończony niepowodzeniem";
const std::string TERMINATE_REQUEST = "Transfer przerwany przez użytkownika";
const std::string SENT_ERROR_PACKET = "wysłano pakiet ERROR o numerze: ";

/*
 * Typowe prefixy kominikatów
*/
const std::string INFO = "INFO: ";
const std::string WARNING = "WARNING: ";
const std::string ERROR = "ERROR: ";

class Logger{

private:
    int logfd; // Deskryptor pliku logu
    bool loggingEnabled;

public:
    //Konstruktor tworzy plik logu w folderze "log" aplikacji
    //Wynikowa nazwa pliku logu:
    //logfileName + logfileNumber(jeśli >= 0) + znacznik czasu
    //caller - typ obiektu, dla którego tworzony jest skojarzony obiekt loggera
    Logger(std::string caller, const std::string& logfileName, int logfileNumber);

    //W destruktorze nastepuje zamkniecie pliku logu
    ~Logger();

    // Zapisuje wiadomosc do pliku logu w oddzielnej linii, poprzedzajac ja obecnym znacznikiem czasu
    // Dziala tylko gdy zezwolono na logowanie (domysnie logowanie jest odblokowane)
    void logEvent(const std::string& logMessage, std::string type);

    // Zezwol na logowanie
    void enableLogging();

    // Zablokuj logowanie  - metoda logEvent nie bedzie zapsisywala komunikatow do pliku
    void disableLogging();
};
#endif
