#include "UI.h"
#include <iostream>
#include <string>

const std::string GET = "get";
const std::string CANCEL = "cancel";
const std::string SHOW = "show";
const std::string TRASFERINFO = "transferinf";


UI:: UI(){

}

void UI::parseCommand(std::string command){
	if(command.find(GET) != std::string::npos){

	}
	else if(command.find(CANCEL)!= std::string::npos){

	}
	else if(command.compare(SHOW) == 0){

	}
	else if(command.find(TRASFERINFO) != std::string::npos){

	}
	else{
		std::cout<<"Unresolved command"<<std::endl;
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
