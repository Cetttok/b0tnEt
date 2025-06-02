#ifndef TCPCONNECT_H
#define TCPCONNECT_H
//base
#include <iostream>
#include <cstring>
//for tcp connect
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
/* класс который занимается tcp подключениями
 * позволяет отправить по нужному адресу и порту std::string и получить ответ
 * пример работы:
 *      TcpConnect connector("192.168.0.13",1234); // создание сокетв и сохранение в полях класса адреса и порта для подключения
        connector.fixConnection(); // подключение по указанному адресу и порту. при успешном подключении возвращает true
        connector.recieveAnswer("What you name?"); //отправка сообщения и ожидание ответа
        connector.sendMessage("Halo from c++ ++++"); // отправка сообщения без ожидания ответа
        connector.closeConnection(); // закрытие соединения

*/
class TcpConnect
{
public:
    TcpConnect(std::string  ipAddress,const int port);
    bool fixConnection();
    std::string recieveAnswer(std::string message);
    void sendMessage(std::string message);
    void closeConnection();
    std::string readSocket();
private:
    int mSocket = 0;
    sockaddr_in mServerAddress;

};
#endif
