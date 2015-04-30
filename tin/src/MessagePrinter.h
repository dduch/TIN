#ifndef MESSAGEPRINTER_H_
#define MESSAGEPRINTER_H_

#include <iostream>
#include <string>
#include <pthread.h>

/*
 * Klasa do drukowania komunikatow na konsole z zapewnieniem synchronizacji
 */

class MessagePrinter
{
private:
    pthread_mutex_t mutex;
    MessagePrinter();
    ~MessagePrinter();

public:
    // Drukuje komuikat na konsole
    // Na koncu dodawany jest znak nowej linii i znak $
    // Jesli ustawiono flage addPrompt = false nie zostanie dodany znak zachety
    static void print(const std::string& toPrint, bool addPrompt = true);
};

#endif
