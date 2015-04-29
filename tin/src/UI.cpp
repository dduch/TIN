#include "UI.h"
#include <iostream>
#include <string>
#include "Downloader.h"

const std::string GET = "get";
const std::string CANCEL = "cancel";
const std::string SHOW = "show";
const std::string TRASFERINFO = "transferinf";
const std::string HELP = "help";
const std::string HELP_INFO = "Possible commands: get, cancel, show, tranferinf\n";

UI:: UI(){

}

void UI::parseCommand(std::string command){
	if(command.find(GET) != std::string::npos){
		char *req = &command[0u];
		new Downloader(req);
	}
	else if(command.find(CANCEL)!= std::string::npos){

	}
	else if(command.compare(SHOW) == 0){

	}
	else if(command.find(TRASFERINFO) != std::string::npos){

	}
	else if(command.find(HELP) != std::string::npos){
		std::cout<<HELP_INFO<<std::endl;
	}
	else{
		std::cout<<"Unresolved command, use help"<<std::endl;
		return;
	}
}

void UI::waitForCommand(){
	while(1)
	{
		std::cout<<"$";
		std::getline(std::cin, command);
		if(!command.empty())
		{
			this->parseCommand(command);
		}
	}
}
