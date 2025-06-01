//base
#include <iostream>
#include <cstring>
//for tcp connect
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
class TcpConnect
{
public:
    TcpConnect();
    bool connect(char* ipAddress,const int port);
    char* recieveAnswer(char* message);
    void sendMessage(char* message);
private:
    int mSocket = 0;
    sockaddr_in mServerAddress;

}
