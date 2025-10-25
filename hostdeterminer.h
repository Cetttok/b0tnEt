#ifndef HOSTDETERMINER_H
#define HOSTDETERMINER_H

#include <sys/types.h>
#include <thread>
#include "my_upnp.h"
#include "saver.h"
#include "stunipgetter.h"
#include "tcpserver.h"
#include "udpworker.h"

/*
можно тут самому свой класс определителя запихать
в какой угодно сети
и как угодно
*/



class AbstractDetermiter // нитерфейсный класс определителя
{
protected:
    AbstractDetermiter();
public:
    virtual void startDeterminerServerInNewThread(); // просто в другом потоке - не блокирующий
    virtual void startDeterminerServer() = 0; // запуск сервера чтобы текущий хост мог определиться - блокирующий
    virtual Host determiteHost() = 0; // определение хоста блокирующий. Работает до определения
    int genPortFromIp(u_int32_t  ipInteger); // сопоставление апи адреса к порту
    int genPortFromIp(std::string ip);
};



class LocalNetSimpleDetermiter : public AbstractDetermiter //определение в локальносй сети путем udp пинга и создания udp серверов
{
public:
    LocalNetSimpleDetermiter(Host currentHost);

    virtual void startDeterminerServer(); // udp сервер
    virtual Host determiteHost(); // блокирующий вызов!! пинг 255 адресов
    virtual void startDeterminerServerInNewThread();

protected:
    virtual uint32_t getNewTarget(); // возвращает слейдущий апи
    virtual uint32_t getFirstTarget(); // для _currentHost = 192.168.0.133 даст 192.168.0.1 ну типо последнюю цифру в ноль
    virtual void clearTarget(); // ставит _target в getFirstTarget()
    uint32_t _target = 0;

    Host _currentHost;
    virtual std::string genPingResponseData(); // _currentHost.toString()
    virtual std::string genPingAnswerData(); // _currentHost.toString()
    virtual bool validatePingResponse(std::string data); // проверка на не null хоста из запроса
private:
    int onTarget = 1;
    UdpWorker * _udpServer = nullptr;
    UdpWorker * _udpClient = nullptr;
};
class GlobalNetSimpleDeterminer : public LocalNetSimpleDetermiter // определение в иннтернете базовый алгоритм. В реальных условиях слишком медленный
// попытка tcp устонавления контакта мультипоток
{
public:
    GlobalNetSimpleDeterminer(MyUPnP * upnp, Host currentHost);
    ~GlobalNetSimpleDeterminer();
    void startDeterminerServer();
    Host determiteHost();
    void startDeterminerServerInNewThread();

protected:

    void checkOnThread(u_int32_t target, int port); // метод который в потоках ходит
    uint32_t _workingIP = 0; // если кто то что то нашел то ставит это в рабочий апи
    std::thread* * _threads;
    uint32_t getNewTarget(); // прибавление с последнего разряда
    uint32_t getFirstTarget(); // первая - апи хоста
    void clearTarget(); // в хост
    MyUPnP * _upnp = nullptr;
    int _mainPort; // порт на котором работет определение1

    TcpServer * _server = nullptr;
};

#endif // HOSTDETERMINER_H
