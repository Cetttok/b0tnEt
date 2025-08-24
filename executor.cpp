#include "executor.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>

// да тут много инклудов
const int REACTION_TIME = 100; // фиг знает можно убрать. Но пофиг
const int SHARED_MEMORY_NAME_SIZE = 20; // длина указателя на память. Не должно быть одинаковых
using namespace std;
Executor::Executor(int seed) {
    srand(seed);
}
Executor::~Executor()
{
    if (!_commandList.empty()){
        for (auto& pair : _commandList){
            stopExecuting(pair.first, true);
            if (_commandList.empty()){
                break;
            }
            continue;
        }
    }
    _commandList.clear();
}

void Executor::startExecute(std::string command, int id)
{
    if (_commandList.find(id) == _commandList.end()){
        // Command * newCommand =  new Command(command);
        // std::thread * newThread = new std::thread(&Command::execute, newCommand);
        pid_t c_pid = fork();

        if (c_pid > 0) {//parent
            //std::cout << "pid - " << c_pid << std::endl;
            _commandList.insert(std::make_pair(id , c_pid));

        }
        else{//child
            execlp("/bin/bash", "bash", "-c", command.c_str(), (char*)nullptr);
            return;
        }
    }
    else{
        std::cout << "Executor. Warn! Cant start command becouse it already executing" << std::endl;
    }
}
bool Executor::isDone(int id)
{
    if (_commandList.count(id)==1){
        //std::cout << "kill - " << kill(_commandList[id],0) << std::endl;
        return !(kill(_commandList[id],0)==0);
    }
    std::cout <<  "Executor. Warn! unknow command id -" << id << std::endl;
    return false;
}
int Executor::getFirstExecutedId()
{
    if (!_commandList.empty()){
        for (const auto& [id, command] : _commandList){

            if (isDone(id)){
                stopExecuting(id);
                return id;
            }
        }
        return 0;
    }
    else{
        return 0;
    }

}


bool Executor::stopExecuting(int id, bool isIterator)
{
    if (_commandList.count(id) == 1){

        // _commandList[id ]->stop();
        // _commandList[id]->mThread->join();
        kill(_commandList[id],9);
        if (!isIterator){
            //delete _commandList[id];
            _commandList.erase(id);
        }
        return true;
    }
    else{
        std::cout << "Executor:Cant stop commad. No command wuth this id" <<std::endl;
        return false;
    }
}



// Command::Command(std::string command):mCommand(command)
// {
//     _shmName = generateSharedMemoryName(); // индификатор
//     int shm_fd = shm_open(_shmName.data(), O_CREAT | O_RDWR, 0666);
//     ftruncate(shm_fd, sizeof(bool));
//     pState = (bool*)mmap(0, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // эм длинная комманды. Вообщем нужна она.
//     *pState = true;

// }

// void Command::execute()
// {
//     pid_t c_pid = fork();

//     if (c_pid > 0) {
//         while(*pState){
//             if (!(kill(c_pid, 0) == 0)){
//                 *pState= false;// на всякий случай. Он уже должным быть false

//             }

//             std::this_thread::sleep_for(std::chrono::microseconds(REACTION_TIME)); // можно убрать. Тут зависит нужен ли пустой бесконечный цикл
//         }

//         kill(c_pid,9);
//         wait(nullptr);
//         *pState = false; // на всякий случай. Он уже должным быть false
//     }
//     else if (c_pid == 0){
//         execlp("/bin/bash", "bash", "-c", mCommand.c_str(), (char*)nullptr);
//         // гениально чтобы запустить можно было
//         *pState= false;

//     }
//     *pState= false;
//     _completed.store(true, std::memory_order_release);

// }

// Command::~Command()
// {
//     munmap(pState, sizeof(bool));
//     shm_unlink(_shmName.data());
// }

// bool Command::isDone() const {
//     return !_completed.load(std::memory_order_acquire);
// }

// std::string Command::generateSharedMemoryName()
// {

//     std::string result = "/";
//     for (int i = 0; i < SHARED_MEMORY_NAME_SIZE; i++){
//         result+=char((rand()%25)+97);
//     }
//     return result;
// }

// ExecutingCommand::ExecutingCommand(Command *command, std::thread *thread):mCommand(command), mThread(thread)
// {

// }

// ExecutingCommand::ExecutingCommand()
// {
//     std::cout << "ExecutingCommand::warning null constructor used" << std::endl;
// }

// ExecutingCommand::~ExecutingCommand()
// {
//     delete mThread;
//     delete mCommand;
// }

// void ExecutingCommand::stop()
// {
//     *mCommand->pState = false;
// }
