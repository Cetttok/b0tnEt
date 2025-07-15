#include "tcpconnect.h"
#include <thread>
const int BUFFER_SIZE = 512;
const int CHECK_BUFFER_SIZE = 2;
const int WAIT_BEFORE_CHECK_BUFFER = 3000; //в одной микросекунде 1000 милисекунд ms. микросекунд 3 ms. Для локальной сети 0.25. Для другово соединения сильно завистит от сайта. 10 примерно
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

std::string TcpConnect::recieveAnswer(std::string message, int waitTime)
{
    sendMessage(message); // отправить запрос
    //sleep(1);
    // char answer[BUFFER_SIZE];
    if (waitTime != 0){
        std::this_thread::sleep_for(std::chrono::microseconds(waitTime));//альернативная задердка. Старая версия
    }
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
        mSocket=0;
    }// желательно закрывать. с стороны клиента желательно наверное
}

std::string TcpConnect::readSocket()
{
    std::string result = "";
    char buffer[BUFFER_SIZE] = {0};
    char checkBuffer[CHECK_BUFFER_SIZE] = {0};
    do{ // подождать дату и прочитать буфером
        int bytesRecived = recv(mSocket, buffer, sizeof(buffer), 0);
        result.append(buffer,bytesRecived);
        std::this_thread::sleep_for(std::chrono::microseconds(WAIT_BEFORE_CHECK_BUFFER)); // чтобы читать всю инфорамцию.
    }
    while(recv(mSocket, checkBuffer, CHECK_BUFFER_SIZE, MSG_PEEK | MSG_DONTWAIT)>0); // продолжить если в сокете еще что то есть (проверка на наличие данных)
    return result;
}
