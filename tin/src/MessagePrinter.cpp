#include "MessagePrinter.h"

void MessagePrinter::print(const std::string& toPrint, bool addPrompt){
    static MessagePrinter printer;
    pthread_mutex_lock(&printer.mutex); // Sekcja krytyczna
        std::cout << toPrint << std::endl;
        if (addPrompt)
            std::cout << "$ "<< std::flush;
    pthread_mutex_unlock(&printer.mutex);
}

MessagePrinter:: MessagePrinter(){
    pthread_mutex_init(&mutex, NULL);
}

MessagePrinter:: ~MessagePrinter(){
    pthread_mutex_destroy(&mutex);
}
