#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "ProtocolHandler.h"
#include "FileManager.h"
#include "Logger.h"

//Arbitralnie przydzielony port obiektom Listenere
#define SERVER_PORT 9055

// Maksymalny rozmiar pakietu danych
#define DATA_MAX_SIZE 4096

// Wartości timeout dla oczekiwania na zadany pakiet
#define RESP_TO 10
#define READ_TO	10
#define DATA_TO 5
#define ACK_TO  2


// Wartość czasu, po upływie którego transmisja kończy się niepowodzeniem
#define CRITICAL_TO 20

/*
 * Klasa abstrakcyjna, udostępniająca insterfejs sieciowy klasom: Downloader, Listener oraz Sender
 * Zadania: tworzenia nowego gniazda, zamykanie gniazda, wysyłanie datagramu, nasłuchiwanie na nadchodzące
 * pakiety
 */
class NetworkHandler {

protected:
	//Pola niezbędne do transferu pliku
	ProtocolPacket received_packet;					// bufor  na obierane wiadomości
	ProtocolPacket last_packet;						// ostatnio wysłany pakiet - na wypadek retransmisji
	ProtocolHandler* prot_handler;					// wskazanie na obiekt kontrolujacy poprawnosc protokolu
	Logger* logger;								 	// wsakazanie na obiekt logera
	struct sockaddr_in my_address;					// sieciowy adres dla danego obiektu
	struct sockaddr_in src_address;					// sieciowy adres zrodlowy - skad przyszedl pakiet
	int sock_fd;									// deskryptor gniazda
	int my_port;									// port dla danego obiektu sieciowego
	int file_size;									// rozmiar przesyłanego pliku w bajtach
	int file_descriptor;							// dekryptor pliku, którego transfer dotyczy
	unsigned int current_pacekt = 0x00000;			// aktualna wartość otrzymanego/wsyłanego pakietu
	std::string filename;							// nazwa pliku, którego transfer dotyczy
	int transferID = - 1;							// id transferu

	/*
	*Funkcje, których implementacja jest niezmienna dla obiektów pochodnych
	*/

	/*
	 * Tworzy gniazdo, jedna implementacja dla wszystkich klas dziedziczących
	 */
	bool createSocket();
	/*
	 * Zamyka gniazdo o wskazanym deskryptorze
	 */
	void closeSocket(int socket);
	/*
	 * Wysyła podany jako argument pakiet, na odpowiedzni adres
	 */
	void sendDatagram(ProtocolPacket packet, sockaddr_in dest_address, NetworkHandler* object, std::string log_msg);

	/*
	 * Rozpoczyna nałuchiwanie na wskazanym gnieździe, danego obiektu.
	 * Wszystkie obiekty klas potomnych muszą realizować takie samo nasłuchiwanie, dlatego
	 * funkcja znalazła się w nadrzdnej klasie abstrakcyjnej
	 */
	virtual void startListen(sockaddr_in address, int sockfd, NetworkHandler* object);

	/*
	 * 	Dokonuje retransmisji ostatnio wysyłanego pakietu
	 * 	W argumencie przekazywane wskazanie na obiekt, na rzecz którego funkcja ma
	 * 	zostać wykonana
	 */
	virtual void repeatSending(NetworkHandler* object);


	/*
	 * Funkcje których implementacja różni się w obiektach pochodnych
	 * Czysto wirtualne -koniczna implementacja w klasach pochodnych
	 */

	// Dowiązuje gniazdo do adresu
	virtual bool bindSocket() = 0;

	/*
	 * Funkcja, której zadaniem jest interpretacja pakietu i skierowanie do
	 * dalszej obsługi w zależnośći jakiego typu pakiet został odebrany.
	 */
	virtual void receiveDatagram(char* buffer, int buff_len, sockaddr_in src_address) = 0;

public:
	time_t start_waiting;					// zmienna, przechowująca czas, od którego należy liczyć tiemout_type
	time_t start_critical_waiting;			// zmienna, przechowująca czas, od którego należy liczyć critical_timeout
	int timeout_type;						// zmienna, przechowująca informację z jakiego typu timeoutem mamy
											// do czynienia - ile czasu może upłynąć bez odpowiedzni na pakiet
	bool is_crit_to = false;				// zmienna, przechowująca informację czy nastąpił krytyczny timeout

	/*
	 * Statyczna metoda startowa dla obiektów pochodnych uruchamianych w nowych wątkach
	 */
	static void run();
	virtual ~NetworkHandler();
};


/*
 * Struktura używana do przekazywania argumentów statycznej metodzie startowej Sendera.
 * Sender urchamiany z poziomu Listnera wymaga zawartych w strukturze argumentów.
 */
struct Arguments{
	char file_name[MAX_FILENAME_SIZE] ;
	sockaddr_in dest_address;
};

#endif 
