#ifndef HOSTDETERMINER_H
#define HOSTDETERMINER_H

#include <sys/types.h>
#include "saver.h"
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

private:
    uint32_t getNewTarget();
    uint32_t getFirstTarget();
    void clearTarget();
    uint32_t _target = 0;
    int onTarget = 1;

    Host _currentHost;
    std::string genPingResponseData();
    std::string genPingAnswerData();
    bool validatePingResponse(std::string data);

    UdpWorker * _udpServer = nullptr;
    UdpWorker * _udpClient = nullptr;
};

#endif // HOSTDETERMINER_H
