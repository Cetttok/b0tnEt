#include <iostream>
#include "udppingoperator.h"
#include "iplistcollector.h"


int main() {
    UdpPingOperator pingator;
    IpListCollector ipcollector(&pingator);

    std::string addr;
    int port;

    std::cout<<ipcollector.get(&addr, &port)<<"\n______________\n";
    std::cout<< addr<<":"<<port<<"\n____________\n";

    std::string message("<host>123.45.98.76:7554</host>\n<host>225.226.253.111:8080</host>\n");


    std::cout<<ipcollector.get(message)<<"\n______________\n";

    message += "<host>103.40.90.6:7554</host>\n";

    std::cout<<ipcollector.get(message)<<"\n______________\n";


    std::cout<<ipcollector.get(&addr, &port)<<"\n______________\n";
    std::cout<< addr<<":"<<port<<"\n____________\n";

    ipcollector.check();
    std::cout<<"check()\n____________\n";

    std::cout<<ipcollector.get(&addr, &port)<<"\n______________\n";
    std::cout<< addr<<":"<<port<<"\n____________\n";

    return 0;
}
