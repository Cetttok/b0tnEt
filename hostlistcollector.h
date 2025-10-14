#ifndef HOSTLISTCOLLECTOR_H
#define HOSTLISTCOLLECTOR_H

#include "saver.h"
#include "udppingoperator.h"

class BaseHostsSetManager
/*базовый класс
что то типо интерфейса но уже с минимальным функционалом

*/
{
public:
    BaseHostsSetManager(Host * hosts, int size); // можно использовать - я хотел его пользовать но он мне к global не подошёл
    BaseHostsSetManager(); // пустой

    // эти методы желательно переопределить в них вся логика алгоритма отбора
    virtual bool acceptHost(Host host); // основной метод. в цикле потом используется
    virtual void update(); // типо проверить удалить одинаковые
    virtual std::string getInfo(); //info about algorithm переопределить и написать что делать алгоритм
    virtual int getSize();


    void setSize(int size);// если в конструктор не передал
    void setHosts(Host * hosts);
protected:
    Host * _hosts = nullptr;
    int _size = 0;
    Host pop(int id); // безопасно удалить. сдвиг вправо ( ( 1 2 3 4 5 0) - удаляем 3 ( 1 2 4 5 0 0))
    bool isKnow(Host host); // есть ли хост в списке

};
class SimpleHostsSetManager : public BaseHostsSetManager
/* мой тот самый алгоритм
 1. активные не удаляются никогда
 2. неактивные заменяются активными
 3. если список не полный сохраняются неакивные
 */
{
public:
    SimpleHostsSetManager(Host * hosts, int size, UdpPingOperator * ping); // добавляешь в конструктор нужны классы в global там условная конструкция
    SimpleHostsSetManager(UdpPingOperator * ping);
    virtual bool acceptHost(Host host);
    virtual std::string getInfo(); //info about algorithm
    virtual int getSize();
    // update не переопределить ну тут мне это и не нада
protected:
    UdpPingOperator * _ping = nullptr;

};
class HostListCollector
{
public:
    HostListCollector(BaseHostsSetManager * setManager, UdpPingOperator *ping); // тут надо обьект своего алгоритма передать
    ~HostListCollector();
    Host getNewTarget(); // блокирующий
    void sync(std::string input); // принять список

    bool isNeedDetermeHost();
    std::string share(); //список для отпраки

    void load();
    void save();

    bool addHost(Host host);
private:

    Saver * _saver = nullptr;
    Host * _hosts = nullptr;
    BaseHostsSetManager * _set= nullptr;
    UdpPingOperator * _ping = nullptr;

    int _targetIdState = 0;

};



#endif // HOSTLISTCOLLECTOR_H
