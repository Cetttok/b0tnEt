#include "udppingoperator.h"
#include <thread>

//const int PORT = 9563; // порт на котором будет работать сервер. Туда и обращаются другие
const int WAIT_TIME = 100000; // сколько ждать пакета перед повторной отправкой
const int CHECKS_ON_PACKET = 100; //сколько раз за WAIT_TIME проверять наличие пакета в буфере
const int TRY_COUNT = 3; // сколько попыток делать (сколько пакетов отправлять)
const int AT_CHECK_SERVER = 1000; // через сколько сервер проверяет свой буфер (время реакции сервера)
const std::string PING_MESSAGE = "udpPingResponse"; // сообщение запроса
const std::string PING_SERVER_MESSAGE = "udpPingAnswer"; // сообщение ответа сервера
const int MAX_UDP_PORT = 65536;
const int MAX_UDP_SYSTEM_PORT = 1024;
UdpPingOperator::UdpPingOperator(int port) :_udpForServer(), _udpForClient(){
    _udpForServer.start(port);
    srand(time(0));
    _udpForClient.start(rand()%(MAX_UDP_PORT-MAX_UDP_SYSTEM_PORT) + MAX_UDP_SYSTEM_PORT);
}

UdpPingOperator::~UdpPingOperator()
{
    if(_pingServerState){
        stopPingServer();
    }
}

bool UdpPingOperator::ping(std::string address, int port)
{
    for (int attemps = 0 ; attemps < TRY_COUNT; attemps++){
        _udpForClient.send(PING_MESSAGE,address,port);
        for (int checks = 0; checks<CHECKS_ON_PACKET; checks++){
            std::this_thread::sleep_for(std::chrono::microseconds(WAIT_TIME/CHECKS_ON_PACKET));
            if (_udpForClient.checkBuffer()){
                if(isAnswerPingDataValid(_udpForClient.listen())){
                    return true;
                }
            }
        }
    }
    return false;

}

void UdpPingOperator::startPingServer()
{
    if (_serverLoopThread == nullptr){
        _pingServerState = true;
         std::cout << "UdpPingOperator: Ping server started! "  << _udpForServer.getSocketPort()<< std::endl;
        _serverLoopThread = new std::thread(&UdpPingOperator::pingLoop, this);
    }
    else {
        std::cout << "UdpPingOperator::cant start. One already work!" << std::endl;
    }

}

void UdpPingOperator::stopPingServer()
{
    if (_serverLoopThread != nullptr){

        _pingServerState = false;
        _serverLoopThread->join(); // ждем завершение потока
        delete _serverLoopThread;
        _serverLoopThread = nullptr;
    }
    else{
        std::cout << "UdpPingOperator::cant stop please start first"<< std::endl;
    }
}

void UdpPingOperator::pingLoop()
{

    while(_pingServerState){
        std::this_thread::sleep_for(std::chrono::microseconds(AT_CHECK_SERVER));
        if (_udpForServer.checkBuffer()){
            //std::cout << "ping" << std::endl;
            if(isResponsePingDataValid(_udpForServer.listen())){
                _udpForServer.send(PING_SERVER_MESSAGE);

            }
        }
    }

}

bool UdpPingOperator::isAnswerPingDataValid(std::string pingData)
{
    //std::cout << pingData << std::endl;
    return pingData=="udpPingAnswer";
}

bool UdpPingOperator::isResponsePingDataValid(std::string pingData)
{
    //std::cout << pingData << std::endl;

    return pingData=="udpPingResponse";
}
