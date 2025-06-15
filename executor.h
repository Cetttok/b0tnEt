#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <atomic>
#include <list>
#include <map>
#include <string>
#include <thread>

class Command{
public:
    Command(std::string command);
    void execute();
    ~Command();
    std::string mCommand;
    bool  * pState = nullptr;
private:
    std::string generateSharedMemoryName(int seed);
    std::string _shmName;
};
class ExecutingCommand
{
public:
    ExecutingCommand(Command * command, std::thread * thread);
    ExecutingCommand();

    ~ExecutingCommand();
    Command * mCommand;
    std::thread * mThread;
    void stop();
};

class Executor
{
public:
    Executor();
    ~Executor();
    void startExecute(std::string command, int id);
    int getLastExecutedId();
    bool stopExecuting(int id);
    //void execute(Command *command);
private:
    std::map< int , ExecutingCommand * > _commandList;
};

#endif // EXECUTOR_H
