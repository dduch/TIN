#include "UI.h"
#include <string.h>
#include <iomanip>


// Komunikaty do wypisywania (a przynajmniej wiekszosc z nich)

const std::string CMD_GET = "get";
const std::string CMD_CANCEL = "cancel";
const std::string CMD_SHOW = "show";
const std::string CMD_TRANSFERINFO = "transferinf";
const std::string CMD_HELP = "help";
const std::string CMD_EXIT = "exit";

const std::string MSG_WELCOME = "Welcome to TIN UDP File Downloader!\nType \"help\" for more info";
const std::string MSG_BADID = "ERROR: Incorrect transferid or transfer finished";
const std::string MSG_BADCMD = "ERROR: Unknown command \"";
const std::string MSG_HELP = "\" - type \"help\" for command list";
const std::string MSG_TOO_MANY_DOWNLOADS = "ERROR: Too many running downloads to start new task";
const std::string MSG_STARTED_NEW_DOWNLOAD = "Commencing download: TransferID = \"";
const std::string MSG_NO_RUNNING_TASK = "No running downloads";
const std::string MSG_DOWNLOAD_STOP = "Stopping thread ...";
const std::string MSG_FILE_EXISTS = "You already have such file.";
const std::string MSG_CLOSING = "Closing Application ... Please wait.";



const std::string MSG_HELPINFO =    "Usage:\n\
    get FILE                - download FILE from remote server\n\
    show                    - show running transfers identificators\n\
    transferinf TRANSFER_ID - show progress of specified by TRANSFERID_ID download\n\
    cancel TRANSFER_ID      - terminate specified by TRANSFERID_ID download\n\
    exit                    - terminate all transfers and quit application\n\
    help                    - display this help message";

void UI::run(void){
    bool running = true;
    std::string tmp;

    MessagePrinter::print(MSG_WELCOME);
    while (running) {

        // Do debugowania:
        //RunningTasks::getIstance().debug__printAllTasks();

        waitForCommand(); // command = "polecenie"
        switch (parseCommand()){ // Translacja: command -> CommandType
            case GET :
                getFileName(); // filename = "nazwa_pliku"

                //sprawdz w systemie plikow czy nie mamy juz takiego w resources
                if(FileManager::checkFile(filename)){
                	 MessagePrinter::print(MSG_FILE_EXISTS); // Nieporawny identyfikator
                }
                else{
                	startNewTransfer(); // Rozpocznij nowy transfer
                }
                break;

            case CANCEL :
                if (getTransferCredentials()){
                    stopTransfer(); // Wyslij zadanie zatrzymania transferu
                }
                else {
                    MessagePrinter::print(MSG_BADID); // Nieporawny identyfikator
                }
                break;

            case SHOW :
                showTransfers();
                break;

            case TRANSFERINFO :
                if (getTransferCredentials()){
                    chceckTransferProgress(); // Sprawdz postepy transferu
                }
                else {
                    MessagePrinter::print(MSG_BADID); // Nieporawny identyfikator
                }
                break;

            case UNKNOWN :
                tmp = MSG_BADCMD; // Przygotowanie komunikatu
                tmp.append(command);
                tmp.append(MSG_HELP);
                MessagePrinter::print(tmp); // Niepoprawne polecenie
                break;

            case HELP:
                MessagePrinter::print(MSG_HELPINFO);
                break;

            case EXIT :
            	closeApplication();
            	return;
        }
    }
}

// Czeka na polecenie: command = "polecenie"
void UI::waitForCommand(){
    std::cin >> command;
}

// Interpretuje wczytane polecenie
UI::CommandType UI::parseCommand(){
    CommandType cmd;

    // Interpretacja polecenia
    if (command.compare(CMD_GET)==0) {
        cmd = GET;
    } else if (command.compare(CMD_CANCEL)==0) {
        cmd = CANCEL;
    } else if (command.compare(CMD_SHOW)==0) {
        cmd = SHOW;
    } else if (command.compare(CMD_TRANSFERINFO)==0) {
        cmd = TRANSFERINFO;
    } else if (command.compare(CMD_HELP)==0) {
        cmd = HELP;
    } else if (command.compare(CMD_EXIT)==0) {
        cmd = EXIT;
    } else {
        cmd = UNKNOWN;
    }

    return cmd;
}

// Czekna na nazwe pliku command = "filename"
void UI::getFileName(){
    std::cin >> filename;
}

// Czeka na identyfikator transferu
// filename = "nazwa_pobieranego_pliku"
// chosenTransferID = nr_transferu
// Zwraca: true - poprawne dane transferu, false - w przeciwnym przypadku
bool UI::getTransferCredentials(){
    std::cin >> filename;

    // Odnalezienie znaku separatora - '_'
    unsigned int separatorPosition = filename.rfind('_');
    if (separatorPosition == std::string::npos) // Brak separatora
        return false;

    if (separatorPosition >= filename.length() - 1) // Nie ma nic po znaku separatora
        return false;

    // Wyluskanie liczby z identyfikatora
    std::string number = filename.substr(separatorPosition + 1);

     // Wyrzucenie separatora  i liczby identyfikatora z nazwy pliku
    filename.erase(separatorPosition);

    // Konwersja testowej liczby z identyfkatora na wartsc liczbowa
    try {
        chosenTransferID = std::stoi(number, NULL); // Konwersja tekstu na liczbe
    }
    catch (const std::invalid_argument& ia) { // Nieporawny argument - nie jest liczba
        return false;
    }

    std::string tmp;
    // Sprawdzenie czy identyfiaktor opisuje aktywny transfer
    if (!RunningTasks::getIstance().checkFileName(chosenTransferID, tmp))
        return false; // Zly id
    else if (filename.compare(tmp) != 0)
        return false; // Dobry id, ale nazwa pliku sie nie zgadza
    return true; // OK
}

// Rozpoczyna nowy transfer
void UI::startNewTransfer(){
    int* newTransferID = new int;
    *newTransferID = RunningTasks::getIstance().addNewTask(filename);
    if (*newTransferID < 0) { // Brak miejsca na nowy watek pobierania
        MessagePrinter::print(MSG_TOO_MANY_DOWNLOADS);
    } else {
        // Budowa komunikatu o odpaleniu nowego watku
        std::string tmp = MSG_STARTED_NEW_DOWNLOAD;
        tmp.append(filename);
        tmp.append("_");
        tmp.append(std::to_string(*newTransferID));
        tmp.append("\"");

        MessagePrinter::print(tmp);
        pthread_t tid;
        if (pthread_create(&tid, NULL, Downloader::run, newTransferID)) {
            MessagePrinter::print("Error while initializing thread");
        }
    }
}

// Wysyla zadanie zakonczenia transferu
void UI::stopTransfer() {
    if (!RunningTasks::getIstance().terminateTask(chosenTransferID))
        MessagePrinter::print(MSG_BADID); // Niepoprawny id transferu
    else
        MessagePrinter::print(MSG_DOWNLOAD_STOP);
}

// Wypisuje biezace transfery
void UI::showTransfers() {
    std::list<int> taskList;
    RunningTasks::getIstance().getActiveTransfersID(taskList);
    if (taskList.empty()) {
        MessagePrinter::print(MSG_NO_RUNNING_TASK);
    }
    else {
        std::string tmp;
        int j = 1;
        for(std::list<int>::iterator i = taskList.begin(); i != taskList.end(); ++i){
            RunningTasks::getIstance().checkFileName(*i, tmp);
            if (tmp.empty())
                continue;
            // Przygotowanie napisu do wyswietlenia w postaci nr: "nazwa_pliku_id"
            tmp.insert(0, ": \"");
            tmp.insert(0, std::to_string(j));
            tmp.append("_");
            tmp.append(std::to_string(*i));
            tmp.append("\"");
            MessagePrinter::print(tmp, false); // Wyswietl komunikat bez znaku zachety
            ++j;
        }
        MessagePrinter::print("------------------------------"); // Wyswietl znak zachety
    }
}

// Wypisuje postepy wybranego transferu
void UI::chceckTransferProgress(){
    unsigned long int downloadedBytes;
    unsigned long int allBytes;

    if (!RunningTasks::getIstance().chceckTaskProgress(chosenTransferID, &downloadedBytes, &allBytes))
        MessagePrinter::print(MSG_BADID); // Niepoprawny id transferu
    else { // Przygotuj stosowny komunikat to wypisania
        std::string tmp = "Transfer properties: id = ";
        tmp.append(std::to_string(chosenTransferID));
        tmp.append(", file = \"");
        tmp.append(filename);
        tmp.append("\"\tProgress: ");
        tmp.append(std::to_string(downloadedBytes));
        tmp.append(" of ");
        if (allBytes != 0){ // Zabezpiecznenie na wypadek gdyby Downloader jeszcze nie poznal wielkoscie pobieranego pliku
            tmp.append(std::to_string(allBytes));
            tmp.append(" bytes   ");
            long int percent = (downloadedBytes * 100) / allBytes;
            tmp.append("  ~" + std::to_string(percent) + "%");
        }
        else
            tmp.append("size yet unknown");
        MessagePrinter::print(tmp);
    }
}
/*
 * Kończy działanie programu w uprzorządkowany sposób - wysyła do wszystykich
 * wątków pobierających komendę zakończenia. Oczekuje aż wszystkie się zakończą
 */
void UI:: closeApplication()
{
	MessagePrinter::print(MSG_CLOSING);
	RunningTasks::getIstance().cancelAllThreads();

	/*
	 * Funkcja po wysłaniu polecenia cancel do wszystkich wątków wysyłających i odbierających,
	 * przechodzi w tryb sprawdzania czy już się zakończyły. Nie ma ryzyka zapętlenia - najpoźniej
	 * po czasie timeout, równym największemy z timeoutów niekrytycznych poprawnie zakończy sowje działanie.
	 */
	while(1){
		sleep(1);
		if(RunningTasks::getIstance().checkThreads()){
			break;
		}
	}
	return;
}




