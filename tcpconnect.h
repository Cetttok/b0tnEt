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
class TcpConnect
{
public:
    TcpConnect(std::string  ipAddress,const int port);
    bool fixConnection();
    std::string recieveAnswer(std::string message);
    void sendMessage(std::string message);
    void closeConnection();
private:
    int mSocket = 0;
    sockaddr_in mServerAddress;

};
#endif
