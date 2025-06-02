#include <iostream>
#include "tcpserver.h"
#include "tcpconnect.h"
#include <unistd.h>

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
        cout << "new connection" << std::endl;
        //sleep(2);
        //sleep(1);
        //cout << "message send" << std::endl;
        cout << "Message: " << server.readSocket() << std::endl;
        cout << "socket read succes"<< std::endl;
        server.sendMessage("Go back!");
        //server.closeLastConnection();
        cout << "connection closed" <<  std::endl;
        return 0;
    }
    else if (strcmp(argv[1],"client") == 0){
        TcpConnect connector("127.0.0.1",1234);
        if (connector.fixConnection()){
            cout<< "connected" << std::endl;
        }
        else{
            cout << " cant fix connection! Error" << std::endl;
            return 100;
        }
        std::cout <<"Answer: " <<  connector.recieveAnswer("Halo from c++ ++++") << std::endl;
        //sleep(3);
        //std::cout <<"Answer: " << connector.readSocket() <<std::endl;
        connector.closeConnection();

        return 0;
    }
    else{
        cout << "bad argument" << std::endl;
        cout << argv[1];
        return 2;
    }
}
