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
class TcpServer
{
public:
    TcpServer(const int port);
    bool start();
    void nextPendingConnection();
    void closeLastConnection();
    void sendMessage(std::string message);
    std::string readSocket();
private:
    int mServerSocket;
    int mClientSocket;
    sockaddr_in mServerAddress;

};
#endif //TCPSERVER_H
