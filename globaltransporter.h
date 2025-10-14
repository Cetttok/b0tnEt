#ifndef GLOBALTRANSPORTER_H
#define GLOBALTRANSPORTER_H
#include "commandtrasporter.h"
//`#include "iplistcollector.h"
#include "my_upnp.h"
#include "stunipgetter.h"
#include "tcpconnect.h"
#include "tcpserver.h"
#include "hostlistcollector.h"
#include "hostdeterminer.h"
//сердце проекта
class GlobalTransporter
{
public:
    GlobalTransporter(bool isSimpleMode = true, bool isLocalNetwork = false); // основной конструктор
    ~GlobalTransporter(); // удаляет чисто все поля динамические
    void startListening(); // блокирующий запускает сервер и реакцию на зпросы запускать через  std::thread * thread = new std::thread(&GlobalTransporter::startListening,global);
    void eventLoop(); //блокирующий запускает обновление по списку

    void init(); // выбирает рандомный порт и сохраняет его
    void load(); // загружает с этого порта при isLocalNetwork = false запускает upnp
    void save(); // сохраняет все на диск (дополнительно)
private:
    bool accept(std::string response); // принимает и сообщение
    std::string genAnswer(); // генерирует ответ-обновление

    const bool _isSimpleMode;
    const bool _isLocalNetwork;

    CommandTrasporter * _commands = nullptr;
    HostListCollector * _hostList = nullptr;
    TcpServer * _server = nullptr;
    TcpConnect * _connect = nullptr;
    UdpPingOperator * _ping = nullptr;

    MyUPnP * _upnp = nullptr;
    StunIpGetter * _ipGetter = nullptr;
    std::string _ip = "";

    bool update(Host host);// обновление с хостом


    std::string commandsToString(std::map<int, std::string> *commands); // преобразует map для отправки по сети

    void processCommands(std::map<int , std::string> commands); // обрабатывает команды
    void processHostList(std::string hosts); // обрабатывает хосты

    std::string getBadAnswer(); // плохой ответ на плохой запрос
    std::string getIp(); // stun или локальный ip для отправки вместе с списком

    std::map<int, std::string> parseCommands(std::string from); // парсинг
    std::string parseIpList(std::string from); //парсинг

    AbstractDetermiter *  _hostDeterminer = nullptr;


};

#endif // GLOBALTRANSPORTER_H
