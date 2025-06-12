#include "udpworker.h"
const int CHECK_BUFFER_SIZE = 16;
const int BUFFER_SIZE = 256;

UdpWorker::UdpWorker():_serverAddress()
{
    _socket = socket(AF_INET, SOCK_DGRAM, 0);
    if ( _socket <= 0) {
        std::cout << "UdpWorker::socket create error. error - " << strerror(errno) << std::endl;
    }
}

bool UdpWorker::start(int port)
{
    _serverAddress.sin_family    = AF_INET; // IPv4
    _serverAddress.sin_addr.s_addr = INADDR_ANY;
    _serverAddress.sin_port = htons(port);
    if ( bind(_socket, (const struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) < 0 )
    {
        std::cout<< "UdpWorker::bind server failed. error - " << strerror(errno) << std::endl;
        return false;
    }
    else {
        return true;
    }

}

std::string UdpWorker::listen()
{

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    //socklen_t len  = sizeof(*_lastListenedAddress);
    std::string result = "";
    char buffer[BUFFER_SIZE] = {0};
    do{ // подождать дату и прочитать буфером
        int bytesRecived = recvfrom(_socket, (char *)buffer, BUFFER_SIZE,
                                    0, (struct sockaddr *)&addr, &len);
        result.append(buffer,bytesRecived);
    }
    while(recvfrom(_socket, nullptr, sizeof(nullptr),
                      MSG_DONTWAIT|MSG_PEEK, nullptr,
                      0)>0); // продолжить если в сокете еще что то есть (проверка на наличие данных)
    _lastListenedAddress = addr.sin_addr.s_addr;
    return result;
}

bool UdpWorker::send(std::string message, int port,std::string address)
{
    struct sockaddr *addr;
    uint32_t s_addr;
    if (address == "none"){
        if (_lastListenedAddress != 0){

            s_addr = _lastListenedAddress;
        }
        else{
            std::cout << "UdpWorker::cant send. Please listen first or define address" << std::endl;
            return false;
        }
    }
    else{
        s_addr = inet_addr(address.data());
    }
    sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = s_addr;
    addr = (struct sockaddr *)&addr_in;
    if (sendto(_socket, message.data(), message.length(),
               MSG_CONFIRM, addr,
               sizeof(*addr))>0){
        return true;
    }
    return false;

}

bool UdpWorker::checkBuffer()
{
    char buffer[CHECK_BUFFER_SIZE ] = {0};
    if (recvfrom(_socket, buffer, CHECK_BUFFER_SIZE,
                 MSG_DONTWAIT|MSG_PEEK, nullptr,
                 0)>0){
        return true;
    }
    return false;
}
