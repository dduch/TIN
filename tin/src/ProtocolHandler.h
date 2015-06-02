#ifndef PROTOCOLHANDLER_H_
#define PROTOCOLHANDLER_H_
#include <string>
#include <vector>

//Rodzaje błędów
#define ERROR_CODE0 0
#define ERROR_CODE1 1
#define ERROR_CODE2 2
#define ERROR_CODE3 3
#define ERROR_CODE4 4

//Rodzaje pakietow jak w dokumentacji
#define DATA 		0x00
#define ACK 		0x01
#define RQ 			0x02
#define RESP 		0x03
#define RD 			0x04
#define ERR 		0x05
#define WTF			0xff

//maksymalna wielkosc w bajtach pola danych w DATA:
#define MAX_DATA_BLOCK_SIZE 4096

//maksymalna długość nazwy pliku
#define MAX_FILENAME_SIZE 50



struct ProtocolPacket
{
	unsigned int type :4;							//typ pakietu: RQ, RESP, RD, DATA, ACK, ERR
	unsigned int str_length :8;						//dlugosc nazwy pliku (RQ, RD)
	unsigned int number :20;						//numer (bloku danych (DATA) | potwierdzenia otrzymania bloku danych (ACK) | bledu (ERR) )
	unsigned int data_size :32;						//rozmiar (bloku danych w bajtach (DATA) | calkowity rozmiar pliku (RESP) )
	char filename[MAX_FILENAME_SIZE];				//dla RQ, RD
	char data[MAX_DATA_BLOCK_SIZE];					//dla DATA
};

class ProtocolHandler
{
public:
	ProtocolHandler();
	/*
	 * Funckce, przygotowujące odpowiedni typ pakietu
	 */
	struct ProtocolPacket prepareDATA(unsigned int data_number, unsigned int data_data_size, char* data, unsigned int size_of_data);
	struct ProtocolPacket prepareACK(unsigned int ack_number);
	struct ProtocolPacket prepareRQ(unsigned int rq_str_length, const char* filename, unsigned int size_of_filename);
	struct ProtocolPacket prepareRESP(unsigned int resp_data_size);
	struct ProtocolPacket prepareRD(unsigned int rd_str_length, const char* filename, unsigned int size_of_filename);
	struct ProtocolPacket prepareERR(unsigned int err_number);

	/*
	 * Funkcje, odpowiadające za interpretację otrzymanych pakietów
	 */
	unsigned int interpretDatagramType(struct ProtocolPacket packet);
	unsigned int isDATA(struct ProtocolPacket packet);
	unsigned int isACK(struct ProtocolPacket packet);
	unsigned int isRQ(struct ProtocolPacket packet);
	unsigned int isRESP(struct ProtocolPacket packet);
	unsigned int isRD(struct ProtocolPacket packet);
	unsigned int isERR(struct ProtocolPacket packet);

	char* prepareDatagram(struct ProtocolPacket packet);
	struct ProtocolPacket interpretDatagram(char* datagram, unsigned int datagram_size);

	static std::vector <std::string> errors_code;

	~ProtocolHandler();
};

#endif 
