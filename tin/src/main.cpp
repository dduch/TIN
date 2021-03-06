#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include "UI.h"
#include "Listener.h"
#include "RunningTasks.h"

int main(){

	pthread_t listnerId;

    if(pthread_create(&listnerId, NULL, Listener::run,NULL) != 0)
    {
        std::cout << "Error while initializing thread\n";
        return -1;
    }

    RunningTasks::setMaxTask(10); // Okreslamy max liczbe obslugiwanych downloaderow
    
    UI userUI;
    userUI.run();
  
    std::exit(0);
	return 0;
}
