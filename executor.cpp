#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>

const int REACTION_TIME = 100;
const int SHARED_MEMORY_NAME_SIZE = 20;
using namespace std;
Executor::Executor() {}

Executor::~Executor()
{
    if (!_commandList.empty()){
        for (const auto& [id, command] : _commandList){
            stopExecuting(id);
            if (_commandList.empty()){
                break;
            }
        }
    }
}

void Executor::startExecute(std::string command, int id)
{
    Command * newCommand =  new Command(command);
    std::cout << "funjny day" << endl;
    std::thread * newThread = new std::thread(&Command::execute, newCommand);
    _commandList.insert(std::make_pair(id , new ExecutingCommand(newCommand, newThread)));
    //_commandList.push_back(newCommand);
}

int Executor::getLastExecutedId()
{
    //std::this_thread::sleep_for(std::chrono::microseconds(REACTION_TIME));
    if (!_commandList.empty()){
        for (const auto& [id, command] : _commandList){

            if (!*command->mCommand->pState){
                stopExecuting(id);
                return id;
            }
        }
    }
    else{
        return -1;
    }
    return 0;
}


bool Executor::stopExecuting(int id)
{
    if (_commandList.count(id) == 1){

        _commandList[id ]->stop();
        _commandList[id]->mThread->join();
        delete _commandList[id];
        _commandList.erase(id);
        return true;
    }
    else{
        std::cout << "Executor:Cant stop commad. No command wuth this id" <<std::endl;
        return false;
    }
}



Command::Command(std::string command):mCommand(command)
{
    _shmName = generateSharedMemoryName(time(0));
    std::cout << _shmName << std::endl;
    // Создаем разделяемую память
    int shm_fd = shm_open(_shmName.data(), O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(bool)); // Устанавливаем размер разделяемой памяти

    // Отображаем разделяемую память в адресное пространство процесса
    pState = (bool*)mmap(0, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *pState = true;

}

void Command::execute()
{
    pid_t c_pid = fork();

    if (c_pid > 0) {
        //  wait(nullptr);
        // cout << "printed from parent process " << getpid()
        //      << endl;
        //system(command->mCommand.data());
        while(*pState){
            std::this_thread::sleep_for(std::chrono::microseconds(REACTION_TIME));
            if (!(kill(c_pid, 0) == 0)){
                *pState= false;
                std::cout <<  "enter"<<std::endl;
                return;
            }
        }
        system((std::string ("kill ") + std::to_string(c_pid)).data());
        wait(nullptr);
        *pState = false;
    }
    else if (c_pid == 0){
        system(mCommand.data());
        std::cout << "plkil" << *pState<< std::endl;
        *pState= false;
        std::cout << "plkil" << *pState<< std::endl;

    }
}

Command::~Command()
{
    munmap(pState, sizeof(bool));
    shm_unlink(_shmName.data());
}

string Command::generateSharedMemoryName(int seed)
{
    srand(seed);
    std::string result = "/";
    for (int i = 0; i < SHARED_MEMORY_NAME_SIZE; i++){
        result+=char((rand()%25)+97);
    }
    return result;
}

ExecutingCommand::ExecutingCommand(Command *command, std::thread *thread):mCommand(command), mThread(thread)
{

}

ExecutingCommand::ExecutingCommand()
{
    std::cout << "ExecutingCommand::warning null constructor used" << std::endl;
}

ExecutingCommand::~ExecutingCommand()
{
    delete mThread;
    delete mCommand;
}

void ExecutingCommand::stop()
{
    *mCommand->pState = false;
}
