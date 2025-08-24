#ifndef COMMANDTRASPORTER_H
#define COMMANDTRASPORTER_H
#include "executor.h"
#include "saver.h"
#include <iostream>
#include <string>
#include <map>


class CommandTrasporter
{
public:
    CommandTrasporter(); //начать выполнение всех известных не выполненных комманд
    ~CommandTrasporter(); //сохранение и проверка выполнения

    bool addCommand(int id, std::string command); // взять в выполнение комнаду
    void update(); //отправить все невыполняющиеся комманды в выполнение
    std::map<int, std::string> *  shareCommands(); // команды для отправки другим устройствам
    void saveOnDrive(); // сохранить текущее состояние на диск без обновление. Жедательно вызвать перед этим update
    int getPort();
    void setPort(int port);
    void clearExecutedCommand(); // обновить и проверить состояние комманд отредачить _executedId

private:

    void sendCommandsToExecutor();  //тоже что и update незнаю почему я жто сделал???
    void addExecutedId(int id); // эмм как бы добавляет в конец последнюю команду первая уходит (при забитом спике) или просто добоавляет в конец массива
    bool isAlreadyExecuted(int id); // есть ли комманда в списке выполненных?

    std::map<int, std::string> _commands; // map<id, текст команды>
    int * _executedId; // массив id выполненных комманд

    Executor _executor;
    Saver *  _saver;
};

#endif // COMMANDTRASPORTER_H
