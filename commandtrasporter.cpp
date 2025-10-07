#include "commandtrasporter.h"
#include <ctime>
#include <signal.h>
const int LAST_LIST_ID_COMMANDS_SIZE = 50;



CommandTrasporter::CommandTrasporter():_executedId(new int[LAST_LIST_ID_COMMANDS_SIZE]),_executor(time(NULL)),_saver(Saver::getSaver())
{
    _crypter = new Crypter();
    signal(SIGCHLD, SIG_IGN);
    for (int i = 0 ; i < LAST_LIST_ID_COMMANDS_SIZE; i++){
        _executedId[i] = 0;
    }
    _commands = _saver->loadCommands();
    if (_saver->countCommandIdInFile() < LAST_LIST_ID_COMMANDS_SIZE){
        _saver->readCommandsIdFromFile(_executedId,_saver->countCommandIdInFile());
    }
    else{
        _saver->readCommandsIdFromFile(_executedId, LAST_LIST_ID_COMMANDS_SIZE);
    }
    sendCommandsToExecutor();


}

CommandTrasporter::~CommandTrasporter()
{
    clearExecutedCommand();
    saveOnDrive();
    delete[] _executedId;
}

bool CommandTrasporter::addCommand(int id, std::string command)
{

    //std::cout << "cmd: " << command << std::endl;
    if (_crypter->checkValidity(command)){
        if (_commands.find(id) == _commands.end()){
            if (!isAlreadyExecuted(id)){
                _commands[id] = command;
                _executor.startExecute(_crypter->decrypt(command),id);
                return true;
            }
            else{
                //std::cout << "CommandTransporter. Warn! Cant add command. Is already been executed!" << std::endl;
                return false;
            }
        }
        else{
            //std::cout<< "CommandTranporter. Warn! Cant add command. Already in added or loaded from drive..." << std::endl;
            return false;
        }
    }
    else{
        std::cout << "CommandTransporter. checkValidity FAILED!" << std::endl;
        return false;
    }
}

void CommandTrasporter::update()
{
    //clearExecutedCommand();
    sendCommandsToExecutor();
}

std::map<int, std::string> *CommandTrasporter::shareCommands()
{
    return &_commands;
}

void CommandTrasporter::saveOnDrive()
{
    for (const auto & [id,command] : _commands){
        _saver->saveCommandOnDisk(command,id);
    }
    _saver->writeCommandsIdToFile(_executedId, LAST_LIST_ID_COMMANDS_SIZE);
}

int CommandTrasporter::getMainPort()
{
    return _saver->getMainPort();
}

void CommandTrasporter::setMainPort(int port)
{
    _saver->setMainPort(port);
}


int CommandTrasporter::getPingPort()
{
    return _saver->getPingPort();
}

void CommandTrasporter::setPingPort(int port)
{
    _saver->setPingPort(port);
}

void CommandTrasporter::sendCommandsToExecutor()
{
    if (!_commands.empty()){
        for (const auto& [id, command] : _commands){
            if (_crypter->checkValidity(command)){
                if (!isAlreadyExecuted(id))
                {
                    _executor.startExecute(_crypter->decrypt(command),id);
                }
            }
        }
    }
    clearExecutedCommand();
}
void CommandTrasporter::clearExecutedCommand()
{
    int firstExecutedId = _executor.getFirstExecutedId();
    while(firstExecutedId != 0){
        // for (std::map<int, std::string>::iterator iterator = _commands.begin();
        //     iterator!= _commands.end(); ++iterator){
        //     if (iterator->first == firstExecutedId){
                addExecutedId(firstExecutedId);
        //         continue;
        //     }
        // }
        firstExecutedId = _executor.getFirstExecutedId();
    }






}

void CommandTrasporter::addExecutedId(int id)
{
    int lastIdNum = 0;
    for (; lastIdNum < LAST_LIST_ID_COMMANDS_SIZE; lastIdNum++){
        if (_executedId[lastIdNum ] == 0){
            break;
        }
        if (_executedId[lastIdNum]  == id){
            return;
        }
    }
    if (lastIdNum != (LAST_LIST_ID_COMMANDS_SIZE - 1)){
        _executedId[lastIdNum] = id;
    }
    else{
        for (int i = 0; i < LAST_LIST_ID_COMMANDS_SIZE; i++){
            if (i != LAST_LIST_ID_COMMANDS_SIZE - 2){
                _executedId[i] = _executedId[i+1];
            }
            else{
                _executedId[i] = id;
            }
        }
    }
}

bool CommandTrasporter::isAlreadyExecuted(int id)
{
    for (int i = 0 ; i < LAST_LIST_ID_COMMANDS_SIZE; i++){
        if (_executedId[i] == id){
            return true;
        }
    }
    return false;
}
