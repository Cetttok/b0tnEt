#include "hostlistcollector.h"

const int HOSTS_COUNT  = 50;
const int WAIT_TARGET_DURATION = 10000;  // microsec ждать перед попыткой нового пинга для getNewTarget()
const char * HOST_START_TAG = "<host>";
const char * HOST_END_TAG = "</host>\n";
const char * HOST_DELIMER = ":";



HostListCollector::HostListCollector(BaseHostsSetManager * setManager, UdpPingOperator * ping): _set(setManager), _ping(ping)
{
    _hosts = new Host[HOSTS_COUNT];
    for (int i = 0 ; i < HOSTS_COUNT; i++){
        _hosts[i] = Host(); //clear init
    }
    _set->setHosts(_hosts);
    _set->setSize(HOSTS_COUNT);
    _saver = Saver::getSaver();
    load();
    std::cout <<"HostListCollector: MAIN LOGIC:" << std::endl <<"   " << _set->getInfo()<< std::endl;
}

HostListCollector::~HostListCollector()
{
    save();
    delete _set;
    delete[] _hosts;


}

Host HostListCollector::getNewTarget()
{
    while (true){
        _targetIdState++;
        if (_targetIdState>= HOSTS_COUNT ){
            _targetIdState = 0;
        }
        if (_hosts[_targetIdState].isNull()){
            _targetIdState = 0;
        }
        if (_ping->ping(_hosts[_targetIdState].ip(),_hosts[_targetIdState].pingPort() )){
            return _hosts[_targetIdState];
        }
        else{
            //std::cout << "Bad ping for " << _hosts[_targetIdState].toString() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(WAIT_TARGET_DURATION));

    }
}
void HostListCollector::sync(std::string input){
    while (input.find(HOST_START_TAG)!=input.npos){
        std::string forHost = input.substr(std::string(HOST_START_TAG).size(),input.find(HOST_END_TAG)-std::string(HOST_END_TAG).size()+2);
        if (_set->acceptHost(Host(forHost))){
            save();
        }
        input = input.substr(std::string(HOST_START_TAG).size()  + std::string(HOST_END_TAG).size() + forHost.size(),input.size() );
    }
}

std::string HostListCollector::share()
{
    std::string result;
    for (int i = 0; i < HOSTS_COUNT; i++){
        if (!_hosts[i].isNull()){
            result += HOST_START_TAG + _hosts[i].toString() + HOST_END_TAG;
        }
        else{
            break;
        }
    }
    return result;
}

void HostListCollector::load()
{
    _saver->readHostsFromFile(_hosts, HOSTS_COUNT);
}

void HostListCollector::save()
{
    _saver->writeHostsToFile(_hosts, HOSTS_COUNT);
}

bool HostListCollector::addHost(Host host)
{
    return _set->acceptHost(host);
}

BaseHostsSetManager::BaseHostsSetManager(Host *hosts, int size):_hosts(hosts),_size(size)
{
    update();
}

BaseHostsSetManager::BaseHostsSetManager()
{
}

bool BaseHostsSetManager::acceptHost(Host host)
{
    if (host.isNull()){
        std::cout << "BaseHostsSetManager. WARN! host is null"   << std::endl;
        return false;
    }
    if (isKnow(host)){
        return false;
    }
    if (_hosts[_size-1].isNull()){
        for (int i = 0; i < _size; i++){
            if (_hosts[i].isNull()){
                _hosts[i] = host;
                return true;
            }
        }
    }
    for (int i = 1; i < _size ; i ++){
        if (!_hosts[i].isNull()){
            _hosts[i-1] = _hosts[i];
        }
        else{
            _hosts[i-1] = host;
            return true;
        }
        if (i == _size-1){
            _hosts[i] = host;
            return true;
        }

    }

    return false;

}

void BaseHostsSetManager::update()
{
    for (int i = 0;  i < _size; i++){
        if (_hosts[i].isNull()){
            return;
        }
        for (int ii = 0; ii < _size; ii++){
            if (i == ii ){
                continue;
            }
            if (_hosts[ii].isNull()){
                break;
            }
            if (_hosts[i] == _hosts[ii]){
                pop(ii);
            }
        }
    }
}

std::string BaseHostsSetManager::getInfo()
{
    return "BaseHostsSetManager: new hosts append to end of list if its unical";
}

void BaseHostsSetManager::setSize(int size)
{
    _size = size;
}

void BaseHostsSetManager::setHosts(Host *hosts)
{
    _hosts = hosts;
}

bool BaseHostsSetManager::isKnow(Host host)
{
    if (host.isNull()){
        return false;
    }
    for (int i = 0; i < _size; i++){
        if (_hosts[i].isNull()){
            return false;
        }
        if (_hosts[i] == host){
            return true;
        }
    }
    return false;
}

Host BaseHostsSetManager::pop(int id)
{

    Host result = _hosts[id];
    _hosts[id] = Host();
    for (int i = id+1; i < _size; i ++){
        if (!_hosts[i].isNull()){
            _hosts[i-1] =  _hosts[i];
        }
        else{
            return result;
        }
        if (i == (_size-1)){
            _hosts[i] = Host();

        }
    }
}

SimpleHostsSetManager::SimpleHostsSetManager(Host *hosts, int size, UdpPingOperator *ping):BaseHostsSetManager(hosts,size), _ping(ping)
{

}

SimpleHostsSetManager::SimpleHostsSetManager(UdpPingOperator *ping): _ping(ping)
{

}

bool SimpleHostsSetManager::acceptHost(Host host)
{
    if (isKnow(host) | host.isNull()){ //пустые не добавляем ХОСТ ПУСТОЙ КОГДА ОН ПУСТОЙ ИЛИ ОДИН ИЗ ПОРТОВ ХОТЯБ РАВНО НОЛЬ
        return false;
    }
    if (_ping->ping(host.ip(),host.pingPort())){
        if (_hosts[_size].isNull()){
            for (int i = 0; i < _size; i ++){
                if (_hosts[i].isNull()){
                    _hosts[i] = host;
                    return true;
                }
            }
        }
        for (int i = 0; i < _size; i++){
            if (!_ping->ping(_hosts[i].ip(), _hosts[i].pingPort())){
                _hosts[i] = host;
                return true;
            }
        }
        return false;
    }
    else{
        for (int i = 0; i < _size; i++){
            if (_hosts[i].isNull()){
                _hosts[i] = host;
                return true;
            }
        }
    }
}

std::string SimpleHostsSetManager::getInfo()
{
    return "SimpleHostsSetManager: new host added to end if its unical and (if active). active hosts not remover always";
}
