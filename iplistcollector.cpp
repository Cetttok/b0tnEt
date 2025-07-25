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
    //std::cout << result[0].toString() << " "<< result[1].toString() << " "<<result[2].toString() << std::endl;
    return num_slash;
 }


std::string IpListCollector::get( std::string ipList){
    /* <host>123.44.67.234:8878</host> */

    for (int i = 0; i< size; i++){
        //std::cout << _hosts[i].toString() << " back" << std::endl;
    }
    Host* newHosts = nullptr;
    int num_hosts = parse(ipList,newHosts);

    bool* isNewHost = new bool[num_hosts]{true};
    for (int i = 0; i < num_hosts; i++){
        isNewHost[i] = true;
    }
    //std::cout << isNewHost[0] << isNewHost[1] << isNewHost[2] << "Hosts: " << num_hosts<<std::endl;    // отметка новых хостов и получение их количества
    int countNewHosts = num_hosts;
    //std::cout<< "start" << std::endl<< std::endl<< std::endl;
    for(int i=0;i<num_hosts;i++){
        for(int j=0;j<size;j++){
            if(_hosts[j].toString()==newHosts[i].toString()){
                isNewHost[i] = false;
                countNewHosts --;
                //std::cout << i<<"great'"<<_hosts[j].toString()<<"'=='" << newHosts[i].toString() << "'"<<std::endl;
                break;
            }
            else{
                //std::cout << "'"<<_hosts[j].toString()<<"'!='" << newHosts[i].toString() << "'"<<std::endl;
            }

        }
        //std::cout << std::endl;
    }
    //std::cout << newHosts[0].toString() << newHosts[1].toString()  << newHosts[2].toString() << std::endl;

    if(countNewHosts!=0) {  // объединение списков хостов, обновление list_host и сохранение на диск
        Host *buff = _hosts;
        _hosts = new Host[size + countNewHosts];
        for (int i = 0; i < size; i++) {
            _hosts[i] = buff[i];
        }
        //std::cout << isNewHost[0] << isNewHost[1] << isNewHost[2] << "newHosts: " << countNewHosts <<std::endl;
        int posNewHost = size;
        for (int i = 0; i < num_hosts; i++) {

            if(isNewHost[i]){
                _hosts[posNewHost] = newHosts[i];
                posNewHost++;
            }

        }
        size += countNewHosts;
        for (int i = size - countNewHosts; i < size; i++) {
            list_host += startTag + _hosts[i].toString() + endTag + "\n";
        }
        for (int i = 0; i< size; i++){
            //std::cout << _hosts[i].toString() << " and\n ";
        }
        //std::cout << std::endl;
        _saver->writeHostsToFile(_hosts,size);

        delete[] buff;
    }

    delete[] newHosts;
    delete[] isNewHost;

    return list_host;

}
