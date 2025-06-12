#ifndef STUNIPGETTER_H
#define STUNIPGETTER_H
#include "udpworker.h"
class StunIpGetter
{
public:
    StunIpGetter();
    StunIpGetter(int port);
    StunIpGetter(int port, std::string stunServerIp,int stunServerPort);

    std::string getStringIp();
    u_int32_t get32tIp();
private:
    UdpWorker _udp;
    std::string _stunServerAddess;
    int _stunServerPort;
    int* getIp();
    u_int32_t parse32IpFromNums(u_int8_t first, u_int8_t second, u_int8_t third, u_int8_t fourth);
    void generateTransactionId(int seed, char * result);

};

#endif // STUNIPGETTER_H
