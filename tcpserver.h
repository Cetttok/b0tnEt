#ifndef TCPSERVER_H
#define TCPSERVER_H
//base
#include <iostream>
#include <cstring>
#include <errno.h>

//for tcp connect
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
/*
класс реализующий функциональность tcp сервера
может работать только с одним подключением одновременно
имеет многое общее с TcpConnecor можно наверное потом сделать подкласс TcpMan который возьмет на себя общую функциональность
пример работы:
    TcpServer server(1234);
    start();
    nextPendingConnection();
    readSocket();
    sendMessage("TcpServer message");
    closeLastConnection();
 */
class TcpServer
{
public:
    TcpServer(const int port);
    ~TcpServer(); //закрываем все сокеты
    //создает сервер и занимет порт. При аварийном завершении порт и не разрыве подключения порт модет остаться занятым и класс сервера не будет работать.
    bool start(); // начинает слушать на порту. Новые подключения добавляет в очередь. Размер очереди можно изменить. Подключения выходящие за очередь отклоняются. При ошибке в запуске возвращат false
    void nextPendingConnection(); // принимает и создает сокет с самым первым подключением из очереди. Если чоередь пуста ждет подключения
    void closeLastConnection(); // закрывает последнее (единственное рабочее) соединение. Предпочтительно закрывать соединение будет клиент
    void sendMessage(std::string message); // отправляет сообщение
    std::string readSocket(); // читает или же ждет все сообщение
private:
    int mServerSocket;
    int mClientSocket;
    sockaddr_in mServerAddress;

};
#endif //TCPSERVER_H
