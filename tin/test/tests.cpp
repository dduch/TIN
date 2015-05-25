#include "../src/ProtocolHandler.h"
#include <iostream>

int test_protocol_handler_01();
int test_protocol_handler_02();
int test_protocol_handler_03();
int test_protocol_handler_04();
int test_protocol_handler_05();
int test_protocol_handler_06();

int main()
{
	if(test_protocol_handler_01() == -1){
		std::cout<<"TEST FAILED: test_protocol_handler_01"<<std::endl;
	}

	if(test_protocol_handler_02() == -1){
		std::cout<<"TEST FAILED: test_protocol_handler_02"<<std::endl;
	}

	if(test_protocol_handler_03() == -1){
		std::cout<<"TEST FAILED: test_protocol_handler_03"<<std::endl;
	}

	if(test_protocol_handler_04() == -1){
		std::cout<<"TEST FAILED: test_protocol_handler_04"<<std::endl;
	}

	if(test_protocol_handler_05() == -1){
		std::cout<<"TEST FAILED: test_protocol_handler_05"<<std::endl;
	}

	if(test_protocol_handler_06() == -1){
		std::cout<<"TEST FAILED: test_protocol_handler_06"<<std::endl;
	}
	std::cout<<"ALL TESTS FINISHED"<<std::endl;
	return 0;
}


int test_protocol_handler_01(){
	//Test DATA packet creation for correct input

	ProtocolHandler ph;
	unsigned int data_size = 50;
	unsigned int packet_number = 5;
	char* data = new char[data_size];
	for(unsigned int i = 0; i<data_size; ++i){
		data[i]=32+i;
	}
	ProtocolPacket pp = ph.prepareDATA(packet_number, data_size, data, data_size);
	for(unsigned int i = 0; i<data_size; ++i){
		if(pp.data[i] != char(32+i))
			return -1;
	}
	if(pp.data_size != data_size)
		return -1;
	if(pp.number != packet_number)
		return -1;
	if(!ph.isDATA(pp))
		return -1;
	if(ph.isRQ(pp))
		return -1;

	return 0;
}


int test_protocol_handler_02(){
	//Test DATA packet creation for incorrect input

	ProtocolHandler ph;
	unsigned int data_size = 50;
	unsigned int big_packet_number = 1048579;
	char* data = new char[data_size];
	for(unsigned int i = 0; i<data_size; ++i){
		data[i]=32+i;
	}
	ProtocolPacket pp = ph.prepareDATA(big_packet_number, data_size, data, data_size);
	if(ph.isDATA(pp))
		return -1;
	return 0;
}


int test_protocol_handler_03(){
	//Test DATA packet creation for incorrect input

	ProtocolHandler ph;
	unsigned int data_size = 5000;
	unsigned int packet_number = 104857;
	char* data = new char[data_size];
	for(unsigned int i = 0; i<data_size; ++i){
		data[i]=32+i;
	}
	ProtocolPacket pp = ph.prepareDATA(packet_number, data_size, data, data_size);
	if(ph.isDATA(pp))
		return -1;
	return 0;
}


int test_protocol_handler_04(){
	//Test preapre datagram and interpret datagram

	ProtocolHandler ph;
	unsigned int data_size = 50;
	unsigned int packet_number = 5;
	char* data = new char[data_size];
	for(unsigned int i = 0; i<data_size; ++i){
		data[i]=32+i;
	}
	ProtocolPacket pp = ph.prepareDATA(packet_number, data_size, data, data_size);
	char* datagram = ph.prepareDatagram(pp);
	ProtocolPacket pp_from_datagram = ph.interpretDatagram(datagram, sizeof(datagram));
	for(unsigned int i = 0; i<data_size; ++i){
		if(pp_from_datagram.data[i] != char(32+i))
			return -1;
	}
	if(pp_from_datagram.data_size != data_size)
		return -1;
	if(pp_from_datagram.number != packet_number)
		return -1;
	if(!ph.isDATA(pp_from_datagram))
		return -1;
	if(ph.isRQ(pp_from_datagram))
		return -1;

	return 0;
}


int test_protocol_handler_05(){
	//Test RQ packet creation for correct input

	ProtocolHandler ph;
	unsigned int filename_size = 50;
	char* filename = new char[filename_size];
	for(unsigned int i = 0; i<filename_size; ++i){
		filename[i]=32+i;
	}
	ProtocolPacket pp = ph.prepareRQ(filename_size, filename, filename_size);
	for(unsigned int i = 0; i<filename_size-1; ++i){
		if(pp.filename[i] != char(32+i))
			return -1;
	}
	if(pp.str_length != filename_size)
		return -1;
	if(ph.isDATA(pp))
		return -1;
	if(!ph.isRQ(pp))
		return -1;

	return 0;

}

int test_protocol_handler_06(){
	//Test RQ packet creation for incorrect input

	ProtocolHandler ph;
	unsigned int filename_size = 51;
	char* filename = new char[filename_size];
	for(unsigned int i = 0; i<filename_size; ++i){
		filename[i]=32+i;
	}
	ProtocolPacket pp = ph.prepareRQ(filename_size, filename, filename_size);
	if(ph.isRQ(pp))
		return -1;
	
	return 0;

}
