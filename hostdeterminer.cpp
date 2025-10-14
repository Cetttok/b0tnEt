#include "hostdeterminer.h"

#include <thread>
const int START_PORT_DETERMINER_SERVER = 20000;
const int END_PORT_DETERMINER_SERVER = 50000;
const int PING_MAX_LAST_NUMBER = 254;
const int CHEACK_ON_IP = 5;

AbstractDetermiter::AbstractDetermiter()
{
}

int AbstractDetermiter::genPortFromIp(u_int32_t ipInteger)
{
    return ipInteger%(END_PORT_DETERMINER_SERVER-START_PORT_DETERMINER_SERVER)+START_PORT_DETERMINER_SERVER;
}

int AbstractDetermiter::genPortFromIp(std::string ip)
{
    return genPortFromIp(inet_addr(ip.data()));
}
void AbstractDetermiter::startDeterminerServerInNewThread(){
    std::thread * newThread = new std::thread(&AbstractDetermiter::startDeterminerServer, this);

}

LocalNetSimpleDetermiter::LocalNetSimpleDetermiter(Host currentHost):_currentHost(currentHost)
{
    _udpClient = new UdpWorker();
    _udpServer = new UdpWorker();
    std::cout << _currentHost.ip() <<" - ip Determiner at port " << genPortFromIp(_currentHost.ip()) << std::endl;
    // uint32_t  addr = inet_addr("192.168.0.123");

    // ip_addr.s_addr = addr;
    // std::cout << inet_ntoa(ip_addr) <<  " and "<< addr  << " ip" << std::endl;
    //
    clearTarget();
}

void LocalNetSimpleDetermiter::startDeterminerServer()
{
    _udpServer->start(genPortFromIp(_currentHost.ip()));
    while(true){
        if(validatePingResponse(_udpServer->listen())){
            _udpServer->send(genPingAnswerData());
        }

    }
}

Host LocalNetSimpleDetermiter::determiteHost()
{
    Host result;
    while (true){
        for (int i = 0; i < PING_MAX_LAST_NUMBER; i++){
            u_int32_t target = getNewTarget();
            _udpClient->send(genPingResponseData(), target, genPortFromIp(target));
            if (i%CHEACK_ON_IP==0){
                if (_udpClient->checkBuffer()){
                    result=Host(_udpClient->listen());
                    if (!result.isNull() & !(result==_currentHost)){
                        std::cout << "LocalNetSimpleDetermiter. Host determined in " << i << " iterations!" << std::endl;
                        return result;
                    }
                }
            }
        }
        clearTarget();
        std::cout << "GO AGAIN!" << std::endl;
    }
}

void LocalNetSimpleDetermiter::startDeterminerServerInNewThread()
{
    std::thread * newThread = new std::thread(&LocalNetSimpleDetermiter::startDeterminerServer, this);

}

uint32_t LocalNetSimpleDetermiter::getNewTarget()
{
    //uint32_t target = _target;
    //std::cout << "std uip " << _currentHost.ip() << std::endl;
    // u_int8_t  fourth = u_int8_t(target);
    // u_int8_t  third = u_int8_t(target >> 8);
    // u_int8_t second= u_int8_t(target>> 16);
    // u_int8_t first= u_int8_t(target >>24);
    //fourth = 1;
   // std::cout << "ip " << int (first )<< "." << int (second )<< "."<< int (third )<< "."<< int (fourth )<< std::endl;
    //std::cout << "target before << " << _target  << " at num " << onTarget<< std::endl;
    _target = u_int32_t(u_int32_t(u_int32_t(_target<<8) >>  8) + (u_int8_t(onTarget)<<24) );
    onTarget++;
    if(onTarget >= 256){
        onTarget = 1;
    }
    // struct in_addr ip_addr;
    // ip_addr.s_addr = target;
    //std::cout << "target after << " << _target << std::endl;
    // std::cout << "stdring " <<  inet_ntoa(ip_addr)  << std::endl;
     return _target;
}

uint32_t LocalNetSimpleDetermiter::getFirstTarget() //  mode diapason xxx.xxx.xxx.1-256
{
    u_int32_t num = inet_addr(_currentHost.ip().data());
   // std::cout << "std uip " << _currentHost.ip() << std::endl;
    u_int8_t fourth = u_int8_t(num);
    u_int8_t third = u_int8_t(num >> 8);
    u_int8_t second = u_int8_t(num >> 16);
    u_int8_t first = u_int8_t(num>>24);
    first = 1;
    // std::cout << "first ip " << int (first )<< "." << int (second )<< "."<< int (third )<< "."<< int (fourth )<< std::endl;
    u_int16_t one =    u_int16_t(  ((u_int16_t)(first << 8)) | ((u_int16_t)(second)));
    u_int16_t two =    u_int16_t(  ((u_int16_t)(third << 8)) | ((u_int16_t)(fourth)));
    u_int32_t result = u_int32_t(  ((u_int32_t)( one << 16)) | ((u_int32_t)(two)));
    std::cout << "result " << result << std::endl;
    return result;
}

void LocalNetSimpleDetermiter::clearTarget()
{

    _target = getFirstTarget();
}

std::string LocalNetSimpleDetermiter::genPingResponseData()
{
    return _currentHost.toString();
}

std::string LocalNetSimpleDetermiter::genPingAnswerData()
{
    return _currentHost.toString();
}

bool LocalNetSimpleDetermiter::validatePingResponse(std::string data)
{
    return !Host(data).isNull();
}
