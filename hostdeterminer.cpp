#include "hostdeterminer.h"

#include "tcpconnect.h"

#include <thread>
const int START_PORT_DETERMINER_SERVER = 20000;
const int END_PORT_DETERMINER_SERVER = 50000;
const int PING_MAX_LAST_NUMBER = 254;
const int CHEACK_ON_IP = 5;
const int CHEACK_ON_IP_GLOBAL = 1;
//const int PING_MAX_LAST_NUMBER = 254;
const int MAX_THREADS = 600;
const int TCP_CONNECT_TRIES = 2;
const u_int32_t MAX_U_INT32_IP = 4294967295;
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
    std::cout << _currentHost.ip() << " "<<_currentHost.to32Int() <<" - ip Determiner at port " << genPortFromIp(_currentHost.ip()) << std::endl;
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
        std::string response = _udpServer->listen();
        if(validatePingResponse(response)){
            _udpServer->send(genPingAnswerData());
            if (Host(response) == _currentHost){
                std::cout << "Determiner. Try to determy yourself" << std::endl;
            }
            else
            {
                std::cout << "Determiner. Valid ping Response! Response " << response << std::endl;
            }
        }else{
            std::cout << "Determiner. Bad ping resp. Resoinse: " << response << std::endl;
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
        std::cout << "Determiner. 192.168.0.1-255 scaned. GO AGAIN!" << std::endl;
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
   // std::cout << "first number result result " << result << std::endl;
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

GlobalNetSimpleDeterminer::GlobalNetSimpleDeterminer(MyUPnP *upnp, Host currentHost):LocalNetSimpleDetermiter(currentHost), _upnp(upnp)
{
    _threads = new std::thread*[MAX_THREADS];
    _mainPort = genPortFromIp(currentHost.ip());
    clearTarget();
    _upnp->openPort(_mainPort,"TCP");
    _server = new TcpServer(_mainPort);
   // std::cout << "max ip - " << MAX_U_INT32_IP << std::endl;
   // std::cout << "start ip " << _currentHost.ip() << " "<<currentHost.to32Int() << std::endl;
}

GlobalNetSimpleDeterminer::~GlobalNetSimpleDeterminer()
{
    delete _upnp;
    delete _server;
    delete[] _threads;
}

void GlobalNetSimpleDeterminer::startDeterminerServer()
{

    if (_server->start()){
        while(true){
            _server->nextPendingConnection();
            //
            std::string answer = _server->readSocket();
            if (answer.empty()){
                _server->closeLastConnection();
            }
            if (validatePingResponse(answer)){
                _server->sendMessage(genPingAnswerData());
            }
        }
    }
    //LocalNetSimpleDetermiter::startDeterminerServer();
}

Host GlobalNetSimpleDeterminer::determiteHost()
{
    Host result;
    while (true){
        for (u_int32_t i = u_int32_t(0); i < MAX_U_INT32_IP; i++){
            for (int threadNum = 0; threadNum < MAX_THREADS; threadNum++){
                u_int32_t target = getNewTarget();
                int sp = genPortFromIp(target);
                _threads[threadNum] = new std::thread(&GlobalNetSimpleDeterminer::checkOnThread, this ,target, sp);
                i++;
                //std::cout << "new thread!" << threadNum <<target<< std::endl;
            }

            if (_workingIP != 0 ){
                // std::cout << "FINE YOU DO IT! SUCEES! " << _workingIP;
                // return Host();
                TcpConnect connectToWorkingHost(_workingIP, genPortFromIp(_workingIP));
                if (connectToWorkingHost.fixConnection()){
                    connectToWorkingHost.sendMessage(genPingResponseData());
                    Host result = Host(connectToWorkingHost.readSocket());
                    if (!(result==_currentHost)){
                        _workingIP =0;
                        return result;
                    }
                    else{
                        std::cout << "GlobalDeterminer. Finded working host but it this host HOST: " << result.toString() << std::endl;
                        _workingIP=0;
                    }
                }
                else{
                    std::cout << "GlobalDeterminer. Working ip finded but check failed!" << std::endl;
                }
            }

            for (int threadNum = 0; threadNum < MAX_THREADS; threadNum++){
                _threads[threadNum]->join();
                //std::cout << "Thread joined" << threadNum << std::endl;
                delete _threads[threadNum];
            }

            /*
            u_int32_t target = getNewTarget();
            int sp = genPortFromIp(target);

            if (TcpConnect::checkConnect(target,sp,1)){
                u_int8_t fourth = u_int8_t(_target >>24);
                u_int8_t third = u_int8_t(_target >> 16);
                u_int8_t second = u_int8_t(_target >> 8);
                u_int8_t first = u_int8_t(_target);

                std::cout << "FINAL RESULT. Listen string ip " << int (first )<< "." << int (second )<< "."<< int (third )<< "."<< int (fourth )<< std::endl;
                std::cout << "FINAL RESULT Listen. at " << _target << std::endl;
                return Host();
            }*/
            // std::string resp = genPingResponseData();
            // for (int packets = 0; packets < 20; packets++){
            //     _udpClient->send(resp, target,sp);

            // }
            // if (i%CHEACK_ON_IP_GLOBAL==0){
            //     if (_udpClient->checkBuffer()){
            //         result=Host(_udpClient->listen());
            //         if (!result.isNull() /*& !(result==_currentHost)*/){
            //             std::cout << "GlobalNetSimpleDeterminer.  Host determined in " << i << " iterations!" << std::endl;
            //             return result;
            //         }
            //     }
                // u_int8_t fourth = u_int8_t(_target >>24);
                // u_int8_t third = u_int8_t(_target >> 16);
                // u_int8_t second = u_int8_t(_target >> 8);
                // u_int8_t first = u_int8_t(_target);


            //}
                // std::cout << "Listen string ip " << int (first )<< "." << int (second )<< "."<< int (third )<< "."<< int (fourth )<< std::endl;
                // std::cout << "Listen. at " << _target << std::endl;

        }

        std::cout << "GlobalNetSimpleDeterminer. Go back again. All ipv4 scanned"  << std::endl;
    }
    return Host();
}

void GlobalNetSimpleDeterminer::startDeterminerServerInNewThread()
{
    std::thread * newThread = new std::thread(&GlobalNetSimpleDeterminer::startDeterminerServer, this);

}

void GlobalNetSimpleDeterminer::checkOnThread(u_int32_t target, int port)
{

    if (TcpConnect::checkConnect(target, port, TCP_CONNECT_TRIES  )){
        _workingIP  = target;
    }
}

uint32_t GlobalNetSimpleDeterminer::getNewTarget()
{

    //_target = u_int32_t(u_int32_t(u_int32_t(_target<<8) >>  8) + (u_int8_t(u_int8_t(_target)+u_int8_t(1))<<24) );
    if (_target >= MAX_U_INT32_IP){
        _target = (1);
    }
    else{
        _target = htonl(ntohl(_target)+1);
        //inet_ntoa(_target);
    }
    return _target;

}

uint32_t GlobalNetSimpleDeterminer::getFirstTarget()
{
    return  htonl(ntohl(_currentHost.to32Int())-10);
}

void GlobalNetSimpleDeterminer::clearTarget()
{
    _target = getFirstTarget();
}

