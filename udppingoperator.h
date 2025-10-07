#ifndef UDPPINGOPERATOR_H
#define UDPPINGOPERATOR_H
#include <iostream>
#include "udpworker.h"
#include<unistd.h>
#include <thread>
/*
 класс который позволяет запустить пинг сервер или пропинговать другово на наличие пинг сервера.
в данный момент не завершен, тк нету структуры пакета. Все незавершенная функциональность в двух методах - проверка сообщения сервера и клиента
пример работы:
UdpPingOperator ping;
ping.startPingServer();
ping.ping(192.168.0.100);
ping.stopPingServer();
 */
class UdpPingOperator
{
public:
    UdpPingOperator(int port); //содание и занятие сокета
    ~UdpPingOperator(); // закрытие сервера. Рекомендуется закрывать до деструктора
    bool ping(std::string address, int port); // пинг. Есть очень много параметров(cpp файл). Может занять время
    void startPingServer(); // запуск пинг сервера в отдельном потоке
    void stopPingServer(); // осановка пинг сервера
    bool state(); //return _pingServerState ;)
private:
    void pingLoop(); // функция пинг сервера. Запускается в отдельном потоке в startPingServer
    bool isAnswerPingDataValid(std::string pingData); // надо добавить функциональность тут. Проверяет правильный ли ответ пинг сервера
    bool isResponsePingDataValid(std::string pingData); // проверяет правильный ли запрос пинг клиента
    UdpWorker _udpForServer;
    UdpWorker _udpForClient;
    std::thread * _serverLoopThread = nullptr;
    bool _pingServerState = false;
};

#endif // UDPPINGOPERATOR_H
