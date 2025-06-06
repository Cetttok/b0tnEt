#ifndef UDPWORKER_H
#define UDPWORKER_H
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
/*
 простой класс, который позволяет отправлять udp пакеты по ip адресу и порту и при необзодимости читать ответы
пример работы (больше сервер наверное)
UdpWorker worker;
worker.start(4321);
std::cout << worker.listen() << std::endl;
worker.send("message", 1234, "127.0.0.1");
ВНИМАНИЕ!!
метод start(int port) вызывается для занятия порта под udp. Если нужно тольуо отправить сообщение, в слушать при этом необязательно,
то можно просто сразу вызвать send. Если потом придется слушать - start(int port) необходимо вызывать. При send порт занимать необязательно
ТАКЖЕ
чего будет если много пакетов придет не знаю.
*/
class UdpWorker
{
public:
    UdpWorker(); // создание сокета. Необходим для отправки и получеия
    bool start(int port); // привязка сокета к порту. Занятие порта. Нужен, чтоб потом можно было слушать. При одной только отправке необязателен
    // если порт занят (саме частое ) возвращает false. при успешной привящке true
    std::string listen(); // блокирующий вызов чтения. Похож на похожий метод в классе tcp. Ждет и потом читает пока есть дата в буфере
    bool send(std::string message, int port, std::string address = "none"); // отправка udp пакета по адресу и на порт. Порт сервера к этому отношения не имеет.
//Если не указать адрес, то будет отправлен на последний адрес с которого пришел пакет. При нормальной отправек возвращает true
private:
    uint32_t _lastListenedAddress = 0;
    int _socket = 0;
    sockaddr_in _serverAddress;
};

#endif // UDPWORKER_H
