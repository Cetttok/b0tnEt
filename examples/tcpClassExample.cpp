#include <iostream>
#include "tcpserver.h"
#include "tcpconnect.h"
#include <unistd.h>

using namespace std;
const int PORT = 1234; // port for server
const char* SERVER_ADDRESS= "127.0.0.1";
int main(int argc, char* argv[])
{
    cout << "Client/Server test app" <<argv[1] << std::endl;
    if (strcmp(argv[1],"server")==0){

        TcpServer server(PORT);
        cout << "server created"<< std::endl;
        server.start();
        cout << "server started" << std::endl;
        server.nextPendingConnection();
        cout << "new connection. Start waiting messsage..." << std::endl;

        cout << "Message: " << server.readSocket() << std::endl;
        cout << "socket read succes. Sending answer.."<< std::endl;
        server.sendMessage("Go back!");
        server.closeLastConnection(); //client must close connection
        cout << "end" <<  std::endl;
        return 0;
    }
    else if (strcmp(argv[1],"client") == 0){
        TcpConnect connector(SERVER_ADDRESS,PORT);
        if (connector.fixConnection()){
            cout<< "connected" << std::endl;
        }
        else{
            cout << " cant fix connection! Error" << std::endl;
            return 404;
        }
        std::cout << "Sending response and wait answer..."<< std::endl;
        std::cout <<"Answer: " <<  connector.recieveAnswer("Halo from c++ ++++") << std::endl;
        std::cout << "closing socket.." << std::endl;
        connector.closeConnection();
        std::cout << "socket closed. end"<< std::endl;
        return 0;
    }
    else{
        cout << "bad argument" << std::endl;
        cout << argv[1];
        return 2;
    }
}
