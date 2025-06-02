#include "tcpserver.h"
const int MAX_CONNECTIONS =5;
const int BUFFER_SIZE = 64;
TcpServer::TcpServer(const int port):mServerAddress()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSocket<0){
        std::cout << "Server socket create error " << errno;
    }

    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(port);
    mServerAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(mServerSocket, (struct sockaddr*)&mServerAddress, sizeof(mServerAddress)) !=0){
        std::cout << "Server bin socket error. Maybe port used. errno" << errno;
    }
}

bool TcpServer::start()
{
    if (listen(mServerSocket,MAX_CONNECTIONS)==0){
        return true;
    }
    return false;

}

void TcpServer::nextPendingConnection()
{
    mClientSocket = accept(mServerSocket, nullptr, nullptr);
}

void TcpServer::closeLastConnection()
{
    close(mClientSocket);
}

void TcpServer::sendMessage(std::string message)
{
    send(mClientSocket, message.data(), strlen(message.data()), 0);
}

std::string TcpServer::readSocket()
{
    std::string result = "";
    char buffer[BUFFER_SIZE] = {0};
    while(recv(mClientSocket, buffer, sizeof(buffer), 0)>0){
        result = result + buffer;
        std::cout << result;
    };
    return result;
}
