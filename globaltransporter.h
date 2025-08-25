#ifndef GLOBALTRANSPORTER_H
#define GLOBALTRANSPORTER_H
#include "commandtrasporter.h"
#include "iplistcollector.h"
#include "my_upnp.h"
#include "stunipgetter.h"
#include "tcpconnect.h"
#include "tcpserver.h"

class GlobalTransporter
{
public:
    GlobalTransporter(bool isDeleteForBadPing = true, bool isLocalNetwork = false);
    ~GlobalTransporter();
    void startListening();
    void eventLoop();

    void init();
    void load();
    void save();
    bool accept(std::string response);
    std::string genAnswer();
private:
    const bool _isDeleteForBadPing;
    const bool _isLocalNetwork;
    CommandTrasporter * _commands = nullptr;
    IpListCollector * _hostList = nullptr;
    TcpServer * _server = nullptr;
    TcpConnect * _connect = nullptr;
    UdpPingOperator * _ping = nullptr;

    MyUPnP * _upnp = nullptr;
    StunIpGetter * _ipGetter = nullptr;
    std::string _ip = "";

    bool update(Host host);

    std::string commandsToString(std::map<int, std::string> *commands);

    void processCommands(std::map<int , std::string> commands);
    void processHostList(std::string hosts);

    std::string getBadAnswer();
    std::string getIp();

    std::map<int, std::string> parseCommands(std::string from);
    std::string parseIpList(std::string from);


};

#endif // GLOBALTRANSPORTER_H
