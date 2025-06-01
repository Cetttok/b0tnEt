//base
#include <iostream>
#include <cstring>
//for tcp connect
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
class TcpServer
{
public:
    TcpServer();
    start(const int port);
    void nextPendingConnection();
    void sendMessage(char* message);
    char* readSocket();
private:
    int serverSocket;
    sockaddr_in inputConnectionAddress;
}
