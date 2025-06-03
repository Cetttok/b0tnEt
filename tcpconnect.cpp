#include "tcpconnect.h"
const int BUFFER_SIZE = 256;
TcpConnect::TcpConnect(std::string ipAddress, const int port):mServerAddress()
{
    mSocket= socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket <= 0){
        std::cout << "TcpConnect::SocketCreationError - " << strerror(errno) << std::endl;
    }
    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(port);
    mServerAddress.sin_addr.s_addr = inet_addr(ipAddress.data());

}

TcpConnect::~TcpConnect()
{
    closeConnection();
}

bool TcpConnect::fixConnection()
{
    if (connect(mSocket, (struct sockaddr*)&mServerAddress, sizeof(mServerAddress))==0){ // если подключение прошло умпешно
        //std::cout << "TcpConnect::Connection was fixed." << std::endl; //убрал чтобы не засорять лог
        return true;
    }
    else{
        std::cout << "TcpConnect::Connection not fixed! error - " << strerror(errno) << std::endl;
        return false;
    }

}

std::string TcpConnect::recieveAnswer(std::string message)
{
    sendMessage(message); // отправить запрос
    //sleep(1);
    // char answer[BUFFER_SIZE];
    return readSocket(); //ждать и прочитать ответ

}

void TcpConnect::sendMessage(std::string message)
{

    send(mSocket, message.data(), strlen(message.data()), 0);
}

void TcpConnect::closeConnection()
{
    if (mSocket!=0){
        close(mSocket);
    }// желательно закрывать. с стороны клиента желательно наверное
}

std::string TcpConnect::readSocket()
{
    std::string result = "";
    char buffer[BUFFER_SIZE] = {0};

    do{ // подождать дату и прочитать буфером
        int bytesRecived = recv(mSocket, buffer, sizeof(buffer), 0);
        result.append(buffer,bytesRecived);
    }
    while(recv(mSocket, nullptr, sizeof(nullptr), MSG_PEEK | MSG_DONTWAIT)>0); // продолжить если в сокете еще что то есть (проверка на наличие данных)
    return result;
}
