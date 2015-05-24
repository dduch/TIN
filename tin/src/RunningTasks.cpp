#include "RunningTasks.h"

int RunningTasks::ntasks = 0;

void RunningTasks::setMaxTask(int maxTask){
    RunningTasks::ntasks = maxTask;
}

RunningTasks& RunningTasks::getIstance(){
    static RunningTasks instance;
    return instance;
}

RunningTasks:: RunningTasks(){
    lastTakenSlot = -1;
    tasks = new RunningTask[RunningTasks::ntasks];
    mutexes = new pthread_mutex_t[RunningTasks::ntasks];

    // Obowiazkowa inicjalizacja muteksow
    for (int i = 0; i < RunningTasks::ntasks; ++i) {
        pthread_mutex_init(&mutexes[i], NULL);
    }
}

RunningTasks:: ~RunningTasks(){
    delete[] tasks;

    for (int i = 0; i < RunningTasks::ntasks; ++i) {
        pthread_mutex_destroy(&mutexes[i]);
    }
    delete[] mutexes;
}

// Przydziela identyfikator dla transferu pliku o nazwie zadanej parametrem file
// Zwraca: id transferu, lub -1 gdy zbyt wiele aktywnych transferow
int RunningTasks::addNewTask(const std::string& file){
    // Przegladamy tablice w celu znalezienia wolnego miejsca
    int i = lastTakenSlot;
    do {
        i = (i+1) % ntasks;
        if (!tasks[i].dirty) { // Znaleziono wolne miejsce - inicjalizacja struktury
            tasks[i].dirty = true;
            tasks[i].interruptReq = false;
            tasks[i].filename.assign(file);
            lastTakenSlot = i;
            return i;
        }
    } while (i != lastTakenSlot); // Przejrzelismy cala tablice, brak wolnych miejsc
    return -1;
}

// Zwalnia miejsce w tablicy transferow - wywolywana na zakonczenie dzialania watku
void RunningTasks::freeTaskSlot(int transferID){
    pthread_mutex_lock(&mutexes[transferID]); // Sekcja krytyczna
        tasks[transferID].downloadedBytes = 0;
        tasks[transferID].allBytes = 0;
        tasks[transferID].filename.clear();
        tasks[transferID].interruptReq = false;
        tasks[transferID].dirty = false;
    pthread_mutex_unlock(&mutexes[transferID]);
}

// Ustawia flage zadania zakonczenia watku
bool RunningTasks::terminateTask(int transferID){
    if (transferID >= ntasks || transferID < 0)
        return false; // Niepoprawny id

    pthread_mutex_lock(&mutexes[transferID]); // Sekcja krytyczna
        tasks[transferID].interruptReq = true;
    pthread_mutex_unlock(&mutexes[transferID]);

    return true;
}

// Ustawia rozmiar pliku pobieranego w transferze transferID na filesize
void RunningTasks::updateFileSize(int transferID, unsigned int filesize){
    pthread_mutex_lock(&mutexes[transferID]); // Sekcja krytyczna
        tasks[transferID].allBytes = filesize;
    pthread_mutex_unlock(&mutexes[transferID]);
}

// Akutalizuje liczbe pobranych w transferze transferID bajtow, dodajac deltaSize do obecnej wartosci
// Uwaga : zwraca true, gdy ustawiona jest flaga zadania przerwania watku
bool RunningTasks::updateTaskProgress(int transferID, unsigned int deltaSize){
    bool terminationFlag;
    pthread_mutex_lock(&mutexes[transferID]); // Sekcja krytyczna
        tasks[transferID].downloadedBytes += deltaSize;
        terminationFlag = tasks[transferID].interruptReq;
    pthread_mutex_unlock(&mutexes[transferID]);
    return terminationFlag;
}

// Udostepnia informacje o statusie transferu:
// - liczbe pobranych bajtow (downloadedB)
// - rozmiar pliku (fsize)
// - nazwe pliku (fname)
// Zwraca: true - gdy transfer w toku, false - gdy transfer zakonczony/nieaktywny/niepoprawny id
bool RunningTasks::chceckTaskProgress(int transferID, unsigned long int* downloadedB, unsigned long int* fsize){
    if (transferID >= ntasks || transferID < 0)
        return false; // Niepoprawny id

    bool runningFlag;
    pthread_mutex_lock(&mutexes[transferID]); // Sekcja krytyczna
        *downloadedB = tasks[transferID].downloadedBytes;
        *fsize = tasks[transferID].allBytes;
        runningFlag = tasks[transferID].dirty;
    pthread_mutex_unlock(&mutexes[transferID]);
    return runningFlag;
}

bool RunningTasks::checkTerminateFlag(int transferID)
{
    if (transferID >= ntasks || transferID < 0)
        return false; // Niepoprawny id

    bool reply = false;
    pthread_mutex_lock(&mutexes[transferID]); // Sekcja krytyczna
    	if(tasks[transferID].interruptReq)
    	{
    		reply = true;
    	}
    pthread_mutex_unlock(&mutexes[transferID]);
	return reply;
}


// Umieszcza w fname nazwe pliku transferu transferID
bool RunningTasks::checkFileName(int transferID, std::string& fname){
    if (transferID >= ntasks || transferID < 0)
        return false; // Niepoprawny id

    fname.assign(tasks[transferID].filename);
    return true;
}

// Umieszcza w obiekcie transfersList identyfikatory aktywnych transferow
void RunningTasks::getActiveTransfersID(std::list<int>& transfersList){
    for (int i = 0; i < ntasks; ++i) {
        if(tasks[i].dirty) {
            transfersList.push_back(i);
        }
    }
}

// Funkca testowa, drukuje zawartosc tablicy transferow na stdout
void RunningTasks::debug__printAllTasks() {
    for (int i = 0; i < ntasks; ++i) {
    std::cout << "\t\t\t\t" << i <<": ";
    if (tasks[i].dirty)
        std::cout << " active ";
    else
        std::cout << "nactive ";
    std::cout << "\"" << tasks[i].filename <<"\"\t";
    std::cout << tasks[i].downloadedBytes << " of " << tasks[i].allBytes << " B ";
    if (tasks[i].interruptReq)
        std::cout << "INT";
    std::cout << "\n";
    }
}
