#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_
#include "NetworkHandler.h"
#include "RunningTasks.h"

/*
 *  Obiekty downloadera są odpowiedzialne za wyszukanie w sieci zasobu oraz za relizacje transferu,
 *  downloader startuje w nowym wątku - statyczna metoda run
 */
class Downloader : public NetworkHandler {
private:

	/*
	 *  Pola potrzebne do transferu w obiekcie pobierającym:
	 */
        bool is_RESP_received = false;                      // czy odebrano jakikolwiek pakiet RESP
        struct sockaddr_in broadcast_address;		    	// adres broadcastowy, na który należy wysłać żądanie

        bool sendBroadcast(std::string filename);           // wyślij żądanie udostępnienia pliku do wszystkich węzłów
        bool connectInit();                                 // zainicjuj połączenie

	/*
	 * Funkcje do obsługi poszczególnych typów pakietów
	 * Każda z nich dostaje jako argumenty: strukture zawierająca odpowiedni pakiet, a także
	 * strukture adresu docelowego na jaki należy odpowiedzieć
	 */
	void handleRESPPacket(ProtocolPacket resp_packet, sockaddr_in src_address);
	void handleDATAPacket(ProtocolPacket rd, sockaddr_in src_address);
	void handleERRPacket(ProtocolPacket err, sockaddr_in src_address);
	
	/*
     * Odziedziczone funkcje wirtualne
	 */
	/*
	 * Funkcja realizująca 'bindowanie' gniazda
	 */
	virtual bool bindSocket();
	/*
	 * Funkcja wywoływana po otrzymaniu pakietu jako argumenty przyjmuje tablice z przesłanymi danymi,
	 * długość tablicy oraz adres źródłowy skąd wiadomość została odebrana
	 */
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address);

public:
    Downloader(std::string filename, int transferID);
    ~Downloader();

    int received_data; 			// przechowuje aktualnie pobraną liczbę bajtów
	/*
	 * statyczna metoda uruchamiana  jako metoda startowa w nowym wątku
	 */
	static void* run(void*);
};

#endif
