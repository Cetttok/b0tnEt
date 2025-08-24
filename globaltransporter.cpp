#include "globaltransporter.h"
#include <time.h>
int MICROSECONDS_ON_TIC = 10000;
int MICROSECONDS_IN_SECOND = 1000000;
int MAX_TICS_ON_UPDATE = 1000;
int ADD_TICS_ON_UPDATE = 300;
GlobalTransporter::GlobalTransporter() {
    srand(time(0));
    _commands = new CommandTrasporter();
    _ping = new UdpPingOperator();
    _hostList = new IpListCollector(_ping);
    _ping->startPingServer();

}

GlobalTransporter::~GlobalTransporter()
{
    delete _commands;
    _ping->stopPingServer();
    delete _ping;
    delete _hostList;

}

void GlobalTransporter::eventLoop()
{
    srand(time(0));
    int waitTicsToUpdate = rand()%MAX_TICS_ON_UPDATE +ADD_TICS_ON_UPDATE ;
    //std::cout << "Wait: sec " << double(waitTicsToUpdate*10000)/1000000 << std::endl;
    Host newTarget;
    while(true){
        newTarget = _hostList->getNewTargetHost();
        std::cout << "----------------------"<<std::endl<<"next update for host -  "<<newTarget.toString() << " Wait: sec " << waitTicsToUpdate*MICROSECONDS_ON_TIC/MICROSECONDS_IN_SECOND  << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(waitTicsToUpdate*MICROSECONDS_ON_TIC));
        if (update(newTarget)){
            std::cout << "updated!"<<std::endl <<"----------------------"<<std::endl <<std::endl <<std::endl<<std::endl;
        }
        else{
            std::cout << "CANT CONNECT!!"<<std::endl <<"----------------------"<<std::endl <<std::endl <<std::endl<<std::endl;
        }
            // if (_server->nextPendingConnection(true)){
        //     std::cout << "CONNECTED!" << std::endl;
        //     std::string resp = _server->readSocket();
        //     accept(resp);
        //     std::cout << "READ: " << resp << std::endl;
        //     _server->sendMessage(genAnswer());
        //     std::cout << "SEND: " << genAnswer() << std::endl;
        //     _server->closeLastConnection();
        // }
        // std::cout << "TICS AMOUNT: " << waitTicsToUpdate <<std::endl;
        // waitTicsToUpdate--;
        // if (waitTicsToUpdate == 0){
        //     // std::string *ip;
        //     // int * port;
        //     // _hostList->get(ip,port);
        //     // std::cout << ip << ":" << port << std::endl;
        //     update(_hostList->getNewTargetHost());

        waitTicsToUpdate = rand()%MAX_TICS_ON_UPDATE +ADD_TICS_ON_UPDATE ;
        save();
        // }
    }
}

void GlobalTransporter::init()
{
    int port = rand()%10000;
    _server = new TcpServer(port);
    _commands->setPort(port);
}

void GlobalTransporter::load()
{
    std::cout << "GloabalTransporter. START TCP SERVER ON HOST- " << getIp() << ":"    <<_commands->getPort() << std::endl;

    _server = new TcpServer(_commands->getPort());
}

void GlobalTransporter::save()
{
    _commands->clearExecutedCommand();
    _commands->saveOnDrive();
    _hostList->check();

}

void GlobalTransporter::startListening()
{
    _server->start();
    while(true){
        if (_server->nextPendingConnection(true)){
            //std::cout << "CONNECTED!" << std::endl;
            std::string resp = _server->readSocket();
            //std::cout << "READ: " << resp << std::endl;
            accept(resp);
            _server->sendMessage(genAnswer());
            //std::cout << "SEND: " << genAnswer() << std::endl;
            _server->closeLastConnection();
        }
        save();
    }
}

bool GlobalTransporter::update(Host host)
{
    _connect = new TcpConnect(host.ip(), host.port());
    if (_connect->fixConnection()){
        //_connect->sendMessage(genAnswer());
        _connect->sendMessage(genAnswer());
        //std::cout << "SEND: " << genAnswer() << std::endl;
        std::string answ = _connect->readSocket();
        accept(answ);

        //std::cout << "READ: " <<  answ << std::endl;
        _connect->closeConnection();
        delete _connect;
        return true;
    }
    else{
        std::cout << "GlobalTransporter. Cant connect to host " << host.toString() << std::endl;
        delete _connect;
        return false;
    }
}

std::string GlobalTransporter::genAnswer()
{
    std::string result = "updateResponse{";
    result += "hostList{";
    result+= _hostList->get("")
              +"<host>"+getIp()+":"+std::to_string(_commands->getPort())+
        "</host>\n";
    result += "}hostList";
    result += "commands{";
    result+= commandsToString(_commands->shareCommands());
    result+= "}commands";
    result += "}updateResponse";
    return result;
}

std::string GlobalTransporter::commandsToString(std::map<int, std::string> * commands)
{
    if (!commands->empty()){
        std::string result;
        for (std::pair<const int, std::string> &pair : *commands) {
            result += "<command>";
            result +=  "<id>" + std::to_string(pair.first) + "</id>";
            result += pair.second + "</command>";
        }
        return result;
    }
    return "";
}



bool GlobalTransporter::accept(std::string response)
{
    std::string forHostList;
    std::map<int, std::string> commands;
    if (response.find("updateResponse") != response.npos){
        response = response.substr(std::string("updateResponse{").size(),response.find("}updateResponse")-std::string("}updateResponse").size());
        if (response.find("hostList{") != response.npos && response.find("}hostList") != response.npos ){

            forHostList = response.substr(std::string("hostList{").size(), response.find("}hostList")-std:: string("hostList{").size());
            response =response.substr(forHostList.size() + std::string("}hostList").size()*2, response.size()-1);
            if(forHostList.find("<host>") != forHostList.npos){
                //forHostList = forHostList.substr(forHostList.find("<host>"),forHostList.size()-1);
                processHostList(forHostList);
            }
            else{
                forHostList = "";
            }

        }

        if (response.find("commands{") != response.npos && response.find("}commands") != response.npos ){
            std::string forCommands = response.substr(response.find("commands{")+std::string("commands{").size(), response.find("}commands")-std::string("}commands").size());
            //forCommands = response.substr(0, response.find("}commands"));
            commands = parseCommands(forCommands);
            processCommands(commands);

        }
        return true;

    }
    return false;

}

void GlobalTransporter::processCommands(std::map<int, std::string> commands)
{
    for (std::pair<const int, std::string> &command : commands) {
        _commands->addCommand(command.first, command.second);
    }
}
std::string GlobalTransporter::getBadAnswer()
{
    return std::string("badResponse");
}

std::string GlobalTransporter::getIp()
{
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;

    struct sockaddr_in serv;

    int sock = socket ( AF_INET, SOCK_DGRAM, 0);

    //Socket could not be created
    if(sock < 0)
    {
        std::cout << "GlobalTransporter! Cant get localIp." << std::endl;
        return "0.0.0.0";
    }

    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( google_dns_server );
    serv.sin_port = htons( dns_port );
    int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);
    char buffer[16];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 16);
    close(sock);
    return std::string(buffer);
}

std::map<int, std::string> GlobalTransporter::parseCommands(std::string from)
{
    std::map<int, std::string> result;
    if (from.find("<command>")== from.npos){
        return result;
    }

    while (true){
        std::string oneCommand= from.substr(std::string("<command>").size(),from.find("</command>")-std::string("</command>").size()+1);
        //std::cout << "one command "<<oneCommand << std::endl;
        int oneCommandAllSize = oneCommand.size() + std::string("</command>").size()*2;
        std::string forId= (oneCommand.substr(std::string("<id>").size(), oneCommand.find("</id>")-std::string("</id>").size()+1));
        oneCommand = oneCommand.substr(oneCommand.find("</id>")  + std::string("</id>").size(), oneCommand.size()-1);
        //std::cout << "For id " << forId << std::endl;
        result[std::stoi(forId)] = oneCommand;
        from = from.substr(oneCommandAllSize-1, from.size());
        if (from.find("<command>")== from.npos){
            break;
        }
    }
    return result;
}

void GlobalTransporter::processHostList(std::string hosts)
{
    _hostList->get(hosts);
}
