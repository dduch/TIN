#include "ProtocolHandler.h"
#include<iostream>
#include<string.h>

ProtocolHandler:: ProtocolHandler(){}

ProtocolHandler:: ~ProtocolHandler(){}

struct ProtocolPacket ProtocolHandler::prepareDATA(unsigned int data_number, unsigned int data_data_size, char* data, unsigned int size_of_data)
{
	struct ProtocolPacket data_packet;
	data_packet.data = new char[size_of_data];
	strncpy(data_packet.data, data, size_of_data);
	//data_number do 20 bitow, data_data_size do 12 bitow, size_of_data do 1024*32, inaczej WTF
	data_packet.type = (data_number < 1048576 && data_data_size < 4096 && size_of_data < 1024*32)? DATA : WTF;
	data_packet.number = data_number;
	data_packet.data_size = data_data_size;
	return data_packet;
}

struct ProtocolPacket ProtocolHandler::prepareACK(unsigned int ack_number)
{
	struct ProtocolPacket ack_packet;
	//ack_number tylko do 20 bitow, inaczej WTF
	ack_packet.type = (ack_number < 1048576)? ACK : WTF ;
	ack_packet.number = ack_number;
	return ack_packet;
}

struct ProtocolPacket ProtocolHandler::prepareRQ(unsigned int rq_str_length, const char* filename, unsigned int size_of_filename)
{
	struct ProtocolPacket rq_packet;
	rq_packet.filename = new char[size_of_filename];
	strncpy(rq_packet.filename, filename, size_of_filename);
	rq_packet.filename[size_of_filename - 1] = '\0';
	//rq_str_length tylko do 8 bitow oraz nazwa pliku do 50, inaczej WTF
	rq_packet.type = (rq_str_length < 256 && size_of_filename < 51)? RQ : WTF ;
	rq_packet.str_length = rq_str_length;
	return rq_packet;
}

struct ProtocolPacket ProtocolHandler::prepareRESP(unsigned int resp_data_size)
{
	struct ProtocolPacket resp_packet;
	resp_packet.type = RESP;
	resp_packet.data_size = resp_data_size;
	return resp_packet;
}

struct ProtocolPacket ProtocolHandler::prepareRD(unsigned int rd_str_length, const char* filename, unsigned int size_of_filename)
{
	struct ProtocolPacket rd_packet;
	rd_packet.filename = new char[size_of_filename];
	strncpy(rd_packet.filename, filename, size_of_filename);
	rd_packet.filename[size_of_filename - 1] = '\0';
	//rd_str_length tylko do 8 bitow oraz nazwa pliku do 50, inaczej WTF
	rd_packet.type = (rd_str_length < 256 && size_of_filename < 51)? RD : WTF ;
	rd_packet.str_length = rd_str_length;
	return rd_packet;
}

struct ProtocolPacket ProtocolHandler::prepareERR(unsigned int err_number)
{
	struct ProtocolPacket err_packet;
	//err_number tylko do 8 bitow, inaczej WTF
	err_packet.type = (err_number < 512)? ERR : WTF ;
	err_packet.number = err_number;
	return err_packet;
}

unsigned int ProtocolHandler::interpretDatagramType(struct ProtocolPacket packet)
{
	return packet.type;
}

unsigned int ProtocolHandler::isDATA(struct ProtocolPacket packet)
{
	return packet.type == DATA;
}

unsigned int ProtocolHandler::isACK(struct ProtocolPacket packet)
{
	return packet.type == ACK;
}

unsigned int ProtocolHandler::isRQ(struct ProtocolPacket packet)
{
	return packet.type == RQ;
}

unsigned int ProtocolHandler::isRESP(struct ProtocolPacket packet)
{
	return packet.type == RESP;
}

unsigned int ProtocolHandler::isRD(struct ProtocolPacket packet)
{
	return packet.type == RD;
}

unsigned int ProtocolHandler::isERR(struct ProtocolPacket packet)
{
	return packet.type == ERR;
}


