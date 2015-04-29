#include <iostream>
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include "UI.h"
#include "Listener.h"

int main(){
	pthread_t listnerId;

    if(pthread_create(&listnerId, NULL, Listener::run,NULL) != 0)
    {
        std::cout << "Error while initializing thread\n";
        std::exit(-1);
    }

	UI* userUI = new UI();
	userUI->waitForCommand();

	pthread_exit(NULL);
}
