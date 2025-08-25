#include "globaltransporter.h"
#include <time.h>
int MICROSECONDS_ON_TIC = 10000;
int MICROSECONDS_IN_SECOND = 1000000;
int MAX_TICS_ON_UPDATE = 1000;
int ADD_TICS_ON_UPDATE = 300;
int MAX_PORT = 10000;

//for parsing
const char * UPDATE_RESPONSE_START_TAG = "updateRespone{";
const char * UPDATE_RESPONSE_END_TAG = "}updateRespone";
const char * HOST_LIST_START_TAG  = "hostList{";
const char * HOST_LIST_END_TAG  = "}hostList";
const char * COMMANDS_START_TAG  = "commands{";
const char * COMMANDS_END_TAG  = "}commands";
const char * HOST_START_TAG = "<host>";
const char * HOST_END_TAG = "</host>\n";
const char * HOST_DELIMER = ":";
const char * ONE_COMMAND_START_TAG = "<command>";
const char * ONE_COMMAND_END_TAG = "</command>";
const char * COMMAND_ID_START_TAG = "<id>";
const char * COMMAND_ID_END_TAG = "</id>";

//google dns for localIp
const char * DNS_IP = "8.8.8.8";
const int DNS_PORT = 53;
const int LOCAL_IP_GETTER_BUFFER_SIZE = 16;
const int UDP_PING_OPERATOR_PORT = 1234;

GlobalTransporter::GlobalTransporter(bool isDeleteForBadPing, bool isLocalNetwork):_isDeleteForBadPing(isDeleteForBadPing),_isLocalNetwork(isLocalNetwork)
{
    srand(time(0));
    _commands = new CommandTrasporter();
    _ping = new UdpPingOperator();
    _hostList = new IpListCollector(_ping);
    _ping->startPingServer();
    if (!isLocalNetwork){
        _upnp = new MyUPnP();
        _ipGetter = new StunIpGetter();
    }

}

GlobalTransporter::~GlobalTransporter()
{
    delete _commands;
    _ping->stopPingServer();
    delete _ping;
    delete _hostList;
    delete _upnp;
    delete _ipGetter;

}

void GlobalTransporter::eventLoop()
{
    srand(time(0));
    int waitTicsToUpdate = rand()%MAX_TICS_ON_UPDATE +ADD_TICS_ON_UPDATE ;
    //std::cout << "Wait: sec " << double(waitTicsToUpdate*10000)/1000000 << std::endl;
    Host newTarget;
    while(true){
        std::cout << "----------------------" << std::endl << "waiting host for update...." << std::endl;
        newTarget = _hostList->getNewTargetHost();
        std::cout << std::endl<<"DONE! next update for host -  "<<newTarget.toString() << " Wait: sec " << waitTicsToUpdate*MICROSECONDS_ON_TIC/MICROSECONDS_IN_SECOND  << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(waitTicsToUpdate*MICROSECONDS_ON_TIC));
        if (update(newTarget)){
            std::cout << "updated!"<<std::endl <<"----------------------"<<std::endl <<std::endl <<std::endl<<std::endl;
        }
        else{
            std::cout << "CANT CONNECT!!"<<std::endl <<"----------------------"<<std::endl <<std::endl <<std::endl<<std::endl;
        }

        waitTicsToUpdate = rand()%MAX_TICS_ON_UPDATE +ADD_TICS_ON_UPDATE ;
        save();

    }
}

void GlobalTransporter::init()
{
    int port = rand()%MAX_PORT;
    //_upnp->openPort(port);
    _commands->setPort(port);
    load();
}

void GlobalTransporter::load()
{
    int port = _commands->getPort();
    std::cout << "GloabalTransporter. START TCP SERVER ON HOST- " << getIp() << ":"    <<port<< std::endl;
    if (!_isLocalNetwork){
        _upnp->openPort(port, "TCP");
        _upnp->openPort(UDP_PING_OPERATOR_PORT, "UDP");
    }
    _server = new TcpServer(port);
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
    std::string result = UPDATE_RESPONSE_START_TAG;
    result += HOST_LIST_START_TAG;
    result+= _hostList->get("")
              +HOST_START_TAG+getIp()+HOST_DELIMER+std::to_string(_commands->getPort())+
        HOST_END_TAG;
    result += HOST_LIST_END_TAG;
    result += COMMANDS_START_TAG;
    result+= commandsToString(_commands->shareCommands());
    result+= COMMANDS_END_TAG;
    result += UPDATE_RESPONSE_END_TAG;
    return result;
}

std::string GlobalTransporter::commandsToString(std::map<int, std::string> * commands)
{
    if (!commands->empty()){
        std::string result;
        for (std::pair<const int, std::string> &pair : *commands) {
            result += ONE_COMMAND_START_TAG;
            result +=  COMMAND_ID_START_TAG + std::to_string(pair.first) + COMMAND_ID_END_TAG;
            result += pair.second + ONE_COMMAND_END_TAG;
        }
        return result;
    }
    return "";
}



bool GlobalTransporter::accept(std::string response)
{
    std::string forHostList;
    std::map<int, std::string> commands;
    if (response.find(UPDATE_RESPONSE_START_TAG) != response.npos){
        response = response.substr(std::string(UPDATE_RESPONSE_START_TAG).size(),response.find(UPDATE_RESPONSE_END_TAG)-std::string(UPDATE_RESPONSE_END_TAG).size());
        if (response.find(HOST_LIST_START_TAG) != response.npos && response.find(HOST_LIST_END_TAG) != response.npos ){

            forHostList = response.substr(std::string(HOST_LIST_START_TAG).size(), response.find(HOST_LIST_END_TAG)-std:: string(HOST_LIST_START_TAG).size());
            response =response.substr(forHostList.size() + std::string(HOST_LIST_END_TAG).size()*2, response.size()-1);
            if(forHostList.find(HOST_START_TAG) != forHostList.npos){
                //forHostList = forHostList.substr(forHostList.find(HOST_START_TAG),forHostList.size()-1);
                processHostList(forHostList);
            }
            else{
                forHostList = "";
            }

        }

        if (response.find(COMMANDS_START_TAG) != response.npos && response.find(COMMANDS_END_TAG) != response.npos ){
            std::string forCommands = response.substr(response.find(COMMANDS_START_TAG)+std::string(COMMANDS_START_TAG).size(), response.find(COMMANDS_END_TAG)-std::string(COMMANDS_END_TAG).size());
            //forCommands = response.substr(0, response.find(COMMANDS_END_TAG));
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
    if (_ip != ""){
        return _ip;
    }

    if (_isLocalNetwork)
    {
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
        serv.sin_addr.s_addr = inet_addr(DNS_IP );
        serv.sin_port = htons( DNS_PORT );
        int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
        struct sockaddr_in name;
        socklen_t namelen = sizeof(name);
        err = getsockname(sock, (struct sockaddr*) &name, &namelen);
        char buffer[LOCAL_IP_GETTER_BUFFER_SIZE ];
        const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer,LOCAL_IP_GETTER_BUFFER_SIZE );
        close(sock);
        _ip = std::string(buffer);
        return _ip;
    }
    else{
        _ip = _ipGetter->getStringIp();
        return _ip;
    }
}

std::map<int, std::string> GlobalTransporter::parseCommands(std::string from)
{
    std::map<int, std::string> result;
    if (from.find(ONE_COMMAND_START_TAG)== from.npos){
        return result;
    }

    while (true){
        std::string oneCommand= from.substr(std::string(ONE_COMMAND_START_TAG).size(),from.find(ONE_COMMAND_END_TAG)-std::string(ONE_COMMAND_END_TAG).size()+1);
        //std::cout << "one command "<<oneCommand << std::endl;
        int oneCommandAllSize = oneCommand.size() + std::string(ONE_COMMAND_END_TAG).size()*2;
        std::string forId= (oneCommand.substr(std::string(COMMAND_ID_START_TAG).size(), oneCommand.find(COMMAND_ID_END_TAG)-std::string(COMMAND_ID_END_TAG).size()+1));
        oneCommand = oneCommand.substr(oneCommand.find(COMMAND_ID_END_TAG)  + std::string(COMMAND_ID_END_TAG).size(), oneCommand.size()-1);
        //std::cout << "For id " << forId << std::endl;
        result[std::stoi(forId)] = oneCommand;
        from = from.substr(oneCommandAllSize-1, from.size());
        if (from.find(ONE_COMMAND_START_TAG)== from.npos){
            break;
        }
    }
    return result;
}

void GlobalTransporter::processHostList(std::string hosts)
{
    _hostList->get(hosts);
}
