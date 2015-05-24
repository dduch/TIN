#include "Logger.h"

#include <iostream>

//Konstruktor tworzy plik logu w folderze "log" aplikacji - UWAGA - folder log musi istniec
//Wynikowa nazwa pliku logu:
//logfileName + logfileNumber(jeśli >= 0) + znacznik czasu
Logger::Logger(std::string caller, const std::string& logfileName, int logfileNumber){
    std::string logfile = "log/";

    // dopisanie nazwy typu wątku wywołującego
    logfile.append(caller);
    logfile.append("_");

    // Dodanie znacznika czasu
    std::time_t timer;
    struct std::tm *timeinfo;

    std::time (&timer);                 // Pobranie aktualnego czasu
    timeinfo = std::localtime(&timer);  // Konwersja czasu do lokalnego standardu

    // Dopisanie znacznika czasu
    logfile.append(std::to_string(timeinfo->tm_year + 1900));
    logfile.append("-");
    logfile.append(std::to_string(timeinfo->tm_mon + 1));
    logfile.append("-");
    logfile.append(std::to_string(timeinfo->tm_mday));
    logfile.append("_");
    logfile.append(std::to_string(timeinfo->tm_hour));
    logfile.append(":");
    logfile.append(std::to_string(timeinfo->tm_min));
    logfile.append(":");
    logfile.append(std::to_string(timeinfo->tm_sec));

    //dopisanie nazwy pliku
    logfile.append("_");
    logfile.append(logfileName);

    // dopisanie identyfikatora wątku wywołującego
    if (logfileNumber >= 0) {
    	logfile.append("_");
        logfile.append(std::to_string(logfileNumber));

    }
    logfile.append(".txt");             // Dodanie formatu tekstowego


    logfd = open(logfile.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH); // Otwarcie pliku logu

    if (logfd < 0) { // Niepowodzenie przy otwieraniu pliku logu - wyswietl komunikat
        logfile.insert(0, "Creating logfile \"");
        logfile.append("\" failed, check for existence and permmision rights of \"log\" directory");
        MessagePrinter::print(logfile);
    }
    else {
        loggingEnabled = true;          // Zezwol na logowanie zdarzen
    }

}

//W destruktorze nastepuje zamkniecie pliku logu
Logger:: ~Logger(){
    if (logfd >= 0) {
        close(logfd);
    }
}

// Zapisuje wiadomosc do pliku logu w oddzielnej linii, poprzedzajac ja obecnym znacznikiem czasu
// Dziala tylko gdy zezwolono na logowanie (domysnie logowanie jest odblokowane)
void Logger::logEvent(const std::string& logMessage, std::string type){
    if (loggingEnabled) {
        std::string msg;
        std::time_t timer;
        struct std::tm *timeinfo;

        std::time (&timer);                 // Pobranie aktualnego czasu
        timeinfo = std::localtime(&timer);  // Konwersja czasu do lokalnego standardu

        // Dopisanie znacznika czasu do wiadomosci
        msg.append(std::to_string(timeinfo->tm_year + 1900));
        msg.append("/");
        msg.append(std::to_string(timeinfo->tm_mon + 1));
        msg.append("/");
        msg.append(std::to_string(timeinfo->tm_mday));
        msg.append(" ");
        msg.append(std::to_string(timeinfo->tm_hour));
        msg.append(":");
        msg.append(std::to_string(timeinfo->tm_min));
        msg.append(":");
        msg.append(std::to_string(timeinfo->tm_sec));
        msg.append(" ");
        msg.append(type);


        msg.append(logMessage);             // Dopisanie wiadomosci
        msg.append("\n");                   // Dodanie znaku nowej linii

        write(logfd, msg.c_str(), msg.length());    // Zapis wiadomosci
    }
}

// Zezwol na logowanie
void Logger::enableLogging(){
    if (logfd >= 0) {
        loggingEnabled = true;
    }
}

// Zablokuj logowanie  - metoda logEvent nie bedzie zapsisywala komunikatow do pliku
void Logger::disableLogging(){
    loggingEnabled = false;
}
