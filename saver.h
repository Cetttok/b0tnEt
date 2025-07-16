#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#undef _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include <string>
#include <iostream>

class Host// класс стьруктура с методами парсинга и вывода
{
public:
    Host(std::string ip, int port);
    Host(std::string parseFrom); // парсит из строки вида <ip>:<port>
    Host();

    std::string ip() const;
    int port() const;

    std::string toString(); // возвращавет строку вида <ip>:<port>
private:
    std::string _ip = "0.0.0.0";
    int _port = 0 ;
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
    int countCommandIdInFile() const;
    int countHostInFile() const;
    void writeHostsToFile(Host *list, int size);
    int readHostsFromFile(Host *result, int size); //size должен быть = countHostInFile();
    void writeCommandsIdToFile(int* list, int size);
    int readCommandsIdFromFile(int* result,int size); // size должен быть = countCommandIdInFile();
private:
    static  Saver * _saver;
    Saver();// да я все таки переименовал этот класс
    ~Saver();

    void rewriteFile(std::string fileName, std::string data);
    void createFile(std::string fileName);

    int recountHostsInFile(); // методы вызываемые в конструкторе чтобы прочитать файл и перестчитать их
    int recountCommandIdInFile();

    int _countHostInFile; // эти параметры обновляются при записи в файл.
    int _countCommandIdInFile;
};

