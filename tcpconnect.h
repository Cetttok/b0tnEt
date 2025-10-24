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
 *      TcpConnect connector("192.168.0.13",1234);
        connector.fixConnection();
        connector.recieveAnswer("What you name?");
        connector.sendMessage("Halo from c++ ++++");
        connector.closeConnection();

*/
class TcpConnect
{
public:
    TcpConnect(std::string  ipAddress,const int port);// создание сокетв и сохранение в полях класса адреса и порта для подключения
    TcpConnect(u_int32_t ip,const int port);// числовой

    ~TcpConnect(); //закрыть сокет
    bool fixConnection();// подключение по указанному адресу и порту. при успешном подключении возвращает true
    std::string recieveAnswer(std::string message,int waitTime = 0);  //отправка сообщения и ожидание ответа. Время (ms) можно не ставить. Оно и не нужно, но при медленной реакции сервера пригодится
    void sendMessage(std::string message); // отправка сообщения без ожидания ответа
    void closeConnection(); // закрытие соединения
    std::string readSocket(); // чтение сокета (ожидание сообщения) без отправки сообщения

    static bool checkConnect(u_int32_t ip, int port, int tries);
private:
    int mSocket = 0;
    sockaddr_in mServerAddress;

};
#endif
