#include "tcpserver.h"
const int MAX_CONNECTIONS =5;
const int BUFFER_SIZE = 256;
const int CHECK_BUFFER_SIZE = 16;
TcpServer::TcpServer(const int port):mServerAddress()
{
    mServerSocket = socket(AF_INET, SOCK_STREAM, 0); //создание сокета мало ли
    if (mServerSocket<0){
        std::cout << "TcpServer::Server socket create error - " << strerror(errno);
    }

    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(port);
    mServerAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(mServerSocket, (struct sockaddr*)&mServerAddress, sizeof(mServerAddress)) !=0){ // подключение к определенному порту. Если порт занят то все плохо
        std::cout << "TcpServer::Server bind socket error. Maybe port already used. Error - " << strerror(errno);
    }
}

TcpServer::~TcpServer()
{
    if (mClientSocket != 0){
        close(mClientSocket);
    }
    if (mServerSocket != 0){
        close(mServerSocket);
    }
}

bool TcpServer::start()
{
    if (listen(mServerSocket,MAX_CONNECTIONS)==0){ //запуск сервера и указание количества необработанных соединений
        return true;
    }
    std::cout << "TcpServer::Server start error - " << strerror(errno);
    return false;

}

void TcpServer::nextPendingConnection() // принять (подождать) одно соединение. Сейчас возможна работа только с одним клиентом
{
    mClientSocket = accept(mServerSocket, nullptr, nullptr);
}

void TcpServer::closeLastConnection() // закрыть последнее
{
    close(mClientSocket);
    mClientSocket =0; // 0 - состояние когда сокет закрыт, а <0 когда открыт с ошибкой
}

void TcpServer::sendMessage(std::string message)
{
    send(mClientSocket, message.data(), strlen(message.data()), 0);
}

std::string TcpServer::readSocket() // подождать и прочитать всю дату из сокета
{
    std::string result = "";
    char buffer[BUFFER_SIZE] = {0};
    char checkBuffer[CHECK_BUFFER_SIZE] = {0};
    do{ // подождать дату и прочитать буфером
        int bytesRecived = recv(mClientSocket, buffer, sizeof(buffer), 0);
        result.append(buffer,bytesRecived);
    }
    while(recv(mClientSocket, checkBuffer , CHECK_BUFFER_SIZE, MSG_PEEK | MSG_DONTWAIT)>0); // продолжить если в сокете еще что то есть
    return result;
}
