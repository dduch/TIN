/*
 * server.h
 *
 *  Created on: 03-04-2015
 *      Author: dawid
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

#ifndef SERVER_H_
#define SERVER_H_

#define TRUE 1
#define READ_SIZE 20

class Server
{
	private:
		int sock, length;
		struct sockaddr_in server;
		fd_set ready;
		struct timeval to;
		int msgsock;
		int nfds;
		int nactive;
		char buf[1024];
		int rval;
		int i;

	public:
		Server();
};


#endif /* SERVER_H_ */
