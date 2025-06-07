#include "udppingoperator.h"
#include <iostream>
//на одном компьютере работать не будет
const int SERVER_WORK_TIME = 100000000; // вреия в микросекундах за которое не будут закрывать сервер
int main(int argc, char* argv[])
{
    std::cout << "Client/Server test udp app mode - " <<argv[1] << std::endl;
    if(strcmp(argv[1],"server")==0){
        UdpPingOperator pingServer;
        pingServer.startPingServer();
        std::this_thread::sleep_for(std::chrono::microseconds(SERVER_WORK_TIME)); //время которое будет работать сервер
        std::cout << "stop" << std::endl;
        pingServer.stopPingServer();
        return 0;
    }

    else if (strcmp(argv[1],"client")==0){

        UdpPingOperator pingClient;
        std::cout << pingClient.ping("192.168.0.100") << std::endl; // ip addres
        return 0;
    }
    else {
        std::cout << "invalid argument bad mode" << std::endl;
        return 100;
    }
    return 0;
    // UdpPingOperator ping;
    // ping.startPingServer();
    // std::cout<< ping.ping("127.0.0.1") <<" result " << std::endl; // 127.0.0.1 - localhost
    // ping.stopPingServer();
    // так тоже работает
}
