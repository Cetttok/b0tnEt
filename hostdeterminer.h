#ifndef HOSTDETERMINER_H
#define HOSTDETERMINER_H

#include <sys/types.h>
#include <thread>
#include "my_upnp.h"
#include "saver.h"
#include "stunipgetter.h"
#include "tcpserver.h"
#include "udpworker.h"
class AbstractDetermiter
{
protected:
    AbstractDetermiter();
public:
    virtual void startDeterminerServerInNewThread();
    virtual void startDeterminerServer() = 0;
    virtual Host determiteHost() = 0;
    int genPortFromIp(u_int32_t  ipInteger); //need to test this!!!
    int genPortFromIp(std::string ip); //need to test this!!!
};



class LocalNetSimpleDetermiter : public AbstractDetermiter
{
public:
    LocalNetSimpleDetermiter(Host currentHost);
    virtual void startDeterminerServer();
    virtual Host determiteHost(); // блокирующий вызов!!
    virtual void startDeterminerServerInNewThread();

protected:
    virtual uint32_t getNewTarget();
    virtual uint32_t getFirstTarget();
    virtual void clearTarget();
    uint32_t _target = 0;

    Host _currentHost;
    virtual std::string genPingResponseData();
    virtual std::string genPingAnswerData();
    virtual bool validatePingResponse(std::string data);
private:
    int onTarget = 1;
    UdpWorker * _udpServer = nullptr;
    UdpWorker * _udpClient = nullptr;
};
class GlobalNetSimpleDeterminer : public LocalNetSimpleDetermiter
{
public:
    GlobalNetSimpleDeterminer(MyUPnP * upnp, Host currentHost);
    ~GlobalNetSimpleDeterminer();
    void startDeterminerServer();
    Host determiteHost();
    void startDeterminerServerInNewThread();

protected:

    void checkOnThread(u_int32_t target, int port);
    uint32_t _workingIP = 0;
    std::thread* * _threads;
    uint32_t getNewTarget();
    uint32_t getFirstTarget();
    void clearTarget();
    MyUPnP * _upnp = nullptr;
    int _mainPort;

    TcpServer * _server = nullptr;
};

#endif // HOSTDETERMINER_H
