#include <iostream>
#include "udpworker.h"
using namespace std;
const int SERVER_PORT = 1234;
const int CLIENT_PORT = 4321;
int main(int argc, char* argv[])
{
    cout << "Client/Server test udp app mode - " <<argv[1] << std::endl;
    if (strcmp(argv[1],"server")==0){
        UdpWorker server;
        std::cout <<"server created" << std::endl;
        server.start(SERVER_PORT);
        std::cout <<"server started. Start listening..." << std::endl;
        std::cout  << "Listened: " << server.listen() << std::endl;
        server.send("server answer",CLIENT_PORT);
        std::cout <<"message sended. end" << std::endl;
        return 0;
    }
    else if (strcmp(argv[1],"client")==0){
        UdpWorker client;
        std::cout <<"client created" << std::endl;
        client.start(CLIENT_PORT);
        std::cout <<"client started" << std::endl;
        client.send("client message",SERVER_PORT,"127.0.0.1");
        std::cout << "message sended. Start listening..."<<std::endl;
        std::cout <<"Listned: "<< client.listen() << std::endl;
        std::cout <<"end" << std::endl;
        }
    else {
        std::cout << "invalid argument bad mode" << std::endl;
            return 100;
        }
    return 0;
}
