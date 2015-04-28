#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_
#include "NetworkHandler.h"

class Downloader : public NetworkHandler{
protected:
	bool bindSocket();
public:
	Downloader();
	~Downloader();
};

#endif
