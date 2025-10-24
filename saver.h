#pragma once
#include <map>
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#undef _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include <string>
#include <iostream>
#include <filesystem>
class Host// класс стьруктура с методами парсинга и вывода
{
public:
    Host(std::string ip, int mainPort, int pingPort);
    Host(std::string parseFrom); // парсит из строки вида <ip>:<port>
    Host();

    std::string ip() const;
    int mainPort() const;
    int pingPort() const;

    std::string toString(); // возвращавет строку вида <ip>:<port>
    u_int32_t to32Int();
    bool isNull();


    bool operator==(const Host& b);
private:
    std::string _ip = "0.0.0.0";
    int _mainPort = 0 ;
    int _pingPort = 0;
    bool _isNull = true;
};

//класс который позволяет хранить списки хостов и id команд на диске.
//даже читать их может
/*
как пример:
Saver * save = Saver::getSaver();
Host hosts[save.countHostInFile()];
save.readHostsFromFile(hosts,save.countHostInFile());

 */
class Saver {
public:
    static Saver *getSaver();// создание только через него

    int countHostInFile() const;
    void writeHostsToFile(Host *list, int size);
    int readHostsFromFile(Host *result, int size); //size должен быть = countHostInFile();

    int countCommandIdInFile() const;
    void writeCommandsIdToFile(int* list, int size);
    int readCommandsIdFromFile(int* result,int size); // size должен быть = countCommandIdInFile();

    std::map<int, std::string> loadCommands();
    bool clearCommandOnDisk(int id);
    void saveCommandOnDisk(std::string command, int id);

    int getMainPort();
    void setMainPort(int port);

    int getPingPort();
    void setPingPort(int port);


private:

    static  Saver * _saver;
    Saver();// да я все таки переименовал этот класс
    ~Saver();

    void rewriteFile(std::string fileName, std::string data);
    void createFile(std::string fileName);

    int recountHostsInFile(); // методы вызываемые в конструкторе чтобы прочитать файл и перестчитать их
    int recountCommandIdInFile();

    std::string getCommandById(int id);
    std::string genFileNameFromCommandId(int id);
    int parseCommandIdFromFileName(std::string filename);
    int _countHostInFile; // эти параметры обновляются при записи в файл.
    int _countCommandIdInFile;


    void mkdir();
};

