#ifndef PROTOCOLHANDLER_H_
#define PROTOCOLHANDLER_H_

//Rodzaje pakietow jak w dokumentacji
#define DATA 		0x00
#define ACK 		0x01
#define RQ 			0x02
#define RESP 		0x03
#define RD 			0x04
#define ERR 		0x05
#define WTF			0xff

struct ProtocolPacket
{
	unsigned int type :4;			//typ pakietu: RQ, RESP, RD, DATA, ACK, ERR
	unsigned int str_length :8;		//dlugosc nazwy pliku (RQ, RD)
	unsigned int number :20;		//numer (bloku danych (DATA) | potwierdzenia otrzymania bloku danych (ACK) | bledu (ERR) )
	unsigned int data_size :32;		//rozmiar (bloku danych w bajtach (DATA) | calkowity rozmiar pliku (RESP) )
	char* filename;					//dla RQ, RD
	void* data;						//dla DATA
};

class ProtocolHandler
{

public:
	ProtocolHandler();
	struct ProtocolPacket prepareDatagramACK(unsigned int ack_number);
	struct ProtocolPacket prepareDatagramRQ(unsigned int rq_str_length, const char* filename, unsigned int size_of_filename);
	~ProtocolHandler();
};

#endif 
