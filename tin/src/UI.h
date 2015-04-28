#ifndef UI_H_
#define UI_H_
#include <string>

class UI{
private:
	void parseCommand(std::string commandToParse);

public:
	UI();
	~UI();
	void waitForCommand();
	std::string command;
};

#endif
