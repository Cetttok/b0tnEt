#include "tcpconnect.h"
const int BUFFER_SIZE = 64;
TcpConnect::TcpConnect(std::string ipAddress, const int port):mServerAddress()
{
    mSocket= socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket <= 0){
        std::cout << "Error while tcpconnect " << strerror(errno) << std::endl;
    }
    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(port);
    mServerAddress.sin_addr.s_addr = inet_addr(ipAddress.data());

}

bool TcpConnect::fixConnection()
{
    if (connect(mSocket, (struct sockaddr*)&mServerAddress, sizeof(mServerAddress))==0){
        std::cout << "Connection was fixed.";
        return true;
    }
    else{
        std::cout << "Connection not fixed! Unknow error - " << strerror(errno);
        return false;
    }

}

std::string TcpConnect::recieveAnswer(std::string message)
{
    sendMessage(message);
    // char answer[BUFFER_SIZE];
    std::string result = "";
    char buffer[BUFFER_SIZE] = {0};
    while(recv(mSocket, buffer, sizeof(buffer), 0)>0){
        result = result + buffer;
        std::cout << result;
    };
    return result;

}

void TcpConnect::sendMessage(std::string message)
{

    send(mSocket, message.data(), strlen(message.data()), 0);
}

void TcpConnect::closeConnection()
{
    close(mSocket);
}
