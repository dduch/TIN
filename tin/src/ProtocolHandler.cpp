#include "ProtocolHandler.h"
#include<iostream>
#include<string.h>

ProtocolHandler:: ProtocolHandler(){}

ProtocolHandler:: ~ProtocolHandler(){}

struct ProtocolPacket;

struct ProtocolPacket ProtocolHandler::prepareDatagramACK(unsigned int ack_number)
{
	struct ProtocolPacket ack_packet;
	//ack_number tylko do 20 bitow, inaczej WTF
	ack_packet.type = (ack_number < 1048576)? ACK : WTF ;
	ack_packet.number = ack_number;
	return ack_packet;
}

struct ProtocolPacket ProtocolHandler::prepareDatagramRQ(unsigned int rq_str_length, const char* filename, unsigned int size_of_filename)
{
	struct ProtocolPacket rq_packet;
	rq_packet.filename = new char[size_of_filename];
	strncpy(rq_packet.filename, filename, size_of_filename);
	rq_packet.filename[size_of_filename - 1] = '\0';
	//rq_str_length tylko do 8 bitow, inaczej WTF
	rq_packet.type = (rq_str_length < 256)? RQ : WTF ;
	rq_packet.str_length = rq_str_length;
	return rq_packet;
}

