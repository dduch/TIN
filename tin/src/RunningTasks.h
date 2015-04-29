#ifndef RUNNINGTASKS_H_
#define RUNNINGTASKS_H_

class RunningTasks
{
private:
	RunningTasks();
	~RunningTasks();
	static RunningTasks* instance;
public:
	static RunningTasks* getIstance();
};

#endif
