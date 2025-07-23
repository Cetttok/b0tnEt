#ifndef IPLISTCOLLECTOR_IPLISTCOLLECTOR_H
#define IPLISTCOLLECTOR_IPLISTCOLLECTOR_H

#include <iostream>
#include <cstring>
//#include "commandtrasporter.h"
#include "udppingoperator.h"
#include "saver.h"

#define startTag "<host>"
#define endTag "</host>"
#define lenTag 6 //длина начального тега; end tag len = start tag len + 1

class IpListCollector{
public:
    IpListCollector(UdpPingOperator* pingOperator); // pingOperator нужен для работы метода check
    ~IpListCollector();

    std::string get(std::string *nextIp, int* nextPort); // для клиентской части
    std::string get(std::string ipList); // для серверной части

    // пример принимаемого списка новых хостов / пример возвращаемого списка хостов
    /*
    <host>123.45.67.89:1234</host>/n
    <host>234.45.67.89:8765</host>/n
     */

    int check();


    // result должен быть равен nullptr
    // парсит строку
    // возвращает количество хостов в result
    int parse(std::string ipList, Host* &result);

private:

    Host* _hosts= nullptr;

    Saver* _saver;
    UdpPingOperator* pinger;

    std::string list_host;
    int size = 0;
    int ptrToHost =0;

};


#endif //IPLISTCOLLECTOR_IPLISTCOLLECTOR_H
