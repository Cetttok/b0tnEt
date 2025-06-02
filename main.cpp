#include <iostream>
#include "tcpserver.h"
#include "tcpconnect.h"
using namespace std;

int main(int argc, char* argv[])
{
    cout << "Client/Server test app" <<argv[1] << std::endl;
    if (strcmp(argv[1],"server")==0){

        TcpServer server(1234);
        cout << "server created"<< std::endl;
        server.start();
        cout << "server started" << std::endl;
        server.nextPendingConnection();
        cout << "new connection";
        cout << server.readSocket() << std::endl;
        cout << "socket read succes";
        server.closeLastConnection();
        cout << "connection closed" <<  std::endl;
        return 0;
    }
    else if (strcmp(argv[1],"client") == 0){
        TcpConnect connector("192.168.0.13",1234);
        if (connector.fixConnection()){
            cout<< "connected" << std::endl;
        }
        else{
            cout << " cant fix connection! Error" << std::endl;
            return 100;
        }
        connector.sendMessage("Halo from c++ ++++");
        connector.closeConnection();
        return 0;
    }
    else{
        cout << "bad argument" << std::endl;
        cout << argv[1];
        return 2;
    }
}
