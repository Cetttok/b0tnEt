#include "iplistcollector.h"

std::string IpListCollector::get(std::string* nextIp, int *nextPort) {
    if(size!=0) {
        *nextIp = _hosts[ptrToHost].ip();
        *nextPort = _hosts[ptrToHost].port();
        if (ptrToHost++ >= size) ptrToHost = 0;
    }else{
        *nextIp = "";
        *nextPort =0;
    }
    return list_host;
}


int IpListCollector::check() {
    if(size<=0) {
        return -1;
    }
    bool activeHost[size];
    int countActiveHosts =0;
    for(int  i=0;i<size;i++){
        activeHost[i]=pinger->ping(_hosts[i].ip());
        if(ptrToHost==i)ptrToHost = countActiveHosts;
        if( activeHost[i] )countActiveHosts++;
    }
    if(countActiveHosts==size)return 0;
    if(ptrToHost==countActiveHosts)ptrToHost=0;

    Host* buff = _hosts;
    _hosts= new Host[countActiveHosts];

    for(int i=0, j=0;i<size;i++){
        if(activeHost[i]){
            _hosts[j]=buff[i];
            j++;
        }
    }
    delete[] buff;
    size = countActiveHosts;
    _saver->writeHostsToFile(_hosts,size);

    list_host ="";
    for (int i = 0; i < size; i++) {
        list_host += startTag + _hosts[i].toString() + endTag + "\n";
    }

    return size;
}

IpListCollector::IpListCollector(UdpPingOperator* pingOperator):_saver(Saver::getSaver()),pinger(pingOperator) {
    int a =_saver->countHostInFile();
    list_host ="";
    if (size != a) {
        size = a;
        _hosts = new Host[size];
        _saver->readHostsFromFile(_hosts, size);

        for (int i = 0; i < size; i++) {
            list_host += startTag + _hosts[i].toString() + endTag + "\n";
        }
    }

}

IpListCollector::~IpListCollector() {
    if (_hosts != nullptr) {
        delete[] _hosts;
        _hosts = nullptr;
    }

}

// result должен быть равен nullptr
// возвращает количество хостов в result
int IpListCollector::parse(std::string ipList, Host* &result){
    int num_slash=0; // количество хостов в ipList
    for(int i=0; i<ipList.size(); i++)
        if(ipList[i]=='/')num_slash++;

    result = new Host[num_slash];

    char * p = ipList.data();    // разбор ipList
    for(int i=0;i<num_slash;i++){
        std::string addr ="";
        while(strncmp(p,startTag,lenTag)!=0)p++;
        p+=lenTag;
        while(strncmp(p,endTag,lenTag+1)!=0){
            addr += *p;
            p++;
        }
        result[i] = Host(addr);
        p+=lenTag+1;
    }
    return num_slash;
}


std::string IpListCollector::get( std::string ipList){
    /* <host>123.44.67.234:8878</host> */

    Host* newHosts = nullptr;
    int num_hosts = parse(ipList,newHosts);

    char* isNewHost = new char[num_hosts]{1};  // отметка новых хостов и получение их количества
    int countNewHosts = num_hosts;
    for(int i=0;i<num_hosts;i++){
        for(int j=0;j<size;j++){
            if(_hosts[j].port()==newHosts[i].port() && _hosts[j].ip() == newHosts[i].ip()){
                isNewHost[i] = 0;
                countNewHosts --;
                break;
            }
        }
    }

    if(countNewHosts!=0) {  // объединение списков хостов, обновление list_host и сохранение на диск
        Host *buff = _hosts;
        _hosts = new Host[size + countNewHosts];
        for (int i = 0; i < size; i++) {
            _hosts[i] = buff[i];
        }
        for (int i = 0; i < num_hosts; i++) {
            if(isNewHost[i]==1)_hosts[i] = newHosts[i];
            else i++;
        }
        size += countNewHosts;
        for (int i = size - countNewHosts; i < size; i++) {
            list_host += startTag + _hosts[i].toString() + endTag + "\n";
        }

        _saver->writeHostsToFile(_hosts,size);

        delete[] buff;
    }

    delete[] newHosts;
    delete[] isNewHost;

    return list_host;

}
