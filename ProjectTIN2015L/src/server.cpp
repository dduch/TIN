/*
 * server.cpp
 *
 *  Created on: 03-04-2015
 *      Author: dawid
 */

#include "server.h"

Server:: Server()
{
	this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("opening stream socket");
        exit(1);
    }
	nfds = sock+1;
}


