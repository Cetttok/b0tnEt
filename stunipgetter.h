#ifndef STUNIPGETTER_H
#define STUNIPGETTER_H
#include "udpworker.h"
/*
 * Класс который позволяет получить ip адрес в двух форматах u_int32_t(системный) и std::string (человеческий)
 *  stun сервер - обычная практика для програм где нужно прямое соединение - так что маскировка под легетимный траф
 *  еще можно кстати просто получить внешний порт. У меня он обычно моему же равен. Но пока не нужно - не нужно
 * u_int32_t это четыре подряд записанных u_int8_t (а это 0-255) в памяти которые образуют число
    пример работы:
    StunIpGetter() ip;
    ip.getStringIP(); или ip.get32Ip();
 */

class StunIpGetter
{
public:
    StunIpGetter(); // простой конструктор. Использует автомотический системный порт и stun сервер гугла
    // когда не указываешь порт есть 100 процентов что ты сможешь его занять

    StunIpGetter(int port); // тоже самое только порт отправки запроса и потом слушания свой
    StunIpGetter(int port, std::string stunServerIp,int stunServerPort); // тоже самое только кастомный stun сервер
    // можно еще написать StunIpGetter(std::string stunServerIp,int stunServerPort) с системным портом.
    //Но мне кажеться первый конструктор самый простой. Если нуженг системный порт просто укажи 0 как порт

    std::string getStringIp(); // возврашает ip в виде "255.255.255.255". При оштбке лог в консоль и возвращает ""
    u_int32_t get32tIp(); // возвращает ip в виде 1215631133(32 битное число). При ошибке лог в консоль и 0 в результате
private:

    UdpWorker _udp;
    std::string _stunServerAddress;
    int _stunServerPort;
    int _serverPort;

    u_int32_t parse32IpFromNums(u_int8_t first, u_int8_t second, u_int8_t third, u_int8_t fourth);
    void generateTransactionId(int seed, char * result);
    std::string genStunResponse();
    bool getIp(u_int8_t *result);
};

#endif // STUNIPGETTER_H
